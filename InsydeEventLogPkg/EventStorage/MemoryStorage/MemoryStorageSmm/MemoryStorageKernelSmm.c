/** @file

  Memory Storage Kernel SMM implementation.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "MemoryStorageKernelSmm.h"

//
// Memory Storage Area Address, included Head size
//
EFI_PHYSICAL_ADDRESS                  gMemStorageHeadAddr;
//
// Memory Storage Area Address, NO included Head size
//
EFI_PHYSICAL_ADDRESS                  gMemStorageEntryAddr;

EFI_MEMORY_STORAGE_PROTOCOL           gMemoryStorageProtocol = {
      WriteEventToMemStorage,
      EfiClearMemStorageEvent,
      EfiReadNextMemStorageEvent,
      OverWriteEventToMemStorage
  };


/**
 Write (Log) Event to Memory Storage
            
 @param[in] SensorType      - Event Sensor Type
 @param[in] SensorNum       - Event Sensor Number
 @param[in] EventType       - Event Event Type
 @param[in] OptionDataSize  - Option Data Size
 @param[in] *OptionLogData  - Option Log Data
  
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
WriteEventToMemStorage (
  IN  UINT8                             SensorType,
  IN  UINT8                             SensorNum,
  IN  UINT8                             EventType,
  IN  UINTN                             OptionDataSize,
  IN  UINT8                             *OptionLogData
  )     
{
  EFI_STATUS                            Status;
  MEMORY_STORAGE_ORGANIZATION           *CurrentLog;
  MEMORY_STORAGE_ORGANIZATION           *Buffer;
  UINTN                                 NumBytes;
  EFI_PHYSICAL_ADDRESS                  *MemStorageCurrentLogAddr;

  if ((OptionDataSize == 0) || (OptionLogData == NULL)) {
    OptionDataSize = 0;
    OptionLogData = NULL;
  }

  MemStorageCurrentLogAddr = (EFI_PHYSICAL_ADDRESS *)((UINTN)gMemStorageHeadAddr + sizeof(gMemStorageHeadAddr));
  CurrentLog = (MEMORY_STORAGE_ORGANIZATION *) *MemStorageCurrentLogAddr;

  if (((UINTN)CurrentLog + CurrentLog->Length + MEMORY_STORAGE_BASE_LENGTH + OptionDataSize) >= 
       (UINTN)(gMemStorageEntryAddr + MEMORY_STORAGE_SIZE - sizeof(MEMORY_STORAGE_HEAD_STRUCTURE))) {
    //
    // Event Full
    //
    return EFI_OUT_OF_RESOURCES;
  }

  NumBytes = MEMORY_STORAGE_BASE_LENGTH + OptionDataSize;
  Status = gSmst->SmmAllocatePool (
                               EfiRuntimeServicesData,
                               NumBytes,
                               (VOID **)&Buffer
                               );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (Buffer, NumBytes);

  Buffer->Year   = ReadCmos8(0x09);
  Buffer->Month  = ReadCmos8(0x08);
  Buffer->Day    = ReadCmos8(0x07);
  Buffer->Hour   = ReadCmos8(0x04);
  Buffer->Minute = ReadCmos8(0x02);
  Buffer->Second = ReadCmos8(0x00);

  Buffer->SensorType = SensorType;
  Buffer->SensorNum  = SensorNum;
  Buffer->EventType  = EventType;
  Buffer->Length     = (UINT8)NumBytes;

  if (OptionLogData!= NULL) {
    CopyMem ((UINT8*)((UINTN)Buffer+NumBytes-OptionDataSize), OptionLogData, OptionDataSize);
  }

  //
  // Copy Event to Memory Storage
  //
  CopyMem ((UINT8*)((MEMORY_STORAGE_ORGANIZATION *)CurrentLog), (UINT8*)((MEMORY_STORAGE_ORGANIZATION *)Buffer), NumBytes);
  *MemStorageCurrentLogAddr =  (EFI_PHYSICAL_ADDRESS)((UINTN)CurrentLog + NumBytes);
  AsmWbinvd ();
  
  gSmst->SmmFreePool (Buffer);

  return EFI_SUCCESS;

}

/**
 Clear Event from Memory Storage          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EfiClearMemStorageEvent (
  VOID
  )       
{
  EFI_PHYSICAL_ADDRESS                  *MemStorageCurrentLogAddr;

  //
  // Set Memory Storage Area data to zero. DON'T clear Memory Storage Head Information
  //
  ZeroMem ((VOID *)gMemStorageEntryAddr, MEMORY_STORAGE_SIZE-sizeof(MEMORY_STORAGE_HEAD_STRUCTURE));
  //
  // Reset the Current Log Address of Memory Storage.
  //
  MemStorageCurrentLogAddr = (EFI_PHYSICAL_ADDRESS *)((UINTN)gMemStorageHeadAddr + sizeof(gMemStorageHeadAddr));
  *MemStorageCurrentLogAddr = (EFI_PHYSICAL_ADDRESS)gMemStorageEntryAddr; 
  AsmWbinvd ();

  return EFI_SUCCESS;
}

/**
 Read Next Event from Memory Storage

 @param[in]         MemStorageListAddress - Next Event Address            
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
EfiReadNextMemStorageEvent (
  IN OUT VOID                             **MemStorageListAddress
  )
{
  MEMORY_STORAGE_ORGANIZATION                *CurrentLog;
  MEMORY_STORAGE_HEAD_STRUCTURE              *MemStorageHead = NULL;

  //
  // Current Log is first log
  //
  CurrentLog = (MEMORY_STORAGE_ORGANIZATION *)(EFI_PHYSICAL_ADDRESS)gMemStorageEntryAddr;

  //
  // Check EventListAddress.
  // If it is NULL, and the first log exist, return first Log
  //
  if (*MemStorageListAddress == NULL) {
    if ((CurrentLog->SensorType == 0x00) && 
        (CurrentLog->SensorNum  == 0x00) &&
        (CurrentLog->EventType  == 0x00)) {

      return EFI_NOT_FOUND;
    }
    
    *MemStorageListAddress = (UINT8 *)(UINTN)gMemStorageEntryAddr;

    return EFI_SUCCESS;
  }
  
  //
  //Check current log
  //
  CurrentLog = (MEMORY_STORAGE_ORGANIZATION *)*MemStorageListAddress;
  
  MemStorageHead = (MEMORY_STORAGE_HEAD_STRUCTURE *)(gMemStorageEntryAddr - sizeof(MEMORY_STORAGE_HEAD_STRUCTURE));
  if (((UINTN)((UINTN)CurrentLog + CurrentLog->Length)) >= (UINTN)(MemStorageHead->MemStorageCurrentLogAddr)) {
    return EFI_NOT_FOUND;
  }    
  
  //
  // If the MemStorageListAddress out of range, return error
  //
  if ((((UINTN)CurrentLog + CurrentLog->Length) >= (UINTN)(gMemStorageEntryAddr + (MEMORY_STORAGE_SIZE -sizeof(MEMORY_STORAGE_HEAD_STRUCTURE)))) ||
      ((UINTN)CurrentLog < (UINTN)(gMemStorageEntryAddr)) ||
      (CurrentLog->Length == 0)){  
    return EFI_UNSUPPORTED;
  }

  //
  // If the MemStorageListAddress had not event exist, return
  //
  if ((CurrentLog->SensorType == 0x00) && 
      (CurrentLog->SensorNum  == 0x00) &&
      (CurrentLog->EventType  == 0x00)) {
  
    return EFI_NOT_FOUND;
  }

  //
  // Find out and point to the next Event Log
  //
  *MemStorageListAddress = (UINT8 *)( (UINT8*)CurrentLog + CurrentLog->Length);

  return EFI_SUCCESS;

}

/**
 Over Write (Log) Event to Memory Storage

 @param[in]         InputBuffer    - All Event Data format which included timestamped.           
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
OverWriteEventToMemStorage (
  IN  MEMORY_STORAGE_ORGANIZATION             *InputBuffer
  )
{
  EFI_STATUS                            Status;
  MEMORY_STORAGE_ORGANIZATION           *CurrentLog;
  MEMORY_STORAGE_ORGANIZATION           *Buffer;
  UINTN                                 NumBytes;
  UINTN                                 BaseLenght;
  EFI_PHYSICAL_ADDRESS                  *MemStorageCurrentLogAddr;

  if ((InputBuffer == NULL)) {
    return EFI_UNSUPPORTED;
  }

  MemStorageCurrentLogAddr = (EFI_PHYSICAL_ADDRESS *)((UINTN)gMemStorageHeadAddr + sizeof(gMemStorageHeadAddr));
  CurrentLog = (MEMORY_STORAGE_ORGANIZATION *) *MemStorageCurrentLogAddr;
  if (((UINTN)CurrentLog + CurrentLog->Length + InputBuffer->Length) >= 
      (UINTN)(gMemStorageEntryAddr + MEMORY_STORAGE_SIZE - sizeof(MEMORY_STORAGE_HEAD_STRUCTURE))) {
    //
    // Event Full
    //
    return EFI_OUT_OF_RESOURCES;
  }

  NumBytes = InputBuffer->Length;

  Status = gSmst->SmmAllocatePool (
                               EfiRuntimeServicesData,
                               NumBytes,
                               (VOID **)&Buffer
                               );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ZeroMem (Buffer, NumBytes);

  Buffer->Year   = InputBuffer->Year;
  Buffer->Month  = InputBuffer->Month;
  Buffer->Day    = InputBuffer->Day;
  Buffer->Hour   = InputBuffer->Hour;
  Buffer->Minute = InputBuffer->Minute;
  Buffer->Second = InputBuffer->Second;

  Buffer->SensorType = InputBuffer->SensorType;
  Buffer->SensorNum  = InputBuffer->SensorNum;
  Buffer->EventType  = InputBuffer->EventType;
  Buffer->Length     = InputBuffer->Length;

  BaseLenght = MEMORY_STORAGE_BASE_LENGTH;

  if (InputBuffer->Length > MEMORY_STORAGE_BASE_LENGTH) {
    CopyMem (
         (UINT8*)((UINTN)Buffer+NumBytes-(InputBuffer->Length - BaseLenght)), 
         &InputBuffer->Data[0], 
         InputBuffer->Length - BaseLenght
         );
  }

  //
  // Copy Event to Memory Storage
  //
  CopyMem ((UINT8*)((MEMORY_STORAGE_ORGANIZATION *)CurrentLog), (UINT8*)((MEMORY_STORAGE_ORGANIZATION *)Buffer), NumBytes);
  *MemStorageCurrentLogAddr =  (EFI_PHYSICAL_ADDRESS)((UINTN)CurrentLog + NumBytes);

  gSmst->SmmFreePool (Buffer);

  return Status;

}


/**
 Memory Storage Initial Entry.

 @param[in] ImageHandle       Image handle of this driver.
 @param[in] SystemTable       Global system service table.          
 
 @retval EFI Status                  
*/
EFI_STATUS
EFIAPI
MemoryStorageKernelSmmEntry (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                              Status;
  EFI_HANDLE                              SmmHandle;
  UINTN                                   VarSize;

  SmmHandle = NULL;
  
  //
  // Update Non-SMM Setting to SMM Driver
  //
  VarSize = sizeof (EFI_PHYSICAL_ADDRESS);
  Status = gRT->GetVariable (
                         L"MemoryStorage",
                         &gH2OMemoryStorageVariableGuid,
                         NULL,
                         &VarSize,
                         &gMemStorageHeadAddr
                         );
  if (EFI_ERROR (Status)) {
    return Status; 
  } 
  
  gMemStorageEntryAddr = gMemStorageHeadAddr + sizeof(MEMORY_STORAGE_HEAD_STRUCTURE);
  Status = gSmst->SmmInstallProtocolInterface (
                                           &SmmHandle,
                                           &gH2OSmmMemoryStorageProtocolGuid,
                                           EFI_NATIVE_INTERFACE,
                                           &gMemoryStorageProtocol
                                           );
  return Status;
}
