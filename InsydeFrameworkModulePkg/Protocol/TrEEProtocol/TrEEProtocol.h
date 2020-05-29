//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _TREE_PROTOCOL_H
#define _TREE_PROTOCOL_H

#include EFI_PROTOCOL_DEFINITION (ServiceBinding)

#define EFI_TREE_SERVICE_BINDING_PROTOCOL_GUID \
  {0x4cf01d0a, 0xc48c, 0x4271, 0xa2, 0x2a, 0xad, 0x8e, 0x55, 0x97, 0x81, 0x88}

#define EFI_TREE_PROTOCOL_GUID \
  {0x607f766c, 0x7455, 0x42be, 0x93, 0x0b, 0xe4, 0xd7, 0x6d, 0xb2, 0x72, 0x0f}

typedef struct _EFI_TREE_PROTOCOL  EFI_TREE_PROTOCOL;

EFI_FORWARD_DECLARATION (EFI_TREE_PROTOCOL);

typedef struct _TREE_VERSION {
	UINT8                                   Major;
	UINT8                                   Minor;
} TREE_VERSION;

typedef UINT32                            TREE_EVENT_LOG_BITMAP;
typedef UINT32                            TREE_EVENT_LOG_FORMAT;
#define TREE_EVENT_LOG_FORMAT_TCG_1_2     0x00000001

#define TREE_STRUCTURE_VERSION_MAJOR      (1)
#define TREE_STRUCTURE_VERSION_MINOR      (0)
#define TREE_PROTOCOL_VERSION_MAJOR       (1)
#define TREE_PROTOCOL_VERSION_MINOR       (0)

typedef struct _TREE_BOOT_SERVICE_CAPABILITY {
	UINT8                                   Size;
	TREE_VERSION                            StructureVersion;
	TREE_VERSION                            ProtocolVersion;
	UINT32                                  HashAlgorithmBitmap;
	TREE_EVENT_LOG_BITMAP                   SupportedEventLogs;
	BOOLEAN                                 TrEEPresentFlag;
	UINT16                                  MaxCommandSize;
	UINT16                                  MaxResponseSize;
	UINT32                                  ManufacturerID;
} TREE_BOOT_SERVICE_CAPABILITY;

#define TREE_BOOT_HASH_ALG_SHA1           0x00000001
#define TREE_BOOT_HASH_ALG_SHA256         0x00000002
#define TREE_BOOT_HASH_ALG_SHA384         0x00000004
#define TREE_BOOT_HASH_ALG_SHA512         0x00000008
//
// This bit is shall be set when an event shall be extended but not logged.
//
#define TREE_EXTEND_ONLY                  0x0000000000000001
//
// This bit shall be set when the intent is to measure a PE/COFF image.
//
#define PE_COFF_IMAGE                     0x0000000000000010
#define EV_EFI_VARIABLE_AUTHORITY         (EV_EFI_EVENT_BASE + 0xE0)

typedef UINT32                            TREE_PCRINDEX;
typedef UINT32                            TREE_EVENTTYPE;
typedef UINT32                            TREE_EVENTLOGTYPE;

#pragma pack(1)
typedef struct _TREE_EVENT_HEADER {
	UINT32                                  HeaderSize;
	UINT16                                  HeaderVersion;
	TREE_PCRINDEX                           PCRIndex;
	TREE_EVENTTYPE                          EventType;
} TREE_EVENT_HEADER;

typedef struct _TREE_EVENT {
	UINT32                                  Size;
	TREE_EVENT_HEADER                       Header;
	UINT8                                   Event[1];
} TREE_EVENT;

typedef struct {
  EFI_GUID                          VariableName;
  UINT64                            UnicodeNameLength;   // The TCG Definition used UINTN
  UINT64                            VariableDataLength;  // The TCG Definition used UINTN
  CHAR16                            UnicodeName[1];
  INT8                              VariableData[1];
} EFI_VARIABLE_DATA_TREE;
#pragma pack()

typedef UINT32                            TCG_EVENTTYPE;
typedef UINT32                            TCG_PCRINDEX;

#define SIZE_OF_TREE_EVENT OFFSET_OF (TREE_EVENT, Event)

typedef
EFI_STATUS
(EFIAPI *EFI_TREE_GET_CAPABILITY) (
	IN EFI_TREE_PROTOCOL                    *This,
	IN OUT TREE_BOOT_SERVICE_CAPABILITY     *ProtocolCapability
);
/*++

  Routine Description:
    Provides protocol capability informationand state information about the TrEE

  Arguments:
    This                    - Indicates the calling context
    ProtocolCapability      - The callerallocates memory for a TREE_BOOT_SERVICE_CAPABILITY
                              structureand sets the size field to the size of the structure
                              allocated. The calleefills in the fields with the EFI protocol
                              capabilityinformation and the current TrEEstate informationup
                              to the number of fields which fit within the size of the structure
                              passedin

  Returns:
    EFI_SUCCESS             - Operation completed successfully.
    EFI_DEVICE_ERROR        - The command was unsuccessful. ProtocolCapabilityvariable will not
                              be populated.
    EFI_INVALID_PARAMETER   - One or more of the parameters are incorrect. The ProtocolCapability
                              variable will not be populated.
    EFI_BUFFER_TOO_SMALL    - The ProtocolCapabilityvariableis too smallto hold the full response.
                              It will be partially populated (required Sizefield will be set).

--*/

