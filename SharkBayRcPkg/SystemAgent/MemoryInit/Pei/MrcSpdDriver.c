/** @file

  This file contains functions that read the SPD data for each DIMM slot over
  the SMBus interface.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

**/

#include <MrcSpdDriver.h>
#include <MrcSpdProcessing.h>

#define MAX_SPD_PAGE_COUNT (1)
#define MAX_SPD_PAGE_SIZE  (256)
#define MAX_SPD_SIZE       (MAX_SPD_PAGE_SIZE * MAX_SPD_PAGE_COUNT)
#define SPD_PAGE_ADDRESS_0 (0x6C)
#define SPD_PAGE_ADDRESS_1 (0x6E)

/**
@brief
  Read the SPD data over the SMBus, at the specified SPD address, starting at
  the specified starting offset and read the given amount of data.

  @param[in, out] Inputs - Mrc Inputs structure
  @param[in] SpdAddress  - SPD SMBUS address
  @param[in, out] Buffer - Buffer to store the data.
  @param[in] Start       - Starting SPD offset
  @param[in] Size        - The number of bytes of data to read and also the size of the buffer.
  @param[in, out] Page   - The final page that is being pointed to.

  @retval mrcSuccess if the read is successful, otherwise mrcDimmNotExist, which
  @retval indicates that no slots are populated.
**/
static
MrcStatus
MrcDoSpdRead (
  IN OUT MrcInput *const Inputs,
  IN     const U8        SpdAddress,
  IN OUT U8 *const       Buffer,
  IN     const U16       Start,
  IN           U16       Size,
  IN OUT U8              *Page
  )
{
  MrcDebug  *Debug;
  MrcStatus Status;
  BOOL      PageUpdate;
  U16       Count;
  U16       Index;

  Debug  = &Inputs->Debug;
  Status = mrcFail;
  if ((Buffer != NULL) && (Start < MAX_SPD_SIZE) && ((Start + Size) < MAX_SPD_SIZE)) {
    Count = 0;
    PageUpdate = FALSE;
    while (Size--) {
      Index = Start + Count;
      if ((Index / MAX_SPD_PAGE_SIZE) != *Page) {
        *Page = (U8) (Index / MAX_SPD_PAGE_SIZE);
        PageUpdate = TRUE;
      }
      Index %= MAX_SPD_PAGE_SIZE;
      if (PageUpdate == TRUE) {
        PageUpdate = FALSE;
        MrcOemSmbusWrite8 (Inputs->SmbusBaseAddress, (*Page == 0) ? SPD_PAGE_ADDRESS_0 : SPD_PAGE_ADDRESS_1, 0, 0);
      }
      Status = MrcOemSmbusRead8 (Inputs->SmbusBaseAddress, SpdAddress, (U8) Index, &Buffer[Count]);
      if (mrcSuccess != Status) {
        break;
      }
      Count++;
    }
  }
  return (Status);
}

/**
@brief
  See if there is valid XMP SPD data.

  @param[in] Debug    - Mrc debug structure.
  @param[in, out] Spd - Mrc SPD structure.
  @param[in] XmpStart - The current offset in the SPD.

  @retval TRUE if valid, FALSE in not.
**/
static
BOOL
VerifyXmp (
  IN MrcDebug          *Debug,
  IN OUT MrcSpd *const Spd,
  IN const U16         XmpStart
  )
{
  SPD_EXTREME_MEMORY_PROFILE_HEADER *Header;

  switch (Spd->Ddr3.General.DramDeviceType.Bits.Type) {
#if ((SUPPORT_DDR3 == SUPPORT) || (SUPPORT_LPDDR3 == SUPPORT))
#if (SUPPORT_DDR3 == SUPPORT)
  case MRC_SPD_DDR3_SDRAM_TYPE_NUMBER:
#endif
#if (SUPPORT_LPDDR3 == SUPPORT)
  case MRC_SPD_LPDDR3_SDRAM_TYPE_NUMBER:
#endif
    Header = &Spd->Ddr3.Xmp.Header;
    break;
#endif
  default:
    return (FALSE);
  }
  if (XmpStart == ((U32) (Header) - (U32) Spd)) {
    if ((XMP_ID_STRING == Header->XmpId) && ((Header->XmpRevision.Data & 0xFE) == 0x12)) {
      return (TRUE);
    } else {
      Header->XmpId            = 0;
      Header->XmpOrgConf.Data  = 0;
      Header->XmpRevision.Data = 0;
    }
  } else {
    return (TRUE);
  }
  return (FALSE);
}

