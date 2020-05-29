//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SwapAddressRange.h

Abstract:

--*/

#ifndef _EFI_SWAP_ADDRESS_RANGE_PROTOCOL_H_
#define _EFI_SWAP_ADDRESS_RANGE_PROTOCOL_H_

#define EFI_SWAP_ADDRESS_RANGE_PROTOCOL_GUID \
  { \
    0x1259f60d, 0xb754, 0x468e, 0xa7, 0x89, 0x4d, 0xb8, 0x5d, 0x55, 0xe8, 0x7e \
  }

//
// Forward reference for pure ANSI compatability
//
EFI_FORWARD_DECLARATION (EFI_SWAP_ADDRESS_RANGE_PROTOCOL);

#define EFI_UNSUPPORT_LOCK  0
#define EFI_SOFTWARE_LOCK   1
#define EFI_HARDWARE_LOCK   2

typedef UINT8 EFI_SWAP_LOCK_CAPABILITY;

//
// Protocl APIs
//
typedef
EFI_STATUS
(EFIAPI *EFI_GET_RANGE_LOCATION) (
  IN EFI_SWAP_ADDRESS_RANGE_PROTOCOL            * This,
  OUT EFI_PHYSICAL_ADDRESS                      * BootBlockBase,
  OUT UINTN                                     *BootBlockSize,
  OUT EFI_PHYSICAL_ADDRESS                      * BackupBlockBase,
  OUT UINTN                                     *BackupBlockSize
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_SWAP_STATE) (
  IN EFI_SWAP_ADDRESS_RANGE_PROTOCOL            * This,
  OUT BOOLEAN                                   *SwapState
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_SWAP_STATE) (
  IN EFI_SWAP_ADDRESS_RANGE_PROTOCOL            * This,
  IN BOOLEAN                                    NewSwapState
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_RTC_POWER_STATUS) (
  IN EFI_SWAP_ADDRESS_RANGE_PROTOCOL            * This,
  OUT BOOLEAN                                   *RtcPowerFailed
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_SWAP_LOCK_CAPABILITY) (
  IN EFI_SWAP_ADDRESS_RANGE_PROTOCOL            * This,
  OUT EFI_SWAP_LOCK_CAPABILITY                  * LockCapability
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_SWAP_LOCK) (
  IN EFI_SWAP_ADDRESS_RANGE_PROTOCOL            * This,
  IN EFI_SWAP_LOCK_CAPABILITY                   LockCapability,
  IN BOOLEAN                                    NewLockState
  );

typedef struct _EFI_SWAP_ADDRESS_RANGE_PROTOCOL {
  EFI_GET_RANGE_LOCATION        GetRangeLocation;       // has output parameters for base and length
  EFI_GET_SWAP_STATE            GetSwapState;           // are ranges swapped or not
  EFI_SET_SWAP_STATE            SetSwapState;           // swap or unswap ranges
  EFI_GET_RTC_POWER_STATUS      GetRtcPowerStatus;      // checks RTC battery, or whatever...
  EFI_GET_SWAP_LOCK_CAPABILITY  GetSwapLockCapability;  // Get TOP_SWAP lock capability,
  EFI_SET_SWAP_LOCK             SetSwapLock;            // Set TOP_SWAP lock state
} EFI_SWAP_ADDRESS_RANGE_PROTOCOL;

extern EFI_GUID gEfiSwapAddressRangeProtocolGuid;

#endif
