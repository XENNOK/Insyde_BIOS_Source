/** @file
   Implement variable library related functions.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/VariableLib.h>
#include <Library/IrsiRegistrationLib.h>
#include <Library/VariableSupportLib.h>
#include <Library/PcdLib.h>

#include <Guid/EventGroup.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SmmVariable.h>
#include <SmiTable.h>

UINT8
EFIAPI
SmmSensitiveVariableCall (
  IN     UINT8            *InPutBuff,       // rcx
  IN     UINTN            DataSize,         // rdx
  IN     UINT8            SubFunNum,        // r8
  IN     UINT16           SmiPort           // r9
  );


EFI_SMM_VARIABLE_PROTOCOL             *mSmmVariable;
EFI_SMM_SYSTEM_TABLE2                 *mSmst;
BOOLEAN                               mAtRuntime;
EFI_EVENT                             mExitBootServicesEvent;
EFI_RUNTIME_SERVICES                  *mRT;
/**
  Notification function of gEfiEventExitBootServicesGuid.

  This is a notification function registered on gEfiEventExitBootServicesGuid event.

  @param  Event        Event whose notification function is being invoked.
  @param  Context      Pointer to the notification function's context.

**/
STATIC
VOID
EFIAPI
ExitBootServicesEvent (
  IN EFI_EVENT                            Event,
  IN VOID                                 *Context
  )
{
  mAtRuntime = TRUE;
  gBS->CloseEvent (Event);
}

/**
  This function allows the caller to determine if UEFI ExitBootServices() has been called.

  This function returns TRUE after all the EVT_SIGNAL_EXIT_BOOT_SERVICES functions have
  executed as a result of the OS calling ExitBootServices().  Prior to this time FALSE
  is returned. This function is used by runtime code to decide it is legal to access
  services that go away after ExitBootServices().

  @retval  TRUE  The system has finished executing the EVT_SIGNAL_EXIT_BOOT_SERVICES event.
  @retval  FALSE The system has not finished executing the EVT_SIGNAL_EXIT_BOOT_SERVICES event.

**/
STATIC
BOOLEAN
AtRuntime (
  VOID
  )
{
  return mAtRuntime;
}

/**
  According to system mode to allocate pool.
  Allocate BootServicesData pool in protect mode if AtRuntime () returns FALSE.
  Allocate memory from SMM ram if system in SMM mode.
  Return NULL pointer if system is in protected mode and AtRuntime () returns TRUE.

  @param[in] Size    The size of buffer to allocate

  @retval NULL       Buffer unsuccessfully allocated.
  @retval Other      Buffer successfully allocated.
**/
STATIC
VOID *
InternalAllocateZeroBuffer (
  IN UINTN     Size
  )
{
  VOID           *Buffer;
  EFI_STATUS     Status;

  if (mSmst == NULL) {
    if (AtRuntime ()) {
      return NULL;
    }
    Status = gBS->AllocatePool (
                    EfiBootServicesData,
                    Size,
                    (VOID **)&Buffer
                    );
  } else {
    Status = mSmst->SmmAllocatePool (
                      EfiRuntimeServicesData,
                      Size,
                      (VOID **)&Buffer
                      );
  }

  if (!EFI_ERROR (Status)) {
    ZeroMem (Buffer, Size);
  } else {
    Buffer = NULL;
  }
  return Buffer;
}

