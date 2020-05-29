/** @file
  Initializes the PCH Serial IO Controllers.

@copyright
  Copyright (c) 2012 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#include "PchInit.h"

#ifdef SERIAL_IO_FLAG
#include <Library/PchAslUpdateLib.h>

#include <Protocol/AcpiTable.h>



#define SERIAL_IO_ADDRESS_AREA 0xFE101000;

//
// Resource Group Header
//
typedef struct {
  UINT32 ResourceGroupLength;
  UINT32 VendorId;
  UINT32 SubVendorId;
  UINT16 DeviceId;
  UINT16 SubDeviceId;
  UINT16 Revision;
  UINT16 Reserved;
  UINT32 SharedInfoLength;
} EFI_ACPI_5_0_CSRT_RESOURCE_GROUP_HEADER;

//
// Resource Group1 Share Info
//
typedef struct {
 UINT16 MajorVersion1;
 UINT16 MinorVersion0;
 UINT32 MmioBaseL;
 UINT32 MmioBaseH;
 UINT32 InterruptGSI;
 UINT8  InterruptPolarity;
 UINT8  InterruptMode;
 UINT8  NumberofChannels;
 UINT8  DMAAddressWidth;
 UINT16 BaseRequestLine;
 UINT16 NumberofHandshakeSignals;
 UINT32 MaximumBlockTransferSize;
} EFI_ACPI_5_0_RESOURCE_GROUP1_SHARED_INFO;

//
// Resource Descriptor Header
//
typedef struct {
  UINT32 ResourceDescriptorLength;
  UINT16 ResourceType;
  UINT16 ResourceSubType;
  UINT32 UID;
} EFI_ACPI_5_0_CSRT_RESOURCE_DESCRIPTOR_HEADER;

//
// Core System Resources Table Structure (CSRT)
//
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER                  Header;
  EFI_ACPI_5_0_CSRT_RESOURCE_GROUP_HEADER      ResourceGroup1;
  EFI_ACPI_5_0_RESOURCE_GROUP1_SHARED_INFO     ResourceGroup1SharedInfo;
  EFI_ACPI_5_0_CSRT_RESOURCE_DESCRIPTOR_HEADER ControllerResourceDescriptor1;
  EFI_ACPI_5_0_CSRT_RESOURCE_DESCRIPTOR_HEADER ChannelResourceDescriptor1[8];
} EFI_ACPI_5_0_CORE_SYSTEM_RESOURCES_TABLE;

typedef enum {
  INDEX_DMA,
  INDEX_I2C0,
  INDEX_I2C1,
  INDEX_SPI0,
  INDEX_SPI1,
  INDEX_UART0,
  INDEX_UART1,
  INDEX_SDIO,
  MAX_SIO_INDEX
} SIO_DEVICE;

typedef struct {
  UINT8  DevNum;
  UINT8  FuncNum;
  UINT32 DisableAddr;
  UINT32 AcpiSwitchAddr;
  UINT32 DeviceModeSwitchBit; // turns device into ACPI mode by hiding its PCI config space
  UINT32 IrqModeSwitchBit;    // turns device's interrupts into ACPI mode
  UINT32 Signature;
  UINT8  IrqPin;
  UINT32 Bar0;
  UINT32 Bar1;
} SERIAL_IO_DEVICE_DESCRIPTOR;

#define SERIAL_IO_DEVICE_QUANTITY 8

SERIAL_IO_DEVICE_DESCRIPTOR SerialIoDevice [SERIAL_IO_DEVICE_QUANTITY] = 
{ {21, 0, 0xCE00AA07, 0xCB000240, BIT20, BIT21, EFI_SIGNATURE_32('S','D','M','A'), 2, 0xFE101000, 0xFE102000},
  {21, 1, 0xCE00AA47, 0xCB000248, BIT20, BIT21, EFI_SIGNATURE_32('I','2','C','0'), 3, 0xFE103000, 0xFE104000},
  {21, 2, 0xCE00AA87, 0xCB000250, BIT20, BIT21, EFI_SIGNATURE_32('I','2','C','1'), 3, 0xFE105000, 0xFE106000},
  {21, 3, 0xCE00AAC7, 0xCB000258, BIT20, BIT21, EFI_SIGNATURE_32('S','P','I','0'), 3, 0xFE107000, 0xFE108000},
  {21, 4, 0xCE00AB07, 0xCB000260, BIT20, BIT21, EFI_SIGNATURE_32('S','P','I','1'), 3, 0xFE109000, 0xFE10A000},
  {21, 5, 0xCE00AB47, 0xCB000268, BIT20, BIT21, EFI_SIGNATURE_32('U','A','0','0'), 4, 0xFE10B000, 0xFE10C000},
  {21, 6, 0xCE00AB87, 0xCB000270, BIT20, BIT21, EFI_SIGNATURE_32('U','A','0','1'), 4, 0xFE10D000, 0xFE10E000},
  {23, 0, 0xCE00AE07, 0xCB000000, BIT4,  BIT5,  EFI_SIGNATURE_32('S','D','H','C'), 1, 0xFE110000, 0xFE112000}
};

EFI_STATUS
InstallDmaAcpiTable (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy
);

BOOLEAN IsSerialIoDeviceEnabled (
  SIO_DEVICE Device
  )
{
  UINTN  RegBase;
  UINT32 VendorDeviceId;

  RegBase = MmPciAddress (0, 0, SerialIoDevice[Device].DevNum, SerialIoDevice[Device].FuncNum, 0);
  VendorDeviceId = MmioRead32 (RegBase + R_PCH_LP_SERIAL_IO_VENDOR_ID);
  if (VendorDeviceId == 0xFFFFFFFF) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Disable Serial IO Controllers based on PchPlatformPolicy.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar address of this PCH device

  @retval None
**/
VOID
DisableSerialIoControllers (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy,
  IN UINT32                           RootComplexBar
  )
{
  UINT8       FunctionEnable[SERIAL_IO_DEVICE_QUANTITY];
  UINTN       Address;
  UINT8       i;
  BOOLEAN     Func0Missing;

  //
  // Higher functions of a PCI device can't exist if there's no Function0
  // This issue only applies in PCI mode, not in ACPI.
  //
  Func0Missing = PchPlatformPolicy->DeviceEnabling->SerialIoDma == PCH_DEVICE_DISABLE && 
                 PchPlatformPolicy->SerialIoConfig->SerialIoMode == PchSerialIoIsPci;

  //
  // DMA is useless without at least one SerialIo device
  //
  FunctionEnable[INDEX_DMA] = PchPlatformPolicy->DeviceEnabling->SerialIoDma &&
                              ( PchPlatformPolicy->DeviceEnabling->SerialIoI2c0 ||
                                PchPlatformPolicy->DeviceEnabling->SerialIoI2c1 ||
                                PchPlatformPolicy->DeviceEnabling->SerialIoSpi0 ||
                                PchPlatformPolicy->DeviceEnabling->SerialIoSpi1 ||
                                PchPlatformPolicy->DeviceEnabling->SerialIoUart0 ||
                                PchPlatformPolicy->DeviceEnabling->SerialIoUart1 ||
                                PchPlatformPolicy->DeviceEnabling->SerialIoSdio );

  FunctionEnable[INDEX_I2C0]  = PchPlatformPolicy->DeviceEnabling->SerialIoI2c0 && !Func0Missing;
  FunctionEnable[INDEX_I2C1]  = PchPlatformPolicy->DeviceEnabling->SerialIoI2c1 && !Func0Missing;
  FunctionEnable[INDEX_SPI0]  = PchPlatformPolicy->DeviceEnabling->SerialIoSpi0 && !Func0Missing;
  FunctionEnable[INDEX_SPI1]  = PchPlatformPolicy->DeviceEnabling->SerialIoSpi1 && !Func0Missing;
  FunctionEnable[INDEX_UART0] = PchPlatformPolicy->DeviceEnabling->SerialIoUart0 && !Func0Missing;
  FunctionEnable[INDEX_UART1] = PchPlatformPolicy->DeviceEnabling->SerialIoUart1 && !Func0Missing;
  //
  // SDIO doesn't care about DMA missing - it has its own device number
  //
  FunctionEnable[INDEX_SDIO]  = PchPlatformPolicy->DeviceEnabling->SerialIoSdio;

  DEBUG ((EFI_D_INFO, "DisableSerialIoControllers() Start\n"));

  ///
  /// PCH BIOS Spec Rev 0.7.0 Section 23.2 Disabling Serial IO Controllers
  /// By default all controllers are enabled in hardware.
  ///

  for (i = 0; i < SERIAL_IO_DEVICE_QUANTITY; i++) {
    if (FunctionEnable[i] == PCH_DEVICE_DISABLE) {
      ///
      /// Step 1
      /// Set the Dx:Fx:84h[1:0] = 11b
      ///
      Address = MmPciAddress (0, 0, SerialIoDevice[i].DevNum, SerialIoDevice[i].FuncNum, R_PCH_LP_SERIAL_IO_PME_CTRL_STS);
      MmioAndThenOr32WithScript(Address, (UINT32)~(B_PCH_LP_SERIAL_IO_PME_CTRL_STS_PWR_ST), BIT1 | BIT0 );
      ///
      /// Step 2
      /// Program IOBP register CE00Axx7h[8] = 1b
      ///
      ProgramIobpWithScript (RootComplexBar, SerialIoDevice[i].DisableAddr, (UINT32)~(BIT8), BIT8);
    }
  }

  DEBUG ((EFI_D_INFO, "DisableSerialIoControllers() End\n"));
}

