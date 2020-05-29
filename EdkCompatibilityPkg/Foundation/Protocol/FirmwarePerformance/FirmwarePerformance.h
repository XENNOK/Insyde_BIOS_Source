/*++

Copyright (c) 2011 - 2012, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

    FirmwarePerformance.h

Abstract:

    Firmware Performance Protocol
  
--*/

#ifndef _FIRMWARE_PERFORMANCE_H_
#define _FIRMWARE_PERFORMANCE_H_

#define FIRMWARE_PERFORMANCE_PROTOCOL_GUID \
  { \
    0xbc412d75, 0x2729, 0x4c3a, 0xb1, 0x93, 0x5b, 0x9a, 0x58, 0x8f, 0xf6, 0x6f \
  }

#define EFI_NULL_GUID \
  { \
    0x00000000, 0x0000, 0x0000, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 \
  }

EFI_FORWARD_DECLARATION (FIRMWARE_PERFORMANCE_PROTOCOL);

//
// FPDT Record types
//
typedef enum {
  BASIC_BOOT_PTR_TYPE           = 0x0000,
  S3_TABLE_PTR_TYPE,
  BOOT_MODULE_TABLE_PTR_TYPE    = 0x1002,
  RUNTIME_MODULE_TABLE_PTR_TYPE,
  TIMESTAMP_DELTA_TYPE,
  HARDWARE_BOOT_TYPE,
  GUID_EVENT_REC_TYPE           = 0x1010,
  STRING_EVENT_REC_TYPE,
  BDS_ATTEMPT_EVENT_REC_TYPE,
  PERFORMANCE_RECORD_TYPE_MAX
} PERFORMANCE_RECORD_TYPE;

//
//  Progress Identifiers for Event Records
//
typedef enum {
  MODULE_START_ID               = 1,
  MODULE_END_ID,
  MODULE_LOADIMAGE_START_ID,
  MODULE_LOADIMAGE_END_ID,
  MODULE_DRIVERBINDING_START_ID,
  MODULE_DRIVERBINDING_END_ID
} EVENT_RECORD_PROGRESS_ID;

//
// Performance tokens
//
#define PEI_TOK                         L"PEI"
#define DXE_TOK                         L"DXE"
#define BDS_TOK                         L"BDS"
#define START_IMAGE_TOK                 L"StartImage"
#define LOAD_IMAGE_TOK                  L"LoadImage"
#define DRIVERBINDING_START_TOK         L"DriverBinding:Start"
#define DRIVERBINDING_SUPPORT_TOK       L"DriverBinding:Support"
#define BASIC_BOOT_TOK                  L"BasicBoot"
#define HARDWARE_BOOT_TOK               L"HardwareBoot"
#define EVENT_REC_TOK                   L"EventRec"
#define BDS_ATTEMPT_TOK                 L"BdsAttempt"
#define DXE_CORE_DISP_INIT_TOK          L"CoreInitializeDispatcher"
#define COREDISPATCHER_TOK              L"CoreDispatcher"
#define SMM_MODULE_TOK                  L"SmmModule"
#define SMM_FUNCTION_TOK                L"SmmFunction"

#define FIRMWARE_MAX_BUFFER             0x16800             // pre-defined buffer size of 90K to accomodate all FPDT records
#define RECORD_REVISION_1               0x1
#define RECORD_REVISION_2               0x2
#define STRING_EVENT_RECORD_NAME_LENGTH 24
#define RMPT_SIG                        EFI_SIGNATURE_32 ('R', 'M', 'P', 'T')
#define RUNTIME_MODULE_TABLE_PTR_TYPE   0x1003
#define RUNTIME_MODULE_REC_TYPE         0x1020
#define RUNTIME_FUNCTION_REC_TYPE       0x1021
#define DXE_START_ID                    0x7000
#define DXE_END_ID                      0x7001
#define DXE_CORE_DISP_START_ID          0x7010
#define DXE_CORE_DISP_END_ID            0x7011
#define COREDISPATCHER_START_ID         0x7020
#define COREDISPATCHER_END_ID           0x7021
//
// Fpdt record table structures
//
#pragma pack(push, 1)
typedef struct _BASIC_BOOT_REC {
  UINT16  RecType;
  UINT8   RecLength;
  UINT8   Revision;
  UINT32  Reserved;
  UINT64  ResetEnd;
  UINT64  BootLoaderLoadImage;
  UINT64  BootLoaderStartImage;
  UINT64  ExitBootServiceEntry;
  UINT64  ExitBootServiceExit;
} BASIC_BOOT_REC;

