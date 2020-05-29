/** @file
  Platform Level Data Model

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DASH_PLDM_H_
#define _DASH_PLDM_H_

#include "DashCommon.h"

#define PLDM_SIGNATURE               SIGNATURE_32('P', 'L', 'D', 'M')

#define MAX_BOOT_COUNT           16
#define TABLE_INIT_SIZE          256

typedef struct _PLDM_CONTEXT PLDM_CONTEXT;

#pragma pack(1)

typedef struct {
  UINT32 TransferHandle;
  UINT8  Flag;
} PLDM_TRANSFER_HEADER;

typedef struct {
  UINT8 *Table;
  UINTN Size;
  UINTN MaxSize;
} BIOS_TABLE;  

typedef struct {
  LIST_ENTRY     BiosStringList;
  LIST_ENTRY     BiosAttrList;
  LIST_ENTRY     BiosAttrValueList;
  UINT32         BiosStringChecksum;
  UINT32         BiosAttrChecksum;
  UINT32         BiosAttrValueChecksum;
} BIOS_TABLE_STRUCTURE;

typedef struct {
  UINT8 BootConfigType;
  UINT8 OrderedFailThroughMode;
  UINT8 MinBootSource;
  UINT8 MaxBootSource;
  UINT8 NumPossibleBootSource;
  UINT16 BootSourceStrHandle[MAX_BOOT_COUNT];
} BOOT_CONFIG_ATTR;

typedef struct {
  UINT8 BootConfigType;
  UINT8 OrderedFailThroughMode;
  UINT8 NumBootSource;
  UINT8 BootSourceStrHandleIndex[MAX_BOOT_COUNT];
} BOOT_CONFIG_ATTR_VALUE;



#pragma pack()

struct _PLDM_CONTEXT {
  UINT32                              Signature;
  EFI_PLDM_PROTOCOL                   Pldm;
  EFI_MCTP_PROTOCOL                   *Mctp;
  UINT8                               InstanceId;
  UINT8                               *Buffer;
  UINT32                              TransferHandle;
  UINTN                               MaxPayloadSize;
  BOOLEAN                             ResponseMode;
  UINT8                               *SmbiosBuf;
  UINT16                              SmbiosTableLen;
  UINT32                              SmbiosCrc32;
  BIOS_TABLE                          BiosStringTable;
  BIOS_TABLE                          BiosAttrTable;
  BIOS_TABLE                          BiosAttrValueTable;
  UINT32                              BiosTags[3];
  EFI_EVENT                           SensorReadingCheckEvent;
  EFI_EVENT                           SensorReadingEvent;  
  BOOLEAN                             TransactionInProgress;
};



EFI_STATUS
EFIAPI
PldmSetBindingMctp (
  IN EFI_PLDM_PROTOCOL              *This,
  IN EFI_MCTP_PROTOCOL              *BindingMctp
);

EFI_MCTP_PROTOCOL *
EFIAPI
PldmGetBindingMctp (
  IN EFI_PLDM_PROTOCOL              *This
);

EFI_STATUS
EFIAPI
PldmSendMessage (
  IN EFI_PLDM_PROTOCOL *This,
  IN PLDM_HEADER       Header,
  IN VOID              *Message,
  IN UINTN             MessageSize
);

EFI_STATUS
EFIAPI
PldmReceiveMessage (
  IN EFI_PLDM_PROTOCOL    *This,
  OUT PLDM_HEADER      *Header,
  OUT VOID             *Message,
  OUT UINTN            *MessageSize
);

EFI_STATUS
EFIAPI
ProcessSimplePldmMessage (
  IN     EFI_PLDM_PROTOCOL *This,
  IN     UINT8              PldmType,
  IN     UINT8              Command,
  IN     VOID               *RequestMessage,
  IN     UINTN              RequestMessageSize,
  IN     VOID               *ResponseMessage,
  IN OUT UINTN              *ResponseMessageSize
);

EFI_STATUS
EFIAPI
ProcessPldmMessageWithSplitRequests (
  IN EFI_PLDM_PROTOCOL *This,
  IN UINT8              PldmType,
  IN UINT8              Command,
  IN VOID               *TransferHeader,
  IN UINTN              TransferHeaderSize,
  IN VOID               *RequestMessage,
  IN UINTN              RequestMessageSize
);

EFI_STATUS
EFIAPI
ProcessPldmMessageWithSplitResponses (
  IN EFI_PLDM_PROTOCOL *This,
  IN UINT8              PldmType,
  IN UINT8              Command,
  IN VOID               *TransferHeader,
  IN UINTN              TransferHeaderSize,
  OUT VOID              *ResponseMessage,
  IN OUT UINTN          *ResponseMessageSize
);

EFI_STATUS
EFIAPI
PldmSetSmbiosStructureTableMetaData (
  EFI_PLDM_PROTOCOL                              *This
);

EFI_STATUS
EFIAPI
PldmGetSmbiosStructureTableMetaData (
  EFI_PLDM_PROTOCOL                              *This,
  PLDM_SMBIOS_STRUCTURE_TABLE_METADATA          *PldmResponse
);

EFI_STATUS
EFIAPI
PldmGetSmbiosStructureTable (
  IN     EFI_PLDM_PROTOCOL *This,
  OUT    UINT8             *SmbiosBuf,
  IN OUT UINTN             *Size
);

EFI_STATUS
EFIAPI
PldmSetSmbiosStructureTable(
  EFI_PLDM_PROTOCOL                              *This
);

EFI_STATUS
EFIAPI
PldmBiosSetDateTime(
  IN EFI_PLDM_PROTOCOL   *This
);

EFI_STATUS
EFIAPI
PldmGetBiosTable (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             TableType,
  OUT VOID              *Table,
  OUT UINTN             *TableSize
);

EFI_STATUS
EFIAPI
PldmSetBiosTable (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             TableType,
  IN  VOID              *Table,
  IN  UINTN             TableSize
);

EFI_STATUS
EFIAPI
PldmGetBiosTableTags (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8           NumberOfTables,
  IN  UINT8           *TableTypes,
  OUT UINT32          *TableTags
);


EFI_STATUS
EFIAPI
PldmSetBiosTableTags (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8           NumberOfTables,
  IN  UINT8           *TableTypes,
  IN  UINT32          *TableTags

);

EFI_STATUS
EFIAPI
PldmGetBiosAttrCurrentValueByHandle (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT16             AttrHandle,
  OUT VOID              *AttrData,
  OUT UINTN             *AttrDataSize
);

EFI_STATUS
EFIAPI
PldmSetBiosAttrCurrentValue (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  VOID              *AttrData,
  IN  UINTN             AttrDataSize
);

EFI_STATUS
EFIAPI
PldmGetBiosAttrCurrentValueByType (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             AttrType,
  OUT VOID              *AttrData,
  OUT UINTN             *AttrDataSize
);

EFI_STATUS
EFIAPI
PldmGetBiosAttrPendingValueByHandle (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT16             AttrHandle,
  OUT VOID              *AttrData,
  OUT UINTN             *AttrDataSize
);


EFI_STATUS
EFIAPI
PldmGetBiosAttrPendingValueByType (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8             AttrType,
  OUT VOID              *AttrData,
  OUT UINTN             *AttrDataSize
);

EFI_STATUS
EFIAPI
PldmAcceptBiosAttrPendingValues (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT16            NumOfHandles,
  IN  UINT16            *AttrHandles
);

EFI_STATUS
EFIAPI
PldmPlatformEventMessage (
  IN  EFI_PLDM_PROTOCOL *This,
  IN  UINT8 FormatVersion,
  IN  UINT8 Tid,
  IN  UINT8 EventClass,
  IN  VOID  *EventData,
  IN  UINTN EventDataLen,
  OUT UINT8 *ReturnStatus
);

EFI_STATUS 
InitBiosTables (
PLDM_CONTEXT *PldmContext
);

EFI_STATUS 
EFIAPI
ResetBiosTables (
EFI_PLDM_PROTOCOL *This
);

VOID 
EFIAPI
AppendBiosString (
  EFI_PLDM_PROTOCOL *This,
  UINT16     StrHandle,
  UINTN      StrLength,
  CHAR8      *String
);

VOID 
EFIAPI
AppendBiosAttr (
  EFI_PLDM_PROTOCOL *This,
  UINT16     AttrHandle,
  UINT8      AttrType,
  UINT16     AttrNameHandle,
  UINTN      AttrDataSize,
  VOID       *AttrData
);

VOID 
EFIAPI
AppendBiosAttrValue (
  EFI_PLDM_PROTOCOL *This,
  UINT16     AttrHandle,
  UINT8      AttrType,
  UINTN      AttrValueDataSize,
  VOID       *AttrValueData
);

EFI_STATUS
EFIAPI
PushBiosTables (                                                                    
EFI_PLDM_PROTOCOL *Pldm
);


VOID
EFIAPI
PushManagementDataToMC(
  IN EFI_EVENT        Event,
  IN VOID             *Context
);

EFI_STATUS
EFIAPI
RegisterSensorReadingFunction (
  EFI_PLDM_PROTOCOL *This,
  EFI_EVENT_NOTIFY SensorReadingFun
);



#define PLDM_CONTEXT_FROM_THIS(a)     CR(a, PLDM_CONTEXT, Pldm, PLDM_SIGNATURE)

#endif
