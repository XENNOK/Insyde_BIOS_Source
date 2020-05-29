/** @file

  Definition of Memory Storage protocol.
    
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

#ifndef _MEMORY_STORAGE_PROTOCOL_H_
#define _MEMORY_STORAGE_PROTOCOL_H_   

typedef struct {

  UINT8   SensorType;
  UINT8   SensorNum;
  UINT8   EventType;
  
  UINT8   Length;

  UINT8   Year;
  UINT8   Month;
  UINT8   Day;
  UINT8   Hour;
  UINT8   Minute;
  UINT8   Second;

  UINT8   Data[8];
  //
  //Log Variable Data is optional
  //
} MEMORY_STORAGE_ORGANIZATION;

#define MEMORY_STORAGE_BASE_LENGTH                               0x0A

/**
 Write (Log) Event to Memory Storage
            
 @param[in] SensorType      - Event Sensor Type
 @param[in] SensorNum       - Event Sensor Number
 @param[in] EventType       - Event Event Type
 @param[in] OptionDataSize  - Option Data Size
 @param[in] *OptionLogData  - Option Log Data
  
 @retval EFI Status                  
*/
typedef
EFI_STATUS
(EFIAPI *EFI_MEMORY_STORAGE_WRITE) (
  IN  UINT8                             SensorType,
  IN  UINT8                             SensorNum,
  IN  UINT8                             EventType,
  IN  UINTN                             OptionDataSize,
  IN  UINT8                             *OptionLogData
  );

/**
 Clear Event from Memory Storage          
 
 @retval EFI Status                  
*/
typedef
EFI_STATUS
(EFIAPI *EFI_MEMORY_STORAGE_CLEAR) (
  VOID
  );

/**
 Read Next Event from Memory Storage

 @param[in]         MemStorageListAddress - Next Event Address            
 
 @retval EFI Status                  
*/
typedef
EFI_STATUS
(EFIAPI *EFI_MEMORY_STORAGE_READ_NEXT) (
  IN OUT VOID                           **EventListAddress
  );

/**
 Over Write (Log) Event to Memory Storage

 @param[in]         InputBuffer    - All Event Data format which included timestamped.           
 
 @retval EFI Status                  
*/
typedef
EFI_STATUS
(EFIAPI *EFI_MEMORY_STORAGE_OVERWRITE) (
  IN  MEMORY_STORAGE_ORGANIZATION             *InputBuffer
  );

typedef struct _EFI_MEMORY_STORAGE_PROTOCOL {
  EFI_MEMORY_STORAGE_WRITE                     Write;
  EFI_MEMORY_STORAGE_CLEAR                     Clear;
  EFI_MEMORY_STORAGE_READ_NEXT                 ReadNext;  
  EFI_MEMORY_STORAGE_OVERWRITE                 OverWrite;
} EFI_MEMORY_STORAGE_PROTOCOL;

extern EFI_GUID gH2OMemoryStorageProtocolGuid;
extern EFI_GUID gH2OSmmMemoryStorageProtocolGuid;

#endif