UINT8
GetSerialIoIrqNumber(
  UINT8 DeviceNumber,
  UINT8 InterruptPin,
  UINT8 InterruptMode
)
{
  UINT32 IrqRoute;
  UINT8  InterruptRoute;
  UINT32 RootComplexBar;
  UINT32 IrqRoutingRegister;

  if (InterruptMode == PchSerialIoIsAcpi) {
    //
    // ACPI IRQs are wired to irq pins 5,6,7,13
    //
    switch(InterruptPin) {
      case 0x1:
        return 5;
        break;
      case 0x2:
        return 6;
        break;
      case 0x3:
        return 7;
        break;
      case 0x4:
        return 13;
        break;
      case 0x0:
      default:
        return 0;
    }
  } else {
    if(InterruptPin > 0) {
      //
      // PCI INTs are first routed to PIRQs according to R_PCH_RCRB_D21IR / R_PCH_RCRB_D23IR register
      //
      RootComplexBar = MmioRead32 (
                        MmPciAddress (0,
                        0,
                        PCI_DEVICE_NUMBER_PCH_LPC,
                        PCI_FUNCTION_NUMBER_PCH_LPC,
                        R_PCH_LPC_RCBA)
                        ) & B_PCH_LPC_RCBA_BAR;

      if (DeviceNumber == 21) {
        IrqRoutingRegister = R_PCH_RCRB_D21IR;
      } else if (DeviceNumber == 23) {
        IrqRoutingRegister = R_PCH_RCRB_D23IR;
      } else {
        ASSERT(FALSE);
        return 0;
      }
      IrqRoute = MmioRead32 (RootComplexBar + IrqRoutingRegister);

      InterruptRoute = ( IrqRoute >> (4* (InterruptPin-1) ) ) & 0x7;

      //
      // ...and PIRQs are wired to irq pins 16..23
      // A 16, B 17, C 18, D 19, E 20, F 21, G 22, H 23
      //
      return (InterruptRoute + 16);
    } else {
      return 0;
    }
  }
}


