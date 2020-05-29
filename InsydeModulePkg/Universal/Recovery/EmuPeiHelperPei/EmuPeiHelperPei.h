/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++

Module Name:

  EmuPeiHelperPei.h

Abstract:

  Header file of EmuPeiHelper.

--*/

#ifndef _EMU_PEI_HELPER_H_
#define _EMU_PEI_HELPER_H_

#include <Uefi.h>
#include <Ppi/EmuPei.h>
#include <Ppi/DeviceRecoveryModule.h>
#include <Guid/RecoveryDevice.h>
#include <Guid/RecoveryFileAddress.h>
#include <Library/HobLib.h>
#include <Library/DebugLib.h>
#include <Guid/EmuPeiMark.h>
#include <FastRecoveryData.h>

#pragma pack(1)
#define EFI_PEI_HELPER_PRIVATE_DATA_SIGNATURE SIGNATURE_32 ('R', 'c', 'H', 'r')

typedef struct {
  UINT32                                Signature;
  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI    DeviceRecoveryPpi;
  EFI_PEI_PPI_DESCRIPTOR                PpiDescriptor;
  EFI_PHYSICAL_ADDRESS                  RecoveryImageAddress;
  UINT64                                RecoveryImageSize;
} EMU_PEI_HELPER_PRIVATE_DATA;

#define EMU_PEI_HELPER_PRIVATE_DATA_FROM_THIS(a) \
  CR (a, \
          EMU_PEI_HELPER_PRIVATE_DATA, \
          DeviceRecoveryPpi, \
          EFI_PEI_HELPER_PRIVATE_DATA_SIGNATURE \
      )


EFI_STATUS
EFIAPI
GetNumberRecoveryCapsules (
  IN EFI_PEI_SERVICES                               **PeiServices,
  IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI             *This,
  OUT UINTN                                         *NumberRecoveryCapsules
  );

EFI_STATUS
EFIAPI
GetRecoveryCapsuleInfo (
  IN  EFI_PEI_SERVICES                              **PeiServices,
  IN  EFI_PEI_DEVICE_RECOVERY_MODULE_PPI            *This,
  IN  UINTN                                         CapsuleInstance,
  OUT UINTN                                         *Size,
  OUT EFI_GUID                                      *CapsuleType
  );

EFI_STATUS
EFIAPI
LoadRecoveryCapsule (
  IN OUT EFI_PEI_SERVICES                           **PeiServices,
  IN EFI_PEI_DEVICE_RECOVERY_MODULE_PPI             *This,
  IN UINTN                                          CapsuleInstance,
  OUT VOID                                          *Buffer
  );
#pragma pack()

#endif
