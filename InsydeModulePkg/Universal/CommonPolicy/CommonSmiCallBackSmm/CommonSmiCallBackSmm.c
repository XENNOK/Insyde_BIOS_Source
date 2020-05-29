/** @file
  It register callbacks for common Sw or Sx(s3/S4).

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "CommonSmiCallBackSmm.h"

/**
  The driver's entry point.

  It register callbacks for common Sw or Sx(s3/S4).

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval Others          Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
CommonSmiInitialize (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            WakeOnAcLossHandle;
  EFI_SMM_SX_DISPATCH2_PROTOCOL         *SxDispatch2;
  EFI_SMM_SX_REGISTER_CONTEXT           EntryDispatchContext2;
  EFI_SMM_SX_DISPATCH_PROTOCOL          *SxDispatch;
  EFI_SMM_SX_DISPATCH_CONTEXT           EntryDispatchContext;

  //
  // Get the Sx dispatch2 protocol
  //
  Status = gSmst->SmmLocateProtocol (&gEfiSmmSxDispatch2ProtocolGuid, NULL, (VOID **)&SxDispatch2);
  if (EFI_ERROR(Status)) {
    //
    // Get the Sx dispatch protocol
    //
    Status = gBS->LocateProtocol (&gEfiSmmSxDispatchProtocolGuid, NULL, (VOID **)&SxDispatch);
    if (!EFI_ERROR(Status)) {
      EntryDispatchContext.Type  = SxS4;
      EntryDispatchContext.Phase = SxEntry;
      Status = SxDispatch->Register (
                             SxDispatch,
                             S4SleepEntryCallBack,
                             &EntryDispatchContext,
                             &WakeOnAcLossHandle
                             );
      if (EFI_ERROR(Status)) {
        return Status;
      }
    }
  } else {
    EntryDispatchContext2.Type  = SxS4;
    EntryDispatchContext2.Phase = SxEntry;
    Status = SxDispatch2->Register (
                            SxDispatch2,
                            S4SleepEntryCallBack2,
                            &EntryDispatchContext2,
                            &WakeOnAcLossHandle
                            );
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }
  return Status;
}

/**
  Change the BBS index to Boot current when legacy boot

  @retval   0xff            Failed
  @retval   Others          Boot current value

**/
UINT8
GetBootCurrent (
  VOID
  )
{
  UINTN               OptionOrderSize;
  UINT16              *OptionOrder;
  UINT16              PriorityIndex;
  UINT16              BootOption[10];
  UINT16              BootOrderNumber[5];
  UINT8               *Ptr = NULL, *BootOptionVar = NULL;
  UINT8               CmosData = 0;
  UINT16              DevPathSize;
  CHAR16              *BootDesc;
  UINT8               BootCurrentValue;
  UINT16              BbsIndex;


 OptionOrderSize = 0;
 OptionOrder     = NULL;
 CommonGetVariableDataAndSize (
   L"BootOrder",
   &gEfiGlobalVariableGuid,
   &OptionOrderSize,
   (VOID **) &OptionOrder
   );
  if (OptionOrder == NULL) {
    return INVALID;
  }

  BootCurrentValue = INVALID;
  //
  // Set BBS priority according OptionOrder variable
  //
  for (PriorityIndex = 0; PriorityIndex < OptionOrderSize / sizeof (UINT16); PriorityIndex++) {
    ZeroMem ((VOID *)BootOption, sizeof(BootOption));
    ZeroMem ((VOID *)BootOrderNumber, sizeof(BootOrderNumber));
    StrCpy (BootOption, L"Boot");
    UnicodeValueToString (BootOrderNumber, PREFIX_ZERO, OptionOrder[PriorityIndex], 4);
    StrCat (BootOption, BootOrderNumber);

    BootOptionVar = CommonGetVariableData (BootOption, &gEfiGlobalVariableGuid);
    if (BootOptionVar == NULL) {
      break;
    }
    //
    // Skip the native boot options(EFI shell...)
    //
    Ptr = BootOptionVar + sizeof(UINT32) + sizeof(UINT16) + StrSize ((CHAR16 *)(BootOptionVar + 6));
    if (*Ptr != BBS_DEVICE_PATH) {
      FreePool (BootOptionVar);
      continue;
    }

    Ptr = BootOptionVar;
    Ptr += sizeof (UINT32);
    DevPathSize = *((UINT16 *) Ptr);
    Ptr += sizeof (UINT16);
    BootDesc = (CHAR16*) Ptr;
    Ptr += StrSize (BootDesc);
    Ptr += DevPathSize;
    Ptr += sizeof (BBS_TABLE);

    BbsIndex = *((UINT16 *) Ptr);
    FreePool (BootOptionVar);
    //
    // Change BBS index to boot current
    //
    CmosData = ReadCmos8 (LastBootDevice);
    if (CmosData == (UINT8) BbsIndex) {
      //
      // Save boot current to CMOS
      //
      BootCurrentValue = (UINT8) OptionOrder[PriorityIndex];
      break;
    }
  }
  FreePool (OptionOrder);
  return BootCurrentValue;
}


