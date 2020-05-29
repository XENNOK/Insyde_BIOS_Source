/** @file

  SPS definitions common for various SPS modules.

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

#ifndef _SPS_ME_H_
#define _SPS_ME_H_

#pragma pack(1)

//============================================================================
//  Server Platform Services(SPS) Firmware ME-BIOS Interface
//    Denlow   Revision 1.0.1(#503664)
//    Grantley Revision 1.0.1(#516145)
//============================================================================
//

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.3  HECI Status Registers 
// Grantley Revision 1.0.1(#516145) - 3.3  HECI Status Registers
//***************************************************************************//
//
// ME status (read-only for host, read-write for ME):
//  Host Firmware Status (HFS) located at offset 40h
//
#define SPS_HECI_REGISTER_HFS          0x40
#define SPS_R_MEFS1                    SPS_HECI_REGISTER_HFS ///< HECI-1 HFS is used as ME Firmware Status #1
#define SPS_R_NMFS                     SPS_HECI_REGISTER_HFS ///< HECI-2 HFS is used as NM Firmware Status
//
//  Miscellaneous Shadow (MISC_SHDW) at offset 44h 
//
#define SPS_HECI_REGISTER_MISC_SHDW    0x44

//
//  General Status Shadow (GS_SHDW) at offset 48h
//
#define SPS_HECI_REGISTER_GS_SHDW      0x48
#define SPS_R_MEFS2                    SPS_HECI_REGISTER_GS_SHDW ///< HECI-1 GS_SHDW is used as ME Firmware Status #2
//
//  General Status Shadow <n> (GS_SHDW<n>)
//  where n=2,3,4,5 at offsets 60h, 64h, 68h, and 6Ch respectively.
//
#define SPS_HECI_REGISTER_GS_SHDW2     0x60
#define SPS_HECI_REGISTER_GS_SHDW3     0x64
#define SPS_HECI_REGISTER_GS_SHDW4     0x68
#define SPS_HECI_REGISTER_GS_SHDW5     0x6C
#define SPS_R_GS_SHDW4                 SPS_HECI_REGISTER_GS_SHDW4 ///< HECI-1
//
// Host status (read-write for host, read-only for ME):
//  Host General Status (H_GS) at offset 4Ch
#define SPS_HECI_REGISTER_H_GS         0x4C
#define SPS_R_HOST_H_GS                SPS_HECI_REGISTER_H_GS ///< HECI-1 H_GS is used as Host Status
#define SPS_R_NM_H_GS                  SPS_HECI_REGISTER_H_GS ///< HECI-2 H_GS is used by Node Manager
//
//  Host General Status <n> (H_GS<n>)
//  where n=2,3 at offsets 70h and 74h respectively.
//
#define SPS_HECI_REGISTER_H_GS2        0x70
#define SPS_HECI_REGISTER_H_GS3        0x74

#define NON_BLOCKING                   0
#define BLOCKING                       1

//
// HECI Interrupt Delivery Mode  (HIDM).
//   These bits control what type of interrupt the HECI will
//   send when ARC writes to set the M_IG bit in AUX space.
//   00:  Generate Legacy or MSI interrupt
//   01:  Generate SCI
//   10:  Generate SMI
// 
#define SPS_HECI_REGISTER_HIDM         0xA0
#define SPS_R_HIDM                     SPS_HECI_REGISTER_HIDM

#define SPS_HECI_REGISTER_HECIMBAR     0x10
#define SPS_R_HECIMBAR                 SPS_HECI_REGISTER_HECIMBAR

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.3.2  ME firmware status registers 
// Grantley Revision 1.0.1(#516145) - 3.3.2  ME Firmware Status Registers
//***************************************************************************//
//
// ME Firmware Status #1 (MEFS1) in HECI-1 HFS Register
// The ME firmware writes status information about its current state in two 32-bit registers
// in the HECI-1(B0:D16:F0) PCI configuration space: HFS at offset 40h
//
typedef union {
  UINT32 UInt32;
  struct {
    UINT32 CurrentState      : 4;  ///< [3:0]   Current operation state of the firmware.
    UINT32 ManufacturingMode : 1;  ///< [4]     Set when Platform is in Manufacturing Mode.
    UINT32 FptBad            : 1;  ///< [5]     Flash Partition Table(FPT) or factory defaults is bad.
    UINT32 OperatingState    : 3;  ///< [8:6]   Current operating state of ME.
    UINT32 InitComplete      : 1;  ///< [9]     Set when firmware finished initialization
    UINT32 FtBupLdFlr        : 1;  ///< [10]    set when firmware is not able to load recovery bring-up from the flash.
    UINT32 UpdateInprogress  : 1;  ///< [11]    This bit is set if any type of ME firmware update is in progress.
    UINT32 ErrorCode         : 4;  ///< [15:12] If set to non zero value the ME firmware has encountered a fatal error and stopped normal operation.
    UINT32 OperatingMode     : 4;  ///< [19:16] Current operating mode of ME.
    UINT32 Reserved          : 5;  ///< [24:20] Reserved
    UINT32 MsgAckData        : 3;  ///< [27:25] Message specific data for acknowledged BIOS message.
    UINT32 MsgAck            : 4;  ///< [31:28] Acknowledge for register based BIOS message in HECI-1 H_GS Register.
  } Bits;
} SPS_MEFS1;

//
// SPS_MEFS1::CurrentState Values
// This field describes the current operation state of the firmware.
// The values are shared with client ME firmware.
//
#define SPS_ME_CURSTATE_RESET               0x0  ///< ME is in reset, will exit this state within 1 milisecond.
#define SPS_ME_CURSTATE_INIT                0x1  ///< ME is initializing, will exit this state within 2 seconds.
#define SPS_ME_CURSTATE_RECOVERY            0x2  ///< ME is in recovery mode, check other bits to determine cause.
#define SPS_ME_CURSTATE_DISABLED            0x4  ///< ME functionality has been disabled - not used with SPS.
#define SPS_ME_CURSTATE_NORMAL              0x5  ///< ME is in normal operational state.
#define SPS_ME_CURSTATE_DISABLE_WAIT        0x6  ///< Not used with SPS, Reserved.
#define SPS_ME_CURSTATE_TRANSITION          0x7  ///< ME is transitioning to a new Operating State.
#define SPS_ME_CURSTATE_NOT_READY           0x0F ///< ME is not ready.

