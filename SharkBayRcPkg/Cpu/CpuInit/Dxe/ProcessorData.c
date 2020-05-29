/** @file
   Produces CPU data records.

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

#include "ProcessorData.h"
#include "MpCommon.h"
#include "MpService.h"
#include <FrameworkDxe.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/HiiLib.h>
#include <Guid/ProcessorProducer.h>
#include <Guid/DataHubRecords.h>

///
/// This is the VFR compiler generated header file which defines the
/// string identifiers.
///
#include "CpuInitDxeStrDefs.h"


extern MP_SYSTEM_DATA *mMPSystemData;
extern DXE_CPU_PLATFORM_POLICY_PROTOCOL *mPlatformCpu;

extern EFI_DATA_HUB_PROTOCOL            *mDataHub;
extern DXE_CPU_PLATFORM_POLICY_PROTOCOL *mPlatformCpu;
extern EFI_HII_HANDLE                   mStringHandle;

EFI_SUBCLASS_TYPE1_HEADER mCpuDataRecordHeader = {
  EFI_PROCESSOR_SUBCLASS_VERSION,       ///< Version
  sizeof (EFI_SUBCLASS_TYPE1_HEADER),   ///< Header Size
  0,                                    ///< Instance, Initialize later
  EFI_SUBCLASS_INSTANCE_NON_APPLICABLE, ///< SubInstance
  0                                     ///< RecordType, Initialize later
};

EFI_STATUS
LogCpuData (
  EFI_DATA_HUB_PROTOCOL *DataHub,
  UINT8                 *Buffer,
  UINT32                Size
  );

typedef struct _PROCESSOR_FAMILY_FIELD {
  CHAR8  ProcessorFamily[48];
  UINT16 ProcessorEnum;
} PROCESSOR_FAMILY_FIELD;

PROCESSOR_FAMILY_FIELD ProcessorFamilyField[] = {
  {
    "Intel(R) Core(TM) i7",
    0xC6
  },
  {
    "Intel(R) Core(TM) i5",
    0xCD
  },
  {
    "Intel(R) Core(TM) i3",
    0xCE
  },
  {
    "Intel(R) Xeon(R) CPU",
    0xB3
  },
  {
    "Intel(R) Pentium(R) CPU",
    0x0B
  },
  {
    "Intel(R) Celeron(R) CPU",
    0x0F
  },
};

/**
  Converts an ascii string to unicode string 16 chars at a time.

  @param[in] AsciiString   - Pointer to input Ascii string.
  @param[in] UnicodeString - Pointer to output Unicode string buffer.
**/
VOID
AsciiToUnicode (
  IN CHAR8      *AsciiString,
  IN OUT CHAR16 *UnicodeString
  )
{
  UINT8 Index;

  for (Index = 0; Index < 16; Index++) {
    UnicodeString[Index] = (CHAR16) AsciiString[Index];
  }

  return;
}
///
/// Processor-specific routines
///
/**
  Returns the procesor version string token installed in the system.

  @retval Processor Version string token
**/
VOID
GetProcessorVersion (
  OUT PROCESSOR_VERSION_INFORMATION *Version
  )
{
  CHAR16             BrandIdString[MAXIMUM_CPU_BRAND_STRING_LENGTH + 1];
  EFI_CPUID_REGISTER CpuExtendedSupport;
  EFI_CPUID_REGISTER CpuBrandString;
  UINT8              Index;

  ///
  /// Create the string using Brand ID String.
  ///
  Version->StringValid = FALSE;

  if (IsIntelProcessor ()) {
    Version->StringRef = STRING_TOKEN (STR_INTEL_GENUINE_PROCESSOR);

    AsmCpuid (
            CPUID_EXTENDED_FUNCTION,
            &CpuExtendedSupport.RegEax,
            &CpuExtendedSupport.RegEbx,
            &CpuExtendedSupport.RegEcx,
            &CpuExtendedSupport.RegEdx
            );
    AsmCpuid (
            CPUID_BRAND_STRING1,
            &CpuBrandString.RegEax,
            &CpuBrandString.RegEbx,
            &CpuBrandString.RegEcx,
            &CpuBrandString.RegEdx
            );
    ///
    /// Check if Brand ID String is supported or filled up
    ///
    if (CpuExtendedSupport.RegEax != 0 && CpuBrandString.RegEax != 0) {
      AsciiToUnicode ((CHAR8 *) &CpuBrandString, (CHAR16 *) &BrandIdString[0]);
      AsmCpuid (
              CPUID_BRAND_STRING2,
              &CpuBrandString.RegEax,
              &CpuBrandString.RegEbx,
              &CpuBrandString.RegEcx,
              &CpuBrandString.RegEdx
              );
      AsciiToUnicode ((CHAR8 *) &CpuBrandString, (CHAR16 *) &BrandIdString[16]);
      AsmCpuid (
              CPUID_BRAND_STRING3,
              &CpuBrandString.RegEax,
              &CpuBrandString.RegEbx,
              &CpuBrandString.RegEcx,
              &CpuBrandString.RegEdx
              );
      AsciiToUnicode ((CHAR8 *) &CpuBrandString, (CHAR16 *) &BrandIdString[32]);

      ///
      /// Remove preceeding spaces
      ///
      Index = 0;
      while (BrandIdString[Index] == 0x20) {
        Index++;
        if (Index >= MAXIMUM_CPU_BRAND_STRING_LENGTH) {
          break;
        }
      }

      CopyMem (
              Version->BrandString,
              &BrandIdString[Index],
              (MAXIMUM_CPU_BRAND_STRING_LENGTH - Index) * sizeof (CHAR16)
              );
      Version->BrandString[MAXIMUM_CPU_BRAND_STRING_LENGTH - Index] = 0;
      Version->StringValid = TRUE;
    }
  } else {
    Version->StringRef = STRING_TOKEN (STR_UNKNOWN);
  }
}

