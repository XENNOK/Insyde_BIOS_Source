/** @file
  Legacy Functions for USB

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "UsbCoreDxe.h"
#include "Legacy.h"
#include "MemMng.h"
#include "UsbPciIo.h"

STATIC
EFI_STATUS
EFIAPI
PeriodicTimerCallback (
  IN  VOID          *Data,
  IN  UINTN         DataLength,
  IN  VOID          *Context,
  IN  UINT32        Result
  )
{
  PERIODIC_TIMER_TOKEN        *PeriodicTimerToken;
  PERIODIC_TIMER_TOKEN        *NextPeriodicTimerToken;
  EFI_USB3_HC_CALLBACK        Callback;
  UINTN                       PollingInterval;

  PollingInterval = (UINTN)Context;
  PeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)GetFirstNode(&mPrivate->PeriodicTimerListHead);
  while (PeriodicTimerToken != (PERIODIC_TIMER_TOKEN*)&mPrivate->PeriodicTimerListHead) {
    //
    // Setup the next link to preventing the link be destroyed by callback
    //
    NextPeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)GetFirstNode(&PeriodicTimerToken->Link);
    if (PeriodicTimerToken->Unit == PollingInterval) {
      PeriodicTimerToken->Countdown -= PollingInterval;
      if (PeriodicTimerToken->Countdown <= 0) {
        //
        // Backup PeriodicTimerToken
        //
        Callback = PeriodicTimerToken->Callback;
        Context  = PeriodicTimerToken->Context;
        if (PeriodicTimerToken->Type == USB_CORE_PERIODIC_TIMER) {
          PeriodicTimerToken->Countdown = PeriodicTimerToken->Timeout;
        } else if (PeriodicTimerToken->Type == USB_CORE_ONCE_TIMER) {
          RemovePeriodicTimer(PeriodicTimerToken);
        }
        //
        // Launchs the callback
        //
        Callback(0, Context);
        //
        // The Callback function may kill the next entity in PeriodicTimerList. Check it out and forward to next link
        //
        if (!CheckExist(&mPrivate->PeriodicTimerListHead, (LIST_ENTRY*)NextPeriodicTimerToken)) {
          NextPeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)GetFirstNode(&PeriodicTimerToken->Link);
        }
      }
    }
    PeriodicTimerToken = NextPeriodicTimerToken;
  }
  return EFI_SUCCESS;
}

/**

  Timer handlers for IRQ based periodic timer

  @param  Event type
  @param  Context fo the event

**/
STATIC
VOID
EFIAPI
PeriodicTimerNotifyFunction (
  IN     UINTN      Event,
  IN     VOID       *Context
  )
{
  if (Event == LEGACY_FREE_HC_TIMER_EVENT) {
    PeriodicTimerCallback (
      NULL,
      0,
      Context,
      0
      );
  }
}

/**

  Get next Usb3HC protocol in ProviderListHead

  @param  ListEntry             Pointer of LIST_ENTRY for linking list
  @param  Provider              Pointer Usb3Hc protocol storage

**/
STATIC
VOID
GetNextProvider(
  IN     LIST_ENTRY                     *ListEntry,
  IN OUT EFI_USB3_HC_PROTOCOL           **Provider
  )
{
  PROVIDER_TOKEN  *ProviderToken;
  PROVIDER_TOKEN  *NextProviderToken;

  ProviderToken = (PROVIDER_TOKEN*)ListEntry;
  do {
    ProviderToken = (PROVIDER_TOKEN*)GetFirstNode((LIST_ENTRY*)ProviderToken);
    if (ProviderToken->Usb3Hc == *Provider) {
      if (IsNodeAtEnd(ListEntry, (LIST_ENTRY*)ProviderToken)) {
        NextProviderToken = (PROVIDER_TOKEN*)GetFirstNode(ListEntry);
      } else {
      	NextProviderToken = (PROVIDER_TOKEN*)GetFirstNode((LIST_ENTRY*)ProviderToken);
      }
      *Provider = NextProviderToken->Usb3Hc;
      break;
    }
  } while (!IsNodeAtEnd(ListEntry, (LIST_ENTRY*)ProviderToken));
}

