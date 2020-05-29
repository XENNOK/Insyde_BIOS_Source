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

#include <SetupUtility.h>
#include <CpuAccess.h>
#include <PowerMgmtDefinitions.h>

STATIC UINT8                      mProcessorPowerUnit = 0;
STATIC UINT8                      mProcessorTimeUnit = 0;

VOID
DisplayMaxPowerLimit (
  IN  EFI_HII_HANDLE              HiiHandle
  );

VOID
DisplayMinPowerLimit (
  IN  EFI_HII_HANDLE              HiiHandle
  );

VOID
DisplayAdditionalcTDPLevels (
  IN  EFI_HII_HANDLE              HiiHandle
  );

VOID
DisplaycTDPNominal (
  IN  EFI_HII_HANDLE              HiiHandle
  );

VOID
DisplaycTDPLevel1 (
  IN  EFI_HII_HANDLE              HiiHandle
  );

VOID
DisplaycTDPLevel2 (
  IN  EFI_HII_HANDLE              HiiHandle
  );

VOID
DisplaycTDPControl (
  IN  EFI_HII_HANDLE              HiiHandle
  );

VOID
DisplaycTurboRatio (
  IN  EFI_HII_HANDLE              HiiHandle
  );

VOID
DisplayLongTermPowerLimit (
  IN  EFI_HII_HANDLE              HiiHandle
  );

VOID
DisplayShortTermPowerLimit (
  IN  EFI_HII_HANDLE              HiiHandle
  );

EFI_STATUS
CTDPDataInit (
  IN  EFI_HII_HANDLE              HiiHandle
  )
{
  UINT64                          MSRData;
  
  MSRData = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU_UNIT);
  mProcessorPowerUnit = (UINT8)(MSRData & PACKAGE_POWER_UNIT_MASK);
  if (mProcessorPowerUnit == 0) {
    mProcessorPowerUnit = 1;
  } else {
    mProcessorPowerUnit = (UINT8) LShiftU64 (2, (mProcessorPowerUnit - 1));
  }  
  mProcessorTimeUnit  = (UINT8) RShiftU64((MSRData & PACKAGE_TIME_UNIT_MASK), 16);
  if (mProcessorTimeUnit == 0) {
    mProcessorTimeUnit = 1;
  } else {
    mProcessorTimeUnit = (UINT8) LShiftU64 (2, (mProcessorTimeUnit - 1));
  }

  DisplayMaxPowerLimit (HiiHandle);
  DisplayMinPowerLimit (HiiHandle);
  DisplayAdditionalcTDPLevels (HiiHandle);
  DisplaycTDPNominal (HiiHandle);
  DisplaycTDPLevel1 (HiiHandle);
  DisplaycTDPLevel2 (HiiHandle);
  DisplaycTDPControl (HiiHandle);
  DisplaycTurboRatio (HiiHandle);
  DisplayLongTermPowerLimit (HiiHandle);
  DisplayShortTermPowerLimit (HiiHandle);
  return EFI_SUCCESS;
}

VOID
DisplayMaxPowerLimit (
  IN  EFI_HII_HANDLE              HiiHandle
  )
{
  CHAR16                          *StringBuffer;
  UINT64                          MSRData;
  UINT32                          TempData;  
  UINT16                          MaxPowerLimit;

  StringBuffer = AllocateZeroPool (0x100);
//[-start-130207-IB10870073-add]//
  ASSERT (StringBuffer != NULL);
  if (StringBuffer == NULL) {
    return;
  }
//[-end-130207-IB10870073-add]//

  MSRData = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU);
  TempData = (UINT32) RShiftU64 (MSRData, 32);
  MaxPowerLimit = TempData & PACKAGE_MAX_POWER_MASK;
  if (MaxPowerLimit == 0) {
    MaxPowerLimit = (UINT16)(LShiftU64 (2, 14) - 1);
  }
  
  UnicodeSPrint (
    StringBuffer,
    0x100,
    L"%d.%d",
    MaxPowerLimit / mProcessorPowerUnit,
    ((MaxPowerLimit % mProcessorPowerUnit) * 1000) / mProcessorPowerUnit
    );

  HiiSetString (HiiHandle, STRING_TOKEN(STR_CTDP_MAX_DEFAULT_STRING), StringBuffer, NULL);
    
  gBS->FreePool (StringBuffer);
}

