/** @file
  This driver is responsible for the registration of child drivers
  and the abstraction of the PCH SMI sources.

;******************************************************************************
;* Copyright (c) 1983 - 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

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
#include <PchSmm.h>
#include <PchSmmHelpers.h>

///
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// MODULE / GLOBAL DATA
///
/// Module variables used by the both the main dispatcher and the source dispatchers
/// Declared in PchSmmSources.h
///
EFI_SMM_SYSTEM_TABLE  *mSmst;
UINT32                mAcpiBaseAddr;
UINT32                mGpioBaseAddr;
EFI_SMM_BASE_PROTOCOL *mSmmBase;
//
// The reserved mmio address can only be used in Sx handler
// it's not reserved in ACPI.
//
EFI_PHYSICAL_ADDRESS  mResvMmioBaseAddr;

PRIVATE_DATA          mPrivateData = {  ///< for the structure
  {
    NULL
  },                                    ///< CallbackDataBase linked list head
  NULL,                                 ///< EFI handle returned when calling InstallMultipleProtocolInterfaces
  {                                     ///< protocol arrays
    ///
    /// elements within the array
    ///
    {
      PROTOCOL_SIGNATURE,
      UsbType,
      {
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister
      }
    },
    {
      PROTOCOL_SIGNATURE,
      SxType,
      {
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister
      }
    },
    {
      PROTOCOL_SIGNATURE,
      SwType,
      {
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister,
        (UINTN) MAXIMUM_SWI_VALUE
      }
    },
    {
      PROTOCOL_SIGNATURE,
      GpiType,
      {
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister,
        (UINTN) NUM_SUPPORTED_GPIS
      }
    },
    {
      PROTOCOL_SIGNATURE,
      IchnType,
      {
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister
      }
    },
    {
      PROTOCOL_SIGNATURE,
      IchnExType,
      {
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister
      }
    },
    {
      PROTOCOL_SIGNATURE,
      PowerButtonType,
      {
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister
      }
    },
    {
      PROTOCOL_SIGNATURE,
      PeriodicTimerType,
      {
        (PCH_SMM_GENERIC_REGISTER) PchSmmCoreRegister,
        (PCH_SMM_GENERIC_UNREGISTER) PchSmmCoreUnRegister,
        (UINTN) PchSmmPeriodicTimerDispatchGetNextShorterInterval
      }
    },
  }
};

CONTEXT_FUNCTIONS     mContextFunctions[PchSmmProtocolTypeMax] = {
  {
    NULL,
    NULL
  },
  {
    SxGetContext,
    SxCmpContext
  },
  {
    SwGetContext,
    SwCmpContext
  },
  {
    NULL,
    NULL
  },
  {
    NULL,
    NULL
  },
  {
    NULL,
    NULL
  },
  {
    PowerButtonGetContext,
    PowerButtonCmpContext
  },
  {
    PeriodicTimerGetContext,
    PeriodicTimerCmpContext
  },
};

///
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// PROTOTYPES
///
/// Functions use only in this file
///
EFI_STATUS
EFIAPI
PchSmmCoreDispatcher (
  IN EFI_HANDLE             SmmImageHandle,
  IN OUT VOID               *CommunicationBuffer,
  IN OUT UINTN              *SourceSize
  );

///
/// ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// FUNCTIONS
///
/// Driver entry point
///

/**
  Initializes the PCH SMM Dispatcher

  @param[in] ImageHandle          Pointer to the loaded image protocol for this driver
  @param[in] SystemTable          Pointer to the EFI System Table

  @retval EFI_SUCCESS             PchSmmDispatcher Initialization completed.
**/
EFI_STATUS
EFIAPI
InitializePchSmmDispatcher (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  ///
  /// Locate SMM Base Protocol
  ///
  Status = gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, (VOID **) &mSmmBase);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return Status;
  }
  ///
  /// Initialize our module variables
  ///
  Status = mSmmBase->GetSmstLocation (mSmmBase, &mSmst);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return Status;
  }

  PERF_START_EX (ImageHandle, "SmmModule", NULL, AsmReadTsc(), 8);

  ///
  /// Access ACPI Base Addresses Register
  ///
  mAcpiBaseAddr = (UINT32) (MmioRead16 (
                              MmPciAddress (0,
                              DEFAULT_PCI_BUS_NUMBER_PCH,
                              PCI_DEVICE_NUMBER_PCH_LPC,
                              PCI_FUNCTION_NUMBER_PCH_LPC,
                              R_PCH_LPC_ACPI_BASE)
                              ) & B_PCH_LPC_ACPI_BASE_BAR);
  ASSERT (mAcpiBaseAddr != 0);

  ///
  /// Access GPIO Base Addresses Register
  ///
  mGpioBaseAddr = (UINT32) (MmioRead16 (
                              MmPciAddress (0,
                              DEFAULT_PCI_BUS_NUMBER_PCH,
                              PCI_DEVICE_NUMBER_PCH_LPC,
                              PCI_FUNCTION_NUMBER_PCH_LPC,
                              R_PCH_LPC_GPIO_BASE)
                              ) & B_PCH_LPC_GPIO_BASE_BAR);
  ASSERT (mGpioBaseAddr != 0);

  ///
  /// Fix the ICHN_SOURCE_DESCS base, in order to obtain independence with platform.
  ///
  PchSmmIchnFixSourceBase ();

  PchSmmPublishDispatchProtocols ();

  ///
  /// Register a callback function to handle subsequent SMIs.  This callback
  /// will be called by SmmCoreDispatcher.
  ///
  mSmmBase->RegisterCallback (mSmmBase, ImageHandle, PchSmmCoreDispatcher, FALSE, FALSE);

  ///
  /// Initialize Callback DataBase
  ///
  InitializeListHead (&mPrivateData.CallbackDataBase);

  ///
  /// Enable SMIs on the PCH now that we have a callback
  ///
  PchSmmInitHardware ();

  mResvMmioBaseAddr = 0x0ffffffff;
  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateMaxAddressSearchBottomUp,
                  EfiGcdMemoryTypeMemoryMappedIo,
                  16,  // 2^16: 64K Alignment
                  0x10000,     // 64K Length
                  &mResvMmioBaseAddr,
                  ImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  DEBUG((EFI_D_INFO,"mResvMmioBaseAddr %x\n", mResvMmioBaseAddr));

  return EFI_SUCCESS;
}

