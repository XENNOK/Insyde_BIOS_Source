//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright (c) 2006, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  CapsuleService.c

Abstract:

  Capsule Runtime Service.

--*/



#include "CapsuleService.h"

EFI_GUID mEfiCapsuleHeaderGuid = EFI_CAPSULE_GUID;
EFI_GUID gEfiSystemResourceTableGuid = EFI_SYSTEM_RESOURCE_TABLE_GUID;
EFI_GUID gWindowsUxCapsuleGuid = WINDOWS_UX_CAPSULE_GUID;
//
// The times of calling UpdateCapsule ()
//
UINTN       mTimes      = 0;


EFI_GUID mSupportedCapsuleGuidList[] = {
   WINDOWS_UX_CAPSULE_GUID
};


EFI_STATUS
EFIAPI
SupportCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
/*++

Routine Description:
  Check if the capsules are supported by the firmwares.

Arguments:
  CapsuleHeader            - Points to a capsule header.

Returns:
  EFI_SUCCESS              - Input capsules are supported by firmware.
  EFI_INCOMPATIBLE_VERSION - Incompatible firmware version.
  EFI_UNSUPPORTED          - Input capsule is not supported by the firmware.

--*/
{
  EFI_STATUS                 Status;
  EFI_SYSTEM_RESOURCE_TABLE  *Esrt;
  UINTN                      Index;
  
  for (Index = 0; Index < sizeof(mSupportedCapsuleGuidList)/sizeof(EFI_GUID); Index++) {
    if (EfiCompareGuid(&CapsuleHeader->CapsuleGuid, &mSupportedCapsuleGuidList[Index])) {
      return EFI_SUCCESS;
    }
  }
  
  Status = EfiLibGetSystemConfigurationTable (&gEfiSystemResourceTableGuid, &Esrt);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  for (Index = 0; Index < Esrt->FirmwareResourceCount; Index++) {
    if (EfiCompareGuid (&Esrt->FirmwareResources[Index].FirmwareClass, &CapsuleHeader->CapsuleGuid)) {
      return EFI_SUCCESS;
    }
  }
  return EFI_UNSUPPORTED;
}


/**
  The firmware implements to process the capsule image.

  @param  CapsuleHeader         Points to a capsule header.

  @retval EFI_SUCESS            Process Capsule Image successfully.
  @retval EFI_UNSUPPORTED       Capsule image is not supported by the firmware.
  @retval EFI_VOLUME_CORRUPTED  FV volume in the capsule is corrupted.
  @retval EFI_OUT_OF_RESOURCES  Not enough memory.
**/
EFI_STATUS
EFIAPI
ProcessCapsuleImage (
  IN EFI_CAPSULE_HEADER *CapsuleHeader
  )
