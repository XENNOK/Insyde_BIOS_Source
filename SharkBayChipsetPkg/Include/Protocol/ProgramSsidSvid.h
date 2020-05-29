/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PROGRAM_SSID_SVID_PROTOCOL_H_
#define _PROGRAM_SSID_SVID_PROTOCOL_H_



#define EFI_PROGRAM_SSID_SVID_PROTOCOL_GUID \
  { 0x67BA958, 0x7D99, 0x461D, 0x9D, 0xC, 0x8C, 0x55, 0x74, 0xA9, 0x83, 0x5B }


typedef
EFI_STATUS
( EFIAPI *PROGRAM_SSID_SVID_FUNC ) (
  VOID
  );


typedef struct _EFI_PROGRAM_SSID_SVID_PROTOCOL {
  PROGRAM_SSID_SVID_FUNC    ProgramSsidSvidFunc;
} EFI_PROGRAM_SSID_SVID_PROTOCOL;


extern EFI_GUID gEfiProgramSsidSvidProtocolGuid;

#endif
