/** @file
  Hardware IO Interface for DASH

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

#ifndef _DASH_IO_CFG_H_
#define _DASH_IO_CFG_H_

#include <Uefi.h>

#define DASH_IO_CFG_PROTOCOL_GUID \
  { 0x6e0762db, 0x18c0, 0xc0a, 0x8a, 0x6c, 0x6f, 0xfd, 0xdb, 0xf1, 0x0c, 0x91 }
  
extern EFI_GUID gDashIoCfgProtocolGuid;



typedef struct _DASH_IO_CFG_PROTOCOL DASH_IO_CFG_PROTOCOL;

typedef struct {
  UINT8 Major;
  UINT8 Minor;
  UINT8 Week;
  UINT16 Sequence;
} DASH_VERSION;

typedef
UINT8
(EFIAPI *ASF_READ_REG) (
  IN  DASH_IO_CFG_PROTOCOL    *This,
  IN UINT8                   Reg
);

typedef
VOID
(EFIAPI *ASF_WRITE_REG) (
  IN  DASH_IO_CFG_PROTOCOL     *This,
  IN  UINT8                   Reg,
  OUT UINT8                   Value
);

typedef
VOID
(EFIAPI *INIT_ASF) (
  IN  DASH_IO_CFG_PROTOCOL     *This
  )
;

typedef
UINT16
(EFIAPI *UART_GET_VENDOR_ID) (
  VOID
);

typedef
UINT16
(EFIAPI *UART_GET_DEVICE_ID) (
  VOID
);

typedef
UINT8
(EFIAPI *GET_HOST_SMBUS_SLAVE_ADDR) (
  VOID
);

typedef
UINT8
(EFIAPI *GET_MC_SMBUS_ADDR) (
  VOID
);

typedef
UINT8
(EFIAPI *GET_HOST_ENDPOINT_ID) (
  VOID
);

typedef
UINT8
(EFIAPI *GET_MC_ENDPOINT_ID) (
  VOID
);

typedef
EFI_DEVICE_PATH_PROTOCOL *
(EFIAPI *GET_SOL_DEVICE_PATH) (
  DASH_IO_CFG_PROTOCOL  *This
);

typedef
VOID
(EFIAPI *SET_SOL_DEVICE_PATH) (
  DASH_IO_CFG_PROTOCOL  *This,
  EFI_DEVICE_PATH_PROTOCOL *SolDevicePath
);

typedef
VOID
(EFIAPI *GET_DASH_VERSION) (
  DASH_IO_CFG_PROTOCOL   *This,
  DASH_VERSION          *Version
);


struct _DASH_IO_CFG_PROTOCOL {
  ASF_READ_REG              AsfReadReg;
  ASF_WRITE_REG             AsfWriteReg;
  INIT_ASF                  InitAsf;
  UART_GET_VENDOR_ID        UartGetVendorId;
  UART_GET_DEVICE_ID        UartGetDeviceId;
  GET_HOST_SMBUS_SLAVE_ADDR GetHostSmbusSlaveAddr;
  GET_MC_SMBUS_ADDR         GetMcSmbusAddr;
  GET_HOST_ENDPOINT_ID      GetHostEndpointId;
  GET_MC_ENDPOINT_ID        GetMcEndpointId;
  GET_SOL_DEVICE_PATH       GetSolDevicePath;
  SET_SOL_DEVICE_PATH       SetSolDevicePath;
  GET_DASH_VERSION          GetDashVersion;
};

#endif
