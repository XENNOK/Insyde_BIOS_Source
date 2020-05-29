/** @file
  CrHookDxe driver Header

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

#ifndef _CR_HOOK_DRIVER_H
#define _CR_HOOK_DRIVER_H

#include "CrBdaMemManager.h"

#include <PiDxe.h>

#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeServicesLib.h>
#include <Library/DebugLib.h>

#include <Protocol/LegacyBios.h>
#include <Protocol/CRPolicy.h>
#include <Protocol/ConsoleRedirectionService.h>
#include <Protocol/CRBdsHook.h>
#include <Protocol/SmmAccess2.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <Protocol/IsaAcpi.h>


//UART
#define UART_DATA_BITS_8      (BIT0 | BIT1)
#define UART_DATA_BITS_7      (BIT1)
#define UART_STOP_BITS_1      0
#define UART_STOP_BITS_2      (BIT2)
#define UART_PARITY_NONE      0
#define UART_PARITY_EVEN      (BIT3 | BIT4)
#define UART_PARITY_ODD       (BIT3)

//
// Bios Post Phase
//
#define BIOS_PHASE_INIT       0
#define BIOS_PHASE_OPROM      1
#define BIOS_PHASE_SCU        2
#define BIOS_PHASE_OS         3

//
// VGA buffer for text mode
//
#define SEGMENT_SHIFT        4
#define TEXT_PAGE_0_START    0xb8000
#define TEXT_PAGE_SIZE       0x1000
#define CHAR_SPACE           0x20
#define COLOR_BG_BLACK       0
#define COLOR_FG_WHITE       0x07

//
// CR INFO : pass CR information to CR option ROM
//
#define CR_MEM_MANAGER      0xA4
#define CR_CRINFO           0xA6

#pragma pack (push)
#pragma pack (1)

typedef struct {
  UINT8  Irq;
  UINT16 PortAddress;
} OPROM_ISA_DEVICE;

typedef struct {
  UINT8   Bus;
  UINT8   DevFun;
} OPTOM_PCI_DEVICE;

typedef union {
  OPROM_ISA_DEVICE  IsaDevice;
  OPTOM_PCI_DEVICE  PciDevice;
} OPROM_CR_DEVICE;

typedef struct {
  UINT8             Type;
  OPROM_CR_DEVICE   Device;
  UINT8             BaudRateDivisor;
} CR_DEVICE;


#define CR_INFO_REVISION    0x04

typedef struct {
  UINT16      Signature;
  UINT8       Revision;
  UINT8       HLength;
  UINT16      BaudRateDivisor;
  UINT8       Protocol;
  UINT8       FIFOLength;
  UINT8       FlowControl;
  UINT8       TerminalType;
  UINT16      FeatureFlag;
  UINT8       ShowHelp;
  UINT8       InfoWaitTime;
  UINT8       CRAfterPost;
  UINT16      TerminalEscCodeOffset;
  UINT16      CrSpecialCommandTableOffset;
  UINT8       Headless;
  UINT8       CRHeadlessVBuffer;              // Which buffer be chose when headless
  UINT8       DeviceCount;
  UINT8       CRWriteCharInterval;
} CR_EFI_INFO;

#pragma pack (pop)

#endif