#define SPS_ME_CURSTATE(a)                  (((a) & 0xf))
#define IS_SPS_ME_CURSTATE_RESET(a)         (((a) & SPS_ME_CURSTATE_RESET) == SPS_ME_CURSTATE_RESET)
#define IS_SPS_ME_CURSTATE_INIT(a)          (((a) & SPS_ME_CURSTATE_INIT) == SPS_ME_CURSTATE_INIT)
#define IS_SPS_ME_CURSTATE_RECOVERY(a)      (((a) & SPS_ME_CURSTATE_RECOVERY) == SPS_ME_CURSTATE_RECOVERY)
#define IS_SPS_ME_CURSTATE_DISABLED(a)      (((a) & SPS_ME_CURSTATE_DISABLED) == SPS_ME_CURSTATE_DISABLED)
#define IS_SPS_ME_CURSTATE_NORMAL(a)        (((a) & SPS_ME_CURSTATE_NORMAL) == SPS_ME_CURSTATE_NORMAL)
#define IS_SPS_ME_CURSTATE_DISABLE_WAIT(a)  (((a) & SPS_ME_CURSTATE_DISABLE_WAIT) == SPS_ME_CURSTATE_DISABLE_WAIT)
#define IS_SPS_ME_CURSTATE_TRANSITION(a)    (((a) & SPS_ME_CURSTATE_TRANSITION) == SPS_ME_CURSTATE_TRANSITION)
#define IS_SPS_ME_CURSTATE_NOT_READY(a)     (((a) & SPS_ME_CURSTATE_NOT_READY) == SPS_ME_CURSTATE_NOT_READY)

//
// SPS_MEFS1::ManufacturingMode Values
// This field describes the platform is still in manufacturing mode.
//
#define SPS_ME_MANUFACTURING_MODE           0x10  ///< ME in manufacturing mode.
#define IS_SPS_ME_MANUFACTURING_MODE(a)     (((a) & SPS_ME_MANUFACTURING_MODE) == SPS_ME_MANUFACTURING_MODE)

//
// SPS_MEFS1::OperatingState Values
// This field describes the current operating state of ME.
//
#define SPS_ME_OPSTATE_PREBOOT              0  ///< ME in pre-boot
#define SPS_ME_OPSTATE_M0_UMA               1  ///< ME runs using UMA - not used with SPS
#define SPS_ME_OPSTATE_M3                   4  ///< ME runs without UMA, host in Sx
#define SPS_ME_OPSTATE_M0                   5  ///< ME runs without UMA, host in S0 -normal state for SPS
#define SPS_ME_OPSTATE_BRINGUP              6  ///< ME in bringup
#define SPS_ME_OPSTATE_M0_ERROR             7  ///< M0 without UMA but with error

#define SPS_ME_OPSTATE(a, b)                ((((a) >> 6) & 0x07) == (b))
#define IS_SPS_ME_OPSTATE_PREBOOT(a)        SPS_ME_OPSTATE((a), (SPS_ME_OPSTATE_PREBOOT))
#define IS_SPS_ME_OPSTATE_M0_UMA(a)         SPS_ME_OPSTATE((a), (SPS_ME_OPSTATE_M0_UMA))
#define IS_SPS_ME_OPSTATE_M3(a)             SPS_ME_OPSTATE((a), (SPS_ME_OPSTATE_M3))
#define IS_SPS_ME_OPSTATE_M0(a)             SPS_ME_OPSTATE((a), (SPS_ME_OPSTATE_M0))
#define IS_SPS_ME_OPSTATE_BRINGUP(a)        SPS_ME_OPSTATE((a), (SPS_ME_OPSTATE_BRINGUP))
#define IS_SPS_ME_OPSTATE_M0_ERROR(a)       SPS_ME_OPSTATE((a), (SPS_ME_OPSTATE_M0_ERROR))

//
// SPS_MEFS1::InitComplete Values
// This field describes the current operating mode of ME.
//
#define SPS_ME_INIT_COMPLETEL               0x1 ///< SPS ME Firmware initializ complete
#define IS_SPS_ME_INIT_COMPLETEL(a)         ((((a) >> 9) & 0x01) == (SPS_ME_INIT_COMPLETEL))

//
// SPS_MEFS1::UpdateInprogress Values
// This field describes ME firmware update is in progress.
//
#define SPS_ME_UPDATE_IN_PROGRESS           0x1 ///< SPS ME firmware update is in progress.
#define IS_SPS_ME_UPDATE_IN_PROGRESS(a)     ((((a) >> 11) & 0x01) == (SPS_ME_UPDATE_IN_PROGRESS))

//
// SPS_MEFS1::ErrorCode Values
// If set to non zero value the ME firmware has encountered a fatal error and stopped normal operation.
//
#define SPS_ME_ERRCODE_NOERROR              0  ///< No error.
#define SPS_ME_ERRCODE_UNKNOWN              1  ///< The ME firmware has experienced an uncategorized error.
#define SPS_ME_ERRCODE_DISABLED             2  ///< Firmware was disabled on this platform.
#define SPS_ME_ERRCODE_IMAGE_FAIL           3  ///< The ME firmware stored in the system flash is not valid.

#define SPS_ME_ERRCODE(a, b)                ((((a) >> 12) & 0x0F) == (b))
#define IS_SPS_ME_ERROR_NOERROR(a)          SPS_ME_ERRCODE((a), (SPS_ME_ERRCODE_NOERROR))
#define IS_SPS_ME_ERROR_NOERROR(a)          SPS_ME_ERRCODE((a), (SPS_ME_ERRCODE_NOERROR))
#define IS_SPS_ME_ERROR_UNKNOW(a)           SPS_ME_ERRCODE((a), (SPS_ME_ERRCODE_UNKNOWN))
#define IS_SPS_ME_ERROR_DISABLED(a)         SPS_ME_ERRCODE((a), (SPS_ME_ERRCODE_DISABLED))
#define IS_SPS_ME_ERROR_IMAGE_FAIL(a)       SPS_ME_ERRCODE((a), (SPS_ME_ERRCODE_IMAGE_FAIL))

//
// SPS_MEFS1::OperatingMode Values
// This field describes the current operating mode of ME.
//
#define SPS_ME_OPMODE_NORMAL                0    ///< Client firmware is running in ME
#define SPS_ME_OPMODE_IGNITION              1    ///< Ignition firmware is running in ME
//#define SPS_ME_OPMODE_ALT_DISABLED          2    ///< ME is disabled
#define SPS_ME_OPMODE_DEBUG                 2    ///< Debug Mode - ME is disabled using PCHSTRP10 bit [7]
#define SPS_ME_OPMODE_TEMP_DISABLE          3    ///<
#define SPS_ME_OPMODE_SECOVR_JMPR           4    ///< Security Override activated with jumper
#define SPS_ME_OPMODE_SECOVR_MSG            5    ///< Security Override activated with HMRFPO_ENABLE request
#define SPS_ME_OPMODE_SPS                   15   ///< SPS firmware is running in ME
#define SPS_ME_OPMODE_NOT_SPS               0xff ///< Current firmware is not Server Platform Services ME.

