//
// This file contains an 'Intel Peripheral Driver' and is      
// licensed for Intel CPUs and chipsets under the terms of your
// license agreement with Intel or your vendor.  This file may 
// be modified by the user, subject to additional terms of the 
// license agreement                                           
//
/** @file
  Include file for record processor subclass data with Smbios protocol.

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#ifndef _PROCESSOR_H_
#define _PROCESSOR_H_

#include "MpCommon.h"

//
// This is the string tool generated data representing our strings.
//
extern UINT8                    CpuMpDxeStrings[];
extern EFI_SMBIOS_PROTOCOL      *mSmbios;
extern EFI_HII_HANDLE           mStringHandle;
extern UINT32                   mPopulatedSocketCount;

//
// This constant defines the maximum length of the CPU brand string. According to the
// IA manual, the brand string is in EAX through EDX (thus 16 bytes) after executing
// the CPUID instructions with EAX as 80000002, 80000003, 80000004.
//
#define MAXIMUM_CPU_BRAND_STRING_LENGTH 48

typedef struct {
  BOOLEAN           StringValid;
  CHAR16            BrandString[MAXIMUM_CPU_BRAND_STRING_LENGTH + 1];
  EFI_STRING_ID     StringRef;
} CPU_PROCESSOR_VERSION_INFORMATION;

//
// It is defined for SMBIOS_TABLE_TYPE4.Status.
//
typedef struct {
  UINT8 CpuStatus       :3; // Indicates the status of the processor.
  UINT8 Reserved1       :3; // Reserved for future use. Should be set to zero.
  UINT8 SocketPopulated :1; // Indicates if the processor socket is populated or not.
  UINT8 Reserved2       :1; // Reserved for future use. Should be set to zero.
} CPU_PROCESSOR_STATUS_DATA;

//
// It is defined for SMBIOS_TABLE_TYPE4.ProcessorCharacteristics.
//
typedef struct {
  UINT16 Reserved       :1;
  UINT16 Unknown        :1;
  UINT16 Capable64Bit   :1;
  UINT16 Reserved2      :13;
} CPU_PROCESSOR_CHARACTERISTICS_DATA;

/**
  Add Processor Information to Type 4 SMBIOS Record for Socket Populated.

  @param[in]    ProcessorNumber     Processor number of specified processor.
  @param[in]    L1CacheHandle       The handle of the L1 Cache SMBIOS record.
  @param[in]    L2CacheHandle       The handle of the L2 Cache SMBIOS record.
  @param[in]    L3CacheHandle       The handle of the L3 Cache SMBIOS record.

**/
VOID
AddSmbiosProcessorTypeTable (
  IN UINTN              ProcessorNumber,
  IN EFI_SMBIOS_HANDLE  L1CacheHandle,
  IN EFI_SMBIOS_HANDLE  L2CacheHandle,
  IN EFI_SMBIOS_HANDLE  L3CacheHandle
  );

/**
  Register notification functions for Pcds related to Smbios Processor Type.
**/
VOID
SmbiosProcessorTypeTableCallback (
  VOID
  );

/**
  Returns the processor voltage of the processor installed in the system.

  @param    ProcessorNumber     Processor number of specified processor.

  @return   Processor Voltage in mV

**/
UINT16
GetProcessorVoltage (
  IN UINTN  ProcessorNumber
  );

/**
  Returns the procesor version string token installed in the system.

  @param    ProcessorNumber  Processor number of specified processor.
  @param    Version          Pointer to the output processor version.

**/
VOID
GetProcessorVersion (
  IN UINTN                                  ProcessorNumber,
  OUT CPU_PROCESSOR_VERSION_INFORMATION     *Version
  );

/**
  Returns the processor family of the processor installed in the system.

  @param    ProcessorNumber     Processor number of specified processor.

  @return   Processor Family

**/
PROCESSOR_FAMILY_DATA
GetProcessorFamily (
  IN UINTN  ProcessorNumber
  );

/**
  Returns the procesor manufaturer string token installed in the system.

  @param    ProcessorNumber     Processor number of specified processor.

  @return   Processor Manufacturer string token.

**/
EFI_STRING_ID
GetProcessorManufacturer (
  IN UINTN  ProcessorNumber
  );

/**
  Checks if processor is Intel or not.

  @param    ProcessorNumber     Processor number of specified processor.

  @return   TRUE                Intel Processor.
  @return   FALSE               Not Intel Processor.

**/
BOOLEAN
IsIntelProcessor (
  IN UINTN  ProcessorNumber
  );

#endif

