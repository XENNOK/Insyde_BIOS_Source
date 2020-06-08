/** @file
  PciUtilityNDMD_16 H Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PCI_UTILITY_NDMD_16_H_
#define _PCI_UTILITY_NDMD_16_H_

#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>

#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>

// PciUtility(1)
#define MAIN_PAGE_MODE   0
#define BRANCH_PAGE_MODE 1
#define WRITE_MODE       2
#define BYTE             1
#define WORD             2
#define DWORD            4
// (1)

#define EFI_PCI_UTILITY_PROTOCOL_GUID                                             \
  {                                                                               \
    0x11111111, 0x1111, 0x1111, {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11 } \
  }

typedef struct _EFI_PCI_UTILITY_PROTOCOL EFI_PCI_UTILITY_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_MAIN_PAGE)(
  IN EFI_PCI_UTILITY_PROTOCOL    *This
  );

// EFI_STATUS
// PciUtilityUnload (
//   IN EFI_HANDLE ImageHandle
//   );

EFI_STATUS
MainPage (
  IN EFI_PCI_UTILITY_PROTOCOL    *This
  );

typedef struct _EFI_PCI_UTILITY_PROTOCOL {
  UINT32         Apple;
  EFI_MAIN_PAGE  MainPage;
};

extern EFI_GUID  gEfiPciUtilityProtocolGuid;
//  = EFI_PCI_UTILITY_PROTOCOL_GUID;

// PciUtility(2)
typedef struct {
  INT16 column;         /// cursor column
  INT16 row;            /// cursor row
} POSITION;

typedef struct {
  UINT32    value;      /// write value
  POSITION  position;   /// cursor position
} WRITE_BUFFER;

typedef struct {
  UINT16   Bus;         
  UINT16   Device;      
  UINT16   Func;
  UINT16   Offset;
} ADDRESS;

UINT8
ShowMainPage (
  POSITION *CursorPosition
  );

VOID 
ShowRegistPage (
  ADDRESS *Address, 
  POSITION *CursorPosition
  );

VOID 
ReadRegist (
  ADDRESS *Address,
  POSITION *CursorPosition, 
  UINT8 ReadMode
  );

VOID 
WriteRegist (
  ADDRESS *Address, 
  UINT32 WriteBufferConvert, 
  UINT8 ReadMode
  );

VOID 
DisplayPosition (
  UINT8 ReadMode, 
  POSITION *CursorPosition
  );

const POSITION  MainPage_ResetPosition  = {16, 5};
const POSITION  ReadMode1_ResetPosition = {5, 4};
const POSITION  ReadMode2_ResetPosition = {6, 4};                  
const POSITION  ReadMode4_ResetPosition = {8, 4};
// (2)

#endif