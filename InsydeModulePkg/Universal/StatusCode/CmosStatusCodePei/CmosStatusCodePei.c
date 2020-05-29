/** @file
  PEIM for Cmos Report Status Code

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

#include "CmosStatusCodePei.h"

//
// module variables
//
EFI_PEI_NOTIFY_DESCRIPTOR mNotifyOnRscHandlerPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiRscHandlerPpiGuid,
  RscHandlerPpiNotifyCallback 
};

/**
  Entry point of Cmos Status Code PEIM.

  This function is the entry point of this DXE Status Code Driver.
  It registers notify event for EFI_RSC_HANDLER_PROTOCOL installed.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
CmosStatusCodePeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                    Status;
  EFI_PEI_RSC_HANDLER_PPI       *RscHandlerPpi;

  //
  // Shift Status Code saved in Cmos during the last time boot
  //
  ShiftDebugCodeInCmos ();

  Status = PeiServicesLocatePpi (
             &gEfiPeiRscHandlerPpiGuid,
             0,
             NULL,
             (VOID **) &RscHandlerPpi
             );

  if (EFI_ERROR(Status)) {
    //
    // Notified when ReportStatusCodeRouter is ready.
    //
    return (*PeiServices)->NotifyPpi (PeiServices, &mNotifyOnRscHandlerPpiList);
  }

  //
  // LocatePpi success, register StatusCodeReportWorker now
  //
  Status = RscHandlerPpi->Register (CmosStatusCodeReportWorker);

  ASSERT_EFI_ERROR (Status);
  return Status;
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
  IN CONST  EFI_PEI_SERVICES        **PeiServices,
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN CONST EFI_GUID                 *CallerId,
  IN CONST EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
{
  EFI_STATUS                        Status;
  UINT8                             CodeValue;
  UINT8                             GroupValue;

  if (Data == NULL) {
    //
    // The DebugInfo was not sent by PeiCore Dispatcher.
    //
    return EFI_SUCCESS;
  }
  //
  // Progress or error code, Output to CMOS Offset 0x38 and 0x39
  //
  if (Value == (EFI_SOFTWARE_PEI_CORE | EFI_SW_PC_INIT_BEGIN)) {
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

  } else if (Value == (EFI_SOFTWARE_PEI_CORE | EFI_SW_PC_INIT_END)) {
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
EFI_STATUS
EFIAPI
RscHandlerPpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_RSC_HANDLER_PPI           *RscHandlerPpi;

  Status = PeiServicesLocatePpi (
             &gEfiPeiRscHandlerPpiGuid,
             0,
             NULL,
             (VOID **) &RscHandlerPpi
             );

  if (!EFI_ERROR(Status)) {
    //
    // Register the worker function to ReportStatusCodeRouter
    //
    Status = RscHandlerPpi->Register (CmosStatusCodeReportWorker);
  } 
  ASSERT_EFI_ERROR (Status);

  return Status;
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
EFIAPI
SearchDebugCodeValue (
  IN  CONST EFI_STATUS_CODE_DATA    *Data,
  OUT UINT8                         *DebugCodeValue,
  OUT UINT8                         *DebugGroupValue
  )
{
  EFI_DEVICE_HANDLE_EXTENDED_DATA   *ExtendedDataPtr;
  EFI_DEVICE_HANDLE_EXTENDED_DATA   *DataPtr;
  EFI_PEI_FILE_HANDLE               FileHandle;
  EFI_FV_FILE_INFO                  FvFileInfo;
  UINT32                            Index;
  EFI_STATUS                        Status;
  DEBUG_CODE_DATA                   *KernelDebugCodeTbl = NULL;   //retrieved from Pcd
  DEBUG_CODE_DATA                   *ChipsetDebugCodeTbl = NULL;  //retrieved from Pcd
  DEBUG_CODE_DATA                   EndEntry;

  ZeroMem (&EndEntry, sizeof (DEBUG_CODE_DATA));

  //
  // The ExtendedData was sent by Dispatcher of PeiCore
  //
  ExtendedDataPtr = (EFI_DEVICE_HANDLE_EXTENDED_DATA *)Data;
  DataPtr = (EFI_DEVICE_HANDLE_EXTENDED_DATA *)(&ExtendedDataPtr->Handle);
  FileHandle = DataPtr->Handle;

  //
  // Use PeiServicesLib to get file info.
  //
  Status = PeiServicesFfsGetFileInfo (
             FileHandle,
	     &FvFileInfo
	     );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  KernelDebugCodeTbl = (DEBUG_CODE_DATA *)PcdGetPtr (PcdPeiKernelDebugCodeTable);
  ChipsetDebugCodeTbl = (DEBUG_CODE_DATA *)PcdGetPtr (PcdPeiChipsetDebugCodeTable);

  //
  // Search the FileGuid in KernelDebugCodeTable and ChipsetDebugCodeTable
  //
  Status = EFI_NOT_FOUND;
  //
  // Search in KernelDebugCodeTbl
  //
  Index = 0;
  while (CompareMem (&EndEntry, &KernelDebugCodeTbl[Index], sizeof (DEBUG_CODE_DATA)) != 0) {
    if (CompareGuid (&FvFileInfo.FileName, (EFI_GUID *)&KernelDebugCodeTbl[Index].NameGuid)) {
      *DebugCodeValue  = KernelDebugCodeTbl[Index].DebugCode;
      *DebugGroupValue = KernelDebugCodeTbl[Index].DebugGroup;
      Status = EFI_SUCCESS;
      return Status;
    }
    Index++;
  }

  //
  // Search in ChipsetDebugCodeTbl
  //
  Index = 0;
  while (CompareMem (&EndEntry, &ChipsetDebugCodeTbl[Index], sizeof (DEBUG_CODE_DATA)) != 0) {
    if (CompareGuid (&FvFileInfo.FileName, (EFI_GUID *)&ChipsetDebugCodeTbl[Index].NameGuid)) {
      *DebugCodeValue  = ChipsetDebugCodeTbl[Index].DebugCode;
      *DebugGroupValue = ChipsetDebugCodeTbl[Index].DebugGroup;
      Status = EFI_SUCCESS;
      return Status;
    }
    Index++;
  }

  return Status;
}


/**
  Helper function to shift the DebugCode/DebugGroup written in Cmos during the last time boot.

  @param  none

  @retval none

**/    
VOID
ShiftDebugCodeInCmos (
  VOID
  )
{
  //
  // Shift the last Status Code written to Cmos in the last time boot.
  // There are 8 bytes (0x38~0x3F) used to save the Status Code.
  // Please refer to CmosLib.h.
  //
  WriteCmos8 (BackupDebugGroup3, ReadCmos8 (BackupDebugGroup2));
  WriteCmos8 (BackupDebugCode3,  ReadCmos8 (BackupDebugCode2));
  WriteCmos8 (BackupDebugGroup2, ReadCmos8 (BackupDebugGroup1));
  WriteCmos8 (BackupDebugCode2,  ReadCmos8 (BackupDebugCode1));
  WriteCmos8 (BackupDebugGroup1, ReadCmos8 (CurrentDebugGroup));
  WriteCmos8 (BackupDebugCode1,  ReadCmos8 (CurrentDebugCode));
}