/**

  Insert a periodic timer callback

  @param  Type                  USB_CORE_PERIODIC_TIMER / USB_CORE_ONCE_TIMER
  @param  Callback              Callback function
  @param  Context               Context for callback function
  @param  MilliSecond           Countdown in milli-second
  @param  Handle                Pointer of Handle for output

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
InsertPeriodicTimer (
  IN     UINTN                          Type,
  IN     EFI_USB3_HC_CALLBACK           Callback,
  IN     VOID                           *Context,
  IN     UINTN                          MilliSecond,
  IN OUT VOID                           **Handle
  )
{
  EFI_STATUS            Status;
  PERIODIC_TIMER_TOKEN  *PeriodicTimerToken;
  UINTN                 ExistInterval;
  UINTN                 RequiredInterval;
  UINTN                 Zero;
  UINTN                 PollingInterval;
  EFI_USB_HC_STATE      State;
  EFI_USB3_HC_PROTOCOL  *Provider;
  BOOLEAN               TokenFound;
  UINTN                 Mode;
  //
  // Enter critical section to privent SMI interfere with priodic timer
  //
  EnterCriticalSection();
  Status          = EFI_SUCCESS;
  TokenFound      = FALSE;
  ExistInterval   = 0;
  if (MilliSecond < MAXIMUM_POLLING_INTERVAL) {
    PollingInterval  = MINIMUM_POLLING_INTERVAL;
    RequiredInterval = 0x01;
  } else {
    PollingInterval  = MAXIMUM_POLLING_INTERVAL;
    RequiredInterval = 0x02;
  }
  //
  // Search for list to find out is needed to startup basic timer 
  //
  PeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)&mPrivate->PeriodicTimerListHead;
  do {
    if (IsListEmpty(&mPrivate->PeriodicTimerListHead)) break;
    PeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)GetFirstNode((LIST_ENTRY*)PeriodicTimerToken);
    if (PeriodicTimerToken == *Handle && Type == USB_CORE_PERIODIC_TIMER) TokenFound = TRUE;
    if (PeriodicTimerToken->Unit == MINIMUM_POLLING_INTERVAL) ExistInterval |= 0x01;
    if (PeriodicTimerToken->Unit == MAXIMUM_POLLING_INTERVAL) ExistInterval |= 0x02;
  } while (!IsNodeAtEnd(&mPrivate->PeriodicTimerListHead, (LIST_ENTRY*)PeriodicTimerToken));
  //
  // Handle checking 
  //
  if (*Handle == NULL || !TokenFound) {
    //
    // New a Handle
    //
    Status = AllocateBuffer(
               sizeof(PERIODIC_TIMER_TOKEN),
               ALIGNMENT_32,
               (VOID **)&PeriodicTimerToken
               );
    if (Status != EFI_SUCCESS) {
      Status = EFI_OUT_OF_RESOURCES;
      goto ON_ERROR;
    }
    //
    // Setup Smm address convert table for Smm security policy
    //
    InsertAddressConvertTable (
      ACT_FUNCTION_POINTER,
      &PeriodicTimerToken->Callback,
      1
      );
    InsertAddressConvertTable (
      ACT_INSTANCE_BODY,
      PeriodicTimerToken,
      sizeof (PERIODIC_TIMER_TOKEN)
      );
    InsertAddressConvertTable (
      ACT_INSTANCE_POINTER,
      &PeriodicTimerToken->Context,
      1
      );
    //
    // Insert it to PeriodicTimerList
    //
    PeriodicTimerToken->Callback   = Callback;
    PeriodicTimerToken->Context    = Context;
    PeriodicTimerToken->Type       = Type;
    PeriodicTimerToken->Timeout    = MilliSecond;
    PeriodicTimerToken->Countdown  = MilliSecond;
    PeriodicTimerToken->Unit       = PollingInterval;
    InsertTailList (&mPrivate->PeriodicTimerListHead, (LIST_ENTRY*)PeriodicTimerToken);
  } else {
    //
    // Use of existence Handle
    //
    PeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)(*Handle);
    //
    // Update Unit value in case the token migrated from EFI timer(which been changed the Unit value)
    //
    PeriodicTimerToken->Unit       = PollingInterval;
  }
  //
  // Insert USB interrupt SMI if list empty
  //
  if (mPrivate->PeriodicTimerProvider) {
    //
    // SMI based periodic timer
    //
    Provider = mPrivate->PeriodicTimerProvider;
    Provider->GetState (
                Provider,
                &State
                );
    //
    // Checking is the ExistInterval really exist on HC
    //
    if (State == EfiUsbHcStateOperational && ExistInterval != 0) {
      if ((ExistInterval & 0x01) && (Provider->QueryIo (Provider, 0, 0, MINIMUM_POLLING_INTERVAL) == EFI_NOT_FOUND)) {
        RequiredInterval |= 0x01;
        ExistInterval    &= ~0x01;
      }
      if ((ExistInterval & 0x02) && (Provider->QueryIo (Provider, 0, 0, MAXIMUM_POLLING_INTERVAL) == EFI_NOT_FOUND)) {
        RequiredInterval |= 0x02;
        ExistInterval    &= ~0x02;
      }
    }
    if (State == EfiUsbHcStateHalt) {
      RequiredInterval |= ExistInterval;
      ExistInterval = 0;
    }
    //
    // Issues the InterruptTransfer
    //
    if ((RequiredInterval & ExistInterval) != RequiredInterval) {
      Zero = 0;
NEXT_PROVIDER:
      Status = EFI_SUCCESS; 
      if (RequiredInterval & 0x01) {
        Status = Provider->InterruptTransfer(
                             Provider,
                             0,
                             0,
                             EFI_USB_SPEED_LOW,
                             8,
                             NULL,
                             &Zero,
                             (UINT8*)&Zero,
                             0,
                             NULL,
                             PeriodicTimerCallback,
                             (VOID*)(UINTN)MINIMUM_POLLING_INTERVAL,
                             MINIMUM_POLLING_INTERVAL,
                             (UINT32*)&Zero
                             );
      }
      if (RequiredInterval & 0x02) {
        Status = Provider->InterruptTransfer(
                             Provider,
                             0,
                             0,
                             EFI_USB_SPEED_LOW,
                             8,
                             NULL,
                             &Zero,
                             (UINT8*)&Zero,
                             0,
                             NULL,
                             PeriodicTimerCallback,
                             (VOID*)(UINTN)MAXIMUM_POLLING_INTERVAL,
                             MAXIMUM_POLLING_INTERVAL,
                             (UINT32*)&Zero
                             );
      }
      if (Status == EFI_UNSUPPORTED || Status == EFI_DEVICE_ERROR) {
        //
        // The provider has killed, transfer to next one
        //
        GetNextProvider(
          &mPrivate->PeriodicTimerProviderListHead,
          &Provider
          );
        if (Provider == mPrivate->PeriodicTimerProvider) {
          Status = EFI_OUT_OF_RESOURCES;
          goto ON_ERROR;
        } else {
          goto NEXT_PROVIDER;
        }      
      } else if (Status == EFI_SUCCESS && mPrivate->PeriodicTimerProvider != Provider) {
        mPrivate->PeriodicTimerProvider = Provider;
      }
    }
  } else {
    //
    // IRQ based periodic timer
    //
    GetMode (&Mode);
    if (Mode != USB_CORE_RUNTIME_MODE) {
      //
      // Update the Unit value to meet the IRQ timer frequency
      //
      PeriodicTimerToken->Unit = mPrivate->PeriodicTimerPeriod;
      if (!mPrivate->PeriodicTimerHandle) {
        RegisterLegacyFreeHcCallback (
          PeriodicTimerNotifyFunction,
          0,
          (VOID*)(UINTN)mPrivate->PeriodicTimerPeriod,
          &mPrivate->PeriodicTimerHandle
          );
      }
    }
  }
  *Handle = PeriodicTimerToken;
ON_ERROR:
  LeaveCriticalSection();
  return Status;
}

/**

  Remove a periodic timer callback

  @param  Handle                Handle created by InsertPeriodicTimer

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
RemovePeriodicTimer (
  IN     VOID                           *Handle
  )
{
  EFI_STATUS                 Status;
  PERIODIC_TIMER_TOKEN       *PeriodicTimerToken;
  UINTN                      PollingInterval = 0;
  UINTN                      Zero;
  UINTN                      Remain;

  //
  // Enter critical section to privent SMI interfere with priodic timer
  //
  EnterCriticalSection();
  PeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)&mPrivate->PeriodicTimerListHead;
  do {
    if (IsListEmpty(&mPrivate->PeriodicTimerListHead)) break;
    PeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)GetFirstNode((LIST_ENTRY*)PeriodicTimerToken);
    if (Handle == (VOID*)PeriodicTimerToken) {
      PollingInterval = PeriodicTimerToken->Unit;
      //
      // Waiting for thread safe
      //
      RemoveEntryList((LIST_ENTRY*)PeriodicTimerToken);
      FreeBuffer(
        sizeof(PERIODIC_TIMER_TOKEN),
        PeriodicTimerToken
        );
      //
      // Remove Smm address convert table
      //
      RemoveAddressConvertTable (
        ACT_FUNCTION_POINTER,
        &PeriodicTimerToken->Callback
        );
      RemoveAddressConvertTable (
        ACT_INSTANCE_BODY,
        PeriodicTimerToken
        );
      RemoveAddressConvertTable (
        ACT_INSTANCE_POINTER,
        &PeriodicTimerToken->Context
        );
      break;
    }
  } while (!IsNodeAtEnd(&mPrivate->PeriodicTimerListHead, (LIST_ENTRY*)PeriodicTimerToken));
  //
  // Exit if the handle not found
  //
  if (!PollingInterval) {
    Status = EFI_NOT_FOUND;
    goto ON_ERROR;
  }
  //
  // Remove USB interrupt SMI if list empty
  //
  Remain = 0;
  if (!IsListEmpty(&mPrivate->PeriodicTimerListHead)) {
    PeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)&mPrivate->PeriodicTimerListHead;
    do {
      PeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)GetFirstNode((LIST_ENTRY*)PeriodicTimerToken);
      if (PeriodicTimerToken->Unit == PollingInterval) {
        Remain ++;
      }
    } while (!IsNodeAtEnd(&mPrivate->PeriodicTimerListHead, (LIST_ENTRY*)PeriodicTimerToken));
  }
  if (!Remain) {
    if (mPrivate->PeriodicTimerProvider) {
      Zero = 0;
      mPrivate->PeriodicTimerProvider->CancelIo(
                                         mPrivate->PeriodicTimerProvider,
                                         0,
                                         0,
                                         PollingInterval,
                                         (UINT8*)&Zero
                                         );
    } else if (mPrivate->PeriodicTimerHandle) {
      UnregisterLegacyFreeHcCallback (
        mPrivate->PeriodicTimerHandle
        );
      mPrivate->PeriodicTimerHandle = NULL;
    }
  }
  Status = EFI_SUCCESS;
ON_ERROR:
  LeaveCriticalSection();
  return Status;
}

/**

  Remove a Usb3HC protocol from ProviderListHead

  @param  Usb3Hc                Usb3Hc protocol
  @param  ListEntry             Pointer of LIST_ENTRY for linking list
  @param  Provider              Pointer Usb3Hc protocol storage

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
RemoveProvider (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc,
  IN     LIST_ENTRY                     *ListEntry,
  IN     EFI_USB3_HC_PROTOCOL           **Provider
  )
{
  PROVIDER_TOKEN  *ProviderToken;

  ProviderToken = (PROVIDER_TOKEN*)ListEntry;
  do {
    if (IsListEmpty(ListEntry)) break;
    ProviderToken = (PROVIDER_TOKEN*)GetFirstNode((LIST_ENTRY*)ProviderToken);
    if (ProviderToken->Usb3Hc == Usb3Hc) {
      if (*Provider == Usb3Hc) {
        *Provider = NULL;
      }
      RemoveEntryList((LIST_ENTRY*)ProviderToken);
      FreeBuffer(
        sizeof(PROVIDER_TOKEN),
        ProviderToken
        );
      //
      // Remove Smm address convert table
      //
      RemoveAddressConvertTable (
        ACT_INSTANCE_BODY,
        ProviderToken
        );
      RemoveAddressConvertTable (
        ACT_INSTANCE_POINTER,
        &ProviderToken->Usb3Hc
        );
      break;
    }
  } while (!IsNodeAtEnd(ListEntry, (LIST_ENTRY*)ProviderToken));
  //
  // Transfer the Provider to first of ProviderList if original Provider is removed
  //
  if (*Provider == NULL && !IsListEmpty(ListEntry)) {
    ProviderToken = (PROVIDER_TOKEN*)GetFirstNode((LIST_ENTRY*)ListEntry);
    *Provider = ProviderToken->Usb3Hc;
  }
  return EFI_SUCCESS;
}

/**

  Insert a Usb3HC protocol into PeriodicTimerProviderListHead

  @param  Usb3Hc                Usb3Hc protocol

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
InsertPeriodicTimerProvider (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc
  )
{
  EFI_STATUS                 Status;
  PROVIDER_TOKEN             *ProviderToken;
  UINT8                      MaxSpeedOld;
  UINT8                      MaxSpeedNew;
  UINT8                      PortNumber;
  UINT8                      Is64BitCapable;
  VOID                       *Handle;
  PERIODIC_TIMER_TOKEN       *PeriodicTimerToken;
  UINTN                      Zero;
  EFI_USB3_HC_PROTOCOL       *Provider;
  
  Status = AllocateBuffer(
             sizeof(PROVIDER_TOKEN),
             ALIGNMENT_32,
             (VOID **)&ProviderToken
             );
  if (Status != EFI_SUCCESS) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Insert it to PeriodicTimerProviderList
  //
  ProviderToken->Usb3Hc     = Usb3Hc;
  InsertTailList (&mPrivate->PeriodicTimerProviderListHead, (LIST_ENTRY*)ProviderToken);
  //
  // Setup Smm address convert table for Smm security policy
  //
  InsertAddressConvertTable (
    ACT_INSTANCE_BODY,
    ProviderToken,
    sizeof (PROVIDER_TOKEN)
    );
  InsertAddressConvertTable (
    ACT_INSTANCE_POINTER,
    &ProviderToken->Usb3Hc,
    1
    );
  if (mPrivate->PeriodicTimerProvider == NULL) {
    mPrivate->PeriodicTimerProvider = Usb3Hc;
    if (mPrivate->PeriodicTimerPeriod && mPrivate->LegacyFreeHcTimerEvent) {
      //
      // The EFI timer been installed, close timer event before any InsertPeriodicTimer to
      // preventing both of periodic SMI timer and EFI timer triggers same timer handler  
      //
      gBS->CloseEvent (mPrivate->LegacyFreeHcTimerEvent);
      mPrivate->LegacyFreeHcTimerEvent = NULL;
    }
    if (!IsListEmpty(&mPrivate->PeriodicTimerListHead)) {
      //
      // The periodic timer registration prior then periodic timer provider registration
      //
      PeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)&mPrivate->PeriodicTimerListHead;
      do {
        PeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)GetFirstNode((LIST_ENTRY*)PeriodicTimerToken);
        Handle = (VOID*)PeriodicTimerToken;
        InsertPeriodicTimer (
          USB_CORE_PERIODIC_TIMER,
          PeriodicTimerToken->Callback,
          PeriodicTimerToken->Context,
          PeriodicTimerToken->Timeout,
          &Handle
          );
      } while (!IsNodeAtEnd(&mPrivate->PeriodicTimerListHead, (LIST_ENTRY*)PeriodicTimerToken));
    }
    if (mPrivate->PeriodicTimerPeriod) {
      //
      // The EFI timer been installed, uninstall it and use SMI periodic timer instead 
      //
      mPrivate->PeriodicTimerPeriod = 0;
      mPrivate->TimerArch->SetTimerPeriod = mPrivate->SetTimerPeriod;
      if (mPrivate->LegacyFreeHcTimerEvent) {
        gBS->CloseEvent (mPrivate->LegacyFreeHcTimerEvent);
        mPrivate->LegacyFreeHcTimerEvent = 0;
      }
      //
      // Use of SMI interrupt for HC interrupt polling
      //
      Status = InsertPeriodicTimer(
                 USB_CORE_PERIODIC_TIMER,
                 DispatchLegacyFreeHcNotifyFunction,
                 (VOID*)(UINTN)LEGACY_FREE_HC_TIMER_EVENT,
                 MINIMUM_POLLING_INTERVAL,
                 &mPrivate->LegacyFreeHcTimerEvent
                 );
      if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
      }
      InsertAddressConvertTable (
        ACT_INSTANCE_POINTER,
        &mPrivate->LegacyFreeHcTimerEvent,
        1
        );
    }
    return EFI_SUCCESS;
  }
  //
  // In order to save HC resource, change to lower speed HC if PeriodicTimerProvider was higer speed HC
  //
  mPrivate->PeriodicTimerProvider->GetCapability (
                                     mPrivate->PeriodicTimerProvider,
                                     &MaxSpeedOld,
                                     &PortNumber,
                                     &Is64BitCapable
                                     );
  Usb3Hc->GetCapability (
            Usb3Hc,
            &MaxSpeedNew,
            &PortNumber,
            &Is64BitCapable
            );
  if (MaxSpeedOld > MaxSpeedNew) {
    //
    // Cancel the interrupt on the original provider
    //
    Provider = mPrivate->PeriodicTimerProvider;
    if (Provider->QueryIo (Provider, 0, 0, MINIMUM_POLLING_INTERVAL) == EFI_SUCCESS) {
      Zero = 0;
      Provider->CancelIo(
                  Provider,
                  0,
                  0,
                  MINIMUM_POLLING_INTERVAL,
                  (UINT8*)&Zero
                  );
      Zero = 0;
      Usb3Hc->InterruptTransfer(
                Usb3Hc,
                0,
                0,
                EFI_USB_SPEED_LOW,
                8,
                NULL,
                &Zero,
                (UINT8*)&Zero,
                0,
                NULL,
                PeriodicTimerCallback,
                (VOID*)(UINTN)MINIMUM_POLLING_INTERVAL,
                MINIMUM_POLLING_INTERVAL,
                (UINT32*)&Zero
                );
    }
    if (Provider->QueryIo (Provider, 0, 0, MAXIMUM_POLLING_INTERVAL) == EFI_SUCCESS) {
      Zero = 0;
      Provider->CancelIo(
                  Provider,
                  0,
                  0,
                  MAXIMUM_POLLING_INTERVAL,
                  (UINT8*)&Zero
                  );
      Zero = 0;
      Usb3Hc->InterruptTransfer(
                Usb3Hc,
                0,
                0,
                EFI_USB_SPEED_LOW,
                8,
                NULL,
                &Zero,
                (UINT8*)&Zero,
                0,
                NULL,
                PeriodicTimerCallback,
                (VOID*)(UINTN)MAXIMUM_POLLING_INTERVAL,
                MAXIMUM_POLLING_INTERVAL,
                (UINT32*)&Zero
                );
    }
    mPrivate->PeriodicTimerProvider = Usb3Hc;
    return EFI_SUCCESS;
  }
  return EFI_ALREADY_STARTED;
}

/**

  Remove a Usb3HC protocol from PeriodicTimerProviderListHead

  @param  Usb3Hc                Usb3Hc protocol

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
RemovePeriodicTimerProvider (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc
  )
{
  return RemoveProvider(
           Usb3Hc,
           &mPrivate->PeriodicTimerProviderListHead,
           &mPrivate->PeriodicTimerProvider
           );
}

/**

  Get the Usb3HC protocol which provided LegacySupport feature

  @param  Usb3Hc                Pointer of the Usb3Hc protocol

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
GetLegacySupportProvider (
  IN     EFI_USB3_HC_PROTOCOL           **Usb3Hc
  )
{
  PROVIDER_TOKEN  *ProviderToken;

  ProviderToken = (PROVIDER_TOKEN*)&mPrivate->LegacySupportProviderListHead;
  do {
    if (IsListEmpty(&mPrivate->LegacySupportProviderListHead)) break;
    ProviderToken = (PROVIDER_TOKEN*)GetFirstNode((LIST_ENTRY*)ProviderToken);
    if (*Usb3Hc == NULL || ProviderToken->Usb3Hc == *Usb3Hc) {
      if (*Usb3Hc != NULL) {
        if (IsNodeAtEnd(&mPrivate->LegacySupportProviderListHead, (LIST_ENTRY*)ProviderToken)) {
          return EFI_NOT_FOUND;
        } else {
          ProviderToken = (PROVIDER_TOKEN*)GetFirstNode((LIST_ENTRY*)ProviderToken);
        }
      }
      *Usb3Hc = ProviderToken->Usb3Hc;
      return EFI_SUCCESS;
    }
  } while (!IsNodeAtEnd(&mPrivate->LegacySupportProviderListHead, (LIST_ENTRY*)ProviderToken));
  return EFI_NOT_FOUND;
}

/**

  Insert a set of KBC KeyCode

  @param  KbcCmd                KBC command
  @param  KeyCode               Porinter of KeyCode 
  @param  Length                Length of KeyCode

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
InsertKbcKeyCode (
  IN     UINT8                          QueueSlot,
  IN     UINT8                          *KeyCode,
  IN     UINTN                          Length
  )
{
  if (mPrivate->KbcEmulator) {
    mPrivate->KbcEmulator->InsertKbcKeyCode(
                             QueueSlot,
                             KeyCode,
                             Length
                             );
  }
  return EFI_SUCCESS;
}

/**

  The KBC trap processor

  @param  TrapType              TrapType
  @param  Data                  Pointer of Data for reading
  @param  IrqChannel            Pointer for IrqChannel for next trigger

  @retval EFI_SUCCESS  

**/
EFI_STATUS
EFIAPI
KbcTrapProcessor (
  IN     UINTN                          TrapType,
  IN     UINT8                          Data
  )
{
  if (mPrivate->KbcEmulator) {
    mPrivate->KbcEmulator->KbcTrapProcessor(
                             TrapType,
                             Data
                             );
  }
  return EFI_SUCCESS;
}