/**
  Returns the procesor manufaturer string token installed in the system.

  @retval Processor Manufacturer string token
**/
EFI_PROCESSOR_MANUFACTURER_DATA
GetProcessorManufacturer (
  VOID
  )
{

  if (IsIntelProcessor ()) {
    return STRING_TOKEN (STR_INTEL_CORPORATION);
  } else {
    return STRING_TOKEN (STR_UNKNOWN);
  }
}

/**
  Returns if processor is Intel or not.

  @retval TRUE - Intel Processor.
  @retval FALSE - Not Intel Processor.
**/
BOOLEAN
IsIntelProcessor (
  VOID
  )
{
  EFI_CPUID_REGISTER Reg;

  AsmCpuid (CPUID_SIGNATURE, &Reg.RegEax, &Reg.RegEbx, &Reg.RegEcx, &Reg.RegEdx);

  if ((Reg.RegEbx != 'uneG') || (Reg.RegEdx != 'Ieni') || (Reg.RegEcx != 'letn')) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Returns the processor family of the processor installed in the system.

  @retval Processor Family
**/
//[-start-110825-IB03610346-modify]//
EFI_PROCESSOR_FAMILY_DATA_PLUS
//[-end-110825-IB03610346-modify]//
GetProcessorFamily (
  VOID
  )
{

  UINTN              j;
  UINTN              Index;
  EFI_CPUID_REGISTER CpuExtendedSupport;
  EFI_CPUID_REGISTER CpuBrandString;
  CHAR8              BrandString[MAXIMUM_CPU_BRAND_STRING_LENGTH + 1];
  UINT16             ProcessorFamily;
  ProcessorFamily = EfiProcessorFamilyUnknown;
  Index           = 0;

  if (IsIntelProcessor ()) {
    ///
    /// Get Brand string
    ///
    AsmCpuid (
            CPUID_EXTENDED_FUNCTION,
            &CpuExtendedSupport.RegEax,
            &CpuExtendedSupport.RegEbx,
            &CpuExtendedSupport.RegEcx,
            &CpuExtendedSupport.RegEdx
            );
    AsmCpuid (
            CPUID_BRAND_STRING1,
            &CpuBrandString.RegEax,
            &CpuBrandString.RegEbx,
            &CpuBrandString.RegEcx,
            &CpuBrandString.RegEdx
            );

    if (CpuExtendedSupport.RegEax != 0 && CpuBrandString.RegEax != 0) {
      AsciiStrCpy (&BrandString[0], (CHAR8 *) &CpuBrandString);
      AsmCpuid (
              CPUID_BRAND_STRING2,
              &CpuBrandString.RegEax,
              &CpuBrandString.RegEbx,
              &CpuBrandString.RegEcx,
              &CpuBrandString.RegEdx
              );
      AsciiStrCpy (&BrandString[16], (CHAR8 *) &CpuBrandString);
      AsmCpuid (
              CPUID_BRAND_STRING3,
              &CpuBrandString.RegEax,
              &CpuBrandString.RegEbx,
              &CpuBrandString.RegEcx,
              &CpuBrandString.RegEdx
              );
      AsciiStrCpy (&BrandString[32], (CHAR8 *) &CpuBrandString);

      ///
      /// Remove preceeding spaces
      ///
      while (BrandString[Index] == 0x20) {
        Index++;
      }

      BrandString[MAXIMUM_CPU_BRAND_STRING_LENGTH - Index] = 0;
    }

    ProcessorFamily = 0xC6;

    for (j = 0; j < sizeof (ProcessorFamilyField) / sizeof (PROCESSOR_FAMILY_FIELD); j++) {
      if (AsciiStrnCmp (
            &BrandString[Index],
            ProcessorFamilyField[j].ProcessorFamily,
            (AsciiStrLen (ProcessorFamilyField[j].ProcessorFamily))
            ) == 0) {
        ProcessorFamily = ProcessorFamilyField[j].ProcessorEnum;
        break;
      }
    }
  } else {
    ProcessorFamily = EfiProcessorFamilyOther;
  }

  return ProcessorFamily;
}

/**
  Returns the processor voltage of the processor installed in the system.

  @retval Processor Voltage
**/
INT16
GetProcessorVoltage (
  VOID
  )
{
  INT16  VoltageInmV;
  UINT64 MsrValue;

  VoltageInmV = 0;
  MsrValue = 0;

  ///
  /// Core voltage = (float) IA32_PERF_STS(47:32) * (float) 1/(2^13)
  ///
  MsrValue = AsmReadMsr64 (MSR_IA32_PERF_STS);
  MsrValue = RShiftU64(MsrValue, 32);
  MsrValue &= 0x0FFFF;

  ///
  /// Convert unit to mV
  ///
  MsrValue = MultU64x32(MsrValue, 1000);
  MsrValue = RShiftU64 (MsrValue, 13);
  VoltageInmV = (UINT16) MsrValue;

  return VoltageInmV;
}

/**
  Returns the processor microcode revision of the processor installed in the system.

  @retval Processor Microcode Revision
**/
UINT32
GetCpuUcodeRevision (
  VOID
  )
{
  AsmWriteMsr64 (MSR_IA32_BIOS_SIGN_ID, 0);
  AsmCpuid (CPUID_VERSION_INFO, NULL, NULL, NULL, NULL);
  return (UINT32) RShiftU64 (AsmReadMsr64 (MSR_IA32_BIOS_SIGN_ID), 32);
}

/**
  Get processor status by specific CPU number

  @param[in] CpuNumber - indicate which CPU status are requested

  @retval EFI_PROCESSOR_STATUS_DATA  for specific CPU number
**/
EFI_PROCESSOR_STATUS_DATA
GetProcessorStatus (
  IN UINTN CpuNumber
  )
{
  EFI_PROCESSOR_STATUS_DATA ProcessorStatus;
  CPU_DATA_BLOCK            *CpuData;

  CpuData                   = &mMPSystemData->CpuData[CpuNumber];

  ProcessorStatus.Reserved1 = 0;
  ProcessorStatus.Reserved2 = 0;
  ProcessorStatus.Reserved3 = 0;

  ProcessorStatus.CpuStatus = EfiCpuStatusEnabled;
  if (CpuData->State == CPU_STATE_DISABLED) {
    switch (mMPSystemData->DisableCause[CpuNumber]) {
      case CPU_CAUSE_USER_SELECTION:
      case CPU_CAUSE_BY_ASSOCIATION:
        ProcessorStatus.CpuStatus = EfiCpuStatusDisabledByUser;
        break;

      case CPU_CAUSE_INTERNAL_ERROR:
      case CPU_CAUSE_THERMAL_ERROR:
      case CPU_CAUSE_SELFTEST_FAILURE:
      case CPU_CAUSE_PREBOOT_TIMEOUT:
      case CPU_CAUSE_CONFIG_ERROR:
        ProcessorStatus.CpuStatus = EfiCpuStatusDisabledbyBios;
        break;

      case CPU_CAUSE_FAILED_TO_START:
      case CPU_CAUSE_UNSPECIFIED:
      default:
        ProcessorStatus.CpuStatus = EfiCpuStatusOther;
        break;
    }
  }

  ProcessorStatus.SocketPopulated = TRUE;
  ProcessorStatus.ApicEnable      = 1;

  if (mMPSystemData->BSP == CpuNumber) {
    ProcessorStatus.BootApplicationProcessor = 1;
  } else {
    ProcessorStatus.BootApplicationProcessor = 0;
  }

  return ProcessorStatus;
}

/**
  This function gets called with the processor number and will log all data to data hub
  pertaining to this processor.

  @param[in] CpuNumber         - Processor Number
  @param[in] CpuDataForDatahub - Contains CPU data which is collected for data hub

  @retval EFI_OUT_OF_RESOURCES - failed to allocate pool for CPU data
  @retval EFI_SUCCESS          - CPU data Hub has been created successfully
**/
EFI_STATUS
InitializeProcessorData (
  IN UINTN                CpuNumber,
  IN CPU_DATA_FOR_DATAHUB *CpuDataForDatahub
  )
{
  EFI_STATUS                 Status;
  EFI_CPU_DATA_RECORD_BUFFER RecordBuffer;
  UINT32                     HeaderSize;
  UINT32                     TotalSize;
  PLATFORM_CPU_INFORMATION   PlatformCpuInfo;
  STRING_REF                 Token;
  UINT64                     MsrValue;
  UINT64                     ProcessorCoreCount;
  UINT64                     ProcessorThreadCount;
//[-start-110825-IB03610346-add]//
  EFI_CPUID_REGISTER           CPUID;
  BOOLEAN                      MultiCore;
  BOOLEAN                      HardwareThread;
  BOOLEAN                      ExecuteProtection;
  BOOLEAN                      EnhancedVirtualization;
//[-end-110825-IB03610346-add]//

  mCpuDataRecordHeader.Instance = (UINT16) (CpuNumber + 1);

  HeaderSize                    = sizeof (EFI_SUBCLASS_TYPE1_HEADER);
  RecordBuffer.Raw              = AllocatePool (HeaderSize + EFI_CPU_DATA_MAXIMUM_LENGTH);
  if (RecordBuffer.Raw == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem (&PlatformCpuInfo, sizeof (PLATFORM_CPU_INFORMATION));
  CopyMem (RecordBuffer.Raw, &mCpuDataRecordHeader, HeaderSize);

  ///
  /// Record following data by socket base
  ///
  if (CpuNumber == 0) {
    ///
    /// Record Type 1
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType = ProcessorCoreFrequencyRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorCoreFrequency.Value    = ( UINT16 )CpuDataForDatahub->IntendCoreFrequency;
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorCoreFrequency.Exponent = 6;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_CORE_FREQUENCY_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 2
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType                    = ProcessorFsbFrequencyRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorFsbFrequency.Value    = 100;
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorFsbFrequency.Exponent = 6;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_FSB_FREQUENCY_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 3
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType = ProcessorVersionRecordType;
    if (CpuDataForDatahub->Version.StringValid) {
      Token = HiiSetString (mStringHandle, 0, CpuDataForDatahub->Version.BrandString, NULL);
      if (Token == 0) {
        Token = CpuDataForDatahub->Version.StringRef;
      }
    } else {
      Token = CpuDataForDatahub->Version.StringRef;
    }

//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorVersion = Token;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_VERSION_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 4
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType          = ProcessorManufacturerRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorManufacturer = CpuDataForDatahub->Manufacturer;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_MANUFACTURER_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 6.
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType = ProcessorIdRecordType;
    CopyMem (
//[-start-110825-IB03610346-modify]//
    &RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorId,
//[-end-110825-IB03610346-modify]//
      &CpuDataForDatahub->CpuidData,
      sizeof (CpuDataForDatahub->CpuidData)
      );
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_ID_DATA);
    Status    = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 7.
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType  = ProcessorTypeRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorType = EfiCentralProcessor;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_TYPE_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 8.
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType    = ProcessorFamilyRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorFamily = CpuDataForDatahub->Family;
    TotalSize = HeaderSize + sizeof ( EFI_PROCESSOR_FAMILY_DATA_PLUS );
//[-end-110825-IB03610346-modify]//
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 9.
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType              = ProcessorVoltageRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorVoltage.Value    = CpuDataForDatahub->Voltage;
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorVoltage.Exponent = -3;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_VOLTAGE_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);
  }
  ///
  /// log data by socket base
  ///
  /// Record Type 10.
  ///
  RecordBuffer.DataRecord->DataRecordHeader.RecordType      = ProcessorApicBaseAddressRecordType;
//[-start-110825-IB03610346-modify]//
  RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorApicBase = CpuDataForDatahub->ApicBase;
//[-end-110825-IB03610346-modify]//
  TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_APIC_BASE_ADDRESS_DATA);
  Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

  ///
  /// Record Type 11.
  ///
  RecordBuffer.DataRecord->DataRecordHeader.RecordType    = ProcessorApicIdRecordType;
