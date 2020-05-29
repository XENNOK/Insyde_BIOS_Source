/** @file
  Provide Cache services for DXE phase

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

#include "CpuAccess.h"
#include "MpService.h"
#include "CpuInitDxe.h"
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>

#define ALIGNED_SEED  0x01010101

extern EFI_MTRR_VALUES  *mFixedMtrrValues;

FIXED_MTRR              mFixedMtrrTable[];
/**

    Disable cache and its mtrr
    
    @param[in] OldMtrr - To return the Old MTRR value


**/
VOID
EfiDisableCacheMtrr (
  OUT UINT64                   *OldMtrr
  )
{
  UINT64  TempQword;

  AsmDisableCache ();
  
  ///
  /// Disable Cache MTRR
  ///
  *OldMtrr  = AsmReadMsr64 (CACHE_IA32_MTRR_DEF_TYPE);
  TempQword = (*OldMtrr) &~B_CACHE_MTRR_VALID &~B_CACHE_FIXED_MTRR_VALID;
  AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, TempQword);

  return ;
}

/**
 
    Recover cache MTRR
    
    @param[in] EnableMtrr - Whether to enable the MTRR
    @param[in] OldMtrr    - The saved old MTRR value to restore when not to 
                 enable the MTRR


**/
VOID
EfiRecoverCacheMtrr (
  IN BOOLEAN                  EnableMtrr,
  IN UINT64                   OldMtrr
  )
{
  UINT64  TempQword;

  TempQword = 0;
  ///
  /// Enable Cache MTRR
  ///
  if (EnableMtrr) {
    TempQword = AsmReadMsr64 (CACHE_IA32_MTRR_DEF_TYPE);
    TempQword |= (B_CACHE_MTRR_VALID | B_CACHE_FIXED_MTRR_VALID);
  } else {
    TempQword = OldMtrr;
  }

  AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, TempQword);

  AsmEnableCache ();
  return ;
}

/**

  Programming MTRR according to Memory address, length, and type.

  @param[in] MtrrNumber           - the variable MTRR index number
  @param[in] MemoryAddress        - the address of target memory
  @param[in] MemoryLength         - the length of target memory
  @param[in] MemoryCacheType      - the cache type of target memory
  @param[in] ValidMtrrAddressMask - the MTRR address mask


**/
VOID
EfiProgramMtrr (
  IN UINT32                    MtrrNumber,
  IN EFI_PHYSICAL_ADDRESS      MemoryAddress,
  IN UINT64                    MemoryLength,
  IN EFI_MEMORY_CACHE_TYPE     MemoryCacheType,
  IN UINT64                    ValidMtrrAddressMask
  )
{
  UINT64  TempQword;
  UINT64  OldMtrr;

  EfiDisableCacheMtrr (&OldMtrr);

  ///
  /// MTRR Physical Base
  ///
  TempQword = (MemoryAddress & ValidMtrrAddressMask) | MemoryCacheType;
  AsmWriteMsr64 (MtrrNumber, TempQword);

  ///
  /// MTRR Physical Mask
  ///
  TempQword = ~(MemoryLength - 1);
  AsmWriteMsr64 (MtrrNumber + 1, (TempQword & ValidMtrrAddressMask) | B_CACHE_MTRR_VALID);

  EfiRecoverCacheMtrr (TRUE, OldMtrr);

  return ;
}

/**

  Program the unaligned MTRR register.

  @param[in] AlignedQword          - The aligned 64-bit cache type.
  @param[in] MsrNum                - The index of current MTRR.
  @param[in] UnalignedBase         - Base Address of the current unaligned MTRR.
  @param[in] UnalignedLimit        - Limit Address of the current unaligned MTRR.

  @retval EFI_SUCCESS           - The unaligned MTRR is set successfully.
  @retval EFI_DEVICE_ERROR      - The unaligned address is not the multiple of the basic length of MTRR.

**/
EFI_STATUS
ProgramUnalignedMtrr (
  IN UINT64                    AlignedQword,
  IN UINTN                     MsrNum,
  IN UINT32                    UnalignedBase,
  IN UINT32                    UnalignedLimit
  )
{
  UINT32  UnalignedOffset;
  UINT64  TempQword;
  UINT64  Mask;
  UINT8   ByteShift;

  UnalignedOffset = UnalignedBase - mFixedMtrrTable[MsrNum].BaseAddress;
  if (UnalignedOffset % mFixedMtrrTable[MsrNum].Length != 0) {
    return EFI_DEVICE_ERROR;
  }

  ByteShift = (UINT8) (UnalignedOffset / mFixedMtrrTable[MsrNum].Length);
  Mask      = ~(LShiftU64 (1, ByteShift * 8) - 1);

  if (UnalignedLimit < mFixedMtrrTable[MsrNum + 1].BaseAddress) {
    UnalignedOffset = UnalignedLimit - mFixedMtrrTable[MsrNum].BaseAddress;
    if (UnalignedOffset % mFixedMtrrTable[MsrNum].Length != 0) {
      return EFI_DEVICE_ERROR;
    }

    ByteShift = (UINT8) (UnalignedOffset / mFixedMtrrTable[MsrNum].Length);
    Mask &= LShiftU64 (1, ByteShift * 8) - 1;
  }

  TempQword = AsmReadMsr64 (mFixedMtrrTable[MsrNum].Msr) &~Mask;
  TempQword |= AlignedQword & Mask;

  AsmWriteMsr64 (mFixedMtrrTable[MsrNum].Msr, TempQword);
  return EFI_SUCCESS;
}

