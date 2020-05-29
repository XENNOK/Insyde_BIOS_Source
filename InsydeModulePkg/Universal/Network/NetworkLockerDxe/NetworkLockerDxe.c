/** @file
  When user enable PxeToLan.
  Control network stack behavior via callback.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/KernelConfigLib.h>
#include "NetworkLockerDxe.h"

NETWORK_LOCKER_INSTANCE           *NetworkLockerInstance;

/**
  This function called when OPROM loaded from LegacyPci driver.
  It will check PCI device's Class Code, if is network and not need loaded.
  Return EFI_UNSUPPORTED to exit LegacyPci OPROM loader.

  @param[in]  This                  Will bypass to next caller
  @param[in]  Mode                  Will bypass to next caller
  @param[in]  Type                  Will bypass to next caller
  @param[in]  DeviceHandle          Will bypass to next caller
  @param[in]  ShadowAddress         Will bypass to next caller
  @param[in]  Compatibility16Table  Will bypass to next caller
  @param[in]  AdditionalData        Will bypass to next caller

  @retval     EFI_SUCCESS           OPROM will loading
  @retval     EFI_UNSUPPORTED       OPROM will not loading in policy

**/
EFI_STATUS
EFIAPI
LegacyBiosPlatformNetworkHook (
  IN EFI_LEGACY_BIOS_PLATFORM_PROTOCOL *This,
  IN EFI_GET_PLATFORM_HOOK_MODE        Mode,
  IN UINT16                            Type,
  IN  EFI_HANDLE                       DeviceHandle,
  IN  OUT UINTN                        *ShadowAddress,
  IN  EFI_COMPATIBILITY16_TABLE        *Compatibility16Table,
  IN  VOID                             **AdditionalData OPTIONAL
  )
{
  EFI_STATUS                           Status;
  EFI_PCI_IO_PROTOCOL                  *PciIo;
  UINT8                                ClassCode [4];
  EFI_LEGACY_BIOS_PLATFORM_HOOKS       LegacyBiosPlatformHooks;
  LEGACY_PXE_EXPANSION_ROM_HEADER      *OptionRomHeader;
  UNDI_ROM_ID_STRUCTURE                *UndiRomIdStruct;

  LegacyBiosPlatformHooks = NetworkLockerInstance->LegacyBiosPlatformHooks;

  //
  // The NetworkStack is enable when go to there.
  //
  if (NetworkLockerInstance->BootType == DUAL_BOOT_TYPE) {
    if (NetworkLockerInstance->NetworkStackStatus != UEFI_NETWORK_BOOT_OPTION_LEGACY) {
      //
      // Check the option rom type, if is the network will reject.
      //
      Status = gBS->HandleProtocol (
                      DeviceHandle,
                      &gEfiPciIoProtocolGuid,
                      (VOID **) &PciIo
                      );
      if (!EFI_ERROR (Status)) {
        OptionRomHeader = PciIo->RomImage;
        UndiRomIdStruct = (UNDI_ROM_ID_STRUCTURE *) ((UINTN) PciIo->RomImage + OptionRomHeader->PxeRomIdOffset);
        if (UndiRomIdStruct->Signature == UNDI_SIGNATURE) {
          return EFI_UNSUPPORTED;
        }
        Status = PciIo->Pci.Read (
                          PciIo,
                          EfiPciIoWidthUint32,
                          0x08,
                          1,
                          &ClassCode[0]
                          );
        if (!EFI_ERROR (Status) && ClassCode[3] == PCI_CLASS_NETWORK) {
          return EFI_UNSUPPORTED;
        }
      }
    }
  }

  Status = LegacyBiosPlatformHooks (
             This,
             Mode,
             Type,
             DeviceHandle,
             ShadowAddress,
             Compatibility16Table,
             AdditionalData
             );

  return Status;
}

/**
  This function called when PlatformProtocol installed.
  It will hook PlatformHooks service, to check Network OPROM is loaded or not need.

  @param                        NONE
  @retval                       NONE

**/
VOID
EFIAPI
NetworkLockerHookLegacyBiosPlatform (
  IN EFI_EVENT                         Event,
  IN VOID                              *Context
  )
{
  EFI_STATUS                           Status;
  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL    *LegacyBiosPlatform;

  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosPlatformProtocolGuid,
                  NULL,
                  (VOID **) &LegacyBiosPlatform
                  );

  if (!EFI_ERROR (Status)) {
    NetworkLockerInstance->LegacyBiosPlatform      = LegacyBiosPlatform;
    NetworkLockerInstance->LegacyBiosPlatformHooks = LegacyBiosPlatform->PlatformHooks;
    LegacyBiosPlatform->PlatformHooks              = LegacyBiosPlatformNetworkHook;
  }
}