typedef struct _HARDWARE_BOOT_REC {
  UINT16  RecType;
  UINT8   RecLength;
  UINT8   Revision;
  UINT64  HardwareBoot;
} HARDWARE_BOOT_REC;

typedef struct _STRING_EVENT_REC {
  UINT16    RecType;
  UINT8     RecLength;
  UINT8     Revision;
  UINT16    ProgressID;
  UINT32    ApicID;
  UINT64    Timestamp;
  EFI_GUID  Guid;
  UINT8     NameString[STRING_EVENT_RECORD_NAME_LENGTH];
} STRING_EVENT_REC;

typedef struct _GUID_EVENT_REC {
  UINT16    RecType;
  UINT8     RecLength;
  UINT8     Revision;
  UINT16    ProgressID;
  UINT32    ApicID;
  UINT64    Timestamp;
  EFI_GUID  Guid;
} GUID_EVENT_REC;

typedef struct _BDS_ATTEMPT_REC {
  UINT16  RecType;
  UINT8   RecLength;
  UINT8   Revision;
  UINT32  ApicID;
  UINT16  BdsAttemptNo;
  UINT64  Timestamp;
  UINT64  UEFIBootVar;
  CHAR16  DevicePathString;
} BDS_ATTEMPT_REC;

typedef struct _RUNTIME_PERF_TABLE_HEADER {
  UINT32    Signature;
  UINT32    Length;
  EFI_GUID  Guid;
} RUNTIME_PERF_TABLE_HEADER;

typedef struct _RUNTIME_MODULE_PERF_RECORD {
  UINT16  RuntimeRecType;
  UINT8   Reclength;
  UINT8   Revision;
  UINT32  ModuleCallCount;
  UINT64  ModuleResidency;
} RUNTIME_MODULE_PERF_RECORD;

typedef struct _RUNTIME_FUNCTION_PERF_RECORD {
  UINT16  RuntimeRecType;
  UINT8   Reclength;
  UINT8   Revision;
  UINT32  Reserved;
  UINT32  FunctionId;
  UINT32  FunctionCallCount;
  UINT64  FunctionResidency;
} RUNTIME_FUNCTION_PERF_RECORD;

#pragma pack(pop)
//
// Firmware Performance Protocol definition
//
typedef
EFI_STATUS
(EFIAPI *FIRMWARE_PERFORMANCE_PROTOCOL_INSERT_MEASUREMENT) (
  IN FIRMWARE_PERFORMANCE_PROTOCOL            *This,
  IN EFI_HANDLE                               Handle,
  IN UINT16                                   RecordType,
  IN UINT64                                   Ticker,
  IN UINT16                                   Identifier OPTIONAL
  );

typedef
EFI_STATUS
(EFIAPI *FIRMWARE_PERFORMANCE_PROTOCOL_GET_PERFBUFFER_ADDR) (
  IN  FIRMWARE_PERFORMANCE_PROTOCOL *This,
  OUT UINT32                        *PerformanceBuffer
  );

typedef
UINT32
(EFIAPI *FIRMWARE_PERFORMANCE_PROTOCOL_GET_PERFBUFFER_LEN) (
  IN FIRMWARE_PERFORMANCE_PROTOCOL *This
  );


typedef struct _FIRMWARE_PERFORMANCE_PROTOCOL {
  FIRMWARE_PERFORMANCE_PROTOCOL_INSERT_MEASUREMENT  InsertMeasurement;
  FIRMWARE_PERFORMANCE_PROTOCOL_GET_PERFBUFFER_ADDR GetPerfBufferAddr;
  FIRMWARE_PERFORMANCE_PROTOCOL_GET_PERFBUFFER_LEN  GetPerfBufferLength;
} FIRMWARE_PERFORMANCE_PROTOCOL;

extern EFI_GUID gFirmwarePerformanceProtocolGuid;

#endif