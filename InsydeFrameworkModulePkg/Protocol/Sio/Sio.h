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
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  Sio.h

Abstract:

--*/

#ifndef _SIO_H_
#define _SIO_H_

#define SIO_IO_PROTOCOL_GUID \
  { \
    0x964e5b21, 0x6000, 0x23d2, 0x9e, 0x39, 0x1, 0xa0, 0xc9, 0x69, 0x72, 0x3b \
  }
#define VENDOR_INTEL \
  { \
    0x8aa4fb2d, 0x9e81, 0x43d2, 0xb2, 0xad, 0x90, 0xc, 0x21, 0x79, 0x65, 0x20 \
  }

EFI_FORWARD_DECLARATION (EFI_SIO_PROTOCOL);

typedef
EFI_STATUS
(EFIAPI *EFI_GET_DEVICE_POWER) (
  IN EFI_SIO_PROTOCOL          * This,
  IN UINT8                     LogicalDevice,
  OUT UINT8                    *Power
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_DEVICE_POWER) (
  IN EFI_SIO_PROTOCOL          * This,
  IN UINT8                     LogicalDevice,
  OUT UINT8                    Power
  );

typedef
EFI_STATUS
(EFIAPI *EFI_ENABLEDISABLE_INTELLIGIENT_POWER_MGMT) (
  IN EFI_SIO_PROTOCOL          * This,
  IN UINT8                     LogicalDevice,
  OUT UINT8                    Power
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_INTELLIGIENT_POWER_MGMT) (
  IN EFI_SIO_PROTOCOL          * This,
  IN UINT8                     LogicalDevice,
  OUT UINT8                    *Power
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SETENABLEDISABLE_SERIAL) (
  IN EFI_SIO_PROTOCOL          * This,
  IN  UINT8                    SerialNo,
  IN  UINT8                    EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_ENABLEDISABLE_SERIAL) (
  IN EFI_SIO_PROTOCOL          * This,
  IN  UINT8                    SerialNo,
  OUT UINT8                    *EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_BASEIOADDRESS_SERIAL) (
  IN EFI_SIO_PROTOCOL          * This,
  IN  UINT8                    SerialNo,
  IN  UINT16                   BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_BASEIOADDRESS_SERIAL) (
  IN EFI_SIO_PROTOCOL           * This,
  IN  UINT8                     SerialNo,
  IN  UINT16                    *BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_INTERRUPT_SERIAL) (
  IN EFI_SIO_PROTOCOL          * This,
  IN  UINT8                    SerialNo,
  IN  UINT8                    InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_INTERRUPT_SERIAL) (
  IN EFI_SIO_PROTOCOL          * This,
  IN  UINT8                    SerialNo,
  IN  UINT8                    *InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SET_MODE_SERIAL) (
  IN EFI_SIO_PROTOCOL          * This,
  IN  UINT8                    SerialNo,
  IN  UINT8                    Mode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_MODE_SERIAL) (
  IN EFI_SIO_PROTOCOL          * This,
  IN  UINT8                    SerialNo,
  IN  UINT8                    *Mode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PARALLEL_SET_ENABLEDISABLE) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  UINT8                        EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PARALLEL_GET_ENABLEDISABLE) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT UINT8                        *EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PARALLEL_SET_BASEIOADDRESS) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  UINT16                       BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PARALLEL_GET_BASEIOADDRESS) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT UINT16                       *BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PARALLEL_SET_INTERRUPT) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  UINT8                        InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PARALLEL_GET_INTERRUPT) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT UINT8                        *InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PARALLEL_SET_DMA) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  UINT8                        DMAChannel
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PARALLEL_GET_DMA) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT UINT8                        *DMAChannel
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PARALLEL_SET_MODE) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  UINT8                        Mode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PARALLEL_GET_MODE) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT UINT8                        *Mode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SET_ENABLEDISABLE) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  UINT8                        EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_GET_ENABLEDISABLE) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT UINT8                        *EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SET_BASEIOADDRESS) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  UINT16                       BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_GET_BASEIOADDRESS) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT UINT16                       *BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SET_INTERRUPT) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  UINT8                        InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_GET_INTERRUPT) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT UINT8                        *InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SET_DMA) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  UINT8                        DMAChannel
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_GET_DMA) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT UINT8                        *DMAChannel
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SET_WRITEPROTECT) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  BOOLEAN                      Enable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_GET_WRITEPROTECT) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT BOOLEAN                      *Enable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_SET_ENABLEDISABLE) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  UINT8                        EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_GET_ENABLEDISABLE) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT UINT8                        *EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_SET_KBINTERRUPT) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  UINT8                        InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_GET_KBINTERRUPT) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT UINT8                        *InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_SET_MSINTERRUPT) (
  IN EFI_SIO_PROTOCOL              * This,
  IN  UINT8                        InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_GET_MSINTERRUPT) (
  IN EFI_SIO_PROTOCOL              * This,
  OUT UINT8                        *InterruptNo
  );