VOID
DisplayMinPowerLimit (
  IN  EFI_HII_HANDLE              HiiHandle
  )
{
  CHAR16                          *StringBuffer;
  UINT64                          MSRData;
  UINT16                          MinPowerLimit;

  StringBuffer = AllocateZeroPool (0x100);


  MSRData = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU);
  MSRData = MSRData & PACKAGE_MIN_POWER_MASK;
  MinPowerLimit = (UINT16) RShiftU64 (MSRData, 16);
  if (MinPowerLimit != 0) {
    UnicodeSPrint (
      StringBuffer,
      0x100,
      L"%d.%d",
      MinPowerLimit / mProcessorPowerUnit,
      ((MinPowerLimit % mProcessorPowerUnit) * 1000) / mProcessorPowerUnit
      );
    
  HiiSetString (HiiHandle, STRING_TOKEN(STR_CTDP_MIN_DEFAULT_STRING), StringBuffer, NULL);
  }
      
  gBS->FreePool (StringBuffer);
}

VOID
DisplayAdditionalcTDPLevels (
  IN  EFI_HII_HANDLE              HiiHandle
  )
{
  CHAR16                          *StringBuffer;
  UINT64                          MSRData;
  UINT8                           NumberOfcTDPLevels;

  StringBuffer = AllocateZeroPool (0x100);


  MSRData = AsmReadMsr64 (MSR_PLATFORM_INFO);   
  NumberOfcTDPLevels = (UINT8) RShiftU64 ((MSRData & V_CONFIG_TDP_NUM_LEVELS_MASK), 32);
  NumberOfcTDPLevels = (UINT8)RShiftU64 ((UINT64)NumberOfcTDPLevels, 1);
  
  UnicodeSPrint (
    StringBuffer,
    0x100,
    L"%d",
    NumberOfcTDPLevels
    );
    
  HiiSetString (HiiHandle, STRING_TOKEN(STR_CTDP_ADDITIONAL_LEVELS_DATA_STRING), StringBuffer, NULL);
      
  gBS->FreePool (StringBuffer);
}

VOID
DisplaycTDPNominal (
  IN  EFI_HII_HANDLE              HiiHandle
  )
{
  CHAR16                          *StringBuffer;
  UINT64                          MSRData;
  UINT32                          CTDPNominal;
  UINT32                          TDPNominal;  

  StringBuffer = AllocateZeroPool (0x100);


  MSRData = AsmReadMsr64 (MSR_CONFIG_TDP_NOMINAL);
  CTDPNominal = (UINT32) (MSRData & CONFIG_TDP_NOMINAL_RATIO_MASK);
  MSRData = AsmReadMsr64 (MSR_PACKAGE_POWER_SKU);
  TDPNominal = (UINT32)(MSRData & PACKAGE_TDP_POWER_MASK);
  
  UnicodeSPrint (
    StringBuffer,
    0x100,
    L"Ratio:%d  TDP:%d.%d",
    CTDPNominal,
    TDPNominal / mProcessorPowerUnit,
    ((TDPNominal % mProcessorPowerUnit) * 1000) / mProcessorPowerUnit
    );
    
  HiiSetString (HiiHandle, STRING_TOKEN(STR_CTDP_NOMINAL_DATA_STRING), StringBuffer, NULL);
      
  gBS->FreePool (StringBuffer);
}

