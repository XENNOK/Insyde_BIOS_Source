//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _UEFI_RAID_OPROM_READY_H_
#define _UEFI_RAID_OPROM_READY_H_

// {E553A1E3-E26E-4edf-8AE1-E3ED9357AF56}
#define UEFI_RAID_OPROM_READY_GUID \
 { 0xe553a1e3, 0xe26e, 0x4edf, 0x8a, 0xe1, 0xe3, 0xed, 0x93, 0x57, 0xaf, 0x56}

EFI_FORWARD_DECLARATION (EFI_UEFI_RAID_OPROM_READY_PROTOCOL);

typedef
EFI_STATUS
(EFIAPI *EFI_UEFI_RAID_OPROM_READY_INTERFACE) (
  IN EFI_UEFI_RAID_OPROM_READY_PROTOCOL    *This
);

typedef struct _EFI_UEFI_RAID_OPROM_READY_PROTOCOL {
  EFI_UEFI_RAID_OPROM_READY_INTERFACE      UefiRaidOpromReadyInterface;
} EFI_UEFI_RAID_OPROM_READY_PROTOCOL;

extern EFI_GUID gUefiRaidOpromReadyGuid;

#endif // _UEFI_RAID_OPROM_READY_H_
