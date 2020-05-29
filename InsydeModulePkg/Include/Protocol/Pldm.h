/** @file
  Platform Level Data Model Protocol definition

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PLDM_BASE_H_
#define _PLDM_BASE_H_

#include <Uefi.h>
#include <Protocol/Mctp.h>

#define EFI_PLDM_PROTOCOL_GUID \
  { 0x5b6d567e, 0xb2e9, 0x470f, 0x9e, 0x21, 0x94, 0xb7, 0xde, 0xd0, 0x6a, 0xc9 }


typedef struct _EFI_PLDM_PROTOCOL EFI_PLDM_PROTOCOL;
//
// PLDM Generic Completion Codes
//
#define PLDM_SUCCESS                               0x00
#define PLDM_ERROR                                 0x01
#define PLDM_ERROR_INVALID_DATA                    0x02
#define PLDM_ERROR_INVALID_LENGTH                  0x03
#define PLDM_ERROR_NOT_READY                       0x04
#define PLDM_ERROR_UNSUPPORTED_COMMAND             0x05
#define PLDM_ERROR_INVALID_PLDM_TYPE               0x20

#define PLDM_ERROR_INVALID_DATA_TRANSFER_HANDLE    0x80
#define PLDM_ERROR_INVALID_TRANSFER_OPERATION_FLAG 0x81
#define PLDM_ERROR_INVALID_TYPE_IN_REQUEST_DATA    0x83


#define PLDM_TYPE_MESSAGE_CONTROL_AND_DISCOVERY    0x00
#define PLDM_TYPE_SMBIOS                           0x01
#define PLDM_TYPE_PLATFORM_MONITORING_AND_CONTROL  0x02
#define PLDM_TYPE_BIOS_CONTROL_AND_CONFIGURATION   0x03

#define PLDM_BASE_SET_TID                          0x01
#define PLDM_BASE_GET_TID                          0x02
#define PLDM_BASE_GET_PLDM_VERSION                 0x03
#define PLDM_BASE_GET_PLDM_TYPES                   0x04
#define PLDM_BASE_GET_PLDM_COMMANDS                0x05

#define PLDM_GET_SMBIOS_STRUCTURE_TABLE_METADATA   0x01
#define PLDM_SET_SMBIOS_STRUCTURE_TABLE_METADATA   0x02
#define PLDM_GET_SMBIOS_STRUCTURE_TABLE            0x03
#define PLDM_SET_SMBIOS_STRUCTURE_TABLE            0x04
#define PLDM_GET_SMBIOS_STRUCTURE_BY_TYPE          0x05
#define PLDM_GET_SMBIOS_STRUCTURE_BY_HANDLE        0x06

#define PLDM_GET_BIOS_TABLE                        0x01
#define PLDM_SET_BIOS_TABLE                        0x02
#define PLDM_UPDATE_BIOS_TABLE                     0x03
#define PLDM_GET_BIOS_TAGS                         0x04
#define PLDM_SET_BIOS_TAGS                         0x05
#define PLDM_ACCEPT_BIOS_ATTR_PENDING_VALUES       0x06
#define PLDM_SET_BIOS_ATTR_CURRENT_VALUE           0x07
#define PLDM_GET_BIOS_ATTR_CURRENT_VALUE_BY_HANDLE 0x08
#define PLDM_GET_BIOS_ATTR_PENDING_VALUE_BY_HANDLE 0x09
#define PLDM_GET_BIOS_ATTR_CURRENT_VALUE_BY_TYPE   0x0A
#define PLDM_GET_BIOS_ATTR_PENDING_VALUE_BY_TYPE   0x0B
#define PLDM_GET_DATE_TIME                         0x0C
#define PLDM_SET_DATE_TIME                         0x0D
#define PLDM_GET_BIOS_STRING_TABLE_STRING_TYPE     0x0E
#define PLDM_SET_BIOS_STRING_TABLE_STRING_TYPE     0x0F

#define PLDM_PLATFORM_EVENT_MESSAGE                0x0A
#define PLDM_GET_SENSOR_READING                    0x11
#define PLDM_GET_STATE_SENSOR_READING              0x21


#define PLDM_RESPONSE_MESSAGE                      0x00
#define PLDM_REQUEST_MESSAGE                       0x01
#define PLDM_UNACKOWLEDGED_REQUEST_MESSAGE         0x03

#define PLDM_HEADER_VERSION                        0x00

#define PLDM_BUFFER_SIZE                           64
#define MAX_PLDM_PAYLOAD_SIZE                      60
#define RESPONSE_TRANSFER_HEADER_SIZE               5


#define PLDM_HEADER_COMMON     {\
                                 MCTP_MESSAGE_TYPE_PLDM,         /* MCTP Message Type */\
                                 0,                              /* Integrity Check   */\
                                 0,                              /* Instance ID       */\
                                 0,                              /* Reserved Bit      */\
                                 0,                              /* Datagram Bit      */\
                                 1,                              /* Request Bit       */\
                                 PLDM_HEADER_VERSION,            /* Header Version    */\
                                 0,                              /* PLDM Type         */\
                                 0                               /* Command Code      */\
                               }
                                                               