#define SPS_ME_OPMODE(a, b)                 ((((a) >> 0) & 0x0F) == (b))
#define IS_SPS_ME_OPMODE_NORMAL(a)          SPS_ME_OPMODE((a), (SPS_ME_OPMODE_NORMAL))
#define IS_SPS_ME_OPMODE_IGNITION(a)        SPS_ME_OPMODE((a), (SPS_ME_OPMODE_IGNITION))
#define IS_SPS_ME_OPMODE_DEBUG(a)           SPS_ME_OPMODE((a), (SPS_ME_OPMODE_DEBUG))
#define IS_SPS_ME_OPMODE_TEMP_DISABLE(a)    SPS_ME_OPMODE((a), (SPS_ME_OPMODE_TEMP_DISABLE))
#define IS_SPS_ME_OPMODE_SECOVR_JMPR(a)     SPS_ME_OPMODE((a), (SPS_ME_OPMODE_SECOVR_JMPR))
#define IS_SPS_ME_OPMODE_SECOVR_MSG(a)      SPS_ME_OPMODE((a), (SPS_ME_OPMODE_SECOVR_MSG))
#define IS_SPS_ME_OPMODE_SPS(a)             SPS_ME_OPMODE((a), (SPS_ME_OPMODE_SPS))

//
// ME Firmware Status #2 (MEFS2) in HECI-1 GS_SHDW Register
// The ME firmware writes status information about its current state in two 32-bit registers
// in the HECI-1(B0:D16:F0) PCI configuration space: GS_SHDW at offset 48h.
//
typedef union {
  UINT32 UInt32;
  struct {
    UINT32 BistInProgress         : 1;  ///< [0]     If this bit is set ME Built-In Self Test is in progress.
    UINT32 RecoveryCause          : 3;  ///< [3:1]   If MEFS1.CurrentState says recovery here is reason.
    UINT32 Reserved0              : 2;  ///< [5:4]
    UINT32 MfsFailure             : 1;  ///< [6]     ME File System failure has been detected during recent ME boot.
    UINT32 WarmResetRequest       : 1;  ///< [7]     ME informs BIOS that a warm reset is requested by ME.
    UINT32 Reserved1              : 4;  ///< [11:8]
    UINT32 MeTargetImageBootFault : 1;  ///< [12]    ME target image boot failed, switched to backup image or recovery image.
    UINT32 FirmwareHeartbeat      : 3;  ///< [15:13] The number increments approximately every second if firmware is alive.(debug only).
    UINT32 ExtendedStatusData     : 12; ///< [27:16]
    UINT32 ProgressCode           : 3;  ///< [30:28] ME firmware progress status.
    UINT32 EopStatus              : 1;  ///< [31]    ME notion of EOP status.
  } Bits;
} SPS_MEFS2;

//
// SPS_MEFS2::RecoveryCause Values
// This bits indicate that ME firmware is running in recovery mode
// these values provide the cause of this mode.
//
#define SPS_RCAUSE_RCVJMPR  0  ///< ME recovery jumper asserted.
#define SPS_RCAUSE_MFGJMPR  1  ///< Security strap override jumper asserted.
#define SPS_RCAUSE_IPMICMD  2  ///< Recovery forced with IPMI command.
#define SPS_RCAUSE_FLASHCFG 3  ///< Invalid flash configuration (see ME-BIOS spec. for details).
#define SPS_RCAUSE_MEERROR  4  ///< ME internal error.

//
// SPS_MEFS2::ProgressCode Values
// This field identifies the infrastructure progress code.
//
#define SPS_PROGRESS_CODE_ROM     0  ///< ME is in ROM phase.
#define SPS_PROGRESS_CODE_BRINGUP 1  ///< ME is in BRINGUP phase.
#define SPS_PROGRESS_CODE_UKERNEL 2  ///< ME is in Micro Kernel phase.
#define SPS_PROGRESS_CODE_POLICY  3  ///< ME is in Policy Module phase.
#define SPS_PROGRESS_CODE_OTHER   4  ///< ME is loading modules in MO or M3 Operating State.

//
// NM Firmware Status (NMFS) in HECI-2 HFS Register
// NM Firmware Status (HECI-2 HFS) - SPS firmware writes the information to this register at startup
// and BIOS can read it easily before processor and memory initialization.
//
typedef union {
  UINT32 UInt32;
  struct {
    UINT32 BiosBootingMode : 1;  ///< [0]     NM boot time policy. [0] power-optimized [1] performance-optimized
    UINT32 CoresDisabled   : 7;  ///< [1:7]   Number of physical processor cores that should be disabled on each processor socket.
    UINT32 PowerLimiting   : 1;  ///< [8]     ME is actively limiting platform power consumption
    UINT32 UnderVoltage    : 1;  ///< [9]     Under-voltage event was noticed at least once since last ME reset.
    UINT32 OverCurrent     : 1;  ///< [10]    Over-current event was noticed at least once since last  ME reset.
    UINT32 OverTemperature : 1;  ///< [11]    Over-temperature event was noticed at least once since last ME reset.
    UINT32 Reserved        : 19; ///< [30:12] Reserved
    UINT32 NmEnabled       : 1;  ///< [31]    Node Manager power management enabled.
  } Bits;
} SPS_NMFS;

//
// SPS_NMFS::BiosBootingMode Values
// This bits indicate that NM firmware is request bios running in performance-optimized mode
//
#define SPS_NM_BOOTING_MODE_POWER      0  ///< BIOS should run in power-optimized mode.
#define SPS_NM_BOOTING_MODE_PERF       1  ///< BIOS should run in performance-optimized mode.

//
// SPS_NMFS::NmEnabled Values
// This bits indicate that NM firmware is enable.
//
#define SPS_NM_DISABLE                 0  ///< Node manager disabled.
#define SPS_NM_ENABLE                  1  ///< Node manager enabled.


//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.5.1  ME-BIOS interface version
// Grantley Revision 1.0.1(#516145) - 3.5.1  ME-BIOS Interface Version
//***************************************************************************//
//
// Get ME-BIOS Interface Version Message
//
#define SPS_CMD_GET_MEBIOS_INTERFACE_REQ    0x01
#define SPS_CMD_GET_MEBIOS_INTERFACE_RSP    (0x80 | SPS_CMD_GET_MEBIOS_INTERFACE_REQ)

//
// SPS_GET_MEBIOS_INTERFACE request message format
//
typedef struct _SPS_GET_ME_BIOS_INTERFACE_REQ {
  UINT8            Command;
} SPS_GET_ME_BIOS_INTERFACE_REQ;

