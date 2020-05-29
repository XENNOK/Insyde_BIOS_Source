/** @file

  Definitions for WHEA support protocol.

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

#ifndef _WHEA_SUPPORT_PROTOCOL_H_
#define _WHEA_SUPPORT_PROTOCOL_H_

//=================================================
//  $(INSYDE_APEI_PKG)\Include\
//=================================================
#include <WheaDefs.h>

typedef struct  _EFI_WHEA_SUPPORT_PROTOCOL       EFI_WHEA_SUPPORT_PROTOCOL;

#define EFI_WHEA_SUPPORT_GUID \
  { 0x0F0069BC, 0x016C, 0x43D1, { 0xBF, 0x6C, 0x83, 0x7C, 0xCE, 0x92, 0xF1, 0x82 }}

typedef
EFI_STATUS
(EFIAPI *EFI_ADD_ERROR_SOURCE) (
  IN  EFI_WHEA_SUPPORT_PROTOCOL        *This,
  IN  WHEA_ERROR_TYPE                  Type,
  IN  UINTN                            Flags,
  IN  BOOLEAN                          EnableError,
  OUT UINT16                           *SourceID,
  IN  UINTN                            NoOfRecords,
  IN  UINTN                            MaxSections,
  IN  VOID                             *SourceData
);

typedef
EFI_STATUS
(EFIAPI *EFI_ADD_BOOT_ERROR_LOG) (
  IN EFI_WHEA_SUPPORT_PROTOCOL         *This,
  IN UINT8                             ErrorCondtion,
  IN UINTN                             ErrorSevirity,
  IN EFI_GUID                          *FruID,          OPTIONAL
  IN CHAR8                             *FruDescription, OPTIONAL
  IN EFI_GUID                          *ErrorType, 
  IN UINTN                             ErrorDataSize, 
  IN UINT8                             *ErrorData       OPTIONAL
);

typedef
EFI_STATUS
(EFIAPI *EFI_INSTALL_ERROR_INJECTION_METHOD) (
  IN EFI_WHEA_SUPPORT_PROTOCOL         *This,
  IN UINTN                             ErrorType,
  IN UINTN                             InstCount,
  IN VOID                              *InstEntry
);

typedef
EFI_STATUS
(EFIAPI *EFI_GET_ERROR_INJECTION_CAPABILITY) (
  IN  EFI_WHEA_SUPPORT_PROTOCOL        *This,
  OUT UINTN                            *InjectCapability
);

typedef
EFI_STATUS
(EFIAPI *EFI_GET_ELAR) (
  IN EFI_WHEA_SUPPORT_PROTOCOL          *This,
  OUT UINTN                             *ElarSize,
  OUT VOID                              **LogAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_INSTALL_ERROR_RECORD_METHOD) (
  IN EFI_WHEA_SUPPORT_PROTOCOL         *This,
  IN UINTN                             InstCount,
  IN VOID                              *InstEntry
);

typedef
EFI_STATUS
(EFIAPI *EFI_GET_ERROR_STATUS_BLOCK) (
  IN EFI_WHEA_SUPPORT_PROTOCOL         *This,
  OUT UINTN                            *ErrBlkSize,
  OUT VOID                             **ErrBlkAddr
  );

typedef struct _EFI_WHEA_SUPPORT_PROTOCOL {
  EFI_ADD_ERROR_SOURCE                           InsydeWheaAddErrorSource;
  EFI_ADD_BOOT_ERROR_LOG                         InsydeWheaAddBootErrorLog;
  EFI_INSTALL_ERROR_INJECTION_METHOD             InsydeWheaInstallErrorInjectionMethod;
  EFI_GET_ERROR_INJECTION_CAPABILITY             InsydeWheaGetErrorInjectionCapability;
  EFI_GET_ELAR                                   InsydeWheaGetElar;
  EFI_INSTALL_ERROR_RECORD_METHOD                InsydeWheaInstallErrorRecordMethod;
  EFI_GET_ERROR_STATUS_BLOCK                     InsydeWheaGetErrorStatusBlock;
};

extern EFI_GUID  gEfiWheaSupportProtocolGuid;

#endif