/**
  Update ASL definitions for SerialIo devices.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
UpdateSerialIoAcpiData (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy,
  IN UINT32                           RootComplexBar
  )
{

  EFI_STATUS                                Status;
  UINTN                                     RegBase;
  UINT32                                    Data32;
  UINT16                                    GpioBase;
  UINT8                                     i;
 
  Status = InitializePchAslUpdateLib();
  
  if(EFI_ERROR(Status)) {
   return Status;
  }
  
  //
  // ACPI code update for all devices
  //
  for (i=0; i<SERIAL_IO_DEVICE_QUANTITY; i++) {
  
    if (!IsSerialIoDeviceEnabled(i)) {
      continue;
    }
    
    RegBase = MmPciAddress (0, 0, SerialIoDevice[i].DevNum, SerialIoDevice[i].FuncNum, 0);
    
    Data32 = (MmioRead32(RegBase + R_PCH_LP_SERIAL_IO_BAR0) & B_PCH_LP_SERIAL_IO_BAR0_BAR);
    UpdateResourceTemplateAslCode(SerialIoDevice[i].Signature, 
                                  EFI_SIGNATURE_32 ('R', 'B', 'U', 'F'), 
                                  AML_MEMORY32_FIXED_OP, 
                                  1,   
                                  0x04,
                                  &Data32,
                                  sizeof(Data32)
                                  );
  }
  
  //
  // Update GPIO device ACPI variables
  //
  RegBase =  MmPciAddress (0, 0, PCI_DEVICE_NUMBER_PCH_LPC, PCI_FUNCTION_NUMBER_PCH_LPC, 0);
  
  GpioBase = (MmioRead16(RegBase + R_PCH_LPC_GPIO_BASE) & B_PCH_LPC_GPIO_BASE_BAR);
  
  //
  // Update _MIN[32:0] of the DWord Address Space Descriptor with GPIO BAR0
  //
  Data32 = (UINT32)GpioBase;
  UpdateResourceTemplateAslCode((EFI_SIGNATURE_32 ('G', 'P', 'I', '0')),
                               (EFI_SIGNATURE_32 ('R', 'B', 'U', 'F')),
                                 AML_DWORD_OP,
                                 1,
                                 0x0A,
                                 &Data32,
                                 sizeof(Data32)
                                 );
  
  //
  // Update _MAX[32:0] of the DWord Address Space Descriptor with GPIO BAR0 + 3FFh
  //
  Data32 = (UINT32)GpioBase + 0x3FF;
  UpdateResourceTemplateAslCode((EFI_SIGNATURE_32 ('G', 'P', 'I', '0')),
                               (EFI_SIGNATURE_32 ('R', 'B', 'U', 'F')),
                                 AML_DWORD_OP,
                                 1,
                                 0x0E,
                                 &Data32,
                                 sizeof(Data32)
                                 );
  
  
  return Status;
}

/**
  Hide PCI config space of Serial IO Controllers and do any final initialization.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance

  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
PutSerialIoInAcpiMode (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy,
  IN UINT32                           RootComplexBar
  )
{
  EFI_STATUS Status;
  UINTN      Bar1;
  UINTN      RegBase;
  UINT32     Data32;
  UINT32     Data32And;
  UINT32     Data32Or;
  UINT8      i;

  Status = EFI_SUCCESS;

  Status = InitializePchAslUpdateLib();

  if(EFI_ERROR(Status)) {
    return Status;
  }

  //
  // ACPI code update for all devices
  //
  for (i=0; i<SERIAL_IO_DEVICE_QUANTITY; i++) {

    if (!IsSerialIoDeviceEnabled(i)) {
      continue;
    }
    
    RegBase = MmPciAddress (0, 0, SerialIoDevice[i].DevNum, SerialIoDevice[i].FuncNum, 0);

    Data32 = (MmioRead32(RegBase + R_PCH_LP_SERIAL_IO_BAR1) & B_PCH_LP_SERIAL_IO_BAR1_BAR);
    UpdateResourceTemplateAslCode(EFI_SIGNATURE_32 ('S', 'I', 'R', 'C'),
                                  EFI_SIGNATURE_32 ('B', 'U', 'F', ('1'+i)),
                                  AML_MEMORY32_FIXED_OP,
                                  1,
                                  0x04,
                                  &Data32,
                                  sizeof(Data32)
                                  );
    
    Data32 = GetSerialIoIrqNumber(SerialIoDevice[i].DevNum, SerialIoDevice[i].IrqPin, PchPlatformPolicy->SerialIoConfig->SerialIoInterruptMode);
    UpdateResourceTemplateAslCode(SerialIoDevice[i].Signature,
                                  EFI_SIGNATURE_32 ('R', 'B', 'U', 'F'),
                                  AML_INTERRUPT_DESC_OP,
                                  1,
                                  0x05,
                                  &Data32,
                                  sizeof(UINT8)
                                  );
  
  }
  
   //
   // Install DMA CSRT ACPI table as per ACPI5.0 spec
   //
   if(PchPlatformPolicy->DeviceEnabling->SerialIoDma == PCH_DEVICE_ENABLE) {
     InstallDmaAcpiTable(PchPlatformPolicy);
   }

  ///
  /// PCH BIOS Spec Rev 0.7.0 Section 23.6 Enable Serial IO PCI Controllers ACPI Mode
  ///

  for (i = 0; i < SERIAL_IO_DEVICE_QUANTITY; i++) {
    ///
    /// Check if SerialIo device is present
    ///
    if (!IsSerialIoDeviceEnabled(i)) {
      continue;
    }
    
    ///
    /// Get BAR1 Address
    ///
    Bar1 = MmioRead32(
             MmPciAddress (0, 
               PchPlatformPolicy->BusNumber, 
               SerialIoDevice[i].DevNum, 
               SerialIoDevice[i].FuncNum, 
               R_PCH_LP_SERIAL_IO_BAR1)
             ) & B_PCH_LP_SERIAL_IO_BAR1_BAR;
  
    ///
    /// Step 1
    /// Program Dx:Fx:04h[2:1] = 11b
    /// (skipped - this was performed while assigning BARs)
    ///
  
    ///
    /// Step 2
    /// Program Dx:Fx to ACPI Mode
    ///
    Data32And = 0xFFFFFFFF;
    Data32Or  = SerialIoDevice[i].DeviceModeSwitchBit;
    ProgramIobpWithScript (RootComplexBar, SerialIoDevice[i].AcpiSwitchAddr, Data32And, Data32Or);

    ///
    /// Set D3Hot Power State via BAR1 Address, for all devices except DMA
    ///
    if(i != INDEX_DMA) {
      Data32And = (UINT32) ~B_PCH_LP_SERIAL_IO_PME_CTRL_STS_PWR_ST;
      Data32Or  = B_PCH_LP_SERIAL_IO_PME_CTRL_STS_PWR_ST;
      
      PCH_INIT_COMMON_SCRIPT_MEM_READ_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint32,
        Bar1 + R_PCH_LP_SERIAL_IO_PME_CTRL_STS,
        &Data32Or,
        &Data32And
        );
    }

  }

  return Status;
}

/**
  Assigns MMIO addresses for SerialIO controllers from a predefined pool

  @retval None
**/
EFI_STATUS
AssignBARs (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy
)
{
  EFI_PHYSICAL_ADDRESS   Bar;
  EFI_PHYSICAL_ADDRESS   RegBase;
  EFI_STATUS             Status;
  UINTN                  i;
  UINT64                 TotalSize;
  UINT32                 IrqNumber;

  Bar = SERIAL_IO_ADDRESS_AREA;
  TotalSize = 2 * ((SERIAL_IO_DEVICE_QUANTITY - 1) * V_PCH_LP_SERIAL_IO_BAR_SIZE + V_PCH_LP_SERIAL_SDIO_BAR_SIZE);
  
  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeReserved,
                  Bar,
                  TotalSize,
                  0
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateAddress,
                  EfiGcdMemoryTypeReserved,
                  N_PCH_LP_SERIAL_IO_BAR_ALIGNMENT,
                  TotalSize,
                  &Bar,
                  mImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  for (i = 0; i < SERIAL_IO_DEVICE_QUANTITY; i++) {
    if (!IsSerialIoDeviceEnabled(i)) {
      continue;
    }
    RegBase = MmPciAddress (0, 0, SerialIoDevice[i].DevNum, SerialIoDevice[i].FuncNum, 0);

    MmioAndThenOr32WithScript(RegBase + R_PCH_LP_SERIAL_IO_BAR0, 0x0, SerialIoDevice[i].Bar0);
    MmioAndThenOr32WithScript(RegBase + R_PCH_LP_SERIAL_IO_BAR1, 0x0, SerialIoDevice[i].Bar1);

    MmioAndThenOr32WithScript(RegBase + R_PCH_LP_SERIAL_IO_COMMAND, 
                              0xFFFFFFFF, 
                              B_PCH_LP_SERIAL_IO_COMMAND_BME | B_PCH_LP_SERIAL_IO_COMMAND_MSE
                              );
    IrqNumber = GetSerialIoIrqNumber(SerialIoDevice[i].DevNum, SerialIoDevice[i].IrqPin, PchPlatformPolicy->SerialIoConfig->SerialIoInterruptMode);
    MmioAndThenOr32WithScript(RegBase + R_PCH_PCIE_INTR, (UINT32)~B_PCH_PCIE_INTR_ILINE, IrqNumber);
  }
  return Status;
  
}