/**
  Internal function to return the value of a variable in different mode (DXE/SMM).

  @param[in]     VariableName   Name of Variable to be found.
  @param[in]     VendorGuid     Variable vendor GUID.
  @param[out]    Attributes     Attribute value of the variable found.
  @param[in,out] DataSize       Size of Data found. If size is less than the data, this value contains
                                the required size.
  @param[out]    Data           Data pointer.


  @retval EFI_SUCCESS           Find the specified variable.
  @retval EFI_INVALID_PARAMETER Invalid parameter.
  @retval EFI_NOT_FOUND         Not found.
  @retval EFI_BUFFER_TO_SMALL   DataSize is too small for the result.

**/
STATIC
EFI_STATUS
InternalGetVariable (
  IN      CHAR16            *VariableName,
  IN      EFI_GUID          *VendorGuid,
  OUT     UINT32            *Attributes OPTIONAL,
  IN OUT  UINTN             *DataSize,
  OUT     VOID              *Data
  )
{
  if (mSmst != NULL && mSmmVariable == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (mSmmVariable == NULL) {
    return mRT->GetVariable (
                  VariableName,
                  VendorGuid,
                  Attributes,
                  DataSize,
                  Data
                  );
  } else {
    return mSmmVariable->SmmGetVariable (
                           VariableName,
                           VendorGuid,
                           Attributes,
                           DataSize,
                           Data
                           );
  }
}

/**
  Internal function to set variable in different mode (DXE/SMM).

  @param[in] VariableName        Name of Variable.
  @param[in] VendorGuid          Variable vendor GUID.
  @param[in] Attributes          Attribute value of the variabl.
  @param[in] DataSize            Size of variable data by bytes.
  @param[in] Data                Data pointer.

  @return EFI_SUCCESS            Set variable successfully.
  @retval EFI_INVALID_PARAMETER  Invalid parameter.
  @retval EFI_OUT_OF_RESOURCES   Resource not enough to set variable.
  @retval EFI_WRITE_PROTECTED    Variable is read-only.
  @retval EFI_SECURITY_VIOLATION The variable could not be written due to EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS
                                 or EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACESS being set, but the AuthInfo
                                 does NOT pass the validation check carried out by the firmware.
  @retval EFI_NOT_FOUND          The variable trying to be updated or deleted was not found.
**/
STATIC
EFI_STATUS
InternalSetVariable (
  IN CHAR16      *VariableName,
  IN EFI_GUID    *VendorGuid,
  IN UINT32      Attributes,
  IN UINTN       DataSize,
  IN VOID        *Data
  )
{
  if (mSmst != NULL && mSmmVariable == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (mSmmVariable == NULL) {
    return mRT->SetVariable (
                  VariableName,
                  VendorGuid,
                  Attributes,
                  DataSize,
                  Data
                  );
  } else {
    return mSmmVariable->SmmSetVariable (
                           VariableName,
                           VendorGuid,
                           Attributes,
                           DataSize,
                           Data
                           );
  }
}

/**
  Internal function to enumerates the current variable name in different mode (DXE/SMM).

  @param[in, out] VariableNameSize Size of the variable name.
  @param[in, out] VariableName     Pointer to variable name.
  @param[in, out] VendorGuid       Variable Vendor Guid.

  @retval EFI_SUCCESS              Invalid parameter.
  @retval EFI_BUFFER_TOO_SMALL     Find the specified variable.
  @retval EFI_NOT_FOUND            Not found.
**/
STATIC
EFI_STATUS
InternalGetNextVariableName (
  IN OUT  UINTN       *VariableNameSize,
  IN OUT  CHAR16      *VariableName,
  IN OUT  EFI_GUID    *VendorGuid
  )
{
  if (mSmst != NULL && mSmmVariable == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (mSmmVariable == NULL) {
    return mRT->GetNextVariableName (
                  VariableNameSize,
                  VariableName,
                  VendorGuid
                  );
  } else {
    return mSmmVariable->SmmGetNextVariableName (
                           VariableNameSize,
                           VariableName,
                           VendorGuid
                           );
  }
}

/**
  Detect whether the system is in SMM mode.

  @retval TRUE                  System is at SMM mode.
  @retval FALSE                 System is not at SMM mode.
**/
STATIC
BOOLEAN
IsInSmm (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_BASE2_PROTOCOL         *SmmBase;
  BOOLEAN                        InSmm;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **) &SmmBase
                  );
  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  InSmm = FALSE;
  SmmBase->InSmm (SmmBase, &InSmm);
  return InSmm;
}

/**
  The notification of gEfiSmmVariableProtocolGuid protocol is installed

  @param  Protocol              Points to the protocol's unique identifier.
  @param  Interface             Points to the interface instance.
  @param  Handle                The handle on which the interface was installed.

**/
EFI_STATUS
EFIAPI
SmmVariableProtocolInstalled (
  IN     CONST EFI_GUID                *Protocol,
  IN     VOID                          *Interface,
  IN     EFI_HANDLE                    Handle
  )
{
  return mSmst->SmmLocateProtocol (
                  &gEfiSmmVariableProtocolGuid,
                  NULL,
                  (VOID **) &mSmmVariable
                  );
}

STATIC
EFI_STATUS
InitializeSmstAndSmmVariable (
  VOID
  )
{
  EFI_STATUS                     Status;
  EFI_SMM_BASE2_PROTOCOL         *SmmBase;
  VOID                           *Registration;

  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **) &SmmBase
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SmmBase->GetSmstLocation(
                      SmmBase,
                      &mSmst
                      );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = mSmst->SmmLocateProtocol (
                    &gEfiSmmVariableProtocolGuid,
                    NULL,
                    (VOID **)&mSmmVariable
                    );
  if (EFI_ERROR (Status)) {
    Status = mSmst->SmmRegisterProtocolNotify (
                      &gEfiSmmVariableProtocolGuid,
                      SmmVariableProtocolInstalled,
                      &Registration
                      );
  }
  return Status;
}


