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

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformStatusCode.c

Abstract:

  Contains WestBay specific implementations required to use status codes.

--*/

#include "MonoStatusCode.h"
#include "Port80StatusCodeLib.h"
#include "SerialStatusCodeLib.h"
#include "SimpleCpuIoLib.h"

//
// Ppi Consumed For Notification
//
#include EFI_PPI_CONSUMER (MemoryDiscovered)

//
// Function declarations
//
EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

//
// Global variables (not writable because we are in PEI)
//
static EFI_PEI_NOTIFY_DESCRIPTOR mMonoStatusCodeNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiMemoryDiscoveredPpiGuid,
  MemoryDiscoveredPpiNotifyCallback
};

//
// Lib globals used to for serial config defaults.
//
DEBUG_CODE (
  UINT16  gComBase  = 0x3f8;
  UINTN   gBps      = 115200;
  UINT8   gData     = 8;
  UINT8   gStop     = 1;
  UINT8   gParity   = 0;
  UINT8   gBreakSet = 0;
  )

#define CONFIG_PORT0   0x2E
#define INDEX_PORT0    0x2E
#define DATA_PORT0     0x2F


#define PCI_IDX        0xCF8
#define PCI_DAT        0xCFC

#define LPC_IO_DEC      (0x80)
#define IO_DEC_DEFAUL   (0x170F0010)
#define PCI_LPC_BASE    (0x8000F800)
#define PCI_LPC_REG(x)  (PCI_LPC_BASE + (x))

