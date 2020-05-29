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

  LegacyBios.h

Abstract:
  This code supports a private implementation
  of the Legacy BIOS protocol.

--*/

#ifndef LEGACY_BIOS_H_
#define LEGACY_BIOS_H_

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "Pci22.h"
#include EFI_PROTOCOL_PRODUCER (LegacyBios)
#include "Legacy16.h"
#include "LegacyBiosMpTable.h"
#include "EfiHobLib.h"
#include "IsaIoDefinitions.h"
#include "SetupConfig.h"

#include EFI_PROTOCOL_CONSUMER (LegacyRegion)
#include EFI_PROTOCOL_CONSUMER (LegacyBiosPlatform)
#include EFI_PROTOCOL_CONSUMER (Legacy8259)
#include EFI_PROTOCOL_CONSUMER (LegacyInterrupt)
#ifndef  EFI_NO_MEMORY_TEST
#include EFI_PROTOCOL_CONSUMER (GenericMemoryTest)
#endif
#include EFI_PROTOCOL_CONSUMER (ConsoleControl)

#include EFI_PROTOCOL_PRODUCER (LegacyBios)

#include EFI_ARCH_PROTOCOL_DEFINITION (Cpu)
#include EFI_PROTOCOL_DEFINITION (BlockIo)
#include EFI_PROTOCOL_DEFINITION (SerialIo)
#include EFI_PROTOCOL_DEFINITION (SimplePointer)
#include EFI_PROTOCOL_DEFINITION (DiskInfo)
#include EFI_PROTOCOL_DEFINITION (FirmwareVolume)
#include EFI_PROTOCOL_DEFINITION (LoadedImage)
#include EFI_PROTOCOL_DEFINITION (PciIo)
#include EFI_PROTOCOL_DEFINITION (EfiSetupUtility)

#include EFI_GUID_DEFINITION (Acpi)
#include EFI_GUID_DEFINITION (LegacyBios)
#include EFI_GUID_DEFINITION (SmBios)
#include EFI_GUID_DEFINITION (Hob)

#ifndef EFI_NO_MEMORY_TEST
extern EFI_GENERIC_MEMORY_TEST_PROTOCOL *gGenMemoryTest;
#define LocateMemoryTestIfExist(Protocol,                   \
                                Registration,               \
                                Interface)                  \
        gBS->LocateProtocol ((Protocol),                    \
                             (Registration),                \
                             (Interface))                   \

#define CompatibleMemoryRangeTestIfExist(This,              \
                                         StartAddress,      \
                                         Length)            \
        (This)->CompatibleRangeTest ((This),                \
                                      (StartAddress),       \
                                      (Length))
#else
#define LocateMemoryTestIfExist(Protocol,                   \
                                Registration,               \
                                Interface)                  \
        EFI_SUCCESS
#define CompatibleMemoryRangeTestIfExist(This,              \
                                  StartAddress,             \
                                  Length)                   \
        EFI_SUCCESS
#endif

//
// Thunk Status Codes
//   (These apply only to errors with the thunk and not to the code that was
//   thunked to.)
//
#define THUNK_OK              0x00
#define THUNK_ERR_A20_UNSUP   0x01
#define THUNK_ERR_A20_FAILED  0x02

//
//Before Csm16InitYourself, Assign SW_SMI_Port to Csm16.
//Define the fixed Address to trnasfer SW_SMI_PORT address.
//
#define SW_SMI_PORT_ADDR   0xFE8E4

//
// Trace defines
//
//
#define LegacyBdaTrace    0x000
#define LegacyBiosTrace   0x040
#define LegacyBootTrace   0x080
#define LegacyCmosTrace   0x0C0
#define LegacyIdeTrace    0x100
#define LegacyMpTrace     0x140
#define LegacyPciTrace    0x180
#define LegacySioTrace    0x1C0

#define LegacyPciTrace000 LegacyPciTrace + 0x00
#define LegacyPciTrace001 LegacyPciTrace + 0x01
#define LegacyPciTrace002 LegacyPciTrace + 0x02
#define LegacyPciTrace003 LegacyPciTrace + 0x03
#define LegacyPciTrace004 LegacyPciTrace + 0x04
#define LegacyPciTrace005 LegacyPciTrace + 0x05
#define LegacyPciTrace006 LegacyPciTrace + 0x06
#define LegacyPciTrace007 LegacyPciTrace + 0x07
#define LegacyPciTrace008 LegacyPciTrace + 0x08
#define LegacyPciTrace009 LegacyPciTrace + 0x09
#define LegacyPciTrace00A LegacyPciTrace + 0x0A
#define LegacyPciTrace00B LegacyPciTrace + 0x0B
#define LegacyPciTrace00C LegacyPciTrace + 0x0C
#define LegacyPciTrace00D LegacyPciTrace + 0x0D
#define LegacyPciTrace00E LegacyPciTrace + 0x0E
#define LegacyPciTrace00F LegacyPciTrace + 0x0F
#define MAX_ROMS          0x20
#define BCV               0
#define BEV               1
#define PCI_UNUSED        0x00
#define PCI_USED          0xFF
#define LEGACY_USED       0xFE

