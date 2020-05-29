/** @file
  This code supports a the private implementation
  of the S3 Restore ACPI callback

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

#include <PiPei.h>
#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/IoLib.h>
#include <Library/PcdLib.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/SmmControl.h>

extern EFI_GUID  gPeiS3RestoreAcpiCallbackPpiGuid;

EFI_STATUS
EFIAPI
S3RestoreAcpiCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
;

EFI_STATUS
EFIAPI
MemoryDiscoveredCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
;

EFI_PEI_NOTIFY_DESCRIPTOR mNotifyOnEndOfPeiList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiEndOfPeiSignalPpiGuid,
  S3RestoreAcpiCallback
};

EFI_PEI_NOTIFY_DESCRIPTOR mNotifyOnMemoryDiscoveredList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMemoryDiscoveredPpiGuid,
  MemoryDiscoveredCallback
};

EFI_PEI_PPI_DESCRIPTOR mS3RestoreAcpiPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiS3RestoreAcpiCallbackPpiGuid,
  NULL
};

/**
  ACPI Restore callback to be called when gEfiEndOfPeiSignalPpiGuid is installed.

  @param  PeiServices                 Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor            Address of the notification descriptor data structure. Type
          EFI_PEI_NOTIFY_DESCRIPTOR is defined above.
  @param  Ppi                         Address of the PPI that was installed.

  @retval   EFI_STATUS                This function will install a PPI to PPI database. The status
                                      code will be the code for (*PeiServices)->InstallPpi.

**/
EFI_STATUS
EFIAPI
S3RestoreAcpiCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
{
  EFI_STATUS                       Status;
  PEI_SMM_CONTROL_PPI              *SmmControl;
  UINT8                            SmiCommand;
  UINTN                            Size;

  DEBUG (( EFI_D_INFO, "S3RestoreAcpiPei Callback\r\n"));

  //
  // Get needed resource
  //
  Status = PeiServicesLocatePpi (
             &gPeiSmmControlPpiGuid,
             0,
             NULL,
             (VOID **)&SmmControl
             );
  if (EFI_ERROR (Status)) {
    return EFI_NOT_STARTED;
  }

  //
  // Send command
  //
  SmiCommand = PcdGet8(PcdAcpiRestoreSwSmi);
  Size = sizeof(SmiCommand);
  Status = SmmControl->Trigger (
                         PeiServices,
                         SmmControl,
                         (INT8 *)&SmiCommand,
                         &Size,
                         FALSE,
                         0
                         );
  ASSERT_EFI_ERROR (Status);

  return (*PeiServices)->InstallPpi ((CONST EFI_PEI_SERVICES **)PeiServices, &mS3RestoreAcpiPpiList);
}

/**
  Memory discovered callback to be called when gEfiPeiMemoryDiscoveredPpiGuid is installed.

  @param[in]  PeiServices                 Indirect reference to the PEI Services Table.
  @param[in]  NotifyDescriptor            Address of the notification descriptor data structure. Type
                                          EFI_PEI_NOTIFY_DESCRIPTOR is defined above.
  @param[in]  Ppi                             Address of the PPI that was installed.

  @retval   EFI_STATUS                    This function will install a PPI to PPI database. The status
                                          code will be the code for (*PeiServices)->InstallPpi.

**/
EFI_STATUS
EFIAPI
MemoryDiscoveredCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
{
  EFI_STATUS      Status;
  EFI_BOOT_MODE   BootMode;

  Status = (*PeiServices)->GetBootMode ((CONST EFI_PEI_SERVICES **)PeiServices, &BootMode);

  if (Status == EFI_SUCCESS) {
     if (BootMode == BOOT_ON_S3_RESUME) {
        Status = (*PeiServices)->NotifyPpi((CONST EFI_PEI_SERVICES **)PeiServices, &mNotifyOnEndOfPeiList);
        return Status;
     }
  }

  return EFI_ABORTED;
}

/**
  The Entry point of the S3RestoreAcpi PEIM

  This function is the Entry point of the S3 ACPI Restore PEIM

  @param[in]  FileHandle   Pointer to image file handle.
  @param[in]  PeiServices  Pointer to PEI Services Table   

  @retval EFI_SUCCESS      PPI successfully installed

**/
EFI_STATUS
EFIAPI
S3RestoreAcpiPeiInitialize (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS      Status;


  Status = (*PeiServices)->NotifyPpi((CONST EFI_PEI_SERVICES **)PeiServices, &mNotifyOnMemoryDiscoveredList);

  return Status;
}