/**

  Insert a Usb3HC protocol into LegacySupportProviderListHead

  @param  Usb3Hc                Usb3Hc protocol

  @retvalEFI_SUCCESS

**/
EFI_STATUS
EFIAPI
InsertLegacySupportProvider (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc
  )
{
  EFI_STATUS      Status;
  PROVIDER_TOKEN  *ProviderToken;
  
  Status = AllocateBuffer(
             sizeof(PROVIDER_TOKEN),
             ALIGNMENT_32,
             (VOID **)&ProviderToken
             );
  if (Status != EFI_SUCCESS) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Insert it to LegacySupportProviderList
  //
  ProviderToken->Usb3Hc     = Usb3Hc;
  InsertTailList (&mPrivate->LegacySupportProviderListHead, (LIST_ENTRY*)ProviderToken);
  //
  // Setup Smm address convert table for Smm security policy
  //
  InsertAddressConvertTable (
    ACT_INSTANCE_BODY,
    ProviderToken,
    sizeof (PROVIDER_TOKEN)
    );
  InsertAddressConvertTable (
    ACT_INSTANCE_POINTER,
    &ProviderToken->Usb3Hc,
    1
    );
  if (mPrivate->LegacySupportProvider == NULL) {
    mPrivate->LegacySupportProvider = Usb3Hc;
    return EFI_SUCCESS;
  }
  return EFI_ALREADY_STARTED;
}

