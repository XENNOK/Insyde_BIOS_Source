/** @file

  EFI 2.0 PEIM to initialize the cache and load the BSP microcode

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

    Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
    This software and associated documentation (if any) is furnished
    under a license and may only be used or copied in accordance
    with the terms of the license. Except as permitted by such
    license, no part of this software or documentation may be
    reproduced, stored in a retrieval system, or transmitted in any
    form or by any means without the express written consent of
    Intel Corporation.
*/

#include "CpuInitPeim.h"
#include <CpuAccess.h>
#include <Ppi/Cache.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>

#define ALIGNED_SEED  0x01010101

INT8
CheckDirection (
  IN UINT64 Input
  );

UINT64
PeiPower2MaxMemory (
  IN UINT64 MemoryLength
  );

VOID
EfiDisableCacheMtrr (
  IN UINT64 *OldMtrr
  );

VOID
EfiRecoverCacheMtrr (
  IN BOOLEAN EnableMtrr,
  IN UINT64  OldMtrr
  );

VOID
EfiProgramMtrr (
  IN PEI_CACHE_PPI         *This,
  IN  UINT32                     MtrrNumber,
  IN EFI_PHYSICAL_ADDRESS  MemoryAddress,
  IN UINT64                MemoryLength,
  IN EFI_MEMORY_CACHE_TYPE MemoryCacheType,
  IN UINT64                ValidMtrrAddressMask
  );

EFI_STATUS
EFIAPI
PeiResetCacheAttributes (
  IN  CONST EFI_PEI_SERVICES     **PeiServices,
  IN PEI_CACHE_PPI    *This
  );

EFI_STATUS
EFIAPI
PeiActivateCache (
  IN  CONST EFI_PEI_SERVICES     **PeiServices,
  IN PEI_CACHE_PPI    *This
  );

EFI_STATUS
EFIAPI
PeiSetCacheAttributes (
  IN  CONST EFI_PEI_SERVICES         **PeiServices,
  IN PEI_CACHE_PPI         *This,
  IN EFI_PHYSICAL_ADDRESS  MemoryAddress,
  IN UINT64                MemoryLength,
  IN EFI_MEMORY_CACHE_TYPE MemoryCacheType
  );

EFI_STATUS
SearchForExactMtrr (
  IN  CONST EFI_PEI_SERVICES          **PeiServices,
  IN PEI_CACHE_PPI          *This,
  IN EFI_PHYSICAL_ADDRESS   MemoryAddress,
  IN UINT64                 MemoryLength,
  IN UINT64                 ValidMtrrAddressMask,
  OUT UINT32                *UsedMsrNum,
  OUT EFI_MEMORY_CACHE_TYPE *MemoryCacheType
  );

BOOLEAN
IsDefaultType (
  IN EFI_MEMORY_CACHE_TYPE MemoryCacheType
  );

EFI_STATUS
DisableCacheAsRam (
  VOID
  );

typedef struct _ALIGNED_DWORD {
  UINT32 High;
  UINT32 Low;
} ALIGNED_DWORD;

typedef union _ALIGNED {
  UINT64        AlignedQword;
  ALIGNED_DWORD AlignedDword;
} ALIGNED;

typedef struct {
  UINT32 Msr;
  UINT32 BaseAddress;
  UINT32 Length;
} FIXED_MTRR;

FIXED_MTRR mFixedMtrrTable[] = {
  {
    IA32_MTRR_FIX64K_00000,
    0,
    0x10000
  },
  {
    IA32_MTRR_FIX16K_80000,
    0x80000,
    0x4000
  },
  {
    IA32_MTRR_FIX16K_A0000,
    0xA0000,
    0x4000
  },
  {
    IA32_MTRR_FIX4K_C0000,
    0xC0000,
    0x1000
  },
  {
    IA32_MTRR_FIX4K_C8000,
    0xC8000,
    0x1000
  },
  {
    IA32_MTRR_FIX4K_D0000,
    0xD0000,
    0x1000
  },
  {
    IA32_MTRR_FIX4K_D8000,
    0xD8000,
    0x1000
  },
  {
    IA32_MTRR_FIX4K_E0000,
    0xE0000,
    0x1000
  },
  {
    IA32_MTRR_FIX4K_E8000,
    0xE8000,
    0x1000
  },
  {
    IA32_MTRR_FIX4K_F0000,
    0xF0000,
    0x1000
  },
  {
    IA32_MTRR_FIX4K_F8000,
    0xF8000,
    0x1000
  },
  {
    0,
    0x100000,
    0
  }
};

