//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement
**/

/**

Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


@file

  LegacyRegion.c

  This code provides a private implementation of the Legacy Region protocol.

**/
#include "LegacyRegion.h"

///
/// Module Global:
///   Since this driver will only ever produce one instance of the Private Data
///   protocol you are not required to dynamically allocate the PrivateData.
///
LEGACY_REGION_INSTANCE        mPrivateData;

UINT8                         mRomData[16] = {
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00,
  0x00
};

UINT8                         mLockData[16] = {
  0x01,
  0x10,
  0x01,
  0x10,
  0x01,
  0x10,
  0x01,
  0x10,
  0x03,
  0x30,
  0x03,
  0x30, /// it was: 0x03, 0x30, 0x03, 0x30, <-- why ??
  0x10,
  0x10,
  0x10,
  0x30
};

UINT8                         mUnlockData[16] = {
  0x03,
  0x30,
  0x03,
  0x30,
  0x03,
  0x30,
  0x03,
  0x30,
  0x03,
  0x30,
  0x03,
  0x30,
  0x30,
  0x30,
  0x30,
  0x30
};

UINT8                         mMaskData[16] = {
  0x30,
  0x03,
  0x30,
  0x03,
  0x30,
  0x03,
  0x30,
  0x03,
  0x30,
  0x03,
  0x30,
  0x03,
  0x00,
  0x00,
  0x00,
  0x00
};

UINT8                         mReg[16] = {
  R_SA_PAM1,
  R_SA_PAM1,
  R_SA_PAM2,
  R_SA_PAM2,
  R_SA_PAM3,
  R_SA_PAM3,
  R_SA_PAM4,
  R_SA_PAM4,
  R_SA_PAM5,
  R_SA_PAM5,
  R_SA_PAM6,
  R_SA_PAM6,
  R_SA_PAM0,
  R_SA_PAM0,
  R_SA_PAM0,
  R_SA_PAM0
};

EFI_STATUS
LegacyRegionManipulation (
  IN  EFI_LEGACY_REGION_PROTOCOL  * This,
  IN  UINT32                      Start,
  IN  UINT32                      Length,
  IN  UINT32                      Mode,
  OUT UINT32                      *Granularity OPTIONAL
  )