//
// SPS_GET_MEBIOS_INTERFACE response message format
//
typedef union _SPS_ME_FEATURE_SET {
  UINT16 Data;
  struct {
    UINT16 NM        : 1;  ///< [0] Node Manager support
    UINT16 PeciProxy : 1;  ///< [1] PECI Proxy support
    UINT16 ICC       : 1;  ///< [2] ICC support
    UINT16 Reserved1 : 1;  ///< [3] Reserved
    UINT16 IDER      : 1;  ///< [4] IDER Enabled
    UINT16 SOL       : 1;  ///< [5] SOL Enabled
    UINT16 Network   : 1;  ///< [6] Network Enabled
    UINT16 KVM       : 1;  ///< [7] KVM Enabled
    UINT16 PMBus     : 1;  ///< [8] PMBus Proxy support
    UINT16 Reserved2 : 7;  ///< [9:15] Reserved
  } Fields;
} SPS_ME_FEATURE_SET;

typedef struct _SPS_GET_ME_BIOS_INTERFACE_RSP_DATA {
  UINT8                 Command;
  UINT8                 MajorVersion;  ///< Major number of the ME-BIOS interface version.
  UINT8                 MinorVersion;  ///< Minor number of the ME-BIOS interface version.
  SPS_ME_FEATURE_SET    BitMask;       ///< Bit coded ME FW feature set.
} SPS_GET_ME_BIOS_INTERFACE_RSP_DATA;

typedef struct _SPS_GET_ME_BIOS_INTERFACE_RSP {
  SPS_GET_ME_BIOS_INTERFACE_RSP_DATA   Data;
} SPS_GET_ME_BIOS_INTERFACE_RSP;

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.5.2  Vendor label 
// Grantley Revision 1.0.1(#516145) - 3.5.3  Vendor Label
//***************************************************************************//
//
// Get Vendor Label Message
//
#define SPS_CMD_GET_VENDOR_LABEL_REQ        0x02
#define SPS_CMD_GET_VENDOR_LABEL_RSP        (0x80 | SPS_CMD_GET_VENDOR_LABEL_REQ)

typedef struct _SPS_GET_VENDOR_LABEL_REQ {
  UINT8            Command;
} SPS_GET_VENDOR_LABEL_REQ;

typedef struct _SPS_GET_VENDOR_LABEL_RSQ {
  UINT8            Command;
  UINT8            Reserved[3];
  UINT32           VendorLabel;
  UINT8            Signature[32];
} SPS_GET_VENDOR_LABEL_RSQ;

//***************************************************************************//
/// Denlow   Revision 1.0.1(#503664) - 3.5.3  ME kernel host interface version
/// Grantley Revision 1.0.1(#516145) - 3.5.5  ME Kernel Host Interface Version
//***************************************************************************//
//
// MKHI Header format
//
typedef union  {
  UINT32 Data;
  struct {
    UINT32 GroupId    : 8;
    UINT32 Command    : 7;
    UINT32 IsResponse : 1;
    UINT32 Reserved   : 8;
    UINT32 Result     : 8;
  } Fields;
} SPS_MKHI_HEADER;

//
// Typedef for GroupID
//
typedef enum {
  SPS_MKHI_GRP_HMRFPO = 5,
  SPS_MKHI_GRP_DEBUG  = 8,
  SPS_MKHI_GRP_MAX,
  SPS_MKHI_GRP_GEN    = 0xFF
} SPS_MKHI_GROUP_ID;

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.5.6  Get firmware version message
// Grantley Revision 1.0.1(#516145) - 3.5.8  Get Firmware Version Message
//***************************************************************************//
//
// MKHI_GET_FW_VERSION request message format
//
#define SPS_MKHI_CMD_GET_FW_VERSION_REQ     0x02  // MKHI_CMD_GET_FW_VERSION
#define SPS_MKHI_CMD_GET_FW_VERSION_RSP     (0x80 | SPS_MKHI_CMD_GET_FW_VERSION_REQ)

typedef struct _SPS_GET_FW_VER_REQ {
  SPS_MKHI_HEADER  MkhiHeader;
} SPS_GET_FW_VER_REQ;

typedef struct _SPS_GET_FW_VER_RSP_DATA_VERSION {
  UINT16           MinorNumber;        ///< Version minor number
  UINT8            MajorNumber;        ///< Version major number
  UINT8            ServerSegmentCode;  ///< Version Server segment code
  UINT16           BuildNumber;        ///< Version build number
  UINT16           PatchNumber;        ///< Version patch number
} SPS_GET_FW_VER_RSP_DATA_VERSION;

typedef struct _SPS_GET_FW_VER_RSP_DATA {
  SPS_GET_FW_VER_RSP_DATA_VERSION  ActFw;
  SPS_GET_FW_VER_RSP_DATA_VERSION  RcvFw;
  SPS_GET_FW_VER_RSP_DATA_VERSION  BkpFw;
} SPS_GET_FW_VER_RSP_DATA;

typedef struct _SPS_GET_FW_VER_RSP {
  SPS_MKHI_HEADER            MkhiHeader;
  SPS_GET_FW_VER_RSP_DATA    Data;
} SPS_GET_FW_VER_RSP;

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.6  DRAM Initialization Done Message
// Grantley Revision 1.0.1(#516145) - 3.6  DRAM Initialization Done Message
//***************************************************************************//
//
// DRAM_INT_DONE message Format 
//
typedef union {
  UINT32 UInt32;
  struct {
    UINT32 MeUmaAddr       : 16; ///< [15:0]  ME UMA Address - not used in SPS.
    UINT32 Reserved        : 8;  ///< [23:16] Reserved
    UINT32 Status          : 4;  ///< [27:24] Status. [0] Success [1] No memory in channels [2] Memory initialization error
    UINT32 Command         : 4;  ///< [31:28] DRAM_INIT_DONE command code.
  } Bits;
} SPS_DRAM_INIT_DONE_MESSAGE;

//
// SPS_DRAM_INIT_DONE_MESSAGE::Status Values
//
#define SPS_DRAM_INIT_DONE_STS_SUCCESS      0
#define SPS_DRAM_INIT_DONE_STS_NO_MEMORY    1
#define SPS_DRAM_INIT_DONE_STS_INIT_ERROR   2

#define SPS_DRAM_INIT_DONE_COMMAND          (1)

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.7  EOP indication requirement
// Grantley Revision 1.0.1(#516145) - 3.7  EOP Indication Requirement
//***************************************************************************//
//
// END_OF_POST request message format
//
#define SPS_MKHI_CMD_END_OF_POST_REQ   0x0C  // MKHI_CMD_END_OF_POST
#define SPS_MKHI_CMD_END_OF_POST_RSP   (0x80 | SPS_MKHI_CMD_END_OF_POST_REQ)
typedef struct _SPS_END_OF_POST_REQ {
  SPS_MKHI_HEADER  MkhiHeader;
} SPS_END_OF_POST_REQ;

