/** @file
  Header file for SA Initialization Driver.

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/
#ifndef _SA_INITIALIZATION_DRIVER_H_
#define _SA_INITIALIZATION_DRIVER_H_

///
/// External include files do NOT need to be explicitly specified in real EDKII
/// environment
///
//f !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
//nclude "EdkIIGlueDxe.h"
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/IoLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>

#include <EfiScriptLib.h>
#include <SaBuildFlags.h>
#include "LegacyRegion.h"
#include "VTd.h"
#include "GraphicsInit.h"
#include "IgdOpRegion.h"
#include "PciExpressInit.h"
#include "AudioInit.h"
#include <RcFviDxeLib.h>
#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>
#include "PcieComplex.h"
#include "VTd.h"
//[-start-140227-IB10920078-add]//
#include "SwitchableGraphicsInit.h"
//[-end-140227-IB10920078-add]//

///
/// Driver Consumed Protocol Prototypes
///
#include <Protocol/SaPlatformPolicy.h>
#include <Protocol/SaInfo.h>
//#endif
///
/// Data definitions
///
#define CRID_DATA 0x69
#define CRID_LOCK 0x17

typedef struct {
  UINT64  BaseAddr;
  UINT32  Offset;
  UINT32  AndMask;
  UINT32  OrMask;
} BOOT_SCRIPT_REGISTER_SETTING;

typedef struct {
  UINT64                Address;
  EFI_BOOT_SCRIPT_WIDTH Width;
  UINT32                Value;
} BOOT_SCRIPT_PCI_REGISTER_SAVE;

typedef struct {
  EFI_SA_INFO_PROTOCOL  SaInfo;
} SA_INSTANCE_PRIVATE_DATA;

typedef struct {
  UINT8 DeviceNumber;
  UINT8 FunctionNumber;
  UINT8 SvidRegOffset;
} SA_SVID_SID_INIT_ENTRY;

#define SA_FVI_STRING          "Reference Code - SA - System Agent"
#define SA_FVI_SMBIOS_TYPE     0xDD  ///< Default value
#define SA_FVI_SMBIOS_INSTANCE 0x06
#define MEM_FVI_STRING         "Reference Code - MRC"
#define MEM_RC_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }
#define PCIE_FVI_STRING "SA - PCIe Version"
#define PCIE_RC_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }
#define SA_CRID_STATUS          "SA-CRID Status"
#define SA_CRID_ORIGINAL_VALUE  "SA-CRID Original Value"
#define SA_CRID_NEW_VALUE       "SA-CRID New Value"
#define SA_CRID_ENABLED         "Enabled "
#define SA_CRID_DISABLED        "Disabled"
#define SA_CRID_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }
#define VBIOS_FVI_STRING        "OPROM - VBIOS"
#define VBIOS_RC_VERSION \
  { \
    0xFF, 0xFF, 0xFF, 0xFFFF \
  }

enum {
  SA_RC_VER     = 0,
  MEM_RC_VER,
  PCIE_VER,
  CRID_STATUS,
  CRID_ORIGINAL,
  CRID_NEW,
  VBIOS_VER
} SA_FVI_INDEX;

extern FVI_ELEMENT_AND_FUNCTION       mSaFviElementsData[];
extern FVI_DATA_HUB_CALLBACK_CONTEXT  mSaFviVersionData;
extern UINTN                          mSaFviElements;

///
/// Function Prototype
///
VOID
EFIAPI
SaPciEnumCompleteCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
/**
  This function gets registered as a callback to perform SA initialization before ExitPmAuth

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.

  @retval EFI_SUCCESS   - Always.

  **/
;

VOID
EFIAPI
SaExitPmAuthCallback (
  IN EFI_EVENT Event,
  IN VOID      *Context
  )
/**
  This function gets registered as a callback to perform SA configuration security lock

  @param[in] Event     - A pointer to the Event that triggered the callback.
  @param[in] Context   - A pointer to private data registered with the callback function.

  @retval EFI_SUCCESS   - Always.

  **/
;

VOID
DeviceConfigure (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
  This function performs SA internal devices enabling/disabling

  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol

  **/
;

VOID
ProgramSvidSid (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
/**
    Program SA devices Subsystem Vendor Identifier (SVID) and Subsystem Identifier (SID).

    @param[in] DxePlatformSaPolicy   The SAPlatform Policy protocol instance
**/
;

VOID
SaDxePolicyDump (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *mDxePlatformSaPolicy
  )
/**
  This function prints the DXE phase platform policy.

  @param[in] mDxePlatformSaPolicy - SA DxePlatformPolicy protocol

  **/
;

#endif