/**

  Modify PAM registers for region specified to MODE state.

  @param[in] This    - Pointer to EFI_LEGACY_REGION_PROTOCOL instance.
  @param[in] Start   - Starting address of a memory region covered by PAM registers (C0000h - FFFFFh).
  @param[in] Length  - Memory region length.
  @param[in] Mode    - Action to perform on a PAM region: UNLOCK, LOCK or BOOTLOCK.
  @param[out] Granularity - Granularity of region in bytes.

  @retval EFI_SUCCESS - PAM region(s) state modified as requested.
  @retval EFI_INVALID_PARAMETER - parameter out of boundary

**/
{
  UINT8                           Index;
  UINT8                           Data;
  UINT8                           TempData;
  UINT32                          TempAddr;
  UINT32                          NewStartAddr;
  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *Pci;
  EFI_CPU_ARCH_PROTOCOL           *CpuArch;
  UINT64                          PciAddress;
  UINT64                          Attributes;
  EFI_STATUS                      Status;

  Pci                     = mPrivateData.PciRootBridgeIo;
  if ((Start  < 0xC0000) ||
      (Start  > 0xFFFFF) ||
      (Length > 0x40000) ||
      ((Start + Length - 1) > 0xFFFFF)) {
    return EFI_INVALID_PARAMETER;
  }
  TempAddr                = Start;
  Index                   = (UINT8) ((TempAddr - 0xC0000) / PAM_GRANULARITY);
  NewStartAddr            = TempAddr = (TempAddr / PAM_GRANULARITY) * PAM_GRANULARITY;
  while (TempAddr <= (Start + Length - 1)) {
    if (Index >= 16) {
      return EFI_INVALID_PARAMETER;
    }
    if ((Mode == LOCK) || (Mode == BOOTLOCK)) {
      Data = mLockData[Index];
    } else {
      if (Mode == UNLOCK) {
        Data = mUnlockData[Index];
      } else {
        Data = mRomData[Index];
      }
    }

    PciAddress = EFI_PCI_ADDRESS (0, 0, 0, mReg[Index]);
    Pci->Pci.Read (Pci, EfiPciWidthUint8, PciAddress, 1, &TempData);
    TempData = (UINT8) ((TempData & mMaskData[Index]) | Data);
    Pci->Pci.Write (Pci, EfiPciWidthUint8, PciAddress, 1, &TempData);
    TempAddr += PAM_GRANULARITY;
    Index++;
  }

  if (Granularity != NULL) {
    *Granularity = PAM_GRANULARITY;
  }
  ///
  ///  Program the MTRRs
  ///
  switch (Mode) {

  case UNLOCK:
    Attributes = EFI_MEMORY_WT;
    break;

  case LOCK:
    Attributes = EFI_MEMORY_WP;
    break;

  case BOOTLOCK:
    Attributes = EFI_MEMORY_WP;
    break;

  default:
    Attributes = EFI_MEMORY_UC;

  }

  Status = gBS->LocateProtocol (
                  &gEfiCpuArchProtocolGuid,
                  NULL,
                  (VOID **) &CpuArch
                  );
  ASSERT_EFI_ERROR (Status);
//[-start-120627-IB05330350-remove]//
//  Status = CpuArch->SetMemoryAttributes (
//                      CpuArch,
//                      NewStartAddr,
//                      TempAddr - NewStartAddr,
//                      Attributes
//                      );
//  ASSERT_EFI_ERROR (Status);
//[-end-120627-IB05330350-remove]//

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
LegacyRegionDecode (
  IN  EFI_LEGACY_REGION_PROTOCOL   *This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  IN  BOOLEAN                      *On
  )
/**

  Enable/Disable decoding of the given region

  @param[in] This    - Pointer to EFI_LEGACY_REGION_PROTOCOL instance.
  @param[in] Start  - Starting address of region.
  @param[in] Length - Length of region in bytes.
  @param[in] On     - 0 = Disable decoding, 1 = Enable decoding.

  @retval EFI_SUCCESS - Decoding change affected.

**/
{
  UINT32  Granularity;
  if (*On) {
    return LegacyRegionManipulation (This, Start, Length, UNLOCK, &Granularity);
  } else {
    return LegacyRegionManipulation (This, Start, Length, LEGACY_REGION_DECODE_ROM, &Granularity);
  }
}

EFI_STATUS
EFIAPI
LegacyRegionBootLock (
  IN  EFI_LEGACY_REGION_PROTOCOL   * This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity OPTIONAL
  )
/**

  Make the indicated region read from RAM / write to ROM.

  @param[in] This        - Pointer to EFI_LEGACY_REGION_PROTOCOL instance.
  @param[in] Start       - Starting address of region.
  @param[in] Length      - Length of region in bytes.
  @param[out] Granularity - Granularity of region in bytes.

  @retval EFI_SUCCESS - Region locked or made R/O.

**/
{
  return LegacyRegionManipulation (This, Start, Length, BOOTLOCK, Granularity);
}

EFI_STATUS
EFIAPI
LegacyRegionLock (
  IN  EFI_LEGACY_REGION_PROTOCOL   * This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity OPTIONAL
  )
/**

  Make the indicated region read from RAM / write to ROM.

  @param[in] This    - Pointer to EFI_LEGACY_REGION_PROTOCOL instance.
  @param[in] Start       - Starting address of region.
  @param[in] Length      - Length of region in bytes.
  @param[out] Granularity - Granularity of region in bytes.

  @retval EFI_SUCCESS - Region locked or made R/O.

**/
{
  return LegacyRegionManipulation (This, Start, Length, LOCK, Granularity);
}

EFI_STATUS
EFIAPI
LegacyRegionUnlock (
  IN  EFI_LEGACY_REGION_PROTOCOL   * This,
  IN  UINT32                       Start,
  IN  UINT32                       Length,
  OUT UINT32                       *Granularity OPTIONAL
  )
/**

  Make the indicated region read from RAM / write to RAM.

  @param[in] This    - Pointer to EFI_LEGACY_REGION_PROTOCOL instance.
  @param[in] Start       - Starting address of region.
  @param[in] Length      - Length of region in bytes.
  @param[out] Granularity - Granularity of region in bytes.

  @retval EFI_SUCCESS - Region unlocked or made R/W.

**/
{
  return LegacyRegionManipulation (This, Start, Length, UNLOCK, Granularity);
}

EFI_STATUS
LegacyRegionInstall (
  IN EFI_HANDLE           ImageHandle
  )
/**

  Install Driver to produce Legacy Region protocol.

  @param[in] ImageHandle             Handle for the image of this driver

  @retval EFI_SUCCESS - Legacy Region protocol installed
  @retval Other       - No protocol installed, unload driver.

**/
{
  EFI_STATUS              Status;
  LEGACY_REGION_INSTANCE  *Private;
  Private = &mPrivateData;

  ///
  /// Grab a copy of all the protocols we depend on. Any error would
  /// be a dispatcher bug!.
  ///
  Status = gBS->LocateProtocol (
                  &gEfiPciRootBridgeIoProtocolGuid,
                  NULL,
                  (VOID **) &Private->PciRootBridgeIo
                  );
  Private->Signature              = LEGACY_REGION_INSTANCE_SIGNATURE;
  Private->LegacyRegion.Decode    = LegacyRegionDecode;
  Private->LegacyRegion.Lock      = LegacyRegionLock;
  Private->LegacyRegion.BootLock  = LegacyRegionBootLock;
  Private->LegacyRegion.UnLock    = LegacyRegionUnlock;
  Private->ImageHandle            = ImageHandle;

  ///
  /// Make a new handle and install the protocol
  ///
  Private->Handle = NULL;
  return gBS->InstallProtocolInterface (
                &Private->Handle,
                &gEfiLegacyRegionProtocolGuid,
                EFI_NATIVE_INTERFACE,
                &Private->LegacyRegion
                );
}
