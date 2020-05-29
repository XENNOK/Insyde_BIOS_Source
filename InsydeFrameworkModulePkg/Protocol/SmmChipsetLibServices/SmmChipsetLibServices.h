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
//; Module Name:
//; 
//;   SmmChipsetLibServices.h
//; 
//; Abstract:
//; 
//;   SmmChipset Lib Interface
//; 

#ifndef _SMM_CHIPSET_LIB_INTERFACE_H_
#define _SMM_CHIPSET_LIB_INTERFACE_H_

#include "Tiano.h"
#include "ChipsetLib.h"

EFI_FORWARD_DECLARATION (EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL);

#define EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL_GUID \
   {0x92363E98, 0x5811, 0x49B4, 0xAA, 0x42, 0xDD, 0x81, 0x7C, 0xA0, 0x4C, 0x20}

typedef
EFI_STATUS
(EFIAPI *EFI_SATA_COMRESET) (
  IN  UINTN                    PortNumber
  );

typedef
VOID
(EFIAPI *EFI_SET_AHCI_ENABLE_BIT) (
  BOOLEAN                      EnableOrDisable
  )
/*++

  Routine Description:
    Set the Ahci controller AE(Ahci enable) bit  
  Arguments:
    EnableOrDisable - Boolean to enable/disable AE bit
  Returns:
    VOID

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_ENABLE_FVB_WRITES) (
  IN  BOOLEAN                  EnableWrites
  )
/*++

  Routine Description:
    Platform specific function to enable flash / hardware
  Arguments:
    EnableWrites - Boolean to enable/disable flash
  Returns:
    EFI_SUCCESS

--*/
;

typedef
EFI_STATUS
(EFIAPI *EFI_INIT_FLASH_MODE) (
  IN  UINT8                    *FlashMode
  )
/*++

  Routine Description:
    Initialize Flash mode pointer for libary to use
  Arguments:
    FlashMode - FlashMode pointer
  Returns:
    EFI_SUCCESS

--*/
;

typedef
VOID
(EFIAPI *EFI_SELECT_FLASH_INTERFACE) (
  IN UINT32                    FlashDev
  )
/*++

  Routine Description:
    Chipset specific function for selected device flash / hardware
  Arguments:
    FlashDev - The number of selected flash device
  Returns:
    EFI_SUCCESS

--*/
;

typedef
BOOLEAN
(EFIAPI *EFI_IS_SPI_FLASH_DEVICE) (
  VOID
  );

typedef
BOOLEAN
(EFIAPI *EFI_IS_LPC_FLASH_DEVICE) (
  VOID
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_FLASH_TABLE) (
  IN OUT   UINT8               *DataBuffer
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_FLASH_NUMBER) (
  OUT   UINT8                  *FlashNumber
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_SPI_SUPPORT_NUMBER) (
  OUT UINTN                    *SpiSupportNumber
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_SPI_FD_SUPPORT_TABLE) (
  OUT VOID                     ***SPIFdSupportTablePtr
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SPI_MEM_TO_BIOS_REG_ADDRESS) (
  IN UINTN                     MemoryAddress,
  OUT UINTN                    *BiosRegionAddress,
  IN BOOLEAN                   SPIDev
  );

typedef
BOOLEAN
(EFIAPI *EFI_NEED_TO_BUILD_IDE_DATA) (
  VOID                         *SystemConfiguration
  )
/*++

  Routine Description:
    Check if system need to build ide data.
  Arguments:
    SystemConfiguration - The pointer to SYSTEM_CONFIGURATION
  Returns:
    True - IDE mode or AHCI mode without OpRom, run BuildIdeData function.
    False - Don't run BuildIdeData function.
    (If no support this,please return "True")
    
--*/
;

typedef
BOOLEAN
(EFIAPI *EFI_IS_RAID_OPROM) (
  IN VOID                      *Pci,
  IN UINTN                     Bus,
  IN UINTN                     Device,
  IN UINTN                     Function,
  IN UINTN                     Index  
  )
/*++

  Routine Description:
    Check if support RAID OpRom.
  Arguments:
    Pci -  Point to Type00 Pci configuration header
    Bus -  PCI bus
    Device -  PCI Device
    Function -  PCI Function
    Index -  Number of device
  Returns:
    True - Support RAID OpRom for onbroad hdd.
    False - No support RAID OpRom for onbroad hdd.
    (If no support this,please return "False")

--*/
;

typedef
BOOLEAN
(EFIAPI *EFI_RAID_AHCI_CDROM_SUPPORT) (
  )
/*++

  Routine Description:
    Check if AHCI OpROM (support ODD)with RAID mode.
  Arguments:
    None
  Returns:
    True - Support 
    False - Not support
  (If no support this,please return "False")
  
--*/
;

typedef
VOID
(EFIAPI *EFI_UPDATE_BBS_DEVICE_TYPE) (        
  VOID                         *SystemConfiguration,
  VOID                         *LocalBbsTable,
  UINTN                        *StartCount
  )
/*++

  Routine Description:
    Determine if need to update BBS device type .
  Arguments:
    SystemConfiguration - The pointer to SYSTEM_CONFIGURATION
    LocalBbsTable - The pointer to BBS_TABLE
    StartCount - The pointer to StartCount
  Returns:
    VOID

--*/
;

