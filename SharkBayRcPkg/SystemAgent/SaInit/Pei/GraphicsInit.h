/** @file

  Graphics header file

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
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

*/

#ifndef _GRAPHICS_INIT_H_
#define _GRAPHICS_INIT_H_

///===============================================
///  MdePkg/Include/
///===============================================
#include <IndustryStandard/Pci.h>
#include <Library/DebugLib.h>
#include <Ppi/Stall.h>

///
/// Driver Consumed PPI Prototypes
///
///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelMch/SystemAgent/
///===============================================
#include <Ppi/SaPlatformPolicy.h>

///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelPch/LynxPoint/Include/
///===============================================
#include <PchRegs.h>
///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelPch/LynxPoint/Include/Library/
///===============================================
#include <PchPlatformLib.h>

#include <CpuRegs.h>
#include <Library/CpuPlatformLib.h>

typedef struct {
  UINT8   Bus;
  UINT8   Device;
  UINT8   Function;
  UINT16  DevenMask;
} PEG_DEVEN;

typedef enum {
  IGD               = 0,
  PEG,
  PCI,
  DISPLAY_DEVICE_MAX
} DISPLAY_DEVICE;

typedef enum {
  VBIOS_DEFAULT     = 0,
  CRT,
  LFP,
  CRT_LFP,
  TV,
  LFPSDVO,
  EFP,
  TVSDVO,
  CRT_LFPSDVO,
  CRT_EFP,
  IGD_BOOT_TYPE_MAX
} IGD_BOOT_TYPE;

typedef enum {
  GMS_FIXED         = 0,
  GMS_DVMT,
  GMS_FIXED_DVMT,
  GMS_MAX
} GRAPHICS_MEMORY_SELECTION;

typedef enum {
  GM_32M            = 1,
  GM_64M            = 2,
  GM_128M           = 4,
  GM_MAX
} STOLEN_MEMORY;

typedef enum {
  PAVP_DISABLED     = 0,
  PAVP_LITE,
  PAVP_HIGH
} PAVP_MODE;

#define GTTMMADR_SIZE_4MB   0x400000
#define GTT_SIZE_2MB        2
#define GT_WAIT_TIMEOUT     3000     ///< ~3 seconds

#define APERTURE_SIZE_128MB 1
#define APERTURE_SIZE_256MB 2
#define APERTURE_SIZE_512MB 3

/**
  CheckAndInitializePegVga:  Check if PEG card is present and configure accordingly

  @param[in] PeiServices             - Pointer to the PEI services table
  @param[in] AlwaysEnablePeg         - 0 - Peg is not always enabled.
  @param[in, out] PrimaryDisplay     - Primary Display - default is IGD
  @param[in] PrimaryDisplaySelection - Primary display selection from BIOS Setup
  @param[in, out] PegMmioLength      - Total PEG MMIO length on all PEG ports
**/
VOID
CheckAndInitializePegVga (
  IN CONST EFI_PEI_SERVICES                 **PeiServices,
  IN     UINT8                              AlwaysEnablePeg,
  IN OUT DISPLAY_DEVICE                     *PrimaryDisplay,
  IN     UINT8                              PrimaryDisplaySelection,
  IN OUT UINT32                             *PegMmioLength
  )
;

/**
  CheckOffboardPciVga: Check if off board PCI graphics Card is present

  @param[in] PeiServices          - Pointer to the PEI services table
  @param[in, out] PciMmioLength   - PCI MMIO length
  @param[in, out] PrimaryDisplay  - Primary Display - default is IGD
**/
VOID
CheckOffboardPciVga (
  IN CONST EFI_PEI_SERVICES                 **PeiServices,
  IN OUT UINT32                             *PciMmioLength,
  IN OUT DISPLAY_DEVICE                     *PrimaryDisplay
  )
;

/**
  CheckOffboardPcieVga: Check if off board PCIe graphics Card is present

  @param[in] PeiServices             - Pointer to the PEI services table
  @param[in, out] PchPcieMmioLength  - Total PCIe MMIO length on all PCH root ports
  @param[in, out] PrimaryDisplay     - Primary Display - default is IGD
**/
VOID
CheckOffboardPcieVga (
  IN CONST EFI_PEI_SERVICES   **PeiServices,
  IN OUT UINT32               *PchPcieMmioLength,
  IN OUT DISPLAY_DEVICE       *PrimaryDisplay
  )
;

/**
  Find the MMIO size that a given PCI device requires

  @param[in] BusNum      - Bus number of the device
  @param[in] DevNum      - device Number of the device
  @param[in] FunNum      - Function number of the device
  @param[out] MmioLength - MMIO Length in bytes
**/
VOID
FindPciDeviceMmioLength (
  IN UINT32  BusNum,
  IN UINT32  DevNum,
  IN UINT32  FunNum,
  OUT UINT32 *MmioLength
  )
;

/**
  This function enumerate all downstream bridge.

  @param[in] BusNum  - Primary bus number of current bridge

  @retval Current bus number: if current bus is an enpoint device
  @retval subordinate bus number: if current bus is a bridge
**/
UINT8
EnumerateDownstream (
  IN UINT8 BusNum
  )
;

/**
  This function enumerate the bridge on the device

  @param[in] PegBus             - Particular Bus number
  @param[in] PegDev             - Particular Device number
  @param[in] PegFunc            - Particular Func number
  @param[in, out] PegMmioLength - PEG MMIO length

  @retval CardDetect : TRUE if current bridge device has a Graphics card.
  @retval CardDetect : FALSE if current bridge device has no Graphics card.
**/
BOOLEAN
EnumerateBridgeDevice (
  IN UINT8                                  PegBus,
  IN UINT8                                  PegDev,
  IN UINT8                                  PegFunc,
  IN OUT UINT32                             *PegMmioLength
  )
;

/**

  "Poll Status" for GT Readiness

  @param[in] PeiServices     - Pointer to the PEI services table
  @param[in] StallPpi        - Pointer to Stall PPI
  @param[in] Base            - Base address of MMIO
  @param[in] Offset          - MMIO Offset
  @param[in] Mask            - Mask
  @param[in] Result          - Value to wait for


**/
VOID
PollGtReady (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN EFI_PEI_STALL_PPI        *StallPpi,
  UINT64 Base,
  UINT32 Offset,
  UINT32 Mask,
  UINT32 Result
  )
;

#endif
