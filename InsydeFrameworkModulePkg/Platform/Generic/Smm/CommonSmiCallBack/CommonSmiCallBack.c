//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#include "CommonSmiCallBack.h"

EFI_SMM_SYSTEM_TABLE                    *mSmst;
EFI_SMM_RUNTIME_PROTOCOL                *mSmmRT;
EFI_SMM_VARIABLE_PROTOCOL               *mSmmVariable;
EFI_PHYSICAL_ADDRESS                    *mBootOrderBuffer;
EFI_PHYSICAL_ADDRESS                    *mBootOptionBuffer;

EFI_DRIVER_ENTRY_POINT(CommonSmiInitialize)

EFI_STATUS
CommonSmiInitialize (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  BOOLEAN                               InSmm;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;
  EFI_SMM_SX_DISPATCH_PROTOCOL          *SxDispatch;
  EFI_SMM_SX_DISPATCH_CONTEXT           EntryDispatchContext;
  EFI_HANDLE                            WakeOnAcLossHandle;

  //
  // Initialize the EFI Runtime Library
  //
  InsydeEfiInitializeSmmDriverLib (ImageHandle, SystemTable, &InSmm);

  Status = gBS->LocateProtocol(&gEfiSmmRuntimeProtocolGuid, NULL, &SmmRT);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  if (InSmm) {

    //
    // Great!  We're now in SMM!
    //
    //
    // Initialize global variables
    //
    Status = gSMM->GetSmstLocation(gSMM, &mSmst);
   	if (EFI_ERROR(Status)) {
      return Status;
   	}

    Status = SmmRT->LocateProtocol (
                      &gEfiSmmRuntimeProtocolGuid,
                      NULL,
                      &mSmmRT
                      );
   	if (EFI_ERROR(Status)) {
      return Status;
   	}
  
    Status = mSmmRT->LocateProtocol (
                       &gEfiSmmVariableProtocolGuid,
                       NULL,
                       &mSmmVariable
                       );
   	if (EFI_ERROR(Status)) {
      return Status;
   	}

    //
    // Get the Sx dispatch protocol
    //
    Status = gBS->LocateProtocol (&gEfiSmmSxDispatchProtocolGuid,
                                  NULL,
                                  &SxDispatch
                                  );
   	if (EFI_ERROR(Status)) {
      return Status;
   	}

    //
    // Allocate the buffer for BootOrder Variable
    //
    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      0x400,
                      &mBootOrderBuffer
                      );

   	if (EFI_ERROR(Status)) {
      return Status;
   	}
    
    EfiCommonLibZeroMem ((VOID *)mBootOrderBuffer, 0x400); 
    
    //
    // Allocate the buffer for Boot Option Variable
    //
    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      EFI_PAGE_SIZE,
                      &mBootOptionBuffer
                      );
   	if (EFI_ERROR(Status)) {
      return Status;
   	}

    EfiCommonLibZeroMem ((VOID *)mBootOptionBuffer, EFI_PAGE_SIZE); 

  
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
  return Status;
}


UINT8
GetBootCurrent (
  )
/*++

Routine Description:

  Change the BBS index to Boot current when legacy boot
  
Arguments:

  None

Returns:

  0xff  - failed
  other - Boot current value
  
--*/

{
  UINTN                                OptionOrderSize = 0x400;
  UINT16                               *OptionOrder = NULL;
  UINT16                               PriorityIndex;
  UINT16                               BootOption[10];
  UINT16                               BootOrderNumber[10];  
  UINTN                                BootOptionSize = 0x1000;
  UINT8                                *Ptr = NULL, *BootOptionVar = NULL;
  UINT8                                CmosData = 0;
  UINT32                               Attributes;
  UINT16                               DevPathSize;
  CHAR16                               *BootDesc;

  OptionOrder = (UINT16 *)(mBootOrderBuffer);

  mSmmVariable->GetVariable ( 
                  L"BootOrder", 
                  &gEfiGlobalVariableGuid, 
                  &Attributes, 
                  &OptionOrderSize, 
                  (VOID *) OptionOrder
                  );
  
  if (OptionOrder == NULL) {
    return INVALID;
  }

  //
  // Set BBS priority according OptionOrder variable
  //
  for (PriorityIndex = 0; PriorityIndex < OptionOrderSize / sizeof (UINT16); PriorityIndex++) {
    EfiCommonLibZeroMem ((VOID *)BootOption, sizeof(BootOption)); 
    EfiCommonLibZeroMem ((VOID *)BootOrderNumber, sizeof(BootOrderNumber)); 
    EfiStrCpy (BootOption, L"Boot");
    EfiValueToString (BootOrderNumber, OptionOrder[PriorityIndex], (PREFIX_ZERO) , 4);
    EfiStrCat (BootOption, BootOrderNumber);

    BootOptionVar = (UINT8 *)(mBootOptionBuffer);
    BootOptionSize = 0x1000;
    mSmmVariable->GetVariable ( 
                    BootOption, 
                    &gEfiGlobalVariableGuid, 
                    &Attributes, 
                    &BootOptionSize, 
                    (VOID *) BootOptionVar); 
    if (BootOptionVar == NULL) {
      return INVALID;
    }
    //
    // Skip the native boot options(EFI shell...)
    //
    Ptr = BootOptionVar + sizeof(UINT32) + sizeof(UINT16) + EfiStrSize ((CHAR16 *)(BootOptionVar + 6));
    if (*Ptr != BBS_DEVICE_PATH) {
      continue;
    }

    Ptr = BootOptionVar;
    Ptr += sizeof (UINT32);
    DevPathSize = *((UINT16 *) Ptr);
    Ptr += sizeof (UINT16);
    BootDesc = (CHAR16*) Ptr;
    Ptr += EfiStrSize (BootDesc);
    Ptr += DevPathSize;
    Ptr += sizeof (BBS_TABLE);

    //
    // Compare BBS index
    //
    CmosData = EfiReadCmos8 (LastBootDevice);

    if (CmosData == *Ptr) {
      CmosData = (UINT8)OptionOrder[PriorityIndex];
      return CmosData;
    }
  }

  return INVALID;
}

UINT8
SaveLastBootDevice (
  VOID
  )
/*++

Routine Description:

  Check whether is Legacy boot
  
Arguments:

  None

Returns:

  VALUE           Legacy boot
  0xFF            UEFI boot or invalid's boot option
  
--*/ 
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

VOID
S4SleepEntryCallBack (
  IN  EFI_HANDLE                    DispatchHandle,
  IN  EFI_SMM_SX_DISPATCH_CONTEXT   *DispatchContext
  )
/*++

Routine Description:

Arguments:

  DispatchHandle  - The handle of this callback, obtained when registering

  DispatchContext - The predefined context which contained sleep type and phase

Returns:

/*++

Routine Description:

Arguments:

Returns:

--*/
{ 
  //
  // Save Boot Current into offset 0x5f of CMOS if Legacy boot,
  // else if UEFI boot then save value to 0xFF(that mean is invalid).
  //
  EfiWriteCmos8 (LastBootDevice, SaveLastBootDevice());
}


