/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  Code to retrieve processor sublcass data.

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "Cpu.h"
#include "Processor.h"

/**
  Returns the procesor version string token installed in the system.

  @param    ProcessorNumber  Processor number of specified processor.
  @param    Version          Pointer to the output processor version.

**/
VOID
GetProcessorVersion (
  IN UINTN                                  ProcessorNumber,
  OUT CPU_PROCESSOR_VERSION_INFORMATION     *Version
  )
{
  CHAR16                BrandIdString[MAXIMUM_CPU_BRAND_STRING_LENGTH + 1];
  EFI_CPUID_REGISTER    *CpuBrandString;
  UINT8                 Index;

  //
  // Create the string using Brand ID String.
  //
  Version->StringValid = FALSE;

  if (IsIntelProcessor (ProcessorNumber)) {
    Version->StringRef = STRING_TOKEN (STR_INTEL_GENUINE_PROCESSOR);

    CpuBrandString = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_BRAND_STRING1);
    ASSERT (CpuBrandString != NULL);

    //
    // Check if Brand ID String is supported or filled up
    //
    if (CpuBrandString->RegEax != 0) {
      AsciiStrToUnicodeStr ((CHAR8 *) CpuBrandString, (CHAR16 *) &BrandIdString[0]);

      CpuBrandString = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_BRAND_STRING2);
      ASSERT (CpuBrandString != NULL);
      AsciiStrToUnicodeStr ((CHAR8 *) CpuBrandString, (CHAR16 *) &BrandIdString[16]);

      CpuBrandString = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_BRAND_STRING3);
      ASSERT (CpuBrandString != NULL);
      AsciiStrToUnicodeStr ((CHAR8 *) CpuBrandString, (CHAR16 *) &BrandIdString[32]);

      //
      // Remove preceeding spaces
      //
      Index = 0;
      while (((Index < MAXIMUM_CPU_BRAND_STRING_LENGTH) && (BrandIdString[Index] == 0x20)) != 0) {
        Index++;
      }

      ASSERT (Index <= MAXIMUM_CPU_BRAND_STRING_LENGTH);
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

  @param    ProcessorNumber     Processor number of specified processor.

  @return   Processor Manufacturer string token.

**/
EFI_STRING_ID
GetProcessorManufacturer (
  IN UINTN  ProcessorNumber
  )
{
  if (IsIntelProcessor (ProcessorNumber)) {
    return STRING_TOKEN (STR_INTEL_CORPORATION);
  } else {
    return STRING_TOKEN (STR_UNKNOWN);
  }
}

/**
  Checks if processor is Intel or not.

  @param    ProcessorNumber     Processor number of specified processor.

  @return   TRUE                Intel Processor.
  @return   FALSE               Not Intel Processor.

**/
BOOLEAN
IsIntelProcessor (
  IN UINTN  ProcessorNumber
  )
{
  EFI_CPUID_REGISTER  *Reg;

  Reg = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_SIGNATURE);
  ASSERT (Reg != NULL);

  //
  // After CPUID(0), check if EBX contians 'uneG', ECX contains 'letn', and EDX contains 'Ieni'
  //
  if ((Reg->RegEbx != 0x756e6547) || (Reg->RegEcx != 0x6c65746e) || (Reg->RegEdx != 0x49656e69)) {
    return FALSE;
  } else {
    return TRUE;
  }
}

/**
  Returns the processor family of the processor installed in the system.

  @param    ProcessorNumber     Processor number of specified processor.

  @return   Processor Family

**/
PROCESSOR_FAMILY_DATA
GetProcessorFamily (
  IN UINTN  ProcessorNumber
  )
{
  UINT32  FamilyId;
  UINT32  ModelId;

  if (IsIntelProcessor (ProcessorNumber)) {

    GetProcessorVersionInfo (ProcessorNumber, &FamilyId, &ModelId, NULL, NULL);

    //
    // If Merom or Conroe processor family
    //
    if (FamilyId == 0x06 && (ModelId == 0x0F || ModelId == 0x16)) {
      switch (mPlatformType) {
      case PLATFORM_DESKTOP:
        return ProcessorFamilyIntelPentiumD;

      case PLATFORM_MOBILE:
        return ProcessorFamilyIntelPentiumM;

      case PLATFORM_SERVER:
        return ProcessorFamilyIntelXeon;

      default:
        return ProcessorFamilyUnknown;
      }
    }

    return ProcessorFamilyPentium4;
  }

  return ProcessorFamilyUnknown;
}

/**
  Returns the processor voltage of the processor installed in the system.

  @param    ProcessorNumber     Processor number of specified processor.

  @return   Processor Voltage in mV

**/
UINT16
GetProcessorVoltage (
  IN UINTN  ProcessorNumber
  )
{
  UINT16             VoltageInmV;
  EFI_CPUID_REGISTER *Reg;

  Reg = GetProcessorCpuid (ProcessorNumber, EFI_CPUID_VERSION_INFO);
  ASSERT (Reg != NULL);

  if ((Reg->RegEax >> 8 & 0x3F) == 0xF) {
    VoltageInmV = 3000;
  } else {
    VoltageInmV = 1600;
  }

  return VoltageInmV;
}

