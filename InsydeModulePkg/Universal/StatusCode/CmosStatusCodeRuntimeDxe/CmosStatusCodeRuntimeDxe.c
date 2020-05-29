/** @file
  Runtime DXE driver for Cmos Report Status Code

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

#include "CmosStatusCodeRuntimeDxe.h"

//
// module variables
//
UINTN                     mKernelDebugCodeTblSize = 0;
UINTN                     mChipsetDebugCodeTblSize = 0;
DEBUG_CODE_DATA           *mSortKernelTblPtr = NULL;
DEBUG_CODE_DATA           *mSortChipSetTblPtr = NULL;
EFI_EVENT                 mExitBootServicesEvent = NULL;
EFI_RSC_HANDLER_PROTOCOL  *mRscHandlerProtocol = NULL;


/**
  Entry point of Cmos Status Code Driver.

  This function is the entry point of this DXE Status Code Driver.
  It registers notify event for EFI_RSC_HANDLER_PROTOCOL installed.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
CmosStatusCodeRuntimeDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  VOID                      *Registration;
  DEBUG_CODE_DATA           *KernelDebugCodeTbl = NULL;   //retrieved from Pcd
  DEBUG_CODE_DATA           *ChipsetDebugCodeTbl = NULL;  //retrieved from Pcd
  DEBUG_CODE_DATA           EndEntry;
  UINTN                     NumOfEntries;
  UINTN                     TotalTblSize;
  
  ZeroMem (&EndEntry, sizeof (DEBUG_CODE_DATA));

  KernelDebugCodeTbl = (DEBUG_CODE_DATA *)PcdGetPtr (PcdDxeKernelDebugCodeTable);
  ChipsetDebugCodeTbl = (DEBUG_CODE_DATA *)PcdGetPtr (PcdDxeChipsetDebugCodeTable);
  
  //
  // Calculate the number of non-zero entries in the debug code table
  //
  NumOfEntries = 0;
  while (CompareMem (&EndEntry, &KernelDebugCodeTbl[NumOfEntries], sizeof (DEBUG_CODE_DATA)) != 0) {
    NumOfEntries++;
  }
  mKernelDebugCodeTblSize = NumOfEntries;    //excluding EndEntry
  TotalTblSize = mKernelDebugCodeTblSize * sizeof (DEBUG_CODE_DATA);

  //
  // initialize module variables:
  // array of kernel modules with debug codes and file guids
  //
  if (mKernelDebugCodeTblSize > 0) {
    mSortKernelTblPtr = (DEBUG_CODE_DATA *)AllocatePool (TotalTblSize);
    CopyMem (mSortKernelTblPtr, KernelDebugCodeTbl, TotalTblSize);
    SortDebugCodeTbl (mSortKernelTblPtr, mKernelDebugCodeTblSize);
  } else {
    mSortKernelTblPtr = NULL;
  }

  //
  // Calculate the number of non-zero entries in the debug code table
  //
  NumOfEntries = 0;
  while (CompareMem (&EndEntry, &ChipsetDebugCodeTbl[NumOfEntries], sizeof (DEBUG_CODE_DATA)) != 0) {
    NumOfEntries++;
  }
  mChipsetDebugCodeTblSize = NumOfEntries;    //excluding EndEntry
  TotalTblSize = mChipsetDebugCodeTblSize * sizeof (DEBUG_CODE_DATA);

  //
  // initialize module variables:
  // array of chipset modules with debug codes and file guids
  //
  if (mChipsetDebugCodeTblSize > 0) {
    mSortChipSetTblPtr = (DEBUG_CODE_DATA *)AllocatePool (TotalTblSize);
    CopyMem (mSortChipSetTblPtr, ChipsetDebugCodeTbl, TotalTblSize);
    SortDebugCodeTbl (mSortChipSetTblPtr, mChipsetDebugCodeTblSize);
  } else {
    mSortChipSetTblPtr = NULL;
  }

  EfiCreateProtocolNotifyEvent (
    &gEfiRscHandlerProtocolGuid,
    TPL_CALLBACK,
    RscHandlerCallBack,
    NULL,
    &Registration
    );  

  return EFI_SUCCESS;
}


/**
  Decode status code value, and write to Cmos.
 
  @param  CodeType         Indicates the type of status code being reported.
  @param  Value            Describes the current status of a hardware or software entity.
                           This included information about the class and subclass that is used to
                           classify the entity as well as an operation.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS      Status code is written to Cmos successfully.

**/
EFI_STATUS
EFIAPI
CmosStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
{
  EFI_STATUS                            Status;	
  UINT8                                 CodeValue;
  UINT8                                 GroupValue;

  if (Data == NULL) {
    //
    // The DebugInfo was not sent by DxeCore Dispatcher.
    //
    return EFI_SUCCESS;
  }

  //
  // Progress or error code, Output to CMOS Offset 0x38 and 0x39
  //
  if (Value == (EFI_SOFTWARE_DXE_CORE | EFI_SW_PC_INIT_BEGIN)) {
    //
    // Search in KernelDebugCodeTbl and ChipsetDebugCodeTbl
    //	  
    Status = SearchDebugCodeValue (Data, &CodeValue, &GroupValue);
    if (!EFI_ERROR (Status)) {
      WriteCmos8 (CurrentDebugGroup, GroupValue);
      WriteCmos8 (CurrentDebugCode, CodeValue);
    } else {
      //
      // Driver GUID is not found.
      //
      WriteCmos8 (CurrentDebugGroup, 0xFF);
      WriteCmos8 (CurrentDebugCode, 0xFF);
    }

  } else if (Value == (EFI_SOFTWARE_DXE_CORE | EFI_SW_PC_INIT_END)) {
    //
    // Reserve for expand.
    //
    return EFI_SUCCESS;
  }

  return EFI_SUCCESS;
}