/**
  Check the Fed SwSmiInputValue to see if there is a duplicated one in the database

  @param[in] FedSwSmiInputValue   Fed SwSmiInputValue

  @retval EFI_SUCCESS             There is no duplicated SwSmiInputValue
  @retval EFI_INVALID_PARAMETER   There is a duplicated SwSmiInputValue
**/
EFI_STATUS
SmiInputValueDuplicateCheck (
  UINTN           FedSwSmiInputValue
  )
{

  DATABASE_RECORD *RecordInDb;
  LIST_ENTRY      *LinkInDb;

  LinkInDb = GetFirstNode (&mPrivateData.CallbackDataBase);
  while (!IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
    RecordInDb = DATABASE_RECORD_FROM_LINK (LinkInDb);

    if (RecordInDb->ProtocolType == SwType) {
      if (RecordInDb->ChildContext.Sw.SwSmiInputValue == FedSwSmiInputValue) {
        return EFI_INVALID_PARAMETER;
      }
    }

    LinkInDb = GetNextNode (&mPrivateData.CallbackDataBase, &RecordInDb->Link);
  }

  return EFI_SUCCESS;
}

/**
  Register a child SMI dispatch function with a parent SMM driver.

  @param[in] This                 Pointer to the PCH_SMM_GENERIC_PROTOCOL instance.
  @param[in] DispatchFunction     Pointer to dispatch function to be invoked for this SMI source.
  @param[in] DispatchContext      Pointer to the dispatch function's context.
  @param[out] DispatchHandle      Handle of dispatch function, for when interfacing
                                  with the parent SMM driver, will be the address of linked
                                  list link in the call back record.

  @retval EFI_OUT_OF_RESOURCES    Insufficient resources to create database record
  @retval EFI_INVALID_PARAMETER   The input parameter is invalid
  @retval EFI_SUCCESS             The dispatch function has been successfully
                                  registered and the SMI source has been enabled.
**/
EFI_STATUS
PchSmmCoreRegister (
  IN  PCH_SMM_GENERIC_PROTOCOL                          *This,
  IN  PCH_SMM_CALLBACK                                  DispatchFunction,
  IN  PCH_SMM_CONTEXT                                   *DispatchContext,
  OUT EFI_HANDLE                                        *DispatchHandle
  )
{
  EFI_STATUS                  Status;
  DATABASE_RECORD             *Record;
  PCH_SMM_QUALIFIED_PROTOCOL  *Qualified;
  PCH_SMM_SOURCE_DESC         NullSourceDesc = NULL_SOURCE_DESC_INITIALIZER;
  PCH_SERIES                  PchSeries;
  UINTN                       Index;

  PchSeries = GetPchSeries();
  Index     = 0;
  ///
  /// Create database record and add to database
  ///
  if (mSmst == NULL) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }

  Status = mSmst->SmmAllocatePool (EfiRuntimeServicesData, sizeof (DATABASE_RECORD), (VOID **) &Record);
  if (EFI_ERROR (Status)) {
    ASSERT (FALSE);
    return EFI_OUT_OF_RESOURCES;
  }
  ///
  /// Gather information about the registration request
  ///
  Record->Callback          = DispatchFunction;
  Record->ChildContext      = *DispatchContext;

  Qualified                 = QUALIFIED_PROTOCOL_FROM_GENERIC (This);

  Record->ProtocolType      = Qualified->Type;

  Record->ContextFunctions  = mContextFunctions[Qualified->Type];
  ///
  /// Perform linked list housekeeping
  ///
  Record->Signature = DATABASE_RECORD_SIGNATURE;

  switch (Qualified->Type) {
  ///
  /// By the end of this switch statement, we'll know the
  /// source description the child is registering for
  ///
  case UsbType:
    ///
    /// Check the validity of Context Type
    ///
    if ((Record->ChildContext.Usb.Type < UsbLegacy) || (Record->ChildContext.Usb.Type > UsbWake)) {
      goto Error;
    }

    InsertTailList (&mPrivateData.CallbackDataBase, &Record->Link);
    MapUsbToSrcDesc (DispatchContext, &(Record->SrcDesc));
    Record->ClearSource = NULL;
    ///
    /// use default clear source function
    ///
    break;

  case SxType:
    ///
    /// Check the validity of Context Type and Phase
    ///
    if ((Record->ChildContext.Sx.Type < SxS0) ||
        (Record->ChildContext.Sx.Type >= EfiMaximumSleepType) ||
        (Record->ChildContext.Sx.Phase < SxEntry) ||
        (Record->ChildContext.Sx.Phase >= EfiMaximumPhase)
        ) {
      goto Error;
    }

    InsertTailList (&mPrivateData.CallbackDataBase, &Record->Link);
    CopyMem ((VOID *) &(Record->SrcDesc), (VOID *) (&SX_SOURCE_DESC), sizeof (PCH_SMM_SOURCE_DESC));
    Record->ClearSource = NULL;
    ///
    /// use default clear source function
    ///
    break;

  case SwType:
    ///
    /// Check the validity of Context Value
    ///
    if (Record->ChildContext.Sw.SwSmiInputValue > MAXIMUM_SWI_VALUE) {
      goto Error;
    }

    if (EFI_ERROR (SmiInputValueDuplicateCheck (Record->ChildContext.Sw.SwSmiInputValue))) {
      goto Error;
    }

    InsertTailList (&mPrivateData.CallbackDataBase, &Record->Link);
    CopyMem ((VOID *) &(Record->SrcDesc), (VOID *) (&SW_SOURCE_DESC), sizeof (PCH_SMM_SOURCE_DESC));
    Record->ClearSource = NULL;
    ///
    /// use default clear source function
    ///
    break;

  case GpiType:
    Index = Record->ChildContext.Gpi.GpiNum;
    if (PchSeries == PchH) {
      Index -= V_PCH_LPTH_ALT_GP_SMI_GPIBASE;
      if (Index >= S_PCH_LPTH_ALT_GP_SMI_GPISIZE) {
        goto Error;
      }
    } else if (PchSeries == PchLp) {
      Index -= V_PCH_LPTLP_ALT_GP_SMI_GPIBASE;
      if (Index >= S_PCH_LPTLP_ALT_GP_SMI_GPISIZE) {
        goto Error;
      }
    } else {
      goto Error;
    }
    InsertTailList (&mPrivateData.CallbackDataBase, &Record->Link);
    if (PchSeries == PchH) {
      CopyMem (
        (VOID *) &(Record->SrcDesc),
        (VOID *) &(LPTH_GPI_SOURCE_DESC[Index]),
        sizeof (PCH_SMM_SOURCE_DESC)
      );
    } else if (PchSeries == PchLp) {
      CopyMem (
        (VOID *) &(Record->SrcDesc),
        (VOID *) &(LPTLP_GPI_SOURCE_DESC[Index]),
        sizeof (PCH_SMM_SOURCE_DESC)
      );
    }
    Record->ClearSource = NULL;
    ///
    /// use default clear source function
    ///
    break;

  case IchnType:
    ///
    /// Check the validity of Context Type
    ///
    if (Record->ChildContext.Ichn.Type >= NUM_ICHN_TYPES) {
      goto Error;
    }

    InsertTailList (&mPrivateData.CallbackDataBase, &Record->Link);
    if (PchSeries == PchLp) {
      CopyMem (
        (VOID *) &(Record->SrcDesc),
        (VOID *) &(ICHN_LP_SOURCE_DESCS[Record->ChildContext.Ichn.Type]),
        sizeof (PCH_SMM_SOURCE_DESC)
        );
    } else if (PchSeries == PchH) {
      CopyMem (
        (VOID *) &(Record->SrcDesc),
        (VOID *) &(ICHN_H_SOURCE_DESCS[Record->ChildContext.Ichn.Type]),
        sizeof (PCH_SMM_SOURCE_DESC)
        );
    }
    Record->ClearSource = PchSmmIchnClearSource;
    break;

  case IchnExType:
    ///
    /// Check the validity of Context Type
    ///
    if ((Record->ChildContext.IchnEx.Type < IchnExPciExpress) || (Record->ChildContext.IchnEx.Type >= IchnExTypeMAX)) {
      goto Error;
    }

    InsertTailList (&mPrivateData.CallbackDataBase, &Record->Link);
    CopyMem (
      (VOID *) &(Record->SrcDesc),
      (VOID *) &(ICHN_EX_SOURCE_DESCS[Record->ChildContext.IchnEx.Type - IchnExPciExpress]),
      sizeof (PCH_SMM_SOURCE_DESC)
      );
    Record->ClearSource = NULL;
    break;

  case PowerButtonType:
    ///
    /// Check the validity of Context Phase
    ///
    if ((Record->ChildContext.PowerButton.Phase < PowerButtonEntry) ||
        (Record->ChildContext.PowerButton.Phase > PowerButtonExit)
        ) {
      goto Error;
    }

    InsertTailList (&mPrivateData.CallbackDataBase, &Record->Link);
    CopyMem ((VOID *) &(Record->SrcDesc), (VOID *) &POWER_BUTTON_SOURCE_DESC, sizeof (PCH_SMM_SOURCE_DESC));
    Record->ClearSource = NULL;
    ///
    /// use default clear source function
    ///
    break;

  case PeriodicTimerType:
    ///
    /// Check the validity of timer value
    ///
    if (DispatchContext->PeriodicTimer.SmiTickInterval <= 0) {
      goto Error;
    }

    InsertTailList (&mPrivateData.CallbackDataBase, &Record->Link);
    MapPeriodicTimerToSrcDesc (DispatchContext, &(Record->SrcDesc));
    Record->ClearSource = PchSmmPeriodicTimerClearSource;
    break;

  default:
    return EFI_INVALID_PARAMETER;
    break;
  }

  if (CompareSources (&Record->SrcDesc, &NullSourceDesc)) {
    goto Error;
  }

  if (Record->ClearSource == NULL) {
    ///
    /// Clear the SMI associated w/ the source using the default function
    ///
    PchSmmClearSource (&Record->SrcDesc);
  } else {
    ///
    /// This source requires special handling to clear
    ///
    Record->ClearSource (&Record->SrcDesc);
  }

  PchSmmEnableSource (&Record->SrcDesc);

  ///
  /// Child's handle will be the address linked list link in the record
  ///
  *DispatchHandle = (EFI_HANDLE) (&Record->Link);

  return EFI_SUCCESS;