/**
  The constructor function to check current mode is protected mode or SMM mode.
  If system is in SMM mode, constructor will locate gEfiSmmVariableProtocolGuid
  instance for future usage.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The constructor always returns EFI_SUCCESS.
**/
EFI_STATUS
EFIAPI
DxeVariableLibConstructor (
  IN      EFI_HANDLE                ImageHandle,
  IN      EFI_SYSTEM_TABLE          *SystemTable
  )
{


  if (IsInSmm ()) {
    return InitializeSmstAndSmmVariable ();
  } else {
    mRT = gRT;
    IrsiAddVirtualPointer ((VOID **) &mRT);
    return gBS->CreateEventEx (
                    EVT_NOTIFY_SIGNAL,
                    TPL_CALLBACK,
                    ExitBootServicesEvent,
                    NULL,
                    &gEfiEventExitBootServicesGuid,
                    &mExitBootServicesEvent
                    );
  }


}

/**
  If a runtime driver exits with an error, it must call this routine
  to free the allocated resource before the exiting.
  It will ASSERT() if gBS is NULL.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval  EFI_SUCCESS      The Runtime Driver Lib shutdown successfully.
**/
EFI_STATUS
EFIAPI
DxeVariableLibDestruct (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS  Status;

  ASSERT (gBS != NULL);
  if (mExitBootServicesEvent != NULL) {
    Status = gBS->CloseEvent (mExitBootServicesEvent);
    ASSERT_EFI_ERROR (Status);
  }
  if (mRT != NULL) {
    IrsiRemoveVirtualPointer ((VOID **) &mRT);
  }

  return EFI_SUCCESS;
}

/**
  Read the EFI variable (VariableName/VendorGuid) according to input varialbe data size.

  @param[in]      VariableName       String part of EFI variable name.
  @param[in]      VendorGuid         GUID part of EFI variable name.
  @param[in, out] VariableDataSize   Returns the size of the EFI variable that was read.
  @param[out]     VariableData       Dynamically allocated memory that contains a copy of the EFI variable.
                                     Caller is responsible freeing the buffer.

  @retval EFI_SUCCESS                Get EFI variable Successful.
  @retval EFI_INVALID_PARAMETER      VariableName is NULL or VendorGuid is NULL or VariableDataSize is NULL or VariableData
                                     is NULL.
  @retval EFI_NOT_FOUND              Cannot find specific EFI variable.
  @retval EFI_BUFFER_TOO_SMALL       Input variable data size is too small.  VariableDataSize has been updated with the size
                                     needed to complete the request.
  @return Other                      Other errors cause get variable failed.
**/
EFI_STATUS
CommonGetVariable (
  IN     CHAR16        *VariableName,
  IN     EFI_GUID      *VendorGuid,
  IN OUT UINTN         *VariableDataSize,
  OUT    VOID          *VariableData
  )
{
  return InternalGetVariable (
           VariableName,
           VendorGuid,
           NULL,
           VariableDataSize,
           VariableData
           );
}

/**
  Read the EFI variable (VariableName/VendorGuid) and return a dynamically allocated buffer.

  @param[in]  VariableName  String part of EFI variable name.
  @param[in]  VendorGuid    GUID part of EFI variable name.

  @retval NULL              The variable could not be retrieved.
  @retval NULL              There are not enough resources available for the variable contents.
  @retval Other             A pointer to allocated buffer containing the variable contents.
**/
VOID *
CommonGetVariableData (
  IN CHAR16    *VariableName,
  IN EFI_GUID  *VendorGuid
  )
{
  UINTN       DataSize;
  VOID        *Data;

  Data = NULL;
  CommonGetVariableDataAndSize (VariableName, VendorGuid, &DataSize, &Data);
  return Data;
}