//
// END_OF_POST response message format
//
typedef struct _SPS_END_OF_POST_RSP {
  SPS_MKHI_HEADER  MkhiHeader;
  UINT32           Action;
} SPS_END_OF_POST_RSP;

//
// SPS_END_OF_POST_RSP::Action Values
//
#define SPS_END_OF_POST_NO_ACTION      0x00000000  ///< No action, continue boot.
#define SPS_END_OF_POST_GLOBAL_RESET   0x00000001  ///< Global reset required.

typedef union _SPS_END_OF_POST_MESSAGE {
  SPS_END_OF_POST_REQ   Req;
  SPS_END_OF_POST_RSP   Rsp;
} SPS_END_OF_POST_MESSAGE;

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.10.1  Host ME Region Flash Protection Override Messages
// Grantley Revision 1.0.1(#516145) - 3.12.1  Host ME Region Flash Protection Override Messages
//***************************************************************************//
//
// HMRFPO Command definitions
//
#define SPS_HMRFPO_CMD_ENABLE          0x1  ///< HMRFPO ENABLE Message Command
#define SPS_HMRFPO_CMD_LOCK            0x2  ///< HMRFPO Lock Message Command
#define SPS_HMRFPO_CMD_GET_STATUS      0x3  ///< HMRFPO Get Status Message Command

//
// HMRFPO_LOCK request message format
//
typedef struct _SPS_HMRFPO_LOCK_REQ {
  SPS_MKHI_HEADER  MkhiHeader;
} SPS_HMRFPO_LOCK_REQ;

//
// HMRFPO_LOCK response message format 
//
typedef struct _SPS_HMRFPO_LOCK_RSP {
  SPS_MKHI_HEADER  MkhiHeader;
  UINT64           Nonce;
  UINT32           FactoryDefaultBase;
  UINT32           FactoryDefaultLimit;
  UINT8            Status;
  UINT8            Reserved[3];
} SPS_HMRFPO_LOCK_RSP;

//
// SPS_HMRFPO_LOCK_RSP::Status Values
//
#define SPS_HMRFPO_LOCK_SUCCESS        0x00  ///< HMRFPO Lock success
#define SPS_HMRFPO_LOCK_FAIL           0x01  ///< HMRFPO Lock Fail
#define SPS_HMRFPO_LOCK_NOT_VALID      0x02  ///< HMRFPO Lock Verification fail

typedef union _SPS_HMRFPO_LOCK {
  SPS_HMRFPO_LOCK_REQ   Req;
  SPS_HMRFPO_LOCK_RSP   Rsp;
} SPS_HMRFPO_LOCK;

//
// HMRFPO_MERESET request message format
//
typedef struct _SPS_HMRFPO_ENABLE_REQ {
  SPS_MKHI_HEADER  MkhiHeader;
  UINT64           Nonce;
} SPS_HMRFPO_ENABLE_REQ;

//
// HMRFPO_MERESET response message format
//
typedef struct _SPS_HMRFPO_ENABLE_RSP {
  SPS_MKHI_HEADER  MkhiHeader;
  UINT32           FactoryDefaultBase;
  UINT32           FactoryDefaultLimit;
  UINT8            Status;
  UINT8            Rsvd[3];
} SPS_HMRFPO_ENABLE_RSP;

//
// SPS_HMRFPO_ENABLE_RSP::Status Values
//
#define SPS_HMRFPO_ENABLE_SUCCESS      0x00
#define SPS_HMRFPO_ENABLE_FAILURE      0x03

typedef union _SPS_HMRFPO_ENABLE {
  SPS_HMRFPO_ENABLE_REQ Req;
  SPS_HMRFPO_ENABLE_RSP Rsp;
} SPS_HMRFPO_ENABLE;

//
// HMRFPO_GET_STATUS request message format
//
typedef struct _SPS_HMRFPO_GET_STATUS_REQ {
  SPS_MKHI_HEADER  MkhiHeader;
} SPS_HMRFPO_GET_STATUS_REQ;

//
// HMRFPO_GET_STATUS response message format
//
typedef struct _SPS_HMRFPO_GET_STATUS_RSP {
  SPS_MKHI_HEADER  MkhiHeader;
  UINT8            Status;
  UINT8            Reserved[3];
} SPS_HMRFPO_GET_STATUS_RSP;

//
// SPS_HMRFPO_GET_STATUS_RSP::Status Values
//
#define SPS_HMRFPO_STS_DISABLED        0x00
#define SPS_HMRFPO_STS_LOCKED          0x01
#define SPS_HMRFPO_STS_ENABLED         0x02

typedef union _SPS_HMRFPO_GET_STATUS {
  SPS_HMRFPO_GET_STATUS_REQ  Req;
  SPS_HMRFPO_GET_STATUS_RSP  Rsp;
} SPS_HMRFPO_GET_STATUS;

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.11  Integrated clock control
// Grantley Revision 1.0.1(#516145) - 3.13  Integrated Clock Control
//***************************************************************************//
//
// ICC header definition
//
typedef struct {
  UINT32           ApiVersion;   ///< API version
  UINT32           IccCommand;   ///< ICC Command
  UINT32           IccStatus;    ///< ICC Status
  UINT32           BufferLength; ///< Buffer Length
  UINT32           Reserved;
} SPS_ICC_HEADER;

//
// SPS_ICC_HEADER::ApiVersion Values
// API version for Lynx Point platforms is 3.0
// API version for Wellsburg platforms is 3.1. 
//
#define SPS_ICC_API_VERSION            0x00030000

//
// SPS_ICC_HEADER::IccCommand Values
// Available ICC commands.
//
typedef enum {
  SPS_SET_CLOCK_ENABLES          = 0x3, ///< Disable or enable clocks
  SPS_SET_SPREAD_SPECTRUM_CONFIG = 0xC, ///< Enable/Disable spread spectrum generators
  SPS_SECURE_SET_CLOCK_ENABLES   = 0xD, ///< It is equivalent to Set Clock Enables but can also be used after end of POST
  SPS_READ_MPHY_SETTINGS         = 0xE, ///<
  SPS_WRITE_MPHY_SETTINGS        = 0xF
} SPS_ICC_COMMAND_CODE;