EFI_STATUS
NetworkStackCallback (
  NETWORK_STACK_CALLBACK_IDENTIFIER    Identifier
  )
{
  BOOLEAN                              NetworkStackEnable;
  UINT8                                NetworkStackStatus;
  UINT8                                BootType;
  EFI_STATUS                           Status;

  NetworkStackEnable = NetworkLockerInstance->NetworkStackEnable;
  NetworkStackStatus = NetworkLockerInstance->NetworkStackStatus;
  BootType           = NetworkLockerInstance->BootType;
  Status             = EFI_UNSUPPORTED;

  if (!NetworkStackEnable) {
    return EFI_UNSUPPORTED;
  }

  if (BootType == SCU_LEGACY_BOOT_TYPE) {
    return EFI_UNSUPPORTED;
  }

  //
  // There will be DUAL and UEFI mode.
  //
  switch (Identifier) {
  case NetworkStackSnpCallback:
    Status = EFI_SUCCESS;
    break;

  case NetworkStackPxe4Callback:
  case NetworkStackMtftp4Callback:
    if (NetworkStackStatus == UEFI_NETWORK_BOOT_OPTION_IPV4 ||
        NetworkStackStatus == UEFI_NETWORK_BOOT_OPTION_BOTH) {
      Status = EFI_SUCCESS;
    }
    break;

  case NetworkStackPxe6Callback:
  case NetworkStackMtftp6Callback:
    if (NetworkStackStatus == UEFI_NETWORK_BOOT_OPTION_IPV6 ||
        NetworkStackStatus == UEFI_NETWORK_BOOT_OPTION_BOTH) {
      Status = EFI_SUCCESS;
    }
    break;

  case NetworkStackIp4Callback:
  case NetworkStackIp6Callback:
  case NetworkStackUdp4Callback:
  case NetworkStackUdp6Callback:
  case NetworkStackTcp4Callback:
  case NetworkStackTcp6Callback:
  case NetworkStackDhcp4Callback:
  case NetworkStackDhcp6Callback:
    Status = EFI_SUCCESS;
    break;

  default:
    break;
  }

  return Status;
}

