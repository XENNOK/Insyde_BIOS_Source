/** @file
  SATA Misc

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <SataMisc.h>
#include <Protocol/GlobalNvsArea.h>
EFI_GLOBAL_NVS_AREA_PROTOCOL      *mGlobalNvsAreaProtocol = NULL;

//
// This GUID must match the AHCI ROM file guid
//
EFI_GUID gSataAhciRomFileGuid = {
    0xB017C09D, 0xEDC1, 0x4940, 0xB1, 0x3E, 0x57, 0xE9, 0x56, 0x60, 0xC9, 0x0F
};

EFI_STATUS
GetImage (
  IN  EFI_GUID           *NameGuid,
  IN  EFI_SECTION_TYPE   SectionType,
  OUT VOID               **Buffer,
  OUT UINTN              *Size
  )
/*++

Routine Description:

  Get specified image from a firmware volume.

Arguments:

  NameGuid    - File Name GUID
  SectionType - Sectio type
  Buffer      - Buffer to contain image
  Size        - Image size

Returns:

  EFI_INVALID_PARAMETER - Invalid parameter
  EFI_NOT_FOUND         - Can not find the file
  EFI_SUCCESS           - Successfully completed

--*/  
{
  EFI_STATUS                    Status;
  EFI_HANDLE                    *HandleBuffer;
  UINTN                         HandleCount;
  UINTN                         Index;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
  EFI_FV_FILETYPE               FileType;
  EFI_FV_FILE_ATTRIBUTES        Attributes;
  UINT32                        AuthenticationStatus;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &HandleCount,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Find desired image in all Fvs
  //
  for (Index = 0; Index < HandleCount; ++Index) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID**)&Fv
                    );

    if (EFI_ERROR (Status)) {
      (gBS->FreePool)(HandleBuffer);
      return Status;
    }

    //
    // Read desired section content in NameGuid file
    //
    *Buffer     = NULL;
    *Size       = 0;
    Status      = Fv->ReadSection (
                        Fv,
                        NameGuid,
                        SectionType,
                        0,
                        Buffer,
                        Size,
                        &AuthenticationStatus
                        );

    if (EFI_ERROR (Status) && (SectionType == EFI_SECTION_TE)) {
      //
      // Try reading PE32 section, since the TE section does not exist
      //
      *Buffer = NULL;
      *Size   = 0;
      Status  = Fv->ReadSection (
                      Fv,
                      NameGuid,
                      EFI_SECTION_PE32,
                      0,
                      Buffer,
                      Size,
                      &AuthenticationStatus
                      );
    }

    if (EFI_ERROR (Status) &&
        ((SectionType == EFI_SECTION_TE) || (SectionType == EFI_SECTION_PE32))) {
      //
      // Try reading raw file, since the desired section does not exist
      //
      *Buffer = NULL;
      *Size   = 0;
      Status  = Fv->ReadFile (
                      Fv,
                      NameGuid,
                      Buffer,
                      Size,
                      &FileType,
                      &Attributes,
                      &AuthenticationStatus
                      );
    }

    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  (gBS->FreePool)(HandleBuffer);

  //
  // Not found image
  //
  if (Index == HandleCount) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

VOID
LoadAHCIRAIDOpROM (
  IN EFI_SATA_CONTROLLER_PRIVATE_DATA  *SataPrivateData,
  IN PCI_TYPE00                        *PciData,
  IN EFI_HANDLE                        Controller
  )
