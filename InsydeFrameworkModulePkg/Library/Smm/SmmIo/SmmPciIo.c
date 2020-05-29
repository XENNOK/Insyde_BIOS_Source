//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;
//;   SMM PCI config space I/O access utility implementation file, for Ia32
//;
#include "SmmIoLib.h"

STATIC
EFI_STATUS
SmmSingleSegmentPciAccess (
  IN EFI_SMM_CPU_IO_INTERFACE    *CpuIo,
  IN BOOLEAN                     IsWrite,
  IN SMM_PCI_IO_WIDTH            Width,
  IN SMM_PCI_IO_ADDRESS          *Address,
  IN OUT VOID                    *Buffer
  );

EFI_STATUS
SmmPciCfgRead (
  IN SMM_PCI_IO_WIDTH       Width,
  IN SMM_PCI_IO_ADDRESS     *Address,
  IN OUT VOID               *Buffer
  )
/*++

Routine Description:

  Read value from the specified PCI config space register

Arguments:

  Width   - The width (8, 16 or 32 bits) of accessed pci config space register
  Address - The address of the accessed pci register (bus, dev, func, offset)
  Buffer  - The returned value

Returns:

  EFI_SUCCESS           - All operations successfully
  EFI_INVALID_PARAMETER - Width is not valid or dosn't match register address
  Other error code      - If any error occured when calling libiary functions

--*/
{
  EFI_SMM_CPU_IO_INTERFACE    *SmmCpuIo;

  ASSERT (mSmst);

  SmmCpuIo = &(mSmst->SmmIo);

  return SmmSingleSegmentPciAccess (SmmCpuIo, FALSE, Width, Address, Buffer);
}

EFI_STATUS
SmmPciCfgWrite (
  IN SMM_PCI_IO_WIDTH       Width,
  IN SMM_PCI_IO_ADDRESS     *Address,
  IN OUT VOID               *Buffer
  )
/*++

Routine Description:

  Write value into the specified PCI config space register

Arguments:

  Width   - The width (8, 16 or 32 bits) of accessed pci config space register
  Address - The address of the accessed pci register (bus, dev, func, offset)
  Buffer  - The returned value

Returns:

  EFI_SUCCESS           - All operations successfully
  EFI_INVALID_PARAMETER - Width is not valid or dosn't match register address
  Other error code      - If any error occured when calling libiary functions

--*/
{
  EFI_SMM_CPU_IO_INTERFACE    *SmmCpuIo;

  ASSERT (mSmst);

  SmmCpuIo = &(mSmst->SmmIo);

  return SmmSingleSegmentPciAccess (SmmCpuIo, TRUE, Width, Address, Buffer);
}

STATIC
EFI_STATUS
SmmSingleSegmentPciAccess (
  IN EFI_SMM_CPU_IO_INTERFACE    *CpuIo,
  IN BOOLEAN                     IsWrite,
  IN SMM_PCI_IO_WIDTH            Width,
  IN SMM_PCI_IO_ADDRESS          *Address,
  IN OUT VOID                    *Buffer
  )
/*++

Routine Description:

  Access a PCI config space address, including read and write

Arguments:

  CpuIo   - The cpu I/O accessing interface provided by EFI runtime sys table
  IsWrite - Indicates whether this operation is a write access or read
  Width   - The width (8, 16 or 32 bits) of accessed pci config space register
  Address - The address of the accessed pci register (bus, dev, func, offset)
  Buffer  - The returned value when this is a reading operation or the data
            to be written when this is a writing one

Returns:

  EFI_SUCCESS           - All operations successfully
  EFI_INVALID_PARAMETER - Width is not valid or dosn't match register address
  Other error code      - If any error occured when calling libiary functions

--*/
{
  EFI_STATUS                        Status;
  PCI_CONFIG_ACCESS_CF8             PciCf8Data;
  UINT64                            PciDataReg;

  //
  // PCI Config access are all 32-bit alligned, but by accessing the
  // CONFIG_DATA_REGISTER (0xcfc) with different widths more cycle types
  // are possible on PCI.
  //
  // To read a byte of PCI config space you load 0xcf8 and
  // read 0xcfc, 0xcfd, 0xcfe, 0xcff
  //
  // The validation of passed in arguments "Address" will be checked in the
  // CPU IO functions, so we don't check them here
  //

  if (Width >= SmmPciWidthMaximum) {
    return EFI_INVALID_PARAMETER;
  }

  PciCf8Data.Reg      = Address->Register & 0xfc;
  PciCf8Data.Func     = Address->Function;
  PciCf8Data.Dev      = Address->Device;
  PciCf8Data.Bus      = Address->Bus;
  PciCf8Data.Reserved = 0;
  PciCf8Data.Enable   = 1;

  Status = CpuIo->Io.Write (CpuIo, SmmPciWidthUint32, 0xcf8, 1, &PciCf8Data);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  PciDataReg = 0xcfc + (Address->Register & 0x03);

  if (IsWrite) {
    //
    // This is a Pci write operation, write data into (0xcfc + offset)
    //
    Status = CpuIo->Io.Write (CpuIo, Width, PciDataReg, 1, Buffer);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  } else {
    //
    // This is a Pci Read operation, read returned data from (0xcfc + offset)
    //
    Status = CpuIo->Io.Read (CpuIo, Width, PciDataReg, 1, Buffer);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}
