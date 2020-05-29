/** @file
  This driver provides some definitions for Smart Tool

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _IHISI_BIOSGCI_H_
#define _IHISI_BIOSGCI_H_

//
// The following codes are the sample. Please customize here.
// The default codes are some dummy function prototypes.
//
#if 0

#include "IhisiSmm.h"
#include <ChipsetSetupConfig.h>
#include <Protocol/SmmFwBlockService.h>
#include <Protocol/SmmVariable.h>

#define BIOSGCI_FUNCTIONS  \
          { 0x60, DataAccessCommunication  }, \
          { 0x61, DataRead                 }, \
          { 0x62, DataWrite                }

#define DATA_ACCESS_COMM_GUID_FOR_IDENTIFICATION            {0x582E8DF2, 0x07B1, 0x40E0, 0xAA, 0x37, 0x43, 0x2C, 0xB3, 0xAC, 0xDD, 0xCB}

//
// 0x00000000-0xDFFFFFFF, 0xF0000000-0xFFFFFFFF: reserved for user defined
//
#define DATA_ACCSEE_COMM_NUM_CREATE_UEFI_VARIABLE           0x00000000
#define DATA_ACCSEE_COMM_NUM_ERASE_UEFI_VARIABLE            0x00000001
#define DATA_ACCSEE_COMM_NUM_QUERY_DATA_FROM_UEFI_VARIABLE  0x00000002
#define DATA_ACCSEE_COMM_NUM_READ_UEFI_VARIABLE_DATA        0x00000003
#define DATA_ACCSEE_COMM_NUM_WRITE_UEFI_VARIABLE_DATA       0x00000004
//
// 0xE0000000-0xEFFFFFFF: reserved for AP used
//
#define DATA_ACCSEE_COMM_NUM_START_EVENT                    0xE0000000
#define DATA_ACCSEE_COMM_NUM_FINISH_EVENT                   0xE0000001

#define DATA_ACCSEE_COMM_NUM_START_EVENT_WINDOWS            0x00
#define DATA_ACCSEE_COMM_NUM_START_EVENT_DOS                0x01
#define DATA_ACCSEE_COMM_NUM_START_EVENT_EFI                0x02

#define DATA_ACCSEE_COMM_NUM_ERROR_RETURN_STATUS_PASS       0
#define DATA_ACCSEE_COMM_NUM_ERROR_RETURN_STATUS_FAIL       1

#define UEFI_VARIABLE_GUID      {0xFBCCF333, 0x78C8, 0x4C8D, 0x85, 0x26, 0x3C, 0xE3, 0xF0, 0x49, 0xFE, 0xB7}
#define TEST_GUID1              {0x174fd905, 0x7298, 0x4eb6, 0x80, 0x96, 0xb7, 0xa9, 0xd6, 0x97, 0xd2, 0x49}
#define TEST_GUID2              {0x8eed5985, 0x9bf3, 0x42a5, 0x8d, 0x30, 0xb4, 0xe6, 0x45, 0x4c, 0xeb, 0xd9}
#define TEST_GUID3              {0xc3d83e9c, 0x7f67, 0x41d3, 0x8e, 0xce, 0x9c, 0x94, 0xb0, 0x7f, 0x39, 0x6f}
#define TEST_GUID4              {0x3cb09da9, 0x7b8b, 0x4f83, 0xb8, 0x94, 0x7b, 0x7a, 0x4a, 0x85, 0x76, 0xae}
#define UEFI_VARIABLE_SIZE      0x100 //256bytes
#define UEFI_VARIABLE_OFFSET    0x20  //32bytes

#pragma pack(1)

//
// Related definitions for AH=60h, Data Access Communication
//
typedef struct {
  UINT32    Size;
  EFI_GUID  GuidForIdentification;
  UINT32    CommandNumber;
  UINT32    DataSize;
} DATA_ACCESS_COMMUNICATION_INPUT;

typedef struct {
  UINT64  ReservedForUserDef                : 32; // Reserved for user definition via tool generator.
  UINT64  ReservedForTool                   : 29; // Reserved for tool generator use only.
  UINT64  DataSizeNotAccept                 : 1;  // 1 - The data size is not accepted .
  UINT64  CommandNotSupport                 : 1;  // 1 - The command number is not supported.
  UINT64  GuidNotMatch                      : 1;  // 1 - GUID is not matched.
} DATA_ACCESS_COMM_STATUS_RETURN_GENERAL;

typedef struct {
  UINT64  CreateVariableResult              : 1;  // 0 - Create UEFI variable successfully.
                                                  // 1 - Create UEFI variable unsuccessfully.
  UINT64  Reserved                          : 63;
} DATA_ACCESS_COMM_STATUS_RETURN_CREATE_UEFI_VARIABLE;

typedef struct {
  UINT64  EraseVariableResult               : 1;  // 0 - Erase UEFI variable successfully.
                                                  // 1 - Erase UEFI variable unsuccessfully.
  UINT64  Reserved                          : 63;
} DATA_ACCESS_COMM_STATUS_RETURN_ERASE_UEFI_VARIABLE;

typedef struct {
  UINT64  QueryVariableResult               : 1;  // 0 - Query UEFI variable exist.
                                                  // 1 - Query UEFI variable doesn't exist.
  UINT64  Reserved                          : 63;
} DATA_ACCESS_COMM_STATUS_RETURN_QUERY_UEFI_VARIABLE_STATUS;

typedef union {
  DATA_ACCESS_COMM_STATUS_RETURN_GENERAL                        General;
  DATA_ACCESS_COMM_STATUS_RETURN_CREATE_UEFI_VARIABLE           CreateUefiVariable;
  DATA_ACCESS_COMM_STATUS_RETURN_ERASE_UEFI_VARIABLE            EraseUefiVariable;
  DATA_ACCESS_COMM_STATUS_RETURN_QUERY_UEFI_VARIABLE_STATUS     QueryUefiVariableStatus;
  UINT64                                                        ErrorStatus;
} DATA_ACCESS_COMM_STATUS_RETURN;

typedef struct {
  UINT32                            Size;
  EFI_GUID                          GuidForIdentification;
  UINT32                            CommandNumber;
  UINT32                            DataSize;
  DATA_ACCESS_COMM_STATUS_RETURN    StatusReturn;
} DATA_ACCESS_COMMUNICATION_OUTPUT;

//
// Related definitions for AH=61h/62h, Data Read/Data Write
//
typedef struct {
  UINT32    Size;
  UINT32    DataSize;
  UINT32    DataOffset;
} DATA_INPUT;

typedef struct {
  UINT32    Size;
  UINT32    DataSize;
  UINT32    DataOffset;
} DATA_OUTPUT;

#pragma pack()


//
// AH=60h, Get OEM1 information
//
EFI_STATUS
DataAccessCommunication (
  VOID
  );

//
// AH=61h, OEM Extra Data Communication
//
EFI_STATUS
DataRead (
  VOID
  );

//
// AH=62h, OEM Extra Data Write
//
EFI_STATUS
DataWrite (
  VOID
  );

EFI_STATUS
BiosGCIInit (
  VOID
  );

EFI_STATUS
Ihisi60hForCreateUefiVariable (
  IN OUT UINT32                  *IhisiStatus
  );

EFI_STATUS
Ihisi60hForEraseUefiVariable (
  IN OUT UINT32                  *IhisiStatus
  );

EFI_STATUS
Ihisi60hForQueryUefiVariableStatus (
  IN OUT UINT32                  *IhisiStatus
  );

EFI_STATUS
Ihisi60hForStartEvent (
  IN OUT UINT32                  *IhisiStatus,
  IN     UINT8                   StartType
  );

EFI_STATUS
Ihisi60hForFinishEvent (
  IN OUT UINT32                  *IhisiStatus
  );

#else

#define BIOSGCI_FUNCTIONS

//
// AH=60h, Get OEM1 information
//
EFI_STATUS
DataAccessCommunication (
  VOID
  );

//
// AH=61h, OEM Extra Data Communication
//
EFI_STATUS
DataRead (
  VOID
  );

//
// AH=62h, OEM Extra Data Write
//
EFI_STATUS
DataWrite (
  VOID
  );

EFI_STATUS
BiosGCIInit (
  VOID
  );

#endif

#endif
