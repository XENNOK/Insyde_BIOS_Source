/** @file
  Runtime DXE driver for USB Report Status Code

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

#include <Protocol/ReportStatusCodeHandler.h>

#include <Guid/EventGroup.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/PrintLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Guid/StatusCodeDataTypeId.h>
#include <Guid/StatusCodeDataTypeDebug.h>
#include <Library/H2ODebugLib.h>
#include <Library/ImageRelocationLib.h>
#include <Protocol/SmmReportStatusCodeHandler.h>
#include <Protocol/SmmBase2.h>

EFI_STATUS
EFIAPI
UsbInitializeStatusCode (
  VOID
  );

VOID
EFIAPI
UsbDebugPrint (
  IN CHAR8    *OutputString
  );


EFI_EVENT                     mExitBootServicesEvent     = NULL;
EFI_RSC_HANDLER_PROTOCOL      *mRscHandlerProtocol       = NULL;
EFI_SMM_RSC_HANDLER_PROTOCOL  *mSmmRscHandlerProtocol    = NULL;
EFI_SMM_SYSTEM_TABLE2         *mSmst                     = NULL;


/**
  Convert status code value and extended data to readable ASCII string, send string to USB debug port
 
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

  @retval EFI_SUCCESS      Status code reported to H2O DDT successfully.
  @retval EFI_DEVICE_ERROR USB debug device cannot work after ExitBootService() is called.
  @retval EFI_DEVICE_ERROR USB debug device cannot work with TPL higher than TPL_CALLBACK.

**/
EFI_STATUS
EFIAPI
UsbStatusCodeReportWorker (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
{
  CHAR8           *Filename;
  CHAR8           *Description;
  CHAR8           *Format;
  CHAR8           Buffer[EFI_STATUS_CODE_DATA_MAX_SIZE];
  UINT32          ErrorLevel;
  UINT32          LineNumber;
  UINTN           CharCount;
  BASE_LIST       Marker;

  CharCount = 0;
  Buffer[0] = '\0';

  if (Data != NULL &&
      ReportStatusCodeExtractAssertInfo (CodeType, Value, Data, &Filename, &Description, &LineNumber)) {
    //
    // Print ASSERT() information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "\n\rDXE_ASSERT!: %a (%d): %a",
                  Filename,
                  LineNumber,
                  Description
                  );
  } else if (Data != NULL &&
             ReportStatusCodeExtractDebugInfo (Data, &ErrorLevel, &Marker, &Format)) {
    //
    // Print DEBUG() information into output buffer.
    //
    CharCount = AsciiBSPrint (
                  Buffer, 
                  sizeof (Buffer), 
                  Format, 
                  Marker
                  );
  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
    //
    // Print ERROR information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer, 
                  sizeof (Buffer), 
                  "ERROR: C%x:V%x I%x", 
                  CodeType, 
                  Value, 
                  Instance
                  );
    ASSERT (CharCount > 0);
   
    if (CallerId != NULL) {
      CharCount += AsciiSPrint (
                     &Buffer[CharCount],
                     (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                     " %g",
                     CallerId
                     );
    }

    if (Data != NULL) {
      CharCount += AsciiSPrint (
                     &Buffer[CharCount],
                     (sizeof (Buffer) - (sizeof (Buffer[0]) * CharCount)),
                     " %x",
                     Data
                     );
    }
  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_PROGRESS_CODE) {
    if (FeaturePcdGet(PcdDdtStatusCodeReportsProgressCode)) {
      //
      // Print PROGRESS information into output buffer.
      //
      CharCount = AsciiSPrint (
                    Buffer,
                    sizeof (Buffer),
                    "PROGRESS CODE: V%x I%x",
                    Value,
                    Instance
                    );
    }
  } else if (Data != NULL &&
             CompareGuid (&Data->Type, &gEfiStatusCodeDataTypeStringGuid) &&
             ((EFI_STATUS_CODE_STRING_DATA *) Data)->StringType == EfiStringAscii) {
    //
    // EFI_STATUS_CODE_STRING_DATA
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "%a",
                  ((EFI_STATUS_CODE_STRING_DATA *) Data)->String.Ascii
                  );
  } else {
    //
    // Code type is not defined.
    //
    CharCount = AsciiSPrint (
                  Buffer, 
                  sizeof (Buffer), 
                  "Undefined: C%x:V%x I%x",
                  CodeType, 
                  Value, 
                  Instance
                  );
  }

  //
  // Call UsbDebugPrint function to do print.
  //
  if (CharCount > 0) {
    UsbDebugPrint ((CHAR8 *) Buffer);
  }

  return EFI_SUCCESS;
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
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  if (FeaturePcdGet (PcdStatusCodeUseUsb)) {
    mRscHandlerProtocol->Unregister (UsbStatusCodeReportWorker);
  }
}

