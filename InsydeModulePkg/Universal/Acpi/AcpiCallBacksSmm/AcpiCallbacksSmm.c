/** @file
  This driver is Common SMM AcpiCallbacks code

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiSmm.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseOemSvcKernelLib.h>

#include <Library/SmmOemSvcKernelLib.h>
#include <Protocol/SmmSwDispatch2.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmRuntime.h>
#include <Protocol/AcpiDisableCallbackDone.h>
#include <Protocol/AcpiEnableCallbackDone.h>
#include <Protocol/AcpiRestoreCallbackDone.h>

#include <SmiTable.h>
#include <PostCode.h>


STATIC
EFI_STATUS
EFIAPI
RegEnableAcpiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

STATIC
EFI_STATUS
EFIAPI
RegDisableAcpiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

STATIC
EFI_STATUS
EFIAPI
RegRestoreAcpiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  );

EFI_HANDLE                          mAcpiCallbacksHandle = NULL;
EFI_SMM_RUNTIME_PROTOCOL            *mSmmRT = NULL;

/**
  Creates and returns a notification event and registers that event with all the protocol
  instances specified by ProtocolGuid.

  This function causes the notification function to be executed for every protocol of type
  ProtocolGuid instance that exists in the system when this function is invoked. In addition,
  every time a protocol of type ProtocolGuid instance is installed or reinstalled, the notification
  function is also executed. This function returns the notification event that was created.
  If ProtocolGuid is NULL, then ASSERT().
  If NotifyTpl is not a legal TPL value, then ASSERT().
  If NotifyFunction is NULL, then ASSERT().
  If Registration is NULL, then ASSERT().


  @param[in]  ProtocolGuid    Supplies GUID of the protocol upon whose installation the event is fired.
  @param[in]  NotifyTpl       Supplies the task priority level of the event notifications.
  @param[in]  NotifyFunction  Supplies the function to notify when the event is signaled.
  @param[in]  NotifyContext   The context parameter to pass to NotifyFunction.
  @param[out] Registration    A pointer to a memory location to receive the registration value.
                              This value is passed to LocateHandle() to obtain new handles that
                              have been added that support the ProtocolGuid-specified protocol.

  @return The notification event that was created.
**/
STATIC
EFI_EVENT
EFIAPI
AcpiSmmCreateProtocolNotifyEvent (
  IN  EFI_GUID          *ProtocolGuid,
  IN  EFI_TPL           NotifyTpl,
  IN  EFI_EVENT_NOTIFY  NotifyFunction,
  IN  VOID              *NotifyContext,  OPTIONAL
  OUT VOID              **Registration
  )
{
  EFI_STATUS  Status;
  EFI_EVENT   Event;

  ASSERT (ProtocolGuid != NULL);
  ASSERT (NotifyFunction != NULL);
  ASSERT (Registration != NULL);

  //
  // Create the event
  //

  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  NotifyTpl,
                  NotifyFunction,
                  NotifyContext,
                  &Event
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Register for protocol notifications on this event
  //

  Status = gBS->RegisterProtocolNotify (
                  ProtocolGuid,
                  Event,
                  Registration
                  );

  ASSERT_EFI_ERROR (Status);

  return Event;
}

/**
  Callback for SMM runtime.

  For compatibility with EDK, if EFI_SMM_RUNTIME_PROTOCOL exists, system also installs
  EFI_SMM_FW_BLOCK_SERVICE_PROTOCOL to database in EFI_SMM_RUNTIME_PROTOCOL instance.

  @param    Event     Event whose notification function is being invoked.
  @param    Context   Pointer to the notification function's context.
**/
VOID
EFIAPI
SmmRuntimeCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;

  if (Event != NULL) {
    gBS->CloseEvent (Event);
  }

  Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRT);
  if (EFI_ERROR (Status)) {
    return;
  }
  Status = SmmRT->LocateProtocol (
                    &gEfiSmmRuntimeProtocolGuid,
                    NULL,
                    (VOID **)&mSmmRT
                    );
  return;
}

