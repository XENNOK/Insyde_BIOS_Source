//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _EFI_VGA_SWITCH_H_
#define _EFI_VGA_SWITCH_H_

//
// GUID Defination
//
#define EFI_VGA_SWITCH_PROTOCOL_GUID \
  { 0xb7cfc339, 0x7575, 0x4116, 0xa4, 0x4c, 0x61, 0xb, 0x34, 0x5d, 0xb7, 0xee }

//
// Constant Defination
//
typedef enum {
  EFI_UGA_MODE,
  EFI_VGA_MODE,
  MAX_VIDEO_MODE,
  EFI_NOT_VALID_MODE = 0xFF
} VGA_SWITCH_MODE;

//
// Protocol Interface Defination
//
typedef struct {
  VGA_SWITCH_MODE                       Switch;
} EFI_VGA_SWITCH_PROTOCOL;


extern EFI_GUID gEfiVgaSwitchprotocolGuid;

#endif
