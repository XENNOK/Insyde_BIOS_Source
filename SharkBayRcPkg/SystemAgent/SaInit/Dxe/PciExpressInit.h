/** @file
  Header file for PciExpress Initialization Driver.

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
**/
#ifndef _PCIEXPRESS_INITIALIZATION_DRIVER_H_
#define _PCIEXPRESS_INITIALIZATION_DRIVER_H_

#include <Uefi.h>
#include <SaAccess.h>
#include <PchAccess.h>
#include <IndustryStandard/Pci30.h>
#include <EfiScriptLib.h>
#include <ExitPmAuth.h>
#include <CpuRegs.h>
#include <Include/Cpu.h>

#include <Library/DebugLib.h>
#include <Library/CpuPlatformLib.h>

#include <Protocol/BootScriptSave.h>
#include <Protocol/SaPlatformPolicy.h>
#include <Protocol/SaGlobalNvsArea.h>

#define DISABLED  0
#define AUTO      1
#define ENABLED   1

#define GEN1      1
#define GEN2      2

typedef struct {
  UINT8 Bus;
  UINT8 Device;
  UINT8 Function;
  UINT8 Slot;
  UINT8 Bus2;
  UINT8 Device2;
  UINT8 Function2;
} PEG_PORT_DEVICE;

///
/// Function prototypes
///
/**
  PCI Express Dxe Initialization.
  Run before PCI Bus Init, where assignment of Bus, Memory,
    and I/O Resources are assigned.

  @param[in] DxePlatformSaPolicy -     SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS     - Pci Express successfully started and ready to be used
  @exception EFI_UNSUPPORTED - Pci Express can't be initialized
**/
EFI_STATUS
PciExpressInit (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
;

/**
  Find the Offset to a given Capabilities ID

  @param[in] Bus       -   Pci Bus Number
  @param[in] Device    -   Pci Device Number
  @param[in] Function  -   Pci Function Number
  @param[in] CapId     -   CAPID to search fo

  @retval 0       - CAPID not found
  @retval Other   - CAPID found, Offset of desired CAPID
**/
UINT32
PcieFindCapId (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function,
  IN UINT8   CapId
  )
;

/**
  Search and return the offset of desired Pci Express Capability ID

  @param[in] Bus       -   Pci Bus Number
  @param[in] Device    -   Pci Device Number
  @param[in] Function  -   Pci Function Number
  @param[in] CapId     -   Extended CAPID to search for

  @retval 0       - CAPID not found
  @retval Other   - CAPID found, Offset of desired CAPID
**/
UINT32
PcieFindExtendedCapId (
  IN UINT8   Bus,
  IN UINT8   Device,
  IN UINT8   Function,
  IN UINT16  CapId
  )
;

/**
  This function enables the CLKREQ# PM on all the end point functions

  @param[in] Bus       -   Pci Bus Number
  @param[in] Device    -   Pci Device Number
**/
VOID
PcieSetClkreq (
  IN  UINT8   Bus,
  IN  UINT8   Device
  )
;

/**
  Perform Egress Port 0 Initialization.

  @param[in] EgressPortBar   - EPBAR Address
**/
EFI_STATUS
Cid1EgressPort0Init (
  IN  UINT64      EgressPortBar
  )
;

#ifdef PEG_FLAG
/**
  Conditionally perform PEG Port Initialization.
  bugbug: organize this code in a way that can utilize the
  PchS3ItemTypeInitPcieRootPortDownstream EFI_PCH_S3_DISPATCH_ITEM_TYPE

  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol
**/
EFI_STATUS
Cid1PegPortInit (
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
;
#endif // PEG_FLAG

/**
  DMI Port Initialization for both CID1 (Port 1 in MCH) and CID2 (Port 0 in ICH).

  @param[in] DmiBar              - DMIBAR Address
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol
**/
EFI_STATUS
Cid1Cid2DmiPortInit (
  IN  UINT64                         DmiBar,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
;

/**
  Perform Root Complex Topology Initialization for CID1.

  @param[in] EgressPortBar     - EPBAR Address
  @param[in] DmiBar            - DMIBAR Address
  @param[in] PchRootComplexBar - ICH RCBA Address
**/
EFI_STATUS
Cid1TopologyInit (
  IN UINT64        EgressPortBar,
  IN  UINT64       DmiBar,
  IN  UINT32       PchRootComplexBar
  )
;

/**
  Perform Root Complex Topology Initialization for CID2.

  @param[in] DmiBar            - DMIBAR Address
  @param[in] PchRootComplexBar - ICH RCBA Address
**/
EFI_STATUS
Cid2TopologyInit (
  IN UINT32   PchRootComplexBar,
  IN UINT64   DmiBar
  )
;

/**
  Additional PEG Programming Steps as in SA BIOS spec

  @param[in] Bus         - Bus Number of PEG device
  @param[in] pegDev      - Dev Number of PEG device
  @param[in] pegFn       - Func Number of PEG device
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol
**/
VOID
AdditionalPEGProgramStepsBeforeASPM (
  IN  UINT8                          Bus,
  IN  UINT8                          pegDev,
  IN  UINT8                          pegFn,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
;

/**
  Additional DMI Programming Steps as in SA BIOS spec

  @param[in] DmiBar              - DMIBAR Address
  @param[in] DxePlatformSaPolicy - SA DxePlatformPolicy protocol
**/
VOID
AdditionalDMIProgramStepsBeforeASPM (
  IN UINT64                          DmiBar,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
;

#endif