typedef enum {
  PldmGetNextPart               = 0x00,
  PldmGetFirstPart              = 0x01
} PLDM_TRANSFER_OPERATION_FLAG;

typedef enum {
  PldmTransferStart             = 0x01,
  PldmTransferMiddle            = 0x02,
  PldmTransferEnd               = 0x04,
  PldmTransferStartAndEnd       = 0x05
} PLDM_TRANSFER_FLAG;

typedef enum {
  BiosEnumberation              = 0x00,
  BiosString                    = 0x01,
  BiosPassword                  = 0x02,
  BiosInteger                   = 0x03,
  BiosBootConfigSetting         = 0x04,
  BiosCollection                = 0x05,
  BiosConfigSet                 = 0x06,
  BiosEnumberationReadOnly      = 0x80,
  BiosStringReadOnly            = 0x81,
  BiosPasswordReadOnly          = 0x82,
  BiosIntegerReadOnly           = 0x83,
  BiosBootConfigSettingReadOnly = 0x84,
  BiosCollectionReadOnly        = 0x85,
  BiosConfigSetReadOnly         = 0x86
} BIOS_ATTRIBUTE;

typedef enum {
  BiosStringTable               = 0x00,
  BiosAttrTable                 = 0x01,
  BiosAttrValueTable            = 0x02,
  BiosAttrPendingValueTable     = 0x03
} BIOS_TABLE_TYPE;

#pragma pack (1)

typedef struct  {
  UINT8 MessageType             : 7;
  UINT8 IntegrityCheck          : 1;
  UINT8 InstanceId              : 5;
  UINT8 Reserved                : 1;
  UINT8 DatagramBit             : 1;
  UINT8 RequestBit              : 1;
  UINT8 PldmType                : 6;
  UINT8 HeaderVersion           : 2;
  UINT8 PldmCmdCode;
} PLDM_HEADER;


typedef struct {
  UINT32 DataTransferHandle;
  UINT8  TransferOperationFlag;
  UINT8  PldmType;
} PLDM_GET_VERSION_REQUEST_DATA;

typedef struct {
  UINT8 PldmType;
  UINT32 Version;
} PLDM_GET_COMMANDS_REQUEST_DATA;

typedef UINT8 PLDM_SUPPORTED_TYPES[8];
typedef UINT8 PLDM_SUPPORTED_COMMANDS[32];



typedef struct {
  UINT8   MajorVersion;
  UINT8   MinorVersion;
  UINT16  MaxStructureSize;
  UINT16  TableLength;
  UINT16  NumberOfSmbiosStructures;
  UINT32  Crc32;
} PLDM_SMBIOS_STRUCTURE_TABLE_METADATA;

