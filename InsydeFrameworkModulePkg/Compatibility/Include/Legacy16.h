//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
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

  Legacy16.h

Abstract:

  API between 16-bit Legacy BIOS and EFI

  We need to figure out what the 16-bit code is going to use to
  represent these data structures. Is a pointer SEG:OFF or 32-bit...

--*/

#ifndef LEGACY_16_H_
#define LEGACY_16_H_

#include "Tiano.h"
#include EFI_PROTOCOL_PRODUCER (LegacyBios)

#define EFI_TO_LEGACY_MAJOR_VERSION 0x02
#define EFI_TO_LEGACY_MINOR_VERSION 0x00
//[-start-111005-IB02700344-add]// 
#define AHCI_ENABLE                 0x8000
//[-end-111005-IB02700344-add]// 
#pragma pack(1)
//
// EFI Legacy to Legacy16 data
// EFI_COMPATIBILITY16_TABLE has been moved to LegacyBios protocol defn file.
//
typedef struct {
  //
  // Memory map used to start up Legacy16 code
  //
  UINT32  BiosLessThan1MB;
  UINT32  HiPmmMemory;
  UINT32  HiPmmMemorySizeInBytes;

  UINT16  ReverseThunkCallSegment;
  UINT16  ReverseThunkCallOffset;
  UINT32  NumberE820Entries;
  UINT32  OsMemoryAbove1Mb;
  UINT32  ThunkStart;
  UINT32  ThunkSizeInBytes;
  UINT32  LowPmmMemory;
  UINT32  LowPmmMemorySizeInBytes;
} EFI_TO_COMPATIBILITY16_INIT_TABLE;

#pragma pack()
//
// Legacy16 Call types
//
typedef enum {
  Legacy16InitializeYourself    = 0x0000,
  Legacy16UpdateBbs             = 0x0001,
  Legacy16PrepareToBoot         = 0x0002,
  Legacy16Boot                  = 0x0003,
  Legacy16RetrieveLastBootDevice= 0x0004,
  Legacy16DispatchOprom         = 0x0005,
  Legacy16GetTableAddress       = 0x0006,
  Legacy16SetKeyboardLeds       = 0x0007,
  Legacy16InstallPciHandler     = 0x0008,
} EFI_COMPATIBILITY16_FUNCTIONS;
#define F0000Region 0x01
#define E0000Region 0x02
//
// Legacy16 call prototypes
//  Input:  AX = EFI_COMPATIBILITY16_FUNCTIONS for all functions.
//  Output: AX = Return status for all functions. It follows EFI error
//               codes.
//
//  Legacy16InitializeYourself
//    Description: This is the first call to 16-bit code. It allows the
//                 16-bit to perform any internal initialization.
//    Input:  ES:BX pointer to EFI_TO_COMPATIBILITY16_INIT_TABLE
//    Output:
//  Legacy16UpdateBbs
//    Description: The 16-bit code updates the BBS table for non-compliant
//                 devices.
//    Input:  ES:BX pointer to EFI_TO_COMPATIBILITY16_BOOT_TABLE
//    Output:
//  Legacy16PrepareToBoot
//    Description: This is the last call to 16-bit code where 0xE0000 -0xFFFFF
//                 is read/write. 16-bit code does any final clean up.
//    Input:  ES:BX pointer to EFI_TO_COMPATIBILITY16_BOOT_TABLE
//    Output:
//  Legacy16Boot
//    Description: Do INT19.
//    Input:
//    Output:
//  Legacy16RetrieveLastBootDevice
//    Description: Return the priority number of the device that booted.
//    Input:
//    Output: BX = priority number of the last attempted boot device.
//  Legacy16DispatchOprom
//    Description: Pass control to the specified OPROM. Allows the 16-bit
//                 code to rehook INT 13,18 and/or 19 from non-BBS
//                 compliant devices.
//    Input:  ES:DI = Segment:Offset of PnPInstallationCheck
//            SI = OPROM segment. Offset assumed to be 3.
//            BH = PCI bus number.
//            BL = PCI device * 8 | PCI function.
//    Output: BX = Number of BBS non-compliant drives detected. Return
//                 zero for BBS compliant devices.
//  Legacy16GetTableAddress
//    Description: Allocate an area in the 0xE0000-0xFFFFF region.
//    Input:  BX = Allocation region.
//                 0x0 = Any region
//                 Bit 0 = 0xF0000 region
//                 Bit 1 = 0xE0000 region
//                 Multiple bits can be set.
//            CX = Length in bytes requested
//            DX = Required address alignment
//                 Bit mapped. First non-zero bit from right to left is
//                 alignment.
//    Output: DS:BX is assigned region.
//            AX = EFI_OUT_OF_RESOURCES if request cannot be granted.
//  Legacy16SetKeyboardLeds
//    Description: Perform any special action when keyboard LEDS change.
//                 Other code performs the LED change and updates standard
//                 BDA locations. This is for non-standard operations.
//    Input:  CL = LED status. 1 = set.
//                 Bit 0 = Scroll lock
//                 Bit 1 = Num lock
//                 Bit 2 = Caps lock
//    Output:
//  Legacy16InstallPciHandler
//    Description: Provides 16-bit code a hook to establish an interrupt
//                 handler for any PCI device requiring a PCI interrupt
//                 but having no OPROM. This is called before interrupt
//                 is assigned. 8259 will be disabled(even if sharded)
//                 and PCI Interrupt Line unprogrammed. Other code will
//                 program 8259 and PCI Interrupt Line.
//    Input:  ES:BX Pointer to EFI_LEGACY_INSTALL_PCI_HANDLER strcture
//    Output:
//
typedef UINT8 SERIAL_MODE;
typedef UINT8 PARALLEL_MODE;