/**

  Given the low memory range ( <= 1MB) and cache type, program the MTRRs.

  @param[in] MemoryCacheType       - Cache Type.
  @param[in] MemoryBase            - Base Address of Memory to program MTRR.
  @param[in] MemoryLimit           - Limit Address of Memory to program MTRR.

  @retval EFI_SUCCESS           - Low memory MTRR is set successfully.
  @retval others                - An error occurs when setting Low memory MTRR.

**/
EFI_STATUS
ProgramLowMemoryMtrr (
  IN EFI_MEMORY_CACHE_TYPE     MemoryCacheType,
  IN UINT32                    MemoryBase,
  IN UINT32                    MemoryLimit
  )
{
  EFI_STATUS  Status;
  UINT64      OldMtrr;
  ALIGNED     Aligned;
  UINTN       MsrNum;

  EfiDisableCacheMtrr (&OldMtrr);
  Status                    = EFI_SUCCESS;

  Aligned.AlignedDword.High = MemoryCacheType * ALIGNED_SEED;
  Aligned.AlignedDword.Low  = Aligned.AlignedDword.High;

  for (MsrNum = 0; mFixedMtrrTable[MsrNum].BaseAddress < MemoryBase; MsrNum++) {
    ;
  }

  if (MemoryBase < mFixedMtrrTable[MsrNum].BaseAddress) {
    Status = ProgramUnalignedMtrr (Aligned.AlignedQword, MsrNum - 1, MemoryBase, MemoryLimit);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
  }

  while (MsrNum < V_FIXED_MTRR_NUMBER && MemoryLimit >= mFixedMtrrTable[MsrNum + 1].BaseAddress) {
    ///
    /// Program aligned MTRR
    ///
    AsmWriteMsr64 (mFixedMtrrTable[MsrNum].Msr, Aligned.AlignedQword);
    MsrNum++;
  }

  if (MemoryLimit > mFixedMtrrTable[MsrNum].BaseAddress) {
    Status = ProgramUnalignedMtrr (Aligned.AlignedQword, MsrNum, mFixedMtrrTable[MsrNum].BaseAddress, MemoryLimit);
  }

Done:
  EfiRecoverCacheMtrr (TRUE, OldMtrr);
  return Status;
}

