/** @file

  Provide all Sio resource structure

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

#ifndef _SIO_RESOURCE_
#define _SIO_RESOURCE_

#include <Sio/SioCommon.h>

typedef struct {
  UINT16       IoPort;
  UINT8        IrqNumber;
  UINT8        DmaChannel;
}EFI_ISA_DEVICE_RESOURCE;

typedef EFI_STATUS(EFIAPI *SET_POWER) (BOOLEAN);
typedef EFI_STATUS(EFIAPI *ENABLE_DEVICE) (BOOLEAN);
typedef EFI_STATUS(EFIAPI *GET_POSSIBLE_RESOURCE) (EFI_ISA_DEVICE_RESOURCE **, UINT32 *);
typedef EFI_STATUS(EFIAPI *GET_CURRENT_RESOURCE) (EFI_ISA_DEVICE_RESOURCE **);
typedef EFI_STATUS(EFIAPI *SET_RESOURCE) (EFI_ISA_DEVICE_RESOURCE *);

typedef struct {
  SET_POWER             SetPower;
  GET_POSSIBLE_RESOURCE GetPossibleResource;
  SET_RESOURCE          SetResource;
  ENABLE_DEVICE         EnableDevice;
  GET_CURRENT_RESOURCE  GetCurrentResource;
}EFI_ISA_ACPI_DEVICE_FUNCTION;

typedef struct {
  UINT8        Instance;
  UINT8        TypeIdHigh;
  UINT8        TypeIdLow;
  UINT16       SioCfgPort;  // 2E/4E/162E/164E
  BOOLEAN      ScriptFlag;
}EFI_SIO_RESOURCE_FUNCTION;

typedef EFI_STATUS (EFIAPI *INSTALL_DEVICE_PROTOCOL) (SIO_DEVICE_LIST_TABLE*);

typedef struct {
  UINT8                    Device;
  UINT8                    Instance;
  UINT8                    Ldn;
  INSTALL_DEVICE_PROTOCOL  InstallDeviceProtocol;
}EFI_INSTALL_DEVICE_FUNCTION;

#endif