/**
  Read the EFI variable (VariableName/VendorGuid) and return a dynamically allocated
  buffer, and the size of the buffer.

  @param[in]  VariableName       String part of EFI variable name.
  @param[in]  VendorGuid         GUID part of EFI variable name.
  @param[out] VariableDataSize   Returns the size of the EFI variable that was read.
  @param[out] VariableData       Dynamically allocated memory that contains a copy of the EFI variable.
                                 Caller is responsible freeing the buffer.

  @retval EFI_SUCCESS            Get EFI variable Successful.
  @retval EFI_INVALID_PARAMETER  VariableName is NULL or VendorGuid is NULL or VariableDataSize is NULL or VariableData
                                 is NULL.
  @retval EFI_NOT_FOUND          Cannot find specific EFI variable.
  @retval EFI_OUT_OF_RESOURCES   Allocate memory for read variable data failed.
  @return Other                  Other errors cause get variable failed.
**/
EFI_STATUS
CommonGetVariableDataAndSize (
  IN  CHAR16              *VariableName,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableDataSize,
  OUT VOID                **VariableData
  )
{
  EFI_STATUS    Status;
  UINTN         DataSize;
  VOID          *DataPtr;

  if (VariableDataSize == NULL || VariableData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mSmst == NULL && AtRuntime ()) {
    return EFI_OUT_OF_RESOURCES;
  }

  DataSize = 0;
  Status = InternalGetVariable (
             VariableName,
             VendorGuid,
             NULL,
             &DataSize,
             NULL
             );
  if (Status != EFI_BUFFER_TOO_SMALL) {
    return Status;
  }

  DataPtr = InternalAllocateZeroBuffer (DataSize);
  if (DataPtr == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = InternalGetVariable (
            VariableName,
            VendorGuid,
            NULL,
            &DataSize,
            DataPtr
            );
  if (EFI_ERROR (Status)) {
    FreePool (DataPtr);
    return Status;
  }

  *VariableDataSize = DataSize;
  *VariableData     = DataPtr;
  return EFI_SUCCESS;
}

/**
  Read the EFI variable (VariableName/VendorGuid) and return a dynamically allocated
  buffer, and the size of the buffer.

  @param[in]  VariableName             String part of EFI variable name.
  @param[in]  VendorGuid               GUID part of EFI variable name.
  @param[out] VariableDataSize         Returns the size of the EFI variable that was read.
  @param[out] VariableData             Dynamically allocated memory that contains a copy of the EFI variable.
                                       Caller is responsible freeing the buffer.
  @param[in]  DefaultVariableDataSize  Default variable data size.
  @param[in]  DefautlVariableData      Pointer to default variable data.

  @retval TRUE                         The variable exists and the VariableDataSize and VariableData is filled by variable
                                       data size and variable data respectively.
  @retval FALSE                        The variable doesn't exit and the VariableDataSize and VariableData is filled by
                                       default variable data size and default variable data respectively.
**/
BOOLEAN
CommonGetVariableWithDefault (
  IN  CHAR16              *VariableName,
  IN  EFI_GUID            *VendorGuid,
  OUT UINTN               *VariableDataSize,
  OUT VOID                **VariableData,
  IN  UINTN               DefaultVariableDataSize,
  IN  VOID                *DefautlVariableData
  )
{
  EFI_STATUS          Status;

  Status = CommonGetVariableDataAndSize (VariableName, VendorGuid, VariableDataSize, VariableData);
  if (!EFI_ERROR (Status)) {
    return TRUE;
  }

  ASSERT (DefaultVariableDataSize != 0 && DefautlVariableData != NULL);
  *VariableData = InternalAllocateZeroBuffer (DefaultVariableDataSize);
  if (*VariableData == NULL) {
    return FALSE;
  }

  CopyMem (*VariableData, DefautlVariableData, DefaultVariableDataSize);
  *VariableDataSize = DefaultVariableDataSize;
  return FALSE;
}
/**
  Enumerates the current variable names.

  @param[in,out]  VariableNameSize    The size of the VariableName buffer.
  @param[in,out]  VariableName        On input, supplies the last VariableName that was returned by GetNextVariableName().
                                      On output, returns the Null-terminated string of the current variable.
  @param[in,out]  VendorGuid          On input, supplies the last VendorGuid that was returned by GetNextVariableName().
                                      On output, returns the VendorGuid of the current variable.

  @retval EFI_SUCCESS                 The function completed successfully.
  @retval EFI_NOT_FOUND               The next variable was not found.
  @retval EFI_BUFFER_TOO_SMALL        The VariableNameSize is too small for the result. VariableNameSize has been updated
                                      with the size needed to complete the request.
  @retval EFI_INVALID_PARAMETER       VariableNameSize is NULL or VariableName is NULL or VendorGuid is NULL.
  @retval EFI_DEVICE_ERROR            The variable name could not be retrieved due to a hardware error.
**/
EFI_STATUS
CommonGetNextVariableName (
  IN OUT UINTN       *VariableNameSize,
  IN OUT CHAR16      *VariableName,
  IN OUT EFI_GUID    *VendorGuid
  )
{
  return InternalGetNextVariableName (
           VariableNameSize,
           VariableName,
           VendorGuid
           );
}

/**
  Sets the value of a variable.

  Since variable PPI only provide read-only services, this function always returns
  EFI_UNSUPPORTED in PEI phase.

  @param[in] VariableName         String part of EFI variable name.
  @param[in] VendorGuid           GUID part of EFI variable name.
  @param[in] Attributes           Attributes bitmask to set for the variable
  @param[in] DataSize             The size in bytes of the Data buffer.
  @param[in] Data                 The contents for the variable.

  @retval EFI_SUCCESS                 The firmware has successfully stored the variable and its data as defined by
                                      the Attributes.
  @retval EFI_INVALID_PARAMETER       An invalid combination of attribute bits, name, and GUID  was supplied, or the
                                      DataSize exceeds the maximum allowed or name is a empty string.
  @retval EFI_OUT_OF_RESOURCES        Not enough storage is available to hold the variable and its data.
  @retval EFI_DEVICE_ERROR            The variable name could not be retrieved due to a hardware error.
  @retval EFI_WRITE_PROTECTED         The variable in question is read-only or the variable cannot be deleted.
  @retval EFI_SECURITY_VIOLATION      The variable could not be written due to EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS or
                                      EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACESS being set, but the AuthInfo does
                                      NOT pass the validation check carried out by the firmware.
**/
EFI_STATUS
CommonSetVariable (
  IN CHAR16      *VariableName,
  IN EFI_GUID    *VendorGuid,
  IN UINT32      Attributes,
  IN UINTN       DataSize,
  IN VOID        *Data
  )
{
  return InternalSetVariable (
           VariableName,
           VendorGuid,
           Attributes,
           DataSize,
           Data
           );
}

/**
  Set changed sensitive setup data to sensitive setup variable.

  @param[in]  VariableName       A pointer to a null-terminated string that is the variable's name.
  @param[in]  VendorGuid         A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                 VendorGuid and VariableName must be unique.
  @param[in]  Attributes         Attributes bitmask to set for the variable.
  @param[out] DataSize           The data size by byte of variable
  @param[out] Data               Pointer to the start address of data.

  @retval EFI_SUCCESS            Set variable from sensitive variable successfully.
  @retval EFI_INVALID_PARAMETER  Any input parameter is incorrect.
  @retval EFI_NOT_FOUND          Cannot set specific variable in sensitive setup variable.
**/
STATIC
EFI_STATUS
CreateSensitiveAuthenData (
  IN  UINTN                                 InputSize,
  IN  UINT8                                 *InputBuffer,
  OUT SENSITIVE_VARIABLE_AUTHENTICATION     **OutputBuffer
  )
{
  UINTN                              DataSize;
  SENSITIVE_VARIABLE_AUTHENTICATION   *Data;

  if (InputBuffer == NULL || InputSize == 0 || OutputBuffer == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  DataSize = sizeof (SENSITIVE_VARIABLE_AUTHENTICATION) + InputSize;
  Data = AllocatePool (DataSize);
  if (Data == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  Data->Signature = SET_SENSITIVE_VARIABLE_SIGNATURE;
  Data->DataSize  = (UINT32) DataSize;
  Data->Status    = EFI_UNSUPPORTED;
  CopyMem (Data + 1, InputBuffer, InputSize);

  *OutputBuffer = Data;

  return EFI_SUCCESS;
}

/**
  Set SENSITIVE_SETUP_VARIABLE_NAME variable to variable store through SMI.

  @param[out] DataSize           The data size by byte of found variable
  @param[out] Data               Pointer to the start address of found data.

  @retval EFI_SUCCESS            Update SENSITIVE_SETUP_VARIABLE_NAME variable successfully.
  @retval EFI_INVALID_PARAMETER  Any input parameter is incorrect.
  @retval Other                  Any other error occurred while updating SENSITIVE_SETUP_VARIABLE_NAME variable.
**/
EFI_STATUS
UpdateSetupSensitiveVariableThroughSmi (
  IN   UINTN              DataSize,
  IN   VOID               *Data
  )
{
  SENSITIVE_VARIABLE_AUTHENTICATION  *WriteData;
  EFI_STATUS                         Status;

  WriteData = NULL;
  Status    = CreateSensitiveAuthenData (DataSize, Data, &WriteData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = SmmSensitiveVariableCall ((UINT8 *) WriteData, (UINTN) WriteData->DataSize, SET_SENSITIVE_VARIABLE_FUN_NUM, SW_SMI_PORT);
  Status = WriteData->Status;
  FreePool (WriteData);
  return Status;
}

/**
  Set changed sensitive setup data to sensitive setup variable.

  @param[in]  VariableName       A pointer to a null-terminated string that is the variable's name.
  @param[in]  VendorGuid         A pointer to an EFI_GUID that is the variable's GUID. The combination of
                                 VendorGuid and VariableName must be unique.
  @param[in]  Attributes         Attributes bitmask to set for the variable.
  @param[out] DataSize           The data size by byte of found variable
  @param[out] Data               Pointer to the start address of found data.

  @retval EFI_SUCCESS            Find variable from sensitive variable successfully.
  @retval EFI_INVALID_PARAMETER  Any input parameter is incorrect.
  @retval EFI_UNSUPPORTED        The data format to set sensitive variable is incorrect.
  @retval EFI_WRITE_PROTECTED    The service to set variable through SMI is disabled.
**/
EFI_STATUS
SetVariableToSensitiveVariable (
  IN   CONST   CHAR16                           *VariableName,
  IN   CONST   EFI_GUID                         *VendorGuid,
  IN           UINT32                           Attributes,
  IN           UINTN                            DataSize,
  IN           VOID                             *Data
  )
{

  SENSITIVE_VARIABLE_STORE_HEADER *CurrentStoreHeader;
  SENSITIVE_VARIABLE_HEADER       *CurrentVariable;
  EFI_STATUS                      Status;
  UINTN                           BufferSize;


  if (AtRuntime () || mSmst != NULL) {
    return EFI_UNSUPPORTED;
  }

  if (VariableName == NULL || VendorGuid == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if ((Attributes & EFI_VARIABLE_APPEND_WRITE) != 0) {
    return EFI_INVALID_PARAMETER;
  }
  if (DataSize != 0 && Data == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  BufferSize = sizeof (SENSITIVE_VARIABLE_STORE_HEADER) + sizeof (SENSITIVE_VARIABLE_HEADER) + StrSize (VariableName) + DataSize;

  CurrentStoreHeader = AllocatePool (BufferSize);
  if (CurrentStoreHeader == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CurrentStoreHeader->VariableCount = 1;
  CurrentStoreHeader->HeaderSize = sizeof (SENSITIVE_VARIABLE_STORE_HEADER);
  CurrentVariable = (SENSITIVE_VARIABLE_HEADER *) (CurrentStoreHeader + 1);

  CurrentVariable->Attributes = Attributes;
  CurrentVariable->DataSize   = (UINT32) DataSize;
  CopyGuid (&CurrentVariable->VendorGuid, VendorGuid);
  CopyMem (CurrentVariable + 1, VariableName, StrSize (VariableName));
  CopyMem (GetSensitiveVariableDataPtr (CurrentVariable), Data, DataSize);
  Status = UpdateSetupSensitiveVariableThroughSmi (BufferSize, (UINT8 *) CurrentStoreHeader);

  FreePool (CurrentStoreHeader);

  return Status;
}