/*++

Routine Description:

  Load AHCI or RAID Option ROM.

Arguments:

  SataPrivateData    
  PciData            

Returns:


--*/  
{
  EFI_STATUS                        Status;
  EFI_LEGACY_BIOS_PROTOCOL          *LegacyBios;
  UINT8                             DiskStart;
  UINT8                             DiskEnd;
  UINTN                             Flags;
#ifdef SEAMLESS_AHCI_SUPPORT    
  VOID                              *AhciRomAddr;
  UINTN                             AhciRomSize;
#endif  

  Status = gBS->LocateProtocol (&gEfiLegacyBiosProtocolGuid, NULL, (VOID **) &LegacyBios);
  if (!EFI_ERROR (Status)) {
    if (PciData->Hdr.ClassCode[1] == V_PCH_SATA_SUB_CLASS_CODE_AHCI ||
        PciData->Hdr.ClassCode[1] == V_PCH_SATA_SUB_CLASS_CODE_RAID) {
        //
        // Load RAID OPROM
        //
        Status = LegacyBios->CheckPciRom (
                               LegacyBios,
                               Controller,
                               NULL,
                               NULL,
                               &Flags
                               );
        if (!EFI_ERROR (Status)) {
          Status = LegacyBios->InstallPciRom (
                                 LegacyBios,
                                 Controller,
                                 NULL,
                                 &Flags,
                                 &DiskStart,
                                 &DiskEnd,
                                 NULL,
                                 NULL
                                 );
        }
#ifdef SEAMLESS_AHCI_SUPPORT    
      if ((PciData->Hdr.ClassCode[1] == V_PCH_SATA_SUB_CLASS_CODE_RAID)) {
        //
        // Install AHCI Rom to control any uncontrolled device
        //
        Status = GetImage(&gSataAhciRomFileGuid, EFI_SECTION_RAW, &AhciRomAddr, &AhciRomSize);
        if (!EFI_ERROR (Status) && AhciRomSize != 0) {
          Status = LegacyBios->InstallPciRom (
                                 LegacyBios,
                                 Controller,
                                 &AhciRomAddr,
                                 &Flags,
                                 NULL,
                                 NULL,
                                 NULL,
                                 NULL
                                 );
          (gBS->FreePool) (AhciRomAddr);
        }
      }
#endif
    }
  }
}

EFI_STATUS
IdeInitSetUdmaTiming (
  IN  UINT8                       Channel,
  IN  UINT8                       Device,
  IN  EFI_PCI_IO_PROTOCOL         *PciIo,
  IN  EFI_ATA_COLLECTIVE_MODE     *Modes
  )