//
// SPS_ICC_HEADER::IccStatus Values
// Available status codes for response messages.
//
typedef enum {
  SPS_ICC_STATUS_SUCCESS                              = 0x00,
  SPS_ICC_STATUS_FAILURE                              = 0x01,
  SPS_ICC_STATUS_INCORRECT_API_VERSION                = 0x02,
  SPS_ICC_STATUS_INVALID_FUNCTION                     = 0x03,
  SPS_ICC_STATUS_INVALID_BUFFER_LENGTH                = 0x04,
  SPS_ICC_STATUS_INVALID_PARAMETERS                   = 0x05,
  SPS_ICC_STATUS_CMD_NOT_SUPPORTED_AFTER_END_OF_POST  = 0x0B,
  SPS_ICC_STATUS_FUNCTION_NOT_SUPPORTED_OVER_SMBUS    = 0x16,
  SPS_ICC_STATUS_OCKEN_MASK_VIOLATION_WELLSBURG       = 0x1A,
  SPS_ICC_STATUS_OCKEN_MASK_VIOLATION_LYNXPOINT       = 0x20,
  SPS_ICC_STATUS_AUTHENTICATION_ERROR                 = 0x1B,
  SPS_ICC_STATUS_SSC_MODE_CHANGE_NOT_SUPPORTED        = 0x24,
  SPS_ICC_STATUS_SURVIVABILITY_TABLE_ACCESS_VIOLATION = 0x101D,
  SPS_ICC_STATUS_SURVIVABILITY_TABLE_TOO_LARGE        = 0x101E
} SPS_ICC_STATUS_CODE;

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.11.1  Set clock enables message
// Grantley Revision 1.0.1(#516145) - 3.13.1  Set Clock Enables Message
//***************************************************************************//
//
// ICC_SET_CLOCK_ENABLES request message format
//
typedef struct {
  SPS_ICC_HEADER   Header;
  UINT32           ClockEnables;     ///< Specifies the values for clock enables bits
  UINT32           ClockEnablesMask; ///< Specifies the values for clock enables mask bits
  UINT32           Params;           ///< Describes how Clock Enables settings should be processed by the ME
} SPS_ICC_SET_CLOCK_ENABLES_REQ;

//
// ICC_SET_CLOCK_ENABLES response message format
//
typedef struct {
  SPS_ICC_HEADER   Header;
} SPS_ICC_SET_CLOCK_ENABLES_RSP;

typedef union {
  SPS_ICC_SET_CLOCK_ENABLES_REQ   Req;
  SPS_ICC_SET_CLOCK_ENABLES_RSP   Rsp;
} SPS_ICC_SET_CLOCK_ENABLES;

//
//
// SPS_ICC_SET_CLOCK_ENABLES_REQ::Params Values
// Response Mode
//   0 - Response is expected
//       If BIOS sets this bit to 0 then it must wait for response.
//   1 - No response is expected
//       In this mode, Intel(R) ME would not respond to the command and
//       BIOS would not wait for a response, and cannot know if the command failed.
//       BIOS should use this mode in production platforms
//       only after it was tested with full response for this command.
//
#define SPS_ICC_RESPONSE_MODE_WAIT    0
#define SPS_ICC_RESPONSE_MODE_SKIP    1

//
// SPS_ICC_SET_CLOCK_ENABLES_REQ::ClockEnables Values
//   Specifies the values for clock enables bits (enabled or disabled) for
//   those clock outputs that should change Formatted according to ICC
//   OCKEN register.
//
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_FLEX0     BIT0
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_FLEX1     BIT1
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_FLEX2     BIT2
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_FLEX3     BIT3
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_Reserved1 (BIT4 | BIT5 | BIT6)
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_PCICLK0   BIT7
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_PCICLK1   BIT8
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_PCICLK2   BIT9
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_PCICLK3   BIT10
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_PCICLK4   BIT11
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_Reserved2 (BIT12 | BIT13| BIT14| BIT15)
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_SRC0      BIT16
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_SRC1      BIT17
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_SRC2      BIT18
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_SRC3      BIT19
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_SRC4      BIT20
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_SRC5      BIT21
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_SRC6      BIT22
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_SRC7      BIT23
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_ITPXDP    BIT24
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_Reserved3 BIT25
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_PEG_A     BIT26
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_PEG_B     BIT27
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_DMI       BIT28
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_DMI2      BIT29  ///< For WellsBurg
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_DP        BIT29  ///< For LynxPoint
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_PEG_C     BIT30  ///< For WellsBurg
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_DPNS      BIT30  ///< For LynxPoint
#define SPS_ICC_SET_CLOCK_ENABLES_BITS_Modulator4Enable BIT31

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.11.1  Set clock enables message
// Grantley Revision 1.0.1(#516145) - 3.13.1  Set Clock Enables Message
//***************************************************************************//
typedef union _SPS_ICC_SET_CLOCK_ENABLES_BITS {
  UINT32 UInt32;
  struct {
    UINT32 FLEX0     :1;
    UINT32 FLEX1     :1;
    UINT32 FLEX2     :1;
    UINT32 FLEX3     :1;
    UINT32 Reserved1 :3;
    UINT32 PCICLK0   :1;
    UINT32 PCICLK1   :1;
    UINT32 PCICLK2   :1;
    UINT32 PCICLK3   :1;
    UINT32 PCICLK4   :1;
    UINT32 Reserved2 :4;
    UINT32 SRC0      :1;
    UINT32 SRC1      :1;
    UINT32 SRC2      :1;
    UINT32 SRC3      :1;
    UINT32 SRC4      :1;
    UINT32 SRC5      :1;
    UINT32 SRC6      :1;
    UINT32 SRC7      :1;
    UINT32 ITPXDP    :1;
    UINT32 Reserved3 :1;
    UINT32 PEG_A     :1;
    UINT32 PEG_B     :1;
    UINT32 DMI       :1;
    UINT32 DP        :1;
    UINT32 DPNS      :1;
    UINT32 Modulator4Enable :1;
  } LynxPoint;
  struct {
    UINT32 FLEX0     :1;
    UINT32 FLEX1     :1;
    UINT32 FLEX2     :1;
    UINT32 FLEX3     :1;
    UINT32 Reserved1 :3;
    UINT32 PCICLK0   :1;
    UINT32 PCICLK1   :1;
    UINT32 PCICLK2   :1;
    UINT32 PCICLK3   :1;
    UINT32 PCICLK4   :1;
    UINT32 Reserved2 :4;
    UINT32 SRC0      :1;
    UINT32 SRC1      :1;
    UINT32 SRC2      :1;
    UINT32 SRC3      :1;
    UINT32 SRC4      :1;
    UINT32 SRC5      :1;
    UINT32 SRC6      :1;
    UINT32 SRC7      :1;
    UINT32 ITPXDP    :1;
    UINT32 Reserved3 :1;
    UINT32 PEG_A     :1;
    UINT32 PEG_B     :1;
    UINT32 DMI       :1;
    UINT32 DMI2      :1;
    UINT32 PEG_C     :1;
    UINT32 Modulator4Enable :1;
  } WellsBurg;
} SPS_ICC_SET_CLOCK_ENABLES_BITS;