typedef struct {
  UINT8 Second;
  UINT8 Minute;
  UINT8 Hour;
  UINT8 Day;
  UINT8 Month;
  UINT16 Year;
} PLDM_DATE_TIME;

typedef struct {
  BIOS_TABLE_TYPE TableType;
  UINT32          TableTag;
} BIOS_TABLE_TAG;

typedef struct {
  UINT16 Handle;
  UINT8  AttrType;
  UINT8 BootConfigType;
  UINT8 OrderAndFailThroughMode;
  UINT8 NumberOfPendingBootSourceSetting;
  UINT8 BootSourceIndex[16];
} BOOT_CONFIG_SETTING_ATTR_VALUE;

#pragma pack ()


typedef
EFI_STATUS
(EFIAPI *PLDM_SET_BINDING_MCTP) (
  IN EFI_PLDM_PROTOCOL              *This,
  IN EFI_MCTP_PROTOCOL              *BindingMctp
);


typedef 
EFI_STATUS
(EFIAPI *PLDM_SEND_MESSAGE) (
  IN EFI_PLDM_PROTOCOL *This,
  IN PLDM_HEADER       Header,
  IN VOID              *Message,
  IN UINTN             MessageSize
);

typedef 
EFI_STATUS
(EFIAPI *PLDM_RECEIVE_MESSAGE) (
  IN EFI_PLDM_PROTOCOL    *This,
  OUT PLDM_HEADER          *Header,
  OUT VOID                 *Message,
  IN OUT UINTN            *MessageSize
);


typedef
EFI_STATUS
(EFIAPI *PLDM_GET_TID_COMMAND) (
  IN EFI_PLDM_PROTOCOL *This,
  OUT UINT8            *Tid
);

typedef
EFI_STATUS
(EFIAPI *PLDM_GET_VERSION_COMMAND) (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             PldmType,
  OUT UINT32            *VersionInfo,
  OUT UINTN             *VersionInfoSize
);

typedef
EFI_STATUS
(EFIAPI *PLDM_GET_SUPPORTED_TYPES_COMMAND) (
  EFI_PLDM_PROTOCOL            *This,
  UINT8                        *SupportedTypes              
);

typedef
EFI_STATUS
(EFIAPI *PLDM_GET_SUPPORTED_COMMANDS_COMMAND) (
  EFI_PLDM_PROTOCOL *This,
  UINT8             PldmType,
  UINT32            PldmTypeVersion,
  UINT8             *SupportedCommands
);

typedef
EFI_STATUS
(EFIAPI *PLDM_SET_SMBIOS_STRUCTURE_TABLE_METADATA_COMMAND) (
  EFI_PLDM_PROTOCOL   *This
);

typedef
EFI_STATUS
(EFIAPI *PLDM_GET_SMBIOS_STRUCTURE_TABLE_METADATA_COMMAND)(
  EFI_PLDM_PROTOCOL                      *This,
  PLDM_SMBIOS_STRUCTURE_TABLE_METADATA   *PldmResponse
);

typedef
EFI_STATUS
(EFIAPI *PLDM_GET_SMBIOS_STRUCTURE_TABLE_COMMAND) (
  IN     EFI_PLDM_PROTOCOL *This,
  OUT    UINT8             *SmbiosBuf,
  IN OUT UINTN             *Size
);

typedef
EFI_STATUS
(EFIAPI *PLDM_SET_SMBIOS_STRUCTURE_TABLE_COMMAND)(
  EFI_PLDM_PROTOCOL                              *This
);

typedef
EFI_STATUS
(EFIAPI *PLDM_BIOS_SET_DATA_TIME_COMMAND) (
  IN EFI_PLDM_PROTOCOL   *This
);



typedef
EFI_STATUS
(EFIAPI *PLDM_GET_BIOS_TABLE_COMMAND) (
  IN EFI_PLDM_PROTOCOL *This,
  IN UINT8             TableType,
  OUT VOID             *Table,
  OUT UINTN            *TableSize
);