EFI_STATUS
InitializeSioDriver (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  ImageHandle - GC_TODO: add argument description
  SystemTable - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
;

typedef struct _EFI_SIO_PROTOCOL {
  //
  // Global interface
  //
  EFI_GET_DEVICE_POWER                      GetDevicePower;
  EFI_SET_DEVICE_POWER                      SetDevicePower;
  EFI_ENABLEDISABLE_INTELLIGIENT_POWER_MGMT EnableDisableIntelliPwrMgmt;
  EFI_GET_INTELLIGIENT_POWER_MGMT           GetIntelliPwrMgmt;

  //
  // interface for serial I/O
  //
  EFI_SETENABLEDISABLE_SERIAL               SerialSetEnableDisable;
  EFI_GET_ENABLEDISABLE_SERIAL              SerialGetEnableDisable;
  EFI_SET_INTERRUPT_SERIAL                  SerialSetInterrupt;
  EFI_GET_INTERRUPT_SERIAL                  SerialGetInterrupt;
  EFI_SET_BASEIOADDRESS_SERIAL              SerialSetBaseIOAddress;
  EFI_GET_BASEIOADDRESS_SERIAL              SerialGetBaseIOAddress;
  EFI_SET_MODE_SERIAL                       SerialSetMode;
  EFI_GET_MODE_SERIAL                       SerialGetMode;

  //
  // interface for Parallel Port
  //
  EFI_PARALLEL_SET_ENABLEDISABLE            ParallelSetEnableDisable;
  EFI_PARALLEL_GET_ENABLEDISABLE            ParallelGetEnableDisable;
  EFI_PARALLEL_SET_BASEIOADDRESS            ParallelSetBaseIOAddress;
  EFI_PARALLEL_GET_BASEIOADDRESS            ParallelGetBaseIOAddress;
  EFI_PARALLEL_SET_INTERRUPT                ParallelSetInterrupt;
  EFI_PARALLEL_GET_INTERRUPT                ParallelGetInterrupt;
  EFI_PARALLEL_SET_DMA                      ParallelSetDMA;
  EFI_PARALLEL_GET_DMA                      ParallelGetDMA;
  EFI_PARALLEL_SET_MODE                     ParallelSetMode;
  EFI_PARALLEL_GET_MODE                     ParallelGetMode;

  //
  // interface for Floppy Disk Controller
  //
  EFI_FDC_SET_ENABLEDISABLE                 FdcSetEnableDisable;
  EFI_FDC_GET_ENABLEDISABLE                 FdcGetEnableDisable;
  EFI_FDC_SET_BASEIOADDRESS                 FdcSetBaseIOAddress;
  EFI_FDC_GET_BASEIOADDRESS                 FdcGetBaseIOAddress;
  EFI_FDC_SET_INTERRUPT                     FdcSetInterrupt;
  EFI_FDC_GET_INTERRUPT                     FdcGetInterrupt;
  EFI_FDC_SET_DMA                           FdcSetDMA;
  EFI_FDC_GET_DMA                           FdcGetDMA;
  EFI_FDC_SET_WRITEPROTECT                  FdcSetWriteProtect;
  EFI_FDC_GET_WRITEPROTECT                  FdcGetWriteProtect;

  //
  // interface for Keyboard Controller
  //
  EFI_KBC_SET_ENABLEDISABLE                 KbcSetEnableDisable;
  EFI_KBC_GET_ENABLEDISABLE                 KbcGetEnableDisable;
  EFI_KBC_SET_KBINTERRUPT                   KbcSetKeyboardInterrupt;
  EFI_KBC_GET_KBINTERRUPT                   KbcGetKeyboardInterrupt;
  EFI_KBC_SET_MSINTERRUPT                   KbcSetMouseInterrupt;
  EFI_KBC_GET_MSINTERRUPT                   KbcGetMouseInterrupt;

} EFI_SIO_PROTOCOL;

extern EFI_GUID gEfiSioProtocolGuid;

#endif
