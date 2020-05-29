/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>

#include <Library/DebugLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiOemSvcKernelLib.h>
#include <Library/PciCf8Lib.h>
#include <Library/PcdLib.h>
// 
// Driver Consumed PPI Prototypes
// 
#include <Ppi/Smbus2.h>

#include <IndustryStandard/SmBus.h>
#include <IndustryStandard/Pci.h>


typedef struct {
  UINT16                           NBVId;
  UINT16                           PlatformId;
  CLOCK_GEN_VGA_TYPE               VgaType;
  UINT8                            *ClkGenData;
  UINT8                            BlockSize;
  UINT8                            ClkGenVId;
  UINT8                            ClkGenDId;
} REAL_CLKGEN_INFO;

EFI_STATUS
ClockGenWriteSmBus (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_SMBUS2_PPI            *SmbusPpi,
  IN EFI_SMBUS_DEVICE_ADDRESS      SmbusSlaveAddress,
  IN EFI_SMBUS_DEVICE_COMMAND      SmbusDeviceCommand,
  IN CLOCK_GEN_DATA_INFO           *InitTable,
  IN UINT8                         *DataBuffer,
  IN UINTN                         DataLength
  );

EFI_STATUS
ShiftBit (
  IN  UINT8                        ByteValue,
  IN  UINT8                        MaskValue,
  OUT UINT8                        *RealByteValue
  );