/**
  Notification function for ReportStatusCode handler Protocol

  This routine is the notification function for Irsi Registration Protocol

  @param EFI_EVENT              Event of the notification
  @param Context                not used in this function

  @retval none

**/
VOID
EFIAPI
RscHandlerCallBack (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS      Status;
  
  Status = gBS->LocateProtocol (
                  &gEfiRscHandlerProtocolGuid,
                  NULL,
                  (VOID **) &mRscHandlerProtocol
                  );

  if (Status == EFI_SUCCESS) {
    mRscHandlerProtocol->Register (UsbStatusCodeReportWorker, TPL_HIGH_LEVEL);
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
}

/**
  Notification function for ReportStatusCode handler Protocol

  This routine is the notification function for Irsi Registration Protocol

  @param[in] Protocol   Points to the protocol's unique identifier.
  @param[in] Interface  Points to the interface instance.
  @param[in] Handle     The handle on which the interface was installed.

  @retval EFI_SUCCESS   Notification handler runs successfully.
**/
EFI_STATUS
EFIAPI
SmmRscHandlerCallBack (
  IN CONST EFI_GUID  *Protocol,
  IN VOID            *Interface,
  IN EFI_HANDLE      Handle
  )
{
  EFI_STATUS      Status;

  Status = mSmst->SmmLocateProtocol (
                  &gEfiSmmRscHandlerProtocolGuid,
                  NULL,
                  (VOID **) &mSmmRscHandlerProtocol
                  );

  if (Status == EFI_SUCCESS) {
    Status = mSmmRscHandlerProtocol->Register (UsbStatusCodeReportWorker);
  }
  return Status;
}


/**
  Entry point of USB Status Code Driver.

  This function is the entry point of this DXE Status Code Driver.
  It initializes registers status code handlers, and registers event for EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE.

  @param  ImageHandle       The firmware allocated handle for the EFI image.
  @param  SystemTable       A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
UsbStatusCodeRuntimeDxeEntry (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
  VOID                                *Registration;
  EFI_STATUS                          Status;
  EFI_SMM_BASE2_PROTOCOL              *SmmBase;
  BOOLEAN                             InSmm;

  if (!FeaturePcdGet(PcdStatusCodeUseUsb)) {
    return EFI_ABORTED;
  }

  InSmm        = FALSE;
  SmmBase      = NULL;
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  }



  if (!InSmm) {
    if (!IsRuntimeDriver (ImageHandle)) {
       Status = RelocateImageToRuntimeDriver (ImageHandle);
       if (EFI_ERROR(Status)) {
         ASSERT_EFI_ERROR (Status);
         return Status;
       }
       return EFI_ALREADY_STARTED;
    }
    EfiCreateProtocolNotifyEvent (
      &gEfiRscHandlerProtocolGuid,
      TPL_CALLBACK,
      RscHandlerCallBack,
      NULL,
      &Registration
      );
  } else {
    //
    // In SMM mode
    //

    Status = SmmBase->GetSmstLocation(SmmBase, &mSmst);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    mSmst->SmmRegisterProtocolNotify (
             &gEfiSmmRscHandlerProtocolGuid,
             SmmRscHandlerCallBack,
             &Registration
             );
  }
  return EFI_SUCCESS;
}