/**
@brief
  Read the SPD data over the SMBus, for all DIMM slots and copy the data to the MrcData structure.
  The SPD data locations read is controlled by the current boot mode.

  @param[in] BootMode - Mrc Boot Mode
  @param[in, out] Inputs - Mrc Inputs structure

  @retval mrcSuccess if the read is successful, otherwise mrcDimmNotExist, which
  @retval indicates that no slots are populated.
**/
MrcStatus
MrcGetSpdData (
  IN     const MrcBootMode BootMode,
  IN OUT MrcInput *const   Inputs
  )
{
#pragma pack (push, 1)
  typedef struct {
    U16 Start;
    U16 End;
    U8  BootMode;
    U8  Profile;
  } SpdOffsetTable;
#pragma pack (pop)
  const SpdOffsetTable Table3[] = {
#ifdef ULT_FLAG
    {   0,              40,           (1 << bmCold),                                   (1 << STD_PROFILE) | (1 << XMP_PROFILE1) | (1 << XMP_PROFILE2) | (1 << USER_PROFILE) },
#else  // ULT_FLAG
    {   0,              38,           (1 << bmCold),                                   (1 << STD_PROFILE) | (1 << XMP_PROFILE1) | (1 << XMP_PROFILE2) | (1 << USER_PROFILE) },
#endif // ULT_FLAG
    {  60,              63,           (1 << bmCold),                                   (1 << STD_PROFILE) | (1 << XMP_PROFILE1) | (1 << XMP_PROFILE2) | (1 << USER_PROFILE) },
    { SPD3_MANUF_START, SPD3_MANUF_END, (1 << bmCold) | (1 << bmWarm) | (1 << bmFast), (1 << STD_PROFILE) | (1 << XMP_PROFILE1) | (1 << XMP_PROFILE2) | (1 << USER_PROFILE) },
    { 128,             145,           (1 << bmCold),                                   (1 << STD_PROFILE) | (1 << XMP_PROFILE1) | (1 << XMP_PROFILE2) | (1 << USER_PROFILE) },
#if (SUPPORT_SPD_CRC == SUPPORT)
    {  39,              59,           (1 << bmCold),                                   (1 << STD_PROFILE) | (1 << XMP_PROFILE1) | (1 << XMP_PROFILE2) | (1 << USER_PROFILE) },
    {  64,             125,           (1 << bmCold),                                   (1 << STD_PROFILE) | (1 << XMP_PROFILE1) | (1 << XMP_PROFILE2) | (1 << USER_PROFILE) },
#endif
#if SUPPORT_XMP == SUPPORT
    { 176,             179,           (1 << bmCold),                                   (1 << STD_PROFILE) | (1 << XMP_PROFILE1) | (1 << XMP_PROFILE2) | (1 << USER_PROFILE) },
    { 180,             184,           (1 << bmCold),                                   (1 << STD_PROFILE) | (1 << XMP_PROFILE1) | (1 << XMP_PROFILE2) | (1 << USER_PROFILE) },
    { 185,             215,           (1 << bmCold),                                   (1 << STD_PROFILE) | (1 << XMP_PROFILE1) | (1 << XMP_PROFILE2) | (1 << USER_PROFILE) },
    { 220,             250,           (1 << bmCold),                                   (1 << STD_PROFILE) | (1 << XMP_PROFILE1) | (1 << XMP_PROFILE2) | (1 << USER_PROFILE) },
#endif
  };
  MrcDebug              *Debug;
  MrcControllerIn       *ControllerIn;
  MrcChannelIn          *ChannelIn;
  MrcDimmIn             *DimmIn;
  U8                    *Buffer;
  const SpdOffsetTable  *Tbl;
  const SpdOffsetTable  *TableSelect;
  MrcStatus             Status;
  U16                   Offset;
#ifdef MRC_DEBUG_PRINT
  U16                   Line;
  U16                   Address;
#endif
  U8                    Controller;
  U8                    Channel;
  U8                    Dimm;
  U8                    Count;
  U8                    Index;
  U8                    Stop;
  U8                    Page;

  Debug = &Inputs->Debug;
  Count = 0;
  Page  = 0;
  for (Controller = 0; Controller < MAX_CONTROLLERS; Controller++) {
    ControllerIn = &Inputs->Controller[Controller];
    for (Channel = 0; Channel < MAX_CHANNEL; Channel++) {
      ChannelIn = &ControllerIn->Channel[Channel];
      if (ChannelIn->Status == CHANNEL_PRESENT) {
        for (Dimm = 0; Dimm < MAX_DIMMS_IN_CHANNEL; Dimm++) {
          Status  = mrcSuccess;
          DimmIn  = &ChannelIn->Dimm[Dimm];
          if ((DimmIn->Status == DIMM_ENABLED) || (DimmIn->Status == DIMM_DISABLED)) {
            Buffer = (U8 *) &DimmIn->Spd;
            if (DimmIn->SpdAddress > 0) {
                    TableSelect = Table3;
                    Stop = (sizeof (Table3) / sizeof (SpdOffsetTable));
              for (Index = 0; (Status == mrcSuccess) && (Index < Stop); Index++) {
                Tbl = &TableSelect[Index];
                if (((1 << BootMode) & Tbl->BootMode) && ((1 << Inputs->MemoryProfile) & Tbl->Profile)) {
                  Status = MrcDoSpdRead (
                    Inputs,
                    DimmIn->SpdAddress,
                    &Buffer[Tbl->Start],
                    Tbl->Start,
                    Tbl->End - Tbl->Start + 1,
                    &Page
                    );
                  if (Status == mrcSuccess) {
                    for (Offset = Tbl->Start; Offset <= Tbl->End; Offset++) {
                      DimmIn->SpdValid[Offset / CHAR_BITS] |= 1 << (Offset % CHAR_BITS);
                    }
#if SUPPORT_XMP == SUPPORT
                    if (bmCold == BootMode) {
                      if (FALSE == VerifyXmp (Debug, (MrcSpd *) Buffer, Tbl->Start)) {
                       MRC_DEBUG_MSG (
                        Debug,
                        MSG_LEVEL_NOTE,
                        "VerifyXmp FALSE\n"
                        );
                       break;
                      }
                    }
#endif // SUPPORT_XMP
                  } else {
                    MRC_DEBUG_MSG (
                      Debug,
                      MSG_LEVEL_WARNING,
                      "ERROR! Fail to read SMB DimmAddress %Xh Offset %Xh - %Xh\n",
                      DimmIn->SpdAddress,
                      Tbl->Start,
                      Tbl->End
                      );
                  } // if (Status...
                } // if (((1 << BootMode)...
              }  // for (Index...
            }  else { // if (DimmIn->SpdAddress > 0), 0 = MemoryDown, see EnableMemoryDown()
              Status = mrcSuccess;
            }

            if (Status == mrcSuccess) {
              Count++;
#ifdef MRC_DEBUG_PRINT
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\nChannel %d Dimm %d\n", Channel, Dimm);
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "SPD:           00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
              for (Line = 0; Line < (sizeof (MrcSpd) / 16); Line++) {
                Address = Line * 16;
                MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, " % 4Xh(% 5u): ", Address, Address);
                for (Offset = 0; Offset < 16; Offset++) {
                  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%02X ", Buffer[Address + Offset]);
                }
                for (Offset = 0; Offset < 16; Offset++) {
                  MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "%c", isprint (Buffer[Address + Offset]) ? Buffer[Address + Offset] : '.');
                }
                MRC_DEBUG_MSG (Debug, MSG_LEVEL_NOTE, "\n");
              }
#endif
            } else {
              MRC_DEBUG_MSG (Debug, MSG_LEVEL_WARNING, "DIMM is not populated on channel %u, slot %u\n", Channel, Dimm);
            } // if (Status...
          }  // if (DimmIn->Status == DIMM_ENABLED)
        }  // for (Dimm...
      }  // if (ChannelIn->Status...
    }  // for (Channel...
  }  // for (Controller...
  return ((Count > 0) ? mrcSuccess : mrcDimmNotExist);
}
