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
  This file contains an 'Intel Pre-EFI Module' and is licensed
  for Intel CPUs and Chipsets under the terms of your license
  agreement with Intel or your vendor.  This file may be
  modified by the user, subject to additional terms of the
  license agreement
--*/
/*++

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
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

  Contains SR870BH2/SR870BN4 specific implementations required to
  use status codes.

--*/

#include "MonoStatusCode.h"
#include "SerialStatusCodeLib.h"
#include "SimpleCpuIoLib.h"

//
// Ppi Consumed For Notification
//
#include EFI_PPI_CONSUMER (SecPlatformInformation)
#include EFI_PPI_CONSUMER (ChipsetInited)
#include EFI_PPI_CONSUMER (MemoryDiscovered)

//
// Platform definitions
//
#define CONFIG_PORT0  0x02E
#define INDEX_PORT0   0x02E
#define DATA_PORT0    0x02F

//
// Logical device in Lpc47b27x
//
#define SIO_FLOPPY    0x00
#define SIO_PARA      0x03
#define SIO_COM1      0x04
#define SIO_COM2      0x05
#define SIO_KYBD      0x07
#define SIO_GAMEPORT  0x09
#define SIO_PME       0x0A
#define SIO_MPU411    0x0B

//
// Global registers in Lpc47b27x
//
#define REG_CONFIG_CONTROL        0x02
#define REG_LOGICAL_DEVICE        0x07
#define REG_DEVICE_ID             0x20
#define REG_DEVICE_REV            0x21
#define REG_POWER_CONTROL         0x22
#define REG_POWER_MGMT            0x23

#define ACTIVATE                  0x30
#define BASE_ADDRESS_HIGH         0x60
#define BASE_ADDRESS_LOW          0x61
#define PRIMARY_INTERRUPT_SELECT  0x70
#define SECOND_INTERRUPT_SELECT   0x72
#define DMA_CHANNEL_SELECT        0x74

//
// Register for COM1
//
#define SERIAL_MODE 0xF0

//
// Register for Parallel Port
//
#define PARALLEL_MODE   0xF0
#define PARALLEL_MODE_2 0xF1

//
// Bit definition for parallel mode
//
#define PARALLEL_MODE_MASK    0x0F
#define PARALLEL_MODE_SPP     0x01
#define PARALLEL_MODE_EPP     0x02
#define PARALLEL_MODE_ECP     0x04
#define PARALLEL_MODE_PRINTER 0x08

//
// Register for Floppy Disk Controller
//
#define FDD_MODE    0xF0
#define FDD_OPTION  0xF1
#define FDD_TYPE    0xF2
#define FDD0        0xF4
#define FDD1        0xF5

//
// Bit definitions for floppy write protect feature
//
#define FDD_WRITE_PROTECT     0x08
#define FDD_WRITE_PROTECT_EN  0x08
#define FDD_WRITE_PROTECT_DIS 0x00

//
// Register for Keyboard Controller
//
#define KRESET_GATE20_SELECT  0xF0

//
// PME I/O block Base address
//
#define DEFAULT_SIO_PME_IO_BASE 0x3000

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

EFI_STATUS
EFIAPI
ChipsetInitedPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

extern EFI_PEI_PPI_DESCRIPTOR mPpiListStatusCode;


//
// Global variables (not writable because we are in PEI)
//
static EFI_PEI_NOTIFY_DESCRIPTOR  mMonoStatusCodeNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiMemoryDiscoveredPpiGuid,
  MemoryDiscoveredPpiNotifyCallback
};

static EFI_PEI_NOTIFY_DESCRIPTOR  mMonoStatusCodeFirstRoundNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiChipsetInitedPpiGuid,
  ChipsetInitedPpiNotifyCallback
};



//
// Lib globals used to for serial config defaults.
//
DEBUG_CODE (
  UINT16 gComBase = 0x2f8;
  UINTN gBps      = 115200;
  UINT8 gData     = 8;
  UINT8 gStop     = 1;
  UINT8 gParity   = 0;
  UINT8 gBreakSet = 0;
)

VOID
WriteRegister (
  IN  UINT8   Index,
  IN  UINT8   Data
  )
/*++

Routine Description:

  Write the register with the special value

Arguments:

  Index  - The index of the register
  Data   - The special data which will be set

Returns:

  None

--*/
{
  IoWrite8 (INDEX_PORT0, Index);
  IoWrite8 (DATA_PORT0, Data);

  return ;
}
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
  IN EFI_GUID                 * CallerId,
  IN EFI_STATUS_CODE_DATA     * Data OPTIONAL
  )