PEI_CACHE_PPI mCachePpi = {
  PeiSetCacheAttributes,
  PeiResetCacheAttributes,
  PeiActivateCache
};

/**
  Update MTRR setting to memory buffer

  @param[in] This                  - Current instance of Pei Cache PPI.
  @param[in] MsrNum                - offset 0-10 maps to Fixed MTRR table
                                      offset above 0x200 maps to Variable MTRR table
  @param[in] UpdateValue           - MTRR setting
  **/
VOID
WriteMsrToBuffer (
  IN PEI_CACHE_PPI *This,
  IN UINT32        MsrNum,
  IN UINT64        UpdateValue
  )
{
  CACHE_PPI_INSTANCE *CachePpiInstance;
  CachePpiInstance = PEI_CACHE_PPI_INSTANCE_FROM_THIS (This);
  if (MsrNum >= CACHE_VARIABLE_MTRR_BASE) {
    if ((MsrNum - CACHE_VARIABLE_MTRR_BASE) >= V_MAXIMUM_VARIABLE_MTRR_NUMBER * 2) {
      ASSERT (FALSE);
      return;
    }

    CachePpiInstance->VariableMtrrChanged = TRUE;
    CachePpiInstance->VariableMtrrValue[MsrNum - CACHE_VARIABLE_MTRR_BASE].Changed  = TRUE;
    CachePpiInstance->VariableMtrrValue[MsrNum - CACHE_VARIABLE_MTRR_BASE].MsrValue = UpdateValue;
  } else {
    if (MsrNum >= V_FIXED_MTRR_NUMBER) {
      ASSERT (FALSE);
      return;
    }

    CachePpiInstance->FixedMtrrChanged                = TRUE;
    CachePpiInstance->FixedMtrrValue[MsrNum].Changed  = TRUE;
    CachePpiInstance->FixedMtrrValue[MsrNum].MsrValue = UpdateValue;
  }
}

/**
  Read MTRR from Buffer. If buffer not ready, read from real MSR instead.

  @param[in] This                  - Current instance of Pei Cache PPI.
  @param[in] MsrNum                - offset 0-10 maps to Fixed MTRR table
                                     offset above 0x200 maps to Variable MTRR table

    @retval Return MTRR setting
  **/
UINT64
ReadMsrFromBuffer (
  IN PEI_CACHE_PPI *This,
  IN UINT32        MsrNum
  )
{
  UINT64             MtrrVal;
  CACHE_PPI_INSTANCE *CachePpiInstance;
  CachePpiInstance = PEI_CACHE_PPI_INSTANCE_FROM_THIS (This);
  if (MsrNum >= CACHE_VARIABLE_MTRR_BASE) {
    if ((MsrNum - CACHE_VARIABLE_MTRR_BASE) >= V_MAXIMUM_VARIABLE_MTRR_NUMBER * 2) {
      ASSERT (FALSE);
      return 0;
    }

    if (CachePpiInstance->VariableMtrrValue[MsrNum - CACHE_VARIABLE_MTRR_BASE].Changed) {
      MtrrVal = CachePpiInstance->VariableMtrrValue[MsrNum - CACHE_VARIABLE_MTRR_BASE].MsrValue;
    } else {
      MtrrVal = AsmReadMsr64 (MsrNum);
    }
  } else {
    if (MsrNum >= V_FIXED_MTRR_NUMBER) {
      ASSERT (FALSE);
      return 0;
    }

    if (CachePpiInstance->FixedMtrrValue[MsrNum].Changed) {
      MtrrVal = CachePpiInstance->FixedMtrrValue[MsrNum].MsrValue;
    } else {
      MtrrVal = AsmReadMsr64 (mFixedMtrrTable[MsrNum].Msr);
    }
  }

  return MtrrVal;
}

