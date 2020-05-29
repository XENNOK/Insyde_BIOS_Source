/** @file

  WheaPlatform driver lib functions.

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

#ifndef _WHEA_ERROR_LIB_H_
#define _WHEA_ERROR_LIB_H_

//
// Statements that include other header files
//
#define PCIE_CAPABILITY_ID                          0x10
#define PCIE_EXTENDED_CAPABILITY_OFFSET             0x100
#define ADVANCED_ERROR_REPORTING_CAPABILITY_ID      0x001
#define ROOT_PORT_OF_PCI_EXPRESS_ROOT_COMPLEX       0x100
#define ROOT_COMPLEX_EVENT_COLLECTOR                0x1010
#define END_OF_PCIE_CAPABILITY_STRUCTURE            0x3c
#define END_OF_PCIE_AER_CAPABILITY_STRUCTURE        0x38

//=================================================
//  $(INSYDE_APEI_PKG)\Include\
//=================================================
#include <Protocol/WheaSupport.h>
#include <Protocol/WheaPlatform.h>

#define PCIE_SPECIFICATION_SUPPORTED                0x0110      // 1.1

#pragma pack(1)

typedef struct {
  UINT32              CapabilityID:16;
  UINT32              Version:4;
  UINT32              NextCapabilityOffset:12;
} PCIE_ENHANCED_CAPABILITY_HEADER;

#pragma pack()

/**
  BIOS SMI handler for handling WHEA error records.

  @param [in] ErrorType           Error Type that inject to platform.
  @param [in] MemInfo             Pointer to the specific memory device info structure.
  @param [in] SysErrSrc           Pointer to the generic error source structure.

  @retval EFI_NOT_FOUND           pointer of object not found.
  @retval EFI_INVALID_PARAMETER   Input invalid paramenter.
  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
InsydeWheaGenElogMemory(
  IN UINT16                            ErrorType,
  IN MEMORY_DEV_INFO                   *MemInfo,
  IN SYSTEM_GENERIC_ERROR_SOURCE       *SysErrSrc
);

/**
  Fill the PCIE Error message to specific errors status block.

  @param [in] ErrorType           Error Type that inject to platform.
  @param [in] ErrPcieDev          Pointer to the specific PCIE device info structure.
  @param [in] SysErrSrc           Pointer to the generic error source structure.

  @retval EFI_NOT_FOUND           pointer of object not found.
  @retval EFI_INVALID_PARAMETER   Input invalid paramenter.
  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
InsydeWheaGenElogPcieRootDevBridge(
  IN UINT16                            ErrorType,
  IN PCIE_PCI_DEV_INFO                 *ErrPcieDev,
  IN SYSTEM_GENERIC_ERROR_SOURCE       *SysErrSrc
);

/**
  Fill the PCI Error message to specific errors status block.

  @param [in] ErrorType           Error Type that inject to platform.
  @param [in] ErrPcieDev          Pointer to the specific PCIE device info structure.
  @param [in] SysErrSrc           Pointer to the generic error source structure.

  @retval EFI_NOT_FOUND           pointer of object not found.
  @retval EFI_INVALID_PARAMETER   Input invalid paramenter.
  @retval EFI_SUCCESS             The function completed successfully.

**/
EFI_STATUS
InsydeWheaGenElogPciDev(
  IN UINT16                            ErrorType,
  IN PCIE_PCI_DEV_INFO                 *ErrPcieDev,
  IN SYSTEM_GENERIC_ERROR_SOURCE       *SysErrSrc
);

/**
  Get the PCI Express Capability structure information from the specific Pci Express Device.

  @param [in, out] AerBuf         Pointer to the Capability Buffer.
  @param [in]      Segment        Specific PCIE Device's segment Number.
  @param [in]      Bus            Specific PCIE Device's bus Number.
  @param [in]      Device         Specific PCIE Device's device Number.
  @param [in]      Function       Specific PCIE Device's Function Number.

  @retval EFI_SUCCESS             Get Capability Structure Success.

**/
STATIC
EFI_STATUS
InsydeWheaGetPcieCapabilities (
  IN OUT PCIE_CAPABILITY               *CapBuf,
  IN     UINT8                         Segment,
  IN     UINT8                         Bus,
  IN     UINT8                         Device,
  IN     UINT8                         Function
);

/**
  Get the advanced error reporting structure information from the specific Pci Express Device.

  @param [in, out] AerBuf         Pointer to the Capability Buffer.
  @param [in]      Segment        Specific PCIE Device's segment Number.
  @param [in]      Bus            Specific PCIE Device's bus Number.
  @param [in]      Device         Specific PCIE Device's device Number.
  @param [in]      Function       Specific PCIE Device's Function Number.

  @retval EFI_SUCCESS             Get Capability Structure Success.

**/
STATIC
EFI_STATUS
InsydeWheaGetPcieAer (
  IN OUT PCIE_AER                      *AerBuf,
  IN     UINT8                         Segment,
  IN     UINT8                         Bus,
  IN     UINT8                         Device,
  IN     UINT8                         Function
) ;

#endif 