typedef
EFI_STATUS
(EFIAPI *PLDM_SET_BIOS_TABLE_COMMAND) (
  IN EFI_PLDM_PROTOCOL *This,
  IN UINT8             TableType,
  IN VOID              *Table,
  IN UINTN             TableSize
);

typedef 
EFI_STATUS
(EFIAPI *PLDM_GET_BIOS_TABLE_TAGS_COMMAND) (
  IN  EFI_PLDM_PROTOCOL  *This,
  IN  UINT8              NumberOfTables,
  IN  UINT8              *TableTypes,
  OUT UINT32             *TableTags
);

typedef
EFI_STATUS
  (EFIAPI *PLDM_SET_BIOS_TABLE_TAGS_COMMAND) (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             NumberOfTables,
  IN  UINT8             *TableTypes,
  IN  UINT32            *TableTags
);

typedef
EFI_STATUS
(EFIAPI *PLDM_GET_BIOS_ATTR_CURRENT_VALUE_BY_HANDLE_COMMAND) (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT16             AttrHandle,
  OUT VOID              *AttrData,
  OUT UINTN             *AttrDataSize
);

typedef
EFI_STATUS
(EFIAPI *PLDM_GET_BIOS_ATTR_PENDING_VALUE_BY_HANDLE_COMMAND) (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT16             AttrHandle,
  OUT VOID              *AttrData,
  OUT UINTN             *AttrDataSize
);


typedef
EFI_STATUS
(EFIAPI *PLDM_SET_BIOS_ATTR_CURRENT_VALUE_COMMAND) (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  VOID              *AttrData,
  IN  UINTN             AttrDataSize
);

typedef
EFI_STATUS
  (EFIAPI *PLDM_GET_BIOS_ATTR_CURRENT_VALUE_BY_TYPE_COMMAND) (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             AttrType,
  OUT VOID              *AttrData,
  OUT UINTN             *AttrDataSize
);

typedef
EFI_STATUS
  (EFIAPI *PLDM_GET_BIOS_ATTR_PENDING_VALUE_BY_TYPE_COMMAND) (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             AttrType,
  OUT VOID              *AttrData,
  OUT UINTN             *AttrDataSize
);

typedef
EFI_STATUS
(EFIAPI *PLDM_ACCEPT_BIOS_ATTR_PENDING_VALUES_COMMAND) (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT16            NumOfHandles,
  IN  UINT16            *AttrHandles
);

typedef
EFI_STATUS
(EFIAPI *PLDM_PLATFORM_EVENT_MESSAGE_COMMAND) (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8 FormatVersion,
  IN  UINT8 Tid,
  IN  UINT8 EventClass,
  IN  VOID  *EventData,
  IN  UINTN EventDataLen,
  OUT UINT8 *ReturnStatus
);

typedef
VOID 
(EFIAPI *APPEND_BIOS_STRING) (
  EFI_PLDM_PROTOCOL *This,
  UINT16     StrHandle,
  UINTN      StrLength,
  CHAR8      *String
);

typedef
VOID 
(EFIAPI *APPEND_BIOS_ATTR) (
  EFI_PLDM_PROTOCOL *This,
  UINT16     AttrHandle,
  UINT8      AttrType,
  UINT16     AttrNameHandle,
  UINTN      AttrDataSize,
  VOID       *AttrData
);

typedef
VOID 
(EFIAPI *APPEND_BIOS_ATTR_VALUE) (
  EFI_PLDM_PROTOCOL *This,
  UINT16     AttrHandle,
  UINT8      AttrType,
  UINTN      AttrValueDataSize,
  VOID       *AttrValueData
);

typedef
EFI_STATUS 
(EFIAPI *RESET_BIOS_TABLES)(
EFI_PLDM_PROTOCOL *This
);

typedef
EFI_STATUS
(EFIAPI *PUSH_BIOS_TABLES) (                                                                    
EFI_PLDM_PROTOCOL *Pldm
);