/**

  Given the memory range and cache type, programs the MTRRs.

  @param[in] MemoryAddress         - Base Address of Memory to program MTRR.
  @param[in] MemoryLength          - Length of Memory to program MTRR.
  @param[in] MemoryCacheType       - Cache Type.

  @retval EFI_SUCCESS           - Mtrr are set successfully.
  @retval EFI_LOAD_ERROR        - No empty MTRRs to use.
  @retval EFI_INVALID_PARAMETER - The input parameter is not valid.
  @retval others                - An error occurs when setting MTTR.
Note:

**/
EFI_STATUS
EFIAPI
EfiSetCacheAttributes (
  IN EFI_PHYSICAL_ADDRESS      MemoryAddress,
  IN UINT64                    MemoryLength,
  IN EFI_MEMORY_CACHE_TYPE     MemoryCacheType
  )
{
  EFI_STATUS            Status;
  UINT32                MsrNum;
  UINT64                TempQword;
  UINT32                UsedMsrNum;
  EFI_MEMORY_CACHE_TYPE UsedMemoryCacheType;
  UINT64                ValidMtrrAddressMask;
  EFI_CPUID_REGISTER    FeatureInfo;
  UINT64                Power2Length[8];
  UINT64                LengthArray[8];
  UINTN                 LengthSize;
  UINTN                 Index;
  UINTN                 Count;
  UINT32                Remainder;
  UINT32                VariableMtrrLimit;

  VariableMtrrLimit     = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);

  ValidMtrrAddressMask  = LShiftU64 ((UINT64) 1, 36) & (~ (UINT64) 0x0FFF);

  AsmCpuid (
    CPUID_EXTENDED_FUNCTION,
    &FeatureInfo.RegEax,
    &FeatureInfo.RegEbx,
    &FeatureInfo.RegEcx,
    &FeatureInfo.RegEdx
    );

  if (FeatureInfo.RegEax >= CPUID_VIR_PHY_ADDRESS_SIZE) {
    AsmCpuid (
      CPUID_VIR_PHY_ADDRESS_SIZE,
      &FeatureInfo.RegEax,
      &FeatureInfo.RegEbx,
      &FeatureInfo.RegEcx,
      &FeatureInfo.RegEdx
      );
    ValidMtrrAddressMask = (LShiftU64 ((UINT64) 1, FeatureInfo.RegEax & 0xFF) - 1) & (~ (UINT64) 0x0FFF);
  }
  ///
  /// Check for valid memory address and valid cache type
  ///
  ///
  if (((MemoryAddress &~ValidMtrrAddressMask) != 0 || (MemoryLength &~ValidMtrrAddressMask) != 0) ||
      (MemoryCacheType > CACHE_WRITEBACK)
      ) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Check if Fixed MTRR
  ///
  if ((MemoryAddress + MemoryLength) <= (1 << 20)) {
    Status = ProgramLowMemoryMtrr (
              MemoryCacheType,
              (UINT32) MemoryAddress,
              (UINT32) (MemoryAddress + MemoryLength)
              );
    return Status;
  }
  ///
  /// Special case for 1 MB base address
  ///
  if (MemoryAddress == 0x100000) {
    MemoryAddress = 0;
    MemoryLength += 0x100000;
  }
  ///
  /// Split MemoryLength into a sum of power of 2
  ///
  ZeroMem (Power2Length, sizeof (Power2Length));
  LengthSize  = 0;
  TempQword   = MemoryLength;
  do {
    Power2Length[LengthSize] = Power2MaxMemory (TempQword);
    TempQword -= Power2Length[LengthSize];
    LengthSize++;
  } while (TempQword != 0 && LengthSize < 8);
  if (TempQword != 0) {
    return EFI_LOAD_ERROR;
  }
  ///
  /// Work out an order of splitted power of 2
  /// so that Base and Length are suitable for MTRR
  /// setting constraints.
  ///
  Count     = 0;
  TempQword = MemoryAddress;
  do {
    for (Index = 0; Index < LengthSize; Index++) {
      if (Power2Length[Index] != 0) {
        if (Power2Length[Index] >= 0x100000000ULL) {
          Remainder = (UINT32) TempQword;
          if (Remainder == 0) {
            DivU64x32Remainder (
              RShiftU64 (TempQword,
              32),
              (UINT32) RShiftU64 (Power2Length[Index],
                      32),
              &Remainder
              );
          }
        } else {
          DivU64x32Remainder (TempQword, (UINT32) Power2Length[Index], &Remainder);
        }

        if (Remainder == 0) {
          LengthArray[Count] = Power2Length[Index];
          TempQword += Power2Length[Index];
          Power2Length[Index] = 0;
          Count++;
          break;
        }
      }
    }

    if (Index == LengthSize) {
      return EFI_LOAD_ERROR;
    }
  } while (Count < LengthSize);
  ///
  /// Begin setting the MTRR according to the order
  ///
  for (Index = 0; Index < LengthSize; Index++, MemoryAddress += MemoryLength) {
    MemoryLength = LengthArray[Index];
    ///
    /// Search if the range attribute has been set before
    ///
    Status = SearchForExactMtrr (
              MemoryAddress,
              MemoryLength,
              ValidMtrrAddressMask,
              &UsedMsrNum,
              &UsedMemoryCacheType
              );

    if (!EFI_ERROR (Status)) {
      ///
      /// Compare if it has the same type as current setting
      ///
      if (UsedMemoryCacheType != MemoryCacheType) {
        ///
        /// Different type
        ///
        ///
        /// Check if the set type is the same as default type
        ///
        if (IsDefaultType (MemoryCacheType)) {
          ///
          /// Clear the mtrr
          ///
          AsmWriteMsr64 (UsedMsrNum, 0);
          AsmWriteMsr64 (UsedMsrNum + 1, 0);

        } else {
          ///
          /// Modify the mtrr type
          ///
          EfiProgramMtrr (
            UsedMsrNum,
            MemoryAddress,
            MemoryLength,
            MemoryCacheType,
            ValidMtrrAddressMask
            );
        }
      }

      continue;
    }
    ///
    /// Find first unused MTRR
    ///
    for (MsrNum = CACHE_VARIABLE_MTRR_BASE; MsrNum < (CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit * 2); MsrNum += 2) {
      if ((AsmReadMsr64 (MsrNum + 1) & B_CACHE_MTRR_VALID) == 0) {
        break;
      }
    }
    ///
    /// Check if we ran out of variable-range MTRRs
    ///
    if (MsrNum >= (CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit * 2)) {
      return EFI_LOAD_ERROR;
    }

    EfiProgramMtrr (
      MsrNum,
      MemoryAddress,
      MemoryLength,
      MemoryCacheType,
      ValidMtrrAddressMask
      );
  }

  return EFI_SUCCESS;
}