/**
 Check whether is Legacy boot

 @retval VALUE          Legacy boot
 @retval 0xFF           UEFI boot or invalid's boot option
**/
UINT8
SaveLastBootDevice (
  VOID
  )
{
  UINT32                                *BootTablePtr;

  if ((BDA(BDA_MEMORY_SIZE_OFFSET) == 0xFFFF) || (BDA(BDA_MEMORY_SIZE_OFFSET) == 0)) {
    //
    // Class 3 Bios
    //
    return INVALID;
  }
  //
  // Check whether BDA is valid (BDA offset 0x13(40:13) * 0x400 is equal EBDA address)
  //
  if ((BOOLEAN)((BDA(BDA_MEMORY_SIZE_OFFSET) * 0x400) == (BDA(EXT_DATA_SEG_OFFSET) << 4))) {
    //
    // Check whether Boot table is valid
    //
    if ((EBDA(LEGACY_BOOT_TABLE_OFFSET) != 0xFFFF) &&
        (EBDA(LEGACY_BOOT_TABLE_OFFSET) != 0) &&
        (EBDA(LEGACY_BOOT_TABLE_OFFSET) >= EBDA_DEFAULT_SIZE)) {

      BootTablePtr = (UINT32 *)(UINTN)((BDA(EXT_DATA_SEG_OFFSET) << 4) + EBDA(LEGACY_BOOT_TABLE_OFFSET));

      if ((*BootTablePtr != 0xFFFFFFFF) && (*BootTablePtr != 0)) {
        return GetBootCurrent();
      }
    }
  }
  return INVALID;
}


/**
  S4 sleep entry callback for save boot current to CMOS using gEfiSmmSxDispatchProtocolGuid

  @param[in]      DispatchHandle  The handle of this callback, obtained when registering
  @param[in]      DispatchContext The predefined context which contained sleep type and phase

**/
VOID
EFIAPI
S4SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
{
  //
  // Save Boot Current into offset 0x5f of CMOS if Legacy boot,
  // else if UEFI boot then save value to 0xFF(that mean is invalid).
  //
  WriteCmos8 (LastBootDevice, SaveLastBootDevice());
}

/**
  S4 sleep entry callback for save boot current to CMOS using gEfiSmmSxDispatch2ProtocolGuid

  @param[in]      DispatchHandle  The unique handle assigned to this handler by SmiHandlerRegister().
  @param[in]      Context         Points to an optional handler context which was specified when the
                                  handler was registered.
  @param[in, out] CommBuffer      A pointer to a collection of data in memory that will
                                  be conveyed from a non-SMM environment into an SMM environment.
  @param[in, out] CommBufferSize  The size of the CommBuffer.

  @retval EFI_SUCCESS             The S4 sleep entry callabck was handled successfully.

**/
EFI_STATUS
EFIAPI
S4SleepEntryCallBack2 (
  IN EFI_HANDLE                     DispatchHandle,
  IN CONST VOID                     *Context         OPTIONAL,
  IN OUT VOID                       *CommBuffer      OPTIONAL,
  IN OUT UINTN                      *CommBufferSize  OPTIONAL
  )
{
  //
  // Save Boot Current into offset 0x5f of CMOS if Legacy boot,
  // else if UEFI boot then save value to 0xFF(that mean is invalid).
  //
  WriteCmos8 (LastBootDevice, SaveLastBootDevice());
  return EFI_SUCCESS;
}
