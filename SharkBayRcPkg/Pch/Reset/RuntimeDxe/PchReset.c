/** @file
  PCH RESET Runtime Driver

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#include <PchReset.h>

PCH_RESET_INSTANCE  *mPchResetInstance;

STATIC UINT8        mDaysOfMonthInfo[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

/**
  Check if it is leap year

  @param[in] Year            year to be check

  @retval True               year is leap year
  @retval FALSE              year is not a leap year
**/
BOOLEAN
IsLeapYear (
  IN UINT16 Year
  )
{
  return (Year % 4 == 0) && ((Year % 100 != 0) || (Year % 400 == 0));
}

/**
  Set System Wakeup Alarm.

  @param[in] WakeAfter       Time offset in seconds to wake from S3

  @retval EFI_SUCCESS        Timer started successfully
**/

STATIC
EFI_STATUS
SetSystemWakeupAlarm (
  IN       UINT32          WakeAfter
  )
{
  EFI_STATUS              Status;
  EFI_TIME                Time;
  EFI_TIME_CAPABILITIES   Capabilities;
  UINT32                  Reminder;
  UINT16                  PmBase;
  UINT8                   DayOfMonth;
  ///
  /// For an instant wake 2 seconds is a safe value
  ///
  if (WakeAfter < 2) {
    WakeAfter = 2;
  }

  Status = EfiGetTime (&Time, &Capabilities);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  
  Reminder = WakeAfter + (UINT32) Time.Second;
  Time.Second = Reminder % 60;
  Reminder = Reminder / 60;
  Reminder = Reminder + (UINT32) Time.Minute;
  Time.Minute = Reminder % 60;
  Reminder = Reminder / 60;
  Reminder = Reminder + (UINT32) Time.Hour;
  Time.Hour = Reminder % 24;
  Reminder = Reminder / 24;

  if (Reminder > 0) {
    Reminder = Reminder + (UINT32) Time.Day;
    if ((Time.Month == 2) && IsLeapYear (Time.Year)) {
      DayOfMonth = 29;
    } else {
      DayOfMonth = mDaysOfMonthInfo[Time.Month - 1];
    }
    if (Reminder > DayOfMonth) {
      Time.Day = (UINT8)Reminder - DayOfMonth;
      Reminder = 1;
    } else {
      Time.Day = (UINT8)Reminder;
      Reminder = 0;
    }
  }

  if (Reminder > 0) {
    if (Time.Month == 12) {
      Time.Month = 1;
      Time.Year = Time.Year + 1;
    } else {
      Time.Month = Time.Month + 1;
    }
  }

  Status = EfiSetWakeupTime (TRUE, &Time);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  PmBase = (UINT16) (PciRead32 (
                      PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC,
                      R_PCH_LPC_ACPI_BASE)
                      ) & B_PCH_LPC_ACPI_BASE_BAR);

  ///
  /// Clear RTC PM1 status
  ///
  IoWrite16 (PmBase + R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_RTC);

  ///
  /// set RTC_EN bit in PM1_EN to wake up from the alarm
  ///
  IoWrite16 (
    PmBase + R_PCH_ACPI_PM1_EN,
    (IoRead16 (PmBase + R_PCH_ACPI_PM1_EN) | B_PCH_ACPI_PM1_EN_RTC)
    );
  return Status;
}

/**
  Initialize the state information for the Timer Architectural Protocol

  @param[in] ImageHandle          Image handle of the loaded driver
  @param[in] SystemTable          Pointer to the System Table

  @retval EFI_SUCCESS             Thread can be successfully created
  @retval EFI_OUT_OF_RESOURCES    Cannot allocate protocol data structure
  @retval EFI_DEVICE_ERROR        Cannot create the timer service
**/
EFI_STATUS
EFIAPI
InstallPchReset (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;
  UINT64      BaseAddress;
  UINT64      Length;
  EFI_GCD_MEMORY_SPACE_DESCRIPTOR MemorySpaceDescriptor;
  UINT64                          Attributes;

  DEBUG ((EFI_D_INFO, "InstallPchReset() Start\n"));

  Status = PciLibConstructor ();
  ASSERT_EFI_ERROR (Status);

  BaseAddress = MmPciAddress(0,
                             DEFAULT_PCI_BUS_NUMBER_PCH,
                             PCI_DEVICE_NUMBER_PCH_LPC,
                             PCI_FUNCTION_NUMBER_PCH_LPC,
                             0
                             );
  Length  = 4096;

  Status  = gDS->GetMemorySpaceDescriptor (BaseAddress, &MemorySpaceDescriptor);
  ASSERT_EFI_ERROR (Status);

  Attributes = MemorySpaceDescriptor.Attributes | EFI_MEMORY_RUNTIME;

  Status = gDS->SetMemorySpaceAttributes (
                  BaseAddress,
                  Length,
                  Attributes
                  );
  ASSERT_EFI_ERROR (Status);

  Status = PciLibRegisterMemory (
            PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
            PCI_DEVICE_NUMBER_PCH_LPC,
            PCI_FUNCTION_NUMBER_PCH_LPC,
            0),
            (UINTN) Length
            );
  ASSERT_EFI_ERROR (Status);
  ///
  /// Allocate Runtime memory for the PchReset protocol instance.
  ///
  mPchResetInstance = AllocateRuntimeZeroPool (sizeof (PCH_RESET_INSTANCE));
  if (mPchResetInstance == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = PchResetProtocolConstructor (mPchResetInstance);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Install protocol interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mPchResetInstance->Handle,
                  &gPchResetProtocolGuid,
                  &mPchResetInstance->PchResetProtocol,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  ///
  /// The Lib Deconstruct will automatically be called when entrypoint return error.
  ///
  DEBUG ((EFI_D_INFO, "InstallPchReset() End\n"));

  return Status;
}

