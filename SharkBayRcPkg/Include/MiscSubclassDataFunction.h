//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

  Module Name:
    MiscSubclassDataFunction.h

  Abstract:

  Revision History

--*/

#ifndef _MISC_SUBCLASS_DATA_FUNCTION_H_
#define _MISC_SUBCLASS_DATA_FUNCTION_H_

//
// Data table entry update function.
//
typedef EFI_STATUS (EFIAPI EFI_MISC_SUBCLASS_DATA_FUNCTION)(
  IN UINT16                         RecordType,
  IN OUT UINT32                     *RecordLen,
  IN OUT EFI_MISC_SUBCLASS_RECORDS  *RecordData,
  OUT BOOLEAN                       *LogRecordData
  );

//
// Data table entry definition.
//
typedef struct {
  UINT16                          RecordType;
  UINT32                          RecordLen;
  UINT16                          Instance;
  VOID                            *RecordData;
  EFI_MISC_SUBCLASS_DATA_FUNCTION *Function;
} EFI_MISC_SUBCLASS_DATA_TABLE;

//
// Data Table extern definitions.
//
#define MISC_SUBCLASS_TABLE_EXTERNS(NAME1, NAME2) \
  extern NAME1 NAME2 ## Data; \
  extern EFI_MISC_SUBCLASS_DATA_FUNCTION NAME2 ## Function

#define MISC_SUBCLASS_TABLE_EXTERN_DATA(NAME1, NAME2) \
  extern NAME1 NAME2 ## Data; \

#define MISC_SUBCLASS_TABLE_EXTERN_FUNCTION(NAME1) \
  extern EFI_MISC_SUBCLASS_DATA_FUNCTION NAME1 ## Function

//
// Data Table entries
//
//
#define MISC_SUBCLASS_TABLE_ENTRY_DATA_ONLY(NAME1, NAME2, INSTANCE) \
{ \
  NAME1 ## _RECORD_NUMBER, \
  sizeof(NAME1), \
  INSTANCE, \
  & NAME2 ## Data, \
  NULL \
}

#define MISC_SUBCLASS_TABLE_ENTRY_FUNCTION_ONLY(NAME1, NAME2, INSTANCE) \
{ \
  NAME1 ## _RECORD_NUMBER,\
  0, \
  INSTANCE, \
  NULL, \
  & NAME2 ## Function \
}

#define MISC_SUBCLASS_TABLE_ENTRY_DATA_AND_FUNCTION(NAME1, NAME2, NAME3, INSTANCE) \
{ \
  NAME1 ## _RECORD_NUMBER, \
  sizeof(NAME1), \
  INSTANCE, \
  & NAME2 ## Data, \
  & NAME3 ## Function \
}

//
// Global definition macros.
//
#define MISC_SUBCLASS_TABLE_DATA(NAME1, NAME2) \
NAME1 NAME2 ## Data

#define MISC_SUBCLASS_TABLE_FUNCTION(NAME2) \
  EFI_STATUS EFIAPI NAME2 ## Function ( \
  IN UINT16 RecordType, \
  IN OUT UINT32 *RecordLen, \
  IN OUT EFI_MISC_SUBCLASS_RECORDS * RecordData, \
  OUT BOOLEAN *LogRecordData \
  )

#endif