/*++

Routine Description:

  Call all status code listeners in the MonoStatusCode.

Arguments:

  PeiServices - The PEI core services table.
  CodeType    - Type of Status Code.
  Value       - Value to output for Status Code.
  Instance    - Instance Number of this status code.
  CallerId    - ID of the caller of this status code.
  Data        - Optional data associated with this status code.

Returns:

  EFI_SUCCESS     Always returns success.

--*/
{
  //
  // If we are in debug mode, we will allow serial status codes
  //
  DEBUG_CODE (
    SerialReportStatusCode (
      PeiServices,
      CodeType,
      Value,
      Instance,
      CallerId,
      Data
      );
  );

  return EFI_SUCCESS;
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
    UINT16 IndexPort;
    UINT16 ConfigPort;
    UINT16 DataPort;
    UINT16 PmeIoBase;

    //
    // hard code for sio init
    //
    ConfigPort  = CONFIG_PORT0;
    DataPort    = DATA_PORT0;
    IndexPort   = INDEX_PORT0;
    PmeIoBase   = 0x1000;

    //
    // Start Configuration Mode
    //
    IoWrite8 (ConfigPort, 0x55);

    //
    // Initialize FDD controller
    //
    WriteRegister (REG_LOGICAL_DEVICE, SIO_FLOPPY);

    //
    // Enable FDD
    //
    WriteRegister (ACTIVATE, 0x01);

    //
    // Initialize Parallel port
    //
    WriteRegister (REG_LOGICAL_DEVICE, SIO_PARA);

    //
    // Enable Parallel port
    //
    WriteRegister (ACTIVATE, 0x01);

    //
    // Parallel Base Addr
    //
    WriteRegister (BASE_ADDRESS_HIGH, 0x03);
    WriteRegister (BASE_ADDRESS_LOW, 0x78);

    //
    // Parallel Port interrupt selection
    //
    WriteRegister (PRIMARY_INTERRUPT_SELECT, 0x05);

    //
    // Initialize Serial port1 (COM1)
    //
    WriteRegister (REG_LOGICAL_DEVICE, SIO_COM1);

    //
    // Enable Parallel port
    //
    WriteRegister (ACTIVATE, 0x01);

    //
    // COM1 Base Addr
    //
    WriteRegister (BASE_ADDRESS_HIGH, 0x03);
    WriteRegister (BASE_ADDRESS_LOW, 0xf8);

    //
    // COM1 interrupt selection
    //
    WriteRegister (PRIMARY_INTERRUPT_SELECT, 0x04);

    //
    // Initialize Serial port2 (COM2)
    //
    //
    // Since signals on Lpc47b27x are connected to shared pins
    // 95 (GP52/RXD2/IRQ5/IRRX) & 96 (GP53/TXD2/IRQ7/IRTX)
    //
    // First Program the shared pins as TXD2 & TXD2
    // This is done by configuring PME Function (0xA) registers
    //
    WriteRegister (REG_LOGICAL_DEVICE, SIO_PME);
    WriteRegister (ACTIVATE, 0x01);

    WriteRegister (BASE_ADDRESS_HIGH, 0x10);
    WriteRegister (BASE_ADDRESS_LOW, 0x0);

    //
    // Program GP52 as I/P & RXD2 (Alternate Function)
    //
    IoWrite8 ((UINT16) (PmeIoBase + 0x41), 0x05);

    //
    // Program GP53 as O/P & TXD2 (Alternate Function)
    //
    IoWrite8 ((UINT16) (PmeIoBase + 0x42), 0x04);

    //
    // Program GP54 as I/P & DSR2 (Alternate Function)
    //
    IoWrite8 ((UINT16) (PmeIoBase + 0x43), 0x05);

    //
    // Program GP55 as O/P & RTS2 (Alternate Function)
    //
    IoWrite8 ((UINT16) (PmeIoBase + 0x44), 0x04);

    //
    // Program GP56 as I/P & CTS2 (Alternate Function)
    //
    IoWrite8 ((UINT16) (PmeIoBase + 0x45), 0x05);

    //
    // Program GP57 as O/P & DTR2 (Alternate Function)
    //
    IoWrite8 ((UINT16) (PmeIoBase + 0x46), 0x04);

    //
    // Program GP50 as I/P & RI2 (Alternate Function)
    //
    IoWrite8 ((UINT16) (PmeIoBase + 0x3F), 0x05);

    //
    // Program GP51 as I/P & DCD2 (Alternate Function)
    //
    IoWrite8 ((UINT16) (PmeIoBase + 0x40), 0x05);

    //
    // Now all pins are directed to COM2. So we can enable COM2
    //
    WriteRegister (REG_LOGICAL_DEVICE, SIO_COM2);
    WriteRegister (ACTIVATE, 0x01);

    //
    // COM2 Base Addr
    //
    WriteRegister (BASE_ADDRESS_HIGH, 0x02);
    WriteRegister (BASE_ADDRESS_LOW, 0xf8);

    //
    // COM2 interrupt selection
    //
    WriteRegister (PRIMARY_INTERRUPT_SELECT, 0x03);

    //
    // Initialize keyboard controller
    //
    WriteRegister (REG_LOGICAL_DEVICE, SIO_KYBD);
    WriteRegister (ACTIVATE, 0x01);

    //
    // KB primary IRQ: 0x01 and secondary IRQ: 0x0c
    //
    WriteRegister (PRIMARY_INTERRUPT_SELECT, 0x01);
    WriteRegister (SECOND_INTERRUPT_SELECT, 0x0c);
    WriteRegister (KRESET_GATE20_SELECT, 0x18);

    //
    // Program GP36/nKBRST as O/P & nKBDRST (Alternate Function)
    //
    IoWrite8 ((UINT16) (PmeIoBase + 0x39), 0x04);

    //
    // Program GP37/A20M as A20M (Alternate Function) & O/P & Non Inv
    //
    IoWrite8 ((UINT16) (PmeIoBase + 0x3A), 0x04);

    //
    // Finally, disable PME since it may conflict with parallel port in Lpc47b27x
    //
    WriteRegister (REG_LOGICAL_DEVICE, SIO_PME);
    WriteRegister (ACTIVATE, 0);

    //
    // End of Configuration Mode
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
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
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
  EFI_STATUS    Status;
  EFI_BOOT_MODE BootMode;

  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  if (!EFI_ERROR (Status) && (BootMode == BOOT_ON_S3_RESUME)) {
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
ChipsetInitedPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
/*++

Routine Description:

  Once chipset has been initialized, do platform specific initialization.


Arguments:

  PeiServices      - General purpose services available to every PEIM.
  NotifyDescriptor - Information about the notify event.
  Ppi              - Context

Returns:

  EFI_SUCCESS     The function should always return success.

--*/
{
  EFI_STATUS    Status;

  PlatformInitializeStatusCode (NULL, PeiServices);

  //
  // Publish the status code capability to other modules
  //
  Status = (*PeiServices)->InstallPpi (PeiServices, &mPpiListStatusCode);

  ASSERT_PEI_ERROR(PeiServices, Status);

  PEI_DEBUG ((PeiServices, EFI_D_ERROR, "\nMono Status Code PEIM Loaded\n"));

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
  EFI_STATUS  Status;
  EFI_SEC_PLATFORM_INFORMATION_PPI  *SecPlatformInfoPpi;
  IPF_HANDOFF_STATUS                IpfStatus;
  UINT64                            RecordSize;

  Status = (**PeiServices).LocatePpi (
                             PeiServices,
                             &gEfiSecPlatformInformationPpiGuid,
                             0,
                             NULL,
                             &SecPlatformInfoPpi
                             );
  ASSERT_PEI_ERROR (PeiServices, Status);

  RecordSize = sizeof (IpfStatus);
  SecPlatformInfoPpi->PlatformInformation (
                        PeiServices,
                        &RecordSize,
                        (SEC_PLATFORM_INFORMATION_RECORD *)&IpfStatus
                        );

  if ((IpfStatus.BootPhase & 0xFF) == 0x03) {

    //
    // Install notify function to initialized the SERIAL port after Chipset has been inited
    //
    Status = (**PeiServices).NotifyPpi (PeiServices, &mMonoStatusCodeFirstRoundNotifyList);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  } else {

    //
    // Install the StatusCode PPI directly in normal boot phase.
    //
    Status = (*PeiServices)->InstallPpi (PeiServices, &mPpiListStatusCode);

    ASSERT_PEI_ERROR(PeiServices, Status);

    PEI_DEBUG ((PeiServices, EFI_D_ERROR, "\nMono Status Code PEIM Loaded\n"));
  }

  Status = (**PeiServices).NotifyPpi (PeiServices, &mMonoStatusCodeNotifyList);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}