#define NO_ROM            0x00
#define ROM_FOUND         0x01
#define VALID_LEGACY_ROM  0x02
#define ROM_WITH_CONFIG   0x04

#define CSM16_SUPPORT_PCI3_0 0x01
#define EFI_SUPPORT_PCI3_0   0x02

#define APIC_REGISTER_APIC_VERSION_OFFSET      0x30
#define CPUID_DETERMINISTIC_CACHE_PARAMETRTS   0x4
#define MAX_NUM_OF_CORE_PER_PKG_OFFSET         26
#define CPUID_PROCESSOR_TOPOLOGY               0xB
#define MADT_LEGACY_MODE                       0x01
#define MADT_MODERN_MODE                       0x00
#define PROCESSOR_HT_DISABLE                   0x01
#define PROCESSOR_HT_ENABLE                    0x00
#define ADDRESS_SPACE_DESCRIPTOR_HEAD_TAG               0x8A
#define ADDRESS_SPACE_DESCRIPTOR_END_TAG                0x79
#define ADDRESS_SPACE_DESCRIPTOR_RESOURCE_TYPE_OFFSET   0x03
#define ADDRESS_SPACE_DESCRIPTOR_ADDR_RANGE_MIN_OFFSET  0x0E
#define ADDRESS_SPACE_DESCRIPTOR_ADDR_LENGTH_OFFSET     0x26
#define ADDRESS_SPACE_DESCRIPTOR_TOTAL_LENGTH           0x2E

typedef struct {
  UINTN   mPciSegment;
  UINTN   mPciBus;
  UINTN   mPciDevice;
  UINTN   mPciFunction;
  UINT32  mShadowAddress;
  UINT32  mShadowedSize;
  UINT8   mDiskStart;
  UINT8   mDiskEnd;
} ROM_INSTANCE_ENTRY;

typedef struct {
  UINT8 Irq;
  UINT8 Used;
} EFI_LEGACY_IRQ_PRIORITY_TABLE_ENTRY;

//
// Interesting CR0 flags
//
#define CR0_PE  0x00000001

//
// Values for RealModeGdt
//
#define NUM_REAL_GDT_ENTRIES  8

#define NULL_SELECTOR_ARRAY_INDEX             0
#define REAL_CODE_SELECTOR_ARRAY_INDEX        1
#define REAL_DATA_SELECTOR_ARRAY_INDEX        2
#define PROTECTED_CODE_SELECTOR_ARRAY_INDEX   3
#define PROTECTED_DATA_SELECTOR_ARRAY_INDEX   4
#define SPARE_SELECTOR_ARRAY_INDEX            5
#define X64_DATA_SELECTOR_ARRAY_INDEX         6
#define X64_CODE_SELECTOR_ARRAY_INDEX         7

//
// Miscellaneous numbers
//
#define PMM_MEMORY_SIZE         0x400000  // 4 MB
#define CONVENTIONAL_MEMORY_TOP 0xA0000   // 640 KB
//
// Processor structres are packed
//
#pragma pack(push, 1)

//
// Define what a processor GDT looks like
//

typedef struct {
  UINT16  LimitLow;
  UINT16  BaseLow;
  UINT8   BaseMid;
  UINT8   Attribute;
  UINT8   LimitHi;
  UINT8   BaseHi;
} GDT_ENTRY;

//
// Define what a processor descriptor looks like
// This data structure must be kept in sync with ASM STRUCT in Thunk.inc
//
typedef struct {
  UINT16  Limit;
  UINT64  Base;
} DESCRIPTOR64;

typedef struct {
  UINT16  Limit;
  UINT32  Base;
} DESCRIPTOR32;

//
// Low stub lay out
//
#define LOW_STACK_SIZE      (8 * 1024)  // 8k?
#define EFI_MAX_E820_ENTRY  100
#define FIRST_INSTANCE      1
#define NOT_FIRST_INSTANCE  0

//
// PnP Expansion Header
//
typedef struct {
  UINT32  PnpSignature;
  UINT8   Revision;
  UINT8   Length;
  UINT16  NextHeader;
  UINT8   Reserved1;
  UINT8   Checksum;
  UINT32  DeviceId;
  UINT16  MfgPointer;
  UINT16  ProductNamePointer;
  UINT8   Class;
  UINT8   SubClass;
  UINT8   Interface;
  UINT8   DeviceIndicators;
  UINT16  Bcv;
  UINT16  DisconnectVector;
  UINT16  Bev;
  UINT16  Reserved2;
  UINT16  StaticResourceVector;
} LEGACY_PNP_EXPANSION_HEADER;

