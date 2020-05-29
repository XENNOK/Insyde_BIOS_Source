/** @file

  WheaSupport driver headfile.

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

#ifndef _WHEA_SUPPORT_DXE_H_
#define _WHEA_SUPPORT_DXE_H_

//
// Statements that include other header files
//
//=================================================
//  MdePkg\Include\
//=================================================
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>

//=================================================
//  IntelFrameworkPkg\Include\
//=================================================
#include <Protocol/AcpiSupport.h>

//=================================================
//  $(INSYDE_APEI_PKG)\Include\
//=================================================
#include <Library/WheaLib.h>
#include <WheaAcpi.h>
#include <Protocol/WheaSupport.h>


typedef struct _WHEA_SUPPORT_INST {
  UINTN                                          Signature;
  APEI_WHEA_HARDWARE_ERROR_SOURCE_TABLE          *Hest;
  APEI_WHEA_BOOT_ERROR_RECORD_TABLE              *Bert;
  APEI_WHEA_ERROR_RECORD_SERIALIZATION_TABLE     *Erst;
  APEI_WHEA_ERROR_INJECTION_TABLE                *Einj;
  UINTN                                          CurErrorStatusBlockSize;
  UINTN                                          ErrorStatusBlockSize;
  VOID                                           *ErrorStatusBlock;
  UINTN                                          ErrorLogAddressRangeSize;
  VOID                                           *ErrorLogAddressRange;
  UINTN                                          BootErrorRegionLen;
  VOID                                           *BootErrorRegion;
  BOOLEAN                                        TablesInstalled;
  UINTN                                          InstalledErrorInj;

  EFI_WHEA_SUPPORT_PROTOCOL                      WheaSupport;
} WHEA_SUPPORT_INST;

#define EFI_WHEA_SUPPORT_SIGNATURE SIGNATURE_32 ('W', 'H', 'E', 'A')

#define WHEA_SUPPORT_FROM_THIS(a) \
  CR (a, WHEA_SUPPORT_INST, WheaSupport, EFI_WHEA_SUPPORT_SIGNATURE)
  
/**
  Installing tables event callback.

  @param [in] Header              Table Address.
  @param [in] TableName           Tabel Name.
  
  @retval EFI_SUCCESS             WHEA Table initial success.
  @retval EFI_INVALID_PARAMETER   Acpi Table Address is null.

**/
EFI_STATUS
InsydeWheaInitialTableHeader (
  IN EFI_ACPI_DESCRIPTION_HEADER        *Header,
  IN UINT32                             TableName
  );

/**
  Installing tables event callback.

  @param [in] None
  
  @retval EFI_SUCCESS             WHEA Table initial success.

**/
EFI_STATUS
InsydeWheaInitialTable (
  VOID
  );

#endif
