/**
  This file contains an 'Intel Peripheral Driver' and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2004 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  CpuDataStruct.h

@brief
  CPU data structure

**/
#ifndef _CPU_DATA_STRUCT_H
#define _CPU_DATA_STRUCT_H

//[-start-120528-IB10820061-modify]// 
#include <Guid/SmramCpuDataVariable.h>
#include <Guid/SmramCpuDataHeader.h>
//[-end-120528-IB10820061-modify]// 

#pragma pack(1)
typedef struct {
  UINT16 Limit;
  UINTN  Base;
} PSEUDO_DESCRIPTOR;
#pragma pack()

typedef struct {
  BOOLEAN              APState;
  BOOLEAN              S3BootPath;
  EFI_PHYSICAL_ADDRESS WakeUpBuffer;
  EFI_PHYSICAL_ADDRESS GdtrProfile;
  EFI_PHYSICAL_ADDRESS IdtrProfile;
  EFI_PHYSICAL_ADDRESS CpuPrivateData;
  EFI_PHYSICAL_ADDRESS StackAddress;
  EFI_PHYSICAL_ADDRESS MicrocodePointerBuffer;
  EFI_PHYSICAL_ADDRESS SmramBase;
  EFI_PHYSICAL_ADDRESS SmmStartImageBase;
  UINT32               SmmStartImageSize;
  UINT32               NumberOfCpus;
} ACPI_CPU_DATA;

typedef struct {
  //
  // Guid as Signature.
  //
  EFI_GUID             HeaderGuid;
  EFI_PHYSICAL_ADDRESS AcpiCpuPointer;
  ACPI_CPU_DATA        AcpiCpuData;

  //
  // It points the data defined below.
  //
  EFI_PHYSICAL_ADDRESS GdtrProfileOffset;
  EFI_PHYSICAL_ADDRESS GdtOffset;
  EFI_PHYSICAL_ADDRESS IdtrProfileOffset;
  EFI_PHYSICAL_ADDRESS IdtOffset;
  EFI_PHYSICAL_ADDRESS CpuPrivateDataOffset;
  EFI_PHYSICAL_ADDRESS S3BootScriptTableOffset;
  EFI_PHYSICAL_ADDRESS S3BspMtrrTableOffset;
  EFI_PHYSICAL_ADDRESS MicrocodePointerBufferOffset; ///< It is pointer to pointer array.
  EFI_PHYSICAL_ADDRESS MicrocodeDataBufferOffset;    ///< It is pointer to the data.
  ///
  /// We need put all the data buffer here as well.
  /// These data will be copied to original location in S3.
  ///
  //
  // DataBuffer size
  //
  UINT32 GdtrProfileSize;
  UINT32 GdtSize;
  UINT32 IdtrProfileSize;
  UINT32 IdtSize;
  UINT32 CpuPrivateDataSize;
  UINT32 S3BootScriptTableSize;
  UINT32 S3BspMtrrTableSize;
  UINT32 MicrocodePointerBufferSize;
  UINT32 MicrocodeDataBufferSize;

  //
  //  UINT8                 WakeUpBufferData[WakeUpBufferSize];
  //  UINT8                 GdtrProfileData[GdtrProfileSize];
  //  UINT8                 GdtData[GdtSize];
  //  UINT8                 IdtrProfileData[IdtrProfileSize];
  //  UINT8                 IdtData[IdtSize];
  //  UINT8                 CpuPrivateData[CpuPrivateDataSize];
  //  UINT8                 S3BootScriptTable[S3BootScriptTableSize];
  //  UINT8                 S3BspMtrrTable[S3BspMtrrTableSize];
  //  UINT32                MicrocodePointerBufferData[NumberOfCpus + 1];
  //  UINT8                 MicrocodeDataBufferData[MicrocodeDataBufferSize * NumberOfCpus];
  //
} SMRAM_CPU_DATA;

typedef struct {
  UINT32 RegEax;
  UINT32 RegEbx;
  UINT32 RegEcx;
  UINT32 RegEdx;
} EFI_CPUID_REGISTER;

typedef struct {
  UINT32 HeaderVersion;
  UINT32 UpdateRevision;
  UINT32 Date;
  UINT32 ProcessorId;
  UINT32 Checksum;
  UINT32 LoaderRevision;
  UINT32 ProcessorFlags;
  UINT32 DataSize;
  UINT32 TotalSize;
  UINT8  Reserved[12];
} EFI_CPU_MICROCODE_HEADER;

typedef struct {
  UINT32 ExtendedSignatureCount;
  UINT32 ExtendedTableChecksum;
  UINT8  Reserved[12];
} EFI_CPU_MICROCODE_EXTENDED_TABLE_HEADER;

typedef struct {
  UINT32 ProcessorSignature;
  UINT32 ProcessorFlag;
  UINT32 ProcessorChecksum;
} EFI_CPU_MICROCODE_EXTENDED_TABLE;

typedef struct {
  UINT32 Stepping       : 4;
  UINT32 Model          : 4;
  UINT32 Family         : 4;
  UINT32 Type           : 2;
  UINT32 Reserved1      : 2;
  UINT32 ExtendedModel  : 4;
  UINT32 ExtendedFamily : 8;
  UINT32 Reserved2      : 4;
} EFI_CPU_VERSION;

#endif