typedef struct {
  UINT8   PciSegment;
  UINT8   PciBus;
  UINT8   PciDevice;
  UINT8   PciFunction;
  UINT16  Vid;
  UINT16  Did;
  UINT16  SysSid;
  UINT16  SVid;
  UINT8   Class;
  UINT8   SubClass;
  UINT8   Interface;
  UINT8   Reserved;
  UINTN   RomStart;
  UINTN   ManufacturerString;
  UINTN   ProductNameString;
} LEGACY_ROM_AND_BBS_TABLE;

//
// Structure how EFI has mapped a devices HDD drive numbers.
// Boot to EFI aware OS or shell requires this mapping when
// 16-bit CSM assigns drive numbers.
// This mapping is ignored booting to a legacy OS.
//
typedef struct {
  UINT8 PciSegment;
  UINT8 PciBus;
  UINT8 PciDevice;
  UINT8 PciFunction;
  UINT8 StartDriveNumber;
  UINT8 EndDriveNumber;
} LEGACY_EFI_HDD_TABLE;

//
// This data is passed to Leacy16Boot
//
typedef enum {
  EfiAcpiAddressRangeMemory   = 1,
  EfiAcpiAddressRangeReserved = 2,
  EfiAcpiAddressRangeACPI     = 3,
  EfiAcpiAddressRangeNVS      = 4,
  EfiAddressRangeUnusuable    = 5
} EFI_ACPI_MEMORY_TYPE;

//
//  Define Ext attributes 
// 
#define AddressRangeEnabled      (1 << 0)
#define AddressRangeNonVolatile  (1 << 1)
#define AddressRangeSlowAccess   (1 << 2)
#define AddressRangeErrorLog     (1 << 3)

#define CSM16_SUPPORT_E820_EXT_ATTRIBUTES (1 << 0)
#define EFI_SUPPORT_E820_EXT_ATTRIBUTES   (1 << 1)

typedef struct {
  UINT64                BaseAddr;
  UINT64                Length;
  EFI_ACPI_MEMORY_TYPE  Type;
  UINT32                ExtAttributes;
} EFI_E820_ENTRY64;

typedef struct {
  UINT32                BassAddrLow;
  UINT32                BaseAddrHigh;
  UINT32                LengthLow;
  UINT32                LengthHigh;
  EFI_ACPI_MEMORY_TYPE  Type;
  UINT32                ExtAttributes;
} EFI_E820_ENTRY;

typedef struct {
  UINT64                BaseAddr;
  UINT64                Length;
  EFI_ACPI_MEMORY_TYPE  Type;
} EFI_OLD_E820_ENTRY64;

typedef struct {
  UINT32                BassAddrLow;
  UINT32                BaseAddrHigh;
  UINT32                LengthLow;
  UINT32                LengthHigh;
  EFI_ACPI_MEMORY_TYPE  Type;
} EFI_OLD_E820_ENTRY;

typedef struct {
  UINT8   PciBus;
  UINT8   PciDeviceFun;
  UINT8   PciSegment;
  UINT8   PciClass;
  UINT8   PciSubclass;
  UINT8   PciInterface;
  UINT8   PrimaryIrq;
  UINT8   PrimaryReserved;
  UINT16  PrimaryControl;
  UINT16  PrimaryBase;
  UINT16  PrimaryBusMaster;
  UINT8   SecondaryIrq;
  UINT8   SecondaryReserved;
  UINT16  SecondaryControl;
  UINT16  SecondaryBase;
  UINT16  SecondaryBusMaster;

} EFI_LEGACY_INSTALL_PCI_HANDLER;

typedef struct {
  UINT16  PnPInstallationCheckSegment;
  UINT16  PnPInstallationCheckOffset;
  UINT16  OpromSegment;
  UINT8   PciBus;
  UINT8   PciDeviceFunction;
  UINT8   NumberBbsEntries;
  UINT32  BbsTablePointer;                  
  UINT16  RuntimeSegment;             //OpromDestinationSegment
} DISPATCH_OPROM_TABLE;

