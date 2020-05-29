/** @file

  The header file for Ftpm SMM driver.

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

#ifndef __FTPM_SMM_H__
#define __FTPM_SMM_H__

#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>

#include <Protocol/SmmSwDispatch.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/MeGlobalNvsArea.h>
#include <Guid/MeSsdtTableStorage.h>

///
/// Below definition is generic, but NOT in GreenH
///
#include <Protocol/SmmVariable.h>

#ifndef EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE
#define EFI_ACPI_5_0_TRUSTED_COMPUTING_PLATFORM_2_TABLE_SIGNATURE  SIGNATURE_32('T', 'P', 'M', '2')
#endif

#include <IndustryStandard/Tpm2Acpi.h>
#include <Guid/TrEEPhysicalPresenceData.h>
#include <Guid/MemoryOverwriteControl.h>

//
// Below definition is chipset specific
//
#include <Guid/MeDataHob.h>
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
#include <Library/Ptt/Include/PttHciRegs.h>

//
// Below definition is driver specific
//
#include "Tpm2AcpiTableStorage.h"

#pragma pack(1)
typedef struct {
  UINT8                  SoftwareSmi;
  UINT32                 Parameter;
  UINT32                 Response;
  UINT32                 Request;
  UINT32                 LastRequest;
  UINT32                 ReturnCode;
} PHYSICAL_PRESENCE_NVS;

typedef struct {
  UINT8                  SoftwareSmi;
  UINT32                 Parameter;
  UINT32                 Request;
  UINT32                 ReturnCode;
} MEMORY_CLEAR_NVS;

typedef struct {
  UINT8                  SoftwareSmi;
  UINT32                 ReturnCode;
} START_METHOD_NVS;

typedef struct {
  PHYSICAL_PRESENCE_NVS  PhysicalPresence;
  MEMORY_CLEAR_NVS       MemoryClear;
  START_METHOD_NVS       StartMethod;
} TCG_NVS;

typedef struct {
  UINT8                  OpRegionOp;
  UINT32                 NameString;
  UINT8                  RegionSpace;
  UINT8                  DWordPrefix;
  UINT32                 RegionOffset;
  UINT8                  BytePrefix;
  UINT8                  RegionLen;
} AML_OP_REGION_32_8;
#pragma pack()

//
// The definition for TCG physical presence ACPI function
//
#define ACPI_FUNCTION_GET_PHYSICAL_PRESENCE_INTERFACE_VERSION      1
#define ACPI_FUNCTION_SUBMIT_REQUEST_TO_BIOS                       2
#define ACPI_FUNCTION_GET_PENDING_REQUEST_BY_OS                    3
#define ACPI_FUNCTION_GET_PLATFORM_ACTION_TO_TRANSITION_TO_BIOS    4
#define ACPI_FUNCTION_RETURN_REQUEST_RESPONSE_TO_OS                5
#define ACPI_FUNCTION_SUBMIT_PREFERRED_USER_LANGUAGE               6
#define ACPI_FUNCTION_SUBMIT_REQUEST_TO_BIOS_2                     7
#define ACPI_FUNCTION_GET_USER_CONFIRMATION_STATUS_FOR_REQUEST     8

//
// The return code for Get User Confirmation Status for Operation
//
#define PP_REQUEST_NOT_IMPLEMENTED                                 0
#define PP_REQUEST_BIOS_ONLY                                       1
#define PP_REQUEST_BLOCKED                                         2
#define PP_REQUEST_ALLOWED_AND_PPUSER_REQUIRED                     3
#define PP_REQUEST_ALLOWED_AND_PPUSER_NOT_REQUIRED                 4

//
// The return code for Sumbit TPM Request to Pre-OS Environment
// and Sumbit TPM Request to Pre-OS Environment 2
//
#define PP_SUBMIT_REQUEST_SUCCESS                                  0
#define PP_SUBMIT_REQUEST_NOT_IMPLEMENTED                          1
#define PP_SUBMIT_REQUEST_GENERAL_FAILURE                          2
#define PP_SUBMIT_REQUEST_BLOCKED_BY_BIOS_SETTINGS                 3

//
// The definition for TCG MOR
//
#define ACPI_FUNCTION_DSM_MEMORY_CLEAR_INTERFACE                   1
#define ACPI_FUNCTION_PTS_CLEAR_MOR_BIT                            2

//
// The return code for Memory Clear Interface Functions
//
#define MOR_REQUEST_SUCCESS                                        0
#define MOR_REQUEST_GENERAL_FAILURE                                1

//
// Below definition should be in platorm scope
//
// TBD: Use policy to input these data...
#ifndef EFI_TPM2_PP_SW_SMI
#define EFI_TPM2_PP_SW_SMI     0x9E
#endif
#ifndef EFI_TPM2_MOR_SW_SMI
#define EFI_TPM2_MOR_SW_SMI    0x9F
#endif

#endif  // __FTPM_SMM_H__
