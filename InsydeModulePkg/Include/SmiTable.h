/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SMI_TABLE_DEFINE_H_
#define _SMI_TABLE_DEFINE_H_

#define SW_SMI_PORT  PcdGet16(PcdSoftwareSmiPort)

typedef enum {
  THUNK_TEST                 = 0x11,

  SMM_SECURITY_RESERVE_0x14  = 0x14,

  INT15_MXM_HOOK             = 0x15,
  FW_ACCESS_SW_SMI           = 0x16,

  SMM_SECURITY_RESERVE_0x23  = 0x23,
  SMM_SECURITY_RESERVE_0x30  = 0x30,
  SMM_SECURITY_RESERVE_0x31  = 0x31,
  SMM_SECURITY_RESERVE_0x32  = 0x32,

  FW_WRITE                   = 0x42, //no function
  FW_UPDATE                  = 0x43, //no function

  //
  // INT15_0D042H
  //
  INT15_0D042H               = 0x44,

  BIS_DATA_READ              = 0x45, //no function
  BIS_DATA_WRITE             = 0x46, //no function

  //
  // SMM_PnP_BIOS_CALL
  //
  SMM_PnP_BIOS_CALL          = 0x47,

  //
  // TCG MOR SMI
  //
  SMM_TCG_MOR_CALL           = 0x48,

  //
  // TCG PPI Get User Confirmation Status SMI
  //
  SMM_TCG_PPI_USER_CONFIRMATION_CALL      = 0x49,

  //
  // TPM2 CONTROL AREA
  //
  SMM_TPM2_CTRL_AREA_START_CALL = 0x4F,

  //
  // TCG PPI SMI
  //
  SMM_TCG_PPI_CALL           = 0x50,

  //
  // ASPM OVERRIDE SMI
  //
  SW_SMI_ASPM_OVERRIDE       = 0x73,

  //
  // Smm platform
  //
  EFI_KBC_PRESENT            = 0x77,

  EFI_ACPI_ENABLE_SW_SMI     = 0xA0,
  EFI_ACPI_DISABLE_SW_SMI    = 0xA1,
  EFI_ACPI_RESTORE_SW_SMI    = 0xE3,
  EFI_ACPI_S1_SW_SMI         = 0xE1,
  SECURE_BOOT_SW_SMI         = 0xEC,
  IHISI_SW_SMI               = 0xEF,
  SD_LEGACY_SMI              = 0xF8,

  //
  // Common_INT15_SMI
  //
  COMMON_INT15_SMI           = 0xF9,
  //
  // ATA_LEGACY_SMI
  //
  ATA_LEGACY_SMI             = 0xFA,
  //
  // DEFINE_CALL_SMM_HDD_SECURITY_DATA
  //
  SMM_HDD_SECURITY           = 0xFC,

  SMM_OS_RESET_SMI_VALUE    = 0xFB,

  //
  // USB_TX_SMI
  //
  USB_TX_SMI                 = 0xFD,

  //
  // O1394_SOFTSMM_PORT
  //
  O1394_SOFTSMM_PORT         = 0xFE
}SW_SMI_PORT_TABLE;

#endif
