/** @file

  Definitions for WheaPlatform driver.

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

#ifndef _WHEA_PLATFORM_SUPPORT_H_
#define _WHEA_PLATFORM_SUPPORT_H_

///=================================================
///  MdePkg\Include\
///=================================================
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Protocol/SmmSwDispatch2.h>

///=================================================
///  $(INSYDE_APEI_PKG)\Include\
///=================================================
#include <WheaErrorLib.h>
#include <Library/WheaLib.h>
#include <Protocol/WheaSupport.h>
#include <Protocol/WheaPlatform.h>

///
/// Private data of WheaPlatformSupport protocol
///
typedef struct {
  UINT32                          Signature;
  EFI_HANDLE                      Handle;
  EFI_WHEA_PLATFORM_PROTOCOL      WheaPlatformProtocol;
} INSYDEH2O_WHEA_PLATFORM_INSTANCE;

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
);

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
  IN UINT16                            Type
);

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
);

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
);

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
);

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
);

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
);

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
);

#endif
