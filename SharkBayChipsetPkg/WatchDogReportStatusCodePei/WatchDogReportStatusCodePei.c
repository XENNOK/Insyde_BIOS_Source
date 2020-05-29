/** @file

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <WatchDogReportStatusCodePei.h>
#include <Library/WatchDogLib.h>
#include <Library/PcdLib.h>


EFI_PEI_NOTIFY_DESCRIPTOR mRscHandlerPpiNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiRscHandlerPpiGuid,
  RscHandlerPpiNotifyCallback
  };


/**
  Notification function for ReportStatusCode Handler Ppi

  This routine is the notification function for EFI_RSC_HANDLER_PPI

  @param  PeiServices           Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor      Address of the notification descriptor data structure. Type
                                EFI_PEI_NOTIFY_DESCRIPTOR is defined above.
  @param  Ppi                   Address of the PPI that was installed.

  @retval EFI_STATUS            

**/
EFI_STATUS
EFIAPI
RscHandlerPpiNotifyCallback (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_RSC_HANDLER_PPI           *RscHandlerPpi;

  Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  &gEfiPeiRscHandlerPpiGuid,
                                  0,
                                  NULL,
                                  (VOID **) &RscHandlerPpi
                                  );
  if (!EFI_ERROR(Status)) {
    //
    // Register the worker function to ReportStatusCodeRouter
    //
    Status = RscHandlerPpi->Register (PeiWatchDogStatusCode);
  } 
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Report Status Code to PEI Post Message.

  @param  PeiServices      An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param  CodeType         Indicates the type of status code being reported.
  @param  CodeValue        Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. A system may contain multiple entities that match a class/subclass
                           pairing. The instance differentiates between them. An instance of 0 indicates
                           that instance information is unavailable, not meaningful, or not relevant.
                           Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS     
**/
EFI_STATUS
EFIAPI
PeiWatchDogStatusCode (
  IN CONST EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue, 
  IN UINT32                         Instance  OPTIONAL,
  IN CONST EFI_GUID                 *CallerId OPTIONAL,
  IN CONST EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  ) 
{

  UINT32 CodePhase = 0;
  TCO_WATCHDOG_TYPE  WatchDogType = 0;
  
  
  if (FeaturePcdGet(PcdH2OWatchDogSupported) == FALSE) {  
    return EFI_UNSUPPORTED;
  }

    
  CodePhase = CodeValue & (EFI_SOFTWARE|0xFF0000);
  
  switch (CodePhase) {
    case EFI_SOFTWARE_SEC:
    case EFI_SOFTWARE_PEI_CORE:
    case EFI_SOFTWARE_PEI_MODULE:
      WatchDogType = PEI_WATCH_DOG;
        break;
    case EFI_SOFTWARE_DXE_CORE:
    case EFI_SOFTWARE_DXE_BS_DRIVER:
    case EFI_SOFTWARE_DXE_RT_DRIVER:
      WatchDogType = DXE_WATCH_DOG;
        break;

    case EFI_SOFTWARE_SMM_DRIVER:
      WatchDogType = SMM_WATCH_DOG;
        break;
    default:
      WatchDogType = END_WATCH_DOG;
        break;
  }

  if ((CodeType == EFI_PROGRESS_CODE) && (CodeValue & EFI_SW_PC_INIT_BEGIN)) {
    PeiWatchDogGetSetupSetting (PeiServices);
    WatchDogStart (WatchDogType);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PeiWatchDogGetSetupSetting (
  IN CONST EFI_PEI_SERVICES         **PeiServices
)
{
  EFI_STATUS                        Status;
  CHIPSET_CONFIGURATION             *SystemConfiguration;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI   *Variable;
  UINTN                             VariableSize;
  EFI_GUID                          SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  UINT8                             WatchDogAction = 0;

  Variable                = NULL;
  VariableSize            = 0;


  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **)&Variable
             );
  
  if (EFI_ERROR(Status)) {
    return Status;
  }


  VariableSize = PcdGet32 (PcdSetupConfigSize);
  SystemConfiguration = AllocateZeroPool (VariableSize);

  Status = Variable->GetVariable (
             Variable,
             L"Setup",
             &SystemConfigurationGuid,
             NULL,
             &VariableSize,
             (VOID *)SystemConfiguration
             );

  if (EFI_ERROR(Status)) {
    return Status;
  }

    WatchDogAction = (SystemConfiguration->WDTOsLoad) << 2;
    WatchDogAction |= (SystemConfiguration->WDTOpromScan) << 1;
    WatchDogAction |= SystemConfiguration->BiosWDT;
    
    PcdSet8 (PcdH2OWatchDogMask, WatchDogAction);

  return Status;
}


/**
  Perform PEI Post Message Init.

  @param [in] FileHandle           Handle of the file being invoked. 
  @param [in] PeiServices          General purpose services available to every PEIM.

  @retval EFI Status            
**/
EFI_STATUS
EFIAPI
WatchDogReportStatusCodePeiEntryPoint (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                            Status;
  EFI_PEI_RSC_HANDLER_PPI               *RscHandlerPpi;

  if (FeaturePcdGet(PcdH2OWatchDogSupported) == FALSE) {  
    return EFI_SUCCESS;
  }

  Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  &gEfiPeiRscHandlerPpiGuid,
                                  0,
                                  NULL,
                                  (VOID **) &RscHandlerPpi
                                  );
  if (EFI_ERROR (Status)) {
    //
    // Register callback function for PEI_RSC_HANDLER_PPI.
    //
    Status = (**PeiServices).NotifyPpi (PeiServices, &mRscHandlerPpiNotify); 
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {
    Status = RscHandlerPpi->Register (PeiWatchDogStatusCode);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }
  
  return Status;
}