typedef SPS_ICC_SET_CLOCK_ENABLES_BITS SPS_ICC_SET_CLOCK_ENABLES_MASK;

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.11.2  Set spread spectrum configuration message
// Grantley Revision 1.0.1(#516145) - 3.13.2  Set Spread Spectrum Configuration Message
//***************************************************************************//
//
// ICC_SET_SPREAD_SPECTRUM request message format
//
typedef struct {
  SPS_ICC_HEADER   Header;
  UINT32           IccSSMSelect;
  UINT32           Params;
} SPS_ICC_SET_SPREAD_SPECTRUM_REQ;

typedef union _SPS_ICC_SPREAD_SPECTRUM_SELECT {
  UINT32 UInt32;
  struct {
    UINT32 SSC1ModeSelect : 4;
    UINT32 SSC2ModeSelect : 4;
    UINT32 SSC3ModeSelect : 4;
    UINT32 SSC4ModeSelect : 4;
    UINT32 SSC5ModeSelect : 4;
    UINT32 SSC6ModeSelect : 4;
    UINT32 SSC7ModeSelect : 4;
    UINT32 SSC8ModeSelect : 4;
  } Field;
} SPS_ICC_SPREAD_SPECTRUM_SELECT;

//
// SPS_ICC_SET_SPREAD_SPECTRUM_REQ::IccSSMSelect Values
// Selects spread spectrum mode for 8 SSC outputs.
//   0000b - Down Spread
//   0001b - Reserved
//   0010b - Reserved
//   0011b - Disable - Power off SSC# and select bypass path to SSC# output
//   0100b - Do Not Change - SSC# settings from Intel/OEM Record will be applied
//
#define SPS_ICC_SSC_MODE_DOWN_SPREAD   0x0
#define SPS_ICC_SSC_MODE_DISABLE       0x3
#define SPS_ICC_SSC_MODE_NOT_CHANGE    0x4

//
// ICC_SET_SPREAD_SPECTRUM response message format
//
typedef struct {
  SPS_ICC_HEADER   Header;
} SPS_ICC_SET_SPREAD_SPECTRUM_RSP;

typedef union {
  SPS_ICC_SET_SPREAD_SPECTRUM_REQ Req;
  SPS_ICC_SET_SPREAD_SPECTRUM_RSP Rsp;
} SPS_ICC_SET_SPREAD_SPECTRUM;

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - Not found.
// Grantley Revision 1.0.1(#516145) - 3.13.2  Set Spread Spectrum Configuration Message
//***************************************************************************//
//
// ICC_SECURE_SET_CLOCK_ENABLES request message format
//
typedef struct {
  SPS_ICC_HEADER  Header;
  UINT64          Nonce;            ///< Nonce retrieved using HMRFPO_LOCK command
  UINT32          ClockEnables;     ///< Specifies the values for clock enables bits
  UINT32          ClockEnablesMask; ///< Specifies the values for clock enables mask bits
  UINT32          Params;           ///< Describes how Clock Enables settings should be processed by the ME
} SPS_ICC_SECURE_SET_CLOCK_ENABLES_REQ;

//
// ICC_ SECURE_SET_CLOCK_ENABLES response message format.
//
typedef struct {
  SPS_ICC_HEADER  Header;
} SPS_ICC_SECURE_SET_CLOCK_ENABLES_RSP;

typedef union {
  SPS_ICC_SECURE_SET_CLOCK_ENABLES_REQ Req;
  SPS_ICC_SECURE_SET_CLOCK_ENABLES_RSP Rsp;
} SPS_ICC_SECURE_SET_CLOCK_ENABLES_ENABLES;

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.12.2  BIOS to ME mPhy pre-DID commands
// Grantley Revision 1.0.1(#516145) - 3.14.2  BIOS to ME mPhy pre-DID commands
//***************************************************************************//
//
// BIOS to ME mPhy version request (pre-DID)
//
typedef union {
  UINT32 UInt32;
  struct {
    UINT32 Data     : 3;  ///< [3:0]   Defines the mPhy command request being made by BIOS. [0] Close Interface [1] Report mPhy version
    UINT32 Reserved : 25; ///< [27:3]  Reserved
    UINT32 Command  : 4;  ///< [31:28] mPhy version command code.
  } Bits;
} SPS_MPHY_PRE_DID_MESSAGE;

//
// SPS_MPHY_PRE_DID_MESSAGE::IccSSMSelect Values
// Defines the mPhy command request being made by BIOS.
//   0 - No Command / Close Interface
//       This command should be sent once after any other command is specified.
//       This closes the interface and releases any resources used on the ME side.
//   1 - Report mPhy version
//       This command causes the FW to report the first 4 bytes of the BIOS to ME
//       Survivability Table in the HECI-1 GS_SHDW4 register.
//       Value should be populated after command acknowledge is received.  
//
#define SPS_MPHY_PRE_DID_CLOSE_INTERFACE 0
#define SPS_MPHY_PRE_DID_REPORT_VERSION  1

//
// SPS_MPHY_PRE_DID_MESSAGE::Command Values
// A value of 7 indicating the mPhy version command.
//
#define SPS_MPHY_PRE_DID_COMMAND       (7)


//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.12.3  Read mPhy settings message
// Grantley Revision 1.0.1(#516145) - 3.14.3  Read mPhy settings message
//***************************************************************************//
//
// READ_MPHY_SETTINGS request message format
//
typedef struct {
  SPS_ICC_HEADER   Header;
  UINT32           Flags;  ///< Reserved for future use.
} SPS_ICC_READ_MPHY_SETTINGS_REQ;

//
// READ_MPHY_SETTINGS response message format
//
typedef struct {
  SPS_ICC_HEADER   Header;
  UINT8            TableData[256]; ///< The survivability table.(Varies size)
} SPS_ICC_READ_MPHY_SETTINGS_RSP;

typedef union {
  SPS_ICC_READ_MPHY_SETTINGS_REQ Req;
  SPS_ICC_READ_MPHY_SETTINGS_RSP Rsp;
} SPS_ICC_READ_MPHY_SETTINGS;


//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.12.4  Write mPhy settings message
// Grantley Revision 1.0.1(#516145) - 3.14.4  Write mPhy settings message
//***************************************************************************//
//
// WRITE_MPHY_SETTINGS request message format
//
typedef struct {
  SPS_ICC_HEADER   Header;
  UINT32           Flags;           ///< Reserved for future use.
  UINT8            TableData[512];  ///< The survivability table.(Varies size)
} SPS_ICC_WRITE_MPHY_SETTINGS_REQ;

