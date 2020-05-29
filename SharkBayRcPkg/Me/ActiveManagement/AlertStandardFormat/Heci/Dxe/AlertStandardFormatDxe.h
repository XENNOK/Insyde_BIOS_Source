/** @file

  Include file for ASF Driver

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


#ifndef _ALERT_STANDARD_FORMAT_H
#define _ALERT_STANDARD_FORMAT_H

#include "../Common/AlertStandardFormatCommon.h"

#include <PiPei.h>
#include <Library/AslUpdateLib.h>
#include <IndustryStandard/AlertStandardFormatTable.h>

#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/AslUpdateLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiLib.h>
#include <Library/HobLib.h>
#include <Library/Amt/Dxe/AmtLib.h>
#include <Library/MeKernel/Dxe/MeLib.h>

#include <Guid/DataHubStatusCodeRecord.h>
#include <Guid/DebugMask.h>
#include <Guid/HobList.h>
#include <Guid/AmtForcePushPetPolicy.h>
#include <Guid/ConsoleLock.h>
#include <Ppi/AmtStatusCode.h>

///
/// Driver Consumed Protocol Prototypes
///
#include <Protocol/AlertStandardFormat.h>
#include <Protocol/AcpiSupport.h>
#include <Protocol/DataHub.h>
#include <Protocol/Runtime.h>
#include <Protocol/Heci.h>

//[-start-120806-IB05330368-add]//
#include <Protocol/AcpiTable.h>
//[-end-120806-IB05330368-add]//

///
/// ASF Over HECI
///
typedef struct _HECI_ASF_PUSH_PROGRESS_CODE {
  UINT8           Command;
  UINT8           ByteCount;
  EFI_ASF_MESSAGE AsfMessage;
  UINT8           EventData[3];
  UINT8           Reserved[2];
} HECI_ASF_PUSH_PROGRESS_CODE;

#define HECI_ASF_PUSH_PROGRESS_CODE_LENGTH  0x12

typedef struct _HECI_ASF_GET_BOOT_OPTIONS {
  UINT8 Command;
  UINT8 ByteCount;
  UINT8 SubCommand;
  UINT8 VersionNumber;
} HECI_ASF_GET_BOOT_OPTIONS;

#define HECI_ASF_GET_BOOT_OPTIONS_LENGTH  0x04

typedef struct _HECI_ASF_GET_BOOT_OPTIONS_RESPONSE {
  UINT8                 Command;
  UINT8                 ByteCount;
  EFI_ASF_BOOT_OPTIONS  AsfBootOptions;
  UINT8                 Reserved;
} HECI_ASF_GET_BOOT_OPTIONS_RESPONSE;

#define HECI_ASF_GET_BOOT_OPTIONS_RESPONSE_LENGTH 0x0F

typedef struct _HECI_ASF_CLEAR_BOOT_OPTION {
  UINT8                       Command;
  UINT8                       ByteCount;
  EFI_ASF_CLEAR_BOOT_OPTIONS  AsfClearBootOptions;
} HECI_ASF_CLEAR_BOOT_OPTION;

#define HECI_ASF_CLEAR_BOOT_OPTION_LENGTH 0x04

typedef enum _HASFM_COMMAND_CODE
{
  ASF_MESSAGING_CMD             = 0x04,
  ASF_PUSH_PROGESS_CODE_SUBCMD  = 0x12,
  ASF_MENAGEMENT_CONTROL        = 0x02,
  ASF_WDT_START_SUBCMD          = 0x13,
  ASF_WDT_STOP_SUBCMD           = 0x14,
  ASF_CONFIGURATION_CMD         = 0x03,
  ASF_CLEAR_BOOT_OPTION_SUBCMD  = 0x15,
  ASF_RETURN_BOOT_OPTION_SUBCMD = 0x16,
  ASF_NO_BOOT_OPTION_SUBCMD     = 0x17
} HASFM_COMMAND_CODE;

typedef struct {
  EFI_FRAMEWORK_MESSAGE_TYPE  MessageType;
  EFI_ASF_MESSAGE             Message;
} EFI_ASF_FRAMEWORK_MESSAGE;

typedef struct {
  EFI_FRAMEWORK_MESSAGE_TYPE  MessageType;
  EFI_STATUS_CODE_VALUE       StatusCodeValue;
} EFI_ASF_DATA_HUB_MAP;

#define ALERT_STANDARD_FORMAT_PRIVATE_DATA_SIGNATURE  SIGNATURE_32 ('a', 's', 'f', 'd')

///
/// Declare a local instance structure for this driver
///
typedef struct _ALERT_STANDARD_FORMAT_INSTANCE {
  UINTN                           Signature;
  EFI_HANDLE                      Handle;

  ///
  /// Published interface
  ///
  EFI_ALERT_STANDARD_FORMAT_PROTOCOL  AlertStandardFormat;

} ALERT_STANDARD_FORMAT_INSTANCE;

#define EFI_CONSOLE_OUT_DEVICE_GUID \
  { 0xD3B36F2C, 0xD551, 0x11D4, { 0x9A, 0x46, 0x00, 0x90, 0x27, 0x3F, 0xC1, 0x4D }}

#define AMT_FORCE_PUSH_PET_VARIABLE_GUID \
  { 0xD7AC94AF, 0xA498, 0x45EC, { 0xbf, 0xA2, 0xA5, 0x6E, 0x95, 0x34, 0x61, 0x8B }}

#define AMT_FORCE_PUSH_PET_VARIABLE_NAME  L"AmtForcePushErrorPET"

#define AMT_PET_QUEUE_NODE_SIGNATURE      SIGNATURE_32 ('A', 'M', 'T', 'Q')

typedef struct _AMT_PET_QUEUE_NODE {
  UINT32                Signature;
  EFI_LIST_ENTRY        Link;
  EFI_STATUS_CODE_VALUE Value;
  EFI_STATUS_CODE_TYPE  Type;
} AMT_PET_QUEUE_NODE;

#define AMT_PET_QUEUE_NODE_FROM_LINK(_node) CR (_node, AMT_PET_QUEUE_NODE, Link, AMT_PET_QUEUE_NODE_SIGNATURE)

typedef struct {
  EFI_LIST_ENTRY  MessageList;
} AMT_PET_QUEUE_PROTOCOL;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER          Header;
  EFI_ACPI_ASF_INFO                    AsfInfo;
  EFI_ACPI_ASF_ALRT                    AsfAlert;
  EFI_ACPI_ASF_RCTL                    AsfRctl;
  EFI_ACPI_ASF_RMCP                    AsfRmcp;
  EFI_ACPI_ASF_ADDR                    AsfAddr;
} EFI_ACPI_1_0_ASF_DESCRIPTION_TABLE;

///
/// Prototypes
///

/**
  The driver entry point - detect ASF support or not, if support, will install relative protocol.

  @param[in] ImageHandle          Handle for this drivers loaded image protocol.
  @param[in] SystemTable          EFI system table.

  @retval EFI_SUCCESS             The driver installed without error.
  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures.
**/
EFI_STATUS
EFIAPI
AlertStandardFormatDriverEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  );

