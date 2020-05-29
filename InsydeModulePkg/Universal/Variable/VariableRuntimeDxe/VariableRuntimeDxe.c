/** @file
  If platform supports SMM mode, this dirver provides variable services in protected mode and SMM mode
  If platform doesn't support SMM mode, this driver provides variables services in protected mode.

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

#include "Variable.h"
#include "VariableLock.h"

//
// Declare ECP_SMM_VARIABLE_PROTOCOL_GUID for backward compatible
//
#define ECP_SMM_VARIABLE_PROTOCOL_GUID \
  { \
    0x5eb95b0a, 0xd085, 0x4a18, 0x9e, 0x41, 0x8e, 0xbd, 0xa7, 0x9f, 0x30, 0xc8 \
  }
EFI_GUID                            mEcpSmmVariableProtocolGuid = ECP_SMM_VARIABLE_PROTOCOL_GUID;

ESAL_VARIABLE_GLOBAL                *mVariableModuleGlobal;
SMM_VARIABLE_INSTANCE               *mSmmVariableGlobal;
EFI_SMM_SYSTEM_TABLE2               *mSmst;
VOID                                *mStorageArea;
VOID                                *mCertDbList;
EDKII_VARIABLE_LOCK_PROTOCOL        mVariableLock = { VariableLockRequestToLock };

/**
  Check if the GUID is a zero GUID.

  @param[In]  Guid  pointer to the GUID.

  @retval TRUE  The GUID is a zero GUID.
  @retval FALSE The GUID is not a zero GUID.

**/
BOOLEAN
IsZeroGuid (
  IN EFI_GUID                   *Guid
  )
{
  UINT8 *TempPtr;
  UINTN Index;

  TempPtr = (UINT8 *) Guid;

  for (Index = 0; Index < sizeof (EFI_GUID); Index++) {
    if (TempPtr[Index] != 0) {
      return FALSE;
    }
  }

  return TRUE;
}

/**
  Calculate the number of the variable in the table.

  @param[In]  TablePtr  pointer to the table.

  @return UINTN     The number of the variables in the table.

**/
UINTN
GetNumberOfVariable (
  IN UINT8                      *TablePtr
  )
{
  UINTN NumberOfVariable;


  NumberOfVariable = 0;
  while (!IsZeroGuid ((EFI_GUID *) TablePtr)) {
    TablePtr += sizeof (EFI_GUID);
    NumberOfVariable++;
    TablePtr += StrSize ((CHAR16 *) TablePtr);
  }

  //
  // Add one for the end of data
  //
  NumberOfVariable++;

  return NumberOfVariable;
}