typedef
BOOLEAN
(EFIAPI *EFI_NEED_TO_OVERWRITE_VAR) (
  )
/*++

  Routine Description:
    Check if need to OverWrite Boot Variable.
  Arguments: 
    VOID  
  Returns:
    True - Yes, need to OverWrite Boot Variable.
    False - No.
    (If no support this,please return "False")

--*/
;

typedef
INTN
(EFIAPI *EFI_DIFFERENT_SIZE_STRCMP) (
  IN CHAR16                    *String,
  IN CHAR16                    *String2
  )
/*--

  Routine Description:
    Compare the string between String and String2, this function can check the
    same string with different size. Because some OpRom will put a few ¡§space¡¨  
    at the end of string. 
  Arguments:
    String - String to process
    String2 - The other string to process
  Returns:
    Return a positive integer if String is lexically greater than String2; Zero if 
    the two strings are identical; and a negative integer if String is lexically 
    less than String2.

--*/
;

typedef
VOID
(EFIAPI *EFI_RESET_SYSTEM) (
  IN EFI_RESET_TYPE            ResetType,
  IN EFI_STATUS                ResetStatus,
  IN UINTN                     DataSize,
  IN CHAR16                    *ResetData OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *EFI_GET_PORT_NUM_MAP_TABLE) (
  IN OUT PORT_NUMBER_MAP       **PortNumberMapTable,
  IN OUT UINTN                 *NoPorts
  );

typedef
EFI_STATUS
(EFIAPI *EFI_IO_DECODE_CTRL) (
  IN IO_DECODE_TYPE            Type,
  IN IO_DECODE_ADDRESS         Address,
  IN UINT16                    Length
  );

typedef
EFI_STATUS
(EFIAPI *EFI_MMIO_DECODE_CTRL) (
  IN MMIO_DECODE_ADDRESS       Address,
  IN UINT8                     IDSEL
  );

typedef
EFI_STATUS
(EFIAPI *EFI_IS_POSITIVE_DECODE_MODE_CHECK) (
  IN  UINT16                   VendorId,
  IN  UINT16                   DeviceId
  );

typedef
EFI_STATUS
(EFIAPI *EFI_LEGACY_REG_ACCESS_CTRL) (
  IN  UINTN                    Start,
  IN  UINTN                    Length,
  IN  UINTN                    Mode  
  );

typedef
EFI_STATUS
(EFIAPI *EFI_PROGRAM_CHIPSET_SSID) (
  UINT8                        Bus,
  UINT8                        Dev,
  UINT8                        Func,
  UINT16                       VendorId,
  UINT16                       DeviceId,
  UINT16                       ClassCode,
  UINT32                       SsidSvid
  );

typedef struct _EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL {
  EFI_SATA_COMRESET                  SataComreset;
  EFI_SET_AHCI_ENABLE_BIT            SetAhciEnableBit;
  EFI_ENABLE_FVB_WRITES              EnableFvbWrites;
  EFI_INIT_FLASH_MODE                InitFlashMode;
  EFI_SELECT_FLASH_INTERFACE         SelectFlashInterface;
  EFI_IS_SPI_FLASH_DEVICE            IsSpiFlashDevice;
  EFI_IS_LPC_FLASH_DEVICE            IsLpcFlashDevice;
  EFI_GET_FLASH_TABLE                GetFlashTable;
  EFI_GET_FLASH_NUMBER               GetFlashNumber;
  EFI_GET_SPI_SUPPORT_NUMBER         GetSpiSupportNumber;
  EFI_GET_SPI_FD_SUPPORT_TABLE       GetSpiFdSupportTable;
  EFI_SPI_MEM_TO_BIOS_REG_ADDRESS    SpiMemoryToBiosRegionAddress;
  EFI_NEED_TO_BUILD_IDE_DATA         NeedToBuildIdeData;
  EFI_IS_RAID_OPROM                  IsRaidOpRom;
  EFI_RAID_AHCI_CDROM_SUPPORT        RaidAhciCdromSupport;
  EFI_UPDATE_BBS_DEVICE_TYPE         UpdateBBSDeviceType;
  EFI_NEED_TO_OVERWRITE_VAR          NeedToOverWriteVar; 
  EFI_DIFFERENT_SIZE_STRCMP          DifferentSizeStrCmp;
  EFI_RESET_SYSTEM                   ResetSystem;
  EFI_GET_PORT_NUM_MAP_TABLE         GetPortNumberMapTable;
  EFI_IO_DECODE_CTRL                 IoDecodeControl;
  EFI_MMIO_DECODE_CTRL               MmioDecodeControl;
  EFI_IS_POSITIVE_DECODE_MODE_CHECK  IsaPositiveDecodeModeChecking;
  EFI_LEGACY_REG_ACCESS_CTRL         LegacyRegionAccessCtrl;
  EFI_PROGRAM_CHIPSET_SSID           ProgramChipsetSSID;
} EFI_SMM_CHIPSET_LIB_SERVICES_PROTOCOL;
 
extern EFI_GUID gEfiSmmChipsetLibServicesProtocolGuid;

#endif
