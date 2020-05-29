/** @file
  TPM policy definitions

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _TPM_POLICY_H_
#define _TPM_POLICY_H_

//
// For PcdPeiTpmPolicy
//
#define SKIP_TPM_STARTUP                         ( 1 << 0  ) //BIT0
#define SKIP_MEASURE_CRTM_VERSION                ( 1 << 1  ) //BIT1
#define SKIP_MEASURE_MAIN_BIOS                   ( 1 << 2  ) //BIT2
#define SKIP_MEASURE_FV_IMAGE                    ( 1 << 3  ) //BIT3
#define SKIP_TPM_SELF_TEST                       ( 1 << 4  ) //BIT4

//
// For PcdDxeTpmPolicy
//
#define SKIP_TPM_REVOKE_TRUST                    ( 1 << 0  ) //BIT0
#define SKIP_INSTALL_TCG_ACPI_TABLE              ( 1 << 1  ) //BIT1
#define SKIP_TCG_DXE_LOG_EVENT_I                 ( 1 << 2  ) //BIT2
#define SKIP_MEASURE_HANDOFF_TABLES              ( 1 << 3  ) //BIT3
#define SKIP_MEASURE_ALL_BOOT_VARIABLES          ( 1 << 4  ) //BIT4
#define SKIP_MEASURE_ACTION                      ( 1 << 5  ) //BIT5
#define SKIP_MEASURE_SECURE_BOOT_VARIABLES       ( 1 << 6  ) //BIT6
#define SKIP_MEASURE_SEPARATOR_EVENT             ( 1 << 7  ) //BIT7
#define SKIP_MEASURE_LAUNCH_OF_FIRMWARE_DEBUGGER ( 1 << 8  ) //BIT8
#define SKIP_MEASURE_OS_LOADER_AUTHORITY         ( 1 << 9  ) //BIT9
#define SKIP_MEASURE_GPT_TABLE                   ( 1 << 10 ) //BIT10
#define SKIP_MEASURE_PE_IMAGE                    ( 1 << 11 ) //BIT11
#define SKIP_MEASURE_WAKE_EVENT                  ( 1 << 12 ) //BIT12

#endif