/**
    Disable cache and its mtrr

    @param[in] OldMtrr - To return the Old MTRR value
**/
VOID
EfiDisableCacheMtrr (
  OUT UINT64 *OldMtrr
  )
{
  UINT64 TempQword;

  AsmDisableCache ();

  ///
  /// Disable Cache MTRR
  ///
  *OldMtrr  = AsmReadMsr64 (CACHE_IA32_MTRR_DEF_TYPE);
  TempQword = (*OldMtrr) &~B_CACHE_MTRR_VALID &~B_CACHE_FIXED_MTRR_VALID;
  AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, TempQword);
  return;
}

/**
    Recover cache MTRR

    @param[in] EnableMtrr - Whether to enable the MTRR
    @param[in] OldMtrr    - The saved old MTRR value to restore when not to
                            enable the MTRR
**/
VOID
EfiRecoverCacheMtrr (
  IN BOOLEAN EnableMtrr,
  IN UINT64  OldMtrr
  )
{
  UINT64 TempQword;

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
  return;
}

/**
  Programming MTRR according to Memory address, length, and type.

  @param[in] This                 - Pointer to PEI_CACHE_PPI
  @param[in] MtrrNumber           - the variable MTRR index number
  @param[in] MemoryAddress        - the address of target memory
  @param[in] MemoryLength         - the length of target memory
  @param[in] MemoryCacheType      - the cache type of target memory
  @param[in] ValidMtrrAddressMask - the MTRR address mask
**/
VOID
EfiProgramMtrr (
  IN PEI_CACHE_PPI         *This,
  IN UINT32                MtrrNumber,
  IN EFI_PHYSICAL_ADDRESS  MemoryAddress,
  IN UINT64                MemoryLength,
  IN EFI_MEMORY_CACHE_TYPE MemoryCacheType,
  IN UINT64                ValidMtrrAddressMask
  )
{
  UINT64 TempQword;

  ///
  /// MTRR Physical Base
  ///
  TempQword = (MemoryAddress & ValidMtrrAddressMask) | MemoryCacheType;
  WriteMsrToBuffer (This, MtrrNumber, TempQword);

  ///
  /// MTRR Physical Mask
  ///
  TempQword = ~(MemoryLength - 1);
  WriteMsrToBuffer (This, MtrrNumber + 1, (TempQword & ValidMtrrAddressMask) | B_CACHE_MTRR_VALID);

  return;
}

/**
  Calculate max memory of power 2

  @param[in] MemoryLength  - Memory length that will be calculated

  @retval Max memory
**/
UINT64
PeiPower2MaxMemory (
  IN UINT64 MemoryLength
  )
{
  UINT64 Result;
  UINT32 *ResultPointer;
  UINT32 *MemoryLengthPointer;
  MemoryLengthPointer = (UINT32 *) &MemoryLength;
  ResultPointer       = (UINT32 *) &Result;
  Result              = 0;
  if (MemoryLengthPointer[1] != 0) {
    ResultPointer[1] = GetPowerOfTwo32 (MemoryLengthPointer[1]);
  } else {
    ResultPointer[0] = GetPowerOfTwo32 (MemoryLengthPointer[0]);
  }

  return Result;
}

/**
  Program the unaligned MTRR register.

  @param[in] This                  - Pointer to PEI_CACHE_PPI
  @param[in] AlignedQword          - The aligned 64-bit cache type.
  @param[in] MsrNum                - The index of current MTRR.
  @param[in] UnalignedBase         - Base Address of the current unaligned MTRR.
  @param[in] UnalignedLimit        - Limit Address of the current unaligned MTRR.

  @retval EFI_SUCCESS           - The unaligned MTRR is set successfully.
  @retval EFI_DEVICE_ERROR      - The unaligned address is not the multiple of the basic length of MTRR.
**/
EFI_STATUS
PeiProgramUnalignedMtrr (
  IN PEI_CACHE_PPI *This,
  IN UINT64        AlignedQword,
  IN UINTN         MsrNum,
  IN UINT32        UnalignedBase,
  IN UINT32        UnalignedLimit
  )
{
  UINT32 UnalignedOffset;
  UINT64 TempQword;
  UINT64 Mask;
  UINT8  ByteShift;

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

  TempQword = ReadMsrFromBuffer (This, (UINT32)MsrNum) &~Mask;
  TempQword |= AlignedQword & Mask;
  WriteMsrToBuffer (This, (UINT32)MsrNum, TempQword);
  return EFI_SUCCESS;
}