typedef
VOID
(EFIAPI *SET_PLDM_REQUEST_MODE) (
  EFI_PLDM_PROTOCOL  *This
);

typedef
VOID
(EFIAPI *SET_PLDM_RESPONSE_MODE) (
  EFI_PLDM_PROTOCOL  *This
);


typedef
EFI_STATUS
(EFIAPI *REGISTER_SENSOR_READING_FUNCTION) (
  EFI_PLDM_PROTOCOL *This,
  EFI_EVENT_NOTIFY SensorReadingFun
);
  

struct _EFI_PLDM_PROTOCOL {
  PLDM_SET_BINDING_MCTP                              SetBindingMctp;
  SET_PLDM_REQUEST_MODE                              SetRequestMode;
  SET_PLDM_RESPONSE_MODE                             SetResponseMode;  
  PLDM_SEND_MESSAGE                                  SendMessage;
  PLDM_RECEIVE_MESSAGE                               ReceiveMessage;
  PLDM_GET_TID_COMMAND                               GetTid;
  PLDM_GET_VERSION_COMMAND                           GetVersion;
  PLDM_GET_SUPPORTED_TYPES_COMMAND                   GetSupportedTypes;
  PLDM_GET_SUPPORTED_COMMANDS_COMMAND                GetSupportedCommands;

  PLDM_GET_SMBIOS_STRUCTURE_TABLE_METADATA_COMMAND   GetSmbiosStructureTableMetaData;
  PLDM_SET_SMBIOS_STRUCTURE_TABLE_METADATA_COMMAND   SetSmbiosStructureTableMetaData;
  PLDM_GET_SMBIOS_STRUCTURE_TABLE_COMMAND            GetSmbiosStructureTable;
  PLDM_SET_SMBIOS_STRUCTURE_TABLE_COMMAND            SetSmbiosStructureTable;

  PLDM_BIOS_SET_DATA_TIME_COMMAND                    SetDateTime;
  PLDM_GET_BIOS_TABLE_COMMAND                        GetBiosTable;
  PLDM_SET_BIOS_TABLE_COMMAND                        SetBiosTable;
  PLDM_GET_BIOS_TABLE_TAGS_COMMAND                   GetBiosTableTags;
  PLDM_SET_BIOS_TABLE_TAGS_COMMAND                   SetBiosTableTags;
  PLDM_GET_BIOS_ATTR_CURRENT_VALUE_BY_HANDLE_COMMAND GetBiosAttrCurrentValueByHandle;
  PLDM_GET_BIOS_ATTR_CURRENT_VALUE_BY_TYPE_COMMAND   GetBiosAttrCurrentValueByType;
  PLDM_GET_BIOS_ATTR_PENDING_VALUE_BY_HANDLE_COMMAND GetBiosAttrPendingValueByHandle;
  PLDM_GET_BIOS_ATTR_PENDING_VALUE_BY_TYPE_COMMAND   GetBiosAttrPendingValueByType;
  PLDM_SET_BIOS_ATTR_CURRENT_VALUE_COMMAND           SetBiosAttrCurrentValue;
  PLDM_ACCEPT_BIOS_ATTR_PENDING_VALUES_COMMAND       AcceptBiosAttrPendingValues;
  PLDM_PLATFORM_EVENT_MESSAGE_COMMAND                PlatformEventMessage;

  APPEND_BIOS_STRING                                 AppendBiosString;
  APPEND_BIOS_ATTR                                   AppendBiosAttr;
  APPEND_BIOS_ATTR_VALUE                             AppendBiosAttrValue;
  RESET_BIOS_TABLES                                  ResetBiosTables;
  PUSH_BIOS_TABLES                                   PushBiosTables;
  REGISTER_SENSOR_READING_FUNCTION                   RegisterSensorReadingFunction;
};

extern EFI_GUID gPldmProtocolGuid;

#endif
