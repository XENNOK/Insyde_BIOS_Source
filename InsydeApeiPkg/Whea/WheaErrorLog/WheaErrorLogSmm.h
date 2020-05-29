/** @file

  Definitions for Whea Error log driver

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _WHEA_ERROR_LOG_SMM_H_
#define _WHEA_ERROR_LOG_SMM_H_

//=================================================
//  MdePkg\Include\
//=================================================
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>

#include <Protocol/FirmwareVolumeBlock.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmSwDispatch2.h>

//=================================================
//  InsydeModulePkg\Include\
//=================================================
#include <Protocol/SmmFwBlockService.h>

//=================================================
//  $(INSYDE_APEI_PKG)\Include\
//=================================================
#include <WheaAcpi.h>
#include <Protocol/WheaSupport.h>

#define WHEA_ERROR_RECORD_SWSMI 0x9C

#define ErstRegisterExecute { \
  EFI_ACPI_3_0_SYSTEM_IO,     \
  8,                          \
  0,                          \
  EFI_ACPI_3_0_BYTE,          \
  0xB2}

//[-start-131212-IB10310041-modify]//
#define ErstRegisterFiller  { \
  EFI_ACPI_3_0_SYSTEM_MEMORY, \
  64,                         \
  0,                          \
  EFI_ACPI_3_0_QWORD,         \
  0}

#pragma pack (1)

#define  EFI_ERROR_SIGNATURE           SIGNATURE_32('C', 'P', 'E', 'R')

#define  DEFRAG_BELOW_ONE_FOURTH       0
#define  DEFRAG_BELOW_ONE_THIRD        1
#define  DEFRAG_BELOW_HALF             2
#define  DEFRAG_EVERY_ERROR            3

#define  ELOG_PROCESS_SUCCESS          ERST_STATUS_SUCCESS
#define  ELOG_NOT_ENOUGH_SPACE         ERST_STATUS_NOT_ENOUGH_SPACE
#define  ELOG_FAILED                   ERST_STATUS_FAILED
#define  ELOG_RECORD_STORE_EMPTY       ERST_STATUS_RECORD_STORE_EMPTY
#define  ELOG_RECORD_NOT_FOUND         ERST_STATUS_RECORD_NOT_FOUND
#define  RECORD_IN_USE                 1

typedef struct {
  UINT32                Signature;
  UINT16                Revision;
  UINT32                SignatureEnd;
  UINT16                SectionCount;
  UINT32                ErrorSeverity;
  UINT32                ValidationBits;
  UINT32                RecordLength;
  UINT64                TimeStamp;
  EFI_GUID              PlatformID;
  EFI_GUID              PartitionID;
  EFI_GUID              CreatorID;
  EFI_GUID              NotificationType;
  UINT64                RecordID;
  UINT32                Flag;
  UINT64                PersistenceInfoAttribute; ///offset 6Ch
  UINT64                Reserved0;
  UINT32                Reserved1;
} ERROR_RECORD_HEADER;
//[-end-131212-IB10310041-modify]//

typedef struct {
  UINT64                Command;                    // Action 0~3, 11
  UINT64                LogOffset;                  // Action 4
  UINT64                BusyFlag;                   // Action 6
  UINT64                CommandStatus;              // Action 7
  UINT64                CurRecordID;                // Action 8
  UINT64                RecordID;                   // Action 9
  UINT64                RecordCount;                // Action 10
  UINT64                Reserved;                   // Action 12
  UINT64                ErrorLogAddressRange;       // Action 13
  UINT64                ErrorLogAddressLength;      // Action 14
  UINT64                ErrorLogAddressAttributes;  // Action 15
} WHEA_PARAMETER_REGION;

typedef struct {
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL  *SmmFwBlock;
  EFI_PHYSICAL_ADDRESS               BaseAddress;          // Physical Address where Error Records start
  UINT32                             ElogLength;           // ELOG Space for Records in bytes (excluding FV HeaderLength)
  UINT32                             TotalRecordBytes;     // Error Record Bytes in ELOG including cleared
  UINT32                             FreeRecordBytes;      // ElogLength - TotalRecordBytes
  EFI_PHYSICAL_ADDRESS               NextRecordAddress;    // Address where a new record should be appended to
  UINT16                             HeaderLength;         // FV Header
  EFI_FVB_ATTRIBUTES_2               FvBlockAttrubutes;    // EFI FVB attribute
  EFI_PHYSICAL_ADDRESS               FvBlockPhysicalAddress;
  WHEA_PARAMETER_REGION              *ParaRegion;
  UINTN                              DefragmentPolicy;     // SCU defragment setting.
} EFI_WHEA_ERROR_RECORD_INFORMATION;  

#pragma pack()

/**
  BIOS SMI handler for handling WHEA error records.

  @param [in]      DispatchHandle The handle of this callback, obtained when registering.
  @param [in]      Context        The predefined context which contained sleep type and phase.
  @param [in, out] CommBuffer     Common Buffer.
  @param [in, out] CommBufferSize Common Buffer Size.

  @retval EFI_SUCCESS             Error Injection Log successfully.

**/
EFI_STATUS
InsydeWheaElogSwSmiCallback (
  IN       EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                        *Context,        OPTIONAL
  IN OUT   VOID                        *CommBuffer,     OPTIONAL
  IN OUT   UINTN                       *CommBufferSize  OPTIONAL
);


