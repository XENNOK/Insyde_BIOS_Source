/** @file
  This driver checks whether there is pending TPM request. If yes,
  it will display TPM request information and ask for user confirmation.
  The TPM request will be cleared after it is processed.

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

#include <PiDxe.h>

#include <Protocol/GraphicsOutput.h>
#include <Protocol/TrEEPhysicalPresence.h>
#include <Protocol/VariableLock.h>
//
// Work around for OemFormBrowser2
//
#ifdef MDE_CPU_ARM
typedef UINT16 STRING_REF;
#endif
#include <Protocol/H2ODialog.h>
#include <Protocol/EndOfBdsBootSelection.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PrintLib.h>
#include <Library/HiiLib.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/OemGraphicsLib.h>
#include <Library/DxeOemSvcKernelLib.h>

#include <Guid/EventGroup.h>
#include <Guid/TrEEPhysicalPresenceData.h>
#include <Guid/UsbEnumeration.h>

#include <Base.h>
#include <KernelSetupConfig.h>

#include <TrEEData.h>

#define TPM_PP_SUCCESS              0
#define TPM_PP_USER_ABORT           ((TPM_RESULT)(-0x10))
#define TPM_PP_BIOS_FAILURE         ((TPM_RESULT)(-0x0f))

#define CONFIRM_BUFFER_SIZE         4096

EFI_HII_HANDLE mTrEEPpStringPackHandle = NULL;

BOOLEAN        mLockInEntry = FALSE;

EFI_STATUS
EFIAPI
TrEEExecutePhysicalPresence (
  IN      EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL *This,
  IN      UINT8                               CommandCode,
  OUT     TPM_RESULT                          *LastReturnedCode,
  IN OUT  UINT8                               *PpiFlags OPTIONAL
  );

EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL  mTrEEPhysicalPresenceProtocol = {
  TrEEExecutePhysicalPresence
  };

/**
  Provides lastest platformAuth value.

  @param[out] AuthSize     Pointer to the size of Auth.
  @param[out] Auth         Pointer to a Auth value.

  @retval EFI_SUCCESS            Operation completed successfully.
  @retval EFI_INVALID_PARAMETER  Invalid input.

**/
EFI_STATUS
EFIAPI
GetAuthKey (
  OUT TPM2B_AUTH       *Auth
  )
{
  EFI_STATUS           Status;
  EFI_TREE_PROTOCOL    *TrEEProtocol;
  TREE_DXE_DATA        *Tpm2PrivateData;

  if (Auth == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  Status = gBS->LocateProtocol (&gEfiTrEEProtocolGuid, NULL, (VOID **) &TrEEProtocol);
  if (EFI_ERROR (Status)) {
    //
    // Auth value is default before TrEEProtocol installation
    //
    ZeroMem (Auth, sizeof(TPM2B_AUTH));
    return EFI_SUCCESS;
  }

  Tpm2PrivateData = BASE_CR (TrEEProtocol, TREE_DXE_DATA, TrEEProtocol);
  *Auth = Tpm2PrivateData->Auth;

  return EFI_SUCCESS;
}

/**
  Check whether the request needs the confirmation of user.

  @param[in, out] TrEEPpData  Point to the physical presence NV variable.
  @param[in]      Flags       The physical presence interface flags.
**/
BOOLEAN
IsTpmRequestConfirmed (
  IN EFI_TREE_PHYSICAL_PRESENCE     *TrEEPpData,
  IN UINT8                          Flags  
  )
{

  switch (TrEEPpData->PPRequest) {
  case TREE_PHYSICAL_PRESENCE_NO_ACTION:
    return TRUE;

  case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR:
  case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_2:
  case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_3:
  case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_4:
    if ((Flags & TREE_FLAG_NO_PPI_CLEAR) != 0) {
      return TRUE;
    }
    break;
	
  case TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_FALSE:
    return TRUE;

  case TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_TRUE:
    break;
	
  default:
    DEBUG ((EFI_D_INFO, "[TPM2] Invalid operation request. Request=0x%x\n", TrEEPpData->PPRequest));    
    return TRUE; 
  }
  return FALSE;
}

/**
  Get string by string id from HII Interface.

  @param[in] Id          String ID.

  @retval    CHAR16 *    String from ID.
  @retval    NULL        If error occurs.

**/
CHAR16 *
TrEEPhysicalPresenceGetStringById (
  IN  EFI_STRING_ID                           Id
  )
{
  return HiiGetString (mTrEEPpStringPackHandle, Id, NULL);
}

/**
  Send ClearControl and Clear command to TPM.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_BUFFER_TOO_SMALL  Response data buffer is too small.
  @retval EFI_DEVICE_ERROR      Unexpected device behavior.

**/
EFI_STATUS
EFIAPI
TpmCommandClear (
  IN TPM2B_AUTH             *PlatformAuth  OPTIONAL
  )
{
  EFI_STATUS                Status;
  TPMS_AUTH_COMMAND         *AuthSession;
  TPMS_AUTH_COMMAND         LocalAuthSession;

  if (PlatformAuth == NULL) {
    AuthSession = NULL;
  } else {
    AuthSession = &LocalAuthSession;
    ZeroMem (&LocalAuthSession, sizeof(LocalAuthSession));
    LocalAuthSession.sessionHandle = TPM_RS_PW;
    LocalAuthSession.hmac.size = PlatformAuth->size;
    CopyMem (LocalAuthSession.hmac.buffer, PlatformAuth->buffer, PlatformAuth->size);
  }

  DEBUG ((EFI_D_ERROR, "Tpm2ClearControl ... \n"));
  Status = Tpm2ClearControl (TPM_RH_PLATFORM, AuthSession, NO);
  DEBUG ((EFI_D_ERROR, "Tpm2ClearControl - %r\n", Status));
  if (EFI_ERROR (Status)) {
    goto Done;
  }
  DEBUG ((EFI_D_ERROR, "Tpm2Clear ... \n"));
  Status = Tpm2Clear (TPM_RH_PLATFORM, AuthSession);
  DEBUG ((EFI_D_ERROR, "Tpm2Clear - %r\n", Status));

Done:
  ZeroMem (&LocalAuthSession.hmac, sizeof(LocalAuthSession.hmac));
  return Status;
}

/**
  Execute physical presence operation requested by the OS.

  @param[in]      CommandCode         Physical presence operation value.
  @param[in, out] PpiFlags            The physical presence interface flags.

  @retval TPM_PP_BIOS_FAILURE         Unknown physical presence operation.
  @retval TPM_PP_BIOS_FAILURE         Error occurred during sending command to TPM or
                                      receiving response from TPM.
  @retval Others                      Return code from the TPM device after command execution.
**/
EFI_STATUS
EFIAPI
TrEEExecutePhysicalPresence (
  IN      EFI_TREE_PHYSICAL_PRESENCE_PROTOCOL *This,
  IN      UINT8                               CommandCode,
  OUT     TPM_RESULT                          *LastReturnedCode,
  IN OUT  UINT8                               *PpiFlags OPTIONAL
  )
{
  EFI_STATUS                                  Status;
  TPM_RESULT                                  TpmResult;
  TPM2B_AUTH                                  PlatformAuth;
   
  if (LastReturnedCode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (PpiFlags == NULL && (CommandCode == TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_FALSE
      || CommandCode == TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_TRUE)) {
    return EFI_INVALID_PARAMETER;
  }

  TpmResult = TPM_PP_BIOS_FAILURE;

  switch (CommandCode) {
  case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR:
  case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_2:
  case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_3:
  case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_4:
    Status = GetAuthKey (&PlatformAuth);
    if (!EFI_ERROR (Status)) {
      Status = TpmCommandClear (&PlatformAuth);
    }
    
    if (EFI_ERROR (Status)) {
      TpmResult = TPM_PP_BIOS_FAILURE;
    } else {
      TpmResult = TPM_PP_SUCCESS;
    }
    break;

  case TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_FALSE:
    *PpiFlags &= ~TREE_FLAG_NO_PPI_CLEAR;
    TpmResult =  TPM_PP_SUCCESS;
    break;

  case TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_TRUE:
    *PpiFlags |= TREE_FLAG_NO_PPI_CLEAR;
    TpmResult =  TPM_PP_SUCCESS;
    break;

  default:
    if (CommandCode <= TREE_PHYSICAL_PRESENCE_NO_ACTION_MAX) {
      TpmResult = TPM_PP_SUCCESS;
    } else {
      TpmResult = TPM_PP_BIOS_FAILURE;
    }
    break;
  }

  *LastReturnedCode = TpmResult;
  return EFI_SUCCESS;
}

/**
  Display the confirm text and get user confirmation.

  @param[in] TpmPpCommand  The requested TPM physical presence command.

  @retval  TRUE            The user has confirmed the changes.
  @retval  FALSE           The user doesn't confirm the changes.
**/
BOOLEAN
TrEEUserConfirm (
  IN UINT8                                    TpmPpCommand
  )
{
  CHAR16                                      *ConfirmText;
  CHAR16                                      *TmpStr1;
  CHAR16                                      *TmpStr2;
  UINTN                                       BufSize;
  BOOLEAN                                     CautionKey;
  EFI_HANDLE                                  Handle;
  UINT8                                       *Instance;
  EFI_STATUS                                  Status;
  H2O_DIALOG_PROTOCOL                         *H2ODialog;
  EFI_INPUT_KEY                               Key;

  TmpStr2     = NULL;
  CautionKey  = FALSE;
  BufSize     = CONFIRM_BUFFER_SIZE;
  ConfirmText = AllocateZeroPool (BufSize);
  ASSERT (ConfirmText != NULL);
  if (ConfirmText == NULL) {
    return FALSE;
  }

  mTrEEPpStringPackHandle = HiiAddPackages (
                              &gEfiTrEEPhysicalPresenceGuid,
                              NULL,
                              TrEEPhysicalPresenceDxeStrings,
                              NULL
                              );
  ASSERT (mTrEEPpStringPackHandle != NULL);

  switch (TpmPpCommand) {
  case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR:
  case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_2:
  case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_3:
  case TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_4:
    CautionKey = TRUE;
    TmpStr2 = TrEEPhysicalPresenceGetStringById (STRING_TOKEN (TPM_CLEAR));

    TmpStr1 = TrEEPhysicalPresenceGetStringById (STRING_TOKEN (TPM_HEAD_STR));
    UnicodeSPrint (ConfirmText, BufSize, TmpStr1, TmpStr2);
    FreePool (TmpStr1);

    TmpStr1 = TrEEPhysicalPresenceGetStringById (STRING_TOKEN (TPM_WARNING_CLEAR));
    StrnCat (ConfirmText, TmpStr1, (BufSize / sizeof (CHAR16 *)) - StrLen (ConfirmText) - 1);
    StrnCat (ConfirmText, L" \n\n", (BufSize / sizeof (CHAR16 *)) - StrLen (ConfirmText) - 1);
    FreePool (TmpStr1);

    TmpStr1 = TrEEPhysicalPresenceGetStringById (STRING_TOKEN (TPM_CAUTION_KEY));
    StrnCat (ConfirmText, TmpStr1, (BufSize / sizeof (CHAR16 *)) - StrLen (ConfirmText) - 1);
    FreePool (TmpStr1);
    break;

  case TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_TRUE:
    CautionKey = TRUE;
    TmpStr2 = TrEEPhysicalPresenceGetStringById (STRING_TOKEN (TPM_CLEAR));

    TmpStr1 = TrEEPhysicalPresenceGetStringById (STRING_TOKEN (TPM_PPI_HEAD_STR));
    UnicodeSPrint (ConfirmText, BufSize, TmpStr1, TmpStr2);
    FreePool (TmpStr1);

    TmpStr1 = TrEEPhysicalPresenceGetStringById (STRING_TOKEN (TPM_NOTE_CLEAR));
    StrnCat (ConfirmText, TmpStr1, (BufSize / sizeof (CHAR16 *)) - StrLen (ConfirmText) - 1);
    FreePool (TmpStr1);

    TmpStr1 = TrEEPhysicalPresenceGetStringById (STRING_TOKEN (TPM_WARNING_CLEAR));
    StrnCat (ConfirmText, TmpStr1, (BufSize / sizeof (CHAR16 *)) - StrLen (ConfirmText) - 1);
    StrnCat (ConfirmText, L" \n\n", (BufSize / sizeof (CHAR16 *)) - StrLen (ConfirmText) - 1);
    FreePool (TmpStr1);

    TmpStr1 = TrEEPhysicalPresenceGetStringById (STRING_TOKEN (TPM_CAUTION_KEY));
    StrnCat (ConfirmText, TmpStr1, (BufSize / sizeof (CHAR16 *)) - StrLen (ConfirmText) - 1);
    FreePool (TmpStr1);

    TmpStr1 = TrEEPhysicalPresenceGetStringById (STRING_TOKEN (TPM_NO_PPI_INFO));
    StrnCat (ConfirmText, TmpStr1, (BufSize / sizeof (CHAR16 *)) - StrLen (ConfirmText) - 1);
    FreePool (TmpStr1);
    break;

  default:
    ;
  }

  if (TmpStr2 == NULL) {
    FreePool (ConfirmText);
    return FALSE;
  }

  TmpStr1 = TrEEPhysicalPresenceGetStringById (STRING_TOKEN (TPM_REJECT_KEY));
  UnicodeSPrint (ConfirmText + StrLen (ConfirmText), BufSize, TmpStr1, TmpStr2);
  FreePool (TmpStr1);
  FreePool (TmpStr2);

  TmpStr1 = TrEEPhysicalPresenceGetStringById (STRING_TOKEN (TPM_INQUERY));
  StrnCat (ConfirmText, TmpStr1, StrLen (TmpStr1));
  FreePool (TmpStr1);

  //
  // Trigger USB keyboard connection by protocol notify function if need.
  //
  Status = gBS->LocateProtocol (&gEfiUsbEnumerationGuid, NULL, (VOID **) &Instance);
  if (EFI_ERROR (Status)) {
    Handle = 0;
    gBS->InstallProtocolInterface (
           &Handle,
           &gEfiUsbEnumerationGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  }

  Status = gBS->LocateProtocol (&gH2ODialogProtocolGuid, NULL, (VOID **) &H2ODialog);
  if (EFI_ERROR (Status)) {
    return FALSE;
  }
  DisableQuietBoot ();
  gST->ConOut->ClearScreen (gST->ConOut);
  Status = H2ODialog->ConfirmDialog (0, FALSE, 0, NULL, &Key, ConfirmText);
  gST->ConOut->SetCursorPosition (gST->ConOut, 0, 0);
  FreePool (ConfirmText);

  if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
    return TRUE;
  }

  return FALSE;
}

/**
  Check and execute the requested physical presence command.

  @param[in, out] TrEEPpData  Point to the physical presence NV variable.
  @param[in]      Flags       The physical presence interface flags.

**/
VOID
TrEEExecutePendingTpmRequest (
  IN OUT  EFI_TREE_PHYSICAL_PRESENCE          *TrEEPpData,
  IN      UINT8                               Flags
  )
{
  EFI_STATUS                                  Status;
  UINTN                                       DataSize;
  BOOLEAN                                     RequestConfirmed;
  UINT8                                       NewFlags;

  if (TrEEPpData->PPRequest == TREE_PHYSICAL_PRESENCE_NO_ACTION) {
    return;
  }

  if (TrEEPpData->PPRequest > TREE_PHYSICAL_PRESENCE_NO_ACTION_MAX) {
    //
    // Invalid operation request.
    //
    TrEEPpData->PPResponse    = TPM_PP_BIOS_FAILURE;
    TrEEPpData->LastPPRequest = TrEEPpData->PPRequest;
    TrEEPpData->PPRequest     = TREE_PHYSICAL_PRESENCE_NO_ACTION;
    DataSize                  = sizeof (EFI_TREE_PHYSICAL_PRESENCE);
    Status = gRT->SetVariable (
                    TREE_PHYSICAL_PRESENCE_VARIABLE,
                    &gEfiTrEEPhysicalPresenceGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    DataSize,
                    TrEEPpData
                    );
    return;
  }
  
  if ((TrEEPpData->PPRequest != TREE_PHYSICAL_PRESENCE_NO_ACTION) &&
      (TrEEPpData->PPRequest != TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR) &&
      (TrEEPpData->PPRequest != TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_2) &&
      (TrEEPpData->PPRequest != TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_3) &&
      (TrEEPpData->PPRequest != TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_4) &&
      (TrEEPpData->PPRequest != TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_FALSE) &&
      (TrEEPpData->PPRequest != TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_TRUE)) {
    //
    // Valid operation request, but not supported by TPM2
    //
    TrEEPpData->PPResponse    = TPM_PP_SUCCESS;
    TrEEPpData->LastPPRequest = TrEEPpData->PPRequest;
    TrEEPpData->PPRequest     = TREE_PHYSICAL_PRESENCE_NO_ACTION;
    DataSize                  = sizeof (EFI_TREE_PHYSICAL_PRESENCE);
    Status = gRT->SetVariable (
                    TREE_PHYSICAL_PRESENCE_VARIABLE,
                    &gEfiTrEEPhysicalPresenceGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    DataSize,
                    TrEEPpData
                    );
    return;
  }

  RequestConfirmed = IsTpmRequestConfirmed (TrEEPpData, Flags);
  if (!RequestConfirmed) {
    //
    // Print confirm text and wait for approval.
    //
    Status = OemSvcTpmUserConfirmDialog (TrEEPpData->PPRequest, TPM_DEVICE_2_0, &RequestConfirmed);
    if (EFI_ERROR (Status)) {
      RequestConfirmed = TrEEUserConfirm (TrEEPpData->PPRequest);
    }
  }

  //
  // Execute requested physical presence command
  //
  TrEEPpData->PPResponse = TPM_PP_USER_ABORT;
  NewFlags = Flags;
  if (RequestConfirmed) {
    TrEEExecutePhysicalPresence (&mTrEEPhysicalPresenceProtocol, TrEEPpData->PPRequest, &TrEEPpData->PPResponse, &NewFlags);
  }

  //
  // Save the flags if it is updated.
  //
  if (Flags != NewFlags) {
    Status   = gRT->SetVariable (
                      TREE_PHYSICAL_PRESENCE_FLAGS_VARIABLE,
                      &gEfiTrEEPhysicalPresenceGuid,
                      EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                      sizeof (UINT8),
                      &NewFlags
                      ); 
  }

  //
  // Clear request
  //
  if ((NewFlags & TREE_FLAG_RESET_TRACK) == 0) {
    TrEEPpData->LastPPRequest = TrEEPpData->PPRequest;
    TrEEPpData->PPRequest = TREE_PHYSICAL_PRESENCE_NO_ACTION;
  }

  //
  // Save changes
  //
  DataSize = sizeof (EFI_TREE_PHYSICAL_PRESENCE);
  Status = gRT->SetVariable (
                  TREE_PHYSICAL_PRESENCE_VARIABLE,
                  &gEfiTrEEPhysicalPresenceGuid,
                  EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                  DataSize,
                  TrEEPpData
                  );

  if (!mLockInEntry) {
    //
    // Reset system if variable is not locked
    //
    Print (L"Rebooting system to make TPM settings in effect\n");
    gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);
    ASSERT (FALSE);
  }
}

/**
  Lock PPI flags varaible.

  @param[in]  Event        Event whose notification function is being invoked
  @param[in]  Context      Pointer to the notification function's context

**/
VOID
EFIAPI
LockVariableCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  )
{
  EFI_STATUS                        Status;
  EDKII_VARIABLE_LOCK_PROTOCOL      *VariableLockProtocol;

  Status = gBS->LocateProtocol (&gEdkiiVariableLockProtocolGuid, NULL, (VOID **) &VariableLockProtocol);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // This flags variable controls whether physical presence is required for TPM command. 
  // It should be protected from malicious software. We set it as read-only variable here.
  //
  Status = VariableLockProtocol->RequestToLock (
                                   VariableLockProtocol,
                                   TREE_PHYSICAL_PRESENCE_FLAGS_VARIABLE,
                                   &gEfiTrEEPhysicalPresenceGuid
                                   );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "[TPM2] Error when lock variable %s, Status = %r\n", TREE_PHYSICAL_PRESENCE_FLAGS_VARIABLE, Status));
    ASSERT_EFI_ERROR (Status);
  }
}

