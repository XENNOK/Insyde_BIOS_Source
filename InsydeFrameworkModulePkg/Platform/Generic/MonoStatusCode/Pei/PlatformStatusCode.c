//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PlatformStatusCode.c

Abstract:

  Contains WestBay specific implementations required to use status codes.

--*/

#include "MonoStatusCode.h"
#include "Port80StatusCodeLib.h"
#include "SerialStatusCodeLib.h"
#include "UsbStatusCodeLib.h"
#include "SimpleCpuIoLib.h"
#include "CmosStatusCodeLib.h"

#include EFI_GUID_DEFINITION (StatusCodeDataTypeId)
#include EFI_PROTOCOL_CONSUMER (StatusCodeReport)

//
// Ppi Consumed For Notification
//
#include EFI_PPI_CONSUMER (MemoryDiscovered)
#include EFI_PPI_CONSUMER (SioInitialized)
#include EFI_PPI_CONSUMER (LoadFile)

EFI_GUID                            gMonoStatusCodeFfsHeaderHob = {
  0xa69fd98b, 0x4e57, 0x4a74, 0x86, 0x4a, 0xfb, 0x1c, 0xed, 0xd9, 0xea, 0xfb
};

EFI_STATUS_CODE_REPORT_PROTOCOL    *mStatusCodeReport;

//
// Function declarations
//
EFI_STATUS
EFIAPI
SioInitializedPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );

EFI_STATUS
EFIAPI
LoadDxeReportStatusCode (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  );
//
// Global variables (not writable because we are in PEI)
//
static EFI_PEI_NOTIFY_DESCRIPTOR mSioInitializedNotify = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiSioInitializedPpiGuid,
  SioInitializedPpiNotifyCallback
};

static EFI_PEI_NOTIFY_DESCRIPTOR mMonoStatusCodeNotifyList = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiFvFileLoaderPpiGuid,
  LoadDxeReportStatusCode
};

//
// Lib globals used to for serial config defaults.
//
#ifdef EFI_DEBUG
#ifdef EFI_DEBUG_BASE_IO_ADDRESS
  UINT16 gComBase = EFI_DEBUG_BASE_IO_ADDRESS;
#else
  UINT16 gComBase = 0x3f8;
#endif
#ifdef EFI_DEBUG_BAUD_RATE
  UINTN gBps      = EFI_DEBUG_BAUD_RATE;
#else
  UINTN gBps      = 115200;
#endif
  UINT8   gData     = 8;
  UINT8   gStop     = 1;
  UINT8   gParity   = 0;
  UINT8   gBreakSet = 0;
#endif

//
// Function implementations
//
EFI_STATUS
EFIAPI
PlatformReportStatusCode (
  IN EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 *CallerId,
  IN EFI_STATUS_CODE_DATA     *Data OPTIONAL
  )
/*++

Routine Description:

  Call all status code listeners in the MonoStatusCode.

Arguments:

  Same as ReportStatusCode service

Returns:

  EFI_SUCCESS     Always returns success.

--*/
{
#ifdef H2O_DRIVER_CODE_DEBUG
  CmosReportStatusCode (PeiServices, CodeType, Value, Instance, CallerId, Data, mStatusCodeReport);
#endif

#ifndef H2O_PORT_80_DEBUG
  #ifdef EFI_PORT_80_DEBUG
    Port80ReportStatusCode (PeiServices, CodeType, Value, Instance, CallerId, Data);
  #endif
#endif
  //
  // If we are in debug mode, we will allow serial status codes
  //
#ifdef USB_DEBUG_SUPPORT
  DEBUG_CODE (
    {
      UsbReportStatusCode (PeiServices, CodeType, Value, Instance, CallerId, Data);
    }
  );
#else
  DEBUG_CODE (
    {
      EFI_STATUS   Status;
      EFI_PEI_HOB_POINTERS      Hob;

      if (PeiServices == NULL) {
        SerialReportStatusCode (PeiServices, CodeType, Value, Instance, CallerId, Data);

        return EFI_SUCCESS;
      }
      Status  = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);

      while (!END_OF_HOB_LIST (Hob)) {
        if (CompareGuid(&gPeiSioInitializedPpiGuid,&Hob.Guid->Name)) {
          SerialReportStatusCode (PeiServices, CodeType, Value, Instance, CallerId, Data);

          return EFI_SUCCESS;
        }
        Hob.Raw = GET_NEXT_HOB (Hob);
      }
      if (END_OF_HOB_LIST (Hob)) {

        return  EFI_NOT_FOUND ;
      }
    }
  );
#endif

  return EFI_SUCCESS;
}

VOID
PlatformInitializeStatusCode (
  VOID
  )
/*++

Routine Description:

  INIT the SIO. Ported this code and I don't undertand the comments either.

Arguments:

  FfsHeader   - FV this PEIM was loaded from.
  PeiServices - General purpose services available to every PEIM.

Returns:

  None

--*/
{
  //
  // If we are in debug mode, we will allow serial status codes
  //
  DEBUG_CODE (
    {
      //
      // Initialize additional debug status code listeners.
      //
      SerialInitializeStatusCode ();
    }
  );
}

