/** @file

   The definition of Event Storage Variable.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _EVENT_STORAGE_VARIABLE_H_
#define _EVENT_STORAGE_VARIABLE_H_

extern EFI_GUID gH2OEventStorageVariableGuid;

#define H2O_EVENT_STORAGE_VARIABLE_NAME        L"EventStorageVar"

#pragma pack(1)

typedef struct _H2O_EVENT_STORAGE_VARIABLE {
  BOOLEAN                   BiosEventStorageExist;
  UINT8                     BiosReserved[3];
  UINT32                    BiosEventStorageBaseAddress;
  UINT32                    BiosEventStorageLength;
  BOOLEAN                   MemoryEventStorageExist;
  UINT8                     MemoryReserved[3];
  UINT32                    MemoryEventStorageBaseAddress;
  UINT32                    MemoryEventStorageLength;
} H2O_EVENT_STORAGE_VARIABLE;

#pragma pack()

#endif