/*++

Routine Description:

  This function is used to set appropriate UDMA timing on SATA
  controller according supported UDMA modes

Arguments:

  Channel -- IDE channel number (0 based, either 0 or 1)
  Device  -- IDE device number
  PciIo   -- Pointer to PciIo protocol opened by SATA controller driver
  Modes   -- The UDMA mode collection supported by IDE device

Returns:

--*/
{
  UINT16        PciCommandReg;
  UINT16        UdmaControlReg;
  UINT32        IdeIoConfigReg;
  UINT8         Index;
//[-start-130625-IB05400421-add]//
  UINT16        UdmaTimingReg;
  UINT8         DeviceTimingIndex;
//[-end-130625-IB05400421-add]//

  PciCommandReg  = 0;
  UdmaControlReg = 0;
  IdeIoConfigReg = 0;
  Index          = (Channel << 1) + Device;
//[-start-130625-IB05400421-add]//
  DeviceTimingIndex = Index << 2;
//[-end-130625-IB05400421-add]//

  //
  // Read all UDma And Ide Configuration/Timing related registers
  //
  PciIo->Pci.Read (PciIo,
                    EfiPciIoWidthUint16,
                    R_PCH_SATA_COMMAND,
                    1,
                    &PciCommandReg
                     );
  //
  // Ultre DMA Control Register, offset 0x48 Default value 00
  //
 PciIo->Pci.Read (PciIo,
                    EfiPciIoWidthUint16,
                    R_PCH_SIDE_SDMA_CNT,
                    1,
                    &UdmaControlReg
                     );  
//[-start-130709-IB10930000-modify]//
//[-start-130625-IB05400421-add]//
  //
  // Ultra DMA Timing Register, offset 0x4A Default value 00
  //
  PciIo->Pci.Read (PciIo,
                  EfiPciIoWidthUint16,
                  R_PCH_SIDE_SDMA_TIM,
                  1,
                  &UdmaTimingReg
                   );
//[-end-130625-IB05400421-add]//
//[-end-130709-IB10930000-modify]//
  //
  // IDE I/O Configuration 0x54-55 Default value 00
  //
 PciIo->Pci.Read (PciIo,
                    EfiPciIoWidthUint32,
                    R_PCH_SIDE_IDE_CONFIG,
                    1,
                    &IdeIoConfigReg
                     );   
  //
  // Set the Primary Drive 0 UDMA Enable bit offset 0x48
  // BIT0-BIT4 For PSDE0,PSDE1,SSDE0,SSDE1
  //
  UdmaControlReg |= (B_PCH_SIDE_SDMA_CNT_PSDE0 << Index);


  //
  // Calculated best UDMA mode
  //
  switch (Modes->UdmaMode.Mode) {
  case 0:
    //
    // Clear PCB0/PCB1 to 33MHz
    //
    IdeIoConfigReg &= (~(B_PCH_SIDE_IDE_CONFIG_PCB0 << Index));
//[-start-130625-IB05400421-add]//
    UdmaTimingReg |= (V_PCH_SIDE_SDMA_TIM_CT4_RP6 << DeviceTimingIndex);
//[-end-130625-IB05400421-add]//

    break;

  case 1:
  case 2:
    //
    // Clear PCB0/PCB1 to 33MHz
    //
    IdeIoConfigReg &= (~(B_PCH_SIDE_IDE_CONFIG_PCB0 << Index));
//[-start-130625-IB05400421-add]//
    if (Modes->UdmaMode.Mode == 1) {
      UdmaTimingReg |= (V_PCH_SIDE_SDMA_TIM_CT3_RP5 << DeviceTimingIndex);
    } else {
      UdmaTimingReg |= (V_PCH_SIDE_SDMA_TIM_CT2_RP4 << DeviceTimingIndex);
    }
//[-end-130625-IB05400421-add]//
    break;


  case 3:
  case 4:
    //
    // Set PCB0/PCB1 to 66MHz
    //
    IdeIoConfigReg |= (B_PCH_SIDE_IDE_CONFIG_PCB0 << Index);
//[-start-130625-IB05400421-add]//
    if (Modes->UdmaMode.Mode == 3) {
      UdmaTimingReg |= (V_PCH_SIDE_SDMA_TIM_CT3_RP8 << DeviceTimingIndex);
     } else {
      UdmaTimingReg |= (V_PCH_SIDE_SDMA_TIM_CT2_RP8 << DeviceTimingIndex);
     }
//[-end-130625-IB05400421-add]//
    break;

  case 5: // Mode 5, UDma 100
    //
    // Set FAST_PCB0/FAST_PCB1 to 133MHz
    //
    IdeIoConfigReg |= (B_PCH_SIDE_IDE_CONFIG_FAST_PCB0 << Index);
//[-start-130625-IB05400421-add]//
    UdmaTimingReg |= (V_PCH_SIDE_SDMA_TIM_CT3_RP16 << DeviceTimingIndex);
//[-end-130625-IB05400421-add]//
    break;

  default:
    DEBUG((EFI_D_ERROR, "SATA Controller ERROR: IdeInitSetUdmaTiming - Unkown Udma Mode!\n"));
    return EFI_INVALID_PARAMETER;
  }

  //
  // Now set the PCH IDE Bus Master Enable bit, one bit for PCH controller
  // If BME bit is not set, set it
  //
  if (!(PciCommandReg & B_PCH_SATA_COMMAND_BME)) {
    PciCommandReg |= B_PCH_SATA_COMMAND_BME;
    PciIo->Pci.Write (PciIo,
                       EfiPciIoWidthUint16,
                       R_PCH_SATA_COMMAND,
                       1,
                       &PciCommandReg
                        );     
  }

  //
  // Write to Synchronous DMA Control Register
  //
  PciIo->Pci.Write (PciIo,
                     EfiPciIoWidthUint16,
                     R_PCH_SIDE_SDMA_CNT,
                     1,
                     &UdmaControlReg
                      );  
//[-start-130301-IB04560352-add]//
  //
  // Write to Synchronous DMA timing register
  // 
  PciIo->Pci.Write (PciIo,
                   EfiPciIoWidthUint16,
                   R_PCH_SIDE_SDMA_TIM,
                   1,
                   &UdmaTimingReg
                    );   
//[-end-130301-IB04560352-add]//
  //
  // Write IDE I/O Configuration Register contents
  //
  PciIo->Pci.Write (PciIo,
                     EfiPciIoWidthUint32,
                     R_PCH_SIDE_IDE_CONFIG,
                     1,
                     &IdeIoConfigReg
                      );    

  return EFI_SUCCESS;
}

EFI_STATUS
IdeInitSetPioTiming (
  IN  UINT8                       Channel,
  IN  UINT8                       Device,
  IN  EFI_PCI_IO_PROTOCOL         *PciIo,
  IN  EFI_IDENTIFY_DATA           *IdentifyData,
  IN  EFI_ATA_COLLECTIVE_MODE     *Modes
  )