/**
  Get the default preserved variable table from the PCD.
  Adjust the structure of the table.

  @param[Out]  TablePtr  pointer to the preserved variable table.

  @retval EFI_OUT_OF_RESOURCES  Allocate pool failed.
  @retval EFI_SUCCESS           Get the default preserved variable table successfully.

**/
EFI_STATUS
GetDefaultTable (
  OUT PRESERVED_VARIABLE_TABLE   **TablePtr
  )
{
  PRESERVED_VARIABLE_TABLE *TempTablePtr;
  UINTN                    NumberOfVariable;
  UINT8                    *DefaultTablePtr;
  UINTN                    Index;
  CHAR16                   *StringPtr;

  StringPtr = NULL;
  TempTablePtr = NULL;
  DefaultTablePtr = PcdGetPtr (PcdDefaultVariableForReclaimFailTable);

  if (!IsZeroGuid ((EFI_GUID *) DefaultTablePtr)) {
    NumberOfVariable = GetNumberOfVariable ((UINT8 *) DefaultTablePtr);
    mVariableModuleGlobal->PreservedTableSize = (UINT32) (NumberOfVariable * sizeof (PRESERVED_VARIABLE_TABLE));
    TempTablePtr = VariableAllocateZeroBuffer (mVariableModuleGlobal->PreservedTableSize, TRUE);
    if (TempTablePtr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    //
    // Adjust the table from PCD to PRESERVED_VARIABLE_TABLE.
    //
    for (Index = 0; (!IsZeroGuid ((EFI_GUID *) DefaultTablePtr)); Index++) {
      CopyMem (&TempTablePtr[Index].VendorGuid, DefaultTablePtr, sizeof (EFI_GUID));

      DefaultTablePtr += sizeof (EFI_GUID);
      StringPtr = VariableAllocateZeroBuffer (StrSize ((CHAR16 *) DefaultTablePtr), TRUE);
      if (StringPtr == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
      CopyMem (StringPtr, DefaultTablePtr, StrSize ((CHAR16 *) DefaultTablePtr));
      TempTablePtr[Index].VariableName = StringPtr;
      //
      // Go to the next variable.
      //
      DefaultTablePtr += StrSize (TempTablePtr[Index].VariableName);
    }
  }

  *TablePtr = TempTablePtr;
  return EFI_SUCCESS;
}

/**
  Get the preserved variable table. This table will be used when the variable region is full.

**/
VOID
GetPreservedVariableTable (
  VOID
  )
{
  EFI_STATUS                    Status;
  PRESERVED_VARIABLE_TABLE      *PreservedTablePtr;
  UINTN                         Index;
  CHAR16                        *StrPtr;


  StrPtr = NULL;
  Status = GetDefaultTable (&PreservedTablePtr);
  if (!EFI_ERROR (Status)) {
    mVariableModuleGlobal->PreservedVariableTable = PreservedTablePtr;
  }

  Status = OemSvcVariableForReclaimFailTable (
             &PreservedTablePtr,
             &mVariableModuleGlobal->PreservedTableSize
             );
  if (!EFI_ERROR (Status)) {
    return;
  }

  if (mVariableModuleGlobal->PreservedTableSize != 0) {
    //
    // Free unused table if we are using the table from the OemService.
    //
    if ((Status == EFI_MEDIA_CHANGED) && (mVariableModuleGlobal->PreservedVariableTable != NULL)) {
      for (Index = 0; mVariableModuleGlobal->PreservedVariableTable[Index].VariableName != NULL; Index++) {
        FreePool (mVariableModuleGlobal->PreservedVariableTable[Index].VariableName);
      }
      FreePool (mVariableModuleGlobal->PreservedVariableTable);
    }

    mVariableModuleGlobal->PreservedVariableTable = VariableAllocateZeroBuffer (mVariableModuleGlobal->PreservedTableSize, TRUE);
    CopyMem (mVariableModuleGlobal->PreservedVariableTable, PreservedTablePtr, mVariableModuleGlobal->PreservedTableSize);

    for (Index = 0; mVariableModuleGlobal->PreservedVariableTable[Index].VariableName != NULL; Index++) {
      StrPtr = VariableAllocateZeroBuffer (StrSize (mVariableModuleGlobal->PreservedVariableTable[Index].VariableName), TRUE);
      CopyMem (
        StrPtr,
        mVariableModuleGlobal->PreservedVariableTable[Index].VariableName,
        StrSize (mVariableModuleGlobal->PreservedVariableTable[Index].VariableName)
        );
      mVariableModuleGlobal->PreservedVariableTable[Index].VariableName = StrPtr;
    }
  }
}

EFI_STATUS
EFIAPI
RuntimeDxeGetVariable (
  IN CHAR16        *VariableName,
  IN EFI_GUID      * VendorGuid,
  OUT UINT32       *Attributes OPTIONAL,
  IN OUT UINTN     *DataSize,
  OUT VOID         *Data
  )
{
  return VariableServicesGetVariable (
           VariableName,
           VendorGuid,
           Attributes,
           DataSize,
           Data
           );
}

EFI_STATUS
EFIAPI
RuntimeDxeGetNextVariableName (
  IN OUT UINTN     *VariableNameSize,
  IN OUT CHAR16    *VariableName,
  IN OUT EFI_GUID  *VendorGuid
  )
{
  return VariableServicesGetNextVariableName (
           VariableNameSize,
           VariableName,
           VendorGuid
           );
}

EFI_STATUS
EFIAPI
RuntimeDxeSetVariable (
  IN CHAR16        *VariableName,
  IN EFI_GUID      *VendorGuid,
  IN UINT32        Attributes,
  IN UINTN         DataSize,
  IN VOID          *Data
  )
{
  return VariableServicesSetVariable (
          VariableName,
          VendorGuid,
          Attributes,
          DataSize,
          Data
          );
}

EFI_STATUS
EFIAPI
RuntimeDxeQueryVariableInfo (
  IN  UINT32                 Attributes,
  OUT UINT64                 *MaximumVariableStorageSize,
  OUT UINT64                 *RemainingVariableStorageSize,
  OUT UINT64                 *MaximumVariableSize
  )
{
  return VariableServicesQueryVariableInfo (
           Attributes,
           MaximumVariableStorageSize,
           RemainingVariableStorageSize,
           MaximumVariableSize
           );
}

EFI_STATUS
EFIAPI
SmmGetVariable (
  IN CHAR16        *VariableName,
  IN EFI_GUID      * VendorGuid,
  OUT UINT32       *Attributes OPTIONAL,
  IN OUT UINTN     *DataSize,
  OUT VOID         *Data
  )
{
  return VariableServicesGetVariable (
           VariableName,
           VendorGuid,
           Attributes,
           DataSize,
           Data
           );
}

EFI_STATUS
EFIAPI
SmmGetNextVariableName (
  IN OUT UINTN     *VariableNameSize,
  IN OUT CHAR16    *VariableName,
  IN OUT EFI_GUID  *VendorGuid
  )
{
  return VariableServicesGetNextVariableName (
           VariableNameSize,
           VariableName,
           VendorGuid
           );
}

EFI_STATUS
EFIAPI
SmmSetVariable (
  IN CHAR16        *VariableName,
  IN EFI_GUID      *VendorGuid,
  IN UINT32        Attributes,
  IN UINTN         DataSize,
  IN VOID          *Data
  )
{
  return VariableServicesSetVariable (
           VariableName,
           VendorGuid,
           Attributes,
           DataSize,
           Data
           );
}

EFI_STATUS
EFIAPI
SmmQueryVariableInfo (
  IN  UINT32                 Attributes,
  OUT UINT64                 *MaximumVariableStorageSize,
  OUT UINT64                 *RemainingVariableStorageSize,
  OUT UINT64                 *MaximumVariableSize
  )
{
  return VariableServicesQueryVariableInfo (
           Attributes,
           MaximumVariableStorageSize,
           RemainingVariableStorageSize,
           MaximumVariableSize
           );
}



/**
  Variable Driver main entry point. The Variable driver places the 4 EFI
  runtime services in the EFI System Table and installs arch protocols
  for variable read and write services being available. It also registers
  a notification function for an EVT_SIGNAL_VIRTUAL_ADDRESS_CHANGE event.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       Variable service successfully initialized.

**/
EFI_STATUS
EFIAPI
VariableServiceInitialize (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_HANDLE                            NewHandle;
  EFI_STATUS                            Status;
  EFI_EVENT                             Event;
  VOID                                  *Registration;
  EFI_HANDLE                            Handle;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;
  EFI_SMM_BASE2_PROTOCOL                 *SmmBase;
  BOOLEAN                               InSmm;
  CRYPTO_SERVICES_PROTOCOL              *CryptoServiceProtocol;
  EFI_SMM_SW_DISPATCH2_PROTOCOL         *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT           SwContext;
  EFI_SMM_VARIABLE_PROTOCOL             *SmmVariable;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  InSmm = FALSE;
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  }

  if (!InSmm) {

    if (!IsRuntimeDriver (ImageHandle)) {
       Status = RelocateImageToRuntimeDriver (ImageHandle);
       ASSERT_EFI_ERROR (Status);
        //
        // We only want to load runtime services code to memory and don't load boot services code to memory,
        // so just return EFI_ALREADY_STARTED if it isn't a runtime driver.
        //
        return EFI_ALREADY_STARTED;
    }
    //
    // Great!!! Now, this is a runtime driver.
    //
    Status = VariableCommonInitialize ();
    ASSERT_EFI_ERROR (Status);
    if (Status == EFI_UNSUPPORTED ) {
      return Status;
    }

    GetPreservedVariableTable ();

    SystemTable->RuntimeServices->GetVariable         = RuntimeDxeGetVariable;
    SystemTable->RuntimeServices->GetNextVariableName = RuntimeDxeGetNextVariableName;
    SystemTable->RuntimeServices->SetVariable         = RuntimeDxeSetVariable;
    SystemTable->RuntimeServices->QueryVariableInfo   = RuntimeDxeQueryVariableInfo;

    //
    // Now install the Variable Runtime Architectural Protocol on a new handle
    //
    NewHandle = NULL;
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &NewHandle,
                    &gEfiVariableArchProtocolGuid,
                    NULL,
                    &gEfiVariableWriteArchProtocolGuid,
                    NULL,
                    NULL
                    );
    ASSERT_EFI_ERROR (Status);

    Status = gBS->LocateProtocol (&gCryptoServicesProtocolGuid, NULL, (VOID **)&CryptoServiceProtocol);
    if (!EFI_ERROR (Status)) {
      CryptoCallback (NULL, NULL);
    } else {
      Event = VariableCreateProtocolNotifyEvent (
                 &gCryptoServicesProtocolGuid,
                 TPL_CALLBACK,
                 CryptoCallback,
                 NULL,
                 &Registration
                 );
    }

    Event = VariableCreateProtocolNotifyEvent (
               &gBootOrderHookEnableGuid,
               TPL_CALLBACK,
               BootOrderHookEnableCallback,
               NULL,
               &Registration
               );
    Event = VariableCreateProtocolNotifyEvent (
               &gBootOrderHookDisableGuid,
               TPL_CALLBACK,
               BootOrderHookDisableCallback,
               NULL,
               &Registration
               );

    //
    // Register the event to convert the pointer for runtime.
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_NOTIFY,
                    VariableAddressChangeEvent,
                    NULL,
                    &gEfiEventVirtualAddressChangeGuid,
                    &Event
                    );
    ASSERT_EFI_ERROR (Status);
    //
    // Note!!!
    // This driver is SMM and RuntimeDxe hybrid driver and UefiRuntimeLib creates ExitBootServices and
    // VariableAddressChange event in constructor, so we cannot use this library to prevent from system
    // calls function located in SMM RAM in protected mode when ExitBootServices event is signaled.
    //
    Status = gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    ExitBootServicesEvent,
                    NULL,
                    &gEfiEventExitBootServicesGuid,
                    &Event
                    );
    ASSERT_EFI_ERROR (Status);

    mVariableModuleGlobal->SecureBootCallbackEnabled = IsAdministerSecureBootSupport () ? TRUE : FALSE;
    Status = gBS->InstallMultipleProtocolInterfaces (
                    &NewHandle,
                    &gEdkiiVariableLockProtocolGuid,
                    &mVariableLock,
                    NULL
                    );
  } else {

    //
    // Get Smm Syatem Table
    //
    Status = SmmBase->GetSmstLocation(
                        SmmBase,
                        &mSmst
                        );
    if (EFI_ERROR (Status)) {
      return Status;
    }
    mVariableModuleGlobal = VariableAllocateZeroBuffer (sizeof (ESAL_VARIABLE_GLOBAL), TRUE);

    if (mVariableModuleGlobal == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    mSmmVariableGlobal = VariableAllocateZeroBuffer (sizeof (SMM_VARIABLE_INSTANCE), TRUE);

    if (mSmmVariableGlobal == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    //
    // Reserved 56K runtime buffer for "Append" operation in virtual mode.
    //
    mStorageArea  = VariableAllocateZeroBuffer (APPEND_BUFF_SIZE, TRUE);
    if (mStorageArea == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    //
    // Reserved 56K runtime buffer for certificated database list (normal time based authenticated variable)
    // operation in virtual mode.
    //
    mCertDbList  = VariableAllocateZeroBuffer (MAX_VARIABLE_SIZE, TRUE);
    if (mCertDbList == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }


    //
    // Start of relocated code (to avoid uninitialized variable access
    // in the set and get variable functions in callbacks triggered by
    // the installation of the protocol)
    //
    Status = SmmNvsInitialize (SmmBase);
    SmmVariable = VariableAllocateZeroBuffer (sizeof (EFI_SMM_VARIABLE_PROTOCOL), TRUE);

    if (SmmVariable == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }

    GetPreservedVariableTable ();

    SmmVariable->SmmGetVariable          = SmmGetVariable;
    SmmVariable->SmmSetVariable          = SmmSetVariable;
    SmmVariable->SmmGetNextVariableName  = SmmGetNextVariableName;
    SmmVariable->SmmQueryVariableInfo    = SmmQueryVariableInfo;

    mSmmVariableGlobal->ProtectedModeVariableModuleGlobal->SmmSetVariable = SmmSetVariable;

    Status = mSmst->SmmLocateProtocol (
                      &gEfiSmmFwBlockServiceProtocolGuid,
                      NULL,
                      (VOID **)&mSmmVariableGlobal->SmmFwbService
                      );
    //
    // Since we still have some EDK SMM drivers, we still need use gEfiSmmRuntimeProtocolGuid for backward
    // compatible
    //
    Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRT);
    if (!EFI_ERROR(Status)) {
      Status = SmmRT->LocateProtocol (
                        &gEfiSmmRuntimeProtocolGuid,
                        NULL,
                        (VOID **)&mSmmVariableGlobal->SmmRT
                        );
      ASSERT_EFI_ERROR (Status);
      SmmRuntimeCallback (NULL, SmmVariable);
    } else {
      Event = VariableCreateProtocolNotifyEvent (
                &gEfiSmmRuntimeProtocolGuid,
                TPL_CALLBACK,
                SmmRuntimeCallback,
                SmmVariable,
                &Registration
                );
    }

    if (mSmmVariableGlobal->SmmFwbService == NULL) {
      Status = mSmst->SmmLocateProtocol (
                        &gEfiSmmFwBlockServiceProtocolGuid,
                        NULL,
                        (VOID **)&mSmmVariableGlobal->SmmFwbService
                        );

    }

    Handle = NULL;
    Status = mSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gEfiSmmVariableProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      SmmVariable
                      );
  //
  // For compatible with native EDKII variable driver, install gEfiSmmVariableProtocolGuid in gBS.
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiSmmVariableProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  SmmVariable
                  );

    Status = mSmst->SmmLocateProtocol (
                      &gCryptoServicesProtocolGuid,
                      NULL,
                      (VOID **)&CryptoServiceProtocol
                      );
    if (!EFI_ERROR (Status)) {
      CryptoCallback (NULL, NULL);
    } else {
      Status = mSmst->SmmRegisterProtocolNotify (
                        &gCryptoServicesProtocolGuid,
                        CryptoServicesInstalled,
                        &Registration
                        );
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }

    Status = mSmst->SmmLocateProtocol (
                      &gEfiSmmSwDispatch2ProtocolGuid,
                      NULL,
                      (VOID **)&SwDispatch
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Locate Smm Cpu protocol for Cpu save state manipulation
    //
    Status = mSmst->SmmLocateProtocol (
                      &gEfiSmmCpuProtocolGuid,
                      NULL,
                      (VOID **)&mSmmVariableGlobal->SmmCpu
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    SwContext.SwSmiInputValue = SECURE_BOOT_SW_SMI;

    if (IsAdministerSecureBootSupport ()) {
      Status = SwDispatch->Register (
                             SwDispatch,
                             SecureBootCallback,
                             &SwContext,
                             &mSmmVariableGlobal->SwHandle
                             );

    } else {
      Status = SwDispatch->Register (
                             SwDispatch,
                             NonSecureBootCallback,
                             &SwContext,
                             &mSmmVariableGlobal->SwHandle
                             );
    }
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Callback for SMM runtime

  @param    Event     Event whose notification function is being invoked.
  @param    Context   Pointer to the notification function's context.

  None

**/
VOID
EFIAPI
SmmRuntimeCallback (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS                      Status;
  EFI_SMM_RUNTIME_PROTOCOL        *SmmRT;
  EFI_HANDLE                      Handle;

  gBS->CloseEvent (Event);
  if (mSmmVariableGlobal->SmmRT == NULL) {
    Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRT);
    if (EFI_ERROR (Status)) {
      return;
    }
    Status = SmmRT->LocateProtocol (
                      &gEfiSmmRuntimeProtocolGuid,
                      NULL,
                      (VOID **)&mSmmVariableGlobal->SmmRT
                      );
    if (EFI_ERROR (Status)) {
      return;
    }
  }
  SmmRT = mSmmVariableGlobal->SmmRT;
  if (mSmmVariableGlobal->SmmFwbService == NULL) {
    SmmRT->LocateProtocol (
             &gEfiSmmFwBlockServiceProtocolGuid,
             NULL,
             (VOID **)&mSmmVariableGlobal->SmmFwbService
             );
  }

  //
  // Since MdePkg defined gEfiSmmVariableProtocolGuid but the GUID and we defined in ECP are different,
  // using SmmRT to use mEcpSmmVariableProtocolGuid insatll SmmVarialbe services for ECP driver
  //
  Handle = NULL;
  Status = SmmRT->InstallProtocolInterface (
                    &Handle,
                    &mEcpSmmVariableProtocolGuid,
                    EFI_NATIVE_INTERFACE,
                    Context
                    );
  ASSERT_EFI_ERROR (Status);
  //
  // Use gBS to install mEcpSmmVariableProtocolGuid for ECP driver dependency compatible
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &mEcpSmmVariableProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);
  return;
}
