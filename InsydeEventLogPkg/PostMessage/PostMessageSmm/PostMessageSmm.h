/** @file

  Header file of POST Message SMM implementation.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _POST_MESSAGE_SMM_H_
#define _POST_MESSAGE_SMM_H_

//
// Statements that include other header files
//

#include <PostMessageHookSmm.h>

#include <Library/SmmServicesTableLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>

#include <Protocol/EventLog.h>
#include <Protocol/PostMessage.h>

#include <Library/BaseMemoryLib.h>
#include <Protocol/SmmReportStatusCodeHandler.h>
#include <Library/DebugLib.h>
#include <Guid/EventGroup.h>

//
// Define POST Message ('_PMSG_') signatures in F-Segment
// Show POST Message('_MPU') and All POST Message('_MPA')
//
#define POST_MESSAGE_INSTANCE_SIGNATURE         SIGNATURE_64('_', 'P', 'M', 'S', 'G', '_', 0, 0)
#define MESG_UPM_INSTANCE_SIGNATURE             SIGNATURE_32('U', 'P', 'M', '_')
#define MESG_APM_INSTANCE_SIGNATURE             SIGNATURE_32('A', 'P', 'M', '_')

#define MESG_TEMP_INSTANCE_SIGNATURE            SIGNATURE_32('_', 'T', 'M', 'P')

//
// define value for BMC
//
#define MAX_TEMP_DATA                           100
#define BIOS_SOFTWARE_ID                        0x01
#define EVENT_REV                               0x04

//
// Default Memory Size = 64K Byte
//
#define POST_MESSAGE_SIZE                      0x10000
#define SHOW_POST_MESSAGE_SIZE                 (POST_MESSAGE_SIZE/2)                          // UPM Size
#define ALL_POST_MESSAGE_SIZE                  (POST_MESSAGE_SIZE-SHOW_POST_MESSAGE_SIZE)     // APM Size                                                  BEHAVIOR_STORE_MESSAGE | BEHAVIOR_EVENT_BEEP)

//
// Ensure proper structure formats
//
#pragma pack(1)

typedef struct {
  UINT64                  PostMessageSignature;      // '_PMSG_'"
  EFI_PHYSICAL_ADDRESS    PostMessageHeadAddr;       // POST Message Memory buffer Entry Address
} POST_MESSAGE_F_SEGMENT_STRUCTURE;

typedef struct {
  UINT32                  UpmSignature;             // 'UPM_'
  UINT32                  UpmStartAddr;             // Show POST Message Start Address
  UINT32                  UpmEndAddr;               // Show POST Message End Address
  UINT32                  UpmSize;                              
  UINT32                  ApmSignature;             // 'APM_'
  UINT32                  ApmStartAddr;             // All POST Message Start Address
  UINT32                  ApmEndAddr;               // All POST Message End Address  
  UINT32                  ApmSize;
} POST_MESSAGE_HEAD_STRUCTURE;

typedef struct {
  UINT32                  TempSignature;             // '_TMP'
  UINT32                  TempStartAddr;             // POST Message Temp Buffer Start Address
  UINT32                  TempEndAddr;               // POST Message Temp Buffer End Address
  UINT32                  TempSize;                
} TEMP_BUFFER_HEAD_STRUCTURE;

typedef struct {
  EFI_STATUS_CODE_TYPE           CodeType;
  EFI_STATUS_CODE_VALUE          CodeValue;           
} TEMP_BUFFER_STRUCTURE;

#pragma pack()


EFI_STATUS
EFIAPI
PostMessageStatusCode (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue,
  IN UINT32                         Instance    OPTIONAL,
  IN EFI_GUID                       * CallerId  OPTIONAL,
  IN EFI_STATUS_CODE_DATA           * Data      OPTIONAL  
  );

EFI_STATUS
EFIAPI
LogPostMessage (
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          CodeValue
  );

static
VOID
EFIAPI
RscHandlerProtocolCallback (
  IN EFI_EVENT   Event,
  IN VOID        *Context
  );

#endif