/**

   Reset all the MTRRs to a known state.

  @param[in] None
  @retval EFI_SUCCESS - All MTRRs have been reset successfully.

**/
EFI_STATUS
EFIAPI
EfiResetCacheAttributes (
  VOID
  )
{
  UINT32  MsrNum;
  UINT16  Index;
  UINT64  OldMtrr;
  UINT32  VariableMtrrLimit;

  VariableMtrrLimit = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);

  Index             = 0;
  EfiDisableCacheMtrr (&OldMtrr);

  ///
  /// Reset Fixed Mtrrs
  ///
  while (mFixedMtrrTable[Index].Msr != 0) {
    AsmWriteMsr64 (mFixedMtrrTable[Index].Msr, 0);
    Index++;
  }
  ///
  /// Reset Variable Mtrrs
  ///
  for (MsrNum = CACHE_VARIABLE_MTRR_BASE; MsrNum < (CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit * 2); MsrNum++) {
    AsmWriteMsr64 (MsrNum, 0);
  }

  EfiRecoverCacheMtrr (TRUE, OldMtrr);

  return EFI_SUCCESS;
}

/**

  Search the memory cache type for specific memory from MTRR.

  @param[in] MemoryAddress        - the address of target memory
  @param[in] MemoryLength         - the length of target memory
  @param[in] ValidMtrrAddressMask - the MTRR address mask
  @param[in] UsedMsrNum           - the used MSR number
  @param[in] UsedMemoryCacheType  - the cache type for the target memory

  @retval EFI_SUCCESS   - The memory is found in MTRR and cache type is returned
  @retval EFI_NOT_FOUND - The memory is not found in MTRR

**/
EFI_STATUS
SearchForExactMtrr (
  IN  EFI_PHYSICAL_ADDRESS      MemoryAddress,
  IN  UINT64                    MemoryLength,
  IN  UINT64                    ValidMtrrAddressMask,
  OUT UINT32                    *UsedMsrNum,
  OUT EFI_MEMORY_CACHE_TYPE     *UsedMemoryCacheType
  )
{
  UINT32  MsrNum;
  UINT64  TempQword;
  UINT32  VariableMtrrLimit;

  VariableMtrrLimit = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);

  for (MsrNum = CACHE_VARIABLE_MTRR_BASE; MsrNum < (CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit * 2 - 1); MsrNum += 2) {

    TempQword = AsmReadMsr64 (MsrNum + 1);

    if ((TempQword & B_CACHE_MTRR_VALID) == 0) {
      continue;
    }

    if ((TempQword & ValidMtrrAddressMask) != ((~(MemoryLength - 1)) & ValidMtrrAddressMask)) {
      continue;
    }

    TempQword = AsmReadMsr64 (MsrNum);

    if ((TempQword & ValidMtrrAddressMask) != (MemoryAddress & ValidMtrrAddressMask)) {
      continue;
    }

    *UsedMemoryCacheType  = (EFI_MEMORY_CACHE_TYPE) (TempQword & 0xFF);
    *UsedMsrNum           = MsrNum;

    return EFI_SUCCESS;

  }

  return EFI_NOT_FOUND;
}

/**

  Compares provided Cache type to default type

  @param[in] MemoryCacheType - Memory type for testing

  @retval TRUE  - Memory type instance is the default type
  @retval FALSE - Memory type instance is not the default type

**/
BOOLEAN
IsDefaultType (
  IN  EFI_MEMORY_CACHE_TYPE     MemoryCacheType
  )
{

  if ((AsmReadMsr64 (CACHE_IA32_MTRR_DEF_TYPE) & 0xFF) != MemoryCacheType) {
    return FALSE;
  } else {
    return TRUE;
  }
}
