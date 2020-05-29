/** @file

  EFI MEBx Protocol

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _MEBX_PROTOCOL_H
#define _MEBX_PROTOCOL_H

#define MEBX_CALLBACK_INFO_SIGNATURE    SIGNATURE_32 ('m', 'e', 'b', 'x')

#define MEBX_CALLBACK_INFO_FROM_THIS(a) CR (a, MEBX_CALLBACK_INFO, DriverCallback, MEBX_CALLBACK_INFO_SIGNATURE)

#ifndef INTEL_MEBX_PROTOCOL_GUID
#define INTEL_MEBX_PROTOCOL_GUID \
  { 0x01AB1829, 0xCECD, 0x4CFA, { 0xA1, 0x8C, 0xEA, 0x75, 0xD6, 0x6F, 0x3E, 0x74 }}
#endif

typedef
EFI_STATUS
(EFIAPI *EFI_MEBX_API_ENTRY_POINT) (
  IN UINT32  BiosParams,
  OUT UINT32 *MebxReturnValue
  );

typedef struct _MEBX_VER {
  UINT16  Major;
  UINT16  Minor;
  UINT16  Hotfix;
  UINT16  Build;
} MEBX_VER;

typedef enum {
  MEBX_GRAPHICS_AUTO    = 0,
  MEBX_GRAPHICS_640X480,
  MEBX_GRAPHICS_800X600,
  MEBX_GRAPHICS_1024X768
} MEBX_GRAPHICS_MODE;

typedef enum {
  MEBX_TEXT_AUTO        = 0,
  MEBX_TEXT_80X25,
  MEBX_TEXT_100X31
} MEBX_TEXT_MODE;

typedef struct _EFI_MEBX_PROTOCOL {
  MEBX_VER                  MebxVersion;
  EFI_MEBX_API_ENTRY_POINT  CoreMebxEntry;
} EFI_MEBX_PROTOCOL;

typedef struct _MEBX_INSTANCE {
  EFI_HANDLE        Handle;
  EFI_MEBX_PROTOCOL MebxProtocol;
} MEBX_INSTANCE;

extern EFI_GUID gEfiMebxProtocolGuid;

#endif
