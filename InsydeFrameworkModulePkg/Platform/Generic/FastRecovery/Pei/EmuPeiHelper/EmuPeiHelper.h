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
/*++

Module Name:

  EmuPeiHelper.h

Abstract:

  Header file of EmuPeiHelper.

--*/

#ifndef _EMU_PEI_HELPER_H_
#define _EMU_PEI_HELPER_H_

#include "Tiano.h"
#include "Pei.h"
#include "EfiCommonLib.h"

#include EFI_PPI_DEFINITION  (RecoveryModule)
#include EFI_PPI_DEFINITION  (DeviceRecoveryModule)
#include EFI_GUID_DEFINITION (CapsuleOnFatIdeDisk)
#include EFI_GUID_DEFINITION (RecoveryFileAddress)

#pragma pack(1)


#define EMI_PEI_HELPER_PRIVATE_DATA_SIGNATURE EFI_SIGNATURE_32 ('R', 'c', 'H', 'r')

typedef struct {
  UINTN                                 Signature;
  PEI_DEVICE_RECOVERY_MODULE_INTERFACE  DeviceRecoveryPpi;
  EFI_PEI_PPI_DESCRIPTOR                PpiDescriptor;
  EFI_PHYSICAL_ADDRESS                  RecoveryImageAddress;
  UINT64                                RecoveryImageSize;
} EMU_PEI_HELPER_PRIVATE_DATA;

#define EMI_PEI_HELPER_PRIVATE_DATA_FROM_THIS(a) \
  PEI_CR (a, \
          EMU_PEI_HELPER_PRIVATE_DATA, \
          DeviceRecoveryPpi, \
          EMI_PEI_HELPER_PRIVATE_DATA_SIGNATURE \
      )


EFI_STATUS
EFIAPI
GetNumberRecoveryCapsules (
  IN EFI_PEI_SERVICES                               **PeiServices,
  IN PEI_DEVICE_RECOVERY_MODULE_INTERFACE           *This,
  OUT UINTN                                         *NumberRecoveryCapsules
  );

EFI_STATUS
EFIAPI
GetRecoveryCapsuleInfo (
  IN  EFI_PEI_SERVICES                              **PeiServices,
  IN  PEI_DEVICE_RECOVERY_MODULE_INTERFACE          *This,
  IN  UINTN                                         CapsuleInstance,
  OUT UINTN                                         *Size,
  OUT EFI_GUID                                      *CapsuleType
  );

EFI_STATUS
EFIAPI
LoadRecoveryCapsule (
  IN OUT EFI_PEI_SERVICES                           **PeiServices,
  IN PEI_DEVICE_RECOVERY_MODULE_INTERFACE           *This,
  IN UINTN                                          CapsuleInstance,
  OUT VOID                                          *Buffer
  );
#pragma pack()

#endif
