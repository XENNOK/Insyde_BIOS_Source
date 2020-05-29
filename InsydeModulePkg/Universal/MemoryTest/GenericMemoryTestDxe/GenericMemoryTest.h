/** @file
  Header file for Generic memory test Driver's Data Structures

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _GENERIC_MEMORY_TEST_H_
#define _GENERIC_MEMORY_TEST_H_

#include <Library/UefiLib.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Guid/DebugMask.h>
#include <Guid/HobList.h>
#include <Protocol/Cpu.h>
#include <Protocol/PlatformMemTest.h>
#include <Protocol/VirtualMemoryAccess.h>

#include <Library/DebugLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/HobLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>

//
// Some global define
//
#define GENERIC_CACHELINE_SIZE  0x40

//
// attributes for reserved memory before it is promoted to system memory
//
#define EFI_MEMORY_PRESENT      0x0100000000000000ULL
#define EFI_MEMORY_INITIALIZED  0x0200000000000000ULL
#define EFI_MEMORY_TESTED       0x0400000000000000ULL

//
// The SPARSE_SPAN_SIZE size can not small then the MonoTestSize
//
#define TEST_BLOCK_SIZE   0x2000000
#define QUICK_SPAN_SIZE   (TEST_BLOCK_SIZE >> 2)
#define SPARSE_SPAN_SIZE  (TEST_BLOCK_SIZE >> 4)
//
// Above 4G memory access define
//
#define IA32_MAX_LINEAR_ADDRESS 0xFFFFFFFF
#define MAPWINDOWS_ADDRESS      0x40000000
#define MAPWINDOWS_SIZE         0x10000000
#define MAX_PAGESIZE            0x400000

//
// This records the platform's every DIMM's infomation. It is depend on
// platform memory test driver. If firmware fail to get platform memory test
// protocol, there will not generate the DIMM description information,
// of course the generic memory test driver will not have the feature
// "DisabelDimm", "ECC check" etc.
//
#define EFI_DIMM_DESCRIPTION_SIGNATURE  SIGNATURE_32 ('D', 'I', 'm', 'M')
typedef struct {
  UINT32                Signature;
  LIST_ENTRY            Link;
  EFI_MEMTEST_DIMM_INFO DimmInfo;
  BOOLEAN               ReportRequired;
  BOOLEAN               CorrectReportRequired;
  BOOLEAN               UnCorrectReportRequired;
  BOOLEAN               MapoutRequired;
  UINT32                CorrectableError;
  UINT32                UncorrectableError;
} EFI_DIMM_DESCRIPTION;

#define EFI_DIMM_DESCRIPTION_FROM_LINK(link)  CR (link, EFI_DIMM_DESCRIPTION, Link, EFI_DIMM_DESCRIPTION_SIGNATURE)

//
// This structure records the base memory range which have already been
// tested in PEI phase, the base memroy range is get from HOB when the generic
// memory test driver loaded.
//
#define EFI_BASE_MEMORY_RANGE_SIGNATURE SIGNATURE_32 ('B', 'A', 'M', 'E')
typedef struct {
  UINTN                 Signature;
  LIST_ENTRY        Link;
  EFI_PHYSICAL_ADDRESS  StartAddress;
  UINT64                Length;
  UINT64                Capabilities;
} BASE_MEMORY_RANGE;

#define BASE_MEMORY_RANGE_FROM_LINK(link) \
  CR ( \
  link, \
  BASE_MEMORY_RANGE, \
  Link, \
  EFI_BASE_MEMORY_RANGE_SIGNATURE \
  )

//
// This structure records every nontested memory range parsed through GCD
// service.
//
#define EFI_NONTESTED_MEMORY_RANGE_SIGNATURE  SIGNATURE_32 ('N', 'T', 'M', 'E')

typedef struct {
  UINTN                 Signature;
  LIST_ENTRY            Link;
  EFI_PHYSICAL_ADDRESS  StartAddress;
  UINT64                Length;
  UINT64                Capabilities;
  BOOLEAN               Above4G;
  BOOLEAN               AlreadyMapped;
} NONTESTED_MEMORY_RANGE;

#define NONTESTED_MEMORY_RANGE_FROM_LINK(link) \
  CR ( \
  link, \
  NONTESTED_MEMORY_RANGE, \
  Link, \
  EFI_NONTESTED_MEMORY_RANGE_SIGNATURE \
  )

//
// This is the memory test driver's structure definition
//
#define EFI_GENERIC_MEMORY_TEST_PRIVATE_SIGNATURE SIGNATURE_32 ('G', 'E', 'M', 'T')

//
// This is the link list type define used by destroy function
//
typedef enum {
  DimmList,
  BaseMemList,
  NontestedMemList
} LINK_LIST_TYPE;

typedef struct {

  UINTN                             Signature;
  EFI_HANDLE                        Handle;

  //
  // platform memory test driver's relate definition
  //
  EFI_PLATFORM_MEMTEST_PROTOCOL       *PmtMemoryTest;

  EFI_PLATFORM_MEMTEST_CAPABILITIES   PmtCapability;
  EFI_MEMTEST_ADDRESS_PATTERN         PmtAddressLinePattern;
  EFI_MEMTEST_DATA_PATTERN            PmtDataLinePattern;
  EFI_MEMTEST_DATA_PATTERN            PmtMemoryPattern;

  UINT32                              PolicyEntryCount;
  EFI_MEMTEST_POLICY_ENTRY            *PolicyGrid;

  //
  // generic memory test driver's protocol
  //
  EFI_GENERIC_MEMORY_TEST_PROTOCOL  GenericMemoryTest;

  //
  // memory test covered spans
  //
  EXTENDMEM_COVERAGE_LEVEL          CoverLevel;
  UINTN                             CoverageSpan;
  UINT64                            BdsBlockSize;

  //
  // the memory test pattern and size every time R/W/V memory
  //
  VOID                              *MonoPattern;
  UINTN                             MonoTestSize;

  //
  // base memory's size which tested in PEI phase
  //
  UINT64                            BaseMemorySize;

  //
  // memory range list and dimm description list
  //
  LIST_ENTRY                       BaseMemRanList;
  LIST_ENTRY                       DimmDscList;
  LIST_ENTRY                       NonTestedMemRanList;
  //
  // Above 4G memory access protocol
  //
  EFI_VIRTUAL_MEMORY_ACCESS_PROTOCOL  *Above4GMemoryAccess;
  EFI_PHYSICAL_ADDRESS                Below4GWindows;

} GENERIC_MEMORY_TEST_PRIVATE;

#define GENERIC_MEMORY_TEST_PRIVATE_FROM_THIS(a) \
  CR ( \
  a, \
  GENERIC_MEMORY_TEST_PRIVATE, \
  GenericMemoryTest, \
  EFI_GENERIC_MEMORY_TEST_PRIVATE_SIGNATURE \
  )

#endif
