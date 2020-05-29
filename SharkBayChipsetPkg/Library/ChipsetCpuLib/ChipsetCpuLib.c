/** @file
  Provide cpu version extract considering extended family & model ID.

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

#include <Library/BaseLib.h>
#include <Library/ChipsetCpuLib.h>
#include <CpuDataStruct.h>
#include <CpuRegs.h>

BOOLEAN
CheckProcessor (
  IN  CHECK_PROCESSOR_TYPE      CheckType,
  OUT UINT32                    *FamilyModel,
  OUT UINT8                     *Stepping
  )
{
  EFI_CPUID_REGISTER            CpuidRegisters; 
  UINT32                        ProcessorFamilyModel;
  UINT8                         ProcessorStepping;
  BOOLEAN                       Match;

  AsmCpuidEx (EFI_CPUID_VERSION_INFO, 0, &CpuidRegisters.RegEax,  &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);
  
  ProcessorFamilyModel = CpuidRegisters.RegEax & CPUID_NEHALEM_FAMILY_MODEL;
  if ( FamilyModel ) {
    *FamilyModel = ProcessorFamilyModel;
  }

  ProcessorStepping = ( UINT8 )( CpuidRegisters.RegEax & 0xF );
  if ( Stepping ) {
    *Stepping = ProcessorStepping;
  }

  Match = FALSE;
  
  switch ( CheckType ) {
    
    case ProcessorNehalem:
      if ( ( ProcessorFamilyModel == CPUID_FAMILY_MODEL_NEHALEM ) ) {
        Match = TRUE;
      }
      break;

    case ProcessorSandyBridgeA1:
      if ( ( ProcessorFamilyModel == CPUID_FAMILY_MODEL_NEHALEM ) ) {
        if ( ProcessorStepping == 0x0 ) {
          Match = TRUE;
        }
      }
      break;

    case ProcessorSandyBridgeB0:
      if ( ( ProcessorFamilyModel == CPUID_FAMILY_MODEL_NEHALEM ) ) {
        if ( ProcessorStepping == 0x1 ) {
          Match = TRUE;
        }
      }

    case ProcessorSandyBridgeB1:
      if ( ( ProcessorFamilyModel == CPUID_FAMILY_MODEL_NEHALEM ) ) {
        if ( ProcessorStepping == 0x1 ) {
          Match = TRUE;
        }
      }
    case ProcessorSandyBridgeB2:
      if ( ( ProcessorFamilyModel == CPUID_FAMILY_MODEL_NEHALEM ) ) {
        if ( ProcessorStepping == 0x2 ) {
          Match = TRUE;
        }
      }
    case ProcessorSandyBridgeC0:
      if ( ( ProcessorFamilyModel == CPUID_FAMILY_MODEL_NEHALEM ) ) {
        if ( ProcessorStepping == 0x3 ) {
          Match = TRUE;
        }
      }  
      break;

    default:
      break;
  }

  return Match;
}

BOOLEAN
IsNehalem (
  UINT8 *Stepping,
  UINT32 *FamilyModel  
  )
{
  UINT16              FamilyId;
  UINT8               Model;
  UINT8               SteppingId;
  EFI_CPUID_REGISTER  CpuidRegisters; 
  //
  // Check Nehalme, Bloomfield MP/DP, Hillsdale
  //
  NehalemCpuVersion (&FamilyId, &Model, &SteppingId, NULL);
  if (Stepping)  {
    *Stepping = SteppingId;
  }
  
  AsmCpuidEx (EFI_CPUID_VERSION_INFO, 0, &CpuidRegisters.RegEax,  &CpuidRegisters.RegEbx, &CpuidRegisters.RegEcx, &CpuidRegisters.RegEdx);
  *FamilyModel = CpuidRegisters.RegEax & CPUID_NEHALEM_FAMILY_MODEL;  
  if ( (*FamilyModel == CPUID_FAMILY_MODEL_NEHALEM) )  {        
    return TRUE;
  } else { 
    return FALSE;
  }    
}


/**
 Extract CPU detail version infomation

 @param [in, out] FamilyId      FamilyId, including ExtendedFamilyId
 @param [in, out] Model         Model, including ExtendedModel
 @param [in, out] SteppingId    SteppingId
 @param [in, out] Processor     Processor


**/
VOID
NehalemCpuVersion (
  IN  OUT UINT16  *FamilyId,    OPTIONAL
  IN  OUT UINT8   *Model,       OPTIONAL
  IN  OUT UINT8   *SteppingId,  OPTIONAL
  IN  OUT UINT8   *Processor    OPTIONAL
  )
{
  EFI_CPUID_REGISTER  Register; 
  UINT8              TempFamilyId;
  
  AsmCpuidEx (EFI_CPUID_VERSION_INFO, 0, &Register.RegEax,  &Register.RegEbx, &Register.RegEcx, &Register.RegEdx);
  
  if (SteppingId != NULL) {
    *SteppingId = (UINT8) (Register.RegEax & 0xF);
  }

  if (Processor != NULL) {
    *Processor = (UINT8) ((Register.RegEax >> 12) & 0x3);
  }

  if (Model != NULL || FamilyId != NULL) {
    TempFamilyId = (UINT8) ((Register.RegEax >> 8) & 0xF);
  
    if (Model != NULL) {
      *Model = (UINT8) ((Register.RegEax >> 4) & 0xF);
      if (TempFamilyId == 0x6 || TempFamilyId == 0xF) {
        *Model |= (Register.RegEax >> 12) & 0xF0;
      }
    }
  
    if (FamilyId != NULL) {
      *FamilyId = TempFamilyId;
      if (TempFamilyId == 0xF) {
        *FamilyId = *FamilyId + (UINT16) ((Register.RegEax >> 20) & 0xFF);
      }
    }
  } 
}