typedef
EFI_STATUS
(EFIAPI *EFI_TREE_GET_EVENT_LOG) (
	IN EFI_TREE_PROTOCOL                    *This,
	IN TREE_EVENTLOGTYPE                    EventLogFormat,
	OUT EFI_PHYSICAL_ADDRESS                *EventLogLocation,
	OUT EFI_PHYSICAL_ADDRESS                *EventLogLastEntry,
	OUT BOOLEAN                             *EventLogTruncated
);
/*++

  Routine Description:
    Retrieve the address of a given event logand its last entry

  Arguments:
    This                    - Indicates the calling context
    EventLogFormat          - The type of the event log for which the information is requested.
    EventLogLocation        - A pointer to the memory address of the event log.
    EventLogLastEntry       - If the Event Log contains more than one entry, this is a pointer
                              to the address of the start of the last entry in the event log in
                              memory. For information about what values are returned in this
                              parameter in the special cases of an empty Event Log or an Event
                              Log with only one entry, see the Description sectionbelow.
    EventLogTruncated       - If the Event Log is missing at least one entry because an event
                              would have exceeded the area allocated for events, this value is
                              set to TRUE.  Otherwise, the value will be FALSE and the Event Log
                              will be complete.

  Returns:
    EFI_SUCCESS             - Operation completed successfully.
    EFI_INVALID_PARAMETER   - One or more of the parameters are incorrect (e.g. asking for an event
                              log whose format is not supported).

--*/

typedef
EFI_STATUS
(EFIAPI * EFI_TREE_HASH_LOG_EXTEND_EVENT) (
	IN EFI_TREE_PROTOCOL                    *This,
	IN UINT64                               Flags,
	IN EFI_PHYSICAL_ADDRESS                 DataToHash,
	IN UINT64                               DataToHashLen,
	IN TREE_EVENT                           *TrEEEvent
);
/*++

  Routine Description:
    To extend and optionally log events without requiring knowledge of actual TPM commands.
    The extend operation will occur even if this function cannot create an event log entry
    (e.g. due to the event log being full).

  Arguments:
    This                    - Indicates the calling context
    Flags                   - Bitmap providing additional information.
    DataToHash              - Physical address of the start of the data buffer to be hashed.
    DataToHashLen           - The length in bytes of the buffer referenced by DataToHash.
    TrEEEvent               - Pointer to data buffer containing information about the event.

  Returns:
    EFI_SUCCESS             - Operation completed successfully.
    EFI_DEVICE_ERROR        - The command was unsuccessful.
    EFI_VOLUME_FULL         - The extend operation occurred, but the event could not be written
                              to one or more event logs.
    EFI_INVALID_PARAMETER   - One or more of the parameters are incorrect.
    EFI_UNSUPPORTED         - The PE/COFF image type is not supported.

--*/

typedef
EFI_STATUS
(EFIAPI *EFI_TREE_SUBMIT_COMMAND) (
	IN EFI_TREE_PROTOCOL                    *This,
	IN UINT32                               InputParameterBlockSize,
	IN UINT8                                *InputParameterBlock,
	IN UINT32                               OutputParameterBlockSize,
	IN UINT8                                *OutputParameterBlock
);
/*++

  Routine Description:
    This service enables the sending of commands to the TrEE.

  Arguments:
    This                    - Indicates the calling context
    InputParameterBlockSize - Size of the TrEE input parameter block.
    InputParameterBlock     - Pointer to the TrEE input parameter block.
    OutputParameterBlockSize- Size of the TrEE output parameter block.
    OutputParameterBlock    - Pointer to the TrEE output parameter block.

  Returns:
    EFI_SUCCESS             - The command byte stream was successfully sent to
                              the device and a response was successfully received.
    EFI_DEVICE_ERROR        - The command was not successfully sent to the device
                              or a response was not successfully received from the
                              device.
    EFI_INVALID_PARAMETER   - One or more of the parameters are incorrect.
    EFI_BUFFER_TOO_SMALL    - The output parameter blockis too small.

--*/

struct _EFI_TREE_PROTOCOL {
	EFI_TREE_GET_CAPABILITY                 GetCapability;
	EFI_TREE_GET_EVENT_LOG                  GetEventLog;
	EFI_TREE_HASH_LOG_EXTEND_EVENT          HashLogExtendEvent;
	EFI_TREE_SUBMIT_COMMAND                 SubmitCommand;
};

extern EFI_GUID gEfiTrEEProtocolGuid;
extern EFI_GUID gEfiTrEEServiceBindingProtocolGuid;

#endif //_TREE_PROTOCOL_H
