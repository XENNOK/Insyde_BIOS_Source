/** @file
  PEI Module for USB Report Status Code

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

#include <Ppi/ReportStatusCodeHandler.h>

#include <Guid/StatusCodeDataTypeId.h>
#include <Guid/StatusCodeDataTypeDebug.h>

#include <Library/DebugLib.h>
#include <Library/PrintLib.h>
#include <Library/ReportStatusCodeLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/BaseMemoryLib.h>
#include <Library/H2ODebugLib.h>

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

EFI_STATUS
EFIAPI
RscHandlerPpiNotifyCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  );

EFI_PEI_NOTIFY_DESCRIPTOR mNotifyOnRscHandlerPpiList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiRscHandlerPpiGuid,
  RscHandlerPpiNotifyCallback 
};


/**
  Convert status code value and extended data to readable ASCII string, send string to USB debug port.

  @param  PeiServices      An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param  CodeType         Indicates the type of status code being reported.
  @param  Value            Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. A system may contain multiple entities that match a class/subclass
                           pairing. The instance differentiates between them. An instance of 0 indicates
                           that instance information is unavailable, not meaningful, or not relevant.
                           Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS      Status code reported to USB debug port successfully.

**/
EFI_STATUS
EFIAPI
UsbStatusCodeReportWorker (
  IN CONST  EFI_PEI_SERVICES        **PeiServices,
  IN EFI_STATUS_CODE_TYPE           CodeType,
  IN EFI_STATUS_CODE_VALUE          Value,
  IN UINT32                         Instance,
  IN CONST EFI_GUID                 *CallerId,
  IN CONST EFI_STATUS_CODE_DATA     *Data OPTIONAL
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

  Buffer[0] = '\0';

  if (Data != NULL &&
      ReportStatusCodeExtractAssertInfo (CodeType, Value, Data, &Filename, &Description, &LineNumber)) {
    //
    // Print ASSERT() information into output buffer.
    //
    CharCount = AsciiSPrint (
                  Buffer,
                  sizeof (Buffer),
                  "PEI_ASSERT!: %a (%d): %a",
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
    
    ASSERT(CharCount > 0);
    
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
  UsbDebugPrint ((CHAR8 *) Buffer);

  return EFI_SUCCESS;
}

/**
  Notification service to be called when gEfiPeiRscHandlerPpiGuid is installed.

  @param  PeiServices           Indirect reference to the PEI Services Table.
  @param  NotifyDescriptor      Address of the notification descriptor data structure. Type
                                EFI_PEI_NOTIFY_DESCRIPTOR is defined above.
  @param  Ppi                   Address of the PPI that was installed.

  @retval EFI_STATUS            This function will install a PPI to PPI database. The status
                                code will be the code for (*PeiServices)->InstallPpi.

**/
EFI_STATUS
EFIAPI
RscHandlerPpiNotifyCallback (
  IN EFI_PEI_SERVICES              **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR     *NotifyDescriptor,
  IN VOID                          *Ppi
  )
{
  EFI_STATUS                  Status;
  EFI_PEI_RSC_HANDLER_PPI     *RscHandlerPpi;

  Status = PeiServicesLocatePpi (
             &gEfiPeiRscHandlerPpiGuid,
             0,
             NULL,
             (VOID **) &RscHandlerPpi
             );

  if (!EFI_ERROR(Status)) {
    //
    // Dispatch initialization request to sub-statuscode-devices.
    //
    if (FeaturePcdGet (PcdStatusCodeUseUsb)) {
      Status = RscHandlerPpi->Register (UsbStatusCodeReportWorker);
    }
  } 
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Entry point of USB Status Code PEIM.

  This function is the entry point of this Status Code PEIM.
  It initializes supported status code devices according to PCD settings,
  and installs Status Code PPI.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCESS  The entry point of DXE IPL PEIM executes successfully.

**/
EFI_STATUS
EFIAPI
UsbStatusCodePeiEntry (
  IN       EFI_PEI_FILE_HANDLE  FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{ 
  EFI_STATUS                  Status;
  EFI_PEI_RSC_HANDLER_PPI     *RscHandlerPpi;

  Status = PeiServicesLocatePpi (
             &gEfiPeiRscHandlerPpiGuid,
             0,
             NULL,
             (VOID **) &RscHandlerPpi
             );

  if (EFI_ERROR(Status)) {
      return (*PeiServices)->NotifyPpi (PeiServices, &mNotifyOnRscHandlerPpiList);
  }

  UsbInitializeStatusCode();

  //
  // Dispatch initialization request to sub-statuscode-devices.
  //
  if (FeaturePcdGet (PcdStatusCodeUseUsb)) {
    Status = RscHandlerPpi->Register (UsbStatusCodeReportWorker);
  }
  ASSERT_EFI_ERROR (Status);
  return Status;

}