//
// WRITE_MPHY_SETTINGS response message format
//
typedef struct {
  SPS_ICC_HEADER   Header;
} SPS_ICC_WRITE_MPHY_SETTINGS_RSP;

typedef union {
  SPS_ICC_WRITE_MPHY_SETTINGS_REQ Req;
  SPS_ICC_WRITE_MPHY_SETTINGS_RSP Rsp;
} SPS_ICC_WRITE_MPHY_SETTINGS;

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 3.13.1  Dynamic fusing procedure
// Grantley Revision 1.0.1(#516145) - Not found.
//***************************************************************************//
//
// CPU dynamic fusing request message format
//
typedef struct _SPS_DYNAMIC_FUSING_REQ {
  SPS_MKHI_HEADER  MkhiHeader;
} SPS_DYNAMIC_FUSING_REQ;

//
// CPU dynamic fusing response message format
//
typedef struct _SPS_DYNAMIC_FUSING_RSP {
  SPS_MKHI_HEADER  MkhiHeader;
  UINT8            Result;
} SPS_DYNAMIC_FUSING_RSP;

//
// SPS_DYNAMIC_FUSING_RSP::Result Values
//
#define CPU_DYNAMIC_FUSING_SUCCESS                    0  ///< Success continue POST
#define CPU_DYNAMIC_FUSING_GRST_REQUIRED              1  ///< BIOS must perform global platform reset.
#define CPU_DYNAMIC_FUSING_EOP_RECEIVED               2  ///< Msg not accpeted becaues END_OF_POST Msg already received
#define CPU_DYNAMIC_FUSING_ROL_NOT_SUPPORTED_BY_PCH   3  ///< Returned only on Romley platform
#define CPU_DYNAMIC_FUSING_ROL_SOFT_DISABLED          4  ///< Dynamic Fusing procedure not done because OEM disabled this capability on this platform by using softstraps.

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 4.2  Host configuration information
// Grantley Revision 1.0.1(#516145) - 4.2  Host Configuration Information
//***************************************************************************//
//
// NM Host Configuration message format
//
#define SPS_NM_CMD_PROCCFG             0x0
typedef struct _SPS_NM_HOST_CONFIGURATION_DATA {
  UINT16 Capabilities;       ///< Bit coded platform capabilities
  UINT8  PStatesNumber;      ///< Number of supported P-states.
  UINT8  TStatesNumber;      ///< Number of supported T-states.
  UINT16 MaxPower;           ///< Maximum power in watts that can be consumed by the platform, zero if unknown.
  UINT16 MinPower;           ///< Minimum power in watts that can be consumed by the platform when idle, zero if unknown.
  UINT8  ProcNumber;         ///< The number of processors (packages)installed in the system during platform boot.
  UINT8  ProcCoresNumber;    ///< Number of all processor cores in all installed processor packages.
  UINT8  ProcCoresEnabled;   ///< Number of all currently enabled processor cores in all installed processor packages.
  UINT8  ProcThreadsEnabled; ///< Number of all currently enabled hardware threads, logical processors, in all installed processor packages.
  UINT64 TurboRatioLimit;    ///< Turbo Ratio Limit MSR, MSR 1ADh for BSP processor.
  UINT64 PlatformInfo;       ///< Platform Info MSR, MSR CEh for BSP processor.
  UINT32 Altitude;           ///< The altitude of the platform location above the see level, expressed in meters.
  UINT8  PStatesRatio[16];   ///< Each byte contains a ratio for a P-state with the corresponding index, starting from P0.
} SPS_NM_HOST_CONFIGURATION_DATA;

//
// SPS_NM_HOST_CONFIGURATION_DATA::Capabilities Values
// Bit coded platform capabilities
//
#define SPS_NM_HOST_CAP_TURBO               BIT0
#define SPS_NM_HOST_CAP_SMI_OPTIMIZED       BIT1
#define SPS_NM_HOST_CAP_POWER_MEASUREMENT   BIT2
#define SPS_NM_HOST_CAP_HADRWARE_CHANGE     BIT3

typedef struct {
  SPS_MKHI_HEADER                 MkhiHeader; ///< [0:4]  MKHI Header
  SPS_NM_HOST_CONFIGURATION_DATA  Config;     ///< [5:52] Host Configuration
} SPS_NM_HOST_CONFIGURATION_MESSAGE;


//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 4.3.1  Performance change message
// Grantley Revision 1.0.1(#516145) - 4.3.1  Performance Change Message
//***************************************************************************//
//
// Command summary for the power limiting runtime operation
//
#define NM_CMD_MAXPTSTATE_REQ               0x00

#define NM_CMD_MAXPTSTATE_NACK              0x00
#define NM_CMD_MAXTSTATE_ACK                0x40
#define NM_CMD_MAXPSTATE_ACK                0x80
#define NM_CMD_MAXPTSTATE_ACK               0xC0

//
// Performance change request message format 
//

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 5.3.1  ME firmware HECI clients
// Grantley Revision 1.0.1(#516145) - 6.3.1  ME Firmware HECI Clients
//***************************************************************************//
#define SPS_HECI_CLIENT_HOST           0x00  ///< HECI Host ID. HECI discovery and HECI dynamic connections support
#define SPS_HECI_CLIENT_CORE           0x07  ///< Core BIOS messages like EOP message
#define SPS_HECI_CLIENT_ICC            0x08  ///< Integrated Clock Control messages
#define SPS_HECI_CLIENT_NM             0x11  ///< Node Manager messages
#define SPS_HECI_CLIENT_SPS            0x20  ///< SPS firmware basic messages
#define SPS_HECI_CLIENT_MCTP           0x21  ///< Management Component Transport Protocol proxy

//***************************************************************************//
// Denlow   Revision 1.0.1(#503664) - 5.5  HECI transaction layer
// Grantley Revision 1.0.1(#516145) - 6.5  HECI Transaction Layer
//***************************************************************************//
//
// HECI Message Header
//
typedef union _SPS_HECI_MESSAGE_HEADER {
  UINT32  Data;
  struct {
    UINT32  MeAddress       : 8; ///< This is the logical address of the ME client of the message.
    UINT32  HostAddress     : 8; ///< This is the logical address of the Host client of the message
    UINT32  Length          : 9; ///< This is the message length in bytes not including the SPS_HECI_MESSAGE_HEADER.
    UINT32  Reserved        : 6; ///< These  bits  are  reserved  for  future  use.
    UINT32  MessageComplete : 1; ///< This bit is used to indicate that this is the last message of a multi message HECI transfer of a client's message that is larger that the HECI circular buffer.
  } Fields;
} SPS_HECI_MESSAGE_HEADER;


#pragma pack()

#endif
