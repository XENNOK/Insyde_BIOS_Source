/** @file
  USB KBC Emulator Protocol Header

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

#ifndef _USB_KBC_EMULATOR_H_
#define _USB_KBC_EMULATOR_H_

#include <Uefi.h>

#define EFI_KBC_EMULATOR_PROTOCOL_GUID \
{ \
  0x55668581, 0x8fc2, 0x4717, 0x93, 0xf, 0x4, 0xc1, 0x18, 0x95, 0x54, 0x34 \
}

#define KBC_STATUS_OBF                  0x01
#define KBC_STATUS_IBF                  0x02
#define KBC_STATUS_FLAG                 0x04
#define KBC_STATUS_CMDDATA              0x08
#define KBC_STATUS_INHIBIT              0x10
#define KBC_STATUS_AOBF                 0x20
#define KBC_STATUS_TIMEOUT              0x40
#define KBC_STATUS_PARITY               0x80

//
// Protocol definitions
//
typedef
EFI_STATUS
(EFIAPI *EFI_KBC_EMULATOR_INSERT_KBC_KEYCODE) (
  IN     UINT8                          QueueSlot,
  IN     UINT8                          *KeyCode,
  IN     UINTN                          Length
  );

#define KBC_TRAP_READ_DATA_PORT         0x01
#define KBC_TRAP_WRITE_DATA_PORT        0x02
#define KBC_TRAP_READ_STATUS_PORT       0x04
#define KBC_TRAP_WRITE_COMMAND_PORT     0x08
#define KBC_TRAP_A20GATE_CHANGED        0x10

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_EMULATOR_KBC_TRAP_PROCESSOR) (
  IN     UINTN                          TrapType,
  IN     UINT8                          Data
  );

typedef struct _EFI_KBC_EMULATOR_PROTOCOL {
  EFI_KBC_EMULATOR_INSERT_KBC_KEYCODE                   InsertKbcKeyCode;
  EFI_KBC_EMULATOR_KBC_TRAP_PROCESSOR                   KbcTrapProcessor;
} EFI_KBC_EMULATOR_PROTOCOL;

#endif
