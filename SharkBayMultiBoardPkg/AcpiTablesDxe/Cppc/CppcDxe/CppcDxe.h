/** @file
  Header file for the Collaborative Processor Performance Control (CPPC) driver.

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
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

#ifndef _CPPC_H_
#define _CPPC_H_

#include <SaAccess.h>
#include <CpuAccess.h>
#include <IndustryStandard/Acpi30.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/IoLib.h>
#include <Library/BaseLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/CpuPlatformLib.h>
#include <Library/S3BootScriptLib.h>
#include <Protocol/AcpiSystemDescriptionTable.h>
#include <Protocol/AcpiPlatformPolicy.h>
#include <Protocol/FirmwareVolume.h>
#include <Protocol/AcpiTable.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Guid/CppcAcpiTableStorage.h>


#define CPPC_SIZE                 0x1000 /// 4kb of runtime memory for CPPC shared memory buffer
#define PCI_DEV_NUM_ICH_LPC       31     /// ICH is Device 31
#define R_ACPI_BAR                0x40   /// ACPI Base Address Register
#define R_ACPI_GPE_CNTL           0x42   /// GPE control register offset
#define CPUID_FULL_FAMILY_MODEL_HASWELL  0x000306C0
#define EFI_MSR_POWER_CTL         0x000001FC

///
/// SSDT data storage file
///
/// AML parsing definitions
///
#define AML_NAME_OP         0x08

///
/// ASL NAME structure
///
#pragma pack(1)
typedef struct {
  UINT8   NameOp;       //  Byte [0]=0x08:NameOp.
  UINT32  NameString;   //  Byte [4:1]=Name of object.
  UINT8   DWordPrefix;  //  Byte [5]=0x0C:DWord Prefix.
  UINT32  Value;        //  Byte [9:6] ; Value of named object.
} NAME_LAYOUT;
#pragma pack()

///
/// ACPI 5.0 Generic Address Space definition
///
#pragma pack(1)
typedef struct {
  UINT8   AddressSpaceId;
  UINT8   RegisterBitWidth;
  UINT8   RegisterBitOffset;
  UINT8   AccessSize;
  UINT64  Address;
} EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE;
#pragma pack()

///
/// Platform Communications Channel Table (PCCT)
///
#pragma pack(1)
typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER Header;
  UINT32                      Flags;
  UINT64                      Reserved;
} EFI_ACPI_5_0_PLATFORM_COMMUNICATION_CHANNEL_TABLE_HEADER;
#pragma pack()

///
/// Generic Communications Subspace Structure
///
#pragma pack(1)
typedef struct {
  UINT8                                    Type;
  UINT8                                    Length;
  UINT8                                    Reserved[6];
  UINT64                                   BaseAddress;
  UINT64                                   AddressLength;
  EFI_ACPI_5_0_GENERIC_ADDRESS_STRUCTURE   DoorbellRegister;
  UINT64                                   DoorbellPreserve;
  UINT64                                   DoorbellWrite;
  UINT32                                   NominalLatency;
  UINT32                                   MaximumPeriodicAccessRate;
  UINT16                                   MinimumRequestTurnaroundTime;
} EFI_ACPI_5_0_PCCT_SUBSPACE_GENERIC;
#pragma pack()

///
/// UINT64 workaround
///
/// The MS compiler doesn't handle QWORDs very well.  I'm breaking
/// them into DWORDs to circumvent the problems.  Converting back
/// shouldn't be a big deal.
///
#pragma pack(1)
typedef union _MSR_REGISTER {
  UINT64  Qword;

  struct _DWORDS {
    UINT32  Low;
    UINT32  High;
  } Dwords;

  struct _BYTES {
    UINT8 FirstByte;
    UINT8 SecondByte;
    UINT8 ThirdByte;
    UINT8 FouthByte;
    UINT8 FifthByte;
    UINT8 SixthByte;
    UINT8 SeventhByte;
    UINT8 EighthByte;
  } Bytes;

} MSR_REGISTER;
#pragma pack()

#ifndef BIT63
#define BIT0                     0x0001
#define BIT1                     0x0002
#define BIT2                     0x0004
#define BIT3                     0x0008
#define BIT4                     0x0010
#define BIT5                     0x0020
#define BIT6                     0x0040
#define BIT7                     0x0080
#define BIT8                     0x0100
#define BIT9                     0x0200
#define BIT10                    0x0400
#define BIT11                    0x0800
#define BIT12                    0x1000
#define BIT13                    0x2000
#define BIT14                    0x4000
#define BIT15                    0x8000
#define BIT16                0x00010000
#define BIT17                0x00020000
#define BIT18                0x00040000
#define BIT19                0x00080000
#define BIT20                0x00100000
#define BIT21                0x00200000
#define BIT22                0x00400000
#define BIT23                0x00800000
#define BIT24                0x01000000
#define BIT25                0x02000000
#define BIT26                0x04000000
#define BIT27                0x08000000
#define BIT28                0x10000000
#define BIT29                0x20000000
#define BIT30                0x40000000
#define BIT31                0x80000000
#define BIT32               0x100000000
#define BIT33               0x200000000
#define BIT34               0x400000000
#define BIT35               0x800000000
#define BIT36              0x1000000000
#define BIT37              0x2000000000
#define BIT38              0x4000000000
#define BIT39              0x8000000000
#define BIT40             0x10000000000
#define BIT41             0x20000000000
#define BIT42             0x40000000000
#define BIT43             0x80000000000
#define BIT44            0x100000000000
#define BIT45            0x200000000000
#define BIT46            0x400000000000
#define BIT47            0x800000000000
#define BIT48           0x1000000000000
#define BIT49           0x2000000000000
#define BIT50           0x4000000000000
#define BIT51           0x8000000000000
#define BIT52          0x10000000000000
#define BIT53          0x20000000000000
#define BIT54          0x40000000000000
#define BIT55          0x80000000000000
#define BIT56         0x100000000000000
#define BIT57         0x200000000000000
#define BIT58         0x400000000000000
#define BIT59         0x800000000000000
#define BIT60        0x1000000000000000
#define BIT61        0x2000000000000000
#define BIT62        0x4000000000000000
#define BIT63        0x8000000000000000
#endif

#define MmPciExpressAddress(Segment, Bus, Device, Function, Register) \
  ( (UINTN) (PciRead32 (PCI_LIB_ADDRESS (0,0,0,0x60)) & 0xFC000000) + \
    (UINTN) (Bus << 20) + \
    (UINTN) (Device << 15) + \
    (UINTN) (Function << 12) + \
    (UINTN) (Register) \
  )

EFI_STATUS
EFIAPI
InitializeCppc (
  IN EFI_HANDLE               ImageHandle,
  IN EFI_SYSTEM_TABLE         *SystemTable
  );

VOID
LoadAcpiTables(
  VOID
  );

VOID
LoadAcpiTablesPcct(
  VOID
  );

VOID
LoadAcpiTablesCpc(
  VOID
  );

EFI_STATUS
LocateSupportProtocol (
  IN  EFI_GUID                *Protocol,
  OUT VOID                    **Instance,
  IN  BOOLEAN                 Type
  );

#endif