/*++

Routine Description:

  This function is used to set appropriate PIO timing on Ide
  controller according supported PIO modes

Arguments:

  Channel      -- IDE channel number (0 based, either 0 or 1)
  Device       -- IDE device number
  PciIo        -- Pointer to PciIo protocol opened by Ide controller driver
  IdentifyData -- The identify struct submitted by IDE device
  Modes        -- The PIO mode collection supported by IDE device

Returns:

--*/
{
  UINT16        TimingReg;
  UINT8         SlaveTimingReg;
  BOOLEAN       IsAtapiDevice;
  UINT16        Index;
  UINT16        DeviceID;
  EFI_STATUS    Status;

  ASSERT (IdentifyData);

  Index         = (Channel << 1) + Device;
  DeviceID      = 0;

  Status = gBS->LocateProtocol (&gEfiGlobalNvsAreaProtocolGuid, NULL, (VOID **)&mGlobalNvsAreaProtocol);
  if (EFI_ERROR(Status)){
    DEBUG ((EFI_D_ERROR, "Can not get GlobalNvs protocal\n")); 
    ASSERT_EFI_ERROR (Status);
    return Status;
  }

  if ((IdentifyData->AtaData.config & BIT15) == 0) {
    IsAtapiDevice = FALSE;

  } else {
    IsAtapiDevice = TRUE;
  }
  
  PciIo->Pci.Read (PciIo,
                    EfiPciIoWidthUint16,
                    (R_PCH_SIDE_TIMP + (Channel << 1)),
                    1,
                    &TimingReg
                     );   
  PciIo->Pci.Read (PciIo,
                    EfiPciIoWidthUint8,
                    R_PCH_SIDE_SIDETIM,
                    1,
                    &SlaveTimingReg
                     );

  //
  // General rules for setting those timing control bits
  //
  // 1. DMA-only is enabled with DTE bit set  where a drive reports a PIO speed
  //    much slower than its reported DMA speed.
  // 2. IORDY sample point enable (IE) bit is enabled if the operating mode is
  //    mode-2 or faster, but the device must also support IORDY flow control.
  // 3. Pre-fetch/posting enable bit shall be set if the operating mode is
  //    mode-2 or faster, but not if the device is an ATAPI device.
  // 4. Fast Timing Bank enable shall be enabled if the it is mode-2 or faster.
  // 5. The operating mode shall be controlled by the device's PIO speed if the
  //    best PIO mode is 2 mode speeds faster than the DMA mode.
  //

  //
  // Only set conSATAr STRIE bit for Slave
  //
  if (Device == PCH_SATA_SLAVE_DRIVE) {
    TimingReg |= B_PCH_SIDE_TIM_SITRE;
  }

  switch (Modes->PioMode.Mode) {

    case 0:
    case 1:
      //
      // ATA_PIO_MODE_BELOW_2, slowest PIO mode, disable all advanced features
      //
      TimingReg &= (~(B_PCH_SIDE_TIM_TIME0 << (Device * 4))); // Use default, disable all advanced features
      TimingReg &= (~(B_PCH_SIDE_TIM_IE0 << (Device * 4)));
      TimingReg &= (~(B_PCH_SIDE_TIM_PPE0 << (Device * 4)));
      TimingReg &= (~(B_PCH_SIDE_TIM_DTE0 << (Device * 4)));
      //
      // Set for IORDY and Recovery time, first clear these bits
      //
      if (Device == PCH_SATA_MASTER_DRIVE) {
        TimingReg &= (~B_PCH_SIDE_TIM_ISP_MASK);
        TimingReg &= (~B_PCH_SIDE_TIM_RCT_MASK);
        //
        // Then set clocks
        //
        TimingReg |= (V_PCH_SIDE_TIM_ISP_5_CLOCK << 12);
        TimingReg |= (V_PCH_SIDE_TIM_RCT_4_CLOCK << 8);
      } else {
        SlaveTimingReg &= (~B_PCH_SIDE_SIDETIM_PISP1_MASK << (Channel * 4));
        SlaveTimingReg &= (~B_PCH_SIDE_SIDETIM_PRCT1_MASK << (Channel * 4));
        //
        // Then set clocks
        //
        SlaveTimingReg |= (V_PCH_SIDE_TIM_ISP_5_CLOCK << 2);
        SlaveTimingReg |= (V_PCH_SIDE_TIM_RCT_4_CLOCK << 0);
      }
      break;

    case 2:
      //
      // ATA_PIO_MODE_2
      //
      TimingReg |= (B_PCH_SIDE_TIM_TIME0 << (Device * 4));    // Enable fast timing bank
      TimingReg &= (~(B_PCH_SIDE_TIM_IE0 << (Device * 4)));   // Disable IODRY sampling
      TimingReg &= (~(B_PCH_SIDE_TIM_DTE0 << (Device * 4)));  // Share PIO, MDAM and SDMA timing
      //
      // Enable prefetch for ATA device but not for ATAPI device
      //
      if (IsAtapiDevice == TRUE) {
        TimingReg &= (~(B_PCH_SIDE_TIM_PPE0 << (Device * 4)));
      } else {
        TimingReg |= (B_PCH_SIDE_TIM_PPE0 << (Device * 4));
      }
      //
      // Set for IORDY and Recovery time, first clear these bits
      //
      if (Device == PCH_SATA_MASTER_DRIVE) {
        TimingReg &= (~B_PCH_SIDE_TIM_ISP_MASK);
        TimingReg &= (~B_PCH_SIDE_TIM_RCT_MASK);
        //
        // Then set clocks
        //
        TimingReg |= (V_PCH_SIDE_TIM_ISP_4_CLOCK << 12);
        TimingReg |= (V_PCH_SIDE_TIM_RCT_4_CLOCK << 8);
      } else {
        SlaveTimingReg &= (~B_PCH_SIDE_SIDETIM_PISP1_MASK << (Channel * 4));
        SlaveTimingReg &= (~B_PCH_SIDE_SIDETIM_PRCT1_MASK << (Channel * 4));
        //
        // Then set clocks
        //
        SlaveTimingReg |= (V_PCH_SIDE_TIM_ISP_4_CLOCK << 2);
        SlaveTimingReg |= (V_PCH_SIDE_TIM_RCT_4_CLOCK << 0);
      }

    case 3:
      //
      // ATA_PIO_MODE_3, should support PIO with flow control (ATA/ATAPI spec)
      //
      TimingReg |= (B_PCH_SIDE_TIM_TIME0 << (Device * 4));   // Enable fast timing bank
      TimingReg |= (B_PCH_SIDE_TIM_IE0 << (Device * 4));     // Enable IODRY sampling
      TimingReg &= (~(B_PCH_SIDE_TIM_DTE0 << (Device * 4))); // Share PIO, MDAM and SDMA timing
      //
      // Enable prefetch for ATA device but not for ATAPI device
      //
      if (IsAtapiDevice == TRUE) {
        TimingReg &= (~(B_PCH_SIDE_TIM_PPE0 << (Device * 4)));
      } else {
        TimingReg |= (B_PCH_SIDE_TIM_PPE0 << (Device * 4));
      }
      //
      // Set for IORDY and Recovery time, first clear these bits
      //
      if (Device == PCH_SATA_MASTER_DRIVE) {
        TimingReg &= (~B_PCH_SIDE_TIM_ISP_MASK);
        TimingReg &= (~B_PCH_SIDE_TIM_RCT_MASK);
        //
        // Then set clocks
        //
        TimingReg |= (V_PCH_SIDE_TIM_ISP_3_CLOCK << 12);
        TimingReg |= (V_PCH_SIDE_TIM_RCT_3_CLOCK << 8);
      } else {
        SlaveTimingReg &= (~B_PCH_SIDE_SIDETIM_PISP1_MASK << (Channel * 4));
        SlaveTimingReg &= (~B_PCH_SIDE_SIDETIM_PRCT1_MASK << (Channel * 4));
        //
        // Then set clocks
        //
        SlaveTimingReg |= (V_PCH_SIDE_TIM_ISP_3_CLOCK << 2);
        SlaveTimingReg |= (V_PCH_SIDE_TIM_RCT_3_CLOCK << 0);
      }
      break;

    case 4:
      //
      // ATA_PIO_MODE_4, fasest PIO mode, should support PIO with flow control
      //
      TimingReg |= (B_PCH_SIDE_TIM_TIME0 << (Device * 4));   // Enable fast timing bank
      TimingReg |= (B_PCH_SIDE_TIM_IE0 << (Device * 4));     // Enable IODRY sampling
      TimingReg &= (~(B_PCH_SIDE_TIM_DTE0 << (Device * 4))); // Share PIO, MDAM and SDMA timing
      //
      // Enable prefetch for ATA device but not for ATAPI device
      //
      if (IsAtapiDevice == TRUE) {
        TimingReg &= (~(B_PCH_SIDE_TIM_PPE0 << (Device * 4)));
      } else {
        TimingReg |= (B_PCH_SIDE_TIM_PPE0 << (Device * 4));
      }
      //
      // Set for IORDY and Recovery time, first clear these bits
      //
      if (Device == PCH_SATA_MASTER_DRIVE) {
        TimingReg &= (~B_PCH_SIDE_TIM_ISP_MASK);
        TimingReg &= (~B_PCH_SIDE_TIM_RCT_MASK);
        //
        // Then set clocks
        //
        TimingReg |= (V_PCH_SIDE_TIM_ISP_3_CLOCK << 12);
        TimingReg |= (V_PCH_SIDE_TIM_RCT_1_CLOCK << 8);
      } else {
        SlaveTimingReg &= (~B_PCH_SIDE_SIDETIM_PISP1_MASK << (Channel * 4));
        SlaveTimingReg &= (~B_PCH_SIDE_SIDETIM_PRCT1_MASK << (Channel * 4));
        //
        // Then set clocks
        //
        SlaveTimingReg |= (V_PCH_SIDE_TIM_ISP_3_CLOCK << 2);
        SlaveTimingReg |= (V_PCH_SIDE_TIM_RCT_1_CLOCK << 0);
      }
      break;
  }

  //
  // Write timing result back to timing register
  //
  PciIo->Pci.Read (PciIo,
                     EfiPciIoWidthUint16,
                     R_PCH_SATA_DEVICE_ID,
                     1,
                     &DeviceID
                      );  
//[-start-120628-IB06460410-modify]//
  if (DeviceID == V_PCH_LPTH_SATA_DEVICE_ID_M_IDE|| DeviceID == V_PCH_LPTH_SATA_DEVICE_ID_D_IDE) {
    if ( Channel == 0){
      mGlobalNvsAreaProtocol->Area->SATA1Channel0PioTiming = TimingReg; 
    }
    if ( Channel == 1){
      mGlobalNvsAreaProtocol->Area->SATA1Channel1PioTiming = TimingReg; 
    }
  } else if (DeviceID == V_PCH_LPTH_SATA2_DEVICE_ID_M_IDE|| DeviceID == V_PCH_LPTH_SATA2_DEVICE_ID_D_IDE) {
    if ( Channel == 0){
      mGlobalNvsAreaProtocol->Area->SATA2Channel0PioTiming = TimingReg; 
    }
    if ( Channel == 1){
      mGlobalNvsAreaProtocol->Area->SATA2Channel1PioTiming = TimingReg; 
    }
  }
//[-end-120628-IB06460410-modify]//
  
  PciIo->Pci.Write (PciIo,
                     EfiPciIoWidthUint8,
                    R_PCH_SIDE_SIDETIM,
                     1,
                     &SlaveTimingReg
                      );   

  return EFI_SUCCESS;
}


