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

  Misc.c

Abstract:

  Lib to provide Misc I/O Routines. This routine
  does not use PPI's but is monolithic.

  In general you should use PPI's, but some times a monolithic driver
  is better. The best justification for monolithic code is debug.

--*/

#include "Misc.h"

EFI_STATUS
EFIAPI
IoRead (
  IN  EFI_CPU_IO_PROTOCOL_WIDTH         Width,
  IN  UINT64                            UserAddress,
  IN  UINTN                             Count,
  IN  OUT VOID                          *UserBuffer
  )
/*++

Routine Description:

  This is the service that implements the I/O read

Arguments:

  Width of the Memory Access
  Address of the I/O access
  Count of the number of accesses to perform
  Pointer to the buffer to read or write from I/O space

Returns:

  Status
  EFI_SUCCESS             - The data was read from or written to the EFI System.
  EFI_INVALID_PARAMETER   - Width is invalid for this EFI System.
  EFI_INVALID_PARAMETER   - Buffer is NULL.
  EFI_UNSUPPORTED         - The Buffer is not aligned for the given Width.
  EFI_UNSUPPORTED         - The address range specified by Address, Width, and
                            Count is not valid for this EFI System.
--*/
{
  UINTN	                   InStride;
  UINTN	                   OutStride;
  UINTN	                   AlignMask;
  UINTN                    Address;
  PTR                      Buffer;


  Address    = (UINTN)  UserAddress;
  Buffer.buf = (UINT8 *)UserBuffer;

  if ( Address > IA32_MAX_IO_ADDRESS) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width >= EfiCpuIoWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  AlignMask = (1 << (Width & 0x03)) - 1;
  if ( Address & AlignMask ) {
    return EFI_INVALID_PARAMETER;
  }

  InStride  = 1 << (Width & 0x03);
  OutStride = InStride;
  if (Width >=EfiCpuIoWidthFifoUint8 && Width <= EfiCpuIoWidthFifoUint64) {
    InStride = 0;
  }
  if (Width >=EfiCpuIoWidthFillUint8 && Width <= EfiCpuIoWidthFillUint64) {
    OutStride = 0;
  }
  Width = Width & 0x03;

  //
  // Loop for each iteration and move the data
  //

  switch (Width) {
  case EfiCpuIoWidthUint8:

    for (; Count > 0; Count--, Buffer.buf += OutStride, Address += InStride) {
      *Buffer.ui8 = CpuIoRead8((UINT16)Address);
    }
    break;

  case EfiCpuIoWidthUint16:
    for (; Count > 0; Count--, Buffer.buf += OutStride, Address += InStride) {
      *Buffer.ui16 = CpuIoRead16((UINT16)Address);
    }
    break;

  case EfiCpuIoWidthUint32:
    for (; Count > 0; Count--, Buffer.buf += OutStride, Address += InStride) {
      *Buffer.ui32 = CpuIoRead32((UINT16)Address);
    }
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
IoWrite (
  IN  EFI_CPU_IO_PROTOCOL_WIDTH         Width,
  IN  UINT64                            UserAddress,
  IN  UINTN                             Count,
  IN  OUT VOID                          *UserBuffer
  )
/*++

Routine Description:


  This is the service that implements the I/O Write

Arguments:

  Width of the Memory Access
  Address of the I/O access
  Count of the number of accesses to perform
  Pointer to the buffer to read or write from I/O space

Returns:

  Status

  Status
  EFI_SUCCESS             - The data was read from or written to the EFI System.
  EFI_INVALID_PARAMETER   - Width is invalid for this EFI System.
  EFI_INVALID_PARAMETER   - Buffer is NULL.
  EFI_UNSUPPORTED         - The Buffer is not aligned for the given Width.
  EFI_UNSUPPORTED         - The address range specified by Address, Width, and
                            Count is not valid for this EFI System.

--*/
{
  UINTN	                   InStride;
  UINTN	                   OutStride;
  UINTN	                   AlignMask;
  UINTN                    Address;
  PTR                      Buffer;

  Address    = (UINTN) UserAddress;
  Buffer.buf = (UINT8 *)UserBuffer;

  if ( Address > IA32_MAX_IO_ADDRESS) {
    return EFI_INVALID_PARAMETER;
  }

  if (Width >= EfiCpuIoWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  AlignMask = (1 << (Width & 0x03)) - 1;
  if ( Address & AlignMask ) {
    return EFI_INVALID_PARAMETER;
  }

  InStride  = 1 << (Width & 0x03);
  OutStride = InStride;
  if (Width >=EfiCpuIoWidthFifoUint8 && Width <= EfiCpuIoWidthFifoUint64) {
    InStride = 0;
  }
  if (Width >=EfiCpuIoWidthFillUint8 && Width <= EfiCpuIoWidthFillUint64) {
    OutStride = 0;
  }
  Width = Width & 0x03;

  //
  // Loop for each iteration and move the data
  //

  switch (Width) {
  case EfiCpuIoWidthUint8:
    for (; Count > 0; Count--, Buffer.buf += OutStride, Address += InStride) {
      CpuIoWrite8((UINT16)Address, *Buffer.ui8);
    }
    break;

  case EfiCpuIoWidthUint16:
    for (; Count > 0; Count--, Buffer.buf += OutStride, Address += InStride) {
      CpuIoWrite16((UINT16)Address, *Buffer.ui16);
    }
    break;
  case EfiCpuIoWidthUint32:
    for (; Count > 0; Count--, Buffer.buf += OutStride, Address += InStride) {
      CpuIoWrite32((UINT16)Address, *Buffer.ui32);
    }
    break;

  default:
    return EFI_INVALID_PARAMETER;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
RootBridgeIoPciRW (
  IN BOOLEAN                                Write,
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT32                                 Address,
  IN OUT VOID                               *Buffer
  )
/*++

Routine Description:

  Read/Write PCI configuration space

Arguments:

  Write         - TRUE for writes, FALSE for reads
  Width	        - Signifies the width of the PCI operation.
  Address       - The PCI address of the operation.
  Buffer        - The destination buffer to store the results.


--*/
{
  UINTN                             PciData;

  PciData = 0xcfc + (Address & 0x03);
  Address &= ~0x03;
  CpuIoWrite32(0xcf8, Address);
  if (Write)
  {
    switch (Width)
    {
      case EfiPciWidthUint8:
        CpuIoWrite8((UINT16)PciData, *(UINT8*)Buffer);
        break;
      case EfiPciWidthUint16:
        CpuIoWrite16((UINT16)PciData, *(UINT16*)Buffer);
        break;
      case EfiPciWidthUint32:
        CpuIoWrite32((UINT16)PciData, *(UINT32*)Buffer);
        break;
    }
  }
  else
  {
    switch (Width)
    {
      case EfiPciWidthUint8:
        *(UINT8*)Buffer = CpuIoRead8((UINT16)PciData);
        break;
      case EfiPciWidthUint16:
        *(UINT16*)Buffer = CpuIoRead16((UINT16)PciData);
        break;
      case EfiPciWidthUint32:
        *(UINT32*)Buffer = CpuIoRead32((UINT16)PciData);
        break;
    }
  }
  return EFI_SUCCESS;
}

EFI_STATUS
PciRead (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT32                                 Address,
  IN OUT VOID                               *Buffer
  )
/*++

Routine Description:

  Enables a PCI driver to read PCI controller registers in a
  PCI configuration space under a PCI Root Bridge.

Arguments:
  Width	  - Signifies the width of the PCI operation.
  Address - The PCI address of the operation.
  Buffer  - The destination buffer to store the results.


Returns:

  EFI_SUCCESS	            The data was read from the PCI root bridge.
  EFI_INVALID_PARAMETER	  Width is invalid for this PCI root bridge.
  EFI_INVALID_PARAMETER	  Buffer is NULL.
  EFI_OUT_OF_RESOURCES	  The request could not be completed due to a lack of resources.


--*/
{
  return RootBridgeIoPciRW (
           FALSE,
           Width,
           Address,
           Buffer
           );
}

EFI_STATUS
PciWrite (
  IN EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_WIDTH  Width,
  IN UINT32                                 Address,
  IN OUT VOID                               *Buffer
  )
/*++


Routine Description:

  Enables a PCI driver to write to PCI controller registers in a
  PCI configuration space under a PCI Root Bridge.

Arguments:

  Width	  - Signifies the width of the PCI operation.
  Address - The PCI address of the operation.
  Buffer  - The source buffer to get the data.


Returns:

  EFI_SUCCESS	            The data was read from the PCI root bridge.
  EFI_INVALID_PARAMETER	  Width is invalid for this PCI root bridge.
  EFI_INVALID_PARAMETER	  Buffer is NULL.
  EFI_OUT_OF_RESOURCES	  The request could not be completed due to a lack of resources.

--*/
{
  return RootBridgeIoPciRW (
           TRUE,
           Width,
           Address,
           Buffer
           );
}

VOID
Memzero (
  IN VOID   *Buffer,
  IN UINTN  Size
  )
{
  INT8  *Ptr;

  Ptr = Buffer;
  while (Size--) {
    *(Ptr++) = 0;
  }
}
