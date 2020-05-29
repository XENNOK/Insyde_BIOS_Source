/** @file
  This protocol is used to report and control what BIOS is mapped to the
  BIOS address space anchored at 4GB boundary.

  This protocol is EFI compatible.

  E.G. For current generation ICH, the 4GB-16MB to 4GB range can be mapped
  to PCI, SPI, or FWH.

@copyright
  Copyright (c) 2005 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
#ifndef _EFI_ACTIVE_BIOS_PROTOCOL_H_
#define _EFI_ACTIVE_BIOS_PROTOCOL_H_

///
/// Define the  protocol GUID
///
/// EDK and EDKII have different GUID formats
///
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#define EFI_ACTIVE_BIOS_PROTOCOL_GUID \
  { \
    0xebbe2d1b, 0x1647, 0x4bda, 0xab, 0x9a, 0x78, 0x63, 0xe3, 0x96, 0xd4, 0x1a \
  }
#else
#define EFI_ACTIVE_BIOS_PROTOCOL_GUID \
  { \
    0xebbe2d1b, 0x1647, 0x4bda, \
    { \
      0xab, 0x9a, 0x78, 0x63, 0xe3, 0x96, 0xd4, 0x1a \
    } \
  }
#endif
//
// Extern the GUID for protocol users.
//
extern EFI_GUID                           gEfiActiveBiosProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_ACTIVE_BIOS_PROTOCOL  EFI_ACTIVE_BIOS_PROTOCOL;

///
/// Protocol definitions
///
typedef enum {
  ActiveBiosStateSpi,
  ActiveBiosStateLpc,
  ActiveBiosStateMax
} EFI_ACTIVE_BIOS_STATE;

/**
  Change the current active BIOS settings to the requested state.
  The caller is responsible for requesting a supported state from
  the EFI_ACTIVE_BIOS_STATE selections.
  This will fail if someone has locked the interface and the correct key is
  not provided.

  @param[in] This                 Pointer to the EFI_ACTIVE_BIOS_PROTOCOL instance.
  @param[in] DesiredState         The requested state to configure the system for.
  @param[in] Key                  If the interface is locked, Key must be the Key
                                  returned from the LockState function call.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_ACCESS_DENIED       The interface is currently locked.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ACTIVE_BIOS_SET_ACTIVE_BIOS_STATE) (
  IN EFI_ACTIVE_BIOS_PROTOCOL     * This,
  IN EFI_ACTIVE_BIOS_STATE        DesiredState,
  IN UINTN                        Key
  );

/**
  Lock the current active BIOS state from further changes. This allows a
  caller to implement a critical section. This is optionally supported
  functionality.  Size conscious implementations may choose to require
  callers cooperate without support from this protocol.

  @param[in] This                 Pointer to the EFI_ACTIVE_BIOS_PROTOCOL instance.
  @param[in] Lock                 TRUE to lock the current state, FALSE to unlock.
  @param[in, out] Key             If Lock is TRUE, then a key will be returned.  If
                                  Lock is FALSE, the key returned from the prior call
                                  to lock the protocol must be provided to unlock the
                                  protocol.  The value of Key is undefined except that
                                  it cannot be 0.

  @retval EFI_SUCCESS             Command succeed.
  @exception EFI_UNSUPPORTED      The function is not supported.
  @retval EFI_ACCESS_DENIED       The interface is currently locked.
  @retval EFI_DEVICE_ERROR        Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ACTIVE_BIOS_LOCK_ACTIVE_BIOS_STATE) (
  IN     EFI_ACTIVE_BIOS_PROTOCOL   * This,
  IN     BOOLEAN                    Lock,
  IN OUT UINTN                      *Key
  );

///
/// Protocol definition
///
/// Note that some functions are optional.  This means that they may be NULL.
/// Caller is required to verify that an optional function is defined by checking
/// that the value is not NULL.
///
/// This protocol allows the PCH to be configured to map the top 16 MB of memory
/// below 4 GB to different buses, LPC, SPI, or PCI. The State reflects the current
/// setting. SetState() allows consumers to request a new state, and LockState()
/// allows consumers to prevent other consumers from changing the state. It is the
/// caller's responsibility to configure and lock the desired state to prevent issues
/// resulting from other consumers changing the state.
///
struct _EFI_ACTIVE_BIOS_PROTOCOL {
  EFI_ACTIVE_BIOS_STATE                   State;        ///< The current state mapping that is selected.
  EFI_ACTIVE_BIOS_SET_ACTIVE_BIOS_STATE   SetState;     ///< Change the current state to the requested state mapping.
  EFI_ACTIVE_BIOS_LOCK_ACTIVE_BIOS_STATE  LockState;    ///< Lock the current state mapping to prevent changes to the current state.
};

#endif
