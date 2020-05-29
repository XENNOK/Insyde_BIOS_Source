/** @file

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

#include <PlatformInfo.h>

CPU_INFO_DISPLAY_TABLE        mDisplayCpuInfoFunction[]       = {CPU_INFO_DISPLAY_TABLE_LIST};
CPU_CAPABILITY_DISPLAY_TABLE  mDisplayCpuCapabilityFunction[] = {CPU_CAPABILITY_DISPLAY_TABLE_LIST};

//[-start-120529-IB10540007-add]//
CACHE_INFO_SIZE_TABLE         mCacheTable[10];
//[-end-120529-IB10540007-add]//

#define IS_DATA_RECORD_FOR_CPU_INFO     (DataHeader->RecordType == ProcessorEnabledCoreCountRecordType) || \
                                        (DataHeader->RecordType == ProcessorThreadCountRecordType)      || \
                                        (DataHeader->RecordType == CpuUcodeRevisionDataRecordType)      || \
                                        (DataHeader->RecordType == ProcessorIdRecordType)

//[-start-121025-IB10370026-add]//
STEPPING_STRING_DEFINITION          CpuSteppingRevString[] = {
                                      {HSW_STEP_A0, L"A0"},
                                      {HSW_STEP_B0, L"B0"},
                                      {HSW_STEP_C0, L"C0"},
                                      {HSW_STEP_D0, L"D0"},
                                      {0xFF, L""}
                                    };

STEPPING_STRING_DEFINITION          ULTCpuSteppingRevString[] = {
                                      {HSW_ULT_STEP_B0, L"B0"},
                                      {HSW_ULT_STEP_C0, L"C0"},
                                      {0xFF, L""}
                                    };
//[-end-121025-IB10370026-add]//

static BOOLEAN DisplayMicroCodeStatus = FALSE;

//[-start-121030-IB10370026-add]//
static BOOLEAN DisplayCpuSteppingStatus = FALSE;
//[-end-121030-IB10370026-add]//

EFI_STATUS
GetCpuMiscInfo (
  IN   VOID                       *OpCodeHandle,
  IN   EFI_HII_HANDLE             MainHiiHandle,
  IN   EFI_HII_HANDLE             AdvanceHiiHandle,
  IN   CHAR16                     *StringBuffer
  )
{
  EFI_STATUS                    Status;
  UINTN                         Index;
  UINT64                        MonotonicCount;
  EFI_DATA_RECORD_HEADER        *Record;
  EFI_SUBCLASS_TYPE1_HEADER     *DataHeader;
  EFI_CPUID_REGISTER            CpuIdRegisters;
  EFI_DATA_HUB_PROTOCOL         *DataHub;
  EFI_GUID                      ProcessorSubClass = EFI_PROCESSOR_SUBCLASS_GUID;

  Status         = EFI_SUCCESS;
  DataHub        = NULL;
  Record         = NULL;
  DataHeader     = NULL;
  MonotonicCount = 0;

  Status = gBS->LocateProtocol (
                &gEfiDataHubProtocolGuid,
                NULL,
                (VOID **)&DataHub
                );
  
  if (EFI_ERROR (Status) || DataHub == NULL) {
    goto ExecuteCpuCapability;
  }

//[-start-130701-IB05400425-add]//
  //
  // Reset status flag so that we can update these information every time
  // entering Setup Utility.
  //
  DisplayMicroCodeStatus = FALSE;
  DisplayCpuSteppingStatus = FALSE;
//[-end-130701-IB05400425-add]//
  
  //
  // Get Processor info from DataHub
  //
  do {
    Status = DataHub->GetNextRecord (DataHub, &MonotonicCount, NULL, &Record);
    DataHeader  = (EFI_SUBCLASS_TYPE1_HEADER*) (Record + 1);
    if ((Record->DataRecordClass == EFI_DATA_RECORD_CLASS_DATA) && 
       (CompareGuid (&Record->DataRecordGuid, &ProcessorSubClass)) && (IS_DATA_RECORD_FOR_CPU_INFO)) {
      for (Index = 0; mDisplayCpuInfoFunction[Index].DisplayCpuInfoFunction != NULL; Index++) {
        if (mDisplayCpuInfoFunction[Index].Option == DISPLAY_ENABLE) {
          ZeroMem(StringBuffer, 0x100);
          mDisplayCpuInfoFunction[Index].DisplayCpuInfoFunction (OpCodeHandle, MainHiiHandle, AdvanceHiiHandle, StringBuffer, DataHeader);
        }
      }
    }
  } while (!EFI_ERROR(Status) && (MonotonicCount != 0));
  

ExecuteCpuCapability:
  //
  // Check if the processor supports VT-x, VT-x and TXT.
  //
  AsmCpuid (CPUID_VERSION_INFO, &CpuIdRegisters.RegEax, &CpuIdRegisters.RegEbx, &CpuIdRegisters.RegEcx, &CpuIdRegisters.RegEdx );

  for (Index = 0; mDisplayCpuCapabilityFunction[Index].DisplayCpuCapabilityFunction != NULL; Index++) {
    if (mDisplayCpuCapabilityFunction[Index].Option == DISPLAY_ENABLE) {
      ZeroMem(StringBuffer, 0x100);
      mDisplayCpuCapabilityFunction[Index].DisplayCpuCapabilityFunction (OpCodeHandle, MainHiiHandle, AdvanceHiiHandle, StringBuffer, CpuIdRegisters);
    }
  }

  return Status;
}


EFI_STATUS
GetCpuIdFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  )
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  STRING_REF     CpuIdText;
  STRING_REF     CpuIdString;

  if (DataHeader->RecordType != ProcessorIdRecordType) {
    return Status;
  }

  UnicodeSPrint ( StringBuffer, 0x100, L"0x%x", ( ( EFI_CPU_VARIABLE_RECORD_PLUS * )( DataHeader + 1 ) )->EfiCpuVariableRecord.ProcessorId.Signature );

  CpuIdString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL); 
//[-start-120911-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_CPUID_TEXT), NULL);
//  
//  CpuIdText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL); 
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_CPUID_TEXT),
    MainHiiHandle,
    &CpuIdText
    );
//[-end-120911-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle,CpuIdText, 0, CpuIdString );
  
  return Status;
}


//[-start-120523-IB10540007-add]//
EFI_STATUS
GetCpuSpeedFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  )
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  STRING_REF     CpuSpeedText;
  STRING_REF     CpuSpeedString;
  UINT64         MsrValue;
  UINTN          MaxTurbo;
  UINTN          CpuSpeed;

  if (DataHeader->RecordType != ProcessorEnabledCoreCountRecordType) {
    return Status;
  }

  // Read PLATFORM_INFO MSR
  MsrValue = AsmReadMsr64 (0x000000CE);
  
  // BIT15~BIT8 is Maximum Non Turbo Ratio (RO)
  MaxTurbo = (UINTN) ((MsrValue & 0x0000ff00) >> 8);

  // Maximum_Non_Turbo_Ratio * Maximum_Efficiency_Ratio_Unit
  CpuSpeed = MaxTurbo * MAXIMUN_EFFICIENCY_RATIO;
  
  UnicodeSPrint (StringBuffer, 0x100, L"%d MHz", CpuSpeed);

  CpuSpeedString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  
//[-start-121002-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_CPU_SPEED_TEXT), NULL);
//  
//  CpuSpeedText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_CPU_SPEED_TEXT),
    MainHiiHandle,
    &CpuSpeedText
    );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle, CpuSpeedText, 0, CpuSpeedString);

  return Status;
}
//[-end-120523-IB10540007-add]//


//[-start-120528-IB10540007-add]//
EFI_STATUS
GetCacheInfo (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  )
{
  EFI_CPUID_REGISTER   CpuReg;
  UINT8                CacheType;
  UINT8                Num;
  UINT32               Way;
  UINT32               Partition;
  UINT32               LineSize;
  UINT32               Set;
  UINT32               Eax = 0x04;
  UINT32               Ecx = 0x00;
  UINTN                Index;

  AsmCpuidEx(Eax, Ecx, &CpuReg.RegEax, &CpuReg.RegEbx, &CpuReg.RegEcx, &CpuReg.RegEdx);
  //
  // EAX[4:0] Cache Type
  //
  CacheType = (UINT8)(CpuReg.RegEax & 0x0000001f);

  //
  //EAX[4:0] = 0, no more caches
  //
  for (Index = 0; CacheType != 0 && Index < 10; Index++) {
    //
    // EAX[31:26] Number of APIC IDs reserved for the package;
    //
    Num = (UINT8)((CpuReg.RegEax >> 26) & 0x0000000f) + 1;

    //
    // EBX[31:22] Ways of Associativity
    //
    Way = (UINT32)((CpuReg.RegEbx >> 22) & 0x0000003f) + 1;

    //
    // EBX[21:12] Physical Line partitions
    //
    Partition = (UINT32)((CpuReg.RegEbx >> 12) & 0x0000003f) + 1;

    //
    // EBX[11:0] System Coherency Line Size
    //
    LineSize = (UINT32)(CpuReg.RegEbx & 0x0000003f) + 1;;

    //
    // ECX[31:0] Number of Sets
    //
    Set = CpuReg.RegEcx + 1;

    //
    // EAX[7:5] Cache Level
    //
    mCacheTable[Index].Level = (CpuReg.RegEax & 0x000000e0) >> 5;
    
    mCacheTable[Index].Type = CacheType;
    mCacheTable[Index].CacheSize = Way * Partition * LineSize * Set;
    
    Ecx++;
    Eax = 0x04;
    AsmCpuidEx(Eax, Ecx, &CpuReg.RegEax, &CpuReg.RegEbx, &CpuReg.RegEcx, &CpuReg.RegEdx);
    CacheType = (UINT8)(CpuReg.RegEax & 0x0000001f);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetL1DataCache (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  )
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  STRING_REF     DataCacheText;
  STRING_REF     DataCacheString;
  UINTN          Size = 0;
  UINTN          CacheNum;

  if (DataHeader->RecordType != ProcessorEnabledCoreCountRecordType) {
    return Status;
  }

  for (CacheNum = 0; CacheNum < 10; CacheNum++) {
    //
    // Data Cache Type is 1 represented L1 Data Cache
    //
    if (mCacheTable[CacheNum].Level == 1 && mCacheTable[CacheNum].Type == 1) {
      Size = mCacheTable[CacheNum].CacheSize;
      break;
    }
  }

  UnicodeSPrint (StringBuffer, 0x100, L"%d KB", Size/1024);
  
  DataCacheString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  
//[-start-121002-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_DATA_CACHE_TEXT), NULL);
//  
//  DataCacheText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_DATA_CACHE_TEXT),
    MainHiiHandle,
    &DataCacheText
    );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle, DataCacheText, 0, DataCacheString);

  return Status;
}


EFI_STATUS
GetL1Insruction (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  )
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  STRING_REF     InstructionCacheText;
  STRING_REF     InstructionString;
  UINTN          Size = 0;
  UINTN          CacheNum;

  if (DataHeader->RecordType != ProcessorEnabledCoreCountRecordType) {
    return Status;
  }

  for (CacheNum = 0; CacheNum < 10; CacheNum++) {
    //
    // Data Cache Type is 2 represented L1 Instruction Cache
    //
    if (mCacheTable[CacheNum].Level == 1 && mCacheTable[CacheNum].Type == 2) {
      Size = mCacheTable[CacheNum].CacheSize;
      break;
    }
  }

  UnicodeSPrint (StringBuffer, 0x100, L"%d KB", Size/1024);

  InstructionString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  
//[-start-121002-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_INSTRUCTION_CACHE_TEXT), NULL);
//  
//  InstructionCacheText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_INSTRUCTION_CACHE_TEXT),
    MainHiiHandle,
    &InstructionCacheText
    );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle, InstructionCacheText, 0, InstructionString);

  return Status;
}


EFI_STATUS
GetL2Cache (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  )
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  STRING_REF     L2CacheText;
  STRING_REF     L2CacheString;
  UINTN          Size = 0;
  UINTN          CacheNum;

  if (DataHeader->RecordType != ProcessorEnabledCoreCountRecordType) {
    return Status;
  }
  
  for (CacheNum = 0; CacheNum < 10; CacheNum++) {
    if (mCacheTable[CacheNum].Level == 2 && mCacheTable[CacheNum].CacheSize != 0) {
      Size = mCacheTable[CacheNum].CacheSize;
      break;
    }
  }

  UnicodeSPrint (StringBuffer, 0x100, L"%d KB", Size/1024);

  L2CacheString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  
//[-start-121002-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_L2_CACHE_TEXT), NULL);
//  
//  L2CacheText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_L2_CACHE_TEXT),
    MainHiiHandle,
    &L2CacheText
    );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle, L2CacheText, 0, L2CacheString);

  return Status;
}


EFI_STATUS
GetL3Cache (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  )
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  STRING_REF     L3CacheText;
  STRING_REF     L3CacheString;
  UINTN          Size = 0;
  UINTN          CacheNum;

  if (DataHeader->RecordType != ProcessorEnabledCoreCountRecordType) {
    return Status;
  }
  
  for (CacheNum = 0; CacheNum < 10; CacheNum++) {
    if (mCacheTable[CacheNum].Level == 3 && mCacheTable[CacheNum].CacheSize != 0) {
      Size = mCacheTable[CacheNum].CacheSize;
      break;
    }
  }

  UnicodeSPrint (StringBuffer, 0x100, L"%d KB", Size/1024);

  L3CacheString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL); 
  
//[-start-121002-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_L3_CACHE_TEXT), NULL);
//
//  L3CacheText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);  
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_L3_CACHE_TEXT),
    MainHiiHandle,
    &L3CacheText
    );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle, L3CacheText, 0, L3CacheString);

  return Status;
}
//[-end-120528-IB10540007-add]//

//[-start-121022-IB10540014-modify]//
//[-start-120620-IB10540011-modify]//
EFI_STATUS
GetCoreNumFunc (
  IN     VOID                       *OpCodeHandle,
  IN     EFI_HII_HANDLE             MainHiiHandle,
  IN     EFI_HII_HANDLE             AdvanceHiiHandle,
  IN     CHAR16                     *StringBuffer,
  IN     EFI_SUBCLASS_TYPE1_HEADER  *DataHeader
)
//[-end-120620-IB10540011-modify]//
//[-end-121022-IB10540014-modify]//
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  UINT8          NumOfCore;
  STRING_REF     CpuNumText;
  STRING_REF     CpuNumString;
//[-start-121022-IB10540014-remove]//
////[-start-120620-IB10540011-add]//
//  UINT8          NumOfThread;
////[-end-120620-IB10540011-add]//
//[-end-121022-IB10540014-remove]//

  if (DataHeader->RecordType != ProcessorEnabledCoreCountRecordType) {
    return Status;
  }

  NumOfCore = ( ( EFI_CPU_VARIABLE_RECORD_PLUS * )( DataHeader + 1 ) )->EfiCpuVariableRecord.ProcessorEnabledCoreCount;
//[-start-121022-IB10540014-remove]//
////[-start-120605-IB10540008-add]//
//  NumOfThread = ( ( EFI_CPU_VARIABLE_RECORD_PLUS * )( DataHeader + 1 ) )->EfiCpuVariableRecord.ProcessorThreadCount;
////[-end-120605-IB10540008-add]//
//[-end-121022-IB10540014-remove]//

//[-start-121022-IB10540014-modify]//
//[-start-120620-IB10540011-modify]//
  UnicodeSPrint (StringBuffer, 0x100, L"%x Core(s)", (UINTN)NumOfCore);
//[-end-120620-IB10540011-modify]//
//[-end-121022-IB10540014-modify]//

  CpuNumString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);

//[-start-121002-IB06460449-modify]//
//[-start-120620-IB10540011-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_NUM_OF_CORE_THREAD_TEXT), NULL);
//[-end-120620-IB10540011-modify]//
//  
//  CpuNumText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
//[-start-121022-IB10540014-modify]//
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_NUM_OF_CORE_TEXT),
    MainHiiHandle,
    &CpuNumText
    );
//[-end-121022-IB10540014-modify]//
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle,CpuNumText, 0, CpuNumString );
  
  return Status;
}


//[-start-121019-IB10540014-modify]//
//[-start-120620-IB10540011-remove]//
EFI_STATUS
GetThreadNumFunc (
  IN    VOID                      *OpCodeHandle,
  IN     EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  )
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  UINT8          NumOfThread;
  STRING_REF     CpuNumOfThreadText;
  STRING_REF     CpuNumOfThreadString;

  if (DataHeader->RecordType != ProcessorThreadCountRecordType) {
    return Status;
  }
  
  NumOfThread = ( ( EFI_CPU_VARIABLE_RECORD_PLUS * )( DataHeader + 1 ) )->EfiCpuVariableRecord.ProcessorThreadCount;
  UnicodeSPrint (StringBuffer, 0x100, L"%x Thread(s)", (UINTN)NumOfThread);

  CpuNumOfThreadString = HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);

  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_NUM_OF_THREAD_TEXT),
    MainHiiHandle,
    &CpuNumOfThreadText
    );

  HiiCreateTextOpCode (OpCodeHandle, CpuNumOfThreadText, 0, CpuNumOfThreadString );
  
  return Status;
}
//[-end-120620-IB10540011-remove]//
//[-end-121019-IB10540014-modify]//


EFI_STATUS
GetMicrocodeVersion (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  )
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  UINT8          uCcodeVersion;
  STRING_REF     MicroCodeVersionText;
  STRING_REF     MicroCodeVersionString;

  if (DisplayMicroCodeStatus == TRUE) {
    return Status;
  }

  if (DataHeader->RecordType != CpuUcodeRevisionDataRecordType) {
    return Status;
  }

  uCcodeVersion = ( UINT8 )( ( EFI_CPU_VARIABLE_RECORD_PLUS * )( DataHeader + 1 ) )->EfiCpuVariableRecord.CpuUcodeRevisionData.ProcessorMicrocodeRevisionNumber;
  UnicodeSPrint (StringBuffer, 0x100, L"0x%02x", (UINTN)uCcodeVersion);

  MicroCodeVersionString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  
//[-start-121002-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_MICROCODE_UPD_VERSION_TEXT), NULL);
//  MicroCodeVersionText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_MICROCODE_UPD_VERSION_TEXT),
    MainHiiHandle,
    &MicroCodeVersionText
    );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle,MicroCodeVersionText, 0, MicroCodeVersionString );
  
  DisplayMicroCodeStatus = TRUE;
  
  return Status;
}


EFI_STATUS
GetTxtCapability (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_CPUID_REGISTER        CpuIdRegisters
  )
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  STRING_REF     TxtCapabilityText;
  STRING_REF     TxtCapabilityString;
  
  if ((CpuIdRegisters.RegEcx & B_CPUID_VERSION_INFO_ECX_SME) != 0) {
    //
    // TXT is supported.
    //
    UnicodeSPrint (StringBuffer, 0x100, L"Supported");
  } else {
    //
    // TXT isn't supported.
    //
    UnicodeSPrint (StringBuffer, 0x100, L"Un-Supported");
  }

  TxtCapabilityString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
//[-start-121002-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_SMX_TEXT), NULL);
//  
//  TxtCapabilityText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_SMX_TEXT),
    MainHiiHandle,
    &TxtCapabilityText
    );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle,TxtCapabilityText, 0, TxtCapabilityString );
  
  return Status;
}


EFI_STATUS
GetVtdCapability (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_CPUID_REGISTER        CpuIdRegisters
  )
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  STRING_REF     VtdCapabilityText;
  STRING_REF     VtdCapabilityString;
  
//[-start-120404-IB05300309-modify]//
  //
  // Please refer to SNB SA v1.1.1 Ch14.1 on how to determine if SA supported VT-d
  //
  if ((McD0PciCfg32 (R_SA_MC_CAPID0_A_OFFSET) & BIT23) == 0) {
    //
    // VT-d is supported.
    //
    UnicodeSPrint (StringBuffer, 0x100, L"Supported");
  } else {
    //
    // VT-d isn't supported.
    //
    UnicodeSPrint (StringBuffer, 0x100, L"Un-Supported");
  }
//[-end-120404-IB05300309-modify]//

  VtdCapabilityString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
//[-start-121002-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_VTD_TEXT), NULL);
//  
//  VtdCapabilityText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_VTD_TEXT),
    MainHiiHandle,
    &VtdCapabilityText
    );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle,VtdCapabilityText, 0, VtdCapabilityString );
  
  return Status;
}


EFI_STATUS
GetVtxCapability (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_CPUID_REGISTER        CpuIdRegisters
  )
{
  EFI_STATUS     Status = EFI_UNSUPPORTED;
  STRING_REF     VtxCapabilityText;
  STRING_REF     VtxCapabilityString;
  if ((CpuIdRegisters.RegEcx & B_CPUID_VERSION_INFO_ECX_VME) != 0) {
    //
    // VT-x is supported.
    //
    UnicodeSPrint (StringBuffer, 0x100, L"Supported");
  } else {
    //
    // VT-x isn't supported.
    //
    UnicodeSPrint (StringBuffer, 0x100, L"Un-Supported");
  }

  VtxCapabilityString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
//[-start-121002-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_VMX_TEXT), NULL);
//  
//  VtxCapabilityText=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_VMX_TEXT),
    MainHiiHandle,
    &VtxCapabilityText
    );
//[-end-121002-IB06460449-modify]//
  
  HiiCreateTextOpCode (OpCodeHandle,VtxCapabilityText, 0, VtxCapabilityString );
  
  return Status;
}

//[-start-121024-IB10370026-add]//
EFI_STATUS
GetCpuSteppingFunc (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer,
  IN    EFI_SUBCLASS_TYPE1_HEADER *DataHeader
  )
{
  EFI_STATUS          Status = EFI_SUCCESS;
  UINTN               Index;
  STRING_REF          CpuSteppingText;
  STRING_REF          CpuSteppingIdString;
  EFI_CPUID_REGISTER  Cpuid;

  if (DisplayCpuSteppingStatus == TRUE) {
    return Status;
  }

  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx );

  if ((Cpuid.RegEax & 0xFFFF0)== 0x000306C0 ) {
    for (Index = 0; CpuSteppingRevString[Index].ReversionValue != 0xFF; Index++) {
      if ((Cpuid.RegEax & 0x0F) == CpuSteppingRevString[Index].ReversionValue) {
        UnicodeSPrint ( StringBuffer, 0x100, L"%02x (%s Stepping)", (UINTN)(Cpuid.RegEax & 0x0F), CpuSteppingRevString[Index].SteppingString);
        break;
      }
    }
  } else if ((Cpuid.RegEax & 0xFFFF0) == 0x00040650 ){
    //
    // ULT platform
    //
    for (Index = 0; ULTCpuSteppingRevString[Index].ReversionValue != 0xFF; Index++) {
      if ((Cpuid.RegEax & 0x0F) == ULTCpuSteppingRevString[Index].ReversionValue) {
        UnicodeSPrint (StringBuffer, 0x100, L"%02x (%s Stepping)", (UINTN)(Cpuid.RegEax & 0x0F), ULTCpuSteppingRevString[Index].SteppingString);
        break;
      }
    }
  }

  CpuSteppingIdString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL);
  
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_CPU_STEPPING_TEXT),
    MainHiiHandle,
    &CpuSteppingText
    );
  
  HiiCreateTextOpCode (OpCodeHandle,CpuSteppingText, 0, CpuSteppingIdString );

  DisplayCpuSteppingStatus = TRUE;

  return Status;

}
//[-end-121024-IB10370026-add]//