/**
  User entry for this PEIM driver.
  
  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCESS Program clock generator successfully.
  @return Others      Fail to program clock generator

**/
EFI_STATUS
EFIAPI
PeimInitializeClkGen (
  IN EFI_PEI_FILE_HANDLE     FileHandle,
  IN CONST EFI_PEI_SERVICES  **PeiServices
  )
{
  EFI_STATUS                    Status;
  UINT8                         SlaveAddress;
  UINT8                         StartOffset;
  CLOCK_GEN_UNION_INFO          *ClkGenBigTable;
  REAL_CLKGEN_INFO              RealClkGenInfo;
  UINT16                        BigTableSize;
  EFI_PEI_SMBUS2_PPI            *SmbusPpi;
  UINT8                         TableIndex;
  UINTN                         DataLength;
  UINT8                         *DataBuffer;
  EFI_SMBUS_DEVICE_ADDRESS      SmbusSlaveAddress;
  EFI_SMBUS_DEVICE_COMMAND      SmbusDeviceCommand;
  EFI_SMBUS_OPERATION           SmbusOperation;
  UINTN                         OperationLength;
  UINTN                         ClockGenIndex;
  UINTN                         IndexEnd;
  UINT16                        VenderID;
  UINT32                        ReadData;
  UINT8                         ClassCode;

    
  ClkGenBigTable = NULL;
  DataLength = 0;
  DataBuffer = NULL;
  ClassCode  = 0;
 
  BigTableSize = 0;
  
  //
  // (OemServices) Get CLKGEN Modify data information
  // 
  Status = OemSvcChipsetModifyClockGenInfo (
             &SlaveAddress,
             &StartOffset,
             &ClkGenBigTable,
             &RealClkGenInfo.PlatformId,
             &BigTableSize
             );         

  if (!EFI_ERROR (Status)) {
    //
    // Message will show on command line and Procedure will not dead lock.
    //
    DEBUG ((EFI_D_ERROR, "Get ClkGenBigTable error\n"));
    return EFI_UNSUPPORTED;
  }
  
  Status = PeiServicesLocatePpi (
             &gEfiPeiSmbus2PpiGuid,             // GUID
             0,                                 // INSTANCE
             NULL,                              // PEI_PPI_DESCRIPTOR
             (VOID **)&SmbusPpi                          // PPI
             );
  if (EFI_ERROR(Status)) {
    //
    // Message will show on command line and Procedure will not dead lock.
    //
    DEBUG ((EFI_D_ERROR, "Locate SMBus error\n"));
    return Status;
  }
  
  if (ClkGenBigTable == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  if (ClkGenBigTable[0].UnionDataInfo->DataInfo[0].WriteType == CLKGEN_FAST_BLOCK_OPERATION) {
    
    TableIndex = 0;
    
    while (ClkGenBigTable[0].UnionDataInfo->DataInfo[TableIndex].WriteType != CLKGEN_DATA_END) {
      TableIndex++;
    }
    
    DataLength = TableIndex;
    
    Status = PeiServicesAllocatePool (
                   DataLength,
                   (VOID **)&DataBuffer
                   );
                   
    if (EFI_ERROR (Status)) {
      //
      // Message will show on command line and Procedure will not dead lock.
      //
      DEBUG ((EFI_D_ERROR, "Allocate DataBuffer error\n"));
      return Status;
    }
    
    SetMem(DataBuffer, DataLength, 0);
    
    TableIndex = 0;
    
    while (ClkGenBigTable[0].UnionDataInfo->DataInfo[TableIndex].WriteType != CLKGEN_DATA_END) {
      DataBuffer [ClkGenBigTable[0].UnionDataInfo->DataInfo[TableIndex].Byte] = ClkGenBigTable[0].UnionDataInfo->DataInfo[TableIndex].OrSourceValue;
      TableIndex++;
    }
    
    SmbusSlaveAddress.SmbusDeviceAddress = SlaveAddress;
    SmbusDeviceCommand = 0;
    SmbusOperation = EfiSmbusWriteBlock;
    OperationLength = TableIndex;
    
    Status = SmbusPpi->Execute (
                         SmbusPpi,
                         SmbusSlaveAddress,
                         SmbusDeviceCommand,
                         SmbusOperation,
                         FALSE,
                         &OperationLength,
                         &DataBuffer [0]
                         );
    
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Write CLKGEN data error\n"));
      return Status;
    } else {
      return EFI_SUCCESS;
    }                   
  }
 
  //
  // Get NB Vendor ID
  //  
  RealClkGenInfo.NBVId = PciCf8Read16 (PCI_CF8_LIB_ADDRESS (0, 0, 0, PCI_VENDOR_ID_OFFSET));
  
  //
  // Get Graphic Type
  //

  //
  // Check UMA Graphic
  //  
  if (PcdGet8 (PcdUmaDeviceNumber) == 0xFF ) {
    VenderID = 0xFFFF;
  } else {
    VenderID = PciCf8Read16 (PCI_CF8_LIB_ADDRESS (0, PcdGet8 (PcdUmaDeviceNumber), 0, PCI_VENDOR_ID_OFFSET));
  }              

  if (VenderID == 0xFFFF) {
    //
    // check DESCRIPTER Graphic
    //
    
    //
    // Set PEG PortBus = 1 to Read Endpoint.
    //
    if ( PcdGet8 (PcdPegDeviceNumber) == 0xFF ) {
      VenderID = 0xFFFF;
    } else {
      ReadData = PciCf8Read32 (PCI_CF8_LIB_ADDRESS (0, PcdGet8 (PcdPegDeviceNumber), 0, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET));
      
      ReadData = ((ReadData & 0xFF0000FF) | 0x00010100);

      PciCf8Write32 (
              PCI_CF8_LIB_ADDRESS (0, PcdGet8 (PcdPegDeviceNumber), 0, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET),
              ReadData
              );

      //
      // Get VID.
      //
      VenderID = PciCf8Read16 (PCI_CF8_LIB_ADDRESS (1, 0, 0, PCI_VENDOR_ID_OFFSET));

      //
      // Get device class code.
      //
      ClassCode = PciCf8Read8 (PCI_CF8_LIB_ADDRESS (1, 0, 0, (PCI_CLASSCODE_OFFSET + 2)));

      ReadData = (ReadData & 0xFF0000FF);
      PciCf8Write32 (
              PCI_CF8_LIB_ADDRESS (0, PcdGet8 (PcdPegDeviceNumber), 0, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET),
              ReadData
              );
    }
    if (VenderID != 0xFFFF) {
      //
      // Perform PEG Endpoint Class Code Check.  If the Endpoint Class Code is
      // not GFX, then the Port is being used as a standard PCI Express Port.
      //
      if (ClassCode == PCI_CLASS_DISPLAY || ClassCode == PCI_CLASS_BRIDGE) {
        RealClkGenInfo.VgaType = CLOCK_GEN_VGA_DESCRIPTER;
      } else {
        return EFI_UNSUPPORTED; 
      }
    } else {    
      return EFI_UNSUPPORTED;        
    }

  } else {
    //
    // To check if this is an UMA and PEG combined SKU.
    // the UMA device was detected, check if there is other PEG device.
    //
    
    //
    // Set PEG PortBus = 1 to Read Endpoint.
    //
    if ( PcdGet8 (PcdPegDeviceNumber) == 0xFF ) {
      VenderID = 0xFFFF;
    } else {
      ReadData = PciCf8Read32 (PCI_CF8_LIB_ADDRESS (0, PcdGet8 (PcdPegDeviceNumber), 0, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET));

      ReadData = ((ReadData & 0xFF0000FF) | 0x00010100);
      
      PciCf8Write32 (
              PCI_CF8_LIB_ADDRESS (0, PcdGet8 (PcdPegDeviceNumber), 0, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET),
              ReadData
              );

      //
      // Get VID.
      //
      VenderID = PciCf8Read16 (PCI_CF8_LIB_ADDRESS (1, 0, 0, PCI_VENDOR_ID_OFFSET));

      //
      // Get device class code.
      //
      ClassCode = PciCf8Read8 (PCI_CF8_LIB_ADDRESS (1, 0, 0, (PCI_CLASSCODE_OFFSET + 2)));

      ReadData = (ReadData & 0xFF0000FF);
      PciCf8Write32 (
              PCI_CF8_LIB_ADDRESS (0, PcdGet8 (PcdPegDeviceNumber), 0, PCI_BRIDGE_PRIMARY_BUS_REGISTER_OFFSET),
              ReadData
              );
    }

    if (VenderID != 0xFFFF) {
      //
      // Perform PEG Endpoint Class Code Check.  If the Endpoint Class Code is
      // not GFX, then the Port is being used as a standard PCI Express Port.
      //
      if (ClassCode == PCI_CLASS_DISPLAY || ClassCode == PCI_CLASS_BRIDGE) {
        RealClkGenInfo.VgaType = CLOCK_GEN_VGA_UNITED;
      } else {
        RealClkGenInfo.VgaType = CLOCK_GEN_VGA_UMA;
      }
    } else {    
      RealClkGenInfo.VgaType = CLOCK_GEN_VGA_UMA;
    }
  }
 
  //
  // Get CLKGEN Data and Length
  //
 
  //
  // Get CLKGEN Length
  //
  DataLength = 1;
  Status = PeiServicesAllocatePool (
                   DataLength,
                   (VOID **)&DataBuffer
                   );
                   
  if (EFI_ERROR (Status)) {
    //
    // Message will show on command line and Procedure will not dead lock.
    //
    DEBUG ((EFI_D_ERROR, "Allocate DataBuffer error\n"));
    return Status;
  }
  
  SetMem (DataBuffer, DataLength, 0);
 
  SmbusSlaveAddress.SmbusDeviceAddress = SlaveAddress;
  SmbusDeviceCommand = (UINTN)StartOffset;
 
  SmbusOperation = EfiSmbusReadBlock;
  OperationLength = DataLength;
  
  Status = SmbusPpi->Execute (
                       SmbusPpi,
                       SmbusSlaveAddress,
                       SmbusDeviceCommand,
                       SmbusOperation,
                       FALSE,
                       &OperationLength,
                       &DataBuffer[0]
                       );
                              
  RealClkGenInfo.BlockSize = (UINT8)OperationLength;
  
  //
  // Get CLKGEN Data
  //  
  DataLength = RealClkGenInfo.BlockSize;
  
  Status = PeiServicesAllocatePool (
                   DataLength,
                   (VOID **)&DataBuffer
                   );
                   
  if (EFI_ERROR (Status)) {
    //
    // Message will show on command line and Procedure will not dead lock.
    //
    DEBUG ((EFI_D_ERROR, "Allocate DataBuffer error\n"));
    return Status;
  }
  
  SetMem (DataBuffer, DataLength, 0);
 
  SmbusSlaveAddress.SmbusDeviceAddress = SlaveAddress;
  SmbusDeviceCommand = (UINTN)StartOffset;
 
  SmbusOperation = EfiSmbusReadBlock;
  OperationLength = DataLength;
  
  Status = SmbusPpi->Execute (
                       SmbusPpi,
                       SmbusSlaveAddress,
                       SmbusDeviceCommand,
                       SmbusOperation,
                       FALSE,
                       &OperationLength,
                       &DataBuffer[0]
                       );
                       
  if (EFI_ERROR (Status)) {  
    //
    // Message will show on command line and Procedure will not dead lock.
    //
    DEBUG ((EFI_D_ERROR, "Read CLKGEN data error\n"));
    return Status;
  } else if ((DataBuffer[0] == 0xFF) && 
             (DataBuffer[1] == 0xFF) && 
             (DataBuffer[2] == 0xFF) && 
             (DataBuffer[3] == 0xFF)) {
    DEBUG ((EFI_D_ERROR, "Read CLKGEN data error\n"));        
    return EFI_UNSUPPORTED;   
  }
  
  RealClkGenInfo.ClkGenData = DataBuffer;
 
  IndexEnd = BigTableSize;
  
  //
  // Find PRI_INIT table and pre init CLKGEN
  //
  for (ClockGenIndex = 0; ClockGenIndex < IndexEnd ; ClockGenIndex++) {
    
    ShiftBit(RealClkGenInfo.ClkGenData[ClkGenBigTable[ClockGenIndex].DeviceInfo->VenderIdReg.Byte],
             ClkGenBigTable[ClockGenIndex].DeviceInfo->VenderIdReg.Mask, 
             &RealClkGenInfo.ClkGenVId);
    
    ShiftBit(RealClkGenInfo.ClkGenData[ClkGenBigTable[ClockGenIndex].DeviceInfo->DeviceIdReg.Byte],
             ClkGenBigTable[ClockGenIndex].DeviceInfo->DeviceIdReg.Mask, 
             &RealClkGenInfo.ClkGenDId);
    
    if (ClkGenBigTable[ClockGenIndex].UnionDataInfo->ExtraInfo->ChipVId == NO_VERFY_CHIP_VID) {
      RealClkGenInfo.NBVId = NO_VERFY_CHIP_VID;
    }  
      
    if (ClkGenBigTable[ClockGenIndex].UnionDataInfo->ExtraInfo->VgaType == NO_VERFY_VGA) {
      RealClkGenInfo.VgaType = NO_VERFY_VGA;
    }
          
    if (ClkGenBigTable[ClockGenIndex].DeviceInfo->VenderId == NO_VERFY_CLKGEN_VID_DID) {
      RealClkGenInfo.ClkGenVId = NO_VERFY_CLKGEN_VID_DID;
    }
    
    if (ClkGenBigTable[ClockGenIndex].DeviceInfo->DeviceId == NO_VERFY_CLKGEN_VID_DID) {
      RealClkGenInfo.ClkGenDId = NO_VERFY_CLKGEN_VID_DID;
    } 
    
    //
    // Compare NB Vendor ID, Compare Platform ID, Compare Graphic Type, Compare Init Type, Compare CLKGEN Vendor ID and CLKGEN Device ID
    //
    if ((ClkGenBigTable[ClockGenIndex].UnionDataInfo->ExtraInfo->InitType == CLOCK_GEN_PRI_INIT) && 
        (ClkGenBigTable[ClockGenIndex].UnionDataInfo->ExtraInfo->ChipVId == RealClkGenInfo.NBVId) && 
        (ClkGenBigTable[ClockGenIndex].UnionDataInfo->ExtraInfo->PlatformId == RealClkGenInfo.PlatformId) && 
        (ClkGenBigTable[ClockGenIndex].UnionDataInfo->ExtraInfo->VgaType == RealClkGenInfo.VgaType) && 
        (ClkGenBigTable[ClockGenIndex].DeviceInfo->VenderId == RealClkGenInfo.ClkGenVId) && 
        (ClkGenBigTable[ClockGenIndex].DeviceInfo->DeviceId == RealClkGenInfo.ClkGenDId)){    
      //
      // Write Smbus
      //                  
      SmbusSlaveAddress.SmbusDeviceAddress = SlaveAddress;
      SmbusDeviceCommand = (UINTN)StartOffset;
  
      Status = ClockGenWriteSmBus ((EFI_PEI_SERVICES**) PeiServices,
                                   SmbusPpi,
                                   SmbusSlaveAddress,
                                   SmbusDeviceCommand,
                                   ClkGenBigTable[ClockGenIndex].UnionDataInfo->DataInfo,
                                   RealClkGenInfo.ClkGenData,
                                   RealClkGenInfo.BlockSize
                                   );
                                   
      if (EFI_ERROR (Status)) {
        //
        // Message will show on command line and Procedure will not dead lock.
        //
        DEBUG ((EFI_D_ERROR, "Write CLKGEN data error\n"));
        return Status;
      }
      //
      // Get CLKGEN Length again for pre init change CLKGEN Length.
      //
      SmbusSlaveAddress.SmbusDeviceAddress = SlaveAddress;
      SmbusDeviceCommand = (UINTN)StartOffset;
    
      SmbusOperation = EfiSmbusReadBlock;
      OperationLength = RealClkGenInfo.BlockSize;
      
      Status = SmbusPpi->Execute (
                           SmbusPpi,
                           SmbusSlaveAddress,
                           SmbusDeviceCommand,
                           SmbusOperation,
                           FALSE,
                           &OperationLength,
                           &DataBuffer[0]
                           );
                                  
      if ((UINT8)OperationLength != RealClkGenInfo.BlockSize) {
       
        RealClkGenInfo.BlockSize = (UINT8)OperationLength;
        
        SetMem (DataBuffer, DataLength, 0);
    
        SmbusSlaveAddress.SmbusDeviceAddress = SlaveAddress;
        SmbusDeviceCommand = (UINTN)StartOffset;
      
        SmbusOperation = EfiSmbusReadBlock;
        
        Status = SmbusPpi->Execute (
                             SmbusPpi,
                             SmbusSlaveAddress,
                             SmbusDeviceCommand,
                             SmbusOperation,
                             FALSE,
                             &OperationLength,
                             &DataBuffer[0]
                             );
      }
    }
  }
  
  //
  // Find INIT table and init CLKGEN 
  //  
  for (ClockGenIndex = 0; ClockGenIndex < IndexEnd ; ClockGenIndex++) {
  
    ShiftBit(RealClkGenInfo.ClkGenData[ClkGenBigTable[ClockGenIndex].DeviceInfo->VenderIdReg.Byte],
             ClkGenBigTable[ClockGenIndex].DeviceInfo->VenderIdReg.Mask, 
             &RealClkGenInfo.ClkGenVId);
    
    ShiftBit(RealClkGenInfo.ClkGenData[ClkGenBigTable[ClockGenIndex].DeviceInfo->DeviceIdReg.Byte],
             ClkGenBigTable[ClockGenIndex].DeviceInfo->DeviceIdReg.Mask, 
             &RealClkGenInfo.ClkGenDId);
    
    if (ClkGenBigTable[ClockGenIndex].UnionDataInfo->ExtraInfo->ChipVId == NO_VERFY_CHIP_VID) {
      RealClkGenInfo.NBVId = NO_VERFY_CHIP_VID;
    }  
      
    if (ClkGenBigTable[ClockGenIndex].UnionDataInfo->ExtraInfo->VgaType == NO_VERFY_VGA) {
      RealClkGenInfo.VgaType = NO_VERFY_VGA;
    }
          
    if (ClkGenBigTable[ClockGenIndex].DeviceInfo->VenderId == NO_VERFY_CLKGEN_VID_DID) {
      RealClkGenInfo.ClkGenVId = NO_VERFY_CLKGEN_VID_DID;
    }
    
    if (ClkGenBigTable[ClockGenIndex].DeviceInfo->DeviceId == NO_VERFY_CLKGEN_VID_DID) {
      RealClkGenInfo.ClkGenDId = NO_VERFY_CLKGEN_VID_DID;
    } 
    
    //
    // Compare NB Vendor ID, Compare Platform ID, Compare Graphic Type, Compare Init Type, Compare CLKGEN Vendor ID and CLKGEN Device ID
    //
    
    if ((ClkGenBigTable[ClockGenIndex].UnionDataInfo->ExtraInfo->InitType == CLOCK_GEN_INIT) && 
        (ClkGenBigTable[ClockGenIndex].UnionDataInfo->ExtraInfo->ChipVId == RealClkGenInfo.NBVId) && 
        (ClkGenBigTable[ClockGenIndex].UnionDataInfo->ExtraInfo->PlatformId == RealClkGenInfo.PlatformId) && 
        (ClkGenBigTable[ClockGenIndex].UnionDataInfo->ExtraInfo->VgaType == RealClkGenInfo.VgaType) && 
        (ClkGenBigTable[ClockGenIndex].DeviceInfo->VenderId == RealClkGenInfo.ClkGenVId) && 
        (ClkGenBigTable[ClockGenIndex].DeviceInfo->DeviceId == RealClkGenInfo.ClkGenDId)){
        
      //
      // Write Smbus
      //                   
      SmbusSlaveAddress.SmbusDeviceAddress = SlaveAddress;
      SmbusDeviceCommand = (UINTN)StartOffset;
  
      Status = ClockGenWriteSmBus ((EFI_PEI_SERVICES**) PeiServices,
                                   SmbusPpi,
                                   SmbusSlaveAddress,
                                   SmbusDeviceCommand,
                                   ClkGenBigTable[ClockGenIndex].UnionDataInfo->DataInfo,
                                   RealClkGenInfo.ClkGenData,
                                   RealClkGenInfo.BlockSize
                                   );
      
      if (EFI_ERROR (Status)) {
        //
        // Message will show on command line and Procedure will not dead lock.
        //
        DEBUG ((EFI_D_ERROR, "Write CLKGEN data error\n"));
        return Status;
      } else {
        break;
      }         
            
    } else if (ClockGenIndex == (IndexEnd - 1)){
      return EFI_UNSUPPORTED;
    }
  }
  
  return EFI_SUCCESS;
}