typedef struct {
  //
  // Space for the code
  //  The address of Code is also the beginning of the relocated Thunk code
  //
  CHAR8                             CodeBuffer[4096]; // ?
  //
  // The address of the Reverse Thunk code
  //  Note that this member CONTAINS the address of the relocated reverse thunk
  //  code unlike the member variable 'Code', which IS the address of the Thunk
  //  code.
  //  Since Nobody use these field, we currently did not implement it
  //UINT64                            LowReverseThunkStart;

  //
  // Data for the code (cs releative)
  //
  DESCRIPTOR64                      x64GdtDesc;          // Protected mode GDT
  DESCRIPTOR64                      x64IdtDesc;          // Protected mode IDT
  UINTN                             x64Ss;
  UINTN                             x64Esp;

  UINTN                             RealStack;
  DESCRIPTOR32                      RealModeIdtDesc;
  DESCRIPTOR32                      RealModeGdtDesc;

  //
  // real-mode GDT (temporary GDT with two real mode segment descriptors)
  //
  GDT_ENTRY                         RealModeGdt[NUM_REAL_GDT_ENTRIES];
  UINT64                            PageMapLevel4;
  //
  // Members specifically for the reverse thunk
  //   Since Nobody use these field, we currently did not implement it
  //
  //UINT16                            RevRealDs;
  //UINT16                            RevRealSs;
  //UINT32                            RevRealEsp;
  //DESCRIPTOR                        RevRealIdtDesc;
  //UINT16                            RevFlatDataSelector;  // Flat data selector in GDT
  //UINT32                            RevFlatStack;

  //
  // A low memory stack
  //
  CHAR8                             Stack[LOW_STACK_SIZE];

  //
  // Stack for flat mode after reverse thunk
  //  BugBug - This may no longer be necessary if the reverse thunk interface
  //           is changed to have the flat stack in a different location.
  //  Since Nobody use these field, we currently did not implement it
  //CHAR8                             RevThunkStack[LOW_STACK_SIZE];

  //
  // Legacy16 Init memory map info
  //
  EFI_TO_COMPATIBILITY16_INIT_TABLE EfiToLegacy16InitTable;

  EFI_TO_COMPATIBILITY16_BOOT_TABLE EfiToLegacy16BootTable;

  CHAR8                             InterruptRedirectionCode[32];
  CHAR8                             DummyInterruptCode[1];
  EFI_LEGACY_INSTALL_PCI_HANDLER    PciHandler;
  DISPATCH_OPROM_TABLE              DispatchOpromTable;
  BBS_TABLE                         BbsTable[MAX_BBS_ENTRIES];
} LOW_MEMORY_THUNK;

#define LEGACY_BIOS_INSTANCE_SIGNATURE  EFI_SIGNATURE_32 ('B', 'I', 'O', 'S')
typedef struct {
  UINT32                            Signature;

  EFI_HANDLE                        Handle;
  EFI_LEGACY_BIOS_PROTOCOL          LegacyBios;

  EFI_HANDLE                        ImageHandle;

  //
  // Protocol to Lock and Unlock 0xc0000 - 0xfffff
  //
  EFI_LEGACY_REGION_PROTOCOL        *LegacyRegion;

  EFI_LEGACY_BIOS_PLATFORM_PROTOCOL *LegacyBiosPlatform;

  //
  // Interrupt control for thunk and PCI IRQ
  //
  EFI_LEGACY_8259_PROTOCOL          *Legacy8259;

  //
  // PCI Interrupt PIRQ control
  //
  EFI_LEGACY_INTERRUPT_PROTOCOL     *LegacyInterrupt;

  //
  // If size reduction is not applied, do memory test
  //
  #ifndef EFI_NO_MEMORY_TEST
  EFI_GENERIC_MEMORY_TEST_PROTOCOL  *GenericMemoryTest;
  #endif

  //
  // TRUE if PCI Interupt Line registers have been programmed.
  //
  BOOLEAN                           PciInterruptLine;

  //
  // Code space below 1MB needed by thunker to transition to real mode.
  // Contains stack and real mode code fragments
  //
  LOW_MEMORY_THUNK                  *IntThunk;

  //
  // Starting shadow address of the Legacy BIOS
  //
  UINT32                            BiosStart;
  UINT32                            LegacyBiosImageSize;

  //
  // Start of variables used by CsmItp.mac ITP macro file and/os LegacyBios
  //
  UINT8                             Dump[4];

  //
  // $EFI Legacy16 code entry info in memory < 1 MB;
  //
  EFI_COMPATIBILITY16_TABLE         *Legacy16Table;
  VOID                              *Legacy16InitPtr;
  VOID                              *Legacy16BootPtr;
  VOID                              *InternalIrqRoutingTable;
  UINT32                            NumberIrqRoutingEntries;
  VOID                              *BbsTablePtr;
  UINT32                            NumberBbsEntries;
  VOID                              *HddTablePtr;
  UINT32                            NumberHddControllers;

  //
  // Cached copy of Legacy16 entry point
  //
  UINT16                            Legacy16CallSegment;
  UINT16                            Legacy16CallOffset;

  //
  // Returned from $EFI and passed in to OPROMS
  //
  UINT16                            PnPInstallationCheckSegment;
  UINT16                            PnPInstallationCheckOffset;

  //
  // E820 table
  //
  EFI_E820_ENTRY                    E820Table[EFI_MAX_E820_ENTRY];
  UINT32                            NumberE820Entries;

  //
  // True if legacy VGA INT 10h handler installed
  //
  BOOLEAN                           VgaInstalled;

  //
  // Number of IDE drives
  //
  UINT8                             IdeDriveCount;

  //
  // Current Free Option ROM space. An option ROM must NOT go past
  // BiosStart.
  //
  UINT32                            OptionRom;

  //
  // Save Legacy16 unexpected interrupt vector. Reprogram INT 68-6F from
  // EFI values to legacy value just before boot.
  //
  UINT32                            BiosUnexpectedInt;
  UINT32                            ThunkSavedInt[8];
  UINT16                            ThunkSeg;
  LEGACY_EFI_HDD_TABLE              *LegacyEfiHddTable;
  UINT16                            LegacyEfiHddTableIndex;
  UINT8                             DiskEnd;
  UINT8                             Disk4075;
  UINT16                            TraceIndex;
  UINT16                            Trace[0x200];

  //
  // Indicate that whether GenericLegacyBoot is entered or not
  //
  BOOLEAN                           LegacyBootEntered;

  //
  // CSM16 PCI Interface Version
  //
  UINT16                            Csm16PciInterfaceVersion;
//[-start-120224-IB02700354-remove]//
//  //
//  // LegacyRegion Bitmap (E0000h~FFFFFh)
//  //
//  VOID                              *LegacyRegionBitmap;
//[-end-120224-IB02700354-remove]//
} LEGACY_BIOS_INSTANCE;

