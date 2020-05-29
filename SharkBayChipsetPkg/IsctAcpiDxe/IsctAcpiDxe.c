/** @file

  This Dxe driver will initialize ISCT Persistent Data Variable and also verify ISCT Store valid or not
  
;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
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

*/

#include <ChipsetSetupConfig.h>
#include <IndustryStandard/Acpi.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>

#include <Guid/AcpiVariable.h>
#include <Guid/IsctAcpiTableStorage.h>
#include <Guid/IsctPersistentData.h>

#include <Protocol/IsctNvsArea.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/AcpiSystemDescriptionTable.h>

ISCT_NVS_AREA_PROTOCOL mIsctNvsAreaProtocol;

/**
  Initialize ISCT ACPI tables

  @retval EFI_SUCCESS          Isct ACPI tables are initialized successfully
  @retval EFI_OUT_OF_RESOURCES Isct ACPI tables not found.
**/
STATIC
EFI_STATUS
InitializeIsctAcpiTables (
  VOID
  )
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         NumberOfHandles;
  EFI_FV_FILETYPE               FileType;
  UINT32                        FvStatus;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINTN                         Size;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *FwVol;
  INTN                          Instance;
  EFI_ACPI_COMMON_HEADER        *CurrentTable;
  UINT8                         *CurrPtr;
  UINT8                         *EndPtr;
  UINT32                        *Signature;
  EFI_ACPI_DESCRIPTION_HEADER   *IsctAcpiTable;
  BOOLEAN                       LoadTable;
  UINTN                         TableHandle;
  EFI_ACPI_TABLE_VERSION        Version;
  EFI_ACPI_TABLE_PROTOCOL       *AcpiTable;

  FwVol         = NULL;
  IsctAcpiTable = NULL;

  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  DEBUG((EFI_D_INFO, "ISCT :gBS->LocateProtocol -> AcpiTable Status = %x\n", Status));  
  ASSERT_EFI_ERROR (Status);

  //
  // Locate protocol.
  // There is little chance we can't find an FV protocol
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NumberOfHandles,
                  &HandleBuffer
                  );
  DEBUG((EFI_D_INFO, "ISCT :gBS->LocateHandleBuffer Status = %x\n", Status));  
  ASSERT_EFI_ERROR (Status);
  //
  // Looking for FV with ACPI storage file
  //
  for (Index = 0; Index < NumberOfHandles; Index++) {
    //
    // Get the protocol on this handle
    // This should not fail because of LocateHandleBuffer
    //
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID **)&FwVol
                    );
    DEBUG((EFI_D_INFO, "ISCT :gBS->HandleProtocol Status = %x\n", Status));  
    ASSERT_EFI_ERROR (Status);

    //
    // See if it has the ACPI storage file
    //
    Size      = 0;
    FvStatus  = 0;
    Status = FwVol->ReadFile (
                      FwVol,
                      &gIsctAcpiTableStorageGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );
    DEBUG((EFI_D_INFO, "ISCT :FwVol->ReadFile Status = %x\n", Status));  
    //
    // If we found it, then we are done
    //
    if (Status == EFI_SUCCESS) {
      break;
    }
  }
  //
  // Free any allocated buffers
  //
  FreePool (HandleBuffer);

  //
  // Sanity check that we found our data file
  //
  ASSERT (FwVol != NULL);
  if (FwVol == NULL) {
    return EFI_NOT_FOUND;
  }
  //
  // By default, a table belongs in all ACPI table versions published.
  //
  Version = EFI_ACPI_TABLE_VERSION_1_0B | EFI_ACPI_TABLE_VERSION_2_0 | EFI_ACPI_TABLE_VERSION_3_0;

  //
  // Our exit status is determined by the success of the previous operations
  // If the protocol was found, Instance already points to it.
  // Read tables from the storage file.
  //
  Instance      = 0;
  CurrentTable  = NULL;
  while (Status == EFI_SUCCESS) {
    Status = FwVol->ReadSection (
                      FwVol,
                      &gIsctAcpiTableStorageGuid,
                      EFI_SECTION_RAW,
                      Instance,
                      (VOID **)&CurrentTable,
                      &Size,
                      &FvStatus
                      );
    DEBUG((EFI_D_INFO, "ISCT :FwVol->ReadSection Status = %x\n", Status));
  
    if (!EFI_ERROR (Status)) {
      LoadTable = FALSE;
      //
      // Check the table ID to modify the table
      //
      if (((EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable)->OemTableId == SIGNATURE_64 ('I', 's', 'c', 't', 'T', 'a', 'b', 'l')) {
        IsctAcpiTable = (EFI_ACPI_DESCRIPTION_HEADER *) CurrentTable;
	      DEBUG((EFI_D_ERROR, "ISCT :Find out IsctTabl\n"));  
        //
        // Locate the SSDT package
        //
        CurrPtr = (UINT8 *) IsctAcpiTable;
        EndPtr  = CurrPtr + IsctAcpiTable->Length;

        for (; CurrPtr <= EndPtr; CurrPtr++) {
          Signature = (UINT32 *) (CurrPtr + 3);
          if (*Signature == SIGNATURE_32 ('I', 'S', 'C', 'T')) {
            LoadTable = TRUE;
            if((*(UINT32 *) (CurrPtr + 3 + sizeof (*Signature) + 2) == 0xFFFF0008)) {
            //
            // ISCT NVS Area address
            //
            *(UINT32 *) (CurrPtr + 3 + sizeof (*Signature) + 2) = (UINT32) (UINTN) mIsctNvsAreaProtocol.Area;       
            DEBUG((EFI_D_INFO, "ISCT :Modify OpRegion Address to %x\n", (*(UINT32 *) (CurrPtr + 3 + sizeof (*Signature) + 2))));           
            }

            if((*(UINT16 *) (CurrPtr + 3 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) == 0xAA58)) {
            //
            // ISCT NVS Area size
            //
            *(UINT16 *) (CurrPtr + 3 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1) = sizeof (ISCT_NVS_AREA);
            DEBUG((EFI_D_INFO, "ISCT :Modify OpRegion Size to %x\n", *(UINT16 *) (CurrPtr + 3 + sizeof (*Signature) + 2 + sizeof (UINT32) + 1)));
            }

            ///
            /// Add the table
            ///
            if (LoadTable) {
              TableHandle = 0;
              Status = AcpiTable->InstallAcpiTable (
                                  AcpiTable,
                                  CurrentTable,
                                  CurrentTable->Length,
                                  &TableHandle
                                  );
            }
            return EFI_SUCCESS;
          }
        }
      }
      //
      // Increment the instance
      //
      Instance++;
      CurrentTable = NULL;
    }
  }

  return Status;
}