/**

  Remove a Usb3HC protocol from LegacySupportProviderListHead

  @param  Usb3Hc                Usb3Hc protocol

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
RemoveLegacySupportProvider (
  IN     EFI_USB3_HC_PROTOCOL           *Usb3Hc
  )
{
  return RemoveProvider(
           Usb3Hc,
           &mPrivate->LegacySupportProviderListHead,
           &mPrivate->LegacySupportProvider
           );
}

/**

  Enter critical section in order to privent SMI interrupted during critical priod

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
EnterCriticalSection (
  )
{
  //
  // Increase the count of CriticalSection
  //
  mPrivate->CriticalSection ++;
  return EFI_SUCCESS;
}

/**

  Leave critical section and dispatches deferred callback

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
LeaveCriticalSection (
  )
{
  if (mPrivate->CriticalSection) {
    //
    // Decrease the count of CritiocalSection
    //
    mPrivate->CriticalSection --;
    //
    // Dispatches deferred callback
    //
    if (!mPrivate->CriticalSection && !IsListEmpty(&mPrivate->DeferredHcCallbackListHead)) {
      //
      // Use of S/W SMI to proceed the deferred callback
      //
      if (mPrivate->SmmReady) {
        mPrivate->ProceedDeferredProcedure = TRUE;
        TriggerUsbSwSmi();
      } else {
        ProcessDeferredProcedure();
      }
    }
  }
  return EFI_SUCCESS;
}

/**

  Process Deferred Procedure to dispatch deferred callbacks

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ProcessDeferredProcedure (
  )
{
  CALLBACK_TOKEN                *DeferredHcCallback;
  CALLBACK_TOKEN                DeferredCallback;
  
  while (!mPrivate->CriticalSection && !IsListEmpty(&mPrivate->DeferredHcCallbackListHead)) {
    DeferredHcCallback = (CALLBACK_TOKEN*)GetFirstNode(&mPrivate->DeferredHcCallbackListHead);
    //
    // Remove entity before dispatch since it may insert a entity in callback
    //
    RemoveEntryList((LIST_ENTRY*)DeferredHcCallback);
    CopyMem (&DeferredCallback, DeferredHcCallback, sizeof(CALLBACK_TOKEN));
    ZeroMem (DeferredHcCallback, sizeof(CALLBACK_TOKEN));
    //
    // Dispatch the deferred callback
    //
    DeferredCallback.Callback(DeferredCallback.Event, DeferredCallback.Context);
  }
  mPrivate->ProceedDeferredProcedure = FALSE;
  return EFI_SUCCESS;
}

/**

  Dispatches HC callback routine or make it into deferred callback list

  @param  Callback              Callback routine
  @param  Event                 Event parameter for callback
  @param  Context               Context parameter for callback

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
DispatchHcCallback (
  IN     EFI_USB3_HC_CALLBACK           Callback,
  IN     UINTN                          Event,
  IN     VOID                           *Context
  )
{
  UINTN                   Index;
  CALLBACK_TOKEN          *DeferredHcCallback;
  
  if (!mPrivate->CriticalSection) {
    //
    // Dispatches it immediately
    //
    Callback(Event, Context);
  } else {
    //
    // Check exist first
    //
    DeferredHcCallback = (CALLBACK_TOKEN*)&mPrivate->DeferredHcCallbackListHead;
    do {
      if (IsListEmpty(&mPrivate->DeferredHcCallbackListHead)) break;
      DeferredHcCallback = (CALLBACK_TOKEN*)GetFirstNode((LIST_ENTRY*)DeferredHcCallback);
      if (DeferredHcCallback->Callback == Callback &&
          DeferredHcCallback->Event    == Event &&
          DeferredHcCallback->Context  == Context) {
        return EFI_SUCCESS;
      }
    } while (!IsNodeAtEnd(&mPrivate->DeferredHcCallbackListHead, (LIST_ENTRY*)DeferredHcCallback));
    //
    // Insert callback into deferred callback list
    //
    for (Index = 0; Index < MAX_DEFERRED_CALLBACK_TOKENS; Index ++) {
      if (!mPrivate->DeferredCallbackTokens[Index].Callback) {
        //
        // Insert it to DeferredHcCallbackListHead
        //
        DeferredHcCallback = &mPrivate->DeferredCallbackTokens[Index];
        DeferredHcCallback->Callback = Callback;
        DeferredHcCallback->Event    = Event;
        DeferredHcCallback->Context  = Context;
        InsertTailList (&mPrivate->DeferredHcCallbackListHead, (LIST_ENTRY*)DeferredHcCallback);
        break;
      }
    }
    if (Index == MAX_DEFERRED_CALLBACK_TOKENS) {
      //
      // Just let it ignore the SMI
      //
      ;
    }
  }
  return EFI_SUCCESS;
}

/**

  Register Usb Binding Protocol for Legacy Hot Plug Mechanism

  @param  Support               Support subroutine
  @param  Start                 Start subroutine
  @param  Stop                  Stop subroutine

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
RegisterUsbBindingProtocol (
  IN     EFI_USB_BINDING_SUPPORTED      Support,
  IN     EFI_USB_BINDING_START          Start,
  IN     EFI_USB_BINDING_STOP           Stop
  )
{
  EFI_STATUS        Status;
  USB_BINDING_TOKEN *UsbBindingToken;
  
  Status = AllocateBuffer(
             sizeof(USB_BINDING_TOKEN),
             ALIGNMENT_32,
             (VOID **)&UsbBindingToken
             );
  if (Status != EFI_SUCCESS) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Insert it to UsbBindingListHead
  //
  UsbBindingToken->Support = Support;
  UsbBindingToken->Start   = Start;
  UsbBindingToken->Stop    = Stop;
  InsertTailList (&mPrivate->UsbBindingListHead, (LIST_ENTRY*)UsbBindingToken);
  //
  // Setup Smm address convert table for Smm security policy
  //
  InsertAddressConvertTable (
    ACT_INSTANCE_BODY,
    UsbBindingToken,
    sizeof (USB_BINDING_TOKEN)
    );
  return EFI_SUCCESS;
}

/**

  Check the UsbDevice is in BootDevicesTable 

  @param  UsbDevice             Usb Device

  @retval TRUE                  In the table
  @retval FALSE                 Not in the table

**/
STATIC
BOOLEAN
CheckBootDevices (
  IN     USB_DEVICE                     *UsbDevice
  )
{
  UINTN Index;

  if (mPrivate->BootDevicesTable) {
    for (Index = 0; mPrivate->BootDevicesTable[Index]; Index ++) {
      if (mPrivate->BootDevicesTable[Index] == UsbDevice) {
        return TRUE;
      }
    }
  }
  return FALSE;
}