#pragma pack(pop)

#define LEGACY_BIOS_INSTANCE_FROM_THIS(this)  CR (this, LEGACY_BIOS_INSTANCE, LegacyBios, LEGACY_BIOS_INSTANCE_SIGNATURE)

BOOLEAN
EFIAPI
LegacyBiosInt86 (
  IN  EFI_LEGACY_BIOS_PROTOCOL          *This,
  IN  UINT8                             BiosInt,
  IN  EFI_IA32_REGISTER_SET             *Regs
  )
/*++

  Routine Description:
    Thunk to 16-bit real mode and execute a software interrupt with a vector
    of BiosInt. Regs will contain the 16-bit register context on entry and
    exit.

  Arguments:
    This    - Protocol instance pointer.
    BiosInt - Processor interrupt vector to invoke
    Reg     - Register contexted passed into (and returned) from thunk to
              16-bit mode

  Returns:
    FALSE   - Thunk completed, and there were no BIOS errors in the target code.
              See Regs for status.
    TRUE    - There was a BIOS erro in the target code.

--*/
;

BOOLEAN
EFIAPI
LegacyBiosFarCall86 (
  IN  EFI_LEGACY_BIOS_PROTOCOL          *This,
  IN  UINT16                            Segment,
  IN  UINT16                            Offset,
  IN  EFI_IA32_REGISTER_SET             *Regs,
  IN  VOID                              *Stack,
  IN  UINTN                             StackSize
  )
/*++

  Routine Description:
    Thunk to 16-bit real mode and call Segment:Offset. Regs will contain the
    16-bit register context on entry and exit. Arguments can be passed on
    the Stack argument

  Arguments:
    This      - Protocol instance pointer.
    Segment   - Segemnt of 16-bit mode call
    Offset    - Offset of 16-bit mdoe call
    Reg       - Register contexted passed into (and returned) from thunk to
                16-bit mode
    Stack     - Caller allocated stack used to pass arguments
    StackSize - Size of Stack in bytes

  Returns:
    FALSE     - Thunk completed, and there were no BIOS errors in the target code.
                See Regs for status.
    TRUE      - There was a BIOS erro in the target code.

--*/
;

EFI_STATUS
EFIAPI
LegacyBiosCheckPciRom (
  IN  EFI_LEGACY_BIOS_PROTOCOL          *This,
  IN  EFI_HANDLE                        PciHandle,
  OUT VOID                              **RomImage, OPTIONAL
  OUT UINTN                             *RomSize, OPTIONAL
  OUT UINTN                             *Flags
  )