#pragma pack(1)

#define DEVICE_SERIAL_MODE_NORMAL               0x00
#define DEVICE_SERIAL_MODE_IRDA                 0x01
#define DEVICE_SERIAL_MODE_ASK_IR               0x02
#define DEVICE_SERIAL_MODE_DUPLEX_HALF          0x00
#define DEVICE_SERIAL_MODE_DUPLEX_FULL          0x10

#define DEVICE_PARALLEL_MODE_MODE_OUTPUT_ONLY   0x00
#define DEVICE_PARALLEL_MODE_MODE_BIDIRECTIONAL 0x01
#define DEVICE_PARALLEL_MODE_MODE_EPP           0x02
#define DEVICE_PARALLEL_MODE_MODE_ECP           0x03

typedef struct {
  UINT16      Address;
  UINT8       Irq;
  SERIAL_MODE Mode;
} DEVICE_PRODUCER_SERIAL;

typedef struct {
  UINT16        Address;
  UINT8         Irq;
  UINT8         Dma;
  PARALLEL_MODE Mode;
} DEVICE_PRODUCER_PARALLEL;

typedef struct {
  UINT16  Address;
  UINT8   Irq;
  UINT8   Dma;
  UINT8   NumberOfFloppy;
} DEVICE_PRODUCER_FLOPPY;

typedef struct {
  UINT32  A20Kybd : 1;
  UINT32  A20Port90 : 1;
  UINT32  Reserved : 30;
} LEGACY_DEVICE_FLAGS;

typedef struct {
  DEVICE_PRODUCER_SERIAL    Serial[4];
  DEVICE_PRODUCER_PARALLEL  Parallel[3];
  DEVICE_PRODUCER_FLOPPY    Floppy;
  UINT8                     MousePresent;
  LEGACY_DEVICE_FLAGS       Flags;
} DEVICE_PRODUCER_DATA_HEADER;
//
// SMM Table definitions
// SMM table has a header that provides the number of entries. Following
// the header is a variable length amount of data.
//
#define PORT_SIZE_8   0x00
#define PORT_SIZE_16  0x01
#define PORT_SIZE_32  0x02
#define PORT_SIZE_64  0x03
#define DATA_SIZE_8   0x00
#define DATA_SIZE_16  0x01
#define DATA_SIZE_32  0x02
#define DATA_SIZE_64  0x03

typedef struct {
  UINT16  Type : 3;
  UINT16  PortGranularity : 3;
  UINT16  DataGranularity : 3;
  UINT16  Reserved : 7;
} SMM_ATTRIBUTES;

#define INT15_D042         0x0000
#define GET_USB_BOOT_INFO  0x0001
#define DMI_PNP_50_57      0x0002
#define TCG_MOR_SMM_FUN    0x0004
#define ATA_LEGACY_SMM_FUN 0x0008
#define SD_LEGACY_SMM_FUN  0x0010
#define STANDARD_OWNER     0x0
#define OEM_OWNER          0x1

typedef struct {
  UINT16  Function : 15;
  UINT16  Owner : 1;
} SMM_FUNCTION;

//
// If MAX_IDE_CONTROLLER changes value 16-bit legacy code needs to change
//
#define MAX_IDE_CONTROLLER  8

typedef struct {
  UINT16                      MajorVersion;
  UINT16                      MinorVersion;

  UINT32                      AcpiTable;   // 4 GB range
  UINT32                      SmbiosTable; // 4 GB range
  UINT32                      SmbiosTableLength;

  //
  // Legacy SIO state
  //
  DEVICE_PRODUCER_DATA_HEADER SioData;

  UINT16                      DevicePathType;
  UINT16                      PciIrqMask;
  UINT32                      NumberE820Entries;
  //
  // Controller & Drive Identify[2] per controller information
  //
  HDD_INFO                    HddInfo[MAX_IDE_CONTROLLER];
  UINT32                      NumberBbsEntries;
  UINT32                      BbsTable;
  UINT32                      SmmTable;
  UINT32                      OsMemoryAbove1Mb;
  UINT32                      UnconventionalDeviceTable;
} EFI_TO_COMPATIBILITY16_BOOT_TABLE;

