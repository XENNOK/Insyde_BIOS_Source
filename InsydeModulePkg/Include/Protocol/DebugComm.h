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

#ifndef _DEBUG_COMM_H_
#define _DEBUG_COMM_H_

#include <Uefi.h>

#define H2O_DEBUG_COMM_PROTOCOL_GUID \
         {0xaf124cf2, 0x745f, 0x46a1, 0x86, 0xfc, 0x8f, 0x3a, 0xe3, 0x6d, 0x9f, 0xb0}

extern EFI_GUID gH2ODebugCommProtocolGuid;

typedef
struct _H2O_DEBUG_COMM_PROTOCOL H2O_DEBUG_COMM_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *H2O_DEBUG_SEND_INFO) (
  IN H2O_DEBUG_COMM_PROTOCOL      *This,
  IN EFI_PHYSICAL_ADDRESS         ImageBase,
  IN UINT32                       ImageSize,
  IN CHAR8                        *PdbPointer,
  IN EFI_PHYSICAL_ADDRESS         ImageEntry
  );

typedef
EFI_STATUS
(EFIAPI *H2O_DEBUG_RESET) (
  IN H2O_DEBUG_COMM_PROTOCOL      *This
  );

typedef
EFI_STATUS
(EFIAPI *H2O_DEBUG_PRINT) (
  IN H2O_DEBUG_COMM_PROTOCOL      *This,
  IN CHAR8                        *Format,
  IN VA_LIST                      Parameters
  );

struct _H2O_DEBUG_COMM_PROTOCOL {
  H2O_DEBUG_SEND_INFO            SendInfo;
  H2O_DEBUG_RESET                Reset;
  H2O_DEBUG_PRINT                DDTPrint;
};

#endif