EFI_STATUS
IdeInitSetting (
  IN  EFI_IDE_CONTROLLER_INIT_PROTOCOL    *This,
  IN  UINT8                               Channel,
  IN  UINT8                               Device,
  IN  EFI_ATA_COLLECTIVE_MODE             *Modes
  )
/*++

Routine Description:

  This function is hooked by IdeInitSetTiming function for setting IDE timing

Arguments:

  Channel        -- IDE channel number (0 based, either 0 or 1)
  Device         -- IDE device number
  Modes          -- Operation modes

Returns:
  EFI_SUCCESS       - This function always returns EFI_SUCCESS    

--*/
{
  EFI_SATA_CONTROLLER_PRIVATE_DATA  *SataPrivateData;
  SataPrivateData = SATA_CONTROLLER_PRIVATE_DATA_FROM_THIS (This);
  ASSERT (SataPrivateData);

  if ((Channel >= This->ChannelCount) || (Modes == NULL) || (Device >= PCH_SATA_1_MAX_DEVICES)) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Make sure we've got the valid identify data of the device from SubmitData()
  //
  if (!(SataPrivateData->IdentifyValid[Channel][Device]) ) {
    return EFI_NOT_READY;
  }

  //If Pio and UDMA are not supported, return EFI_UNSUPPORTED
  if ((!Modes->PioMode.Valid) && (!Modes->UdmaMode.Valid)) {
    return EFI_UNSUPPORTED;
  }
  
  //
  // Only PIO mode and UDMA mode are supported
  //
  //if UDMA is supported, Set UDMA Mode
  if (Modes->UdmaMode.Valid) {
    IdeInitSetUdmaTiming (Channel, Device, SataPrivateData->PciIo, Modes);    
  }

  //If PIO is supported, Set PIO Mode
  if (Modes->PioMode.Valid) {
    IdeInitSetPioTiming(Channel, Device, SataPrivateData->PciIo, &(SataPrivateData->IdentifyData[Channel][Device]), Modes);
  }
  
  return EFI_SUCCESS;
}