#define LEGACY_AHCI_OPROM   1
#define SEAMLESS_AHCI       2
#define SEAMLESS_IDE        4
//
// H2O table
//
typedef struct {                                         //   CSM16 H2O table
  UINT32                      H2OSingnature;             //   '$H2O'                                                                       
  UINT16                      SwSmiPort;                 //   s/w smi port to be filled by CSM32                                                                           
  UINT16                      UsbBufferAddr;             //   buf segment @ to be filled by UsbLegacy     
  UINT16                      O1394BufferAddr;           //   buf segment @ to be filled by legacy1394                                          
  UINT16                      BootTableOffset;           //   EfiToLegacy16BootTable addr
  UINT16                      BootTableSegment;          //   EfiToLegacy16BootTable addr
  UINT8                       UsbUhciCtrl;               //   Usb Uhci Ctrl Number from csm32                                       
  UINT8                       UsbEhciCtrl;               //   Usb Ehci Ctrl Number from csm32
  UINT16                      CSM16OemSwitch;            //   Switch CSM16 function by OEM service
  UINT16                      SataINT13;                 //   (0:Normal<INT13 support by CSM16>, 1:Legacy Ahci OpRom, 2.Seamless Ahci/RAID(ODD only), 4.Seamless IDE)
  UINT16                      DebugPort;                 //   Debug Port number      
  UINT8                       PCI30Support;              //   PCI 3.0 support for EFI and CSM16
  UINT8                       E820SupportExtAttributes;  //   Support E820 new structure (bit 0:CSM16 support , bit 1:EFI support)
  UINT16                      Reserve2[4];               //   Reserve        
} H2O_TABLE;


//[-start-110915-IB02700340-add]//  
//  40:00-01 Com1
//  40:02-03 Com2
//  40:04-05 Com3
//  40:06-07 Com4
//  40:08-09 Lpt1
//  40:0A-0B Lpt2
//  40:0C-0D Lpt3
//  40:0E-0E Ebda segment
//  40:10-11 MachineConfig
//  40:12    Bda12 - skip
//  40:13-14 MemSize below 1MB
//  40:15-16 Bda15_16 - skip
//  40:17    Keyboard Shift status
//  40:18-19 Bda18_19 - skip
//  40:1A-1B Key buffer head
//  40:1C-1D Key buffer tail
//  40:1E-3D Bda1E_3D- key buffer -skip
//  40:3E-3F FloppyData 3E = Calibration status 3F = Motor status
//  40:40    FloppyTimeout
//  40:41-74 Bda41_74 - skip
//  40:75    Number of HDD drives
//  40:76-77 Bda76_77 - skip
//  40:78-79 78 = Lpt1 timeout, 79 = Lpt2 timeout
//  40:7A-7B 7A = Lpt3 timeout, 7B = Lpt4 timeout
//  40:7C-7D 7C = Com1 timeout, 7D = Com2 timeout
//  40:7E-7F 7E = Com3 timeout, 7F = Com4 timeout
//  40:80-81 Pointer to start of key buffer
//  40:82-83 Pointer to end of key buffer
//  40:84-87 Bda84_87 - skip
//  40:88    HDD Data Xmit rate
//  40:89-8f skip
//  40:90    Floppy data rate
//  40:91-95 skip
//  40:96    Keyboard Status
//  40:97    LED Status
//  40:98-101 skip

typedef struct {
  UINT16  Com1;
  UINT16  Com2;
  UINT16  Com3;
  UINT16  Com4;
  UINT16  Lpt1;
  UINT16  Lpt2;
  UINT16  Lpt3;
  UINT16  Ebda;
  UINT16  MachineConfig;
  UINT8   Bda12;
  UINT16  MemSize;
  UINT8   Bda15_16[0x02];
  UINT8   ShiftStatus;
  UINT8   Bda18_19[0x02];
  UINT16  KeyHead;
  UINT16  KeyTail;
  UINT16  Bda1E_3D[0x10];
  UINT16  FloppyData;
  UINT8   FloppyTimeout;
  UINT8   Bda41_74[0x34];
  UINT8   NumberOfDrives;
  UINT8   Bda76_77[0x02];
  UINT16  Lpt1_2Timeout;
  UINT16  Lpt3_4Timeout;
  UINT16  Com1_2Timeout;
  UINT16  Com3_4Timeout;
  UINT16  KeyStart;
  UINT16  KeyEnd;
  UINT8   Bda84_87[0x4];
  UINT8   DataXmit;
  UINT8   Bda89_8F[0x07];
  UINT8   FloppyXRate;
  UINT8   Bda91_95[0x05];
  UINT8   KeyboardStatus;
  UINT8   LedStatus;
} BDA_STRUC;
//[-end-110915-IB02700340-add]//  
#pragma pack()

#endif
