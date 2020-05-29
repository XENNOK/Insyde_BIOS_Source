/** @file

   WheaLib headfile.

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

#ifndef _WHEA_LIB_H_
#define _WHEA_LIB_H_

//
// Statements that include other header files
//
//=================================================
//  MdePkg\Include\
//=================================================
#include <PiDxe.h>
#include <IndustryStandard/Acpi.h>
#include <Protocol/SmmBase2.h>

//=================================================
//  $(INSYDE_APEI_PKG)\Include\
//=================================================
#include <WheaDefs.h>
#include <Protocol/WheaPlatform.h>

//
// Data & Tables defined in WheaPlatform hooks
//
#define EFI_ACPI_CREATOR_ID           0x00000001
#define EFI_ACPI_CREATOR_REVISION     0x00000001
#define EFI_ACPI_OEM_TABLE_ID         SIGNATURE_64('H','2','O',' ','R','e','v','5') /// OEM table id 8 bytes long
#define EFI_ACPI_OEM_REVISION         0x00000001

extern SYSTEM_GENERIC_ERROR_SOURCE      SysGenErrSources[];
extern UINTN                            NumSysGenericErrorSources;
extern SYSTEM_NATIVE_ERROR_SOURCE       SysNativeErrSources[];
extern UINTN                            NumNativeErrorSources;

/**
  Upadte the Whea Acpi Table Information.

  @param [in, out] TableHeader    A pointer of the WHEA ACPI table address

  @retval None

**/
VOID
InsydeWheaUpdateAcpiTableIds(
  IN OUT EFI_ACPI_DESCRIPTION_HEADER     *TableHeader
);

/**
  Initialize and configure GPIO[0] parameter for preparing to trigger SCI.

  @param [in] EnableWhea          Is WHEA enable.
  @param [in] Smst                A pointer of Smst.

  @retval EFI_SUCCESS             Initial the GPIO Success.

**/
EFI_STATUS
InsydeWheaProgramSignalsHook (
  IN BOOLEAN                           EnableWhea,
  IN EFI_SMM_SYSTEM_TABLE2             *Smst
);

/**
  Get the Memory Device Fru Information.

  @param [in] SmmBase             A pointer of Smm Base protocol interface.
  @param [in] Smst                A pointer of Smst.

  @retval EFI_SUCCESS             Trigger SCI Success.

**/
EFI_STATUS
InsydeWheaTriggerSci (
  IN EFI_SMM_SYSTEM_TABLE2             *Smst
);

/**
  Set the specific IO port to trigger NMI.

  @param [in] SmmBase             A pointer of Smm Base protocol interface.
  @param [in] Smst                A pointer of Smst.

  @retval EFI_SUCCESS             Trigger NMI Success.

**/
EFI_STATUS
InsydeWheaTriggerNmi (
  IN EFI_SMM_SYSTEM_TABLE2             *Smst
);

/**
  Get PM Base Address from ICH LPC 0x40h.

  @param None

  @retval Return the Power management base address.

**/
UINT16
InsydeWheaGetPmBaseAddr(
  VOID
);

/**
  Write IO port.

  @param [in] mIoPort             IO port.
  @param [in] mValue              Value to write.
  @param [in] Smst                A pointer of Smst.

  @retval None

**/
VOID
InsydeWheaWriteIo16 (
  IN UINT16                            mIoPort,
  IN UINT16                            mValue,
  IN EFI_SMM_SYSTEM_TABLE2             *Smst
);

/**
  Read IO port.

  @param [in] mIoPort             IO port.
  @param [in] mValue              Value to write.
  @param [in] Smst                A pointer of Smst.

  @retval None

**/
VOID
InsydeWheaReadIo16 (
  IN  UINT16                           mIoPort,
  OUT UINT16                           *mValue,
  IN  EFI_SMM_SYSTEM_TABLE2            *Smst
);

#endif