/**

  Get Usb Devices list

  @param  Type                  Type of USB device
  @param  Count                 Entities of Usb Devices
  @param  Devices               Pointer of Usb Devices table

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI 
GetUsbDevices (
  IN     UINTN                          Type,
  IN OUT UINTN                          *Count,
  IN OUT USB_DEVICE                     ***Devices
  )
{
  EFI_STATUS                     Status;
  USB_DEVICE_TOKEN               *UsbDeviceToken;
  UINTN                          Total;
  USB_DEVICE                     **Buffer;
  
  *Count   = Total = 0;
  *Devices = NULL;
  UsbDeviceToken = (USB_DEVICE_TOKEN*)&mPrivate->UsbDeviceListHead;
  do {
    if (IsListEmpty(&mPrivate->UsbDeviceListHead)) break;
    UsbDeviceToken = (USB_DEVICE_TOKEN*)GetFirstNode((LIST_ENTRY*)UsbDeviceToken);
    if (UsbDeviceToken->UsbDevice.Header.Type == Type) Total ++;
  } while (!IsNodeAtEnd(&mPrivate->UsbDeviceListHead, (LIST_ENTRY*)UsbDeviceToken));
  if (Total > 0) {
    Status = AllocateBuffer(
               sizeof(USB_DEVICE*) * (Total + 1),
               ALIGNMENT_32,
               (VOID*)&Buffer
               );
    if (Status != EFI_SUCCESS) {
      return EFI_OUT_OF_RESOURCES;
    }
    *Count   = Total;
    *Devices = Buffer;
    UsbDeviceToken = (USB_DEVICE_TOKEN*)&mPrivate->UsbDeviceListHead;
    do {
      UsbDeviceToken = (USB_DEVICE_TOKEN*)GetFirstNode((LIST_ENTRY*)UsbDeviceToken);
      if (UsbDeviceToken->UsbDevice.Header.Type == Type) {
        *Buffer = &UsbDeviceToken->UsbDevice;
        Buffer ++;
      }
    } while (!IsNodeAtEnd(&mPrivate->UsbDeviceListHead, (LIST_ENTRY*)UsbDeviceToken));
    //
    // Make a record for boot devices hot-plug mechanism
    //
    if (Type == USB_CORE_USB_MASS_STORAGE) {
      mPrivate->BootDevicesTable = *Devices;
      //
      // Setup Smm address convert table for Smm security policy
      //
      InsertAddressConvertTable (
        ACT_INSTANCE_BODY,
        mPrivate->BootDevicesTable,
        sizeof(USB_DEVICE*) * (Total + 1)
        );
      InsertAddressConvertTable (
        ACT_INSTANCE_POINTER,
        &mPrivate->BootDevicesTable,
        1
        );
      InsertAddressConvertTable (
        ACT_INSTANCE_POINTER,
        mPrivate->BootDevicesTable,
        Total
        );
    }
    return EFI_SUCCESS;
  }
  return EFI_NOT_FOUND;
}

/**

  Insert a Usb device into UsbDeviceListHead

  @param  UsbDevice             UsbDevice

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
InsertUsbDevice (
  IN     USB_DEVICE                     *UsbDevice
  )
{
  EFI_STATUS            Status;
  USB_DEVICE_TOKEN      *UsbDeviceToken;
  UINTN                 Index;
  USB_DEVICE            *UsbBootDevice;
  BOOLEAN               DeviceFound;
  //
  // Check is it MassStorage device and been detached 
  //
  if (UsbDevice->Header.Type == USB_CORE_USB_MASS_STORAGE && mPrivate->BootDevicesTable) {
    Index = 0;
    DeviceFound = FALSE;
    do {
      UsbBootDevice = mPrivate->BootDevicesTable[Index];
      if (UsbBootDevice &&
          (UsbBootDevice->UsbMassStorage.UsbIo == NULL || 
           UsbBootDevice->UsbMassStorage.UsbIo == UsbDevice->UsbMassStorage.UsbIo) &&
          UsbBootDevice->UsbMassStorage.VendorID == UsbDevice->UsbMassStorage.VendorID &&
          UsbBootDevice->UsbMassStorage.ProductID == UsbDevice->UsbMassStorage.ProductID &&
          UsbBootDevice->UsbMassStorage.Lun == UsbDevice->UsbMassStorage.Lun &&
          UsbBootDevice->UsbMassStorage.InterfaceNumber == UsbDevice->UsbMassStorage.InterfaceNumber) {
        if (UsbBootDevice->UsbMassStorage.UsbIo == NULL) {
          //
          // Yes! This device been detached and attach again now
          //
          UsbBootDevice->UsbMassStorage.UsbIo = UsbDevice->UsbMassStorage.UsbIo;
          UsbBootDevice->UsbMassStorage.Media = UsbDevice->UsbMassStorage.Media;
          //
          // Shutdown transport protocol first to release the transport instance
          //
          UsbBootDevice->UsbMassStorage.Transport->Fini(UsbBootDevice->UsbMassStorage.Transport);
          //
          // Reinit transport protocol
          //
          UsbBootDevice->UsbMassStorage.Transport->Init(UsbBootDevice->UsbMassStorage.UsbIo, NULL, (VOID **)&UsbBootDevice->UsbMassStorage.Transport);
          //
          // Transfer the new instance back to caller through UsbDevice
          //
          UsbDevice->UsbMassStorage.Transport = UsbBootDevice->UsbMassStorage.Transport;
        }
        DeviceFound = TRUE;
      }
      Index ++;
    } while (UsbBootDevice);
    if (DeviceFound) {
      return EFI_ALREADY_STARTED;
    }
  }
  if (UsbDevice->Header.Type == USB_CORE_USB_MASS_STORAGE && mPrivate->CurrMode == USB_CORE_RUNTIME_MODE) {
    //
    // The new MassStorage device hot-plug doesn't support under legacy mode
    //
    return EFI_UNSUPPORTED;
  }
  Status = AllocateBuffer(
             sizeof(USB_DEVICE_TOKEN),
             ALIGNMENT_32,
             (VOID **)&UsbDeviceToken
             );
  if (Status != EFI_SUCCESS) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Insert it to UsbDeviceListHead
  //
  CopyMem(&UsbDeviceToken->UsbDevice, UsbDevice, sizeof(USB_DEVICE));
  if (UsbDeviceToken->UsbDevice.Header.Type == USB_CORE_USB_HID && 
      UsbDeviceToken->UsbDevice.UsbHID.SyncLED != NULL) {
    //
    // Setup Smm address convert table for Smm security policy
    //
    InsertAddressConvertTable (
      ACT_FUNCTION_POINTER,
      &UsbDeviceToken->UsbDevice.UsbHID.SyncLED,
      1
      );
  } else if (UsbDeviceToken->UsbDevice.Header.Type == USB_CORE_USB_MASS_STORAGE) {
    //
    // Setup Smm address convert table for Smm security policy
    //
    InsertAddressConvertTable (
      ACT_INSTANCE_POINTER,
      &UsbDeviceToken->UsbDevice.UsbMassStorage.Transport,
      1
      );
  }
  InsertTailList (&mPrivate->UsbDeviceListHead, (LIST_ENTRY*)UsbDeviceToken);
  //
  // Setup Smm address convert table for Smm security policy
  //
  InsertAddressConvertTable (
    ACT_INSTANCE_BODY,
    UsbDeviceToken,
    sizeof (USB_DEVICE_TOKEN)
    );
  InsertAddressConvertTable (
    ACT_INSTANCE_POINTER,
    &UsbDeviceToken->UsbDevice.Header.UsbIo,
    1
    );
  InsertAddressConvertTable (
    ACT_INSTANCE_POINTER,
    &UsbDeviceToken->UsbDevice.Header.Instance,
    1
    );
  //
  // Set the Instance if this is USB_CORE_USB_MASS_STORAGE
  //
  if (UsbDeviceToken->UsbDevice.Header.Type == USB_CORE_USB_MASS_STORAGE) {
    UsbDeviceToken->UsbDevice.Header.Instance = &UsbDeviceToken->UsbDevice;
  }
  //
  // Setup the USB Mouse flag for CSM mouse module
  //
  UsbDeviceToken = (USB_DEVICE_TOKEN*)&mPrivate->UsbDeviceListHead;
  do {
    if (IsListEmpty(&mPrivate->UsbDeviceListHead)) break;
    UsbDeviceToken = (USB_DEVICE_TOKEN*)GetFirstNode((LIST_ENTRY*)UsbDeviceToken);
    if (UsbDeviceToken->UsbDevice.Header.Type == USB_CORE_USB_HID &&
        UsbDeviceToken->UsbDevice.UsbHID.SyncLED == NULL) {
      //
      // USB Mouse found, set the flag in EBDA
      //
      if (mPrivate->CsmEnabled) EBDA(EBDA_USB_MOUSE_FLAG) |= 0x02;
      break;
    }
  } while (!IsNodeAtEnd(&mPrivate->UsbDeviceListHead, (LIST_ENTRY*)UsbDeviceToken));
  return EFI_SUCCESS;
}

/**

  Remove a UsbMassStorage from UsbDeviceListHead

  @param  Handle                The handle of UsbMassStorage

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI 
RemoveUsbDevice (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo
  )
{
  EFI_STATUS       Status = EFI_NOT_FOUND;
  USB_DEVICE_TOKEN *UsbDeviceToken;
  LIST_ENTRY       *BackLink;
  BOOLEAN          UsbMouseFound;

  UsbDeviceToken = (USB_DEVICE_TOKEN*)&mPrivate->UsbDeviceListHead;
  do {
    if (IsListEmpty(&mPrivate->UsbDeviceListHead)) break;
    UsbDeviceToken = (USB_DEVICE_TOKEN*)GetFirstNode((LIST_ENTRY*)UsbDeviceToken);
    if (UsbDeviceToken->UsbDevice.Header.UsbIo == UsbIo) {
      //
      // For HID or MassStorage not in the boot devices, free the whole resources.
      // For Mass Storages which in the boot devices, keep the resource to make the
      // legacy mode hot-plug work
      //
      if (UsbDeviceToken->UsbDevice.Header.Type == USB_CORE_USB_HID || !CheckBootDevices(&UsbDeviceToken->UsbDevice)) {
        //
        // Remove Smm address convert table
        //
        RemoveAddressConvertTable (
          ACT_INSTANCE_BODY,
          UsbDeviceToken
          );
        RemoveAddressConvertTable (
          ACT_INSTANCE_POINTER,
          &UsbDeviceToken->UsbDevice.Header.UsbIo
          );
        RemoveAddressConvertTable (
          ACT_INSTANCE_POINTER,
          &UsbDeviceToken->UsbDevice.Header.Instance
          );
        //
        // Backup the backlink for safety removal
        //
        BackLink = ((LIST_ENTRY*)UsbDeviceToken)->BackLink;
        //
        // Remove USB device
        //
        RemoveEntryList((LIST_ENTRY*)UsbDeviceToken);
        FreeBuffer(
          sizeof(USB_DEVICE_TOKEN),
          UsbDeviceToken
          );
        if (UsbDeviceToken->UsbDevice.Header.Type == USB_CORE_USB_HID && 
            UsbDeviceToken->UsbDevice.UsbHID.SyncLED != NULL) {
          //
          // Remove Smm address convert table
          //
          RemoveAddressConvertTable (
            ACT_FUNCTION_POINTER,
            &UsbDeviceToken->UsbDevice.UsbHID.SyncLED
            );
        } else if (UsbDeviceToken->UsbDevice.Header.Type == USB_CORE_USB_MASS_STORAGE) {
          //
          // Remove Smm address convert table
          //
          RemoveAddressConvertTable (
            ACT_INSTANCE_POINTER,
            &UsbDeviceToken->UsbDevice.UsbMassStorage.Transport
            );
        }
        //
        // Rollback to previous link
        //
        UsbDeviceToken = (USB_DEVICE_TOKEN*)BackLink;
        Status = EFI_SUCCESS;
      } else {
        //
        // Remove Mass Storage device(Just set the UsbIo to NULL to signal that this is empty device).
        //
        UsbDeviceToken->UsbDevice.Header.UsbIo = NULL;
        Status = EFI_ALREADY_STARTED;
      }
    }
  } while (!IsNodeAtEnd(&mPrivate->UsbDeviceListHead, (LIST_ENTRY*)UsbDeviceToken) && Status == EFI_NOT_FOUND);
  //
  // Setup the USB Mouse flag for CSM mouse module
  //
  UsbMouseFound  = FALSE;
  UsbDeviceToken = (USB_DEVICE_TOKEN*)&mPrivate->UsbDeviceListHead;
  do {
    if (IsListEmpty(&mPrivate->UsbDeviceListHead)) break;
    UsbDeviceToken = (USB_DEVICE_TOKEN*)GetFirstNode((LIST_ENTRY*)UsbDeviceToken);
    if (UsbDeviceToken->UsbDevice.Header.Type == USB_CORE_USB_HID &&
        UsbDeviceToken->UsbDevice.UsbHID.SyncLED == NULL) {
      UsbMouseFound = TRUE;
      break;
    }
  } while (!IsNodeAtEnd(&mPrivate->UsbDeviceListHead, (LIST_ENTRY*)UsbDeviceToken));
  if (!UsbMouseFound) {
    //
    // USB Mouse not found, clear the flag in EBDA
    //
    if (mPrivate->CsmEnabled) EBDA(EBDA_USB_MOUSE_FLAG) &= ~0x02;
  }
  return Status;
}

/**

  Connect a Usb Device for Hot Plug mechanism

  @param  UsbIo                 UsbIo

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ConnectUsbDevices (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo,
  IN     EFI_DEVICE_PATH_PROTOCOL       *DevicePath
  )
{
  USB_BINDING_TOKEN *UsbBindingToken;
  
  UsbBindingToken = (USB_BINDING_TOKEN*)&mPrivate->UsbBindingListHead;
  do {
    if (IsListEmpty(&mPrivate->UsbBindingListHead)) break;
    UsbBindingToken = (USB_BINDING_TOKEN*)GetFirstNode((LIST_ENTRY*)UsbBindingToken);
    if (UsbBindingToken->Support(UsbIo, &mPrivate->UsbCoreProtocol, DevicePath) == EFI_SUCCESS) {
      UsbBindingToken->Start(UsbIo, &mPrivate->UsbCoreProtocol);
      break;
    }
  } while (!IsNodeAtEnd(&mPrivate->UsbBindingListHead, (LIST_ENTRY*)UsbBindingToken));
  return EFI_SUCCESS;
}

/**

  Disconnect a Usb Device for Hot Plug mechanism

  @param  UsbIo                 UsbIo

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
DisconnectUsbDevices (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo
  )
{
  USB_BINDING_TOKEN       *UsbBindingToken;
  USB_DEVICE_TOKEN        *UsbDeviceToken;
  VOID                    *Instance;
  
  Instance = NULL;
  //
  // Find out the Usb device which UsbIo belong to
  //
  UsbDeviceToken = (USB_DEVICE_TOKEN*)&mPrivate->UsbDeviceListHead;
  do {
    if (IsListEmpty(&mPrivate->UsbDeviceListHead)) break;
    UsbDeviceToken = (USB_DEVICE_TOKEN*)GetFirstNode((LIST_ENTRY*)UsbDeviceToken);
    if (UsbDeviceToken->UsbDevice.Header.UsbIo == UsbIo) {
      Instance = UsbDeviceToken->UsbDevice.Header.Instance;
      break;
    }
  } while (!IsNodeAtEnd(&mPrivate->UsbDeviceListHead, (LIST_ENTRY*)UsbDeviceToken));
  if (Instance != NULL) {
    UsbBindingToken = (USB_BINDING_TOKEN*)&mPrivate->UsbBindingListHead;
    do {
      if (IsListEmpty(&mPrivate->UsbBindingListHead)) break;
      UsbBindingToken = (USB_BINDING_TOKEN*)GetFirstNode((LIST_ENTRY*)UsbBindingToken);
      if (UsbBindingToken->Stop(UsbIo, Instance) == EFI_SUCCESS) break;
    } while (!IsNodeAtEnd(&mPrivate->UsbBindingListHead, (LIST_ENTRY*)UsbBindingToken));
  }
  return EFI_SUCCESS;
}

/**

  Non-Smm polling callback 

  @param  Event                 Event.
  @param  Context               Context.

  @retval EFI_SUCCESS

**/
VOID
EFIAPI
NonSmmCallback (
  IN    EFI_EVENT    Event,
  IN    VOID         *Context
  )
{
  CALLBACK_TOKEN *NonSmmCallbackToken;

  while (!IsListEmpty(&mPrivate->NonSmmCallbackListHead)) {
    NonSmmCallbackToken = (CALLBACK_TOKEN*)GetFirstNode(&mPrivate->NonSmmCallbackListHead);
    //
    // Remove entity before dispatch since it may insert a entity in callback
    //
    RemoveEntryList((LIST_ENTRY*)NonSmmCallbackToken);
    FreeBuffer(
      sizeof(CALLBACK_TOKEN),
      NonSmmCallbackToken
      );
    NonSmmCallbackToken->Callback(NonSmmCallbackToken->Event, NonSmmCallbackToken->Context);
  }
}