/**
  This driver will locate SetupUtilityProtocol
  To check PxeToLan and NetowrkProtocol configuration

  As the status table to install NetworkLockerProtocol.
  To control network driver need loaded in DXE

   Boot Type  | Network Stack | PXE Boot To LAN | PXE capability | DHCP                | UEFI   | Legacy    | UNDI
              |               |                 |                | (BitLocker Support) | PXE    | PXE OPROM | Driver
  ------------+---------------+-----------------+----------------+---------------------+--------+-----------+---------
  UEFI / Dual | Disable       |  Hide           | Gray out       |                     |        |           |
  UEFI / Dual | Enable        |  Hide           | IPv4           |  DHCP4              | PXE4   |           | Loaded
  UEFI / Dual | Enable        |  Hide           | IPv6           |  DHCP6              | PXE6   |           | Loaded
  UEFI / Dual | Enable        |  Hide           | IPv4/IPv6      |  DHCP4+6            | PXE4+6 |           | Loaded
  UEFI / Dual | Enable        |  Hide           | Disable        |  DHCP4+6            |        |           | Loaded
         Dual | Enable        |  Hide           | Legacy         |                     |        | Loaded    |
  Legacy      | Hide          |  Disable        | Hide           |                     |        |           |
  Legacy      | Hide          |  Enable         | Hide           |                     |        | Loaded    |


  @param[in]  ImageHandle       The firmware allocated handle for the UEFI image.
  @param[in]  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS           Driver loaded and initialize success
  @retval EFI_NOT_READY         The dependency protocol not ready
  @retval other                 The error from Protocol serverice

**/
EFI_STATUS
EFIAPI
NetworkLockerEntryPoint (
  IN EFI_HANDLE                        ImageHandle,
  IN EFI_SYSTEM_TABLE                  *SystemTable
  )
{
  EFI_STATUS                           Status;
  KERNEL_CONFIGURATION                 KernelConfiguration;
  VOID                                 *Registration;
  NETWORK_LOCKER_PROTOCOL              *NetworkLockerProtocol;
  EFI_EVENT                            LegacyBiosPlatformProtocolNotifyEvent;
  NETWORK_STACK_DHCP6_DUID_SELECTOR    *Selector;
  UINT8                                *Pool;
  UINTN                                Index;
  UINTN                                Size;
  UINT8                                BootType;

  NetworkLockerInstance                 = NULL;
  LegacyBiosPlatformProtocolNotifyEvent = NULL;

  if (gBS == NULL) {
    return EFI_NOT_READY;
  }

  Status = gBS->LocateProtocol (
                  &gNetworkLockerProtocolGuid,
                  NULL,
                  (VOID **) &NetworkLockerProtocol
                  );
  if (!EFI_ERROR (Status)) {
    return EFI_ALREADY_STARTED;
  }

  NetworkLockerInstance = (NETWORK_LOCKER_INSTANCE *) AllocateZeroPool (sizeof (NETWORK_LOCKER_INSTANCE));
  if (NetworkLockerInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = GetKernelConfiguration (&KernelConfiguration);
  if (EFI_ERROR (Status)) {
    return EFI_NOT_READY;
  }

  //
  // Block Legacy OPROM process
  //
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_CALLBACK,
                  NetworkLockerHookLegacyBiosPlatform,
                  NULL,
                  &LegacyBiosPlatformProtocolNotifyEvent
                  );
  if (!EFI_ERROR (Status)) {
    gBS->RegisterProtocolNotify (
           &gEfiLegacyBiosPlatformProtocolGuid,
           LegacyBiosPlatformProtocolNotifyEvent,
           &Registration
           );
    NetworkLockerInstance->LegacyBiosPlatformProtocolNotifyEvent = LegacyBiosPlatformProtocolNotifyEvent;
  }

  if (KernelConfiguration.PxeBootToLan == SCU_PXE_BOOT_TO_LAN_ENABLED) {
    BootType = DUAL_BOOT_TYPE;
    Size = sizeof (UINT8);
    Status = gRT->GetVariable (
                    L"BootType",
                    &gSystemConfigurationGuid,
                    NULL,
                    &Size,
                    &BootType
                    );
    NetworkLockerInstance->NetworkStackEnable = TRUE;
    NetworkLockerInstance->BootType           = BootType;
    NetworkLockerInstance->NetworkStackStatus = KernelConfiguration.NetworkProtocol;
  }

  NetworkLockerInstance->NetworkLockerProtocol.Signature = NETWORK_LOCKER_SIGNATURE;
  NetworkLockerInstance->NetworkLockerProtocol.Version   = NETWORK_LOCKER_VERSION;

  //
  // Network Stack controll callback function notify
  //
  NetworkLockerInstance->NetworkLockerProtocol.NetworkStackCallback = NetworkStackCallback;
  Status = gBS->InstallProtocolInterface (
                  &(NetworkLockerInstance->Handle),
                  &gNetworkLockerProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  (VOID *) &(NetworkLockerInstance->NetworkLockerProtocol)
                  );

  //
  // Initialize Dhcp6 DUID selector
  //
  NetworkLockerInstance->NetworkLockerProtocol.NetworkStackDhcp6DuidSelector = NULL;
  Pool = (UINT8 *) PcdGetPtr (PcdDhcp6DuidSelector);
  if ((Pool != NULL) && (Pool[0] != 0) && (Pool[0] < 4)) {
    Selector = AllocateZeroPool (sizeof (NETWORK_STACK_DHCP6_DUID_SELECTOR) + Pool[0] - 1);
    if (Selector != NULL) {
      Selector->Signature   = DHCP6_DUID_SELECTOR_SIGNATURE;
      Selector->SelectorCnt = Pool[0];
      for (Index = 0; Index < Pool[0]; Index++) {
        Selector->Pool[Index] = Pool[Index + 1];
      }
      NetworkLockerInstance->NetworkLockerProtocol.NetworkStackDhcp6DuidSelector = Selector;
    }

  }

  //
  // Setup UNDI driver dependency protocol
  //
  if (KernelConfiguration.PxeBootToLan == SCU_PXE_BOOT_TO_LAN_ENABLED) {
    if (KernelConfiguration.NetworkProtocol != UEFI_NETWORK_BOOT_OPTION_LEGACY) {
      gBS->InstallProtocolInterface (&ImageHandle, &gEfiNetworkStackUndiDependencyGuid, EFI_NATIVE_INTERFACE, NULL);
    }
  }

  return Status;
}