/**
  SMBus write function for clock gen.
  
  @param  InitTable     Clock Generator information table
  @param  DataBuffer    Real Clock Generator data
  @param  DataLength    Real Clock Generator data length
  @param  DataInfoSize  Clock Generator information table size

  @retval EFI_SUCCESS If Clock Generator write success.

**/
EFI_STATUS
ClockGenWriteSmBus (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_SMBUS2_PPI            *SmbusPpi,
  IN EFI_SMBUS_DEVICE_ADDRESS      SmbusSlaveAddress,
  IN EFI_SMBUS_DEVICE_COMMAND      SmbusDeviceCommand,
  IN CLOCK_GEN_DATA_INFO           *InitTable,
  IN UINT8                         *DataBuffer,
  IN UINTN                         DataLength
  )
{
  EFI_SMBUS_OPERATION           SmbusOperation;
  UINTN                         InitTableIndex;
  EFI_STATUS                    Status;
  UINTN                         OperationLength;
  BOOLEAN                       SmbusWriteBlock;
  EFI_SMBUS_DEVICE_COMMAND      Command;
  UINT8                         FixData;
  UINT8                         Index;
 
  Index = 0;
  FixData = 0;
  InitTableIndex = 0;
  
  SmbusWriteBlock = FALSE;
  while (InitTable [InitTableIndex].WriteType != CLKGEN_DATA_END) {
    switch (InitTable [InitTableIndex].WriteType) {
      
      case CLKGEN_BLOCK_OPERATION:
      case CLKGEN_FAST_BLOCK_OPERATION:
        DataBuffer [InitTable[InitTableIndex].Byte] = ((DataBuffer [InitTable[InitTableIndex].Byte] & InitTable [InitTableIndex].AndSourceValue) | InitTable [InitTableIndex].OrSourceValue);
        SmbusWriteBlock = TRUE;
        break;
      case CLKGEN_BYTE_OPERATION:
        SmbusOperation = EfiSmbusWriteByte;
        OperationLength = 1;
        Command = InitTable [InitTableIndex].Byte;
        
        FixData = ((DataBuffer [InitTable[InitTableIndex].Byte] & InitTable [InitTableIndex].AndSourceValue) | InitTable [InitTableIndex].OrSourceValue);
        
        Status = SmbusPpi->Execute (
                             SmbusPpi,
                             SmbusSlaveAddress,
                             Command,
                             SmbusOperation,
                             FALSE,
                             &OperationLength,
                             &FixData
                             );
        if (EFI_ERROR (Status)) {
          return Status;
        }
        break;
      default:
        break;
    }
    
    InitTableIndex++;
  }
 
  if (SmbusWriteBlock) {
    Command = SmbusDeviceCommand;
    SmbusOperation = EfiSmbusWriteBlock;
    while (TRUE) {
      
      if (DataLength > MAX_SMBUS_RW_BYTE) {
      
        OperationLength = MAX_SMBUS_RW_BYTE;
       
        Status = SmbusPpi->Execute (
                             SmbusPpi,
                             SmbusSlaveAddress,
                             Command,
                             SmbusOperation,
                             FALSE,
                             &OperationLength,
                             &DataBuffer[MAX_SMBUS_RW_BYTE * Index]
                             );
        if (EFI_ERROR (Status)) {
          return Status;
        }
        
        Index++;
        DataLength -= MAX_SMBUS_RW_BYTE;
      
      } else {
 
        OperationLength = DataLength;
        
        Status = SmbusPpi->Execute (
                             SmbusPpi,
                             SmbusSlaveAddress,
                             Command,
                             SmbusOperation,
                             FALSE,
                             &OperationLength,
                             &DataBuffer[MAX_SMBUS_RW_BYTE * Index]
                             );
        if (EFI_ERROR (Status)) {
          return Status;
        }
 
        break;
      }
    }
  }
  return EFI_SUCCESS;
}

/**
  Shift Byte.
  
  @param  ByteValue  Original byte value
  @param  MaskValue  Mask value

  @retval EFI_SUCCESS If byte value shift success.
  
**/
EFI_STATUS
ShiftBit (
  IN  UINT8                        ByteValue,
  IN  UINT8                        MaskValue,
  OUT UINT8                        *RealByteValue
  )
{
  *RealByteValue = ByteValue & MaskValue;
 
  return EFI_SUCCESS;
}

