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

#ifndef _PLATFORM_IDE_H
#define _PLATFORM_IDE_H

#include "Uefi.h"
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <ChipsetSetupConfig.h>
#include <Protocol/PlatformIde.h>
#include <Protocol/SetupUtility.h>

#define PLATFORM_IDE_SIGNATURE   SIGNATURE_32('P','i','d','e')


#define ICH_MAX_CHANNEL                 1
#define ICH_MAX_DEVICE                  2

//
// GPIO Base definition
//

//
// Setup definition
//
#define SETUP_IDE_CONTROLLER_DISABLE    0
#define SETUP_IDE_CONTROLLER_PATA       1
#define SETUP_IDE_CONTROLLER_SATA       2
#define SETUP_IDE_CONTROLLER_BOTH       3

#define SETUP_IDE_TYPE_AUTO             0
#define SETUP_IDE_TYPE_USER_DEFINED     1

#define SETUP_IDE_TRANSFER_AUTO         0
#define SETUP_IDE_TRANSFER_FPIO         1
#define SETUP_IDE_TRANSFER_UDMA_33      2
#define SETUP_IDE_TRANSFER_UDMA_66      3
#define SETUP_IDE_TRANSFER_UDMA_100     4

#define SETUP_IDE_CABLE_TYPE_AUTO       0
#define SETUP_IDE_CABLE_TYPE_40_PIN     1
#define SETUP_IDE_CABLE_TYPE_80_PIN     2

#define EFI_LEGACY_AUTO                 0
#define EFI_LEGACY_PATA_ONLY            1
#define EFI_LEGACY_SATA_ONLY            2

//
// PATA setup option define
//
typedef struct {
  UINT8                                 Type;
  UINT8                                 BitIo32;
  UINT8                                 BlockMode;
  UINT8                                 TransferMode;
} PATA_IDE_OPTION;

//
// Platform Ide private data definition
//
typedef struct {
  UINT32                                Signature;
  EFI_HANDLE                            Handle;

  BOOLEAN                               Channel[ICH_MAX_CHANNEL];

  EFI_SETUP_UTILITY_PROTOCOL            *SetupUtility;
  EFI_PCI_IO_PROTOCOL                   *PciIo;

  EFI_PLATFORM_IDE_PROTOCOL             PlatformIde;

} PLATFORM_IDE_PRIVATE_DATA;

#define PLATFORM_IDE_FROM_THIS(a) \
  BASE_CR(a, PLATFORM_IDE_PRIVATE_DATA, PlatformIde, PLATFORM_IDE_SIGNATURE)

//
// [Mark for Reduce Code]
//
#if 0
//
// Platform Ide services
//
EFI_STATUS
PlatformIdeCheckController (
  IN  EFI_PLATFORM_IDE_PROTOCOL         *This,
  IN  EFI_HANDLE                        *Controller
  );

EFI_STATUS
PlatformIdeGetChannelInfo (
  IN  EFI_PLATFORM_IDE_PROTOCOL         *This,
  IN  UINT8                             Channel,
  OUT BOOLEAN                           *Enabled,
  OUT UINT8                             *MaxDevices
  );

EFI_STATUS
PlatformIdeCalculateMode (
  IN  EFI_PLATFORM_IDE_PROTOCOL         *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN OUT EFI_ATA_COLLECTIVE_MODE        *SupportedModes
  );

EFI_STATUS
DetectCableType (
  IN  EFI_PCI_IO_PROTOCOL               *PciIo,
  IN  CHIPSET_CONFIGURATION              *SetupVariable,
  IN  UINT8                             Channel
  );
#endif // [Mark for Reduce Code]

#endif