/**
  Notification function for ReportStatusCode handler Protocol

  This routine is the notification function for EFI_RSC_HANDLER_PROTOCOL

  @param EFI_EVENT              Event of the notification
  @param Context                not used in this function

  @retval none

**/
VOID
EFIAPI
RscHandlerCallBack (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  EFI_STATUS      Status;
  
  Status = gBS->LocateProtocol (
                  &gEfiRscHandlerProtocolGuid,
                  NULL,
                  (VOID **) &mRscHandlerProtocol
                  );
  if (EFI_ERROR(Status)) {
    return;
  }

  //
  // Register the worker function to ReportStatusCodeRouter
  //
  mRscHandlerProtocol->Register (CmosStatusCodeReportWorker, TPL_HIGH_LEVEL);
  Status = gBS->CreateEventEx (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  UnregisterBootTimeHandlers,
                  NULL,
                  &gEfiEventExitBootServicesGuid,
                  &mExitBootServicesEvent
                  );
  ASSERT_EFI_ERROR (Status);

  gBS->CloseEvent(Event);
}


/**
  Unregister status code callback functions only available at boot time from
  report status code router when exiting boot services.

  @param  Event         Event whose notification function is being invoked.
  @param  Context       Pointer to the notification function's context, which is
                        always zero in current implementation.
**/
VOID
EFIAPI
UnregisterBootTimeHandlers (
  IN EFI_EVENT    Event,
  IN VOID         *Context
  )
{
  mRscHandlerProtocol->Unregister (CmosStatusCodeReportWorker);

  if (mSortKernelTblPtr != NULL) {
    FreePool (mSortKernelTblPtr);
  }
  if (mSortChipSetTblPtr != NULL) {
    FreePool (mSortChipSetTblPtr);
  }
}


/**
  Helper function to sort modules with debug code by Guids.

  @param  *Array              array of modules with debug code and guids to be sorted.
  @param  DataCount           number of items in the array.

  @retval None

**/
VOID
SortDebugCodeTbl (
  IN DEBUG_CODE_DATA    *Array,
  IN UINTN              DataCount
  )
{
  DEBUG_CODE_DATA   TempData;
  DEBUG_CODE_DATA   *TempPtr;
  UINTN             DataSize;
  UINTN             DataIndex;
  UINTN             DataIndex2;
  INT32             CmpValue;
  BOOLEAN           NoChange;

  DataSize = DataCount;
  TempPtr  = Array;
  NoChange = FALSE;

  //
  // bubble sort
  //
  for (DataIndex = 1; (DataIndex < DataSize) && (!NoChange); DataIndex++) {
    NoChange = TRUE;
    for (DataIndex2 = 0; DataIndex2 < (DataSize - DataIndex); DataIndex2++) {
      CmpValue = HelperCompareGuid ( (EFI_GUID*)(TempPtr + DataIndex2), (EFI_GUID*)(TempPtr + (DataIndex2 + 1)));
      if (CmpValue == 1) {
        TempData = *(TempPtr + DataIndex2);
        *(TempPtr + DataIndex2) = *(TempPtr + (DataIndex2 + 1));
        *(TempPtr + (DataIndex2 + 1)) = TempData;
        NoChange = FALSE;
      }
    }
  }
}


