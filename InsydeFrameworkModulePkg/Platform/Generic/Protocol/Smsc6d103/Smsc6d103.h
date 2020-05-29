//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
#ifndef _SMSC6D103_H_
#define _SMSC6D103_H_


#define EFI_SMSC6D103_GUID \
	{ 0xd52ddd61, 0x8553, 0x4870, 0xbf, 0x9c, 0x43, 0xeb, 0xb3, 0xd, 0xf6, 0xb9 }



EFI_FORWARD_DECLARATION (EFI_SMSC6D103_PROTOCOL);

typedef
EFI_STATUS
(EFIAPI *EFI_TEMP_READING) (
    IN  EFI_SMSC6D103_PROTOCOL             *This,
    IN  UINTN                              Which,
    OUT INT8                               *pData
);

typedef
EFI_STATUS
(EFIAPI *EFI_TACH_READING) (
    IN  EFI_SMSC6D103_PROTOCOL             *This,
    IN  UINTN                              Which,
    OUT UINT16                               *pData
);

typedef
EFI_STATUS
(EFIAPI *EFI_VOLTAGE_READING) (
    IN  EFI_SMSC6D103_PROTOCOL             *This,
    IN  UINTN                              Which,
    OUT UINT32                             *Vol
);
typedef
EFI_STATUS
(EFIAPI *EFI_SET_FAN_MODE) (
    IN  EFI_SMSC6D103_PROTOCOL             *This,
    IN  UINTN                              Which,
    IN  UINTN                              Mode
);

typedef
EFI_STATUS
(EFIAPI *EFI_SET_MONITORING) (
    IN  EFI_SMSC6D103_PROTOCOL             *This,
    IN  UINT8                              State
);



typedef struct _EFI_SMSC6D103_PROTOCOL {

  EFI_SET_FAN_MODE			SetFanMode;
	EFI_SET_MONITORING    SetMonitoring;
  EFI_TEMP_READING       TempReading;
  EFI_TACH_READING       TachReading;
  EFI_VOLTAGE_READING    VoltageReading;

} EFI_SMSC6D103_PROTOCOL;

extern EFI_GUID gEfiSmsc6d103Guid;

#endif