/**
 Register EnableAcpiCallback, DisableAcpiCallback, RestoreAcpiCallback to Dispatcher

 @param [in]   ImageHandle      Pointer to the loaded image protocol for this driver
 @param [in]   SystemTable      Pointer to the EFI System Table

 @retval Status                 EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SmmAcpiCallbacksEntryPoint (
  IN EFI_HANDLE                 ImageHandle,
  IN EFI_SYSTEM_TABLE           *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_SMM_SW_DISPATCH2_PROTOCOL         *SwDispatch2;
  EFI_SMM_SW_REGISTER_CONTEXT           SwContext;
  EFI_HANDLE                            SwHandle;
  EFI_EVENT                             Event;
  VOID                                  *Registration;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT = NULL;

  Status = gSmst->SmmLocateProtocol (&gEfiSmmSwDispatch2ProtocolGuid, NULL, (VOID **)&SwDispatch2);
  ASSERT_EFI_ERROR (Status);


  //
  // Register ACPI enable handler
  //
  SwContext.SwSmiInputValue = EFI_ACPI_ENABLE_SW_SMI;
  Status = SwDispatch2->Register (
    SwDispatch2,
    RegEnableAcpiCallback,
    &SwContext,
    &SwHandle
    );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "SmmCommunication SwSmi: %x\n", (UINTN)SwContext.SwSmiInputValue));

  //
  // Register ACPI disable handler
  //
  SwContext.SwSmiInputValue = EFI_ACPI_DISABLE_SW_SMI;
  Status = SwDispatch2->Register (
    SwDispatch2,
    RegDisableAcpiCallback,
    &SwContext,
    &SwHandle
    );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "SmmCommunication SwSmi: %x\n", (UINTN)SwContext.SwSmiInputValue));

  //
  // Register ACPI restore handler
  //
  SwContext.SwSmiInputValue = EFI_ACPI_RESTORE_SW_SMI;
  Status = SwDispatch2->Register (
    SwDispatch2,
    RegRestoreAcpiCallback,
    &SwContext,
    &SwHandle
    );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "SmmCommunication SwSmi: %x\n", (UINTN)SwContext.SwSmiInputValue));

  // TODO: Navtive driver need to remove
  Status = gBS->LocateProtocol(&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRT);
  if (EFI_ERROR (Status)) {
    Event = AcpiSmmCreateProtocolNotifyEvent (
              &gEfiSmmRuntimeProtocolGuid,
              TPL_CALLBACK,
              SmmRuntimeCallback,
              NULL,
              &Registration
              );
  } else {
    Status = SmmRT->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&mSmmRT);
  }


  return EFI_SUCCESS;
}

/**
 SMI handler to enable ACPI mode
 Dispatched on reads from APM port with value EFI_ACPI_ENABLE_SW_SMI
 Install gAcpiEnableCallbackStartProtocolGuid, gAcpiEnableCallbackDoneProtocolGuid protocol
 So platform get notification callbacks
 Call OemServices, so OEM gets chances to customize
 Call OemSvcEcAcpiMode(TRUE) and Show POST code


 @retval None.

**/
STATIC
EFI_STATUS
EFIAPI
RegEnableAcpiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{

  //
  // PostCode = 0xA6, OS call ACPI enable function
  //
  POST_CODE (SMM_ACPI_ENABLE_START);

  if (mAcpiCallbacksHandle != NULL) {
    gSmst->SmmUninstallProtocolInterface (
             mAcpiCallbacksHandle,
             &gAcpiEnableCallbackStartProtocolGuid,
             NULL
             );
    // TODO: Navtive driver need to remove
    if (mSmmRT != NULL) {
      mSmmRT->UninstallProtocolInterface (
                mAcpiCallbacksHandle,
                &gAcpiEnableCallbackStartProtocolGuid,
                NULL
                );
    }
  }

  gSmst->SmmInstallProtocolInterface (
           &mAcpiCallbacksHandle,
           &gAcpiEnableCallbackStartProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  // TODO: Navtive driver need to remove
  if (mSmmRT != NULL) {
    mSmmRT->InstallProtocolInterface(
              &mAcpiCallbacksHandle,
              &gAcpiEnableCallbackStartProtocolGuid,
              EFI_NATIVE_INTERFACE,
              NULL
              );
  }

  OemSvcEcAcpiMode (TRUE);
 
  //
  // OemServices
  //
  OemSvcEnableAcpiCallback ();

  if (mAcpiCallbacksHandle != NULL) {
    gSmst->SmmUninstallProtocolInterface (
             mAcpiCallbacksHandle,
             &gAcpiEnableCallbackDoneProtocolGuid,
             NULL
             );
    // TODO: Navtive driver need to remove
    if (mSmmRT != NULL) {
      mSmmRT->UninstallProtocolInterface (
                mAcpiCallbacksHandle,
                &gAcpiEnableCallbackDoneProtocolGuid,
                NULL
                );
    }
  }

  //
  // PostCode = 0xA7, ACPI enable function complete
  //
  POST_CODE (SMM_ACPI_ENABLE_END);

  gSmst->SmmInstallProtocolInterface (
           &mAcpiCallbacksHandle,
           &gAcpiEnableCallbackDoneProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  // TODO: Navtive driver need to remove
  if (mSmmRT != NULL) {
    mSmmRT->InstallProtocolInterface(
              &mAcpiCallbacksHandle,
              &gAcpiEnableCallbackDoneProtocolGuid,
              EFI_NATIVE_INTERFACE,
              NULL
              );
  }
  return EFI_SUCCESS;
}

/**
 SMI handler to disable ACPI mode
 Dispatched on reads from APM port with value EFI_ACPI_DISABLE_SW_SMI
 Install gAcpiDisableCallbackStartProtocolGuid, gAcpiDisableCallbackDoneProtocolGuid protocol
 So platform get notification callbacks
 Call OemServices, so OEM gets chances to customize
 Call OemSvcEcAcpiMode(FALSE) and Show POST code


 @retval None.

**/
STATIC
EFI_STATUS
EFIAPI
RegDisableAcpiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{
  //
  // PostCode = 0xA8, OS call ACPI disable function
  //
  POST_CODE (SMM_ACPI_DISABLE_START);

  if (mAcpiCallbacksHandle != NULL) {
    gSmst->SmmUninstallProtocolInterface (
             mAcpiCallbacksHandle,
             &gAcpiDisableCallbackStartProtocolGuid,
             NULL
             );
    // TODO: Navtive driver need to remove
    if (mSmmRT != NULL) {
      mSmmRT->UninstallProtocolInterface (
                mAcpiCallbacksHandle,
                &gAcpiDisableCallbackStartProtocolGuid,
                NULL
                );
    }
  }

  gSmst->SmmInstallProtocolInterface (
           &mAcpiCallbacksHandle,
           &gAcpiDisableCallbackStartProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  // TODO: Navtive driver need to remove
  if (mSmmRT != NULL) {
    mSmmRT->InstallProtocolInterface(
              &mAcpiCallbacksHandle,
              &gAcpiDisableCallbackStartProtocolGuid,
              EFI_NATIVE_INTERFACE,
              NULL
              );
  }

  OemSvcEcAcpiMode (FALSE);

  //
  // OemServices
  //
  OemSvcDisableAcpiCallback ();

  if (mAcpiCallbacksHandle != NULL) {
    gSmst->SmmUninstallProtocolInterface (
             mAcpiCallbacksHandle,
             &gAcpiDisableCallbackDoneProtocolGuid,
             NULL
             );
    // TODO: Navtive driver need to remove
    if (mSmmRT != NULL) {
      mSmmRT->UninstallProtocolInterface (
                mAcpiCallbacksHandle,
                &gAcpiDisableCallbackDoneProtocolGuid,
                NULL
                );
    }
  }

  //
  // PostCode = 0xA9, ACPI disable function complete
  //
  POST_CODE (SMM_ACPI_DISABLE_END);

  gSmst->SmmInstallProtocolInterface (
           &mAcpiCallbacksHandle,
           &gAcpiDisableCallbackDoneProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  // TODO: Navtive driver need to remove
  if (mSmmRT != NULL) {
    mSmmRT->InstallProtocolInterface(
              &mAcpiCallbacksHandle,
              &gAcpiDisableCallbackDoneProtocolGuid,
              EFI_NATIVE_INTERFACE,
              NULL
              );
  }
  return EFI_SUCCESS;
}

/**
 SMI handler to disable ACPI mode
 Dispatched on reads from APM port with value EFI_ACPI_DISABLE_SW_SMI
 Install gAcpiDisableCallbackStartProtocolGuid, gAcpiDisableCallbackDoneProtocolGuid protocol
 So platform get notification callbacks
 Call OemServices, so OEM gets chances to customize
 Call OemSvcEcAcpiMode(TRUE)


 @retval None.

**/
STATIC
EFI_STATUS
EFIAPI
RegRestoreAcpiCallback (
  IN EFI_HANDLE  DispatchHandle,
  IN CONST VOID  *Context         OPTIONAL,
  IN OUT VOID    *CommBuffer      OPTIONAL,
  IN OUT UINTN   *CommBufferSize  OPTIONAL
  )
{

  if (mAcpiCallbacksHandle != NULL) {
    gSmst->SmmUninstallProtocolInterface (
             mAcpiCallbacksHandle,
             &gAcpiRestoreCallbackStartProtocolGuid,
             NULL
             );
    // TODO: Navtive driver need to remove
    if (mSmmRT != NULL) {
      mSmmRT->UninstallProtocolInterface (
                mAcpiCallbacksHandle,
                &gAcpiRestoreCallbackStartProtocolGuid,
                NULL
                );
    }
  }

  gSmst->SmmInstallProtocolInterface (
           &mAcpiCallbacksHandle,
           &gAcpiRestoreCallbackStartProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  // TODO: Navtive driver need to remove
  if (mSmmRT != NULL) {
    mSmmRT->InstallProtocolInterface(
              &mAcpiCallbacksHandle,
              &gAcpiRestoreCallbackStartProtocolGuid,
              EFI_NATIVE_INTERFACE,
              NULL
              );
  }

  OemSvcEcAcpiMode (TRUE);
  
  //
  // OemServices
  //
  OemSvcRestoreAcpiCallback ();

  if (mAcpiCallbacksHandle != NULL) {
    gSmst->SmmUninstallProtocolInterface (
             mAcpiCallbacksHandle,
             &gAcpiRestoreCallbackDoneProtocolGuid,
             NULL
             );
    // TODO: Navtive driver need to remove
    if (mSmmRT != NULL) {
      mSmmRT->UninstallProtocolInterface (
                mAcpiCallbacksHandle,
                &gAcpiRestoreCallbackDoneProtocolGuid,
                NULL
                );
    }
  } 

  gSmst->SmmInstallProtocolInterface (
           &mAcpiCallbacksHandle,
           &gAcpiRestoreCallbackDoneProtocolGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  // TODO: Navtive driver need to remove
  if (mSmmRT != NULL) {
    mSmmRT->InstallProtocolInterface(
              &mAcpiCallbacksHandle,
              &gAcpiRestoreCallbackDoneProtocolGuid,
              EFI_NATIVE_INTERFACE,
              NULL
              );
  }
  return EFI_SUCCESS;
}

