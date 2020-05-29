/** @file
  Protocol for describing Platform Ide.

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

#ifndef _EFI_PLATFORM_IDE_H_
#define _EFI_PLATFORM_IDE_H_

#include <Protocol/PciIo.h>
#include <Protocol/IdeControllerInit.h>

//
// Global ID for the EFI Platform IDE Protocol GUID
//
#define EFI_PLATFORM_IDE_PROTOCOL_GUID \
  { 0x377c66a3, 0x8fe7, 0x4ee8, 0x85, 0xb8, 0xf1, 0xa2, 0x82, 0x56, 0x9e, 0x3b }
typedef struct _EFI_PLATFORM_IDE_PROTOCOL EFI_PLATFORM_IDE_PROTOCOL;

typedef enum {
  TransferModeFpio0,
  TransferModeFpio1,
  TransferModeFpio2,
  TransferModeFpio3,
  TransferModeFpio4,
  TransferModeFpioMax
} EFI_IDE_TRANSFER_MODE_PIO;

typedef enum {
  TransferModeUdma0,
  TransferModeUdma1,
  TransferModeUdma2,
  TransferModeUdma3,
  TransferModeUdma4,
  TransferModeUdma5,
  TransferModeUdmaMax
} EFI_IDE_TRANSFER_MODE_UDMA;

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_IDE_CHECK_CONTROLLER) (
  IN  struct _EFI_PLATFORM_IDE_PROTOCOL *This,
  IN  EFI_HANDLE                        *Controller
);

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_IDE_GET_CHANNEL_INFO) (
  IN  struct _EFI_PLATFORM_IDE_PROTOCOL *This,
  IN  UINT8                             Channel,
  OUT BOOLEAN                           *Enabled,
  OUT UINT8                             *MaxDevices
);

typedef
EFI_STATUS
(EFIAPI *EFI_PLATFORM_IDE_CALCULATE_MODE) (
  IN  struct _EFI_PLATFORM_IDE_PROTOCOL *This,
  IN  UINT8                             Channel,
  IN  UINT8                             Device,
  IN OUT EFI_ATA_COLLECTIVE_MODE        *SupportedModes
);

typedef struct _EFI_PLATFORM_IDE_PROTOCOL {
  EFI_PLATFORM_IDE_CHECK_CONTROLLER     CheckController;
  EFI_PLATFORM_IDE_GET_CHANNEL_INFO     GetChannelInfo;
  EFI_PLATFORM_IDE_CALCULATE_MODE       CalculateMode;
};

extern EFI_GUID gEfiPlatformIdeProtocolGuid;

#endif
