/** @file
  Header file for CPU MTRR programming driver

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/
#ifndef _EFI_MEMORY_ATTRIB_H
#define _EFI_MEMORY_ATTRIB_H

extern UINT32 mUsedMtrr;

typedef struct {
  UINT32 Msr;
  UINT32 BaseAddress;
  UINT32 Length;
} FIXED_MTRR;

typedef struct {
  UINT64  MsrValue;
  BOOLEAN Changed;
} MTRR_VALUE;

typedef struct {
  UINT64  BaseAddress;
  UINT64  Length;
  UINT64  Type;
  UINT32  Msr;
  BOOLEAN Valid;
} VARIABLE_MTRR;

#define EFI_MEMORY_CACHETYPE_MASK (EFI_MEMORY_UC | EFI_MEMORY_WC | EFI_MEMORY_WT | EFI_MEMORY_WB | EFI_MEMORY_UCE)

/**
  Calculate fixed MTRR

  @param[in] MemoryCacheType - Cache type for this memory range
  @param[in] Base            - Memory range base address
  @param[in] Length          - Memory range length

  @exception EFI_UNSUPPORTED - Fixed MTRR number not enough or not present
  @retval EFI_SUCCESS - Fixed MTRR settings calculated successfully
**/
EFI_STATUS
CalculateFixedMtrr (
  IN UINT64 MemoryCacheType,
  IN UINT64 *Base,
  IN UINT64 *Length
  );

/**
  Disable Cache MTRR
**/
VOID
PreMtrrChange (
  VOID
  );
/**
  Enable Cache MTRR
**/
VOID
PostMtrrChange (
  VOID
  );

/**
  Program fixed MTRR
**/
VOID
ProgramFixedMtrr (
  VOID
  );

/**
  Get all information about memory cache registers

  @retval EFI_SUCCESS - always success
**/
EFI_STATUS
GetMemoryAttribute (
  VOID
  );

/**
  Check if different memory attribute range overlapping with each other

  @param[in] Start - start of memory range that will be checking
  @param[in] End   - end of memory range address that will be checking

  @retval TRUE if overlapping found
  @retval FALSE if not found
**/
BOOLEAN
CheckMemoryAttributeOverlap (
  IN EFI_PHYSICAL_ADDRESS Start,
  IN EFI_PHYSICAL_ADDRESS End
  );

/**
  Combine current memory attribute range to existing memory attribute range

  @param[in] Attribute   - Cache type
  @param[in] Base        - Base address of memory range that will be combined into existing one.
  @param[in] Length      - Length of the memory range that will be combined into existing one.

  @retval EFI_SUCCESS - Memory combined successfully
  @retval EFI_ACCESS_DENIED - memory type that is not allowed to overlap
**/
EFI_STATUS
CombineMemoryAttribute (
  IN UINT64 Attribute,
  IN UINT64 *Base,
  IN UINT64 *Length
  );

/**
  Given the input, check if the number of MTRR is lesser
    if positive or subtractive

    @param[in] Input - Length of Memory to program MTRR
    @param[in] MtrrNumber - return needed Mtrr number
    @param[in] Direction  - TRUE: do positive
                            FALSE: do subtractive

    @retval EFI_SUCCESS - always return success
**/
EFI_STATUS
GetDirection (
  IN UINT64  Input,
  IN UINTN   *MtrrNumber,
  IN BOOLEAN *Direction
  );

/**
  Calculate max memory of power 2

  @param[in] MemoryLength  - Memory length that will be calculated

  @retval Max memory
**/
UINT64
Power2MaxMemory (
  IN UINT64 MemoryLength
  );

/**
  Clear MTRR

  @param[in] MtrrNumber  - MTRR register that will be cleared
  @param[in] Index       - index of MTRR register

  @retval EFI_SUCCESS - always return success
**/
EFI_STATUS
InvariableMtrr (
  IN UINTN MtrrNumber,
  IN UINTN Index
  );

/**
  Programm VARIABLE MTRR

  @param[in] MtrrNumber      - Variable MTRR register
  @param[in] BaseAddress     - Memory base address
  @param[in] Length          - Memory length
  @param[in] MemoryCacheType - Cache type

  @retval EFI_SUCCESS - always return success
**/
EFI_STATUS
ProgramVariableMtrr (
  IN UINTN                MtrrNumber,
  IN EFI_PHYSICAL_ADDRESS BaseAddress,
  IN UINT64               Length,
  IN UINT64               MemoryCacheType
  );

/**
  Get GCD Mem Space type from Mtrr Type

  @param[in] MtrrAttribute - Mtrr type

  @retval GCD Mem Space typed (64-bit)
**/
UINT64
GetMemorySpaceAttributeFromMtrrType (
  IN UINT8 MtrrAttribute
  );

/**
  Refresh the GCD Memory Space Attributes according to MTRRs

  @retval EFI_STATUS - status returned from each sub-routine
**/
EFI_STATUS
RefreshGcdMemoryAttributes (
  VOID
  );

/**
  Search into the Gcd Memory Space for descriptors (from StartIndex
  to EndIndex) that contains the memory range specified by BaseAddress
  and Length.

  @param[in] MemorySpaceMap      - Gcd Memory Space Map as array
  @param[in] NumberOfDescriptors - Number of descriptors in map
  @param[in] BaseAddress         - BaseAddress for the requested range
  @param[in] Length              - Length for the requested range
  @param[in] StartIndex          - Start index into the Gcd Memory Space Map
  @param[in] EndIndex            - End index into the Gcd Memory Space Map

  @retval EFI_SUCCESS   - Search successfully
  @retval EFI_NOT_FOUND - The requested descriptors not exist
**/
EFI_STATUS
SearchGcdMemorySpaces (
  IN EFI_GCD_MEMORY_SPACE_DESCRIPTOR *MemorySpaceMap,
  IN UINTN                           NumberOfDescriptors,
  IN EFI_PHYSICAL_ADDRESS            BaseAddress,
  IN UINT64                          Length,
  OUT UINTN                          *StartIndex,
  OUT UINTN                          *EndIndex
  );

/**
  Set the attributes for a specified range in Gcd Memory Space Map.

  @param[in] MemorySpaceMap      - Gcd Memory Space Map as array
  @param[in] NumberOfDescriptors - Number of descriptors in map
  @param[in] BaseAddress         - BaseAddress for the range
  @param[in] Length              - Length for the range
  @param[in] Attributes          - Attributes to set

  @retval EFI_SUCCESS   - Set successfully
  @retval EFI_NOT_FOUND - The specified range does not exist in Gcd Memory Space
**/
EFI_STATUS
SetGcdMemorySpaceAttributes (
  IN EFI_GCD_MEMORY_SPACE_DESCRIPTOR *MemorySpaceMap,
  IN UINTN                           NumberOfDescriptors,
  IN EFI_PHYSICAL_ADDRESS            BaseAddress,
  IN UINT64                          Length,
  IN UINT64                          Attributes
  );
#endif
