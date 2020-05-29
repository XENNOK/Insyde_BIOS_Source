/** @file

  Core BIOS Messages

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef CORE_BIOS_MSG_H
#define CORE_BIOS_MSG_H

#include "MkhiMsgs.h"

#pragma pack(1)

#define CBM_RESET_REQ_CMD         0x0B
#define CBM_RESET_REQ_CMD_ACK     0x8B
#define CBM_END_OF_POST_CMD       0x0C
#define CBM_END_OF_POST_CMD_ACK   0x8C
#define GEN_SET_DEBUG_MEMORY_CMD  0x11

//
// HECI Client Address - Core Messages
//    Core messages to coordinate memory initialization and UMA allocation with ME
//    as well as to inform ME of the end of POST event
//
#define HECI_CLIENT_CORE_MSG_DISPATCHER 0x07
#define HOST_FIXED_ADDRESS              0x00

typedef union _HECI_MESSAGE_HEADER {
  UINT32  Data;
  struct {
    UINT32  MeAddress : 8;
    UINT32  HostAddress : 8;
    UINT32  Length : 9;
    UINT32  Reserved : 6;
    UINT32  MessageComplete : 1;
  } Fields;
} HECI_MESSAGE_HEADER;

///
/// Reset request message
///
typedef struct _CBM_RESET_REQ_DATA {
  UINT8 RequestOrigin;
  UINT8 ResetType;
} CBM_RESET_REQ_DATA;

typedef struct _CBM_RESET_REQ {
  MKHI_MESSAGE_HEADER MKHIHeader;
  CBM_RESET_REQ_DATA  Data;
} CBM_RESET_REQ;

typedef struct _MKHI_CBM_RESET_REQ {
  HECI_MESSAGE_HEADER Header;
  CBM_RESET_REQ       Msg;
} MKHI_CBM_RESET_REQ;

///
/// Reset request ack message
///
typedef struct _CBM_RESET_ACK {
  MKHI_MESSAGE_HEADER MKHIHeader;
} CBM_RESET_ACK;

typedef struct _MKHI_CBM_RESET_ACK {
  HECI_MESSAGE_HEADER Header;
  CBM_RESET_ACK       Msg;
} MKHI_CBM_RESET_ACK;

//
// ASF Watch Dog Timer
//
#define ASF_MANAGEMENT_CONTROL    0x02
#define ASF_SUB_COMMAND_START_WDT 0x13
#define ASF_SUB_COMMAND_STOP_WDT  0x14

///
/// ASF Start Watch Dog
///
typedef struct _ASF_START_WDT {
  UINT8 Command;
  UINT8 ByteCount;
  UINT8 SubCommand;
  UINT8 VersionNumber;
  UINT8 TimeoutLow;
  UINT8 TimeoutHigh;
  UINT8 EventSensorType;
  UINT8 EventType;
  UINT8 EventOffset;
  UINT8 EventSourceType;
  UINT8 EventSeverity;
  UINT8 SensorDevice;
  UINT8 SensorNumber;
  UINT8 Entity;
  UINT8 EntityInstance;
  UINT8 EventData[5];
} ASF_START_WDT;

#define ASF_START_WDT_LENGTH  0x14

///
/// ASF Stop Watch Dog
///
typedef struct _ASF_STOP_WDT {
  UINT8 Command;
  UINT8 ByteCount;
  UINT8 SubCommand;
  UINT8 VersionNumber;
} ASF_STOP_WDT;

#define ASF_STOP_WDT_LENGTH 0x04

//
// HECI Header Definitions for Core BIOS Messages
//
#define CBM_END_OF_POST_HECI_HDR          0x80080001
#define CBM_END_OF_POST_RESPONSE_HECI_HDR 0x80010001
#define CBM_RESET_REQUEST_HECI_HDR        0x80030001
#define CBM_RESET_RESPONSE_HECI_HDR       0x80020001

//
// Enumerations used in Core BIOS Messages
//
// End Of Post Codes.
//
#define CBM_EOP_EXITING_G3  0x01
#define CBM_EOP_RESERVED    0x02
#define CBM_EOP_EXITING_S3  0x03
#define CBM_EOP_EXITING_S4  0x04
#define CBM_EOP_EXITING_S5  0x05

//
// Reset Request Origin Codes.
//
#define CBM_RR_REQ_ORIGIN_BIOS_MEMORY_INIT  0x01
#define CBM_RR_REQ_ORIGIN_BIOS_POST         0x02
#define CBM_RR_REQ_ORIGIN_MEBX              0x03

//
// Reset Type Codes.
//
#define CBM_HRR_GLOBAL_RESET  0x01

//
// Reset Response Codes.
//
#define CBM_HRR_RES_REQ_NOT_ACCEPTED  0x01

//
// definitions for ICC MEI Messages
//
#define IBEX_PEAK_PLATFORM    0x00010000
#define COUGAR_POINT_PLATFORM 0x00020000
#define LYNX_POINT_PLATFORM   0x00030000

typedef enum {
  ICC_STATUS_SUCCESS                        = 0,
  ICC_STATUS_SUCCESS_WAITING_FOR_RESET,
  ICC_STATUS_INCORRECT_API_VERSION,
  ICC_STATUS_INVALID_FUNCTION,
  ICC_STATUS_INVALID_BUFFER_LENGTH,
  ICC_STATUS_INVALID_PARAMETERS,
  ICC_STATUS_FLASH_WEAR_OUT_VIOLATION,
  ICC_STATUS_CLOCK_REQ_ENTRY_VIOLATION,
  ICC_STATUS_STATIC_REGISTER_MASK_VIOLATION,
  ICC_STATUS_DYNAMIC_REGISTER_MASK_VIOLATION,
  ICC_STATUS_IMMEDIATE_REQUIRES_POWER_CYCLE,
  ICC_STATUS_ILLEGAL_RECORD_ID,
  ICC_STATUS_ENABLED_CLOCK_MASK_VIOLATION,
  ICC_STATUS_INVALID                        = 0xFFFFFFFF
} ICC_MEI_CMD_STATUS;

typedef union _ICC_CLOCK_ENABLES_CONTROL_MASK {
  UINT32  Dword;
  struct {
    UINT32  Flex0 : 1;
    UINT32  Flex1 : 1;
    UINT32  Flex2 : 1;
    UINT32  Flex3 : 1;
    UINT32  Reserved1 : 3;
    UINT32  PCI_Clock0 : 1;
    UINT32  PCI_Clock1 : 1;
    UINT32  PCI_Clock2 : 1;
    UINT32  PCI_Clock3 : 1;
    UINT32  PCI_Clock4 : 1;
    UINT32  Reserved2 : 4;
    UINT32  SRC0 : 1;
    UINT32  SRC1 : 1;
    UINT32  SRC2 : 1;
    UINT32  SRC3 : 1;
    UINT32  SRC4 : 1;
    UINT32  SRC5 : 1;
    UINT32  SRC6 : 1;
    UINT32  SRC7 : 1;
    UINT32  CSI_SRC8 : 1;
    UINT32  CSI_DP : 1;
    UINT32  PEG_A : 1;
    UINT32  PEG_B : 1;
    UINT32  DMI : 1;
    UINT32  Reserved3 : 3;
  } Fields;
} ICC_CLOCK_ENABLES_CONTROL_MASK;

typedef enum {
  LOCK_ICC_REGISTERS                        = 0x2,
  SET_CLOCK_ENABLES                         = 0x3,
  GET_ICC_PROFILE                           = 0x4,
  SET_ICC_PROFILE                           = 0x5,
  GET_ICC_CLOCKS_CAPABILITIES               = 0x6,
  GET_OEM_CLOCK_RANGE_DEFINITION_RECORD     = 0x7,
  GET_ICC_RECORD                            = 0x8,
  READ_ICC_REGISTER                         = 0x9,
  WRITE_ICC_REGISTER                        = 0xa,
  WRITE_UOB_RECORD                          = 0xb,
  READ_MPHY_SETTINGS                        = 0xe,
  WRITE_MPHY_SETTINGS                       = 0xf
} ICC_MEI_COMMAND_ID;

typedef struct {
  UINT32              ApiVersion;
  ICC_MEI_COMMAND_ID  IccCommand;
  ICC_MEI_CMD_STATUS  IccResponse;
  UINT32              BufferLength;
  UINT32              Reserved;
} ICC_HEADER;

typedef struct {
  ICC_HEADER  Header;
} ICC_GET_PROFILE_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
  UINT8       SupportedProfilesNumber;
  UINT8       IccProfileSoftStrap;
  UINT8       IccProfileIndex;
  UINT8       Padding;
} ICC_GET_PROFILE_RESPONSE;

typedef union {
  ICC_GET_PROFILE_MESSAGE   message;
  ICC_GET_PROFILE_RESPONSE  response;
} ICC_GET_PROFILE_BUFFER;

typedef struct {
  ICC_HEADER  Header;
  UINT8       ProfileBIOS;
  UINT8       PaddingA;
  UINT16      PaddingB;
} ICC_SET_PROFILE_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
} ICC_SET_PROFILE_RESPONSE;

typedef union {
  ICC_SET_PROFILE_MESSAGE   message;
  ICC_SET_PROFILE_RESPONSE  response;
} ICC_SET_PROFILE_BUFFER;

typedef struct {
  ICC_HEADER  Header;
  UINT32      ClockEnables;
  UINT32      ClockEnablesMask;
  UINT32      Params;
} ICC_SET_CLK_ENABLES_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
} ICC_SET_CLK_ENABLES_RESPONSE;

typedef union {
  ICC_SET_CLK_ENABLES_MESSAGE   message;
  ICC_SET_CLK_ENABLES_RESPONSE  response;
} ICC_SET_CLK_ENABLES_BUFFER;

typedef struct {
  ICC_HEADER  Header;
} ICC_GET_CLK_CAPABILITIES_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
  UINT32      VersionNumber;
  UINT8       IccHwSku;
  UINT8       Reserved;
  UINT16      MaxSusramRecordSize;
  UINT64      IccSkuEnforcementTable;
  UINT32      IccBootStatusReport;
} ICC_GET_CLK_CAPABILITIES_RESPONSE;

typedef union {
  ICC_GET_CLK_CAPABILITIES_MESSAGE  message;
  ICC_GET_CLK_CAPABILITIES_RESPONSE response;
} ICC_GET_CLK_CAPABILITIES_BUFFER;

#define ICC_RESPONSE_MODE_WAIT    0
#define ICC_RESPONSE_MODE_SKIP    1
#define ICC_LOCK_ACCESS_MODE_SET  0
#define ICC_LOCK_ACCESS_MODE_GET  1
#define ICC_LOCK_MASK_COUNT       255
#define WRITE_ICC_REG_BUNDLE_COUNT  1           ///< 1 bundle for 1 DWORD register write
#define ADDRESS_MASK_FIXED_DATA     0x00017F01  ///< Target ID = 7F (Aux), MWM = 1(16 bits)
#define WRITE_ICC_RECORD_FLAGS      0x00012010  ///< 16 byte record length, No param section, valid bit

typedef struct _ICC_REG_BUNDLES
{
  UINT32 BundlesCnt :16; ///< Bundles Count - number of Address Mask entries
  UINT32 AU         :1;  ///< AU=1 -> All regisaters are Unlocked
  UINT32 Reserved   :15;
} ICC_REG_BUNDLES;

typedef struct {
  ICC_REG_BUNDLES RegBundles;
  UINT32          RegMask[ICC_LOCK_MASK_COUNT];
} ICC_LOCK_REGS_INFO;

typedef struct {
  ICC_HEADER          Header;
  UINT8               AccessMode;
  UINT8               Parameters;
  UINT8               Reserved[2];
  ICC_LOCK_REGS_INFO  LockRegInfo;
} ICC_LOCK_REGISTERS_MESSAGE;

typedef struct {
  ICC_HEADER          Header;
  UINT32              Reserved;
  ICC_LOCK_REGS_INFO  LockRegInfo;
} ICC_LOCK_REGISTERS_RESPONSE;

typedef union {
  ICC_LOCK_REGISTERS_MESSAGE  message;
  ICC_LOCK_REGISTERS_RESPONSE response;
} ICC_LOCK_REGISTERS_BUFFER;

typedef union _ICC_ADDRESS_MASK
{
  UINT32 AddressMaskData;
  struct
  {
    UINT32 MaskWidthModifier :2;
    UINT32 Offset            :6;
    UINT32 TargetId          :7;
    UINT32 Reserved          :1;
  }Fields;
} ICC_ADDRESS_MASK;

typedef struct _ICC_DWORD_RECORD
{
  UINT32            RecordFlags;
  ICC_REG_BUNDLES   BundleCount;
  ICC_ADDRESS_MASK  AddressMask;
  UINT32            RegValue;
} ICC_RECORD_DWORD;

typedef struct {
  ICC_HEADER        Header;
  UINT16            Reserved;
  UINT8             Params;
  UINT8             Reserved1;
  ICC_RECORD_DWORD  RecordDword;
} ICC_WRITE_ICC_REG_DWORD_MESSAGE;

typedef struct {
  ICC_HEADER  Header;
  UINT16      BytesProcessed;
  UINT16      Reserved;
} ICC_WRITE_ICC_REG_DWORD_RESPONSE;

typedef union {
  ICC_WRITE_ICC_REG_DWORD_MESSAGE   message;
  ICC_WRITE_ICC_REG_DWORD_RESPONSE  response;
} ICC_WRITE_ICC_REG_BUFFER;

//
// SPI MEI Messages
//
#define HMRFPO_ENABLE_CMD_ID      0x01
#define HMRFPO_LOCK_CMD_ID        0x02
#define HMRFPO_GET_STATUS_CMD_ID  0x03

typedef enum {
  HMRFPO_ENABLE_SUCCESS                     = 0,
  HMRFPO_ENABLE_LOCKED,
  HMRFPO_NVAR_FAILURE,
  HMRFOP_ATP_POLICY,
  HMRFPO_ENABLE_UNKNOWN_FAILURE
} HMRFPO_ENABLE_STATUS;

typedef struct _MKHI_HMRFPO_ENABLE {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT64              Nonce;
} MKHI_HMRFPO_ENABLE;

typedef struct _HMRFPO_ENABLE {
  HECI_MESSAGE_HEADER Header;
  MKHI_HMRFPO_ENABLE  Msg;
} HMRFPO_ENABLE;

typedef struct _MKHI_HMRFPO_ENABLE_RESPONSE {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT32              FactoryDefaultBase;
  UINT32              FactoryDefaultLimit;
  UINT8               Status;
  UINT8               Rsvd[3];
} MKHI_HMRFPO_ENABLE_RESPONSE;

typedef struct _HMRFPO_ENABLE_RESPONSE {
  HECI_MESSAGE_HEADER         Header;
  MKHI_HMRFPO_ENABLE_RESPONSE Msg;
} HMRFPO_ENABLE_RESPONSE;

typedef enum {
  HMRFPO_LOCK_SUCCESS                       = 0,
  HMRFPO_LOCK_FAILURE
} HMRFPO_LOCK_STATUS;

typedef struct _MKHI_HMRFPO_LOCK {
  MKHI_MESSAGE_HEADER MkhiHeader;
} MKHI_HMRFPO_LOCK;

typedef struct _HMRFPO_LOCK {
  HECI_MESSAGE_HEADER Header;
  MKHI_HMRFPO_LOCK    Msg;
} HMRFPO_LOCK;

typedef struct _MKHI_HMRFPO_LOCK_RESPONSE {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT64              Nonce;
  UINT32              FactoryDefaultBase;
  UINT32              FactoryDefaultLimit;
  UINT8               Status;
  UINT8               Reserved[3];
} MKHI_HMRFPO_LOCK_RESPONSE;

typedef struct _HMRFPO_LOCK_RESPONSE {
  HECI_MESSAGE_HEADER       Header;
  MKHI_HMRFPO_LOCK_RESPONSE Data;
} HMRFPO_LOCK_RESPONSE;

typedef struct _MKHI_HMRFPO_GET_STATUS {
  MKHI_MESSAGE_HEADER MkhiHeader;
} MKHI_HMRFPO_GET_STATUS;

typedef struct _HMRFPO_GET_STATUS {
  HECI_MESSAGE_HEADER     Header;
  MKHI_HMRFPO_GET_STATUS  Msg;
} HMRFPO_GET_STATUS;

typedef struct _MKHI_HMRFPO_GET_STATUS_RESPONSE {
  MKHI_MESSAGE_HEADER MkhiHeader;
  UINT8               Status;
  UINT8               Reserved[3];
} MKHI_HMRFPO_GET_STATUS_RESPONSE;

typedef struct _HMRFPO_GET_STATUS_RESPONSE {
  HECI_MESSAGE_HEADER             Header;
  MKHI_HMRFPO_GET_STATUS_RESPONSE Data;
} HMRFPO_GET_STATUS_RESPONSE;

#define HMRFPO_LOCKED   1
#define HMRFPO_ENABLED  2

//
// ME State Control
//
#define EFI_ME_STATE_STALL_1_SECOND 1000000
#define EFI_ME_STATE_MAX_TIMEOUT    20
//
// KVM support
//
#define EFI_KVM_MESSAGE_COMMAND 0x08
#define EFI_KVM_BYTE_COUNT      0x06
#define EFI_KVM_QUERY_REQUES    0x01
#define EFI_KVM_QUERY_RESPONSE  0x02
#define EFI_KVM_VERSION         0x10

#define EFI_KVM_STALL_1_SECOND  1000000   ///< Stall 1 second
#define EFI_KVM_MAX_WAIT_TIME   (60 * 8)  ///< 8 Mins
typedef enum {
  QUERY_REQUEST                             = 0,
  CANCEL_REQUEST
} QUERY_TYPE;

typedef struct _AMT_QUERY_KVM_REQUEST {
  UINT8   Command;
  UINT8   ByteCount;
  UINT8   SubCommand;
  UINT8   VersionNumber;
  UINT32  QueryType;
} AMT_QUERY_KVM_REQUEST;

typedef enum {
  KVM_SESSION_ESTABLISHED                   = 1,
  KVM_SESSION_CANCELLED
} RESPONSE_CODE;

typedef struct _AMT_QUERY_KVM_RESPONSE {
  UINT8   Command;
  UINT8   ByteCount;
  UINT8   SubCommand;
  UINT8   VersionNumber;
  UINT32  ResponseCode;
} AMT_QUERY_KVM_RESPONSE;

///
/// ME Memory Debug support
///
typedef struct _SET_DEBUG_MEMORY_DATA {
  UINT32  BiosDebugMemoryAddress;
  UINT32  BiosDebugMemorySize;
  UINT32  MeVeDebugMemoryAddress;
  UINT32  MeVeDebugMemorySize;
} SET_DEBUG_MEMORY_DATA;

typedef struct _GEN_SET_DEBUG_MEMORY {
  MKHI_MESSAGE_HEADER   MKHIHeader;
  SET_DEBUG_MEMORY_DATA Data;
} GEN_SET_DEBUG_MEMORY;

typedef struct _GEN_SET_DEBUG_MEMORY_ACK {
  MKHI_MESSAGE_HEADER Header;
} GEN_SET_DEBUG_MEMORY_ACK;

//
// BIOS MDES messaging
//
#define MDES_BIOS_MSG_LOG_REQ_CMD     0x0B
#define MDES_BIOS_MSG_GET_CONFIG_CMD  0x0C

typedef struct _MDES_ATTR {
  UINT16  Severity : 2;
  UINT16  PayLoadType : 6;
  UINT16  Reserved : 8;
} MDES_ATTR;

typedef struct _BIOS_ATTR {
  UINT16  CallerIdData : 1;
  UINT16  ExtendedDataHeader : 1;
  UINT16  Reserved : 14;
} BIOS_ATTR;

typedef struct {
  UINT16    HeaderSize;
  UINT16    Size;
  EFI_GUID  Type;
} MDES_EXTENDED_DATA_HEADER;

#define SIZE_OF_MDES_EXTENDED_DATA  (2048 - \
                                      ( \
                                      16 + sizeof (MDES_ATTR) + sizeof (BIOS_ATTR) + sizeof (EFI_GUID) + sizeof (MDES_EXTENDED_DATA_HEADER) + \
                                    sizeof (MKHI_MESSAGE_HEADER) \
                                ) \
          )

typedef struct _CBM_BIOS_MDES_MSG_DATA {
  MDES_ATTR                 MdesAttr;
  BIOS_ATTR                 BiosAttr;
  UINT32                    Serial;
  UINT32                    StatusType;
  UINT32                    StatusCode;
  UINT32                    Instance;
  EFI_GUID                  CallerId;
  MDES_EXTENDED_DATA_HEADER ExtendedDataHeader;
  UINT8                     ExtendedData[SIZE_OF_MDES_EXTENDED_DATA];
} CBM_BIOS_MDES_MSG_DATA;

typedef struct _CBM_BIOS_MDES_MSG_REQ {
  MKHI_MESSAGE_HEADER     MKHIHeader;
  CBM_BIOS_MDES_MSG_DATA  Data;
} CBM_BIOS_MDES_MSG_REQ;

typedef struct _MKHI_CBM_BIOS_MDES_MSG_REQ {
  HECI_MESSAGE_HEADER   Header;
  CBM_BIOS_MDES_MSG_REQ Msg;
} MKHI_CBM_BIOS_MDES_MSG_REQ;

typedef struct _MKHI_CBM_BIOS_MDES_MSG_GET_CONFIG_REQ {
  MKHI_MESSAGE_HEADER MKHIHeader;
} MKHI_CBM_BIOS_MDES_MSG_GET_CONFIG_REQ;

typedef union _MDES_BIOS_FLAGS {
  volatile UINT32 ul;
  struct {
    UINT32  MdesEnabled : 1;            ///< High level indication if MDES is enabled
    UINT32  MdesLogPaused : 1;          ///< A caller paused MDES logging as part of the log retrieval process
    UINT32  LanInterfaceEnabled : 1;    ///< The user configuration has enabled the LAN debug interface
    UINT32  SmbusInterfaceEnabled : 1;  ///< The user configuration has enabled the SMBus debug interface
    UINT32  PramLogEnabled : 1;         ///< The user configuration has enabled the PRAM debug log
    UINT32  FlashLogEnabled : 1;        ///< The user configuration has enabled the flash error log
    UINT32  MdesBlockingModeEn : 1;     ///< Set to 0 when operating in buffered mode, set to 1 when MDES blocks the caller until the event is dispatched.
    UINT32  Reserved7 : 2;
    UINT32  SensitiveMsgEnabled : 1;    ///< Set to 1 to indicate Sensitive messages are enabled
    UINT32  DescriptorUnlocked : 1;     ///< Set when FW reads all regions are unlocked.  Enables 'none' sensitivity messages.
    UINT32  MdesPolicyEnabled : 1;      ///< Set to 1 to indicate MDES is enabled due to BIOS sending the enable message (on current or previous boot)
    UINT32  MdesEnableRcvdFromBios : 1; ///< BIOS has sent the MDES policy to 'enabled' via a MKHI message on this current boot
    UINT32  Reserved13 : 19;
  } fl;
} MDES_BIOS_FLAGS;

typedef struct _CBM_BIOS_MDES_MSG_GET_CONFIG_DATA {
  MDES_BIOS_FLAGS Flags;
  UINT32          BiosEventFilters;
} CBM_BIOS_MDES_MSG_GET_CONFIG_DATA;

typedef struct _MKHI_CBM_BIOS_MDES_MSG_GET_CONFIG_ACK {
  MKHI_MESSAGE_HEADER               Header;
  CBM_BIOS_MDES_MSG_GET_CONFIG_DATA Data;
} MKHI_CBM_BIOS_MDES_MSG_GET_CONFIG_ACK;

#pragma pack()

#endif
