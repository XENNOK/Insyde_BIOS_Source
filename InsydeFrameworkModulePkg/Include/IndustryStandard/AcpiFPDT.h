//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _ACPI_FPDT_H_
#define _ACPI_FPDT_H_

#include "Acpi.h"
//
// =================================================================
// Performance Recode Type.
//
typedef enum {
  FIRMWARE_BASIC_BOOT_PERFORMANCE_POINTER_RECORD_TYPE = 0,
  S3_PERFORMANCE_TABLE_POINTER_RECORD_TYPE
} FPDT_PERFORMANCE_RECORD_TYPE;
//
// 0x0002 ¡V 0x0FFF Reserved for ACPI specification usage.
// 0x1000 ¡V 0x1FFF Reserved for Platform Vendor usage.
// 0x2000 ¡V 0x2FFF Reserved for Hardware Vendor usage.
// 0x3000 ¡V 0x3FFF Reserved for BIOS Vendor usage.
// 0x4000 ¡V 0xFFFF Reserved for future use.
//

//
// Performance Pointer record revision.
//
#define FIRMWARE_BASIC_BOOT_PERFORMANCE_POINTER_RECORD_REV 0x01
#define S3_PERFORMANCE_TABLE_POINTER_RECORD_REV            0x01

//
// Record signature.
//
#define EFI_ACPI_FBPT_TABLE_SIGNATURE EFI_SIGNATURE_32('F','B','P','T')
#define EFI_ACPI_S3PT_TABLE_SIGNATURE EFI_SIGNATURE_32('S','3','P','T')

//
// =================================================================
// Runtime Performance Record Type.
//
typedef enum {
  S3_RESUME_PERFORMANCE_RECORD_TYPE = 0,
  S3_SUSPENDED_PERFORMANCE_RECORD_TYPE,
  FIRMWARE_BASIC_BOOT_PERFORMANCE_DATA_RECORD_TYPE,
  RUNTIME_PERFORMANCE_RECORD_TYPE_MAX
} FPDT_RUNTIME_PERFORMANCE_RECORD_TYPE;
//
// 0x0003 ¡V 0x0FFF Reserved for ACPI specification usage.
// 0x1000 ¡V 0x1FFF Reserved for Platform Vendor usage.  
// 0x2000 ¡V 0x2FFF Reserved for Hardware Vendor usage.  
// 0x3000 ¡V 0x3FFF Reserved for BIOS Vendor usage.  
// 0x4000 ¡V 0xFFFF Reserved for future use.

//
// Runtime Performance Record revision.
//
#define BASIC_S3_RESUME_PERFORMANCE_RECORD_REV          0x01
#define BASIC_S3_SUSPEND_PERFORMANCE_RECORD_REV         0x01
#define FIRMWARE_BASIC_BOOT_PERFORMANCE_DATA_RECORD_REV 0x02

#pragma pack(1)
//
// A performance record format. 
//
typedef struct {
  UINT16  PerfRecordType;
  UINT8   RecordLength;
  UINT8   Revision;
} FPDT_PERFORMANCE_RECORD_FORMAT;

//
// ====================================================================
// Performance Pointer Record Table.
//

//
// Firmware Basic Boot Performance Pointer Record.
//
typedef struct {
  FPDT_PERFORMANCE_RECORD_FORMAT RecordHeader;
  UINT8                          Reserved[4];
  EFI_PHYSICAL_ADDRESS           FBPTPointer;
} FPDT_FIRMWARE_BASIC_BOOT_PERFORMANCE_POINTER_RECORD;

//
// S3 Performance Table Pointer Record.
//
typedef struct {
  FPDT_PERFORMANCE_RECORD_FORMAT RecordHeader;
  UINT8                          Reserved[4];
  EFI_PHYSICAL_ADDRESS           S3PTPointer;
} FPDT_S3_PERFORMANCE_TABLE_POINTER_RECORD;

//
// ====================================================================
// Performance Record Table. 
// Outside of the FPDT is the range of memory as ACPI AddressRangeReserved in system memory.
//

//
// Firmware Basic Boot Performance Table.
//
typedef struct {
  UINT32                    Signature;
  UINT32                    Length;
} FPDT_FBPT_PERFORMANCE_TABLE_HEADER;

//
// Firmware Basic Boot Performance Data Record.
//
typedef struct {
  FPDT_PERFORMANCE_RECORD_FORMAT RecordHeader;
  UINT8                          Reserved[4];
  UINT64                         ResetEnd;
  UINT64                         OSLoaderLoadImageStart;
  UINT64                         OSLoaderStartImageStart;
  UINT64                         ExitBootServicesEntry;
  UINT64                         ExitBootServicesExit;
} FPDT_FIRMWARE_BASIC_BOOT_PERFORMANCE_DATA_RECORD;

//
// S3 Performance Table.
//
typedef struct {
  UINT32                    Signature;
  UINT32                    Length;
} FPDT_S3PT_PERFORMANCE_TABLE_HEADER;

//
// Basic S3 Resume Performance Record.
//
typedef struct {
  FPDT_PERFORMANCE_RECORD_FORMAT RecordHeader;
  UINT32                         ResumeCount;
  UINT64                         FullResume;
  UINT64                         AvgResume;
} FPDT_BASIC_S3_RESUME_PERFORMANCE_RECORD;

typedef struct {
  FPDT_PERFORMANCE_RECORD_FORMAT RecordHeader;
  UINT64                         SuspendStart;
  UINT64                         SuspendEnd;
} FPDT_BASIC_S3_SUSPEND_PERFORMANCE_RECORD;
  
//
// ===================================================================
// Firmware Performance Data Table
//

#define EFI_ACPI_FPDT_TABLE_SIGNATURE EFI_SIGNATURE_32('F','P','D','T')

#define EFI_ACPI_FPDT_TABLE_REVISION  0x01

typedef struct {
  FPDT_FIRMWARE_BASIC_BOOT_PERFORMANCE_POINTER_RECORD  FBDTPointerRecord;
  FPDT_S3_PERFORMANCE_TABLE_POINTER_RECORD             S3PTPointerRecord;
} FPDT_REQUIRED_PERFORMANCE_RECORD;

typedef struct {
  EFI_ACPI_DESCRIPTION_HEADER          Header;
  FPDT_REQUIRED_PERFORMANCE_RECORD     RequiredPerfRecord;
} EFI_ACPI_FIRMWARE_PERFORMANCE_DATA_TABLE;

#pragma pack()

#endif