/**
  Hides SerialIo controllers from Pci config space
  This prevents BIOS Pci enumerator from assigning mmio resources
  SerialIo controllers will receive addresses from a separate pool

  @param[in] RootComplexBar       RootComplexBar address of this PCH device

  @retval None
**/
VOID
HideSerialIoDevices (
  IN UINT32  RootComplexBar
  )
{
  UINTN  i;
  UINT32 Data32And;
  UINT32 Data32Or;

  for (i = 0; i < SERIAL_IO_DEVICE_QUANTITY; i++) {
    Data32And = (UINT32) 0xFFFFFFFF;
    Data32Or  = (UINT32)  SerialIoDevice[i].DeviceModeSwitchBit;

    ProgramIobpWithScript (RootComplexBar, SerialIoDevice[i].AcpiSwitchAddr, Data32And, Data32Or);
  }
}

/**
  Reveals SerialIo controllers' Pci config space
  This allows BIOS to complete initialization for those devices

  @param[in] RootComplexBar       RootComplexBar address of this PCH device

  @retval None
**/
VOID
RevealConfigSpace (
  IN UINT32  RootComplexBar
  )
{
  UINTN  i;
  UINT32 Data32And;
  UINT32 Data32Or;

  for (i = 0; i < SERIAL_IO_DEVICE_QUANTITY; i++) {
    Data32And = (UINT32)~(SerialIoDevice[i].DeviceModeSwitchBit);
    Data32Or  = (UINT32) 0x0;
    ProgramIobpWithScript (RootComplexBar, SerialIoDevice[i].AcpiSwitchAddr, Data32And, Data32Or);
  }
}