/*++

  Routine Description:
    Test to see if a legacy PCI ROM exists for this device. Optionally return
    the Legacy ROM instance for this PCI device.

  Arguments:
    This      - Protocol instance pointer.
    PciHandle - The PCI PC-AT OPROM from this devices ROM BAR will be loaded
    RomImage  - Return the legacy PCI ROM for this device
    RomSize   - Size of ROM Image
    Flags     - Indicates if ROM found and if PC-AT.

  Returns:
    EFI_SUCCESS  - Legacy Option ROM availible for this device
    EFI_UNSUPPORTED  - Legacy Option ROM not supported.

--*/
;

EFI_STATUS
EFIAPI
LegacyBiosPrepareToBootEfi (
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  OUT UINT16                          *BbsCount,
  OUT IN BBS_TABLE                    **BbsTable
  )
/*++

  Routine Description:
    Assign drive number to legacy HDD drives prior to booting an EFI
    aware OS so the OS can access drives without an EFI driver.
    Note: BBS compliant drives ARE NOT available until this call by
          either shell or EFI.

  Arguments:
    This       - Protocol instance pointer.
    BbsCount   - Number of BBS_TABLE structures
    BbsTable   - List BBS entries

  Returns:
    EFI_SUCCESS  - Drive numbers assigned

--*/
;

EFI_STATUS
EFIAPI
LegacyBiosBootUnconventionalDevice (
  IN EFI_LEGACY_BIOS_PROTOCOL         *This,
  IN UDC_ATTRIBUTES                   Attributes,
  IN UINTN                            BbsEntry,
  IN VOID                             *BeerData,
  IN VOID                             *ServiceAreaData
  )
/*++

  Routine Description:
    To boot from an unconventional device like parties and/or execute
    HDD diagnostics.

  Arguments:
    This            - Protocol instance pointer.
    Attributes      - How to interpret the other input parameters
    BbsEntry        - The 0-based index into the BbsTable for the parent
                      device.
    BeerData        - Pointer to the 128 bytes of ram BEER data.
    ServiceAreaData - Pointer to the 64 bytes of raw Service Area data. The
                      caller must provide a pointer to the specific Service
                      Area and not the start all Service Areas.
  Returns:
    EFI_INVALID_PARAMETER if error. Does NOT return if no error.

--*/
;

EFI_STATUS
EFIAPI
LegacyBiosInstallPciRom (
  IN  EFI_LEGACY_BIOS_PROTOCOL          * This,
  IN  EFI_HANDLE                        PciHandle,
  IN  VOID                              **RomImage,
  OUT UINTN                             *Flags,
  OUT UINT8                             *DiskStart, OPTIONAL
  OUT UINT8                             *DiskEnd, OPTIONAL
  OUT VOID                              *RomShadowAddress, OPTIONAL
  OUT UINT32                            *ShodowedRomSize OPTIONAL
  )

/*++

  Routine Description:
    Load a legacy PC-AT OPROM on the PciHandle device. Return information
    about how many disks were added by the OPROM and the shadow address and
    size. DiskStart & DiskEnd are INT 13h drive letters. Thus 0x80 is C:

  Arguments:
    This      - Protocol instance pointer.
    PciHandle - The PCI PC-AT OPROM from this devices ROM BAR will be loaded.
                This value is NULL if RomImage is non-NULL. This is the normal
                case.
    RomImage  - A PCI PC-AT ROM image. This argument is non-NULL if there is
                no hardware associated with the ROM and thus no PciHandle,
                otherwise is must be NULL.
                Example is PXE base code.
    Flags     - Indicates if ROM found and if PC-AT.
    DiskStart - Disk number of first device hooked by the ROM. If DiskStart
                is the same as DiskEnd no disked were hooked.
    DiskEnd   - Disk number of the last device hooked by the ROM.
    RomShaddowAddress - Shadow address of PC-AT ROM
    RomShdowedSize      - Size of RomShadowAddress in bytes

  Returns:
    EFI_SUCCESS            - Legacy ROM loaded for this device
    EFI_INVALID_PARAMETER  - PciHandle not found
    EFI_UNSUPPORTED        - There is no PCI ROM in the ROM BAR or no onboard ROM

--*/
;

EFI_STATUS
EFIAPI
LegacyBiosUpdateKeyboardLedStatus (
  IN EFI_LEGACY_BIOS_PROTOCOL           *This,
  IN UINT8                              Leds
  )
/*++

Routine Description:
  Fill in the standard BDA for Keyboard LEDs

Arguments:
  This    - Protocol instance pointer.
  Leds    - Current LED status

Returns:
  EFI_SUCCESS - It should always work.

--*/
;

EFI_STATUS
EFIAPI
LegacyBiosGetBbsInfo (
  IN  EFI_LEGACY_BIOS_PROTOCOL          *This,
  OUT UINT16                            *HddCount,
  OUT HDD_INFO                          **HddInfo,
  OUT UINT16                            *BbsCount,
  OUT IN BBS_TABLE                      **BbsTable
  )
