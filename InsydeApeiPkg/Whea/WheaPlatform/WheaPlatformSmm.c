/** @file

  WheaPlatform driver functions.

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

#include "WheaPlatformSmm.h"

BOOLEAN                                mWheaEnable  = FALSE;
BOOLEAN                                mCorrectable = FALSE;
UINTN                                  mErrorStatusBlockSize;
VOID                                   *mErrorStatusBlock = NULL;


INSYDEH2O_WHEA_PLATFORM_INSTANCE       *mWheaPlatformIns;

/**
  Entry point of the Whea Platform Support driver.

  @param [in] ImageHandle         The image handle of the DXE Driver, DXE Runtime Driver, 
                                  DXE SMM Driver, or UEFI Driver.
  @param [in] SystemTable         A pointer to the EFI System Table.

  @retval EFI_SUCCESS             Driver initialized successfully.
  @retval EFI_OUT_OF_RESOURCES    Could not allocate needed resources

**/
EFI_STATUS
EFIAPI
InsydeWheaPlatformEntry (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                           Status;
  EFI_WHEA_SUPPORT_PROTOCOL            *WheaSupport;
  UINTN                                Index;
  EFI_SMM_SW_DISPATCH2_PROTOCOL        *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT          SwContext;
  EFI_HANDLE                           SwHandle;
  INSYDEH2O_WHEA_PLATFORM_INSTANCE     *WheaPlatformIns;

  if (InSmm()) {
    //
    // In SMM now.
    //
    //
    // Allocate smram for private data and interface instance
    //
    Status = gSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      sizeof(INSYDEH2O_WHEA_PLATFORM_INSTANCE),
                      &mWheaPlatformIns
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Initialize protocol interface
    //
    mWheaPlatformIns->WheaPlatformProtocol.InsydeWheaPlatformRestartAllElog = InsydeWheaPlatformRestartAllElog;
    mWheaPlatformIns->WheaPlatformProtocol.InsydeWheaPlatformRestartElog    = InsydeWheaPlatformRestartElog;
    mWheaPlatformIns->WheaPlatformProtocol.InsydeWheaPlatformEndElog        = InsydeWheaPlatformEndElog; 
    mWheaPlatformIns->WheaPlatformProtocol.InsydeWheaPlatformElogMemory     = InsydeWheaPlatformElogMemory;
    mWheaPlatformIns->WheaPlatformProtocol.InsydeWheaPlatformElogPcieRootDevBridge = InsydeWheaPlatformElogPcieRootDevBridge;
    mWheaPlatformIns->WheaPlatformProtocol.InsydeWheaPlatformElogPciDev     = InsydeWheaPlatformElogPciDev;
    //
    // Install runtime protocol for WHEA error log functions
    //
    mWheaPlatformIns->Handle = NULL;
    Status = gSmst->SmmInstallProtocolInterface (
                      &(mWheaPlatformIns->Handle),
                      &gEfiWheaPlatformProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &(mWheaPlatformIns->WheaPlatformProtocol)
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Allocate memory for instance
    //
    Status = gBS->AllocatePool (
                    EfiReservedMemoryType, 
                    sizeof(INSYDEH2O_WHEA_PLATFORM_INSTANCE), 
                    &WheaPlatformIns
                    );

    ZeroMem (WheaPlatformIns, sizeof(INSYDEH2O_WHEA_PLATFORM_INSTANCE));
    //
    //  Install the Protocol Interface in the Boot Time Space. This is requires to satisfy the
    //  dependency within the drivers that are dependent upon Smm Runtime Driver.
    //    
    CopyMem (
      WheaPlatformIns, 
      mWheaPlatformIns, 
      sizeof(INSYDEH2O_WHEA_PLATFORM_INSTANCE)
      );

    //
    // Register our SMI handlers
    //
    Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, &SwDispatch);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Install SW SMI handler to enable WHEA
    //
    SwContext.SwSmiInputValue = EFI_WHEA_ENABLE_SWSMI;
    Status = SwDispatch->Register (
                           SwDispatch,
                           InsydeWheaPlatformEnable,
                           &SwContext,
                           &SwHandle
                           );
    if (EFI_ERROR (Status)) {
      return Status;
    }    
    //
    // Install SW SMI handler to disable WHEA
    //
    SwContext.SwSmiInputValue = EFI_WHEA_DISABLE_SWSMI;
    Status = SwDispatch->Register (
                           SwDispatch,
                           InsydeWheaPlatformDisable,
                           &SwContext,
                           &SwHandle
                           );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = gBS->LocateProtocol (&gEfiWheaSupportProtocolGuid, NULL, &WheaSupport);
    if (EFI_ERROR (Status)) {
      return Status;
    }
    //
    // Retrive ErrorStatusBlock info.
    //
    Status = WheaSupport->InsydeWheaGetErrorStatusBlock (
                            WheaSupport, 
                            &mErrorStatusBlockSize, 
                            &mErrorStatusBlock
                            );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    for (Index = 0; Index < NumSysGenericErrorSources; Index++) {
      //
      // Adding Generic HW error source to HEST table
      //
      Status = WheaSupport->InsydeWheaAddErrorSource (
                              WheaSupport,
                              GenericHw,
                              SysGenErrSources[Index].Flags,
                              TRUE,
                              &SysGenErrSources[Index].SourceId,
                              1,
                              1,
                              SysGenErrSources[Index].SourceData
                              );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
  }

  return EFI_SUCCESS;
}

/**
  Search System Generic error HW source structures

  @param [in]  This               A pointer of WheaPlatformSuppourt protocol interface.
  @param [out] ErrSrc             Return Error Source.

  @retval EFI_SUCCESS             Driver initialized successfully.
  @retval EFI_OUT_OF_RESOURCES    Could not allocate needed resources

**/
STATIC
EFI_STATUS
InsydeWheaPlatformFindSysGenErrorSource (
  IN  UINT16                           Type,
  OUT SYSTEM_GENERIC_ERROR_SOURCE      **ErrSrc
  )
{
  UINTN                       Index;

  //
  // Search System Generic error HW source structures
  //
  for (Index = 0; Index < NumSysGenericErrorSources; Index++) {
    if (SysGenErrSources[Index].Type == Type) {
      *ErrSrc = (SYSTEM_GENERIC_ERROR_SOURCE*) &SysGenErrSources[Index];
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Clear all error data in all Error status blocks

  @param [in] This                A pointer of WheaPlatformSuppourt protocol interface.

  @retval EFI_SUCCESS             Clear success.
  @retval EFI_ABORTED             WHEA is not enabled.
  @retval EFI_NOT_FOUND           ErrorStatusBlock is not ready.

**/
STATIC
EFI_STATUS
InsydeWheaPlatformRestartAllElog(
  IN EFI_WHEA_PLATFORM_PROTOCOL        *This
  )
{
  UINT8     *pData;
  UINTN     Index;

  if (!mWheaEnable) {
    return EFI_ABORTED;
  }

  if (mErrorStatusBlock == NULL) {
    return EFI_NOT_FOUND;
  }
  //
  // Clear all error data in all Error status blocks.
  //
  pData = (UINT8 *)mErrorStatusBlock;
  for (Index = 0; Index < mErrorStatusBlockSize; Index++) {
    *pData = 0;
    pData++;
  }

  return EFI_SUCCESS;
}

/**
  To clean an error record specificed by Error type

  @param [in] This                A pointer of WheaPlatformSuppourt protocol interface
  @param [in] ErrorType           Error type record to clean

  @retval EFI_SUCCESS             Error record clean success.
  @retval EFI_ABORTED             WHEA is not enabled.

**/
STATIC
EFI_STATUS
InsydeWheaPlatformRestartElog(
  IN EFI_WHEA_PLATFORM_PROTOCOL        *This,
  IN UINT16                            ErrorType
  )
{
  UINT8                                     *pData;
  UINTN                                     Index;  
  UINTN                                     ErrSize; 
  UINTN                                     *ErrAddrReg;
  EFI_STATUS                                Status;
  SYSTEM_GENERIC_ERROR_SOURCE               *ErrSrc;  
  GENERIC_HW_ERROR_SOURCE                   *GenErr;
  UINT16                                    GenType = 0;  

  if (!mWheaEnable) {
    return EFI_ABORTED;
  }
  //
  // To identify it is a correct error or uncorrect error
  //
  if (ErrorType == GEN_ERR_SEV_PLATFORM_MEMORY_CORRECTED) {
    GenType = GENERIC_ERROR_CORRECTED;
  } else {
    GenType = GENERIC_ERROR_FATAL;
  }  
  //
  // Get the Error source structure
  //
  Status = InsydeWheaPlatformFindSysGenErrorSource (GenType, &ErrSrc);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  GenErr = (GENERIC_HW_ERROR_SOURCE *)ErrSrc->SourceData;
  
  ErrSize = (UINTN)GenErr->ErrorStatusSize;

  ErrAddrReg = (UINTN *)((UINTN)(GenErr->ErrorStatusAddress.Address));

  pData = (UINT8 *)(*ErrAddrReg);

  //
  // Clear specific error data in Error status blocks.
  //  
  for (Index = 0; Index < ErrSize; Index++) {
    *pData = 0;
    pData++;
  }  

  return EFI_SUCCESS;
}

/**
  To trigger a SCI if it is a correct error

  @param [in] This                A pointer of WheaPlatformSuppourt protocol interface

  @retval EFI_SUCCESS             Trigger SCI success.
  @retval EFI_ABORTED             Driver is not ready.

**/
STATIC
EFI_STATUS
InsydeWheaPlatformEndElog(
  IN EFI_WHEA_PLATFORM_PROTOCOL        *This
  )
{
  EFI_STATUS    Status;
  if (gSmst == NULL) {
    return EFI_ABORTED;
  }
  
  if (mCorrectable) {
    Status = InsydeWheaTriggerSci (gSmst);
  } else {
    Status = EFI_SUCCESS;
  }
  
  return Status;
}

/**
  To prepare Memory error record

  @param [in] This                A pointer of WheaPlatformSuppourt protocol interface
  @param [in] ErrorType           Error type to inject
  @param [in] MemInfo             Memory information

  @retval EFI_SUCCESS             Error record prepared.
  @retval EFI_ABORTED             WHEA is not enabled.

**/
STATIC
EFI_STATUS
InsydeWheaPlatformElogMemory(
  IN EFI_WHEA_PLATFORM_PROTOCOL        *This,
  IN UINT16                            ErrorType,
  IN MEMORY_DEV_INFO                   *MemInfo
  )
{
  EFI_STATUS                                Status;
  SYSTEM_GENERIC_ERROR_SOURCE               *ErrSrc;
  UINT16                                    GenType = 0;
  //
  // Return abort if WHEA is not enabled
  //
  if (!mWheaEnable) {
    return EFI_ABORTED;
  }
  //
  // To identify it is a correct error or uncorrect error
  //
  if (ErrorType == GEN_ERR_SEV_PLATFORM_MEMORY_CORRECTED) {
    GenType = GENERIC_ERROR_CORRECTED;
    mCorrectable = TRUE;
  } else {
    GenType = GENERIC_ERROR_FATAL;
    mCorrectable = FALSE;
  }  
  //
  // Get the Error source structure
  //
  Status = InsydeWheaPlatformFindSysGenErrorSource (GenType, &ErrSrc);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Writing error record
  //
  Status = InsydeWheaGenElogMemory (ErrorType, MemInfo, ErrSrc);

  return Status;
}

/**
  To prepare Pci root bridge Aer error record

  @param [in] This                A pointer of WheaPlatformSuppourt protocol interface.
  @param [in] ErrorType           Error type to inject.
  @param [in] ErrPcieDev          Pcie device information.

  @retval EFI_SUCCESS             Error record prepared.
  @retval EFI_ABORTED             WHEA is not enabled.

**/
STATIC
EFI_STATUS
InsydeWheaPlatformElogPcieRootDevBridge(
  IN EFI_WHEA_PLATFORM_PROTOCOL        *This,
  IN UINT16                            ErrorType,
  IN PCIE_PCI_DEV_INFO                 *ErrPcieDev
  )
{
  EFI_STATUS                                Status;
  SYSTEM_GENERIC_ERROR_SOURCE               *ErrSrc;
  UINT16                                    GenType = 0;

  //
  // Return abort if WHEA is not enabled
  //
  if (!mWheaEnable) {
    return EFI_ABORTED;
  }
  //
  // To identify it is a correct error or uncorrect error
  //
  if (ErrorType == GEN_ERR_SEV_PCIE_CORRECTED) {
    GenType = GENERIC_ERROR_CORRECTED;
    mCorrectable = TRUE;
  } else {
    GenType = GENERIC_ERROR_FATAL;
    mCorrectable = FALSE;
  }  
  //
  // Get the Error source structure
  //
  Status = InsydeWheaPlatformFindSysGenErrorSource (GenType, &ErrSrc);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Writing error record
  //
  Status = InsydeWheaGenElogPcieRootDevBridge (ErrorType, ErrPcieDev, ErrSrc);

  return Status;
}

/**
  To prepare Pci Device Aer error record

  @param [in] This                A pointer of WheaPlatformSuppourt protocol interface.
  @param [in] ErrorType           Error type to inject.
  @param [in] ErrPcieDev          Pcie device information.

  @retval EFI_SUCCESS             Error record prepared.
  @retval EFI_ABORTED             WHEA is not enabled.

**/
STATIC
EFI_STATUS
InsydeWheaPlatformElogPciDev(
  IN EFI_WHEA_PLATFORM_PROTOCOL        *This,
  IN UINT16                            ErrorType,
  IN PCIE_PCI_DEV_INFO                 *ErrPcieDev
  )
{
  EFI_STATUS                                Status;
  SYSTEM_GENERIC_ERROR_SOURCE               *ErrSrc;
  UINT16                                    GenType = 0;

  //
  // Return abort if WHEA is not enabled
  //
  if (!mWheaEnable) {
    return EFI_ABORTED;
  }
  //
  // To identify it is a correct error or uncorrect error
  //
  if (ErrorType == GEN_ERR_SEV_PCI_BUS_CORRECTED || ErrorType == GEN_ERR_SEV_PCI_DEV_CORRECTED) {
    GenType = GENERIC_ERROR_CORRECTED;
    mCorrectable = TRUE;
  } else {
    GenType = GENERIC_ERROR_FATAL;
    mCorrectable = FALSE;
  } 
  //
  // Get the Error source structure
  //
  Status = InsydeWheaPlatformFindSysGenErrorSource (GenType, &ErrSrc);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  //
  // Writing error record
  //
  Status = InsydeWheaGenElogPciDev (ErrorType, ErrPcieDev, ErrSrc);

  return Status;
}

/**
  A Smi handler to enable WHEA

  @param [in] DispatchHandle      Displatch handle.
  @param [in] DispatchContext     Dispatch context.

  @retval None

**/
EFI_STATUS
InsydeWheaPlatformEnable (
  IN       EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                        *DispatchContext, OPTIONAL
  IN OUT   VOID                        *CommBuffer,      OPTIONAL
  IN OUT   UINTN                       *CommBufferSize   OPTIONAL
  )
{
  DEBUG ((DEBUG_INFO, "[APEI_PLATFORM] INFO: %a() Start\n", __FUNCTION__));
  mWheaEnable = TRUE;

  InsydeWheaProgramSignalsHook (mWheaEnable, gSmst);

  DEBUG ((DEBUG_INFO, "[APEI_PLATFORM] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}

/**
  A Smi handler to disable WHEA

  @param [in] DispatchHandle      Displatch handle.
  @param [in] DispatchContext     Dispatch context.

  @retval None

**/
EFI_STATUS
InsydeWheaPlatformDisable (
  IN       EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                        *DispatchContext, OPTIONAL
  IN OUT   VOID                        *CommBuffer,      OPTIONAL
  IN OUT   UINTN                       *CommBufferSize   OPTIONAL
  )
{
  DEBUG ((DEBUG_INFO, "[APEI_PLATFORM] INFO: %a() Start\n", __FUNCTION__));

  mWheaEnable = FALSE;

  InsydeWheaProgramSignalsHook (mWheaEnable, gSmst);

  DEBUG ((DEBUG_INFO, "[APEI_PLATFORM] INFO: %a() End\n", __FUNCTION__));

  return EFI_SUCCESS;
}

