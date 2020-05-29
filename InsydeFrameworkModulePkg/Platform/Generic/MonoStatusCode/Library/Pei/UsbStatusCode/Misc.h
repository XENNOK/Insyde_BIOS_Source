//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  Misc.h

Abstract:

  Lib to provide Misc I/O  Routines. This routine
  does not use PPI's but is monolithic.

  In general you should use PPI's, but some times a monolithic driver
  is better. The best justification for monolithic code is debug.

--*/

#ifndef _MISC_H_
#define _MISC_H_

#include "Tiano.h"

#define IA32API __cdecl
#define IA32_MAX_IO_ADDRESS       0xFFFF

typedef enum {
  EfiPciWidthUint8,
  EfiPciWidthUint16,
  EfiPciWidthUint32,
  EfiPciWidthUint64,
  EfiPciWidthFifoUint8,
  EfiPciWidthFifoUint16,
  EfiPciWidthFifoUint32,
  EfiPciWidthFifoUint64,
  EfiPciWidthFillUint8,
  EfiPciWidthFillUint16,
  EfiPciWidthFillUint32,
  EfiPciWidthFillUint64,
  EfiPciWidthMaximum
} EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH;

#define EFI_PCI_ADDRESS(bus,dev,func,reg) \
  ((UINT32)(0x80000000 + (((UINT32)bus) << 16) + (((UINT32)dev) << 11) + (((UINT32)func) << 8) + ((UINT32)reg)))

EFI_STATUS
PciRead (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT32                                 Address,
  IN OUT VOID                               *Buffer
  );

EFI_STATUS
PciWrite (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT32                                 Address,
  IN OUT VOID                               *Buffer
  );

//*******************************************************
// EFI_CPU_IO_PROTOCOL_WIDTH
//*******************************************************

typedef enum {
  EfiCpuIoWidthUint8,
  EfiCpuIoWidthUint16,
  EfiCpuIoWidthUint32,
  EfiCpuIoWidthUint64,
  EfiCpuIoWidthFifoUint8,
  EfiCpuIoWidthFifoUint16,
  EfiCpuIoWidthFifoUint32,
  EfiCpuIoWidthFifoUint64,
  EfiCpuIoWidthFillUint8,
  EfiCpuIoWidthFillUint16,
  EfiCpuIoWidthFillUint32,
  EfiCpuIoWidthFillUint64,
  EfiCpuIoWidthMaximum
} EFI_CPU_IO_PROTOCOL_WIDTH;


typedef union {
  UINT8       VOLATILE    *buf;
  UINT8       VOLATILE    *ui8;
  UINT16      VOLATILE    *ui16;
  UINT32      VOLATILE    *ui32;
  UINT64      VOLATILE    *ui64;
  UINTN       VOLATILE    ui;
} PTR;

UINT8
EFIAPI
CpuIoRead8 (
  IN  UINT16  Port
  )
/*++
Routine Description:
  Cpu I/O read port
Arguments:
   Port: - Port number to read
Returns:
   Return read 8 bit value
--*/
;


UINT16
EFIAPI
CpuIoRead16 (
  IN  UINT16  Port
  )
/*++
Routine Description:
  Cpu I/O read port
Arguments:
   Port: - Port number to read
Returns:
   Return read 16 bit value
--*/
;


UINT32
EFIAPI
CpuIoRead32 (
  IN  UINT16  Port
  )
/*++
Routine Description:
  Cpu I/O read port
Arguments:
   Port: - Port number to read
Returns:
   Return read 32 bit value
--*/
;


VOID
EFIAPI
CpuIoWrite8 (
  IN  UINT16  Port,
  IN  UINT32  Data
  )
/*++
Routine Description:
  Cpu I/O write 8 bit data to port
Arguments:
   Port: - Port number to read
   Data: - Data to write to the Port
Returns:
   None
--*/
;

VOID
EFIAPI
CpuIoWrite16 (
  IN  UINT16  Port,
  IN  UINT32  Data
  )
/*++
Routine Description:
  Cpu I/O write 16 bit data to port
Arguments:
   Port: - Port number to read
   Data: - Data to write to the Port
Returns:
   None
--*/
;

VOID
EFIAPI
CpuIoWrite32 (
  IN  UINT16  Port,
  IN  UINT32  Data
  )
/*++
Routine Description:
  Cpu I/O write 32 bit data to port
Arguments:
   Port: - Port number to read
   Data: - Data to write to the Port
Returns:
   None
--*/
;

VOID
EFIAPI
Stall (
  IN  UINT32  Count
  )
 ;

VOID
Memzero (
  IN VOID   *Buffer,
  IN UINTN  Size
  )
;
#endif