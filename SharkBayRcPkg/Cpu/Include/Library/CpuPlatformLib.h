/** @file
  Header file for CpuPlatform Lib.

@copyright
  Copyright (c) 2012 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
#ifndef _CPU_PLATFORM_LIB_H_
#define _CPU_PLATFORM_LIB_H_

/**
  Check CPU Type of the platform

  @retval CPU_FAMILY              CPU type
**/
CPU_FAMILY
EFIAPI
GetCpuFamily (
  VOID
  );

/**
  Return Cpu stepping type

  @retval CPU_STEPPING                   Cpu stepping type
**/
CPU_STEPPING
EFIAPI
GetCpuStepping (
  VOID
  );

/**
  Determine if CPU is supported

  @retval TRUE                    CPU is supported
  @retval FALSE                   CPU is not supported
**/
BOOLEAN
IsCpuSupported (
  VOID
  );

/**
  Return CPU Sku

  @retval UINT8              CPU Sku
**/
UINT8
EFIAPI
GetCpuSku (
  VOID
  );

//
//  Mailbox Related Definitions
//
#define MAILBOX_TYPE_PCODE              0x00000001
#define MAILBOX_TYPE_OC                 0x00000002
#define PCODE_MAILBOX_INTERFACE_OFFSET  0x5DA4
#define PCODE_MAILBOX_DATA_OFFSET       0x5DA0
#define OC_MAILBOX_MSR                  0x00000150
#define MCHBAR_OFFSET                   0x48

//
// Mailbox commands
//
#define READ_ICC_MAX_CMD                  0x80002A03
#define SAMPLE_TSC_24AND100_CMD           0x80000009
#define READ_TSC24_LOWER_CMD              0x80000109
#define READ_TSC24_UPPER_CMD              0x80000209
#define READ_TSC100_LOWER_CMD             0x80000309
#define READ_TSC100_UPPER_CMD             0x80000409
#define READ_PCODE_CALIBRATED_CMD         0x80000509
#define WRITE_CONVERTION_RATIO_CMD        0x80000609
#define WRITE_PREVENT_BCLKOFF_CMD         0x80000709
#define WRITE_MEASURE_INTERVAL_CMD        0x80000809
#define WRITE_FSM_MEASURE_INTVL_CMD       0x80000909
#define START_CAL_VALUE                   0x85000000
#define READ_PL1_DUTY_CYCLE_CLAMP_ENABLE  0x00000015
#define WRITE_PL1_DUTY_CYCLE_CLAMP_ENABLE 0x00000016
#define READ_DDR_FORCE_2X_REFRESH         0x00000017
#define WRITE_DDR_FORCE_2X_REFRESH        0x00000018

#ifndef MAILBOX_WAIT_TIMEOUT
#define MAILBOX_WAIT_TIMEOUT  1000  ///< 1 millisecond
#endif
#ifndef MAILBOX_WAIT_STALL
#define MAILBOX_WAIT_STALL    1     ///< 1 microsecond
#endif
#ifndef MAILBOX_READ_TIMEOUT
#define MAILBOX_READ_TIMEOUT  10    ///< 10 microseconds
#endif

//
//  OC Mailbox Structures
//
typedef union _OC_MAILBOX_INTERFACE {
  UINT32 InterfaceData;
  struct {
    UINT8 CommandCompletion : 8;
    UINT8 Param1            : 8;
    UINT8 Param2            : 8;
    UINT8 Reserved          : 7;
    UINT8 RunBusy           : 1;
  } Fields;
} OC_MAILBOX_INTERFACE;

typedef struct _OC_MAILBOX_FULL {
  UINT32               Data;
  OC_MAILBOX_INTERFACE Interface;
} OC_MAILBOX_FULL;

//
//  OC Mailbox completion codes
//
#define OC_MAILBOX_CC_SUCCESS              0
#define OC_MAILBOX_CC_OC_LOCKED            1
#define OC_MAILBOX_CC_INVALID_DOMAIN       2
#define OC_MAILBOX_CC_MAX_RATIO_EXCEEDED   3
#define OC_MAILBOX_CC_MAX_VOLTAGE_EXCEEDED 4
#define OC_MAILBOX_CC_OC_NOT_SUPPORTED     5

//
//  PCODE Mailbox Structures
//
typedef union _PCODE_MAILBOX_INTERFACE {
  UINT32 InterfaceData;
  struct {
    UINT32 Command  : 8;
    UINT32 Address  : 21;
    UINT32 Reserved : 2;
    UINT32 RunBusy  : 1;
  } Fields;
} PCODE_MAILBOX_INTERFACE;

typedef struct _PCODE_MAILBOX_FULL {
  PCODE_MAILBOX_INTERFACE Interface;
  UINT32                  Data;
} PCODE_MAILBOX_FULL;

//
//  Pcode Mailbox completion codes
//
#define PCODE_MAILBOX_CC_SUCCESS              0
#define PCODE_MAILBOX_CC_ILLEGAL_CMD          1
#define PCODE_MAILBOX_CC_TIMEOUT              2
#define PCODE_MAILBOX_CC_ILLEGAL_DATA         3
#define PCODE_MAILBOX_CC_RESERVED             4
#define PCODE_MAILBOX_CC_ILLEGAL_VR_ID        5
#define PCODE_MAILBOX_CC_VR_INTERFACE_LOCKED  6
#define PCODE_MAILBOX_CC_VR_ERROR             7

#define READ_PCH_POWER_LEVELS_CMD             0x8000000A
#define READ_EXT_PCH_POWER_LEVELS_CMD         0x8000000B

EFI_STATUS
EFIAPI MailboxWrite (
  IN UINT32  MailboxType,
  IN UINT32  MailboxCommand,
  IN UINT32  MailboxData,
  OUT UINT32 *MailboxStatus
  )
/**
  Generic Mailbox function for mailbox write commands. This function will
  poll the mailbox interface for control, issue the write request, poll
  for completion, and verify the write was succussful.

  @param[IN]  MailboxType,
  @param[IN]  MailboxCommand,
  @param[IN]  MailboxData,
  @param[OUT] *MailboxStatus

  @retval EFI_STATUS
**/
;

EFI_STATUS
EFIAPI MailboxRead (
  IN UINT32  MailboxType,
  IN UINT32  MailboxCommand,
  OUT UINT32 *MailboxDataPtr,
  OUT UINT32 *MailboxStatus
  )
/**
  Generic Mailbox function for mailbox read commands. This function will write
  the read request, and populate the read results in the output data.

  @param[IN]  MailboxType,
  @param[IN]  MailboxCommand,
  @param[OUT] *MailboxDataPtr,
  @param[OUT] *MailboxStatus

  @retval EFI_STATUS
**/
;

EFI_STATUS
EFIAPI PollMailboxReady (
  IN UINT32 MailboxType
  )
/**
  Poll the run/busy bit of the mailbox until available or timeout expires.

  @param[IN]  MailboxType,

  @retval EFI_STATUS
**/
;
#endif