/**
  Helper function to search the module guid in KernelDebugCodeTable and ChipsetDebugCodeTable.
  The FileGuid of module is embedded in Data.

  @param  Data                   The ExtendedData sent by REPORT_STATUS_CODE_WITH_EXTENDED_DATA.
  @param  DebugCodeValue         The pointer to the value of the DebugCode found.
  @param  DebugGroupValue        The pointer to the value of the DebugGroup found.

  @retval EFI_INVALID_PARAMETER  Invalid file guid value.
  @retval EFI_NOT_FOUND          The module is not in the DebugCode tables.
  @retval EFI_SUCCESS            The module is found in the DebugCode table, 
                                 the DebugCode/DebugGroup values outputed successfully.

**/
EFI_STATUS
SearchDebugCodeValue (
  IN  CONST EFI_STATUS_CODE_DATA    *Data,
  OUT UINT8                         *DebugCodeValue,
  OUT UINT8                         *DebugGroupValue
  )
{
  EFI_STATUS                        Status;
  EFI_STATUS_CODE_DATA              *DataPtr;
  EFI_LOADED_IMAGE_PROTOCOL         *LoadedImage;
  EFI_GUID                          *NameGuid;
  UINTN                             IndexLow;
  UINTN                             IndexMid;
  UINTN                             IndexHigh;
  INT32                             CmpValue;
  EFI_HANDLE                        Handle;

  NameGuid = NULL;
  *DebugCodeValue = 0;
  *DebugGroupValue = 0;

  //
  // The ExtendedData was sent by Dispatcher of DxeCore
  //
  DataPtr = (EFI_STATUS_CODE_DATA *)Data;
  Handle = *(EFI_HANDLE *)(UINTN)(DataPtr + 1);

  //
  // Retrieve the FileGuid on LoadedImageProtocol
  //
  Status = gBS->HandleProtocol (
                  Handle,
		  &gEfiLoadedImageProtocolGuid, 
		  (VOID **)&LoadedImage
		  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (LoadedImage->FilePath == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  NameGuid = EfiGetNameGuidFromFwVolDevicePathNode ((MEDIA_FW_VOL_FILEPATH_DEVICE_PATH *)LoadedImage->FilePath);
  if (NameGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Search the FileGuid in KernelDebugCodeTable and ChipsetDebugCodeTable
  //
  Status     = EFI_NOT_FOUND;
  IndexLow   = 1;
  IndexHigh  = mKernelDebugCodeTblSize;
  //
  // binary search
  //
  if (mSortKernelTblPtr != NULL) {
    while (IndexLow <= IndexHigh) {
      IndexMid = (IndexLow + IndexHigh) / 2;
      CmpValue = HelperCompareGuid ((EFI_GUID *)&mSortKernelTblPtr[IndexMid - 1].NameGuid, NameGuid);
      if (CmpValue == 1) {
        IndexHigh = IndexMid - 1;
      } else if (CmpValue == -1) {
        IndexLow = IndexMid + 1;
      } else {
        *DebugCodeValue = mSortKernelTblPtr[IndexMid - 1].DebugCode;
        *DebugGroupValue = mSortKernelTblPtr[IndexMid - 1].DebugGroup;
        Status = EFI_SUCCESS;
        return Status;
      }
    }
  }

  //
  // binary search
  //
  if (mSortChipSetTblPtr != NULL) {
    IndexLow  = 1;
    IndexHigh = mChipsetDebugCodeTblSize;
    while (IndexLow <= IndexHigh) {
      IndexMid = (IndexLow + IndexHigh) / 2;
      CmpValue = HelperCompareGuid ((EFI_GUID *)&mSortChipSetTblPtr[IndexMid - 1].NameGuid, NameGuid);
      if (CmpValue == 1) {
        IndexHigh = IndexMid - 1;
      } else if (CmpValue == -1) {
        IndexLow = IndexMid + 1;
      } else {
        *DebugCodeValue = mSortChipSetTblPtr[IndexMid - 1].DebugCode;
        *DebugGroupValue = mSortChipSetTblPtr[IndexMid - 1].DebugGroup;
        Status = EFI_SUCCESS;
        return Status;
      }
    }
  }

  return Status;
}


/**
  Helper function to compare two guids.

  @param  *Guid1           guid to be compared.
  @param  *Guid2           guid to be compared.

  @retval 0                if Guid1 == Guid2
          1                if Guid1 > Guid2
          -1               if Guid1 < Guid2
**/       
INT32
HelperCompareGuid (
  IN EFI_GUID    *Guid1,
  IN EFI_GUID    *Guid2
  )
{
  UINTN Index;

  //
  // compare byte by byte
  //
  for (Index = 0; Index < 16; ++Index) {
    if (*(((UINT8*) Guid1) + Index) > *(((UINT8*) Guid2) + Index)) {

      return 1;
    } else if (*(((UINT8*) Guid1) + Index) < *(((UINT8*) Guid2) + Index)) {

      return -1;
    }
  }

  return 0;
}


