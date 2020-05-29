/** @file
  Management Component Transport Protocol definition file

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

#ifndef _MCTP_H_
#define _MCTP_H_

#include <Uefi.h>

#define EFI_MCTP_PROTOCOL_GUID \
  { 0xb0c97127, 0xf863, 0x489c, 0x97, 0x35, 0x40, 0x67, 0x6a, 0x7a, 0xb7, 0x66 }
  

typedef struct _EFI_MCTP_PROTOCOL EFI_MCTP_PROTOCOL;
typedef struct _EFI_MCTP_BUS_PROTOCOL EFI_MCTP_BUS_PROTOCOL;

#define MCTP_MESSAGE_TYPE_MCTP_CONTROL            0x00
#define MCTP_MESSAGE_TYPE_PLDM                    0x01


#define MCTP_CONTROL_SET_ENDPOINT_ID              0x01
#define MCTP_CONTROL_GET_ENDPOINT_ID              0x02
#define MCTP_CONTROL_GET_ENDPOINT_UUID            0x03
#define MCTP_CONTROL_GET_MCTP_VERSION_SUPPORT     0x04
#define MCTP_CONTROL_GET_MESSAGE_TYPE_SUPPORT     0x05
#define MCTP_CONTROL_RESOLVE_ENDPOINT_ID          0x07
#define MCTP_CONTROL_ALLOCATE_ENDPOINT_IDS        0x08
#define MCTP_CONTROL_ROUTING_INFO_UPDATE          0x09
#define MCTP_CONTROL_GET_ROUTING_TABLE_ENTRIES    0x0a
#define MCTP_CONTROL_PREPARE_ENDPOINT_DISCOVERY   0x0b
#define MCTP_CONTROL_ENDPOINT_DISCOVERY           0x0c
#define MCTP_CONTROL_DISCOVERY_NOTIFY             0x0d
#define MCTP_CONTROL_QUERY_HOP                    0x0f

#define MCTP_REQUEST_BIT                          0x80
#define MCTP_DATAGRAM_BIT                         0x40


#define MCTP_ERROR                                0x01
#define MCTP_ERROR_INVALID_DATA                   0x02
#define MCTP_ERROR_INVALID_LENGTH                 0x03
#define MCTP_ERROR_NOT_READY                      0x04
#define MCTP_ERROR_UNSUPPORTED_COMMAND            0x05

#define MCTP_BASELINE_TRANSMISSION_UNIT           64
#define MCTP_HEADER_VERSION                       0x01

#define EFI_MCTP_COMPLETION_ERROR                            EFIERR(0xA001)
#define EFI_PLDM_COMPLETION_ERROR                            EFIERR(0xA002)

#define EFI_ERROR_RETURN(Status)   if(EFI_ERROR(Status)){ return Status; }

#pragma pack (1)

typedef struct _MCTP_HEADER {
  UINT8         HeaderVersion    :4;
  UINT8         Reserved         :4;
  UINT8         DestinationEid;
  UINT8         SourceEid;
  UINT8         MessageTag       :3;
  UINT8         TagOwner         :1;
  UINT8         PacketSequence   :2;
  UINT8         EndOfMessage     :1;
  UINT8         StartOfMessage   :1;
} MCTP_HEADER;

typedef struct {
  UINT8         MessageType;
  UINT8         InstanceId       :5;
  UINT8         Reserved         :1;
  UINT8         Datagram         :1;
  UINT8         Request          :1;
  UINT8         CommandCode;
} MCTP_CONTROL_HEADER;

typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         MessageData[MCTP_BASELINE_TRANSMISSION_UNIT - 3];
} MCTP_CONTROL_REQUEST_DATA;

typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         CompletionCode;
  UINT8         MessageData[MCTP_BASELINE_TRANSMISSION_UNIT - 4];
} MCTP_CONTROL_RESPONSE_DATA;

typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         Operation;
  UINT8         EndpointId;
} MCTP_SET_EID_REQUEST_DATA;

typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         CompletionCode;
  UINT8         AllocationStatus :2;
  UINT8         Reserved1        :2;
  UINT8         AssignmentStatus :2;
  UINT8         Reserved2        :2;
  UINT8         EidSetting;
  UINT8         EidPoolSize;
} MCTP_SET_EID_RESPONSE_DATA;


typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         CompletionCode;
  UINT8         EndpointId;
  UINT8         EndpointType;
  UINT8         MediumSpecific;
} MCTP_GET_EID_RESPONSE_DATA;


typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         CompletionCode;
  EFI_GUID      Uuid;
} MCTP_GET_ENDPOINT_UUID_RESPONSE_DATA;


typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         MessageType;
} MCTP_GET_VERSION_REQUEST_DATA;

typedef struct {
  UINT8         Alpha;
  UINT8         Update;
  UINT8         Minor;
  UINT8         Major;
} MCTP_VERSION_TYPE;

typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         CompletionCode;
  UINT8         EntryCount;
  MCTP_VERSION_TYPE SupportedVersions[1];
} MCTP_GET_VERSION_RESPONSE_DATA;



typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         CompletionCode;
  UINT8         EntryCount;
  UINT8         SupportedTypes[2];
} MCTP_GET_MESSAGE_TYPE_SUPPORT_RESPONSE_DATA;

typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         TargeEid;
} MCTP_RESOLVE_EID_REQUEST_DATA;

typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         CompletionCode;
  UINT8         BridgeEid;
  UINT8         PhysicalAddressBuf[1];
} MCTP_RESOLVE_EID_RESPONSE_DATA;


typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         OperationFlag;
  UINT8         EidCount;
  UINT8         StartingEid;
} MCTP_ALLOCATE_EID_REQUEST_DATA;

typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         CompletionCode;
  UINT8         AllocationStatus;
  UINT8         EidPoolSize;
  UINT8         FirstEid; 
} MCTP_ALLOCATE_EID_RESPONSE_DATA;

typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         CompletionCode;
} MCTP_PREPARE_EID_DISCOVERY_RESPONSE_DATA;

typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         CompletionCode;
} MCTP_DISCOVERY_NOTIFY_RESPONSE_DATA;

typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         TargeEid;
  UINT8         RequestedMessageType;
} MCTP_QUERY_HOP_REQUEST_DATA;

typedef struct {
  MCTP_CONTROL_HEADER Header;
  UINT8         CompletionCode;
  UINT8         NextBridgeEid;
  UINT8         MessageType;
  UINT16        MaxIncommingUnitSize;
  UINT16        MaxOutgoingUnitSize;
} MCTP_QUERY_HOP_RESPONSE;


#pragma pack ()


typedef
EFI_STATUS
(EFIAPI *MCTP_SET_MCTP_BUS) (
  IN EFI_MCTP_PROTOCOL                   *This,
  IN EFI_MCTP_BUS_PROTOCOL               *MctpBus
);

typedef
EFI_STATUS
(EFIAPI *MCTP_SET_DESTINATION_EID) (
  IN EFI_MCTP_PROTOCOL                   *This,
  IN UINT8                               DestinationEid
);

typedef
VOID
(EFIAPI *MCTP_SET_REQUEST_MODE) (
  IN EFI_MCTP_PROTOCOL *This
);

typedef
VOID
(EFIAPI *MCTP_SET_RESPONSE_MODE) (
  IN EFI_MCTP_PROTOCOL *This
);



typedef
BOOLEAN
(EFIAPI *MCTP_NEW_MESSAGE_RECEIVED) (
  IN EFI_MCTP_PROTOCOL *This
);

typedef 
EFI_STATUS
(EFIAPI *MCTP_SEND_MESSAGE) (
  IN EFI_MCTP_PROTOCOL *This,
  IN VOID              *Message,
  IN UINTN             MessageSize
);

typedef 
EFI_STATUS
(EFIAPI *MCTP_RECEIVE_MESSAGE) (
  IN EFI_MCTP_PROTOCOL *This,
  OUT VOID             *Message,
  OUT UINTN            *MessageSize
);

typedef
EFI_STATUS
(EFIAPI *MCTP_SET_ENDPOINT_ID_COMMAND) (
  IN  EFI_MCTP_PROTOCOL          *This,
  IN  VOID                       *Address,
  IN  UINT8                      Operation,
  IN  UINT8                      EndpointId,
  OUT MCTP_SET_EID_RESPONSE_DATA *Response
);

typedef
EFI_STATUS
(EFIAPI *MCTP_GET_ENDPOINT_ID_COMMAND) (
  IN  EFI_MCTP_PROTOCOL          *This,
  IN  VOID                       *Address,
  OUT MCTP_GET_EID_RESPONSE_DATA *Response
);

typedef
EFI_STATUS
(EFIAPI *MCTP_GET_VERSION_SUPPORT_COMMAND) (
  IN  EFI_MCTP_PROTOCOL              *This,
  IN  UINT8                          MessageType,
  OUT MCTP_GET_VERSION_RESPONSE_DATA *Response
);

typedef
EFI_STATUS
(EFIAPI *MCTP_GET_MESSAGE_TYPE_SUPPORT_COMMAND) (
  IN  EFI_MCTP_PROTOCOL     *This,
  OUT MCTP_GET_MESSAGE_TYPE_SUPPORT_RESPONSE_DATA *Response
);

struct _EFI_MCTP_PROTOCOL {
  MCTP_SET_MCTP_BUS                     SetMctpBus;
  MCTP_SET_REQUEST_MODE                 SetRequestMode;
  MCTP_SET_RESPONSE_MODE                SetResponseMode;  
  MCTP_SET_DESTINATION_EID              SetDestinationEid;
  MCTP_NEW_MESSAGE_RECEIVED             NewMessageReceived;
  MCTP_SEND_MESSAGE                     SendMessage;
  MCTP_RECEIVE_MESSAGE                  ReceiveMessage;
  MCTP_SET_ENDPOINT_ID_COMMAND          SetEndpointId;
  MCTP_GET_ENDPOINT_ID_COMMAND          GetEndpointId;
  MCTP_GET_VERSION_SUPPORT_COMMAND      GetVersionSupport;
  MCTP_GET_MESSAGE_TYPE_SUPPORT_COMMAND GetMessageTypeSupport;
};

extern EFI_GUID gMctpProtocolGuid;

#endif