/**
  Check and execute the physical presence command requested

  @param[in]  Event        Event whose notification function is being invoked
  @param[in]  Context      Pointer to the notification function's context

**/
VOID
EFIAPI
TrEExecuteTpmRequestCallback (
  IN EFI_EVENT                                Event,
  IN VOID                                     *Context
  )
{
  EFI_STATUS                                  Status;
  UINTN                                       DataSize;
  EFI_TREE_PHYSICAL_PRESENCE                  TrEEPpData;
  UINT8                                       PpiFlags;
  VOID                                        *Interface;

  //
  // Check pending request, if not exist, just return.
  //
  DataSize = sizeof (EFI_TREE_PHYSICAL_PRESENCE);
  Status = gRT->GetVariable (
                  TREE_PHYSICAL_PRESENCE_VARIABLE,
                  &gEfiTrEEPhysicalPresenceGuid,
                  NULL,
                  &DataSize,
                  &TrEEPpData
                  );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "[TPM2] PPRequest=%x\n", TrEEPpData.PPRequest));

  DataSize = sizeof (UINT8);
  Status = gRT->GetVariable (
                  TREE_PHYSICAL_PRESENCE_FLAGS_VARIABLE,
                  &gEfiTrEEPhysicalPresenceGuid,
                  NULL,
                  &DataSize,
                  &PpiFlags
                  );
  ASSERT_EFI_ERROR (Status);
  DEBUG ((EFI_D_INFO, "[TPM2] Flags=%x\n", PpiFlags));

  if (EFI_ERROR (Status)) {
    return;
  }

  if (IsTpmRequestConfirmed (&TrEEPpData, PpiFlags)) {
    //
    // No need to wait connect console, so close the event.
    //
    gBS->CloseEvent (Event);
    mLockInEntry = TRUE;
  } else {
    //
    // User confirm dialog is needed, executes the request when connect console is ready.
    //
    Status = gBS->LocateProtocol (&gEndOfBdsBootSelectionProtocolGuid, NULL, (VOID **) &Interface);
    if (EFI_ERROR (Status)) {
      return;
    }
  }

  //
  // Execute pending TPM request.
  //
  TrEEExecutePendingTpmRequest (&TrEEPpData, PpiFlags);
  DEBUG ((EFI_D_INFO, "[TPM2] PPResponse = %x\n", TrEEPpData.PPResponse));
}