/**

  Register a Non-Smm Notification callback 

  @param  Callback              UsbIo

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
RegisterNonSmmCallback (
  IN     EFI_USB3_HC_CALLBACK           Callback,
  IN     UINTN                          Event,
  IN     VOID                           *Context
  )
{
  EFI_STATUS              Status;
  CALLBACK_TOKEN          *NonSmmCallbackToken;
  BOOLEAN                 InSmm;
  EFI_TPL                 OldTpl;

  IsInSmm (&InSmm);
  if (!InSmm) {
    //
    // We need to check is it under timer interrupt due to we are use it if USB HC doesn't support SMI
    //
    OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
    gBS->RestoreTPL (OldTpl);
    if (OldTpl == TPL_NOTIFY + 1) InSmm = TRUE;
  }
  if (!InSmm) {
    //
    // Not in SMM. Launch it right now without register callback
    //
    Callback(Event, Context);
  } else {
    //
    // Now in SMM. Registers callback and let timer to launch it druing outside of SMM
    //
    Status = AllocateBuffer(
               sizeof(CALLBACK_TOKEN),
               ALIGNMENT_32,
               (VOID **)&NonSmmCallbackToken
               );
    if (Status != EFI_SUCCESS) {
      return EFI_OUT_OF_RESOURCES;
    }
    NonSmmCallbackToken->Callback = Callback;
    NonSmmCallbackToken->Event    = Event;
    NonSmmCallbackToken->Context  = Context;
    InsertTailList (&mPrivate->NonSmmCallbackListHead, (LIST_ENTRY*)NonSmmCallbackToken);
  }
  return EFI_SUCCESS;
}

/**

  The filter function for SetTimerPeriod used for detect the timer period been changed

  @param  This                  EFI_TIMER_ARCH_PROTOCOL
  @param  TimerPeriod           Timer period

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SetTimerPeriod (
  IN EFI_TIMER_ARCH_PROTOCOL            *This,
  IN UINT64                             TimerPeriod
  )
{
  UINTN                 PeriodicTimerPeriod;
  PERIODIC_TIMER_TOKEN  *PeriodicTimerToken;
  EFI_TPL               OldTpl;
  
  if (TimerPeriod != 0) {
    PeriodicTimerPeriod = (UINTN)(DivU64x32((TimerPeriod + 5000), 10000));
    if (PeriodicTimerPeriod < MINIMUM_POLLING_INTERVAL) PeriodicTimerPeriod = MINIMUM_POLLING_INTERVAL;
    if (PeriodicTimerPeriod != mPrivate->PeriodicTimerPeriod) {
      //
      // The TimerPeriod has been changed, we have to modify whole exist periodic timer to meet the new period
      //
      OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);
      PeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)GetFirstNode(&mPrivate->PeriodicTimerListHead);
      while (PeriodicTimerToken != (PERIODIC_TIMER_TOKEN*)&mPrivate->PeriodicTimerListHead) {
        //
        // Searching for matched token
        //
        if (PeriodicTimerToken->Unit == mPrivate->PeriodicTimerPeriod) {
          PeriodicTimerToken->Unit = PeriodicTimerPeriod;
        }
        PeriodicTimerToken = (PERIODIC_TIMER_TOKEN*)GetFirstNode(&PeriodicTimerToken->Link);
      }
      mPrivate->PeriodicTimerPeriod = PeriodicTimerPeriod;
      gBS->RestoreTPL (OldTpl);
    }
  }
  return mPrivate->SetTimerPeriod (This, TimerPeriod);
}

/**

  Register a laegcy free HC notification callback used for the HC doesn't support SMI

  @param  Callback              Callback function
  @param  Event                 Event type
  @param  Context               Context for callback function
  @param  Handle                Pointer to returned handle

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
RegisterLegacyFreeHcCallback (
  IN     EFI_USB3_HC_CALLBACK           Callback,
  IN     UINT32                         PciAddress,
  IN     VOID                           *Context,
  OUT    VOID                           **Handle
  )
{
  EFI_STATUS              Status;
  CALLBACK_TOKEN          *LegacyFreeHcCallbackToken;
  UINT64                  PeriodicTimerPeriod;

  Status = AllocateBuffer(
             sizeof(CALLBACK_TOKEN),
             ALIGNMENT_32,
             (VOID **)&LegacyFreeHcCallbackToken
             );
  if (Status != EFI_SUCCESS) {
    return EFI_OUT_OF_RESOURCES;
  }
  LegacyFreeHcCallbackToken->Callback = Callback;
  LegacyFreeHcCallbackToken->Context  = Context;
  InsertAddressConvertTable (
    ACT_INSTANCE_BODY,
    LegacyFreeHcCallbackToken,
    sizeof (CALLBACK_TOKEN)
    );
  InsertAddressConvertTable (
    ACT_FUNCTION_POINTER,
    &LegacyFreeHcCallbackToken->Callback,
    1
    );
  InsertAddressConvertTable (
    ACT_INSTANCE_POINTER,
    &LegacyFreeHcCallbackToken->Context,
    1
    );
  if (IsListEmpty(&mPrivate->LegacyFreeHcCallbackListHead)) {
    if (mPrivate->SmmReady && mPrivate->PeriodicTimerProvider) {
      //
      // Use of SMI interrupt for HC interrupt polling
      //
      Status = InsertPeriodicTimer(
                 USB_CORE_PERIODIC_TIMER,
                 DispatchLegacyFreeHcNotifyFunction,
                 (VOID*)(UINTN)LEGACY_FREE_HC_TIMER_EVENT,
                 MINIMUM_POLLING_INTERVAL,
                 &mPrivate->LegacyFreeHcTimerEvent
                 );
      if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
      }
      InsertAddressConvertTable (
        ACT_INSTANCE_POINTER,
        &mPrivate->LegacyFreeHcTimerEvent,
        1
        );
    } else {
      //
      // Use of timer for HC interrupt polling under POST only
      //
      Status = gBS->CreateEvent (
                      EVT_TIMER | EVT_NOTIFY_SIGNAL,
                      TPL_NOTIFY + 1,
                      (EFI_EVENT_NOTIFY)DispatchLegacyFreeHcNotifyFunction,
                      (VOID*)(UINTN)LEGACY_FREE_HC_TIMER_EVENT,
                      &mPrivate->LegacyFreeHcTimerEvent
                      );
      if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
      }
      Status = gBS->SetTimer (
                      mPrivate->LegacyFreeHcTimerEvent,
                      TimerPeriodic,
                      LEGACY_FREE_HC_POLLING_INTERVAL
                      );
      if (EFI_ERROR(Status)) {
        return EFI_OUT_OF_RESOURCES;
      }
      //
      // Get the actual timer period
      //
      mPrivate->TimerArch->GetTimerPeriod (
                             mPrivate->TimerArch,
                             &PeriodicTimerPeriod
                             );
      mPrivate->PeriodicTimerPeriod = (UINTN)(DivU64x32((PeriodicTimerPeriod + 5000), 10000));
      //
      // Hook the SetTimerPeriod used to detect TimerPeriod change
      //
      mPrivate->TimerArch->SetTimerPeriod = SetTimerPeriod;
    }
  }
  InsertTailList (&mPrivate->LegacyFreeHcCallbackListHead, (LIST_ENTRY*)LegacyFreeHcCallbackToken);
  if (Handle) *Handle = (VOID*)LegacyFreeHcCallbackToken;
  return EFI_SUCCESS;
}

/**

  Unregister a legacy free HC notification callback used for the HC doesn't support SMI

  @param  Handle                Pointer to returned handle

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
UnregisterLegacyFreeHcCallback (
  IN     VOID                           *Handle
  )
{
  CALLBACK_TOKEN        *LegacyFreeHcCallbackToken;
  UINTN                 Mode;

  LegacyFreeHcCallbackToken = (CALLBACK_TOKEN*)Handle;
  RemoveEntryList((LIST_ENTRY*)LegacyFreeHcCallbackToken);
  FreeBuffer(
    sizeof(CALLBACK_TOKEN),
    LegacyFreeHcCallbackToken
    );
  RemoveAddressConvertTable (
    ACT_INSTANCE_BODY,
    LegacyFreeHcCallbackToken
    );
  RemoveAddressConvertTable (
    ACT_FUNCTION_POINTER,
    &LegacyFreeHcCallbackToken->Callback
    );
  RemoveAddressConvertTable (
    ACT_INSTANCE_POINTER,
    &LegacyFreeHcCallbackToken->Context
    );
  if (IsListEmpty(&mPrivate->LegacyFreeHcCallbackListHead) && mPrivate->LegacyFreeHcTimerEvent) {
    if (mPrivate->SmmReady && mPrivate->PeriodicTimerProvider) {
      RemovePeriodicTimer(mPrivate->LegacyFreeHcTimerEvent);
    } else {
      GetMode (&Mode);
      if (Mode != USB_CORE_RUNTIME_MODE) {
        //
        // Restore the SetTimerPeriod
        //
        mPrivate->TimerArch->SetTimerPeriod = mPrivate->SetTimerPeriod;
        gBS->CloseEvent (mPrivate->LegacyFreeHcTimerEvent);
      }
    }
    mPrivate->LegacyFreeHcTimerEvent = NULL;
  }
  return EFI_SUCCESS;
}

/**

  Check is the device be detached

  @param  UsbIo                 UsbIo

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND

**/
EFI_STATUS
EFIAPI
CheckDeviceDetached (
  IN     EFI_USB_IO_PROTOCOL            *UsbIo
  )
{
  CALLBACK_TOKEN          *NonSmmCallbackToken;
  UINT8                   *UsbIoController;

  NonSmmCallbackToken = (CALLBACK_TOKEN*)&mPrivate->NonSmmCallbackListHead;
  do {
    if (IsListEmpty(&mPrivate->NonSmmCallbackListHead)) break;
    NonSmmCallbackToken = (CALLBACK_TOKEN*)GetFirstNode((LIST_ENTRY*)NonSmmCallbackToken);
    UsbIoController = (UINT8*)NonSmmCallbackToken->Context;
    //
    // Check the UsbIo is in the Context
    //
    if (*(UINTN*)UsbIoController == SIGNATURE_32 ('u', 's', 'b', 'd') && NonSmmCallbackToken->Event == DISCONNECT_CONTROLLER) {
      if ((UINTN)UsbIo == (UINTN)(UsbIoController + sizeof(UINTN))) {
        return EFI_SUCCESS;
      }
    }
  } while (!IsNodeAtEnd(&mPrivate->NonSmmCallbackListHead, (LIST_ENTRY*)NonSmmCallbackToken));
  //
  // In case this function be called after NonSmmCallback launched, the signature checking is needed due to UsbBus will clear it to indicated this is been freed
  //
  if (*(UINTN*)((UINTN)UsbIo - sizeof(UINTN)) != SIGNATURE_32 ('u', 's', 'b', 'd')) return EFI_SUCCESS;
  return EFI_NOT_FOUND;
}