EFI_STATUS
EFIAPI
SioInitializedPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  PlatformInitializeStatusCode ();
#ifdef EFI_DEBUG
{
  EFI_STATUS                   Status;
  EFI_HOB_GUID_TYPE            *Hob;

  Status = (*PeiServices)->CreateHob (PeiServices, EFI_HOB_TYPE_GUID_EXTENSION, sizeof (EFI_HOB_GUID_TYPE), &Hob);
  if (!EFI_ERROR (Status)) {
    Hob->Name = gPeiSioInitializedPpiGuid;
  }
}
#endif

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
LoadDxeReportStatusCode (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_FV_FILE_LOADER_PPI        *LoadFilePpi;
  EFI_PHYSICAL_ADDRESS              ImageAddress;
  UINT64                            ImageSize;
  EFI_PHYSICAL_ADDRESS              EntryPoint;
  EFI_FFS_FILE_HEADER               *FfsHeader = NULL;
  EFI_PEI_HOB_POINTERS              Hob;
  UINTN                             *Buffer;

  LoadFilePpi = (EFI_PEI_FV_FILE_LOADER_PPI *)Ppi;
  Status = (*PeiServices)->GetHobList (PeiServices, &Hob.Raw);
  while (!END_OF_HOB_LIST (Hob)) {
    if (Hob.Header->HobType == EFI_HOB_TYPE_GUID_EXTENSION && CompareGuid (&Hob.Guid->Name, &gMonoStatusCodeFfsHeaderHob)) {
      Buffer = (UINTN *)((UINT8 *) (&Hob.Guid->Name) + sizeof (EFI_GUID));
      FfsHeader = (EFI_FFS_FILE_HEADER *)(*Buffer);
      break;
    }

    Hob.Raw = GET_NEXT_HOB (Hob);
  }

  if (FfsHeader != NULL) {
    Status = LoadFilePpi->FvLoadFile (
                          LoadFilePpi,
                          FfsHeader,
                          &ImageAddress,
                          &ImageSize,
                          &EntryPoint
                          );
    if (EFI_ERROR (Status)) {

      return Status;
    }

    return (*(EFI_PEIM_ENTRY_POINT)(UINTN)EntryPoint) (NULL, PeiServices);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
PlatformInitializeDxeReportStatusCode (
  IN EFI_PEI_SERVICES       **PeiServices
  )
{
  EFI_STATUS             Status;
  EFI_HOB_GUID_TYPE      *Hob;

  Status = PeiBuildHobGuid (
             PeiServices,
             &gEfiStatusCodeReportProtocolGuid,
             sizeof (UINTN),
             &Hob
             );
  if (EFI_ERROR (Status)) {

    return Status;
  }

  Status = (**PeiServices).AllocatePool (
                            PeiServices,
                            sizeof (EFI_STATUS_CODE_REPORT_PROTOCOL),
                            &mStatusCodeReport
                            );
  if (EFI_ERROR (Status)) {
    mStatusCodeReport = NULL;

    return Status;
  }
  mStatusCodeReport->CurrentStatusCode  = 0x0;
  mStatusCodeReport->OutPutByStatusCode = FALSE;
  Hob++;
  (**PeiServices).CopyMem (Hob, &mStatusCodeReport, sizeof (UINTN));

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
MemoryDiscoveredPpiNotifyCallback (
  IN EFI_PEI_SERVICES       **PeiServices
  )
/*++

Routine Description:

  Once main memory has been initialized, build a hob describing the status code
  listener that has been installed.


Arguments:

  PeiServices      - General purpose services available to every PEIM.
  NotifyDescriptor - Information about the notify event.
  Ppi              - Context

Returns:

  EFI_SUCCESS     The function should always return success.

--*/
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     BootMode;

  Status = (*PeiServices)->GetBootMode (PeiServices, &BootMode);
  if (!EFI_ERROR(Status) && (BootMode == BOOT_ON_S3_RESUME)) {
    return Status;
  }
  //
  // Publish the listener in a HOB for DXE use.
  //
  InitializeDxeReportStatusCode (PeiServices);

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
InstallMonoStatusCode (
  IN EFI_FFS_FILE_HEADER       *FfsHeader,
  IN EFI_PEI_SERVICES          **PeiServices
  )
/*++

Routine Description:

  Install the PEIM.  Initialize listeners, publish the PPI and HOB for PEI and
  DXE use respectively.

Arguments:

  FfsHeader   - FV this PEIM was loaded from.
  PeiServices - General purpose services available to every PEIM.

Returns:

  EFI_SUCCESS   The function always returns success.

--*/
{
  EFI_STATUS    Status;

#ifndef EFI_BUILD_TARGET_X64
{
  if (FfsHeader == NULL) {
    return MemoryDiscoveredPpiNotifyCallback(PeiServices);
  }
}
#endif
  //
  //Shift DebugGroup and DebugCode in CMOS.
  //
#ifdef H2O_DRIVER_CODE_DEBUG
  ShiftDebugCodeInCmos ();
#endif
#ifdef USB_DEBUG_SUPPORT
  DEBUG_CODE (
    {
       //
       // Initialize additional USB debug status code listeners.
       //
       UsbInitializeStatusCode ();
    }
  );
#endif
  //
  // Initialize all listeners
  //
  InitializeMonoStatusCode (FfsHeader, PeiServices);

  Status = (**PeiServices).NotifyPpi (PeiServices, &mSioInitializedNotify);
  if (EFI_ERROR (Status)) {

    return Status;
  }
#ifndef EFI_BUILD_TARGET_X64
{
  //
  // Post a callback to make this status code service available to
  // DXE after memory has been initialized
  //
  EFI_HOB_GUID_TYPE                 *Hob;

  Status = PeiBuildHobGuid (
             PeiServices,
             &gMonoStatusCodeFfsHeaderHob,
             sizeof (UINTN),
             &Hob
             );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Hob++;
  (**PeiServices).CopyMem (Hob, &FfsHeader, sizeof (UINTN));

  Status = (**PeiServices).NotifyPpi (PeiServices, &mMonoStatusCodeNotifyList);
  if (EFI_ERROR(Status)) {

    return Status;
  }
}
#endif

  return EFI_SUCCESS;
}
