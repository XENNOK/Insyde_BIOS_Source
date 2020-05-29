/** @file
  This file defines the BTS INIT Protocol.                                     
;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _BTS_INIT_DXE_H_
#define _BTS_INIT_DXE_H_

///
/// Define protocol GUID
///
#define EFI_BTS_INIT_PROTOCOL_GUID \
  { 0x3fe51719, 0x2b7c, 0x44cc, {0xa0, 0x05, 0xc9, 0x2a, 0xd2, 0x07, 0xbe, 0x56} }

typedef struct _EFI_BTS_INIT_PROTOCOL EFI_BTS_INIT_PROTOCOL;

#define	CPU_MODE16		0
#define	CPU_MODE32		1
#define	CPU_MODE64		2

typedef
EFI_STATUS
(EFIAPI *EFI_SEND_INFO) (
  IN EFI_PHYSICAL_ADDRESS         ImageBase,
  IN UINT32                       ImageSize,
  IN CHAR8                        *PdbPointer,
  IN EFI_PHYSICAL_ADDRESS         ImageEntry
  );

typedef
VOID
(EFIAPI *EFI_START_BTS) (
  );

typedef
VOID
(EFIAPI *EFI_STOP_BTS) (
  );

typedef struct _EFI_BTS_INIT_PROTOCOL {
  EFI_SEND_INFO                   SendInfo;
  EFI_START_BTS                   StartBts;
  EFI_STOP_BTS                    StopBts;
} EFI_BTS_INIT_PROTOCOL;

#define INSYDE_BTS_SEND_INFO(a, b, c, d, e) \
{ \
  EFI_STATUS                 Status; \
  EFI_BTS_INIT_PROTOCOL      *BtsInit; \
  EFI_GUID                   EfiBtsInitProtocolGuid = EFI_BTS_INIT_PROTOCOL_GUID; \
  Status = a->LocateProtocol (&EfiBtsInitProtocolGuid, NULL, &BtsInit); \
  if (Status == EFI_SUCCESS && BtsInit->SendInfo) { \
    BtsInit->SendInfo((EFI_PHYSICAL_ADDRESS)(b), (UINT32)(c), (CHAR8*)(d), (EFI_PHYSICAL_ADDRESS)(e)); \
  } \
}

#define INSYDE_BTS_START_BTS(a) \
{ \
  EFI_STATUS                 Status; \
  EFI_BTS_INIT_PROTOCOL      *BtsInit; \
  EFI_GUID                   EfiBtsInitProtocolGuid = EFI_BTS_INIT_PROTOCOL_GUID; \
  Status = a->LocateProtocol (&EfiBtsInitProtocolGuid, NULL, &BtsInit); \
  if ((Status == EFI_SUCCESS) && BtsInit->StartBts) { \
    BtsInit->StartBts(); \
  } \
}

#define INSYDE_BTS_STOP_BTS(a) \
{ \
  EFI_STATUS                 Status; \
  EFI_BTS_INIT_PROTOCOL      *BtsInit; \
  EFI_GUID                   EfiBtsInitProtocolGuid = EFI_BTS_INIT_PROTOCOL_GUID; \
  Status = a->LocateProtocol (&EfiBtsInitProtocolGuid, NULL, &BtsInit); \
  if (Status == EFI_SUCCESS && BtsInit->StopBts) { \
    BtsInit->StopBts(); \
  } \
}

extern EFI_GUID gEfiBtsInitProtocolGuid;

#endif