VOID
DisplaycTDPLevel1 (
  IN  EFI_HII_HANDLE              HiiHandle
  )
{
  CHAR16                          *StringBuffer;
  UINT64                          MSRData;
  UINT32                          CTDPLevel1;
  UINT32                          TDPLevel1;

  StringBuffer = AllocateZeroPool (0x100);


  MSRData = AsmReadMsr64 (MSR_CONFIG_TDP_LVL1);
  CTDPLevel1 = (UINT32)(MSRData & CONFIG_TDP_LVL1_RATIO_MASK);
  CTDPLevel1 = (UINT32)RShiftU64 ((UINT64)CTDPLevel1, CONFIG_TDP_LVL1_RATIO_OFFSET);
  TDPLevel1  = (UINT32)(MSRData & CONFIG_TDP_LVL1_PKG_TDP_MASK);
  
  UnicodeSPrint (
    StringBuffer,
    0x100,
    L"Ratio:%d  TDP:%d.%d",
    CTDPLevel1,
    TDPLevel1 / mProcessorPowerUnit,
    ((TDPLevel1 % mProcessorPowerUnit) * 1000) / mProcessorPowerUnit
    );
    
  HiiSetString (HiiHandle, STRING_TOKEN(STR_CTDP_LEVEL1_DATA_STRING), StringBuffer, NULL);
      
  gBS->FreePool (StringBuffer);
}

VOID
DisplaycTDPLevel2 (
  IN  EFI_HII_HANDLE              HiiHandle
  )
{
  CHAR16                          *StringBuffer;
  UINT64                          MSRData;
  UINT32                          CTDPLevel2;
  UINT32                          TDPLevel2;

  StringBuffer = AllocateZeroPool (0x100);


  MSRData = AsmReadMsr64 (MSR_CONFIG_TDP_LVL2);
  CTDPLevel2 = (UINT32)(MSRData & CONFIG_TDP_LVL2_RATIO_MASK);
  CTDPLevel2 = (UINT32)RShiftU64 ((UINT64)CTDPLevel2, CONFIG_TDP_LVL2_RATIO_OFFSET);
  TDPLevel2  = (UINT32)(MSRData & CONFIG_TDP_LVL2_PKG_TDP_MASK);
  
  UnicodeSPrint (
    StringBuffer,
    0x100,
    L"Ratio:%d  TDP:%d.%d",
    CTDPLevel2,
    TDPLevel2 / mProcessorPowerUnit,
    ((TDPLevel2 % mProcessorPowerUnit) * 1000) / mProcessorPowerUnit
    );
    
  HiiSetString (HiiHandle, STRING_TOKEN(STR_CTDP_LEVEL2_DATA_STRING), StringBuffer, NULL);
      
  gBS->FreePool (StringBuffer);
}

VOID
DisplaycTDPControl (
  IN  EFI_HII_HANDLE              HiiHandle
  )
{
  CHAR16                          *StringBuffer;
  UINT64                          MSRData;
  UINT8                           TDPLevel;
  UINT8                           LockBit;

  StringBuffer = AllocateZeroPool (0x100);
//[-start-140625-IB05080432-add]//
  if (StringBuffer == NULL)
    return;
//[-end-140625-IB05080432-add]//

  MSRData = AsmReadMsr64 (MSR_CONFIG_TDP_CONTROL);
  TDPLevel = (UINT8)(MSRData & CONFIG_TDP_CONTROL_LVL_MASK);
  LockBit = (UINT8)RShiftU64(MSRData & CONFIG_TDP_CONTROL_LOCK, 31);
  
  UnicodeSPrint (
    StringBuffer,
    0x100,
    L"%d ",
    TDPLevel
    );

  if (LockBit == 0) {
    StrCat (StringBuffer, L"(Unlocked)");
  } else {
    StrCat (StringBuffer, L"(Locked)");  
  }

  HiiSetString (HiiHandle, STRING_TOKEN(STR_CTDP_CONTROL_DATA_STRING), StringBuffer, NULL);
      
  gBS->FreePool (StringBuffer);
}