/*++

  Routine Description:
    Get all BBS info

  Arguments:
    This       - Protocol instance pointer.
    HddCount   - Number of HDD_INFO structures
    HddInfo    - Onboard IDE controller information
    BbsCount   - Number of BBS_TABLE structures
    BbsTable   - List BBS entries

  Returns:
    EFI_SUCCESS      - Tables returned
    EFI_NOT_FOUND    - resource not found
    EFI_DEVICE_ERROR - can not get BBS table

--*/
;

EFI_STATUS
EFIAPI
LegacyBiosShadowAllLegacyOproms (
  IN EFI_LEGACY_BIOS_PROTOCOL   *This
  )
/*++

  Routine Description:
    Shadow all legacy16 OPROMs that haven't been shadowed.
    Warning: Use this with caution. This routine disconnects all EFI
             drivers. If used externally then caller must re-connect EFI
             drivers.
  Arguments:
    This       - Protocol instance pointer.

  Returns:
    EFI_SUCCESS  - OPROMs shadowed

--*/
;

EFI_STATUS
EFIAPI
LegacyBiosLegacyBoot (
  IN  EFI_LEGACY_BIOS_PROTOCOL          *This,
  IN  BBS_BBS_DEVICE_PATH               *BbsDevicePath,
  IN  UINT32                            LoadOptionsSize,
  IN  VOID                              *LoadOptions
  )
/*++

  Routine Description:
    Attempt to legacy boot the BootOption. If the EFI contexted has been
    compromised this function will not return.

  Arguments:
    This             - Protocol instance pointer.
    BbsDevicePath    - EFI Device Path from BootXXXX variable.
    LoadOptionsSize  - Size of LoadOption in size.
    LoadOptions      - LoadOption from BootXXXX variable

  Returns:
    EFI_SUCCESS      - Removable media not present

--*/
;

EFI_STATUS
LegacyBiosInitializeThunk (
  IN  LEGACY_BIOS_INSTANCE    *Private
  )
/*++

  Routine Description:
    Allocate memory < 1 MB and copy the thunker code into low memory. Se up
    all the descriptors.

  Arguments:
    Private - Private context for Legacy BIOS

  Returns:
    EFI_SUCCESS  - Should only pass.

--*/
;

EFI_STATUS
LegacyBiosInitBda (
  IN  LEGACY_BIOS_INSTANCE    *Private
  )
/*++

Routine Description:
  Fill in the standard BDA and EBDA stuff before Legacy16 load

Arguments:
  Private - Legacy BIOS Instance data

Returns:
  EFI_SUCCESS - It should always work.

--*/
;

EFI_STATUS
LegacyBiosBuildIdeData (
  IN  LEGACY_BIOS_INSTANCE      *Private,
  IN  HDD_INFO                  **HddInfo,
  IN  UINT16                    Flag
  )
/*++

Routine Description:
  Collect IDE Inquiry data from the IDE disks

Arguments:
  Private - Legacy BIOS Instance data
  HddInfo - Hdd Information
  Flag    - Reconnect IdeController or not

Returns:
  EFI_SUCCESS - It should always work.

--*/
;

VOID
EnableIdeController (
  IN LEGACY_BIOS_INSTANCE       *Private
  )
/*++

Routine Description:
  Enable ide controller.  This gets disabled when LegacyBoot.c is about
  to run the Option ROMs.

Arguments:
  Private - Legacy BIOS Instance data

Returns:

--*/
;

VOID
InitLegacyIdeController (
  IN EFI_HANDLE                        IdeController
  )
/*++

Routine Description:
  If the IDE channel is in compatibility (legacy) mode, remove all
  PCI I/O BAR addresses from the controller.

Arguments:
  IdeController - The handle of target IDE controller

Returns:

--*/
;

EFI_STATUS
PciProgramAllInterruptLineRegisters (
  IN  LEGACY_BIOS_INSTANCE      *Private
  )
/*++

  Routine Description:
    Program the interrupt routing register in all the PCI devices. On a PC AT system
    this register contains the 8259 IRQ vector that matches it's PCI interrupt.

  Arguments:
    Private - Legacy  BIOS Instance data

  Returns:
    EFI_SUCCESS         - Succeed.
    EFI_ALREADY_STARTED - All PCI devices have been processed.

--*/
;

EFI_STATUS
LegacyBiosBuildSioData (
  IN  LEGACY_BIOS_INSTANCE      *Private
  )
/*++

Routine Description:
  Collect EFI Info about legacy devices.

Arguments:
  Private - Legacy BIOS Instance data

Returns:
  EFI_SUCCESS - It should always work.

--*/
;

EFI_STATUS
PciShadowRoms (
  IN  LEGACY_BIOS_INSTANCE      *Private
  )