//[-start-110825-IB03610346-modify]//
  RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorApicId = CpuDataForDatahub->ApicID;
//[-end-110825-IB03610346-modify]//
  TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_APIC_ID_DATA);
  Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

  ///
  /// Record Type 12.
  ///
  RecordBuffer.DataRecord->DataRecordHeader.RecordType                = ProcessorApicVersionNumberRecordType;
//[-start-110825-IB03610346-modify]//
  RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorApicVersionNumber = CpuDataForDatahub->ApicVersion;
//[-end-110825-IB03610346-modify]//
  TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_APIC_VERSION_NUMBER_DATA);
  Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

  ///
  /// Record Type 13.
  ///
  RecordBuffer.DataRecord->DataRecordHeader.RecordType  = CpuUcodeRevisionDataRecordType;
//[-start-110825-IB03610346-modify]//
  RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.CpuUcodeRevisionData.ProcessorMicrocodeType           = EfiProcessorIa32Microcode;
  RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.CpuUcodeRevisionData.ProcessorMicrocodeRevisionNumber = CpuDataForDatahub->MicrocodeRevision;
//[-end-110825-IB03610346-modify]//
  TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_MICROCODE_REVISION_DATA);
  Status    = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

  ///
  /// Record following data by socket base
  ///
  if (CpuNumber == 0) {
    ///
    /// Record Type 14.
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType    = ProcessorStatusRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorStatus = CpuDataForDatahub->Status;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_STATUS_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 17(0x11). Set in Cache File
    ///
    ///
    /// Record Type 21(0x15). zero based
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType            = ProcessorPackageNumberRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorPackageNumber = CpuDataForDatahub->Location.Package;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_PACKAGE_NUMBER_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 24(0x18).
    /// Health definition in DataHub spec is not the same as BIST format, so add 1 to convert
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType          = ProcessorHealthStatusRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorHealthStatus = CpuDataForDatahub->Health.Uint32 + 1;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_HEALTH_STATUS);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);
  }
  ///
  /// log data by socket base
  ///
  /// The following record data comes from platform driver:
  ///
  PlatformCpuInfo.StringHandle  = mStringHandle;
  PlatformCpuInfo.ApicID        = CpuDataForDatahub->ApicID;
  Status = mPlatformCpu->CpuConfig->GetCpuInfo (
                  mPlatformCpu,
                  &CpuDataForDatahub->Location,
                  &PlatformCpuInfo
                  );

  ///
  /// Record following data by socket base
  ///
  if (CpuNumber == 0) {
    ///
    /// Record Type 5. (Processor Serial Number: this feature is only available on PIII, not support here)
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType      = ProcessorSerialNumberRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorAssetTag = PlatformCpuInfo.SerialNumber;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_SERIAL_NUMBER_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 15.
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType        = ProcessorSocketTypeRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorSocketType = PlatformCpuInfo.SocketType;
    TotalSize = HeaderSize + sizeof ( EFI_PROCESSOR_SOCKET_TYPE_DATA_PLUS );
//[-end-110825-IB03610346-modify]//
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 16(0x10).
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType        = ProcessorSocketNameRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorSocketName = PlatformCpuInfo.SocketName;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_SOCKET_NAME_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 18(0x12).
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType              = ProcessorMaxCoreFrequencyRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorMaxCoreFrequency = PlatformCpuInfo.MaxCoreFrequency;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_MAX_CORE_FREQUENCY_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 19(0x13).
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType      = ProcessorAssetTagRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorAssetTag = PlatformCpuInfo.AssetTag;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_ASSET_TAG_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);
  }
  ///
  /// log data by socket base
  ///
  /// Record Type 20(0x14).
  ///
  RecordBuffer.DataRecord->DataRecordHeader.RecordType              = ProcessorMaxFsbFrequencyRecordType;