/**
  Configures Serial IO Controllers

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar address of this PCH device

  @retval None
**/
EFI_STATUS
ConfigureSerialIoDevices (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy,
  IN UINT32                           RootComplexBar
  )
{
  UINT8                            Index;
  UINTN                            RegBase;
  UINTN                            Bar0;
  UINT32                           Data32And;
  UINT32                           Data32Or;
  UINT8                            i;

  for (i = 0; i < INDEX_SDIO; i++) {
  ///
  /// Check if device is present
  ///
    if (!IsSerialIoDeviceEnabled(i)) {
      continue;
    }
    RegBase = MmPciAddress (0, 0, SerialIoDevice[i].DevNum, SerialIoDevice[i].FuncNum, 0);
    
    Bar0 = MmioRead32 (RegBase + R_PCH_LP_SERIAL_IO_BAR0);
  
    if(i != PCI_FUNCTION_NUMBER_PCH_LP_SERIAL_IO_DMA) {
      ///
      /// PCH BIOS Spec Rev 0.7.0 Section 23.3 Serial IO LTR Programming
      /// Step 1: Program BAR0 + 808h[2] = 0b
      /// Step 2: Program BAR0 + 804h[1:0] = 00b
      /// Step 3: Program BAR0 + 804h[1:0] = 11b
      ///
      MmioAndThenOr32WithScript (Bar0 + R_PCH_LP_SERIAL_IO_PPR_GEN, (UINT32)(~B_PCH_LP_SERIAL_IO_PPR_GEN_LTR_MODE), 0x0);
      MmioAndThenOr32WithScript (Bar0 + R_PCH_LP_SERIAL_IO_PPR_RST, (UINT32)(~(B_PCH_LP_SERIAL_IO_PPR_RST_FUNC | B_PCH_LP_SERIAL_IO_PPR_RST_APB)), 0);
      MmioAndThenOr32WithScript (Bar0 + R_PCH_LP_SERIAL_IO_PPR_RST, 0xFFFFFFFF, (UINT32)(B_PCH_LP_SERIAL_IO_PPR_RST_FUNC | B_PCH_LP_SERIAL_IO_PPR_RST_APB));
  
      ///
      /// Step 4
      /// Program BAR0 + 814h with LTR value for each SerialIo controller
      ///
      MmioAndThenOr32WithScript(Bar0 + R_PCH_LP_SERIAL_IO_PPR_AUTO_LTR, 0, 0);
    }
  
    ///
    /// PCH BIOS Spec Rev 0.7.0 Section 23.7 Serial IO Power Management Programming
    /// Step 4
    /// Program IO Voltage Select for I2C0 & I2C1 as per platform policy
    ///
    if(i == PCI_FUNCTION_NUMBER_PCH_LP_SERIAL_IO_I2C0) {
      if (PchPlatformPolicy->SerialIoConfig->I2c0VoltageSelect == PchSerialIoIs33V) {
        MmioAndThenOr32WithScript(Bar0 + R_PCH_LP_SERIAL_IO_PPR_GEN, (UINT32)~(B_PCH_LP_SERIAL_IO_PPR_GEN_IO_VOLTAGE_SEL), 0);
      } else {
        MmioAndThenOr32WithScript(Bar0 + R_PCH_LP_SERIAL_IO_PPR_GEN, 0xFFFFFFFF, B_PCH_LP_SERIAL_IO_PPR_GEN_IO_VOLTAGE_SEL);
      }
    }

    if(i== PCI_FUNCTION_NUMBER_PCH_LP_SERIAL_IO_I2C1) {
      if (PchPlatformPolicy->SerialIoConfig->I2c1VoltageSelect == PchSerialIoIs33V) {
        MmioAndThenOr32WithScript(Bar0 + R_PCH_LP_SERIAL_IO_PPR_GEN, (UINT32)~(B_PCH_LP_SERIAL_IO_PPR_GEN_IO_VOLTAGE_SEL), 0);
      } else {
        MmioAndThenOr32WithScript(Bar0 + R_PCH_LP_SERIAL_IO_PPR_GEN, 0xFFFFFFFF, B_PCH_LP_SERIAL_IO_PPR_GEN_IO_VOLTAGE_SEL);
      }
    }

    ///
    /// PCH BIOS Spec Rev 0.7.0 Section 23.4 Serial IO Interrupt Programming
    /// Step 4
    /// Program D21:Fx to PCI Interrupt Mode
    ///
    Data32And = (UINT32)~(SerialIoDevice[i].IrqModeSwitchBit);
    if(PchPlatformPolicy->SerialIoConfig->SerialIoInterruptMode == PchSerialIoIsAcpi) {
     Data32Or  = SerialIoDevice[i].IrqModeSwitchBit;
    } else {
     Data32Or  = 0x00;
    }

    ///
    /// PCH BIOS Spec Rev 0.7.0 Section 23.7 Serial IO Power Management Programming
    /// Step 5
    /// Program D21:Fx FAB_PM_CAP_PRSNT_PORT0
    ///
    Data32Or |= BIT1;
    ProgramIobpWithScript (RootComplexBar, SerialIoDevice[i].AcpiSwitchAddr, Data32And, Data32Or);
  }

  ///
  /// Check if SDIO device is present
  ///
  if (IsSerialIoDeviceEnabled(INDEX_SDIO)) {

    RegBase = MmPciAddress (0, 0, SerialIoDevice[INDEX_SDIO].DevNum, SerialIoDevice[INDEX_SDIO].FuncNum, 0);

    Bar0 = MmioRead32 (RegBase + R_PCH_LP_SERIAL_IO_BAR0);

    MmioAndThenOr32WithScript (Bar0 + R_PCH_LP_SERIAL_IO_SDIO_SLAVE_DELAY_DDR50_MODE,   0, 0x00000306);


    ///
    /// PCH BIOS Spec Rev 0.3.0 Section 23.3 Serial IO LTR Programming
    /// Step 5
    /// Program BAR0 + 1008h[2] = 1b
    ///
    MmioAndThenOr32WithScript (Bar0 + R_PCH_LP_SERIAL_IO_SDIO_PPR_GEN, 0xFFFFFFFF, B_PCH_LP_SERIAL_IO_SDIO_PPR_GEN_LTR_MODE);
     
    ///
    /// Step 6
    /// Program BAR0 + 1010h = 0x00000000 for SDIO controller
    ///
    Data32And = 0x00000000;
    MmioAndThenOr32WithScript (Bar0 + R_PCH_LP_SERIAL_IO_SDIO_PPR_SW_LTR, 0, 0);
  
    ///
    /// Program D23:F0 to PCI Interrupt Mode
    ///
    Data32And = (UINT32)~(SerialIoDevice[INDEX_SDIO].IrqModeSwitchBit);
    if(PchPlatformPolicy->SerialIoConfig->SerialIoInterruptMode == PchSerialIoIsAcpi) {
       Data32Or  = SerialIoDevice[INDEX_SDIO].IrqModeSwitchBit;
    } else {
       Data32Or  = 0x00;
    }
    ProgramIobpWithScript (RootComplexBar, SerialIoDevice[INDEX_SDIO].AcpiSwitchAddr, Data32And, Data32Or);
  }

  ///
  /// PCH BIOS Spec Rev 0.7.0 Section 23.4 Serial IO Interrupt Programing
  ///
  if(PchPlatformPolicy->SerialIoConfig->SerialIoInterruptMode == PchSerialIoIsAcpi) {
    ///
    ///  Enable ACPI IRQ for IRQ13, IRQ7, IRQ6, IRQ5 in RCRB
    ///
    Data32Or = (B_PCH_RCRB_INT_ACPIIRQEN_A13E | B_PCH_RCRB_INT_ACPIIRQEN_A7E | B_PCH_RCRB_INT_ACPIIRQEN_A6E | B_PCH_RCRB_INT_ACPIIRQEN_A5E);
    MmioAndThenOr32WithScript((UINTN)(RootComplexBar + R_PCH_RCRB_INT_ACPIIRQEN), 0xFFFFFFFF, Data32Or);
  }

#ifdef ULT_FLAG
  if (GetPchSeries() == PchLp) {  
    for (Index = 0; Index < (sizeof (PchSerialIoIntsLptLp) / sizeof (IOBP_MMIO_TABLE_STRUCT)); Index++) {
      ///
      /// Program IOBP register
      ///
      Data32And = PchSerialIoIntsLptLp[Index].AndMask;
      Data32Or  = PchSerialIoIntsLptLp[Index].OrMask;
      ProgramIobpWithScript (RootComplexBar, PchSerialIoIntsLptLp[Index].Address, Data32And, Data32Or);
    }
 }
#endif //ULT_FLAG

  ///
  /// PCH BIOS Spec Rev 0.3.0 Section 31.22.6 Serial IO Power Management Programming
  /// Step 1
  /// Program CB000154h[12,9:8,4:0] = 1001100011111b
  ///
  Data32And = (UINT32)~(BIT12 | BIT9 | BIT8 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  Data32Or  = (UINT32) (BIT12 | BIT9 | BIT8 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);

  if (PchPlatformPolicy->Revision >= DXE_PCH_PLATFORM_POLICY_PROTOCOL_REVISION_6) {
    if (PchPlatformPolicy->SerialIoConfig->Ddr50Support) {
      Data32Or |= BIT6;
    }
  }

  ProgramIobpWithScript (RootComplexBar, 0xCB000154, Data32And, Data32Or);

  ///
  /// Step 2
  /// Programming done above
  ///
  /// Step 3
  /// Program CB000180h[5:0] = 111111b
  ///
  Data32And = (UINT32)~(BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  Data32Or  = (UINT32) (BIT5 | BIT4 | BIT3 | BIT2 | BIT1 | BIT0);
  ProgramIobpWithScript (RootComplexBar, 0xCB000180, Data32And, Data32Or);

#ifdef ULT_FLAG  
  if (GetPchSeries() == PchLp) {
    ///
    /// PCH BIOS Spec Rev 0.3.0 Section 23.8 Serial IO Snoop Programing
    ///
    for (Index = 0; Index < (sizeof (PchSerialIoSnoopLptLp) / sizeof (IOBP_MMIO_TABLE_STRUCT)); Index++) {
      ///
      /// Program IOBP register
      ///
      ProgramIobpWithScript (
                RootComplexBar,
                PchSerialIoSnoopLptLp[Index].Address,
                PchSerialIoSnoopLptLp[Index].AndMask,
                PchSerialIoSnoopLptLp[Index].OrMask
                );
    }
  }
#endif // ULT_FLAG

  return EFI_SUCCESS;
}

VOID
ConfigureSerialIoAtBoot (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy
  )
{
  UINTN i;
  UINT8 FunctionEnable[SERIAL_IO_DEVICE_QUANTITY];

  if (PchPlatformPolicy->SerialIoConfig->SerialIoMode != PchSerialIoIsAcpi) {
    return;
  }

  FunctionEnable[INDEX_I2C0]  = PchPlatformPolicy->DeviceEnabling->SerialIoI2c0;
  FunctionEnable[INDEX_I2C1]  = PchPlatformPolicy->DeviceEnabling->SerialIoI2c1;
  FunctionEnable[INDEX_SPI0]  = PchPlatformPolicy->DeviceEnabling->SerialIoSpi0;
  FunctionEnable[INDEX_SPI1]  = PchPlatformPolicy->DeviceEnabling->SerialIoSpi1;
  FunctionEnable[INDEX_UART0] = PchPlatformPolicy->DeviceEnabling->SerialIoUart0;
  FunctionEnable[INDEX_UART1] = PchPlatformPolicy->DeviceEnabling->SerialIoUart1;
  FunctionEnable[INDEX_SDIO]  = PchPlatformPolicy->DeviceEnabling->SerialIoSdio;

  for (i=INDEX_I2C0; i<MAX_SIO_INDEX; i++) {
    if (!FunctionEnable[i]) {
      continue;
    }

    ///
    /// Set D3Hot Power State via BAR1 Address, for all devices except DMA
    ///
    if(i != INDEX_DMA) {
      MmioAndThenOr32 (SerialIoDevice[i].Bar1 + R_PCH_LP_SERIAL_IO_PME_CTRL_STS,
                      (UINT32) ~B_PCH_LP_SERIAL_IO_PME_CTRL_STS_PWR_ST,
                      B_PCH_LP_SERIAL_IO_PME_CTRL_STS_PWR_ST
                      );
      MmioRead32(SerialIoDevice[i].Bar1 + R_PCH_LP_SERIAL_IO_PME_CTRL_STS);
    }
  }
}


/**
  Configures Serial IO Controllers after Pci Enum

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar address of this PCH device

  @retval None
**/
EFI_STATUS
ConfigureSerialIoBeforeBoot (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy,
  IN UINT32                           RootComplexBar
  )
{

  RevealConfigSpace(RootComplexBar);   
  DisableSerialIoControllers(PchPlatformPolicy, RootComplexBar);
  AssignBARs(PchPlatformPolicy);
  ConfigureSerialIoDevices(PchPlatformPolicy, RootComplexBar);
  UpdateSerialIoAcpiData(PchPlatformPolicy, RootComplexBar);
  
  if (PchPlatformPolicy->SerialIoConfig->SerialIoMode == PchSerialIoIsAcpi) {
    PutSerialIoInAcpiMode(PchPlatformPolicy, RootComplexBar);
  }
  return EFI_SUCCESS;
}


EFI_STATUS
ConfigureSerialIo (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy,
  IN UINT32                           RootComplexBar
  )
{
  HideSerialIoDevices(RootComplexBar);
  return EFI_SUCCESS;
}

EFI_STATUS
InstallDmaAcpiTable (
  IN DXE_PCH_PLATFORM_POLICY_PROTOCOL *PchPlatformPolicy
)
{
  UINTN                                     AcpiTableKey;
  EFI_ACPI_TABLE_PROTOCOL                   *AcpiTable;
  EFI_ACPI_5_0_CORE_SYSTEM_RESOURCES_TABLE  *CoreSystemResourcesTable;
  UINT64                                    Data64;
  UINT32                                    Data32;
  UINTN                                     RegBase;
  UINT8                                     Index;
  EFI_STATUS                                Status;
  UINT32                                    IrqNumber;

  AcpiTable = NULL;
  CoreSystemResourcesTable = NULL;
  AcpiTableKey = 0;

  //
  // Locate ACPI support protocol
  //
  Status = gBS->LocateProtocol (&gEfiAcpiTableProtocolGuid, NULL, (VOID **)&AcpiTable);
  if( EFI_ERROR(Status) || AcpiTable == NULL) {
    return EFI_UNSUPPORTED;
  }
  //
  // Allocate Memory for Core System Resources Table
  //
  CoreSystemResourcesTable = AllocateZeroPool(sizeof(EFI_ACPI_5_0_CORE_SYSTEM_RESOURCES_TABLE));
  
  if(CoreSystemResourcesTable == NULL) {
    return EFI_UNSUPPORTED;
  }
  
  CoreSystemResourcesTable->Header.Signature = EFI_SIGNATURE_32('C','S','R','T');
  CoreSystemResourcesTable->Header.Length = sizeof(EFI_ACPI_5_0_CORE_SYSTEM_RESOURCES_TABLE);
  CoreSystemResourcesTable->Header.Revision = 0x01;
  CoreSystemResourcesTable->Header.Checksum = 0x00;
  Data64 = EFI_SIGNATURE_64 ('I', 'N', 'T', 'L', 0, 0, 0, 0);
  CopyMem (&CoreSystemResourcesTable->Header.OemId, &Data64, sizeof(CoreSystemResourcesTable->Header.OemId));
  Data64 = EFI_SIGNATURE_64 ('H', 'S', 'W', 'U', 'L', 'T', '-', 'R');
  CopyMem (&CoreSystemResourcesTable->Header.OemTableId, &Data64, sizeof(CoreSystemResourcesTable->Header.OemTableId));
  CoreSystemResourcesTable->Header.OemRevision = 0x00000001;
  CoreSystemResourcesTable->Header.CreatorId = EFI_SIGNATURE_32('I','N','T','L');
  CoreSystemResourcesTable->Header.CreatorRevision = 0x20100528;
  
  CoreSystemResourcesTable->ResourceGroup1.ResourceGroupLength = sizeof(EFI_ACPI_5_0_CORE_SYSTEM_RESOURCES_TABLE) - sizeof(EFI_ACPI_DESCRIPTION_HEADER);
  CoreSystemResourcesTable->ResourceGroup1.VendorId = EFI_SIGNATURE_32('I','N','T','L');
  CoreSystemResourcesTable->ResourceGroup1.SubVendorId = 0x00000000;
  CoreSystemResourcesTable->ResourceGroup1.DeviceId = 0x9C60;
  CoreSystemResourcesTable->ResourceGroup1.SubDeviceId = 0x0000;
  CoreSystemResourcesTable->ResourceGroup1.Revision = 0x0001;
  CoreSystemResourcesTable->ResourceGroup1.Reserved = 0x0000;
  CoreSystemResourcesTable->ResourceGroup1.SharedInfoLength = 0x00000001C;
  CoreSystemResourcesTable->ResourceGroup1SharedInfo.MajorVersion1 = 0x001;
  CoreSystemResourcesTable->ResourceGroup1SharedInfo.MinorVersion0 = 0x000;
  
  RegBase = MmPciAddress (0, 0, SerialIoDevice[INDEX_DMA].DevNum, SerialIoDevice[INDEX_DMA].FuncNum, 0);
  Data32 = (MmioRead32(RegBase + R_PCH_LP_SERIAL_IO_BAR0) & B_PCH_LP_SERIAL_IO_BAR0_BAR);
  CoreSystemResourcesTable->ResourceGroup1SharedInfo.MmioBaseL = Data32;
  CoreSystemResourcesTable->ResourceGroup1SharedInfo.MmioBaseH = 0x00000000;
  //
  // Match DMA interrupt value
  //
  IrqNumber = GetSerialIoIrqNumber(SerialIoDevice[INDEX_DMA].DevNum, SerialIoDevice[INDEX_DMA].IrqPin, PchPlatformPolicy->SerialIoConfig->SerialIoInterruptMode);
  CoreSystemResourcesTable->ResourceGroup1SharedInfo.InterruptGSI = IrqNumber;

  CoreSystemResourcesTable->ResourceGroup1SharedInfo.InterruptPolarity = 0x02;
  CoreSystemResourcesTable->ResourceGroup1SharedInfo.InterruptMode = 0x00;
  CoreSystemResourcesTable->ResourceGroup1SharedInfo.NumberofChannels = 0x08;
  CoreSystemResourcesTable->ResourceGroup1SharedInfo.DMAAddressWidth = 0x20;
  
  CoreSystemResourcesTable->ResourceGroup1SharedInfo.BaseRequestLine = 0x0010;
  CoreSystemResourcesTable->ResourceGroup1SharedInfo.NumberofHandshakeSignals = 0x0010;
  CoreSystemResourcesTable->ResourceGroup1SharedInfo.MaximumBlockTransferSize = 0x00000FFF;
  
  CoreSystemResourcesTable->ControllerResourceDescriptor1.ResourceDescriptorLength = 0x0000000C;
  CoreSystemResourcesTable->ControllerResourceDescriptor1.ResourceType = 0x0003;
  CoreSystemResourcesTable->ControllerResourceDescriptor1.ResourceSubType = 0x0001;
  CoreSystemResourcesTable->ControllerResourceDescriptor1.UID = 0x20495053;
  
  Data32 = 0x30414843;
  for(Index = 0; Index < 8; Index++) {
  CoreSystemResourcesTable->ChannelResourceDescriptor1[Index].ResourceDescriptorLength = 0x0000000C;
  CoreSystemResourcesTable->ChannelResourceDescriptor1[Index].ResourceType = 0x0003;
  CoreSystemResourcesTable->ChannelResourceDescriptor1[Index].ResourceSubType = 0x0000;
  CoreSystemResourcesTable->ChannelResourceDescriptor1[Index].UID = Data32;
    Data32 += 0x1000000;
  }
  Status = AcpiTable->InstallAcpiTable (AcpiTable, CoreSystemResourcesTable, CoreSystemResourcesTable->Header.Length, &AcpiTableKey);
  return Status;
}
  
  
  
#endif // SERIAL_IO_FLAG