/**

  Insert address convert table to convert boot service address to SMM address

  @param  BaseAddress           Address base
  @param  Dist                  Address distance
  @param  Count                 Number of address

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND

**/
EFI_STATUS
EFIAPI 
InsertAddressConvertTable (
  IN     UINTN                          Act,
  IN     VOID                           *BaseAddress,
  IN     UINTN                          Count
  )
{
  EFI_STATUS            Status;
  ADDRESS_CONVERT_TOKEN *Token;

  if (mPrivate->CurrMode == USB_CORE_RUNTIME_MODE) {
    //
    // Only available during POST time
    //
    return EFI_UNSUPPORTED;
  }
  Status = AllocateBuffer(
             sizeof(ADDRESS_CONVERT_TOKEN),
             ALIGNMENT_32,
             (VOID **)&Token
             );
  if (Status != EFI_SUCCESS) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Insert it to AddressConvertList
  //
  Token->BaseAddress = BaseAddress;
  Token->Act         = (UINT16)Act;
  Token->Count       = (UINT16)Count;
  InsertTailList (&mPrivate->AddressConvertListHead, (LIST_ENTRY*)Token);
  //
  // Records the number of instance body registered
  //
  if (Act == ACT_INSTANCE_BODY) {
    mPrivate->NumInstances ++;
  }
  return EFI_SUCCESS;
}

/**

  Remove address convert table which inserted by InsertAddressConvertTable

  @param  BaseAddress           Address base

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND

**/
EFI_STATUS
EFIAPI
RemoveAddressConvertTable (
  IN     UINTN                          Act,
  IN     VOID                           *BaseAddress
  )
{
  ADDRESS_CONVERT_TOKEN *Token;

  if (mPrivate->CurrMode == USB_CORE_RUNTIME_MODE) {
    //
    // Only available during POST time
    //
    return EFI_UNSUPPORTED;
  }
  Token = (ADDRESS_CONVERT_TOKEN*)&mPrivate->AddressConvertListHead;
  do {
    if (IsListEmpty(&mPrivate->AddressConvertListHead)) break;
    Token = (ADDRESS_CONVERT_TOKEN*)GetFirstNode((LIST_ENTRY*)Token);
    if (Token->Act == Act && Token->BaseAddress == BaseAddress) {
      RemoveEntryList((LIST_ENTRY*)Token);
      FreeBuffer(
        sizeof(ADDRESS_CONVERT_TOKEN),
        Token
        );
      //
      // Records the number of instance body registered
      //
      if (Act == ACT_INSTANCE_BODY) {
        mPrivate->NumInstances --;
      }
      break;
    }
  } while (!IsNodeAtEnd(&mPrivate->AddressConvertListHead, (LIST_ENTRY*)Token));
  return EFI_SUCCESS;
}

/**

  Address convert from/to NON_SMM_ADDRESS or SMM_ADDRESS

  @param  Type                  Convert to NON_SMM_ADDRESS or SMM_ADDRESS
  @param  Address               Address
  @param  ConvertedAddress      Target address

  @retval EFI_SUCCESS
  @retval EFI_NOT_FOUND

**/
EFI_STATUS
EFIAPI
AddressConvert (
  IN     UINTN                          Type,
  IN     VOID                           *Addr,
  OUT    VOID                           **ConvertedAddress
  )
{
  UINTN                 Address;
  IMAGE_ATTRIB          *ImageAttrib;
  UINTN                 Index;
  
  *ConvertedAddress = Addr;
  if (mPrivate->CurrMode == USB_CORE_RUNTIME_MODE) {
    //
    // Only available during POST time
    //
    return EFI_UNSUPPORTED;
  }
  //
  // Find out the image distance from IMAGE_ATTRIB array
  //
  ImageAttrib = &mPrivate->ImageAttrib[0];
  Address     = (UINTN)Addr;
  for (Index = 0; Index < mPrivate->NumImageAttrib; Index ++, ImageAttrib ++) {
    if (Type == NON_SMM_ADDRESS) {
      if (Address >= ImageAttrib->ImageBase2 && Address < ImageAttrib->ImageBase2 + ImageAttrib->ImageSize) {
        //
        // Convert SMM_ADDRESS to NON_SMM_ADDRESS
        //
        *ConvertedAddress = (VOID*)(Address - ImageAttrib->ImageDist);
        return EFI_SUCCESS;
      }
    } else {
      if (Address >= ImageAttrib->ImageBase1 && Address < ImageAttrib->ImageBase1 + ImageAttrib->ImageSize) {
        //
        // Convert NON_SMM_ADDRESS to SMM_ADDRESS
        //
        *ConvertedAddress = (VOID*)(Address + ImageAttrib->ImageDist);
        return EFI_SUCCESS;
      }
    }
  }
  return EFI_NOT_FOUND;
}