/**
  The driver's entry point.

  @param[in] ImageHandle  The firmware allocated handle for the EFI image.
  @param[in] SystemTable  A pointer to the EFI System Table.

  @retval EFI_SUCCESS     The entry point is executed successfully.
  @retval other           Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
DriverEntry (
  IN EFI_HANDLE                               ImageHandle,
  IN EFI_SYSTEM_TABLE                         *SystemTable
  )
{
  EFI_STATUS                                  Status;
  UINTN                                       DataSize;
  EFI_TREE_PHYSICAL_PRESENCE                  TrEEPpData;
  UINT8                                       PpiFlags;
  VOID                                        *Registration;
  VOID                                        *Interface;

  Status = gBS->LocateProtocol (&gEfiTrEEProtocolGuid, NULL, (VOID **) &Interface);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Initialize physical presence variable.
  //
  DataSize = sizeof (EFI_TREE_PHYSICAL_PRESENCE);
  Status = gRT->GetVariable (
                  TREE_PHYSICAL_PRESENCE_VARIABLE,
                  &gEfiTrEEPhysicalPresenceGuid,
                  NULL,
                  &DataSize,
                  &TrEEPpData
                  );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      ZeroMem ((VOID*)&TrEEPpData, sizeof (TrEEPpData));
      DataSize = sizeof (EFI_TREE_PHYSICAL_PRESENCE);
      Status   = gRT->SetVariable (
                        TREE_PHYSICAL_PRESENCE_VARIABLE,
                        &gEfiTrEEPhysicalPresenceGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        DataSize,
                        &TrEEPpData
                        );
    }
    ASSERT_EFI_ERROR (Status);
  }
  
  //
  // Initialize physical presence flags.
  //
  DataSize = sizeof (UINT8);
  Status = gRT->GetVariable (
                  TREE_PHYSICAL_PRESENCE_FLAGS_VARIABLE,
                  &gEfiTrEEPhysicalPresenceGuid,
                  NULL,
                  &DataSize,
                  &PpiFlags
                  );
  if (EFI_ERROR (Status)) {
    if (Status == EFI_NOT_FOUND) {
      PpiFlags = 0;
      Status   = gRT->SetVariable (
                        TREE_PHYSICAL_PRESENCE_FLAGS_VARIABLE,
                        &gEfiTrEEPhysicalPresenceGuid,
                        EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                        sizeof (UINT8),
                        &PpiFlags
                        );
    }
    ASSERT_EFI_ERROR (Status);
  }
  
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiTrEEPhysicalPresenceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mTrEEPhysicalPresenceProtocol
                  );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "[TPM2] Install TrEEPhysicalPresenceProtocol failed, Status=%r\n", Status));
    return Status;
  }
  
  EfiCreateProtocolNotifyEvent (&gEndOfBdsBootSelectionProtocolGuid, TPL_CALLBACK, TrEExecuteTpmRequestCallback, NULL, (VOID **) &Registration);

  if (mLockInEntry) {
    //
    // Request have been done, lock the variable.
    //
    EfiCreateProtocolNotifyEvent (&gEdkiiVariableLockProtocolGuid, TPL_CALLBACK, LockVariableCallback, NULL, (VOID **) &Registration);
  }

  return Status;
}