/**
 get cpu's thread ,Core enable  count

 @param [in, out] ThreadCount   thread count number
 @param [in, out] CoreCount     core count number

 @retval EFI_SUCCESS            The function completed successfully.
 @retval EFI_INVALID_PARAMETER  should input one and not null parmeter

**/
EFI_STATUS
GetTheadCoreCount(   
  IN OUT      UINT8   *ThreadCount, OPTIONAL 
  IN OUT      UINT8   *CoreCount    OPTIONAL    
  )
{
 
 
  if( CoreCount != NULL ){
     *CoreCount = (UINT8) ( RShiftU64(AsmReadMsr64(MSR_CORE_THREAD_COUNT), 16) & 0xFFFF  );
  }
  if (ThreadCount!=NULL) {
    *ThreadCount = (UINT8) (  AsmReadMsr64(MSR_CORE_THREAD_COUNT)  & 0xFFFF  ); 
  }
 
  if  ( ( CoreCount == NULL  ) && ( ThreadCount == NULL ) ){
    return EFI_INVALID_PARAMETER;
  }
  return EFI_SUCCESS;
 
}
/**
 check if this cpu support hyperthread

 @param None

 @retval TRUE                   This cpu support hyperthread
 @retval FALSE                  This cpu does not support hyperthread

**/
 BOOLEAN
 IsSupportHT(   
   VOID 
  )
{
  EFI_CPUID_REGISTER Register; 

  AsmCpuidEx (CPUID_CORE_TOPOLOGY, 0, &Register.RegEax,  &Register.RegEbx, &Register.RegEcx, &Register.RegEdx);
  //
  //EBX[15:0] thread number of factory-configured cpu
  //
  if ( ( Register.RegEbx & 0xFFFF ) > 1 ){
     return TRUE;
  }
  else{
     return FALSE;
  }
    
}

 UINT8
 GetThreadcountMax(   
   VOID
 )
{

  EFI_CPUID_REGISTER Register;

  AsmCpuidEx (CPUID_CORE_TOPOLOGY, 1, &Register.RegEax,  &Register.RegEbx, &Register.RegEcx, &Register.RegEdx);

  //
  //EBX[15:0] logical cp number of factory-configured cpu
  // 
  return  (UINT8)( Register.RegEbx & 0xFFFF ) ;

}
 
UINT32
GetNumberOfMtrr(   
  VOID
  )
{
  return (UINT32)((AsmReadMsr64 (IA32_MTRR_CAP) & 0xFF) - 1);
}

UINT64
GetValidMtrrAddress(   
  VOID
  )
{
  UINT64                ValidMtrrAddressMask;
  EFI_CPUID_REGISTER    FeatureInfo;	
	
  ValidMtrrAddressMask = LShiftU64((UINT64) 1, 36) & (~(UINT64)0x0FFF);

  AsmCpuid (CPUID_EXTENDED_FUNCTION, &FeatureInfo.RegEax, NULL, NULL, NULL);
  if (FeatureInfo.RegEax >= CPUID_VIR_PHY_ADDRESS_SIZE) {
    AsmCpuid (CPUID_VIR_PHY_ADDRESS_SIZE, &FeatureInfo.RegEax, NULL, NULL, NULL);
    ValidMtrrAddressMask = (LShiftU64((UINT64) 1, FeatureInfo.RegEax & 0xFF) - 1) & (~(UINT64)0x0FFF);
  }	

  return ValidMtrrAddressMask;
}

UINT64
EFIAPI
GetNextPowerOfTwo64 (
  IN UINT64                         Operand
  )
{
  if (Operand == 0) {
    return 0;
  }

  return LShiftU64 (1, (HighBitSet64 (Operand)) + 1);
}

UINT64
EFIAPI
GetOnePowerOfTwo64 (
  IN UINT64                         Operand
  )
{
  if (Operand == 0) {
    return 0;
  }

  return LShiftU64 (1, (LowBitSet64 (Operand)));
}