/**
  Given the low memory range ( <= 1MB) and cache type, program the MTRRs.

  @param[in] This                  - Current instance of Pei Cache PPI.
  @param[in] MemoryCacheType       - Cache Type.
  @param[in] MemoryBase            - Base Address of Memory to program MTRR.
  @param[in] MemoryLimit           - Limit Address of Memory to program MTRR.

  @retval EFI_SUCCESS           - Low memory MTRR is set successfully.
  @retval others                - An error occurs when setting Low memory MTRR.
**/
EFI_STATUS
PeiProgramLowMemoryMtrr (
  IN PEI_CACHE_PPI         *This,
  IN EFI_MEMORY_CACHE_TYPE MemoryCacheType,
  IN UINT32                MemoryBase,
  IN UINT32                MemoryLimit
  )
{
  EFI_STATUS Status;
  ALIGNED    Aligned;
  UINTN      MsrNum;

  Status                    = EFI_SUCCESS;

  Aligned.AlignedDword.High = MemoryCacheType * ALIGNED_SEED;
  Aligned.AlignedDword.Low  = Aligned.AlignedDword.High;

  for (MsrNum = 0; mFixedMtrrTable[MsrNum].BaseAddress < MemoryBase; MsrNum++) {
    ;
  }

  if (MemoryBase < mFixedMtrrTable[MsrNum].BaseAddress) {
    Status = PeiProgramUnalignedMtrr (This, Aligned.AlignedQword, MsrNum - 1, MemoryBase, MemoryLimit);
    if (EFI_ERROR (Status)) {
      goto Done;
    }
  }

  while (MsrNum < V_FIXED_MTRR_NUMBER && MemoryLimit >= mFixedMtrrTable[MsrNum + 1].BaseAddress) {
    ///
    /// Program aligned MTRR
    ///
    WriteMsrToBuffer (This, (UINT32)MsrNum, Aligned.AlignedQword);
    MsrNum++;
  }

  if (MemoryLimit > mFixedMtrrTable[MsrNum].BaseAddress) {
    Status = PeiProgramUnalignedMtrr (
                    This,
                    Aligned.AlignedQword,
                    MsrNum,
                    mFixedMtrrTable[MsrNum].BaseAddress,
                    MemoryLimit
                    );
  }

Done:
  return Status;
}

