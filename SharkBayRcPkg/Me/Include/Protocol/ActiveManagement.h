/** @file

  Active Management Technology Protocol to return the state of ASF Boot Options
  related to Active Management Technology.

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

#ifndef _EFI_ACTIVE_MANAGEMENT_PROTOCOL_H_
#define _EFI_ACTIVE_MANAGEMENT_PROTOCOL_H_

//[-start-120808-IB10820098-modify]//
#include <Protocol/AlertStandardFormat.h>
//[-end-120808-IB10820098-modify]//
///
/// Define the  protocol GUID
///
#define EFI_ACTIVE_MANAGEMENT_PROTOCOL_GUID \
  { \
    0x8555fd40, 0x140b, 0x4f3c, 0x90, 0x5e, 0x3b, 0xf3, 0x78, 0xa0, 0x99, 0xfa \
  }

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gEfiActiveManagementProtocolGuid;

///
/// Forward reference for ANSI C compatibility
///
//[-start-120808-IB10820098-modify]//
typedef struct _EFI_ACTIVE_MANAGEMENT_PROTOCOL EFI_ACTIVE_MANAGEMENT_PROTOCOL;
//[-end-120808-IB10820098-modify]//
///
/// Protocol definitions
///
/**
  Return current state of Boot Options
  @param[in] This                 Pointer to the EFI_ACTIVE_MANAGEMENT_PROTOCOL instance.
  @param[in] CurrentState         TRUE when the boot options is enabled

  @retval EFI_SUCCESS             Command succeed.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE) (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT BOOLEAN                          *CurrentState
  );

/**
  This will return IDE Redirection boot device to boot

  @param[in] IdeBootDevice        Return the boot device number to boot
                                  Bits 0-1: If IDER boot is selected in Perimeter 1 then
                                  Bits 1,2 define the drive on the IDER controller to be used as the
                                  boot driver.
                                  Bit 1  Bit0
                                   0    0    Primary Master Drive
                                   0    1    Primary Slave Drive
                                   1    0    Secondary Master Drive
                                   1    1    Secondary Slave Drive
                                  Bits 2-7: Reserved set to 0

  @retval EFI_SUCCESS             The function completed successfully.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ACTIVE_MANAGEMENT_IDER_BOOT_DEVICE_SELECTED) (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT UINT8                            *IdeBootDevice
  );

/**
  Return current ASF Boot Options

  @param[in] This                 Pointer to the EFI_ACTIVE_MANAGEMENT_PROTOCOL instance.
  @param[in] AsfBootOptions       ASF Boot Options

  @retval EFI_SUCCESS             Command succeed.
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ACTIVE_MANAGEMENT_ASF_BOOT_OPTIONS_GET) (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT EFI_ASF_BOOT_OPTIONS             **AsfBootOptions
  );

/**
  This will return verbosity request option

  @param[in] CurrentState         Return the state of verbosity option

  @retval EFI_SUCCESS             The function completed successfully.
  @retval CurrentState            00 - No BootOption available
                                  01 - Non-Verbosity request
                                  02 - Verbosity request
**/
typedef
EFI_STATUS
(EFIAPI *EFI_ACTIVE_MANAGEMENT_VERBOSITY_REQUESTED) (
  IN     EFI_ACTIVE_MANAGEMENT_PROTOCOL   *This,
  IN OUT UINT8                            *CurrentState
  );


///
/// Intel Active Management Technology Protocol
/// It provides abstract level function of ASF boot options defined in ASF 2.0 specification for
/// other modules to get ASF boot options status.
///
struct _EFI_ACTIVE_MANAGEMENT_PROTOCOL {
  EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE        GetIderState;
  EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE        GetEnforceSecureBootState;
  EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE        GetSolState;
  EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE        GetRemoteFlashState;
  EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE        GetBiosSetupState;
  EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE        GetBiosPauseState;
  EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE        GetConsoleLockState;
  EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE        GetKvmState;
  EFI_ACTIVE_MANAGEMENT_IDER_BOOT_DEVICE_SELECTED GetIderBootDeviceSelectd;
  EFI_ACTIVE_MANAGEMENT_ASF_BOOT_OPTIONS_GET      GetAsfBootOptions;
  EFI_ACTIVE_MANAGEMENT_BOOT_OPTIONS_STATE        GetProgressMsgRequest;
};

#endif