VOID
DisplaycTurboRatio (
  IN  EFI_HII_HANDLE              HiiHandle
  )
{
  CHAR16                          *StringBuffer;
  UINT64                          MSRData;
  UINT8                           TurboRatio;
  UINT8                           LockBit;

  StringBuffer = AllocateZeroPool (0x100);
//[-start-130207-IB10870073-add]//
  ASSERT (StringBuffer != NULL);
  if (StringBuffer == NULL) {
    return;
  }
//[-end-130207-IB10870073-add]//

  MSRData = AsmReadMsr64 (MSR_TURBO_ACTIVATION_RATIO);
  TurboRatio = (UINT8)(MSRData & MSR_TURBO_ACTIVATION_RATIO_MASK);
  LockBit = (UINT8)RShiftU64(MSRData & MSR_TURBO_ACTIVATION_RATIO_LOCK, 31);
  
  UnicodeSPrint (
    StringBuffer,
    0x100,
    L"%d ",
    TurboRatio
    );

  if (LockBit == 0) {
    StrCat (StringBuffer, L"(Unlocked)");
  } else {
    StrCat (StringBuffer, L"(Locked)");  
  }

  HiiSetString (HiiHandle, STRING_TOKEN(STR_CTDP_TURBO_RATIO_DATA_STRING), StringBuffer, NULL);
      
  gBS->FreePool (StringBuffer);
}

VOID
DisplayLongTermPowerLimit (
  IN  EFI_HII_HANDLE              HiiHandle
  )
{
  CHAR16                          *StringBuffer;
  UINT64                          MSRData;
  UINT64                          MmioData;  
  UINT32                          MchBarAdd;
  UINT16                          MsrPowerLimit;
  UINT16                          MmioPowerLimit;  

  StringBuffer = AllocateZeroPool (0x100);


  MSRData = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
  MsrPowerLimit = (UINT16)(MSRData & POWER_LIMIT_MASK);
  MchBarAdd = MmPci32 (0,0,0,0,0x48) & 0xFFFFFFFE;
  MmioData = MmioRead64 (MchBarAdd + MMIO_TURBO_POWER_LIMIT);
  MmioPowerLimit = (UINT16)(MmioData & POWER_LIMIT_MASK);
  
  UnicodeSPrint (
    StringBuffer,
    0x100,
    L"%d.%d MSR:%d.%d ",
    MmioPowerLimit / mProcessorPowerUnit,
    ((MmioPowerLimit % mProcessorPowerUnit) * 1000) / mProcessorPowerUnit,
    MsrPowerLimit / mProcessorPowerUnit,
    ((MsrPowerLimit % mProcessorPowerUnit) * 1000) / mProcessorPowerUnit    
    );

  HiiSetString (HiiHandle, STRING_TOKEN(STR_CTDP_CURRENT_LONG_TERM_POWER_LIMIT_DATA_STRING), StringBuffer, NULL);
      
  gBS->FreePool (StringBuffer);
}

VOID
DisplayShortTermPowerLimit (
  IN  EFI_HII_HANDLE              HiiHandle
  )
{
  CHAR16                          *StringBuffer;
  UINT64                          MSRData;
  UINT64                          MmioData;  
  UINT32                          MchBarAdd;
  UINT16                          MsrPowerLimit;
  UINT16                          MmioPowerLimit;  

  StringBuffer = AllocateZeroPool (0x100);


  MSRData = AsmReadMsr64 (MSR_PACKAGE_POWER_LIMIT);
  MsrPowerLimit = (UINT16)(RShiftU64(MSRData, 32) & POWER_LIMIT_MASK);  
  MchBarAdd = MmPci32 (0,0,0,0,0x48) & 0xFFFFFFFE;
  MmioData = MmioRead64 (MchBarAdd + MMIO_TURBO_POWER_LIMIT);
  MmioPowerLimit = (UINT16)(RShiftU64(MmioData, 32) & POWER_LIMIT_MASK);  
  
  UnicodeSPrint (
    StringBuffer,
    0x100,
    L"%d.%d MSR:%d.%d ",
    MmioPowerLimit / mProcessorPowerUnit,
    ((MmioPowerLimit % mProcessorPowerUnit) * 1000) / mProcessorPowerUnit,
    MsrPowerLimit / mProcessorPowerUnit,
    ((MsrPowerLimit % mProcessorPowerUnit) * 1000) / mProcessorPowerUnit    
    );

  HiiSetString (HiiHandle, STRING_TOKEN(STR_CTDP_CURRENT_SHORT_TERM_POWER_LIMIT_DATA_STRING), StringBuffer, NULL);
      
  gBS->FreePool (StringBuffer);
}