//
// Function implementations
//
EFI_STATUS
EFIAPI
PlatformReportStatusCode (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
/*++

Routine Description:

  Call all status code listeners in the MonoStatusCode.

Arguments:

  Same as ReportStatusCode service

Returns:

  EFI_SUCCESS     Always returns success.

--*/
{
  Port80ReportStatusCode (PeiServices, CodeType, Value, Instance, CallerId, Data);

  //
  // If we are in debug mode, we will allow serial status codes
  //
  DEBUG_CODE (
    {
      SerialReportStatusCode (PeiServices, CodeType, Value, Instance, CallerId, Data);
    }
  );

  return EFI_SUCCESS;
}

EFI_STATUS
EnableLpcIoDecode(
  VOID
  )
/*++

Routine Description:

  Enable legacy decoding on ICH6

Arguments:

  none

Returns:

  EFI_SUCCESS     Always returns success.

--*/
{
  //
  // Choose I/O base for legacy device and
  // enable I/O decoding for legacy device
  // For size consideration, just combine two operations.
  //

  IoWrite32 (PCI_IDX,  PCI_LPC_REG(LPC_IO_DEC));
  IoWrite32 (PCI_DAT,  IO_DEC_DEFAUL);
  return  EFI_SUCCESS;

}

VOID
PlatformInitializeStatusCode (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:

  INIT the SIO. Ported this code and I don't undertand the comments either.

Arguments:

  FfsHeader   - FV this PEIM was loaded from.
  PeiServices - General purpose services available to every PEIM.

Returns:

  None

--*/
{
  //
  // If we are in debug mode, we will allow serial status codes
  //
  DEBUG_CODE (
    {
      UINT16          ConfigPort;
      UINT16          IndexPort;
      UINT16          DataPort;
      UINT8           Data = 0;

      //
      // hard code for sio init
      //
      ConfigPort = CONFIG_PORT0;
      DataPort   = DATA_PORT0;
      IndexPort  = INDEX_PORT0;

      //
      //Enable LPC decode on ICH6
      //
      EnableLpcIoDecode();

      //
      // Active SIO access
      //
      IoWrite8 (ConfigPort, 0x55);

      // Check if SIO is National
      IoWrite8 (IndexPort, 0x20);
      Data = IoRead8(DataPort);
      if (Data == 0xF0) {
        IoWrite8 (IndexPort, 0x2D);
        IoWrite8 (DataPort,  0x00);
        IoWrite8 (IndexPort, 0x29);
        IoWrite8 (DataPort,  0x80);
      }

      //
      //FloppyFuncGroup->FdcWritePort(SioPort.IndexPort, REG_POWER_CONTROL);
      //
      IoWrite8 (IndexPort, 0x22);

      //
      //FloppyFuncGroup->FdcWritePort(SioPort.DataPort, 0x39);
      //
      IoWrite8 (DataPort, 0x19);

      //
      //FloppyFuncGroup->FdcWritePort ( SioPort.IndexPort, REG_LOGICAL_DEVICE ) ;
      //
      IoWrite8 (IndexPort, 0x07);

      //
      //FloppyFuncGroup->FdcWritePort ( SioPort.DataPort, SIO_COM1 ) ;
      //
      IoWrite8 (DataPort, 0x03);

      //
      //FloppyFuncGroup->FdcWritePort(SioPort.IndexPort, BASE_ADDRESS_LOW);
      //
      IoWrite8 (IndexPort, 0x61);

      //
      //FloppyFuncGroup->FdcWritePort(SioPort.DataPort, 0xf8);
      //
      IoWrite8 (DataPort, 0xf8);

      //
      //FloppyFuncGroup->FdcWritePort(SioPort.IndexPort, BASE_ADDRESS_HIGH);
      //
      IoWrite8 (IndexPort, 0x60);

      //
      //FloppyFuncGroup->FdcWritePort(SioPort.DataPort, 0x03);
      //
      IoWrite8 (DataPort, 0x03);

      //
      //FloppyFuncGroup->FdcWritePort(SioPort.IndexPort, PRIMARY_INTERRUPT_SELECT);
      //
      IoWrite8 (IndexPort, 0x70);

      //
      //FloppyFuncGroup->FdcWritePort(SioPort.DataPort, 0x04);
      //
      IoWrite8 (DataPort, 0x04);

      //
      //FloppyFuncGroup->FdcWritePort(SioPort.IndexPort, ACTIVATE);
      //
      IoWrite8 (IndexPort, 0x30);

      //
      //FloppyFuncGroup->FdcWritePort(SioPort.DataPort, 1);
      //
      IoWrite8 (DataPort, 0x01);

      //
      // Diable SIO access
      //
      IoWrite8 (ConfigPort, 0xAA);

      //
      // Initialize additional debug status code listeners.
      //
      SerialInitializeStatusCode ();
    }
  );
}

EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES       **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                   *Ppi
  )
/*++

Routine Description:

  Once main memory has been initialized, build a hob describing the status code
  listener that has been installed.


Arguments:

  PeiServices      - General purpose services available to every PEIM.
  NotifyDescriptor - Information about the notify event.
  Ppi              - Context

Returns:

  EFI_SUCCESS     The function should always return success.

--*/
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     BootMode;

  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  if (!EFI_ERROR(Status) && (BootMode == BOOT_ON_S3_RESUME)) {
    return Status;
  }
  //
  // Publish the listener in a HOB for DXE use.
  //
  InitializeDxeReportStatusCode (PeiServices);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
InstallMonoStatusCode (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:

  Install the PEIM.  Initialize listeners, publish the PPI and HOB for PEI and
  DXE use respectively.

Arguments:

  FfsHeader   - FV this PEIM was loaded from.
  PeiServices - General purpose services available to every PEIM.

Returns:

  EFI_SUCCESS   The function always returns success.

--*/
{
 // EFI_STATUS    Status;

  //
  // Initialize all listeners
  //
  InitializeMonoStatusCode (FfsHeader, PeiServices);

/*
  //
  // Post a callback to make this status code service available to
  // DXE after memory has been initialized
  //
  Status = (**PeiServices).NotifyPpi (PeiServices, &mMonoStatusCodeNotifyList);
  if (EFI_ERROR(Status)) {
    return Status;
  }
*/
  return EFI_SUCCESS;
}