/**
  Given the memory range and cache type, programs the MTRRs.

  @param[in] PeiServices           - General purpose services available to every PEIM.
  @param[in] This                  - Current instance of Pei Cache PPI.
  @param[in] MemoryAddress         - Base Address of Memory to program MTRR.
  @param[in] MemoryLength          - Length of Memory to program MTRR.
  @param[in] MemoryCacheType       - Cache Type.

  @retval EFI_SUCCESS           - Mtrr are set successfully.
  @retval EFI_LOAD_ERROR        - No empty MTRRs to use.
  @retval EFI_INVALID_PARAMETER - The input parameter is not valid.
  @retval others                - An error occurs when setting MTTR.
**/
EFI_STATUS
EFIAPI
PeiSetCacheAttributes (
  IN CONST EFI_PEI_SERVICES          **PeiServices,
  IN PEI_CACHE_PPI         *This,
  IN EFI_PHYSICAL_ADDRESS  MemoryAddress,
  IN UINT64                MemoryLength,
  IN EFI_MEMORY_CACHE_TYPE MemoryCacheType
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
  UINT32                *TempQwordPointer;
  UINT32                *Power2LengthPointer;

  TempQwordPointer  = (UINT32 *) &TempQword;

  VariableMtrrLimit = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);
  if (VariableMtrrLimit > V_MAXIMUM_VARIABLE_MTRR_NUMBER) {
    VariableMtrrLimit = V_MAXIMUM_VARIABLE_MTRR_NUMBER;
  }
  ValidMtrrAddressMask = 0x1000000000ULL;

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
    ValidMtrrAddressMask = (LShiftU64 ((UINT64) 1, FeatureInfo.RegEax & 0xFF) - 1) & (~(UINT64) 0x0FFF);
  }

  ///
  /// Check for invalid parameter
  ///
  if ((MemoryAddress &~ValidMtrrAddressMask) != 0 || (MemoryLength &~ValidMtrrAddressMask) != 0) {
    return EFI_INVALID_PARAMETER;
  }

  switch (MemoryCacheType) {
    case EfiCacheTypeUncacheable:
    case EfiCacheTypeWriteCombining:
    case EfiCacheTypeWriteThrough:
    case EfiCacheTypeWriteProtected:
    case EfiCacheTypeWriteBack:
      break;

    default:
      return EFI_INVALID_PARAMETER;
  }

  ///
  /// Check if Fixed MTRR
  ///
  if ((MemoryAddress + MemoryLength) <= (1 << 20)) {
    Status = PeiProgramLowMemoryMtrr (
                    This,
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
    Power2Length[LengthSize] = PeiPower2MaxMemory (TempQword);
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
      Power2LengthPointer = (UINT32 *) &Power2Length[Index];
      if (Power2Length[Index] != 0) {
        if (Power2LengthPointer[1] != 0) {
          Remainder = (UINT32) TempQword;
          if (Remainder == 0) {
            DivU64x32Remainder (
                    TempQwordPointer[1],
                    Power2LengthPointer[1],
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
                    PeiServices,
                    This,
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
          WriteMsrToBuffer (This, UsedMsrNum, 0);
          WriteMsrToBuffer (This, UsedMsrNum + 1, 0);

        } else {
          ///
          /// Modify the mtrr type
          ///
          EfiProgramMtrr (
                  This,
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
      if (ReadMsrFromBuffer (This, MsrNum + 1) == 0) {
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
            This,
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
  Update MTRR setting from buffer to MSR.  Disable NEM when NEM is not disabled yet.

  @param[in] PeiServices           - General purpose services available to every PEIM.
  @param[in] This                  - Current instance of Pei Cache PPI.

  @retval EFI_SUCCESS              - Mtrr are set successfully.
**/
EFI_STATUS
EFIAPI
PeiActivateCache (
  IN CONST EFI_PEI_SERVICES          **PeiServices,
  IN PEI_CACHE_PPI    *This
  )
{
  UINT32             VariableMtrrLimit;
  UINT32             MsrNum;
  UINT64             OldMtrr;
  UINT16             Index;
  CACHE_PPI_INSTANCE *CachePpiInstance;
  CachePpiInstance  = PEI_CACHE_PPI_INSTANCE_FROM_THIS (This);

  VariableMtrrLimit = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);
  if (VariableMtrrLimit > V_MAXIMUM_VARIABLE_MTRR_NUMBER) {
    VariableMtrrLimit = V_MAXIMUM_VARIABLE_MTRR_NUMBER;
  }

  ///
  /// Disable NEM when NEM is not disabled yet
  ///
  if (!CachePpiInstance->NemDisabledDone) {
    DisableCacheAsRam ();
    CachePpiInstance->NemDisabledDone = TRUE;
  }

  ///
  /// Disable/Enable cache only when MTRR configuration is changed in MTRR buffer
  ///
  if (CachePpiInstance->FixedMtrrChanged || CachePpiInstance->VariableMtrrChanged) {
    EfiDisableCacheMtrr (&OldMtrr);
    if (CachePpiInstance->FixedMtrrChanged) {
      for (Index = 0; Index < V_FIXED_MTRR_NUMBER; Index++) {
        if (CachePpiInstance->FixedMtrrValue[Index].Changed) {
          AsmWriteMsr64 (mFixedMtrrTable[Index].Msr, CachePpiInstance->FixedMtrrValue[Index].MsrValue);
          CachePpiInstance->FixedMtrrValue[Index].Changed = FALSE;
        }
      }

      CachePpiInstance->FixedMtrrChanged = FALSE;
    }

    if (CachePpiInstance->VariableMtrrChanged) {
      for (MsrNum = CACHE_VARIABLE_MTRR_BASE; MsrNum < (CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit * 2); MsrNum++) {
        if (CachePpiInstance->VariableMtrrValue[MsrNum - CACHE_VARIABLE_MTRR_BASE].Changed) {
          AsmWriteMsr64 (MsrNum, CachePpiInstance->VariableMtrrValue[MsrNum - CACHE_VARIABLE_MTRR_BASE].MsrValue);
          CachePpiInstance->VariableMtrrValue[MsrNum - CACHE_VARIABLE_MTRR_BASE].Changed = FALSE;
        }

        CachePpiInstance->VariableMtrrChanged = FALSE;
      }
    }

    EfiRecoverCacheMtrr (TRUE, OldMtrr);
  }

  return EFI_SUCCESS;
}

/**
    Reset all the MTRRs to a known state.

    @param[in] PeiServices - General purpose services available to every PEIM.
    @param[in] This        - Pointer to the instance of the PEI_CACHE_PPI.

    @retval EFI_SUCCESS - All MTRRs have been reset successfully.
**/
EFI_STATUS
EFIAPI
PeiResetCacheAttributes (
  IN CONST EFI_PEI_SERVICES          **PeiServices,
  IN PEI_CACHE_PPI    *This
  )
{
  UINT32 MsrNum;
  UINT16 Index;
  UINT32 VariableMtrrLimit;

  VariableMtrrLimit = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);
  if (VariableMtrrLimit > V_MAXIMUM_VARIABLE_MTRR_NUMBER) {
    VariableMtrrLimit = V_MAXIMUM_VARIABLE_MTRR_NUMBER;
  }

  Index = 0;

  ///
  /// Reset Fixed Mtrrs
  ///
  while (mFixedMtrrTable[Index].Msr != 0) {
    WriteMsrToBuffer (This, Index, 0);
    Index++;
  }

  ///
  /// Reset Variable Mtrrs
  ///
  for (MsrNum = CACHE_VARIABLE_MTRR_BASE; MsrNum < (CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit * 2); MsrNum++) {
    WriteMsrToBuffer (This, MsrNum, 0);
  }

  return EFI_SUCCESS;
}

/**
  Search the memory cache type for specific memory from MTRR.

  @param[in] PeiServices          - General purpose services available to every PEIM.
  @param[in] This                 - Current instance of Pei Cache PPI.
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
  IN  CONST EFI_PEI_SERVICES          **PeiServices,
  IN PEI_CACHE_PPI          *This,
  IN EFI_PHYSICAL_ADDRESS   MemoryAddress,
  IN UINT64                 MemoryLength,
  IN UINT64                 ValidMtrrAddressMask,
  OUT UINT32                *UsedMsrNum,
  OUT EFI_MEMORY_CACHE_TYPE *UsedMemoryCacheType
  )
{
  UINT32 MsrNum;
  UINT64 TempQword;
  UINT32 VariableMtrrLimit;

  VariableMtrrLimit = (UINT32) (AsmReadMsr64 (IA32_MTRR_CAP) & B_IA32_MTRR_VARIABLE_SUPPORT);
  if (VariableMtrrLimit > V_MAXIMUM_VARIABLE_MTRR_NUMBER) {
    VariableMtrrLimit = V_MAXIMUM_VARIABLE_MTRR_NUMBER;
  }

  for (MsrNum = CACHE_VARIABLE_MTRR_BASE; MsrNum < (CACHE_VARIABLE_MTRR_BASE + VariableMtrrLimit * 2 - 1); MsrNum += 2) {

    TempQword = ReadMsrFromBuffer (This, MsrNum + 1);

    if ((TempQword & B_CACHE_MTRR_VALID) == 0) {
      continue;
    }

    if ((TempQword & ValidMtrrAddressMask) != ((~(MemoryLength - 1)) & ValidMtrrAddressMask)) {
      continue;
    }

    TempQword = ReadMsrFromBuffer (This, MsrNum);

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
  IN EFI_MEMORY_CACHE_TYPE MemoryCacheType
  )
{

  if ((AsmReadMsr64 (CACHE_IA32_MTRR_DEF_TYPE) & 0xFF) != MemoryCacheType) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Install CacheInitPpi

  @retval EFI_OUT_OF_RESOURCES - failed to allocate required pool
**/
EFI_STATUS
CacheInitPpiInit (
  VOID
  )
{
  EFI_STATUS         Status;
  CACHE_PPI_INSTANCE *CachePpiInstance;

  CachePpiInstance = AllocateZeroPool (sizeof (CACHE_PPI_INSTANCE));
  ASSERT (CachePpiInstance != NULL);
  if (CachePpiInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CachePpiInstance->Ppi.SetCache      = PeiSetCacheAttributes;
  CachePpiInstance->Ppi.ResetCache    = PeiResetCacheAttributes;
  CachePpiInstance->Ppi.ActivateCache = PeiActivateCache;

  CachePpiInstance->PpiDesc.Flags     = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  CachePpiInstance->PpiDesc.Guid      = &gPeiCachePpiGuid;
  CachePpiInstance->PpiDesc.Ppi       = &CachePpiInstance->Ppi;

  ///
  /// Install PPI
  ///
  Status = PeiServicesInstallPpi (&CachePpiInstance->PpiDesc);
  ASSERT_EFI_ERROR (Status);
  return Status;
}

VOID
CacheInvd (
  VOID
  );

/**
  Disable NEM (cache-as-ram)

  @retval EFI_SUCCESS - always return success
**/
EFI_STATUS
DisableCacheAsRam (
  VOID
  )
{
  UINT64 CacheAsRamMsr;
  UINT64 McStatus;
  UINT32 McIndex;
  UINT32 McCounter;
  UINT64 TempQword;
  UINT64 OldMtrr;

  CacheAsRamMsr = AsmReadMsr64 (NO_EVICT_MODE);

  ///
  /// Check if CAR has already been disabled. We should not
  /// execute CacheInvd() after cache has been enabled. This
  /// check will avoid that.
  ///
  if ((CacheAsRamMsr & B_NO_EVICT_MODE_RUN) == 0) {
    return EFI_SUCCESS;
  }

  CacheInvd ();

  ///
  /// Step 3: Disable No-Eviction Mode Run State by clearing
  ///         NO_EVICT_MODE MSR 2E0h bit [1] = 0
  ///
  CacheAsRamMsr &= ~B_NO_EVICT_MODE_RUN;
  AsmWriteMsr64 (NO_EVICT_MODE, CacheAsRamMsr);

  ///
  /// Step 4: Disable No-Eviction Mode Setup State by clearing
  ///         NO_EVICT_MODE MSR 2E0h bit [0] = 0
  ///
  CacheAsRamMsr &= ~B_NO_EVICT_MODE_SETUP;
  AsmWriteMsr64 (NO_EVICT_MODE, CacheAsRamMsr);

  ///
  /// Disable Cache MTRR by cleaning IA32_MTRR_DEF_TYPE.E or IA32_MTRR_DEF_TYPE.GE
  ///
  OldMtrr   = AsmReadMsr64 (CACHE_IA32_MTRR_DEF_TYPE);
  TempQword = OldMtrr &~B_CACHE_MTRR_VALID;
  AsmWriteMsr64 (CACHE_IA32_MTRR_DEF_TYPE, TempQword);

  ///
  /// After NEM is disabled, BIOS must clear any Machine Check Bank 5-8 errors that may
  /// have occurred as the result of ... MLC to to LLC Evictions.
  ///
  McStatus  = 0;
  McCounter = (UINT32) (AsmReadMsr64 (IA32_MCG_CAP) & 0x0f);
  for (McIndex = 5; McIndex < McCounter; McIndex++) {
    if (McIndex <= 8) {
      AsmWriteMsr64 (IA32_MC0_STATUS + McIndex * 4, McStatus);
    }
  }

  return EFI_SUCCESS;
}