/**
  Install Isct ACPI tables only when Isct is enabled

  @param[in]  Event    - The event that triggered this notification function  
  @param[in]  Context  - Pointer to the notification functions context

  @retval  None
**/
VOID
EFIAPI
IsctOnReadyToBoot (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS  Status;

  DEBUG ((EFI_D_INFO, "IsctOnReadyToBoot()\n"));

  Status = InitializeIsctAcpiTables ();
  DEBUG((EFI_D_INFO, "Initializes ISCT SSDT tables Status = %x\n", Status));  

  gBS->CloseEvent (Event);
}

/**
  ISCT DXE driver entry point function

  @param[in]  ImageHandle - Image handle for this driver image
  @param[in]  SystemTable - Pointer to the EFI System Table

  @retval  EFI_OUT_OF_RESOURCES - no enough memory resource when installing reference code information protocol
  @retval  EFI_SUCCESS          - function completed successfully
**/
EFI_STATUS
EFIAPI
IsctDxeEntryPoint (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS                    Status;
  EFI_EVENT                     Event;
  ISCT_NVS_AREA                 *IsctNvs;
  EFI_GUID                      IsctPersistentDataGuid = ISCT_PERSISTENT_DATA_GUID;
  ISCT_PERSISTENT_DATA          *mIsctData;
  UINT8                         IsctEnabled;
  EFI_GUID                      SetupGuid = SYSTEM_CONFIGURATION_GUID;
//[-start-130710-IB05160465-modify]//
//[-start-140625-IB05080432-modify]//
  VOID                          *SetupData = NULL;
//[-end-140625-IB05080432-modify]//
  UINTN                         VarSize;

  DEBUG ((EFI_D_INFO, "IsctDxe: Entry Point...\n"));
  IsctEnabled = 0;

  VarSize = PcdGet32 (PcdSetupConfigSize);
  SetupData = AllocateZeroPool (VarSize);
//[-start-140625-IB05080432-add]//
  if (SetupData == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-140625-IB05080432-add]//
  Status = gRT->GetVariable (
                  L"Setup",
                  &SetupGuid,
                  NULL,
                  &VarSize,
                  SetupData
                  );
//[-start-140625-IB05080432-modify]//
  if (EFI_ERROR(Status)) {    
    DEBUG ((EFI_D_INFO, "ISCT: Get Variable Status = %x\n", Status));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
//[-end-140625-IB05080432-modify]//

  IsctEnabled = ((CHIPSET_CONFIGURATION *)SetupData)->ISCTEnable;

  if(IsctEnabled == 0) {
    DEBUG ((EFI_D_INFO, "ISCT is Disabled \n"));    
    return EFI_SUCCESS;
  }

  if (EFI_ERROR (Status)) {
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

//   Status = gBS->LocateProtocol (
//                   &PlatformInfoProtocolGuid, 
//                   NULL, 
//                   &PlatformInfoProtocol
//                   );
//    ASSERT_EFI_ERROR (Status);
  //
  // Allocate pools for ISCT Global NVS area
  //
  Status = gBS->AllocatePool (EfiReservedMemoryType, sizeof (ISCT_NVS_AREA), (VOID **)&mIsctNvsAreaProtocol.Area);              
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Error to allocate pool for ISCT_NVS_AREA"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  ZeroMem ((VOID *) mIsctNvsAreaProtocol.Area, sizeof (ISCT_NVS_AREA));

  Status = (gBS->AllocatePool) (EfiReservedMemoryType, sizeof (ISCT_PERSISTENT_DATA), &mIsctNvsAreaProtocol.IsctData);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Error to allocate pool for ISCT_PERSISTENT_DATA"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }
  ZeroMem ((VOID *) mIsctNvsAreaProtocol.IsctData, sizeof (ISCT_PERSISTENT_DATA));

  IsctNvs = mIsctNvsAreaProtocol.Area;
  IsctNvs->IsctNvsPtr = (UINT32) (UINTN) IsctNvs;
  
  //
  // Isct WA: Add option for use of RTC timer on Mobile systems until EC wake through GP27 is working
  //   RTC = 1; EC = 0;
  //
  IsctNvs->IsctTimerChoice = ((CHIPSET_CONFIGURATION *)SetupData)->IsctTimerChoice; //get timer choice from setup options
//if(PlatformInfoProtocol->PlatformFlavor == FlavorDesktop) {
//	IsctNvs->IsctTimerChoice = 1; //use RTC timer for Desktop
//}
	
  IsctNvs->IsctEnabled = 1;
  IsctNvs->RapidStartEnabled = ((CHIPSET_CONFIGURATION *)SetupData)->EnableRapidStart;

  //
  // Assign IsctData pointer to GlobalNvsArea
  //
  mIsctData = mIsctNvsAreaProtocol.IsctData;  
  mIsctData->IsctNvsPtr = (UINT32) (UINTN) IsctNvs;

  FreePool (SetupData);
//[-end-130710-IB05160465-modify]//

  //
  // Install ISCT Global NVS protocol
  //
  
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &ImageHandle,
                  &gIsctNvsAreaProtocolGuid,
                  &mIsctNvsAreaProtocol,
                  NULL
                  );
  DEBUG((EFI_D_INFO, "Install IsctNvsAreaProtocolGuid = 0x%x\n", Status));

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Error to install ISCT_NVS_AREA_PROTOCOL"));
    ASSERT_EFI_ERROR (Status);
    return Status;
  }                  

  //
  // Save ISCT Data to Variable
  //
  Status = gRT->SetVariable (
                  ISCT_PERSISTENT_DATA_NAME,
                  &IsctPersistentDataGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  sizeof (ISCT_PERSISTENT_DATA),
                  mIsctData
                  );
  DEBUG((EFI_D_INFO, "ISCT DXE: Save ISCT Data to Variable Status = %x\n", Status)); 
  if (EFI_ERROR (Status)) {                   
    ASSERT_EFI_ERROR (Status);
    return Status;
  }   
  //
  // Register ready to boot event for ISCT
  //
   
  Status = EfiCreateEventReadyToBootEx (
            TPL_NOTIFY,
            IsctOnReadyToBoot,
            NULL,
            &Event
            );
  DEBUG((EFI_D_INFO, "Create ReadyToBoot event for ISCT Status = %x\n", Status));  
  if (EFI_ERROR (Status)) {                   
    ASSERT_EFI_ERROR (Status);
    return Status;
  }   


  DEBUG ((EFI_D_INFO, "(IsctDxe) entry End...\n"));
  
  return EFI_SUCCESS;
}