//[-start-110825-IB03610346-modify]//
  RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorMaxFsbFrequency = PlatformCpuInfo.MaxFsbFrequency;
//[-end-110825-IB03610346-modify]//
  TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_MAX_FSB_FREQUENCY_DATA);
  Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

  ///
  /// Record Type 22(0x16).
  ///
  RecordBuffer.DataRecord->DataRecordHeader.RecordType                = ProcessorCoreFrequencyListRecordType;
//[-start-110825-IB03610346-modify]//
  RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorCoreFrequencyList = PlatformCpuInfo.PlatformCoreFrequencyList;
//[-end-110825-IB03610346-modify]//
  TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_CORE_FREQUENCY_LIST_DATA);
  Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

  ///
  /// Record Type 23(0x17).
  ///
  RecordBuffer.DataRecord->DataRecordHeader.RecordType              = ProcessorFsbFrequencyListRecordType;
//[-start-110825-IB03610346-modify]//
  RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorFsbFrequencyList = PlatformCpuInfo.PlatformFsbFrequencyList;
//[-end-110825-IB03610346-modify]//
  TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_FSB_FREQUENCY_LIST_DATA);
  Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

  ///
  /// Record following data by socket base
  ///
  if (CpuNumber == 0) {
    ///
    /// Record Type 30. (Processor Part Number)
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType      = ProcessorPartNumberRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorAssetTag = PlatformCpuInfo.PartNumber;
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_PART_NUMBER_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 28.
    ///
//[-start-110825-IB03610346-add]//
    AsmCpuidEx ( CPUID_CORE_TOPOLOGY, 1, &CPUID.RegEax, &CPUID.RegEbx, &CPUID.RegEcx, &CPUID.RegEdx );
    MultiCore = ( ( CPUID.RegEbx & 0x00FF ) != 1 ) ? 1 : 0;
  
    AsmCpuidEx ( CPUID_CORE_TOPOLOGY, 0, &CPUID.RegEax, &CPUID.RegEbx, &CPUID.RegEcx, &CPUID.RegEdx );
    HardwareThread = ( ( CPUID.RegEbx & 0x00FF ) != 1 ) ? 1 : 0;
  
    AsmCpuid ( CPUID_EXTENDED_FUNCTION, &CPUID.RegEax, &CPUID.RegEbx, &CPUID.RegEcx, &CPUID.RegEdx );
    if ( CPUID.RegEax > CPUID_EXTENDED_FUNCTION ) {
      AsmCpuid ( CPUID_EXTENDED_CPU_SIG, &CPUID.RegEax, &CPUID.RegEbx, &CPUID.RegEcx, &CPUID.RegEdx );
    } else {
      CPUID.RegEdx = 0;
    }
    ExecuteProtection = ( ( CPUID.RegEdx & B_CPUID_VERSION_INFO_EDX_XD ) != 0 ) ? 1 : 0;
  
    AsmCpuid ( CPUID_VERSION_INFO, &CPUID.RegEax, &CPUID.RegEbx, &CPUID.RegEcx, &CPUID.RegEdx );
    EnhancedVirtualization = ( ( CPUID.RegEcx & B_CPUID_VERSION_INFO_ECX_VME ) != 0 ) ? 1 : 0;
//[-end-110825-IB03610346-add]//
    RecordBuffer.DataRecord->DataRecordHeader.RecordType                          = ProcessorCharacteristicsRecordType;
    RecordBuffer.DataRecord->VariableRecord.ProcessorCharacteristics.Reserved2    = 0;
    RecordBuffer.DataRecord->VariableRecord.ProcessorCharacteristics.Reserved     = 0;
    RecordBuffer.DataRecord->VariableRecord.ProcessorCharacteristics.Unknown      = 0;
    RecordBuffer.DataRecord->VariableRecord.ProcessorCharacteristics.Capable64Bit = 1;
//[-start-110825-IB03610346-add]//
    RecordBuffer.DataRecord->VariableRecord.ProcessorCharacteristics.MultiCore               = MultiCore;
    RecordBuffer.DataRecord->VariableRecord.ProcessorCharacteristics.HardwareThread          = HardwareThread;
    RecordBuffer.DataRecord->VariableRecord.ProcessorCharacteristics.ExecuteProtection       = ExecuteProtection;
    RecordBuffer.DataRecord->VariableRecord.ProcessorCharacteristics.EnhancedVirtualization  = EnhancedVirtualization;
    RecordBuffer.DataRecord->VariableRecord.ProcessorCharacteristics.PowerPerformanceControl = 1;
//[-end-110825-IB03610346-add]//
//[-start-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof ( EFI_PROCESSOR_CHARACTERISTICS_DATA_PLUS );
//[-end-110825-IB03610346-modify]//
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    MsrValue = AsmReadMsr64 (MSR_CORE_THREAD_COUNT);
    ProcessorThreadCount = MsrValue & 0xffff;
    ProcessorCoreCount = (MsrValue >> 16) & 0xffff;

    ///
    /// Record Type 25.
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType        = ProcessorCoreCountRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorCoreCount = ( UINT8 )( ProcessorCoreCount );
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_CORE_COUNT_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 26.
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType              = ProcessorEnabledCoreCountRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorEnabledCoreCount = ( UINT8 )( ProcessorCoreCount );
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_ENABLED_CORE_COUNT_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);

    ///
    /// Record Type 27.
    ///
    RecordBuffer.DataRecord->DataRecordHeader.RecordType          = ProcessorThreadCountRecordType;
