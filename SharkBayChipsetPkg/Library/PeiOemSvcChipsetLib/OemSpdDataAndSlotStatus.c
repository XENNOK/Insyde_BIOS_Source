/** @file
    This function offers an interface to customized SPD data and slot status.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#include <Uefi.h>
#include <MrcOemPlatform.h>
#include <Library/BaseMemoryLib.h>
#include <Guid/OemSpdDataGuid.h>

#if 0
MemorySlotStatus mSlotStatus[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL] = {
  MEMORY_SLOT_ONLY,                     // Channel 0, Slot 0
  MEMORY_SLOT_ONLY,                     // Channel 0, Slot 1
  MEMORY_SLOT_ONLY,                     // Channel 1, Slot 0
  MEMORY_SLOT_ONLY,                     // Channel 1, Slot 1
};

UINT8 OemSpdDdr3[] = {
          0x92,0x10,0x0B,0x02,0x02,0x11,0x00,0x09,0x03,0x52,0x01,0x08,0x0c,0x00,0x3C,0x00,
          0x69,0x78,0x69,0x30,0x69,0x11,0x20,0x89,0x70,0x03,0x3C,0x3C,0x00,0xf0,0x83,0x81,
          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x11,0x01,0x01,
          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
          0x00,0x00,0x00,0x00,0x00,0x02,0xFE,0x05,0x10,0x16,0x53,0x2e,0x49,0x85,0xAB,0xA7,
          0x54,0x53,0x32,0x35,0x36,0x4D,0x53,0x4B,0x36,0x34,0x56,0x31,0x55,0x20,0x20,0x20,
          0x20,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
          0x70,0x61,0x73,0x73,0x47,0x50,0x34,0x35,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
          0x70,0x61,0x73,0x73,0x32,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
          0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
          };

UINT8 *mSpdDataTable[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL] = {
  NULL,                     // Channel 0, Slot 0
  OemSpdDdr3,               // Channel 0, Slot 1
  NULL,                     // Channel 1, Slot 0
  NULL                      // Channel 1, Slot 1
};
#endif

/**
  On-board RAM SPD data and Slot Status are customized here. 
    MEMORY_ABSENT    -  No memory down and no physical memory slot.
    MEMORY_SLOT_ONLY -  No memory down and a physical memory slot.
    MEMORY_DOWN_ONLY -  Memory down and not a physical memory slot.

  @param  OemSpdData - Point to a pointer array SpdIn[MAX_CHANNEL][MAX_DIMMS_IN_CHANNEL],
                       each member indicate a SPD data table.
  @param  SlotStatus - Point to a SlotStatus array.  
  @param  SpdSize    - Point to SPD data size.  

  @retval EFI_SUCCESS        OEM config memory and create hob for smbios successfully, Chipset code won't do it again.
  @retval EFI_MEDIA_CHANGED  OEM customize SPD data or slot status successfully. Chipset code will provide this 
                             data for memory configuration, and build gOemSpdDataGuid HOB and gSlotStatusGuid HOB
                             for SMBIOS Memory driver showing memory information.
  @retval EFI_UNSUPPORTED  �@Chipset code will config memroy with SPD data or default CRB SPD data.

**/
EFI_STATUS
OemSpdDataAndSlotStatus (
  IN OUT U8                      **OemSpdData,
  IN OUT MemorySlotStatus        **SlotStatus,
  IN OUT U16                     *SpdSize
  )
{
// Sample code for OEM
#if 0
  CopyMem (OemSpdData, &mSpdDataTable[0][0], (sizeof (UINT8*) * MAX_CHANNEL * MAX_DIMMS_IN_CHANNEL));
  *SlotStatus = &mSlotStatus[0][0];
  *SpdSize    = (MAX_SPD_DATA_SIZE/2);
#endif

  return EFI_UNSUPPORTED;
}
