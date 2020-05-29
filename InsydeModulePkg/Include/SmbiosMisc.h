/** @file
  Header file for the SmbiosMisc Driver.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SMBIOS_MISC_DRIVER_H
#define _SMBIOS_MISC_DRIVER_H

#include <Protocol/Smbios.h>
#include <IndustryStandard/SmBios.h>
#include <Library/HiiLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/CommonSmbiosLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Guid/DebugMask.h>

//
// Data table entry update function.
//
typedef EFI_STATUS (EFIAPI EFI_MISC_SMBIOS_DATA_FUNCTION) (
  IN  VOID                 *RecordData,
  IN  EFI_SMBIOS_PROTOCOL  *Smbios
  );


//
// Data table entry definition.
//
typedef struct {
  //
  // intermediat input data for SMBIOS record
  //
  VOID                              *RecordData;
  EFI_MISC_SMBIOS_DATA_FUNCTION     *Function;
} EFI_MISC_SMBIOS_DATA_TABLE;


//
// Data Table extern definitions.
//
#define MISC_SMBIOS_TABLE_EXTERNS(NAME1, NAME2, NAME3) \
extern NAME1 NAME2 ## Data; \
extern EFI_MISC_SMBIOS_DATA_FUNCTION NAME3 ## Function;


//
// Data Table entries
//

#define MISC_SMBIOS_TABLE_ENTRY_DATA_AND_FUNCTION(NAME1, NAME2) \
{ \
  & NAME1 ## Data, \
  & NAME2 ## Function \
}


//
// Global definition macros.
//
#define MISC_SMBIOS_TABLE_DATA(NAME1, NAME2) \
  NAME1 NAME2 ## Data

#define MISC_SMBIOS_TABLE_FUNCTION(NAME2) \
  EFI_STATUS EFIAPI NAME2 ## Function( \
  IN  VOID                  *RecordData, \
  IN  EFI_SMBIOS_PROTOCOL   *Smbios \
  )

//
// Data Table Array Entries
//
extern EFI_HII_HANDLE               mHiiHandle;

typedef struct _EFI_TYPE11_OEM_STRING{
  UINT8                               Offset;
  EFI_STRING_ID                       RefOemDefineString;
} EFI_TYPE11_OEM_STRING;

typedef struct _EFI_TYPE12_SYSTEM_CONFIGURATION_OPTIONS_STRING{
  UINT8                               Offset;
  EFI_STRING_ID                       RefType12SystemConfigurationOptionsString;
} EFI_TYPE12_SYSTEM_CONFIGURATION_OPTIONS_STRING;

typedef struct _EFI_TYPE13_BIOS_LANGUAGE_INFORMATION_STRING{
  UINT8                               *LanguageSignature;
  EFI_STRING_ID                       InstallableLanguageLongString;
  EFI_STRING_ID                       InstallableLanguageAbbreviateString;
} EFI_TYPE13_BIOS_LANGUAGE_INFORMATION_STRING;

typedef struct _EFI_TYPE40_ADDITIONAL_INFORMATION_ENTRY{
  UINT8           RefType;
  UINT8           RefOffset;
  EFI_STRING_ID   RefString;
  UINT8           Value;
} EFI_TYPE40_ADDITIONAL_INFORMATION_ENTRY;

typedef enum {
  STRING,
  DATA,
} OEM_DEFINE_TYPE;

typedef struct {
  OEM_DEFINE_TYPE                Type;
  UINTN                          Token;
  UINTN                          DataSize;
} OEM_DEFINE_INFO_STRING;

typedef struct {
  OEM_DEFINE_TYPE                Type;
  UINTN                          DataAddress;
  UINTN                          DataSize;
} OEM_DEFINE_INFO_DATA;

typedef union {
  OEM_DEFINE_INFO_STRING         DefineString;
  OEM_DEFINE_INFO_DATA           DefineData;
} EFI_OEM_DEFINE_ARRAY;

EFI_STATUS
FindString (
  IN UINT8                              Type,
  IN UINT8                              Offset,
  IN EFI_STRING_ID                      TokenToUpdate
);

EFI_STATUS
FindUuid (
  EFI_GUID                    *Uuid
);

EFI_STATUS
StringToBiosVeriosn (
  IN  EFI_STRING_ID                     BiosVersionToken,
  OUT UINT8                             *MajorVersion,
  OUT UINT8                             *MinorVersion
);


#endif