//[-start-110825-IB03610346-modify]//
    RecordBuffer.DataRecord->VariableRecord.EfiCpuVariableRecord.ProcessorThreadCount = ( UINT8 )( ProcessorThreadCount );
//[-end-110825-IB03610346-modify]//
    TotalSize = HeaderSize + sizeof (EFI_PROCESSOR_THREAD_COUNT_DATA);
    Status = LogCpuData (mDataHub, RecordBuffer.Raw, TotalSize);
  }
  ///
  /// log data by socket base
  ///
  FreePool (RecordBuffer.Raw);

  return EFI_SUCCESS;
}

/**
  Log CPU data into data hub

  @param[in] DataHub - point to data hub that will be updated
  @param[in] Buffer  - the buffer which will be updated into data hub
  @param[in] Size    - size of the buffer

  @retval EFI_STATUS - status returned when updating Data hub
**/
EFI_STATUS
LogCpuData (
  EFI_DATA_HUB_PROTOCOL *DataHub,
  UINT8                 *Buffer,
  UINT32                Size
  )
{
  EFI_STATUS Status;

  Status = DataHub->LogData (
                      DataHub,
                      &gEfiProcessorSubClassGuid,
                      &gProcessorProducerGuid,
                      EFI_DATA_RECORD_CLASS_DATA,
                      Buffer,
                      Size
                      );
  return Status;

}