/*++

Routine Description:
  The firmware implements to process the capsule image.
  
Arguments:
  CapsuleHeader            - Points to a capsule header.

Returns:
  EFI_SUCCESS              - Input capsules are supported by firmware.
  EFI_INCOMPATIBLE_VERSION - Incompatible firmware version.
  EFI_UNSUPPORTED          - Capsule image is not supported by the firmware.
  EFI_OUT_OF_RESOURCES     - Out of memory
  EFI_LOAD_ERROR           - Failed to load the capsule image
  EFI_DEVICE_ERROR         - Hardware errors
  EFI_SECURITY_VIOLATION   - Security check failed

--*/
{
  EFI_STATUS                  Status;
  EFI_SYSTEM_RESOURCE_TABLE   *Esrt;
  UINTN                       Index;
  ESRT_STATUS                 EsrtStatus;
  UINT32                      AttemptVersion;

  Status     = EFI_SUCCESS;
  EsrtStatus = ESRT_SUCCESS;

  if (SupportCapsuleImage (CapsuleHeader) != EFI_SUCCESS) {
    return EFI_UNSUPPORTED;
  }
  if (EfiCompareGuid(&CapsuleHeader->CapsuleGuid, &gWindowsUxCapsuleGuid)) {
    return PersistCapsuleImage (CapsuleHeader);
  }

  Status = EfiLibGetSystemConfigurationTable (&gEfiSystemResourceTableGuid, &Esrt);
  ASSERT_EFI_ERROR(Status);
  if (EFI_ERROR(Status)) {
    //
    // return EFI_UNSUPPORTED if ESRT table is not found
    //
    return EFI_UNSUPPORTED;
  }
  for (Index = 0; Index < Esrt->FirmwareResourceCount; Index++) {
    if (EfiCompareGuid (&Esrt->FirmwareResources[Index].FirmwareClass, &CapsuleHeader->CapsuleGuid)) {
      AttemptVersion = GetCapsuleSystemFirmwareVersion(CapsuleHeader);
      EfiSetVariable (
             L"EsrtLastAttemptVersion",
             &CapsuleHeader->CapsuleGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
             sizeof(UINT32),
             &AttemptVersion
           );
      EsrtStatus = PreInstallationCheck(CapsuleHeader);
      if (EsrtStatus != ESRT_SUCCESS) {
        switch (EsrtStatus) {
          case ESRT_ERROR_INSUFFICIENT_RESOURCES:
            Status = EFI_OUT_OF_RESOURCES;
            break;
          case ESRT_ERROR_INVALID_IMAGE_FORMAT:
            Status = EFI_LOAD_ERROR;
            break;
          case ESRT_ERROR_INCORRECT_VERSION:
            Status = EFI_INCOMPATIBLE_VERSION;
            break;
          case ESRT_ERROR_AC_NOT_CONNECTED:
          case ESRT_ERROR_INSUFFICIENT_BATTERY:
            Status = EFI_DEVICE_ERROR;
            break;
          case ESRT_ERROR_AUTHENTICATION:
            Status = EFI_SECURITY_VIOLATION;
            break;
          default:
            Status = EFI_UNSUPPORTED;
            break;
        }
        EfiSetVariable (
           L"EsrtLastAttemptStatus",
           &CapsuleHeader->CapsuleGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           sizeof(UINT32),
           &EsrtStatus
         );

        if (EsrtStatus != ESRT_SUCCESS) {
          //
          // System reset is required for updating the ESRT table
          //
          EfiResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
        }
      }
      break;
    }
  }
  if (Index >= (UINTN)Esrt->FirmwareResourceCount) {
    return EFI_UNSUPPORTED;
  }
  if (Status == EFI_SUCCESS) {
    Status = PersistCapsuleImage (CapsuleHeader);
    if (EFI_ERROR(Status)) {
      EsrtStatus = ESRT_ERROR_UNSUCCESSFUL;
    }
  }

  if (EsrtStatus != ESRT_SUCCESS) {
    EfiSetVariable (
           L"EsrtLastAttemptStatus",
           &CapsuleHeader->CapsuleGuid,
           EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
           sizeof(UINT32),
           &EsrtStatus
         );
   }

  return Status;
}



EFI_STATUS
EFIAPI
UpdateCapsule (
  IN EFI_CAPSULE_HEADER      **CapsuleHeaderArray,
  IN UINTN                   CapsuleCount,
  IN EFI_PHYSICAL_ADDRESS    ScatterGatherList OPTIONAL
  )