/**
  Return the SMBus address used by the ASF driver.
  Not applicable in Intel ME/HECI system, need to return EFI_UNSUPPORTED.

  @param[in] This                 The address of protocol
  @param[in] SmbusDeviceAddress   Out put the Smbus Address

  @exception EFI_UNSUPPORTED      The function is unsupported by this driver
**/
EFI_STATUS
EFIAPI
GetSmbusAddr (
  IN  EFI_ALERT_STANDARD_FORMAT_PROTOCOL   *This,
  OUT UINTN                                *SmbusDeviceAddress
  );

/**
  Set the SMBus address used by the ASF driver. 0 is an invalid address.
  Not applicable in Intel ME/HECI system, need to return EFI_UNSUPPORTED.

  @param[in] This                 The address of protocol
  @param[in] SmbusDeviceAddress   SMBus address of the device

  @exception EFI_UNSUPPORTED      The function is unsupported by this driver
**/
EFI_STATUS
EFIAPI
SetSmbusAddr (
  IN  EFI_ALERT_STANDARD_FORMAT_PROTOCOL   *This,
  IN  UINTN                                SmbusDeviceAddress
  );

/**
  Return the ASF Boot Options obtained from the controller. If the
  Boot Options parameter is NULL and no boot options have been retrieved,
  Query the ASF controller for its boot options.
  Get ASF Boot Options through HECI.

  @param[in] This                 The address of protocol
  @param[in] AsfBootOptions       Pointer to ASF boot options to copy current ASF Boot options

  @retval EFI_SUCCESS             Boot options copied
  @retval EFI_NOT_READY           No boot options
**/
EFI_STATUS
EFIAPI
GetBootOptions (
  IN      EFI_ALERT_STANDARD_FORMAT_PROTOCOL   *This,
  IN  OUT EFI_ASF_BOOT_OPTIONS                 **AsfBootOptions
  );

/**
  Send ASF Message through HECI.

  @param[in] This                 The address of protocol
  @param[in] AsfMessage           Pointer to ASF message

  @retval EFI_SUCCESS             Boot options copied
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_NOT_READY           No controller

**/
EFI_STATUS
EFIAPI
SendAsfMessage (
  IN  EFI_ALERT_STANDARD_FORMAT_PROTOCOL   *This,
  IN  EFI_ASF_MESSAGE                      *AsfMessage
  );

/**
  This routine returns ForcePushPetPolicy information.

  @param[in] None

  @retval AMT_FORCE_PUSH_PET_POLICY_HOB   ForcePushPetPolicy information.
**/
AMT_FORCE_PUSH_PET_POLICY_HOB *
GetForcePushPetPolicy (
  VOID
  );

/**
  Filters all the progress and error codes for Asf.

  @param[in] Event                The event registered.
  @param[in] Context              Event context. Not used in this event handler.
**/
VOID
EFIAPI
DataHubEventCallback (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  );

/**
  Sends a POST packet across ASF

  @param[in] MessageType          POST Status Code

  @retval EFI_DEVICE_ERROR        No message found
  @retval EFI_SUCCESS             Boot options copied
  @retval EFI_INVALID_PARAMETER   Invalid pointer
  @retval EFI_NOT_READY           No controller

**/
EFI_STATUS
SendPostPacket (
  IN  EFI_FRAMEWORK_MESSAGE_TYPE      MessageType
  );

/**
  This routine sends PET message in MessageQueue.

  @param[in] PeiServices          PeiServices pointer.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_NOT_READY           No controller
**/
EFI_STATUS
SendPETMessageInQueue (
  VOID
  );

/**
  This routine creats PET MessageQueue.

  @param[in] None

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
AmtCreateMessageQueue (
  VOID
  );

/**
  This routine saves current ForcePush ErrorEvent to Variable, which will be sent again.

  @param[in] MessageType          ASF PET message type.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures
**/
EFI_STATUS
SaveForcePushErrorEvent (
  IN  EFI_FRAMEWORK_MESSAGE_TYPE  MessageType
  );

/**
  This routine converts Hob ForcePush ErrorEvent to Variable, which will be sent again.

  @param[in] None

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
SaveForcePushErrorEventFromPeiToDxe (
  VOID
  );

#endif
