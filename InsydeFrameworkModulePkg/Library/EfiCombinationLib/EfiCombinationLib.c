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
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  EfiCombinationLib.c

Abstract:

--*/

#include "Tiano.h"
#include "Pei.h"

#include EFI_PPI_CONSUMER (Stall)
#include EFI_PPI_CONSUMER (CpuIo)
#include EFI_PPI_CONSUMER (PciCfg)

#include EFI_PROTOCOL_CONSUMER (PciRootBridgeIo)
#include EFI_PROTOCOL_CONSUMER (CpuIo)

#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  #include EFI_ARCH_PROTOCOL_DEFINITION (StatusCode)
  static EFI_STATUS_CODE_PROTOCOL  *gStatusCode = NULL;
#endif

VOID
AsmEfiCommonLibCopyMem (
  IN VOID   *Destination,
  IN VOID   *Source,
  IN UINTN  Length
  );

//
// Modular variable used by common libiary in PEI phase
//
EFI_GUID                        mPeiCpuIoPpiGuid  = PEI_CPU_IO_PPI_GUID;
EFI_GUID                        mPeiPciCfgPpiGuid = PEI_PCI_CFG_PPI_GUID;

EFI_PEI_SERVICES                **mPeiServices    = NULL;
PEI_CPU_IO_PPI                  *mCpuIoPpi        = NULL;
PEI_PCI_CFG_PPI                 *mPciCfgPpi       = NULL;

//
// Modular variable used by common libiary in DXE phase
//
EFI_SYSTEM_TABLE                *mST            = NULL;
EFI_BOOT_SERVICES               *mBS            = NULL;
EFI_RUNTIME_SERVICES            *mRT            = NULL;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *mRootBridgeIo  = NULL;
EFI_CPU_IO_PROTOCOL             *mCpuIo         = NULL;

EFI_STATUS
EfiInitializeCommonDriverLib (
  IN EFI_HANDLE           ImageHandle,
  IN VOID                 *SystemTable
  )