/**
  Update ERST information and install it. And update mailbox information.

  @param [in] None

  @retval EFI_SUCCESS             All procedures work successfully.

**/
EFI_STATUS
InsydeWheaUpdateERST (
  VOID
);

/**
  Update information in EFI_WHEA_ERROR_RECORD_INFORMATION structure for using under OS run time.
  Clear the OEM FV logo in the FV at the first time post.

  @param [in]      DispatchHandle The handle of this callback, obtained when registering.
  @param [in]      Context        The predefined context which contained sleep type and phase.
  @param [in, out] CommBuffer     Common Buffer.
  @param [in, out] CommBufferSize Common Buffer Size.

  @retval EFI_SUCCESS             Error Injection Log successfully.

**/
EFI_STATUS
InsydeWheaRomInit (
  VOID
);

/**
  Do defragment depending on the SCU setting.
  Allocate buffer for storing the NV header and all the IN USE error records.
  Erase the total OEM NV, and write the buffer content to OEM NV.

  @param [in] None

  @retval EFI_SUCCESS

**/
EFI_STATUS
InsydeWheaRomDefragment (
  VOID
);


/**
  Erase WHEA Blocks in FV and Write new data from Buffer. 
  And maintain the global variable "ErrorRecordInfo".

  @param [in] None

  @retval None

**/
VOID
InsydeWheaRomEraseWrite (
  IN UINTN                             *EraseSize,
  IN UINTN                             *WriteSize,
  IN UINT8                             *WriteBuffer
);


/**
  Find a ELOG record using RecordID as Key.
  If the RecordID is 0, out the first record physical address.

  @param [in] RecordID            Identifier of the record that should be searched
  @param [in] PhyAddress          The matched error record address if found
  @param [in] FindNextRecordID    If true, search the entire FV for updating the Current Record ID in Parameter Region.
  @param [in] FirstRecordID       If not NULL, return the first IN USE error record in the NV.

  @retval Defined by WHEA Specification as below
                                  0x00  Success
                                  0x04  Record Store Empty
                                  0x05  Record Not Found

**/

UINT64
InsydeWheaRomSearch (
  IN  UINT64                           RecordID,
  OUT UINTN                            *PhyAddress,
  IN  BOOLEAN                          FindNextRecordID,
  OUT UINT64                           *FirstRecordID
);

/**
  Read a ELOG record into the buffer provided.
 
  @param [in] RecordBuffer        Buffer where the record should be returned
  @param [in] RecordID            Identifier of the record that should be searched and read to buffer

  @retval Defined by WHEA Specification as below
                                  0x00  Success
                                  0x04  Record Store Empty
                                  0x05  Record Not Found

**/
UINT64
InsydeWheaRomRead (
  IN EFI_PHYSICAL_ADDRESS            RecordBuffer,
  IN UINT64                          RecordID
);

/**
  If the record already exists, mark it as cleared.
  Always append the new record in the end.
 
  @param [in] NewRecord         The address of new record which is to be written..

  @retval Defined by WHEA Specification as below
                                  0x00  Success
                                  0x01  Not Enough Space
                                  0x02  Hardware Not Available
                                  0x03  Failed

**/
UINT64
InsydeWheaRomWrite(
  IN EFI_PHYSICAL_ADDRESS              Record
);

/**
  Search for the record with ID. if found, clear the error record.
 
  @param [in] RecordID            The Record which OS want to clear.

  @retval Defined by WHEA Specification as below
                                  0x00  Success
                                  0x01  Not Enough Space
                                  0x02  Hardware Not Available
                                  0x03  Failed

**/
UINT64
InsydeWheaRomClear (
  IN UINT64                            RecordID
);

#endif
