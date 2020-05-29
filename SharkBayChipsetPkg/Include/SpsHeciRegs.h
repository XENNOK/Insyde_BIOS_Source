/** @file

  Register Definitions for HECI

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

#ifndef _SPS_HECI_REGS_H_
#define _SPS_HECI_REGS_H_

#include <SpsMeChipset.h>

#define SPS_HECI_BUS                   SPS_ME_BUS
#define SPS_HECI_DEV                   SPS_ME_DEVICE_NUMBER
#define SPS_HECI_FUN                   SPS_HECI_FUNCTION_NUMBER
#define SPS_HECI_PCI_ADDR              (SPS_HECI_BUS << 24) | (SPS_HECI_DEV << 16) | (SPS_HECI_FUN << 8)
#define SPS_REVID_MSK                  0x000000FF

#if 0 /// Not Used in SPS3EN
#define SPS_BRNGUP_HMRFPO_DISABLE_CMD_MASK  0x0F
#define SPS_BRNGUP_HMRFPO_DISABLE_CMD       0x30
#define SPS_BRNGUP_HMRFPO_DISABLE_OVR_MASK  0xF0000000
#define SPS_BRNGUP_HMRFPO_DISABLE_OVR_RSP   0x30000000
#endif

///
/// HECI PCI register definition
///
#define R_VENDORID 0x00
#define R_DEVICEID 0x02
#define R_COMMAND  0x04
#define B_BME      0x04
#define B_MSE      0x02
#define R_REVID    0x08
#define R_HECIMBAR 0x10
#define R_FWSTATE  0x40
#define R_GEN_STS  0x4C
#define R_HIDM     0xA0

///
/// HECIMBAR register definition
///
#define H_CB_WW    0x00
#define H_CSR      0x04
#define ME_CB_RW   0x08
#define ME_CSR_HA  0x0C

///
/// PCH related registers address
///
#define SPS_PCH_ACPI_TIMER_MAX_VALUE   0x1000000 ///< The timer is 24 bit overflow
///
/// HPET Information
///
#define SPS_HPET_ADDRESS_0             0xFED00000
#define SPS_HPET_ADDRESS_1             0xFED01000
#define SPS_HPET_ADDRESS_2             0xFED02000
#define SPS_HPET_ADDRESS_3             0xFED03000
///
/// HPET Registers will be used as DWORD index
///
#define SPS_HPET_CAPS_REG_LOW          (0x00 / 4)
#define SPS_HPET_CAPS_REG_HIGH         (0x04 / 4)
#define SPS_HPET_GEN_CONFIG_LOW        (0x10 / 4)
#define SPS_HPET_GEN_CONFIG_HIGH       (0x14 / 4)
#define SPS_HPET_INT_STATUS_LOW        (0x20 / 4)
#define SPS_HPET_INT_STATUS_HIGH       (0x24 / 4)
#define SPS_HPET_MAIN_COUNTER_LOW      (0xF0 / 4)
#define SPS_HPET_MAIN_COUNTER_HIGH     (0xF4 / 4)

#define SPS_HPET_START                 0x01
#define SPS_HPET_TICKS_PER_MICRO       14  /// 70ns tick so 14.2 ticks per microsecond ish
///
/// PEI Timeout values
///
#define SPS_PEI_HECI_WAIT_DELAY        50000     ///< 50ms timeout for IO delay
#define SPS_PEI_HECI_INIT_TIMEOUT      10000000  ///< 10 sec timeout in microseconds
#define SPS_PEI_HECI_READ_TIMEOUT      10000000  ///< 10sec timeout in microseconds
#define SPS_PEI_HECI_SEND_TIMEOUT      10000000  ///< 10sec timeout in microseconds
///
/// DXE Timeout values based on HPET
///
#define SPS_HECI_WAIT_DELAY            1000      ///< 1ms timeout for IO delay
#define SPS_HECI_INIT_TIMEOUT          15000000  ///< 15sec timeout in microseconds
#define SPS_HECI_READ_TIMEOUT          5000000   ///< 5sec timeout in microseconds
#define SPS_HECI_SEND_TIMEOUT          5000000   ///< 5sec timeout in microseconds
#define SPS_HECI_MAX_RETRY             3         ///< Value based off HECI HPS
#define SPS_HECI_MSG_DELAY             2000000   ///< show warning msg and stay for 2 seconds.
#define SPS_HECI_MBP_RDY_TIMEOUT       50000     ///< 50ms timeout
#define SPS_HECI_MBP_CLR_TIMEOUT       1000000   ///< 1s timeout
#define SPS_HECI_MBP_READ_MAX_RETRIES  2         ///< MBP read will be retried twice
#pragma pack(1)
///
/// REGISTER EQUATES
///

///
/// ME_CSR_HA - ME Control Status Host Access
///
typedef union {
  UINT32 ul;
  struct {
    UINT32 ME_IE_HRA   : 1; ///< 0 - ME Interrupt Enable (Host Read Access)
    UINT32 ME_IS_HRA   : 1; ///< 1 - ME Interrupt Status (Host Read Access)
    UINT32 ME_IG_HRA   : 1; ///< 2 - ME Interrupt Generate (Host Read Access)
    UINT32 ME_RDY_HRA  : 1; ///< 3 - ME Ready (Host Read Access)
    UINT32 ME_RST_HRA  : 1; ///< 4 - ME Reset (Host Read Access)
    UINT32 Reserved    : 3; ///< 7:5
    UINT32 ME_CBRP_HRA : 8; ///< 15:8 - ME CB Read Pointer (Host Read Access)
    UINT32 ME_CBWP_HRA : 8; ///< 23:16 - ME CB Write Pointer (Host Read Access)
    UINT32 ME_CBD_HRA  : 8; ///< 31:24 - ME Circular Buffer Depth (Host Read Access)
  } r;
} SPS_HECI_ME_CONTROL_REGISTER;

///
/// H_CSR - Host Control Status
///
typedef union {
  UINT32 ul;
  struct {
    UINT32 H_IE     : 1; ///< 0 - Host Interrupt Enable ME
    UINT32 H_IS     : 1; ///< 1 - Host Interrupt Status ME
    UINT32 H_IG     : 1; ///< 2 - Host Interrupt Generate
    UINT32 H_RDY    : 1; ///< 3 - Host Ready
    UINT32 H_RST    : 1; ///< 4 - Host Reset
    UINT32 Reserved : 3; ///< 7:5
    UINT32 H_CBRP   : 8; ///< 15:8 - Host CB Read Pointer
    UINT32 H_CBWP   : 8; ///< 23:16 - Host CB Write Pointer
    UINT32 H_CBD    : 8; ///< 31:24 - Host Circular Buffer Depth
  } r;
} SPS_HECI_HOST_CONTROL_REGISTER;

///
/// FWS
///
typedef union {
  UINT32 ul;
  struct {
    UINT32 CurrentState         : 4; ///< 0:3 - Current State
    UINT32 ManufacturingMode    : 1; ///< 4 Manufacturing Mode
    UINT32 FptBad               : 1; ///< 5 FPT(Flash Partition Table ) Bad
    UINT32 MeOperationState     : 3; ///< 6:8 - ME Operation State
    UINT32 FwInitComplete       : 1; ///< 9
    UINT32 FtBupLdFlr           : 1; ///< 10 - This bit is set when firmware is not able to load BRINGUP from the fault tolerant (FT) code.
    UINT32 FwUpdateInprogress   : 1; ///< 11
    UINT32 ErrorCode            : 4; ///< 12:15 - Error Code
    UINT32 MeOperationMode      : 4; ///< 16:19 - Management Engine Current Operation Mode
    UINT32 Reserved2            : 4; ///< 20:23
    UINT32 MeBootOptionsPresent : 1; ///< 24 - If this bit is set, an Boot Options is present
    UINT32 AckData              : 3; ///< 25:27 Ack Data
    UINT32 BiosMessageAck       : 4; ///< 28:31 BIOS Message Ack
  } r;
} SPS_HECI_FWS_REGISTER;

//
// Abstract ME Status definitions
//
#define SPS_ME_READY                   0x00
#define SPS_ME_INITIALIZING            0x01
#define SPS_ME_IN_RECOVERY_MODE        0x02
#define SPS_ME_DISABLE_WAIT            0x06
#define SPS_ME_TRANSITION              0x07
#define SPS_ME_NOT_READY               0x0F
#define SPS_ME_FW_INIT_COMPLETE        0x80
#define SPS_ME_FW_BOOT_OPTIONS_PRESENT 0x100
#define SPS_ME_FW_UPDATES_IN_PROGRESS  0x200

//
// Abstract ME Mode Definitions
//
#define SPS_ME_MODE_NORMAL              0x00
#define SPS_ME_MODE_DEBUG               0x02
#define SPS_ME_MODE_TEMP_DISABLED       0x03
#define SPS_ME_MODE_SECOVER             0x04
#define SPS_ME_MODE_FAILED              0x06


///
/// MISC_SHDW (44h) ME UMA in HECI-1 (B0:D22:F0) MISC_SHDW Register
///
#if 0 /// Not Used in SPS3EN
typedef union {
  UINT32  ul;
  struct {
    UINT32  MUSZ : 6;       ///< 0:5 - ME UMA Size
    UINT32  Reserved : 8;   ///< 6:13 - Reserved
    UINT32  Reserved2 : 2;  ///< 14:15 - Reserved
    UINT32  MUSZV : 1;      ///< 16:16 - ME UMA Size Valid
    UINT32  Reserved3 : 8;  ///< 17:24 - Reserved
    UINT32  Reserved4 : 6;  ///< 25:30 - Reserved
    UINT32  MSVLD : 1;      ///< 31:31 - Miscellaneous Shadow Valid
  } r;
} SPS_HECI_MISC_SHDW_REGISTER;
#endif

///
/// GS_SHDW (48h) ME Firmware Status #2 (MEFS2) in HECI-1 (B0:D22:F0) GS_SHDW Register
///
#if 0 /// Not Used in SPS3EN
typedef union {
  UINT32  ul;
  struct {
    UINT32  BistInProg : 1;       ///< 0 - BIST in progress
    UINT32  IccProgSts : 2;       ///< 1:2 - ICC Prog STS
    UINT32  InvokeMEBx : 1;       ///< 3 - Invoke MEBX
    UINT32  CpuReplacedSts : 1;   ///< 4 - CPU Replaced STS
    UINT32  MbpRdy : 1;           ///< 5 - MBP RDY
    UINT32  MfsFailure : 1;       ///< 6 - MFS Failure
    UINT32  WarmRstReqForDF : 1;  ///< 7 - Warm Reset Required for Dynamic Fusing
    UINT32  CpuReplacedValid : 1; ///< 8 - CPU Replaced Valid
    UINT32  Reserved : 2;         ///< 9:10 - Reserved
    UINT32  FwUpdIpu : 1;         ///< 11 - FW UPD IPU Needed
    UINT32  Reserved2 : 1;        ///< 12 - Reserved
    UINT32  MbpCleared : 1;       ///< 13 - MBP Cleared
    UINT32  Reserved3 : 2;        ///< 14:15 - Reserved
    UINT32  ExtStatCode1 : 8;     ///< 16:23 - EXT Status Code 1
    UINT32  ExtStatCode2 : 4;     ///< 24:27 - EXT Status Code 2
    UINT32  InfPhaseCode : 4;     ///< 31:28 - Infra. Phase code
  } r;
} SPS_HECI_GS_SHDW_REGISTER;
#endif

///
/// HECI_GS2
///
#if 0 /// Not Used in SPS3EN
typedef union {
  UINT32  ul;
  struct {
    UINT32  MbpGiveUp  : 1;       ///< 0 - MBP Give Up
    UINT32  Reserved   :31;       ///< 1-31 - Reserved
  } r;
} SPS_HECI_GS2_REGISTER;
#endif

///
/// ME Current State Values
///
#define SPS_ME_STATE_RESET             0x00
#define SPS_ME_STATE_INIT              0x01
#define SPS_ME_STATE_RECOVERY          0x02
#define SPS_ME_STATE_DISABLE           0x04
#define SPS_ME_STATE_NORMAL            0x05
#define SPS_ME_STATE_DISABLE_WAIT      0x06
#define SPS_ME_STATE_TRANSITION        0x07
#define SPS_ME_STATE_INVALID_CPU       0x08

///
/// DRAM Initiailization Response Codes.
///
#if 0 /// Not Used in SPS3EN
#define SPS_CBM_DIR_NON_PCR       0x01
#define SPS_CBM_DIR_PCR           0x02
#define SPS_CBM_DIR_GLOBAL_RESET  0x06
#define SPS_CBM_DIR_CONTINUE_POST 0x07
#endif
///
/// ME Firmware FwInitComplete
///
#define SPS_ME_FIRMWARE_COMPLETED      0x01
#define SPS_ME_FIRMWARE_INCOMPLETED    0x00

///
/// ME Boot Options Present
///
#define SPS_ME_BOOT_OPTIONS_PRESENT     0x01
#define SPS_ME_BOOT_OPTIONS_NOT_PRESENT 0x00

///
/// ME Operation State Values
///
#if 0 /// Not Used in SPS3EN
#define SPS_ME_OPERATION_STATE_PREBOOT  0x00
#define SPS_ME_OPERATION_STATE_M0_UMA   0x01
#define SPS_ME_OPERATION_STATE_M3       0x04
#define SPS_ME_OPERATION_STATE_M0       0x05
#define SPS_ME_OPERATION_STATE_BRINGUP  0x06
#define SPS_ME_OPERATION_STATE_M0_ERROR 0x07
#endif

///
/// ME Error Code Values
///
#define SPS_ME_ERROR_CODE_NO_ERROR      0x00
#define SPS_ME_ERROR_CODE_UNKNOWN       0x01
#define SPS_ME_ERROR_CODE_IMAGE_FAILURE 0x03
#define SPS_ME_ERROR_CODE_DEBUG_FAILURE 0x04

///
/// Management Engine Current Operation Mode
///
#define SPS_ME_OPERATION_MODE_NORMAL            0x00

#define SPS_ME_OPERATION_MODE_DEBUG             0x02
#define SPS_ME_OPERATION_MODE_SOFT_TEMP_DISABLE 0x03
#define SPS_ME_OPERATION_MODE_SECOVR_JMPR       0x04
#define SPS_ME_OPERATION_MODE_SECOVR_HECI_MSG   0x05
#define SPS_ME_OPERATION_MODE_SPS               0x0F

#pragma pack()

#endif