/*++

Routine Description:

  Initialize lib function calling phase: PEI or DXE

Arguments:

Returns:

  EFI_STATUS always returns EFI_SUCCESS

--*/
// GC_TODO:    ImageHandle - add argument and description to function comment
// GC_TODO:    SystemTable - add argument and description to function comment
{
  EFI_STATUS  Status;

  if (ImageHandle == NULL) {
    //
    // The function is called in PEI phase, use PEI interfaces
    //
    mPeiServices = (EFI_PEI_SERVICES **) SystemTable;
    ASSERT (mPeiServices);

    mCpuIoPpi   = (**mPeiServices).CpuIo;
    mPciCfgPpi  = (**mPeiServices).PciCfg;

  } else {
    //
    // ImageHandle is not NULL. The function is called in DXE phase
    //
    mST = SystemTable;
    mBS = mST->BootServices;
    mRT = mST->RuntimeServices;
    ASSERT (mST && mBS && mRT);

    Status = mBS->LocateProtocol (
                    &gEfiCpuIoProtocolGuid,
                    NULL,
                    (VOID **) (&mCpuIo)
                    );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    Status = mBS->LocateProtocol (
                    &gEfiPciRootBridgeIoProtocolGuid,
                    NULL,
                    (VOID **) (&mRootBridgeIo)
                    );
    //
    // Ignor error, since RootBridgeIo might be unavailable!!!!!!!!!!!!!!!!
    //
    //    if (EFI_ERROR (Status)) {
    //      return Status;
    //    }
    //
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EfiCommonIoWrite (
  IN  UINT8       Width,
  IN  UINTN       Address,
  IN  UINTN       Count,
  IN  OUT VOID    *Buffer
  )
/*++

Routine Description:

Arguments:

Returns:

  EFI_STATUS always returns EFI_SUCCESS

--*/
// GC_TODO:    Width - add argument and description to function comment
// GC_TODO:    Address - add argument and description to function comment
// GC_TODO:    Count - add argument and description to function comment
// GC_TODO:    Buffer - add argument and description to function comment
{
  EFI_STATUS  Status;

  if (mPeiServices != NULL) {
    //
    // The function is called in PEI phase, use PEI interfaces
    //
    Status = mCpuIoPpi->Io.Write (
                            mPeiServices,
                            mCpuIoPpi,
                            Width,
                            Address,
                            Count,
                            Buffer
                            );
  } else {
    //
    // The function is called in DXE phase
    //
    Status = mCpuIo->Io.Write (
                          mCpuIo,
                          Width,
                          Address,
                          Count,
                          Buffer
                          );
  }

  return Status;
}

EFI_STATUS
EfiCommonIoRead (
  IN  UINT8       Width,
  IN  UINTN       Address,
  IN  UINTN       Count,
  IN  OUT VOID    *Buffer
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
// GC_TODO:    Width - add argument and description to function comment
// GC_TODO:    Address - add argument and description to function comment
// GC_TODO:    Count - add argument and description to function comment
// GC_TODO:    Buffer - add argument and description to function comment
{
  EFI_STATUS  Status;

  if (mPeiServices != NULL) {
    //
    // The function is called in PEI phase, use PEI interfaces
    //
    Status = mCpuIoPpi->Io.Read (
                            mPeiServices,
                            mCpuIoPpi,
                            Width,
                            Address,
                            Count,
                            Buffer
                            );
  } else {
    Status = mCpuIo->Io.Read (mCpuIo, Width, Address, Count, Buffer);
  }

  return Status;
}

EFI_STATUS
EfiCommonPciWrite (
  IN  UINT8       Width,
  IN  UINT64      Address,
  IN  UINTN       Count,
  IN  OUT VOID    *Buffer
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
// GC_TODO:    Width - add argument and description to function comment
// GC_TODO:    Address - add argument and description to function comment
// GC_TODO:    Count - add argument and description to function comment
// GC_TODO:    Buffer - add argument and description to function comment
{
  EFI_STATUS  Status;
  UINTN       Index;
  UINT8       *Buffer8;

  Status = EFI_SUCCESS;

  if (mPeiServices != NULL) {
    //
    // The function is called in PEI phase, use PEI interfaces
    //
    Buffer8 = Buffer;
    for (Index = 0; Index < Count; Index++) {
      Status = mPciCfgPpi->Write (
                            mPeiServices,
                            mPciCfgPpi,
                            Width,
                            Address,
                            Buffer8
                            );

      if (EFI_ERROR (Status)) {
        return Status;
      }

      Buffer8 += Width;
    }

  } else {
    //
    // The function is called in DXE phase
    //
    Status = mRootBridgeIo->Pci.Write (
                                  mRootBridgeIo,
                                  Width,
                                  Address,
                                  Count,
                                  Buffer
                                  );
  }

  return Status;
}

EFI_STATUS
EfiCommonPciRead (
  IN  UINT8       Width,
  IN  UINT64      Address,
  IN  UINTN       Count,
  IN  OUT VOID    *Buffer
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
// GC_TODO:    Width - add argument and description to function comment
// GC_TODO:    Address - add argument and description to function comment
// GC_TODO:    Count - add argument and description to function comment
// GC_TODO:    Buffer - add argument and description to function comment
{
  EFI_STATUS  Status;
  UINTN       Index;
  UINT8       *Buffer8;

  Status = EFI_SUCCESS;

  if (mPeiServices != NULL) {
    //
    // The function is called in PEI phase, use PEI interfaces
    //
    Buffer8 = Buffer;
    for (Index = 0; Index < Count; Index++) {
      Status = mPciCfgPpi->Read (
                            mPeiServices,
                            mPciCfgPpi,
                            Width,
                            Address,
                            Buffer8
                            );

      if (EFI_ERROR (Status)) {
        return Status;
      }

      Buffer8 += Width;
    }

  } else {
    //
    // The function is called in DXE phase
    //
    Status = mRootBridgeIo->Pci.Read (
                                  mRootBridgeIo,
                                  Width,
                                  Address,
                                  Count,
                                  Buffer
                                  );
  }

  return Status;
}

EFI_STATUS
EfiCommonStall (
  IN  UINTN       Microseconds
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
// GC_TODO:    Microseconds - add argument and description to function comment
{
  EFI_STATUS    Status;
  PEI_STALL_PPI *StallPpi;

  Status = EFI_SUCCESS;

  if (mPeiServices != NULL) {
    //
    // The function is called in PEI phase, use PEI interfaces
    //
    Status = (**mPeiServices).LocatePpi (
                                mPeiServices,
                                &gPeiStallPpiGuid,
                                0,
                                NULL,
                                (VOID **) &StallPpi
                                );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    return StallPpi->Stall (mPeiServices, StallPpi, Microseconds);

  } else {
    //
    // The function is called in DXE phase
    //
    return mBS->Stall (Microseconds);
  }
}

VOID
EfiCommonCopyMem (
  IN VOID     *Destination,
  IN VOID     *Source,
  IN UINTN    Length
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
// GC_TODO:    Destination - add argument and description to function comment
// GC_TODO:    Source - add argument and description to function comment
// GC_TODO:    Length - add argument and description to function comment
{
  AsmEfiCommonLibCopyMem (Destination, Source, Length);
}

EFI_STATUS
EfiCommonAllocatePages (
  IN EFI_ALLOCATE_TYPE          Type,
  IN EFI_MEMORY_TYPE            MemoryType,
  IN UINTN                      Pages,
  IN OUT EFI_PHYSICAL_ADDRESS   *Memory
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
// GC_TODO:    Type - add argument and description to function comment
// GC_TODO:    MemoryType - add argument and description to function comment
// GC_TODO:    Pages - add argument and description to function comment
// GC_TODO:    Memory - add argument and description to function comment
{

  if (mPeiServices != NULL) {
    //
    // The function is called in PEI phase, use PEI interfaces
    //
    return (**mPeiServices).AllocatePages (
                              mPeiServices,
                              MemoryType,
                              Pages,
                              Memory
                              );
  } else {
    //
    // The function is called in DXE phase
    //
    return mBS->AllocatePages (Type, MemoryType, Pages, Memory);
  }
}

EFI_STATUS
EfiCommonLocateInterface (
  IN EFI_GUID                  *Guid,
  IN VOID                      **Interface
  )
/*++

Routine Description:

Arguments:

Returns:

--*/
// GC_TODO:    Guid - add argument and description to function comment
// GC_TODO:    Interface - add argument and description to function comment
{
  if (mPeiServices != NULL) {
    //
    // The function is called in PEI phase, use PEI interfaces
    //
    return (**mPeiServices).LocatePpi (
                              mPeiServices,
                              Guid,
                              0,
                              NULL,
                              Interface
                              );

  } else {
    //
    // The function is called in DXE phase
    //
    return mBS->LocateProtocol (Guid, NULL, Interface);
  }

}

EFI_STATUS
EfiCommonReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 * CallerId,
  IN EFI_STATUS_CODE_DATA     * Data OPTIONAL
  )
/*++

Routine Description:

  GC_TODO: Add function description

Arguments:

  CodeType  - GC_TODO: add argument description
  Value     - GC_TODO: add argument description
  Instance  - GC_TODO: add argument description
  CallerId  - GC_TODO: add argument description
  Data      - GC_TODO: add argument description

Returns:

  GC_TODO: add return values

--*/
{
  EFI_STATUS      Status;

  if (mPeiServices != NULL) {
    //
    // The function is called in PEI phase, use PEI interfaces
    //
    return (**mPeiServices).PeiReportStatusCode (
                              mPeiServices,
                              CodeType,
                              Value,
                              Instance,
                              CallerId,
                              Data
                              );
  } else {
#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
    if (gStatusCode == NULL) {
      Status = mBS->LocateProtocol (&gEfiStatusCodeRuntimeProtocolGuid, NULL, (VOID **)&gStatusCode);
      if (EFI_ERROR (Status) || gStatusCode == NULL) {
        return EFI_UNSUPPORTED;
      }
    }
    Status = gStatusCode->ReportStatusCode (CodeType, Value, Instance, CallerId, Data);
#else
    Status = mRT->ReportStatusCode (CodeType, Value, Instance, CallerId, Data);
#endif
    return Status;
  }
}