Error:
  Status = mSmst->SmmFreePool (Record);
  ///
  /// DEBUG((EFI_D_ERROR,"Free pool status %d\n", Status ));
  ///
  return EFI_INVALID_PARAMETER;
}

/**
  Unregister a child SMI source dispatch function with a parent SMM driver.

  @param[in] This                 Pointer to the  EFI_SMM_IO_TRAP_DISPATCH_PROTOCOL instance.
  @param[in] DispatchHandle       Handle of dispatch function to deregister.

  @retval EFI_SUCCESS             The dispatch function has been successfully
                                  unregistered and the SMI source has been disabled
                                  if there are no other registered child dispatch
                                  functions for this SMI source.
  @retval EFI_INVALID_PARAMETER   Handle is invalid.
**/
EFI_STATUS
PchSmmCoreUnRegister (
  IN PCH_SMM_GENERIC_PROTOCOL                           *This,
  IN EFI_HANDLE                                         *DispatchHandle
  )
{
  ///
  /// BOOLEAN SafeToDisable;
  ///
  DATABASE_RECORD *RecordToDelete;

  ///
  /// DATABASE_RECORD *RecordInDb;
  /// EFI_LIST_NODE   *LinkInDb;
  ///
  if (DispatchHandle == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  RecordToDelete = DATABASE_RECORD_FROM_LINK (DispatchHandle);

  ///
  /// Take the entry out of the linked list
  ///
  if (RecordToDelete->Link.ForwardLink == (LIST_ENTRY *) (UINTN) EFI_BAD_POINTER) {
    return EFI_INVALID_PARAMETER;
  }

  RemoveEntryList (&RecordToDelete->Link);

  ///
  /// See if we can disable the source, reserved for future use since this might
  ///  not be the only criteria to disable
  ///
  /// SafeToDisable = TRUE;
  /// LinkInDb = GetFirstNode (&mPrivateData.CallbackDataBase);
  /// while(!IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
  /// RecordInDb = DATABASE_RECORD_FROM_LINK (LinkInDb);
  /// if (CompareEnables (&RecordToDelete->SrcDesc, &RecordInDb->SrcDesc)) {
  /// SafeToDisable = FALSE;
  /// break;
  /// }
  /// LinkInDb = GetNextNode (&mPrivateData.CallbackDataBase, &RecordInDb->Link);
  /// }
  /// if (SafeToDisable) {
  /// PchSmmDisableSource( &Record->SrcDesc );
  /// }
  ///
  return EFI_SUCCESS;
}

/**
  The callback function to handle subsequent SMIs.  This callback will be called by SmmCoreDispatcher.

  @param[in] SmmImageHandle             Not used
  @param[in, out] CommunicationBuffer   Not used
  @param[in, out] SourceSize            Not used

  @retval EFI_SUCCESS                   Function successfully completed
**/
EFI_STATUS
EFIAPI
PchSmmCoreDispatcher (
  IN EFI_HANDLE             SmmImageHandle,
  IN OUT VOID               *CommunicationBuffer,
  IN OUT UINTN              *SourceSize
  )
{
  ///
  /// Used to prevent infinite loops
  ///
  UINTN               EscapeCount;

  BOOLEAN             ContextsMatch;
  BOOLEAN             EosSet;
  BOOLEAN             SxChildWasDispatched;

  DATABASE_RECORD     *RecordInDb;
  LIST_ENTRY          *LinkInDb;
  DATABASE_RECORD     *RecordToExhaust;
  LIST_ENTRY          *LinkToExhaust;

  PCH_SMM_CONTEXT     Context;

  EFI_STATUS          Status;

  PCH_SMM_SOURCE_DESC ActiveSource = NULL_SOURCE_DESC_INITIALIZER;

  EscapeCount           = 100;
  ContextsMatch         = FALSE;
  EosSet                = FALSE;
  SxChildWasDispatched  = FALSE;
  Status                = EFI_SUCCESS;

  if (!IsListEmpty (&mPrivateData.CallbackDataBase)) {
    ///
    /// We have children registered w/ us -- continue
    ///
    while ((!EosSet) && (EscapeCount > 0)) {
      EscapeCount--;

      LinkInDb = GetFirstNode (&mPrivateData.CallbackDataBase);

      while (!IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
        RecordInDb = DATABASE_RECORD_FROM_LINK (LinkInDb);

        ///
        /// look for the first active source
        ///
        if (!SourceIsActive (&RecordInDb->SrcDesc)) {
          ///
          /// Didn't find the source yet, keep looking
          ///
          LinkInDb = GetNextNode (&mPrivateData.CallbackDataBase, &RecordInDb->Link);

          ///
          /// if it's the last one, try to clear EOS
          ///
          if (IsNull (&mPrivateData.CallbackDataBase, LinkInDb)) {
            EosSet = PchSmmSetAndCheckEos ();
          }
        } else {
          ///
          /// We found a source. If this is a sleep type, we have to go to
          /// appropriate sleep state anyway.No matter there is sleep child or not
          ///
          if (RecordInDb->ProtocolType == SxType) {
            SxChildWasDispatched = TRUE;
          }
          ///
          /// "cache" the source description and don't query I/O anymore
          ///
          CopyMem ((VOID *) &ActiveSource, (VOID *) &(RecordInDb->SrcDesc), sizeof (PCH_SMM_SOURCE_DESC));
          LinkToExhaust = LinkInDb;

          ///
          /// exhaust the rest of the queue looking for the same source
          ///
          while (!IsNull (&mPrivateData.CallbackDataBase, LinkToExhaust)) {
            RecordToExhaust = DATABASE_RECORD_FROM_LINK (LinkToExhaust);
            ///
            /// RecordToExhaust->Link might be removed (unregistered) by Callback function, and then the
            /// system will hang in ASSERT() while calling GetNextNode().
            /// To prevent the issue, we need to get next record in DB here (before Callback function).
            ///
            LinkToExhaust = GetNextNode (&mPrivateData.CallbackDataBase, &RecordToExhaust->Link);

            if (CompareSources (&RecordToExhaust->SrcDesc, &ActiveSource)) {
              ///
              /// These source descriptions are equal, so this callback should be
              /// dispatched.
              ///
              if (RecordToExhaust->ContextFunctions.GetContext != NULL) {
                ///
                /// This child requires that we get a calling context from
                /// hardware and compare that context to the one supplied
                /// by the child.
                ///
                ASSERT (RecordToExhaust->ContextFunctions.CmpContext != NULL);

                ///
                /// Make sure contexts match before dispatching event to child
                ///
                RecordToExhaust->ContextFunctions.GetContext (RecordToExhaust, &Context);
                ContextsMatch = RecordToExhaust->ContextFunctions.CmpContext (&Context, &RecordToExhaust->ChildContext);

              } else {
                ///
                /// This child doesn't require any more calling context beyond what
                /// it supplied in registration.  Simply pass back what it gave us.
                ///
                ASSERT (RecordToExhaust->Callback != NULL);
                Context       = RecordToExhaust->ChildContext;
                ContextsMatch = TRUE;
              }

              if (ContextsMatch) {

                ASSERT (RecordToExhaust->Callback != NULL);
                PERF_START_EX (NULL, "SmmFunction", NULL, AsmReadTsc(), RecordToExhaust->ProtocolType);
                RecordToExhaust->Callback ((EFI_HANDLE) & RecordToExhaust->Link, &Context);
                PERF_END_EX (NULL, "SmmFunction", NULL, AsmReadTsc(), RecordToExhaust->ProtocolType);
                if (RecordToExhaust->ProtocolType == SxType) {
                  SxChildWasDispatched = TRUE;
                }
              }
            }
          }

          if (RecordInDb->ClearSource == NULL) {
            ///
            /// Clear the SMI associated w/ the source using the default function
            ///
            PchSmmClearSource (&ActiveSource);
          } else {
            ///
            /// This source requires special handling to clear
            ///
            RecordInDb->ClearSource (&ActiveSource);
          }
          ///
          /// Also, try to clear EOS
          ///
          EosSet = PchSmmSetAndCheckEos ();
          ///
          /// Queue is empty, reset the search
          ///
          break;
        }
      }
    }
  }
  ///
  /// If you arrive here, there are two possible reasons:
  /// (1) you've got problems with clearing the SMI status bits in the
  /// ACPI table.  If you don't properly clear the SMI bits, then you won't be able to set the
  /// EOS bit.  If this happens too many times, the loop exits.
  /// (2) there was a SMM communicate for callback messages that was received prior
  /// to this driver.
  /// If there is an asynchronous SMI that occurs while processing the Callback, let
  /// all of the drivers (including this one) have an opportunity to scan for the SMI
  /// and handle it.
  /// If not, we don't want to exit and have the foreground app. clear EOS without letting
  /// these other sources get serviced.
  ///
  /// This assert is not valid with CSM legacy solution because it generates software SMI
  /// to test for legacy USB support presence.
  /// This may not be illegal, so we cannot assert at this time.
  ///
  ///  ASSERT (EscapeCount > 0);
  ///
  if (SxChildWasDispatched) {
    ///
    /// A child of the SmmSxDispatch protocol was dispatched during this call;
    /// put the system to sleep.
    ///
    PchSmmSxGoToSleep ();
  }

  return Status;

}