/*++

Routine Description:

  This code finds if the capsule needs reset to update, if no, update immediately.

Arguments:

  CapsuleHeaderArray             A array of pointers to capsule headers passed in
  CapsuleCount                   The number of capsule
  ScatterGatherList              Physical address of datablock list points to capsule
  
Returns:

  EFI STATUS
  EFI_SUCCESS                    Valid capsule was passed.If CAPSULE_FLAG_PERSIT_ACROSS_RESET is
                                 not set, the capsule has been successfully processed by the firmware.
                                 If it set, the ScattlerGatherList is successfully to be set.
  EFI_INVALID_PARAMETER          CapsuleCount is less than 1,CapsuleGuid is not supported.
  EFI_DEVICE_ERROR               Failed to SetVariable or AllocatePool or ProcessFirmwareVolume. 
  
--*/
{
  UINTN                     ArrayNumber;
  EFI_STATUS                Status;
  EFI_CAPSULE_HEADER       *CapsuleHeader;
  BOOLEAN                   NeedReset;
  BOOLEAN                   InitiateReset;
  CHAR16                    CapsuleVarName[30];
  CHAR16                    *TempVarName;  

  if ((CapsuleCount < 1) || (CapsuleCount > MAX_SUPPORT_CAPSULE_NUM)){
    return EFI_INVALID_PARAMETER;
  }

  CapsuleHeader     = NULL;
  NeedReset         = FALSE;
  InitiateReset     = FALSE;
  CapsuleVarName[0] = 0;
  //
  //Compare GUIDs with EFI_CAPSULE_GUID, if capsule header contains CAPSULE_FLAGS_PERSIST_ACROSS_RESET
  //and CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE flags,whatever the GUID is ,the service supports.
  //
  for (ArrayNumber = 0; ArrayNumber < CapsuleCount; ArrayNumber++) {
    //
    // A capsule which has the CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE flag must have
    // CAPSULE_FLAGS_PERSIST_ACROSS_RESET set in its header as well.
    //
    CapsuleHeader = CapsuleHeaderArray[ArrayNumber];
    if ((CapsuleHeader->Flags & (CAPSULE_FLAGS_PERSIST_ACROSS_RESET | CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE)) == CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // A capsule which has the CAPSULE_FLAGS_INITIATE_RESET flag must have
    // CAPSULE_FLAGS_PERSIST_ACROSS_RESET set in its header as well.
    //
    if ((CapsuleHeader->Flags & (CAPSULE_FLAGS_PERSIST_ACROSS_RESET | CAPSULE_FLAGS_INITIATE_RESET)) == CAPSULE_FLAGS_INITIATE_RESET) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // Check Capsule image without populate flag by firmware support capsule function  
    //
    if (((CapsuleHeader->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) == 0) && 
        (SupportCapsuleImage (CapsuleHeader) != EFI_SUCCESS)) {
      return EFI_UNSUPPORTED;
    }
    //
    // ScatterGatherList is only referenced if the capsules are defined to persist across
    // system reset. 
    //
    if (((CapsuleHeader->Flags & CAPSULE_FLAGS_PERSIST_ACROSS_RESET) == CAPSULE_FLAGS_PERSIST_ACROSS_RESET) &&
         ScatterGatherList == (EFI_PHYSICAL_ADDRESS) (UINTN) NULL) {
      return EFI_INVALID_PARAMETER;
    } 
  }

  //
  // Walk through all capsules, record whether there is a capsule needs reset
  // or initiate reset. And then process capsules which has no reset flag directly.
  //
  for (ArrayNumber = 0; ArrayNumber < CapsuleCount ; ArrayNumber++) {
    CapsuleHeader = CapsuleHeaderArray[ArrayNumber];
    Status = ProcessCapsuleImage(CapsuleHeader);
      if (EFI_ERROR(Status) && !NeedReset) {
      return Status;
    }    

    //
    // Here should be in the boot-time for non-reset capsule image
    // Platform specific update for the non-reset capsule image.
    //
    if ((CapsuleHeader->Flags & CAPSULE_FLAGS_PERSIST_ACROSS_RESET) == CAPSULE_FLAGS_PERSIST_ACROSS_RESET) {

      NeedReset = TRUE;
      if ((CapsuleHeader->Flags & CAPSULE_FLAGS_INITIATE_RESET) != 0) {
        InitiateReset = TRUE;
      }
    }
  }
  
  //
  // After launching all capsules who has no reset flag, if no more capsules claims
  // for a system reset just return.
  //
  if (!NeedReset) {
    return EFI_SUCCESS;
  }

  //
  // Construct variable name CapsuleUpdateData, CapsuleUpdateData1, CapsuleUpdateData2...
  // if user calls UpdateCapsule multiple times.
  //
  EfiStrCpy (CapsuleVarName, EFI_CAPSULE_VARIABLE_NAME);
  TempVarName = CapsuleVarName + EfiStrLen (CapsuleVarName);
  if (mTimes > 0) {
 //   UnicodeValueToString (TempVarName, 0, mTimes, 0);
  }

  //
  // ScatterGatherList is only referenced if the capsules are defined to persist across
  // system reset. Set its value into NV storage to let pre-boot driver to pick it up 
  // after coming through a system reset.
  //
  Status = EfiSetVariable (
             CapsuleVarName,
             &gEfiCapsuleVendorGuid,
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_BOOTSERVICE_ACCESS,
             sizeof (UINTN),
             (VOID *) &ScatterGatherList
             );
  if (!EFI_ERROR (Status)) {
     //
     // Variable has been set successfully, increase variable index.
     //
     mTimes++;
     if(InitiateReset) {
       //
       // Firmware that encounters a capsule which has the CAPSULE_FLAGS_INITIATE_RESET Flag set in its header
       // will initiate a reset of the platform which is compatible with the passed-in capsule request and will 
       // not return back to the caller.
       //
       EfiResetSystem (EfiResetWarm, EFI_SUCCESS, 0, NULL);
     }
  }
  return Status;
  
}


  
EFI_STATUS
EFIAPI
QueryCapsuleCapabilities(
  IN  EFI_CAPSULE_HEADER   **CapsuleHeaderArray,
  IN  UINTN                CapsuleCount,
  OUT UINT64               *MaxiumCapsuleSize,
  OUT EFI_RESET_TYPE       *ResetType
  )
/*++

Routine Description:

  This code is query about capsule capability.

Arguments:

  CapsuleHeaderArray              A array of pointers to capsule headers passed in
  CapsuleCount                    The number of capsule
  MaxiumCapsuleSize               Max capsule size is supported
  ResetType                       Reset type the capsule indicates, if reset is not needed,return EfiResetCold.
                                  If reset is needed, return EfiResetWarm.

Returns:

  EFI STATUS
  EFI_SUCCESS                     Valid answer returned
  EFI_INVALID_PARAMETER           MaxiumCapsuleSize is NULL,ResetType is NULL.CapsuleCount is less than 1,CapsuleGuid is not supported.
  EFI_UNSUPPORTED                 The capsule type is not supported.

--*/
{
  UINTN                     ArrayNumber;
  EFI_CAPSULE_HEADER        *CapsuleHeader;


  if ((CapsuleCount < 1) || (CapsuleCount > MAX_SUPPORT_CAPSULE_NUM)){
    return EFI_INVALID_PARAMETER;
  }

  if ((MaxiumCapsuleSize == NULL) ||(ResetType == NULL)) {
    return EFI_INVALID_PARAMETER;
  }  

  CapsuleHeader = NULL;
  
  //
  //Compare GUIDs with EFI_CAPSULE_GUID, if capsule header contains CAPSULE_FLAGS_PERSIST_ACROSS_RESET
  //and CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE flags,whatever the GUID is ,the service supports.
  //
  for (ArrayNumber = 0; ArrayNumber < CapsuleCount; ArrayNumber++) {
    //
    // A capsule which has the CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE flag must have
    // CAPSULE_FLAGS_PERSIST_ACROSS_RESET set in its header as well.
    //
    CapsuleHeader = CapsuleHeaderArray[ArrayNumber];
    if ((CapsuleHeader->Flags & (CAPSULE_FLAGS_PERSIST_ACROSS_RESET | CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE)) == CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) {
      return EFI_INVALID_PARAMETER;
    }
    //
    // A capsule which has the CAPSULE_FLAGS_INITIATE_RESET flag must have
    // CAPSULE_FLAGS_PERSIST_ACROSS_RESET set in its header as well.
    //
    if ((CapsuleHeader->Flags & (CAPSULE_FLAGS_PERSIST_ACROSS_RESET | CAPSULE_FLAGS_INITIATE_RESET)) == CAPSULE_FLAGS_INITIATE_RESET) {
      return EFI_INVALID_PARAMETER;
    }
    if (!EfiCompareGuid (&CapsuleHeader->CapsuleGuid, &mEfiCapsuleHeaderGuid)) {
      if ((CapsuleHeader->Flags & CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE) == 0) {
        return EFI_UNSUPPORTED;
      }
    }  
  }

  //
  //Assume that capsules have the same flags on reseting or not. 
  //
  CapsuleHeader = CapsuleHeaderArray[0];  
  if ((CapsuleHeader->Flags & CAPSULE_FLAGS_PERSIST_ACROSS_RESET) != 0) {
    *ResetType = EfiResetWarm;
    *MaxiumCapsuleSize = MAX_SIZE_POPULATE;    
  } else {
    *ResetType = EfiResetCold;
    *MaxiumCapsuleSize = MAX_SIZE_NON_POPULATE;
  }  
  return EFI_SUCCESS;
} 


EFI_DRIVER_ENTRY_POINT (CapsuleServiceInitialize)

EFI_STATUS
EFIAPI
CapsuleServiceInitialize (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
/*++

Routine Description:

  This code is capsule runtime service initialization.
  
Arguments:

  ImageHandle          The image handle
  SystemTable          The system table.

Returns:

  EFI STATUS

--*/
{
  EFI_STATUS  Status;
  EFI_HANDLE  Handle;
  EFI_EVENT   Event;
  
  Handle       = NULL;

  EfiInitializeRuntimeDriverLib (ImageHandle, SystemTable, NULL);

  SystemTable->RuntimeServices->UpdateCapsule                    = UpdateCapsule;
  SystemTable->RuntimeServices->QueryCapsuleCapabilities         = QueryCapsuleCapabilities;

  //
  // Now install the Capsule Architectural Protocol on a new handle
  //

  Status = gBS->InstallMultipleProtocolInterfaces (
                  &Handle,
                  &gEfiCapsuleArchProtocolGuid,
                  NULL,
                  NULL
                  );
   
  if (Status == EFI_SUCCESS) {               
    Status = gBS->CreateEventEx (
                    EFI_EVENT_NOTIFY_SIGNAL,
                    EFI_TPL_NOTIFY,
                    TriggerFirmwareUpdate,
                    ImageHandle,
                    &gEfiEventReadyToBootGuid,
                    &Event
                    );
    InstallEfiSystemResourceTable();                    
  }
                  
  return Status;
}