/*++

  Routine Description:
    Shadow all the PCI legacy ROMs. Use data from the Legacy BIOS Protocol
    to chose the order. Skip any devices that have already have legacy
    BIOS run.

  Arguments:
    Private - Protocol instance pointer.

  Returns:
    EFI_SUCCESS     - Succeed.
    EFI_UNSUPPORTED - Cannot get VGA device handle.

--*/
;

EFI_STATUS
LegacyBiosCompleteBdaBeforeBoot (
  IN  LEGACY_BIOS_INSTANCE    *Private
  )
/*++

Routine Description:
  Fill in the standard BDA and EBDA stuff prior to legacy Boot

Arguments:
  Private - Legacy BIOS Instance data

Returns:
  EFI_SUCCESS - It should always work.

--*/
;

EFI_STATUS
LegacyBiosInitCmos (
  IN  LEGACY_BIOS_INSTANCE    *Private
  )
/*++

Routine Description:
  Fill in the standard CMOS stuff before Legacy16 load

Arguments:
  Private - Legacy BIOS Instance data

Returns:
  EFI_SUCCESS - It should always work.

--*/
;

EFI_STATUS
LegacyBiosCompleteStandardCmosBeforeBoot (
  IN  LEGACY_BIOS_INSTANCE    *Private
  )
/*++

Routine Description:
  Fill in the standard CMOS stuff prior to legacy Boot

Arguments:
  Private - Legacy BIOS Instance data

Returns:
  EFI_SUCCESS - It should always work.

--*/
;

VOID
InterruptRedirectionTemplate (
  VOID
  )
/*++

Routine Description:
  Contains the code that is copied into low memory (below 640K).
  This code reflects interrupts 0x68-0x6f to interrupts 0x08-0x0f.
  This template must be copied into low memory, and the IDT entries
  0x68-0x6F must be point to the low memory copy of this code.  Each
  entry is 4 bytes long, so IDT entries 0x68-0x6F can be easily
  computed.

Arguments:
  None

Returns:
  None

--*/
;

VOID
DummyInterruptTemplate (
  VOID
  )
/*++

Routine Description:
   Dummy interrupt code for IVT initialize

Arguments:
  None

Returns:
  None

--*/
;

EFI_STATUS
LegacyBiosBuildE820 (
  IN  LEGACY_BIOS_INSTANCE    *Private,
  OUT UINTN                   *Size
  )
/*++

Routine Description:
  Build the E820 table.

Arguments:
  Private - Legacy BIOS Instance data
  Size    - Size of E820 Table

Returns:
  EFI_SUCCESS - It should always work.

--*/
;

EFI_STATUS
ReBuildE820Table (
  IN     LEGACY_BIOS_INSTANCE    *Private,
  IN OUT UINTN                   *Size
  )
/*++

Routine Description:
  Re-Build the E820 table to old structure.

Arguments:
  Private - Legacy BIOS Instance data
  Size    - Size of E820 Table

Returns:
  EFI_SUCCESS - It should always work.

--*/
;

VOID
ShutdownAPs (
  IN LEGACY_BIOS_INSTANCE              *Private
  )
/*++

Routine Description:
  This function is to put all AP in halt state.

Arguments:
  Private - Legacy BIOS Instance data

Returns:
  None

--*/
;

VOID
GetRegisters (
  LOW_MEMORY_THUNK    *IntThunk
  )
/*++

Routine Description:
  Worker function for LegacyBiosGetFlatDescs, retrieving content of
  specific registers.

Arguments:
  IntThunk - Pointer to IntThunk of Legacy BIOS context.

Returns:
  None

--*/
;

UINTN
CallRealThunkCode (
  UINT8               *RealCode,
  UINT8               BiosInt,
  UINT32              CallAddress
  )
/*++

Routine Description:
  Routine for calling real thunk code.

Arguments:
  RealCode    - The address of thunk code.
  BiosInt     - The Bios interrupt vector number.
  CallAddress - The address of 16-bit mode call.

Returns:
  Status returned by real thunk code

--*/
;

VOID
GenerateSoftInit (
  UINT8               Vector
  )
/*++

Routine Description:
  Routine for generating soft interrupt.

Arguments:
  Vector - The interrupt vector number.

Returns:
  None

--*/
;

EFI_STATUS
CreateMpTable (
  LEGACY_BIOS_INSTANCE                  *Private
  )
/*++

  Routine Description:
    Create the Mp table in E F -Segment 

  Arguments:
    Private       - Legacy  BIOS Instance data

  Returns:
    EFI_SUCCESS   - Create Table success
    EFI_OUT_OF_RESOURCES - Not enought momory space.

--*/ 
;

#endif