#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
/**
  If need be, do any special reset required for capsules. For this
  implementation where we're called from the ResetSystem() api,
  just set our capsule variable and return to let the caller
  do a soft reset.

  @param[in] None

  @retval None
**/
VOID
CapsuleReset (
  VOID
  )
{
  EFI_STATUS  Status;
  UINTN       Size;
  UINTN       CapsuleDataPtr;
  UINT32      Data32;
  UINT32      Eflags;
  UINT16      PmBase;

  ///
  /// Check if there are pending capsules to process
  ///
  Size = sizeof (CapsuleDataPtr);
  Status = EfiGetVariable (
            EFI_CAPSULE_VARIABLE_NAME,
            &gEfiCapsuleVendorGuid,
            NULL,
            &Size,
            (VOID *) &CapsuleDataPtr
            );

  if (Status == EFI_SUCCESS) {
    ///
    /// Wake up system 2 seconds after putting system into S3 to complete the reset operation.
    ///
    SetSystemWakeupAlarm (2);
    ///
    /// Process capsules across a system reset.
    ///
    PmBase = PciRead16 (
              PCI_LIB_ADDRESS (DEFAULT_PCI_BUS_NUMBER_PCH,
              PCI_DEVICE_NUMBER_PCH_LPC,
              PCI_FUNCTION_NUMBER_PCH_LPC,
              R_PCH_LPC_ACPI_BASE)
              ) & B_PCH_LPC_ACPI_BASE_BAR;

    ASSERT (PmBase != 0);

    Data32  = IoRead32 ((UINTN) (PmBase + R_PCH_ACPI_PM1_CNT));

    Data32  = (UINT32) ((Data32 & ~(B_PCH_ACPI_PM1_CNT_SLP_TYP + B_PCH_ACPI_PM1_CNT_SLP_EN)) | V_PCH_ACPI_PM1_CNT_S3);

    Eflags  = (UINT32) AsmReadEflags ();

    if ((Eflags & 0x200)) {
      DisableInterrupts ();
    }

    AsmWbinvd ();
    AsmWriteCr0 (AsmReadCr0 () | 0x060000000);

    IoWrite32 (
      (UINTN) (PmBase + R_PCH_ACPI_PM1_CNT),
      (UINT32) Data32
      );

    Data32 = Data32 | B_PCH_ACPI_PM1_CNT_SLP_EN;

    IoWrite32 (
      (UINTN) (PmBase + R_PCH_ACPI_PM1_CNT),
      (UINT32) Data32
      );

    if ((Eflags & 0x200)) {
      EnableInterrupts ();
    }
    ///
    /// Should not return
    ///
    EFI_DEADLOOP ();
  }
}
#endif

/**
  Execute call back function for Pch Reset.

  @param[in] PchResetType         Pch Reset Types which includes PowerCycle, Globalreset.

  @retval EFI_SUCCESS             The callback function has been done successfully
  @retval EFI_NOT_FOUND           Failed to find Pch Reset Callback protocol. Or, none of
                                  callback protocol is installed.
  @retval Others                  Do not do any reset from PCH
**/
EFI_STATUS
EFIAPI
PchResetCallback (
  IN     PCH_RESET_TYPE           PchResetType
  )
{
  EFI_STATUS                  Status;
  UINTN                       NumHandles;
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       Index;
  PCH_RESET_CALLBACK_PROTOCOL *PchResetCallback;

  if (EfiAtRuntime () == FALSE) {
    ///
    /// Retrieve all instances of Pch Reset Callback protocol
    ///
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gPchResetCallbackProtocolGuid,
                    NULL,
                    &NumHandles,
                    &HandleBuffer
                    );

    if (EFI_ERROR (Status)) {
      ///
      /// Those drivers that need to install Pch Reset Callback protocol have the responsibility
      /// to make sure themselves execute before Pch Reset Runtime driver.
      ///
      if (Status == EFI_NOT_FOUND) {
        DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Or, none of Pch Reset callback protocol is installed.\n"));
      }

      return Status;
    }

    for (Index = 0; Index < NumHandles; Index++) {
      Status = gBS->HandleProtocol (
                      HandleBuffer[Index],
                      &gPchResetCallbackProtocolGuid,
                      (VOID **) &PchResetCallback
                      );
      ASSERT_EFI_ERROR (Status);

      if (!EFI_ERROR (Status)) {
        PchResetCallback->ResetCallback (PchResetType);
      } else {
        DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Failed to locate Pch Reset Callback protocol.\n"));
        return Status;
      }
    }
  }

#if (EFI_SPECIFICATION_VERSION >= 0x00020000)
  if (PchResetType == WarmReset) {
    CapsuleReset ();
  }
#endif

  return EFI_SUCCESS;
}

/**
  Fixup internal data pointers so that the services can be called in virtual mode.

  @param[in] Event                The event registered.
  @param[in] Context              Event context. Not used in this event handler.

  @retval None
**/
VOID
PchResetVirtualAddressChangeEvent (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID *) &(mPchResetInstance->PchResetProtocol.Reset));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID *) &(mPchResetInstance->PchRootComplexBar));
  gRT->ConvertPointer (EFI_OPTIONAL_PTR, (VOID *) &(mPchResetInstance));
}