/**

  Address patching launched by LegacyBoot or ExitBootServices event

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
AddressPatching (
  )
{
  EFI_STATUS            Status;
  ADDRESS_CONVERT_TOKEN *Token;
  UINTN                 Address;
  IMAGE_ATTRIB          *ImageAttrib;
  IMAGE_ATTRIB          *InstanceAttrib = NULL;
  USB_CORE_PRIVATE      *Private;
  UINTN                 Index;
  UINTN                 Count;
  UINTN                 *Ptr;
  UINTN                 ThisAddress;
  UINTN                 NextAddress;
  INTN                  Dist;
  LIST_ENTRY            *BackLink;
  //
  // Remove the NonSmmCallbackListHead linking in BS
  //
  while (!IsListEmpty(&mPrivate->NonSmmCallbackListHead)) {
    Ptr = (UINTN*)GetFirstNode(&mPrivate->NonSmmCallbackListHead);
    RemoveEntryList((LIST_ENTRY*)Ptr);
    FreeBuffer(
      sizeof(CALLBACK_TOKEN),
      Ptr
      );
  }
  //
  // Step 1, function porinter convert
  //
  Token = (ADDRESS_CONVERT_TOKEN*)&mPrivate->AddressConvertListHead;
  do {
    if (IsListEmpty(&mPrivate->AddressConvertListHead)) break;
    Token = (ADDRESS_CONVERT_TOKEN*)GetFirstNode((LIST_ENTRY*)Token);
    if (Token->Act == ACT_FUNCTION_POINTER) {
      //
      // Find out the image distance from IMAGE_ATTRIB array
      //
      ImageAttrib = &mPrivate->ImageAttrib[0];
      Address     = *(UINTN*)Token->BaseAddress;
      Dist        = 0;
      for (Index = 0; Index < mPrivate->NumImageAttrib; Index ++, ImageAttrib ++) {
        if (Address >= ImageAttrib->ImageBase1 && Address < ImageAttrib->ImageBase1 + ImageAttrib->ImageSize) {
          Dist = ImageAttrib->ImageDist;
          break;
        }
      }
      if (Dist) {
        //
        // Patching the addresses
        //
        for (Index = 0, Ptr = (UINTN*)Token->BaseAddress; Index < (UINTN)Token->Count; Index ++, Ptr ++) {
          *Ptr = *Ptr + Dist;
        }
        //
        // Backup the backlink for safety removal
        //
        BackLink = ((LIST_ENTRY*)Token)->BackLink;
        RemoveEntryList((LIST_ENTRY*)Token);
        FreeBuffer(
          sizeof(ADDRESS_CONVERT_TOKEN),
          Token
          );
        //
        // Rollback to previous link
        //
        Token = (ADDRESS_CONVERT_TOKEN*)BackLink;
      }
    }
  } while (!IsNodeAtEnd(&mPrivate->AddressConvertListHead, (LIST_ENTRY*)Token));
  //
  // Step 2, instance body movement
  //
  if (mPrivate->NumInstances) {
    Status = AllocateBuffer (
               mPrivate->NumInstances * sizeof(IMAGE_ATTRIB),
               ALIGNMENT_32 | NON_SMM_ADDRESS,
               (VOID **)&InstanceAttrib
               );
    if (Status != EFI_SUCCESS) {
      return EFI_OUT_OF_RESOURCES;
    }
    ImageAttrib = InstanceAttrib;
    Token = (ADDRESS_CONVERT_TOKEN*)&mPrivate->AddressConvertListHead;
    do {
      if (IsListEmpty(&mPrivate->AddressConvertListHead)) break;
      Token = (ADDRESS_CONVERT_TOKEN*)GetFirstNode((LIST_ENTRY*)Token);
      if (Token->Act == ACT_INSTANCE_BODY) {
        //
        // Allocate memory in SMM for the instance
        //
        Status = AllocateBuffer (
                   Token->Count,
                   ALIGNMENT_32 | SMM_ADDRESS,
                   (VOID**)&ImageAttrib->ImageBase2
                   );
        if (Status != EFI_SUCCESS) {
          return EFI_OUT_OF_RESOURCES;
        }
        ImageAttrib->ImageBase1 = (UINTN)Token->BaseAddress;
        ImageAttrib->ImageSize  = Token->Count;
        //
        // Copy it
        //
        CopyMem (
          (VOID*)ImageAttrib->ImageBase2,
          (VOID*)ImageAttrib->ImageBase1,
          ImageAttrib->ImageSize
          );
        //
        // Free the original instance
        //
        FreeBuffer(
          Token->Count,
          (VOID*)ImageAttrib->ImageBase1
          );
        //
        // Backup the backlink for safety removal
        //
        BackLink = ((LIST_ENTRY*)Token)->BackLink;
        RemoveEntryList((LIST_ENTRY*)Token);
        FreeBuffer(
          sizeof(ADDRESS_CONVERT_TOKEN),
          Token
          );
        ImageAttrib ++;
        //
        // Rollback to previous link
        //
        Token = (ADDRESS_CONVERT_TOKEN*)BackLink;
      }
    } while (!IsNodeAtEnd(&mPrivate->AddressConvertListHead, (LIST_ENTRY*)Token));
  }
  //
  // Step 3, instance pointer patching
  //
  Token = (ADDRESS_CONVERT_TOKEN*)&mPrivate->AddressConvertListHead;
  Private = mPrivate;
  do {
    if (IsListEmpty(&mPrivate->AddressConvertListHead)) break;
    Token = (ADDRESS_CONVERT_TOKEN*)GetFirstNode((LIST_ENTRY*)Token);
    if (Token->Act == ACT_INSTANCE_POINTER) {
      //
      // In case the pointer located in the moved out instance, check it out and update to new instance
      //
      Address = (UINTN)Token->BaseAddress;
      for (Index = 0, ImageAttrib = InstanceAttrib; Index < mPrivate->NumInstances; Index ++, ImageAttrib ++) {
        if (Address >= ImageAttrib->ImageBase1 && Address < ImageAttrib->ImageBase1 + ImageAttrib->ImageSize) {
          //
          // Update the BaseAddress to new one
          //
          Token->BaseAddress = (VOID*)(ImageAttrib->ImageBase2 + ((UINTN)Token->BaseAddress - ImageAttrib->ImageBase1));
          break;
        }
      }
      for (Count = 0, Ptr = (UINTN*)Token->BaseAddress; Count < (UINTN)Token->Count; Count ++, Ptr ++) {
        //
        // Searching the new address
        //
        Address = *(UINTN*)Ptr;
        for (Index = 0, ImageAttrib = InstanceAttrib; Index < mPrivate->NumInstances; Index ++, ImageAttrib ++) {
          if (Address >= ImageAttrib->ImageBase1 && Address < ImageAttrib->ImageBase1 + ImageAttrib->ImageSize) {
            //
            // Patching the address
            //
            *(VOID**)Ptr = (VOID*)(ImageAttrib->ImageBase2 + (Address - ImageAttrib->ImageBase1));
            break;
          }
        }
      }
      //
      // Refresh mPrivate to update to latest status
      //
      if (mPrivate != Private) {
        CopyMem (
          mPrivate,
          Private,
          sizeof (USB_CORE_PRIVATE)
          );
        //
        // Reset linking head due to no longer used in runtime
        //
        InitializeListHead(&mPrivate->DeferredHcCallbackListHead);
        InitializeListHead(&mPrivate->NonSmmCallbackListHead);
        //
        // Re-chain the AddressConvertLink
        //
        mPrivate->AddressConvertListHead.ForwardLink->BackLink = &mPrivate->AddressConvertListHead;
        mPrivate->AddressConvertListHead.BackLink->ForwardLink = &mPrivate->AddressConvertListHead;
        Private = mPrivate;
      }
      //
      // Backup the backlink for safety removal
      //
      BackLink = ((LIST_ENTRY*)Token)->BackLink;
      RemoveEntryList((LIST_ENTRY*)Token);
      FreeBuffer(
        sizeof(ADDRESS_CONVERT_TOKEN),
        Token
        );
      //
      // Rollback to previous link
      //
      Token = (ADDRESS_CONVERT_TOKEN*)BackLink;
    }
  } while (!IsNodeAtEnd(&mPrivate->AddressConvertListHead, (LIST_ENTRY*)Token));
  //
  // Step 4, patching linking lists
  //
  Token = (ADDRESS_CONVERT_TOKEN*)&mPrivate->AddressConvertListHead;
  do {
    if (IsListEmpty(&mPrivate->AddressConvertListHead)) break;
    Token = (ADDRESS_CONVERT_TOKEN*)GetFirstNode((LIST_ENTRY*)Token);
    if (Token->Act == ACT_LINKING_LIST) {
      //
      // In case the linking list head located in the moved out instance, check it out and update to new instance
      //
      ThisAddress = (UINTN)Token->BaseAddress;
      do {
        Address = (UINTN)Token->BaseAddress;
        for (Index = 0, ImageAttrib = InstanceAttrib; Index < mPrivate->NumInstances; Index ++, ImageAttrib ++) {
          if (Address >= ImageAttrib->ImageBase1 && Address < ImageAttrib->ImageBase1 + ImageAttrib->ImageSize) {
            //
            // Update the BaseAddress to new one
            //
            Token->BaseAddress = (VOID*)(ImageAttrib->ImageBase2 + ((UINTN)Token->BaseAddress - ImageAttrib->ImageBase1));
            break;
          }
        }
        //
        // Searching for link entity in the instance body
        //
        NextAddress = (UINTN)((LIST_ENTRY*)Token->BaseAddress)->ForwardLink;
        for (Index = 0, ImageAttrib = InstanceAttrib; Index < mPrivate->NumInstances; Index ++, ImageAttrib ++) {
          if (NextAddress >= ImageAttrib->ImageBase1 && NextAddress < ImageAttrib->ImageBase1 + ImageAttrib->ImageSize) {
            //
            // Patching the link each other
            //
            ((LIST_ENTRY*)Token->BaseAddress)->ForwardLink  = (VOID*)(ImageAttrib->ImageBase2 + (NextAddress - ImageAttrib->ImageBase1));
            ((LIST_ENTRY*)((LIST_ENTRY*)Token->BaseAddress)->ForwardLink)->BackLink = (VOID*)Token->BaseAddress;
            break;
          }
        }
        Token->BaseAddress = (VOID*)NextAddress;
      } while (Token->BaseAddress != (VOID*)ThisAddress);
      //
      // Backup the backlink for safety removal
      //
      BackLink = ((LIST_ENTRY*)Token)->BackLink;
      RemoveEntryList((LIST_ENTRY*)Token);
      FreeBuffer(
        sizeof(ADDRESS_CONVERT_TOKEN),
        Token
        );
      //
      // Rollback to previous link
      //
      Token = (ADDRESS_CONVERT_TOKEN*)BackLink;
    }
  } while (!IsNodeAtEnd(&mPrivate->AddressConvertListHead, (LIST_ENTRY*)Token));
  //
  // Free instance buffer
  //
  if (mPrivate->NumInstances) {
    FreeBuffer (
      mPrivate->NumInstances * sizeof(IMAGE_ATTRIB),
      InstanceAttrib
      );
    mPrivate->NumInstances = 0;
  }
  return EFI_SUCCESS;
}
