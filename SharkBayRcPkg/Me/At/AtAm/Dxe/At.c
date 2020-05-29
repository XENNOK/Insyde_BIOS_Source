/** @file

  Implementation for the AT driver.
  This driver implements the AT protocol.

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

#include "At.h"

AT_INSTANCE  mAtInstance = {
  AT_PRIVATE_DATA_SIGNATURE,
  NULL,
  {
    (UINT8) AT_PROTOCOL_REVISION,
    (EFI_AT_AUTHETICATE_CREDENTIAL) AuthenticateCredential,
    (EFI_AT_COMPUTE_HASH) ComputeHash,
    (EFI_AT_GET_NONCE) GetNonce,
    (EFI_AT_GET_TIMER_INFO) GetTimerInfo,
    (EFI_AT_GET_RECOVERY_STRING) GetRecoveryString,
    (EFI_AT_GET_ISV_ID) GetIsvId,
    (EFI_AT_SEND_ASSERT_STOLEN) SendAssertStolen,
    (EFI_AT_SET_SUSPEND_STATE) SetSuspendState,
    (EFI_AT_INIT_WWAN_RECOV) InitWWANREcov,
    (EFI_AT_GET_WWAN_NIC_STATUS) GetWWANNicStatus,
    (EFI_AT_GET_STATE_UNSIGNED) GetStateUnsigned

  }
};

EFI_HECI_PROTOCOL  *mHeci = NULL;

/**
  Entry point for the AT Driver.

  @param[in] ImageHandle          Image handle of this driver.
  @param[in] SystemTable          Global system service table.

  @retval EFI_SUCCESS             Initialization complete.
  @exception EFI_UNSUPPORTED      The chipset is unsupported by this driver.
  @retval EFI_OUT_OF_RESOURCES    Do not have enough resources to initialize the driver.
  @retval EFI_DEVICE_ERROR        Device error, driver exits abnormally.
**/
EFI_STATUS
EFIAPI
AtEntryPoint (
  IN EFI_HANDLE                   ImageHandle,
  IN EFI_SYSTEM_TABLE             *SystemTable
  )
{
  EFI_STATUS  Status;

  DEBUG ((EFI_D_INFO, "ATAM: AtEntryPoint()\n"));
  ///
  /// Install the EFI_AT_PROTOCOL interface
  ///
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &(mAtInstance.Handle),
                  &gEfiAtProtocolGuid,
                  &(mAtInstance.AtProtocol),
                  NULL
                  );

  return Status;
}

/**
  This function sends a request to ME AT Services to validate AT
  recovery credentials. The user input is captured in UTF-16
  format and then passed to this funtion. This function converts
  the User recovery password into a HASH by using Salt & Nonce
  and then send the password HASH to ME AT Services for
  validation. ME AT Service compares the Password HASH and
  returns either pass or fail.

  @param[in] This                 The address of protocol
  @param[in] PassPhrase           Passphrase that needs to be authenticated sent to ME
  @param[in] PassType             Password type user or server generated
  @param[in][out] IsAuthenticated The return of the password match 1 for success and 0 for fail

  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures
  @retval EFI_SUCCESS             The function completed successfully
**/
EFI_STATUS
EFIAPI
AuthenticateCredential (
  IN     EFI_AT_PROTOCOL            *This,
  IN     UINT8                      *PassPhrase,
  IN     UINT32                     *PassType,
  IN OUT UINT8                      *IsAuthenticated
  )
{
  UINT32                            HeciLength;
  EFI_STATUS                        Status;
  EFI_HECI_PROTOCOL                 *mHeci;
  ATHI_AUTHENTICATE_CREDENTIAL_CMD  *AtAuthCmd;
  ATHI_AUTHENTICATE_CREDENTIAL_RSP  AtAuthRsp;

  mHeci = NULL;

  AtAuthCmd = AllocateZeroPool (sizeof (ATHI_AUTHENTICATE_CREDENTIAL_CMD) + AT_PASSWORD_LENGTH);
  if (AtAuthCmd == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem ((VOID *) &AtAuthRsp, sizeof (ATHI_AUTHENTICATE_CREDENTIAL_RSP));

  DEBUG ((EFI_D_ERROR, "AT::AutheticateCrdential Password Type:  %x\n", *PassType));

  AtAuthCmd->Credential.Type            = *PassType;
  AtAuthCmd->Credential.Length          = (UINT32) AT_USR_PASS_HASH_LENGTH_MAX;
  AtAuthCmd->Header.Version.Minor       = ATHI_PROTOCOL_VERSION_MINOR;
  AtAuthCmd->Header.Version.Major       = ATHI_PROTOCOL_VERSION_MAJOR;
  AtAuthCmd->Header.Command.Category    = ATHI_CMD_GROUP_RECOVERY;
  AtAuthCmd->Header.Command.IsResponse  = AT_COMMAND;
  AtAuthCmd->Header.Command.Code        = ATHI_RECOVERY_GRP_AUTH_CREDENTIAL_CMD;
  AtAuthCmd->Header.Length              = sizeof (AT_CREDENTIAL) + AT_USR_PASS_HASH_LENGTH_MAX - sizeof (UINT8);

  HeciLength = sizeof (ATHI_AUTHENTICATE_CREDENTIAL_CMD) + AT_USR_PASS_HASH_LENGTH_MAX - sizeof (UINT8);

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR, "AT::AutheticateCrdential AtAuthCmd Body 1: \n"));
  DEBUG_CODE (
    ShowBuffer ((UINT8 *) AtAuthCmd, HeciLength);
  );
#endif

  CopyMem (&AtAuthCmd->Credential.Value, PassPhrase, AT_USR_PASS_HASH_LENGTH_MAX);

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR, "AT::AutheticateCrdential AtAuthCmd Body 2: \n"));
  DEBUG_CODE (
    ShowBuffer ((UINT8 *) AtAuthCmd, HeciLength);
  );
  DEBUG ((EFI_D_ERROR, "AT::AutheticateCrdential PassPhrase Body 3: \n"));
  DEBUG_CODE (
    ShowBuffer ((UINT8 *) PassPhrase, AtAuthCmd->Credential.Length);
  );
#endif
  if (mHeci == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiHeciProtocolGuid,
                    NULL,
                    (VOID **) &mHeci
                    );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "AutheticateCrdential: Locating for HECI Driver Failed!, Status = %r\n", Status));
      FreePool (AtAuthCmd);
      return Status;
    }
  }

  DEBUG ((EFI_D_ERROR, "AT::AutheticateCrdential PassPhrase Length:  %x\n", AtAuthCmd->Credential.Length));
  DEBUG ((EFI_D_ERROR, "AT::AutheticateCrdential AtAuthCmd HeciLength:  %x\n", HeciLength));
#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR, "AT::AutheticateCrdential AtAuthCmd Body 3: \n"));
  DEBUG_CODE (
    ShowBuffer ((UINT8 *) AtAuthCmd, HeciLength);
  );
#endif

  Status = mHeci->SendMsg (
                  (UINT32 *) AtAuthCmd,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_AT_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AutheticateCrdential failed to send message over HECI!(SendMsg), Status = %r\n", Status));
    FreePool (AtAuthCmd);
    return Status;
  }

  FreePool (AtAuthCmd);

  HeciLength = sizeof (ATHI_AUTHENTICATE_CREDENTIAL_RSP);
  Status = mHeci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &AtAuthRsp,
                  &HeciLength
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AutheticateCrdential failed to receive message over HECI!(ReadMsg), Status = %r\n", Status));
    return Status;
  }

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR, "AT::AutheticateCrdential AtAuthCmd Body 4:  %x\n", AtAuthCmd));
  DEBUG_CODE (
    ShowBuffer ((UINT8 *) AtAuthCmd, HeciLength);
  );
#endif
  DEBUG ((EFI_D_ERROR, "AT::AutheticateCrdential AtAuthRsp HeciLength:  %x\n", HeciLength));
  DEBUG ((EFI_D_ERROR, "AT::AutheticateCrdential AtAuthRsp.CompletionCode:  %x\n", AtAuthRsp.CompletionCode));
  DEBUG ((EFI_D_ERROR,
          "AT::AutheticateCrdential AtAuthRsp.Header.Command.IsResponse:  %x\n", AtAuthRsp.Header.Command.
          IsResponse));
  DEBUG ((EFI_D_ERROR, "checkRecoveryPassword AtAuthRsp.Authenticated:  %d\n", AtAuthRsp.Authenticated));

  ///
  /// Assuming 0 is for success
  ///
  *IsAuthenticated = AtAuthRsp.Authenticated;

  return EFI_SUCCESS;
}

/**
  This API compute the SHA1 hash of the user enterted password

  @param[in] This                 The address of protocol
  @param[in] PassPhrase           The passphrase for which SHA1 hash to be computed
  @param[in][out] Hash            The return value of the SHA1 hash

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
ComputeHash (
  IN     EFI_AT_PROTOCOL          *This,
  IN     UINT8                    *PassPhrase,
  IN OUT UINT8                    *Hash
  )
{

  UINT32                  HeciLength;
  EFI_STATUS              Status;

  ATHI_COMPUTE_HASH_CMD   AtComputeHashCmd;
  ATHI_COMPUTE_HASH_RSP   AtComputeHashRsp;

  ZeroMem ((VOID *) &AtComputeHashCmd, sizeof (ATHI_COMPUTE_HASH_CMD));
  ZeroMem ((VOID *) &AtComputeHashRsp, sizeof (ATHI_COMPUTE_HASH_RSP));

  DEBUG ((EFI_D_ERROR, "AT::AtComputeHashRsp DEBUG in ComputHash\n"));

  AtComputeHashCmd.Header.Version.Minor       = ATHI_PROTOCOL_VERSION_MINOR;
  AtComputeHashCmd.Header.Version.Major       = ATHI_PROTOCOL_VERSION_MAJOR;
  AtComputeHashCmd.Header.Command.Category    = ATHI_CMD_GROUP_RECOVERY;
  AtComputeHashCmd.Header.Command.IsResponse  = AT_COMMAND;
  AtComputeHashCmd.Header.Command.Code        = ATHI_RECOVERY_GRP_COMPUTE_HASH_CMD;

  AtComputeHashCmd.Algorithm                  = AT_HASH_ALGO_ID_SHA1;
  AtComputeHashCmd.InputLength                = (UINT8) AsciiStrLen ((CHAR8 *) PassPhrase);

  ///
  /// Check for length
  ///
  ///
  /// 0- Length 0 only header with command is send as message
  ///
  AtComputeHashCmd.Header.Length = sizeof (ATHI_COMPUTE_HASH_CMD) -
    48 -
    sizeof (ATHI_HEADER) +
    AtComputeHashCmd.InputLength;

  CopyMem (&AtComputeHashCmd.InputBuffer, PassPhrase, AtComputeHashCmd.InputLength);
  HeciLength = sizeof (ATHI_COMPUTE_HASH_CMD) - AT_MAX_HASH_OUTPUT_SIZE + AtComputeHashCmd.InputLength;

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR, "AT::AtComputeHashCmd message length = %x\n", HeciLength));
  DEBUG_CODE (
    ShowBuffer ((UINT8 *) &AtComputeHashCmd, HeciLength);
  );
  DEBUG ((EFI_D_ERROR, "AT::Look for UINT8Pass\n"));
  DEBUG_CODE (
    ShowBuffer ((UINT8 *) PassPhrase, AtComputeHashCmd.InputLength);
  );
#endif

  if (mHeci == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiHeciProtocolGuid,
                    NULL,
                    (VOID **) &mHeci
                    );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "AtComputeHash: Locating for HECI Driver Failed!, Status = %r\n", Status));
      return Status;
    }
  }

  Status = mHeci->SendMsg (
                  (UINT32 *) &AtComputeHashCmd,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_AT_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AtComputeHashCmd failed to send message over HECI!(SendMsg), Status = %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (ATHI_COMPUTE_HASH_RSP);
  Status = mHeci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &AtComputeHashRsp,
                  &HeciLength
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AtComputeHashRsp failed to receive message over HECI!(ReadMsg), Status = %r\n", Status));
    return Status;
  }

  DEBUG ((EFI_D_ERROR, "AT::AtComputeHashRsp response message length %x\n", HeciLength));
  DEBUG ((EFI_D_ERROR, "AtComputeHashRsp.CompletionCode = %x\n", AtComputeHashRsp.CompletionCode));
  DEBUG ((EFI_D_ERROR, "AtComputeHashRsp.OutputLength = %x\n", AtComputeHashRsp.OutputLength));

  if (AtComputeHashRsp.OutputLength != 0) {
    DEBUG ((EFI_D_ERROR, "AtComputeHashRsp.OutputLength = %x\n", AtComputeHashRsp.OutputLength));
  }

  CopyMem (Hash, &AtComputeHashRsp.OutputBuffer, AtComputeHashRsp.OutputLength);

  return EFI_SUCCESS;
}

/**
  This API get the AT Unlock Timer values

  @param[in] This                 The address of protocol
  @param[in] Interval             The return value of the Unlock Time Interval that was set by AT Server
  @param[in] TimeLeft             The Timeleft in the Unlock Timer

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetTimerInfo (
  IN     EFI_AT_PROTOCOL          *This,
  IN OUT UINT32                   *Interval,
  IN OUT UINT32                   *TimeLeft
  )
{

  UINT32                    HeciLength;
  EFI_STATUS                Status;

  ATHI_GET_TIMER_INFO_CMD   AtGetTimerInfoCmd;
  ATHI_GET_TIMER_INFO_RSP   AtGetTimerInfoRsp;

  ZeroMem ((VOID *) &AtGetTimerInfoCmd, sizeof (ATHI_GET_TIMER_INFO_CMD));
  ZeroMem ((VOID *) &AtGetTimerInfoRsp, sizeof (ATHI_GET_TIMER_INFO_RSP));

  AtGetTimerInfoCmd.Header.Version.Minor        = ATHI_PROTOCOL_VERSION_MINOR;
  AtGetTimerInfoCmd.Header.Version.Major        = ATHI_PROTOCOL_VERSION_MAJOR;
  AtGetTimerInfoCmd.Header.Command.Category     = ATHI_CMD_GROUP_THEFT_DETECTION;
  AtGetTimerInfoCmd.Header.Command.IsResponse   = AT_COMMAND;
  AtGetTimerInfoCmd.Header.Command.Code         = ATHI_THEFT_DETECT_GRP_GET_TIMER_INFO_CMD;
  //
  // 0- Length 0 only header with command is send as message
  //
  AtGetTimerInfoCmd.Header.Length  = sizeof (ATHI_GET_TIMER_INFO_CMD) - sizeof (ATHI_HEADER);
  AtGetTimerInfoCmd.TimerId        = AT_TID_UNLOCK_TIMER;

  if (mHeci == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiHeciProtocolGuid,
                    NULL,
                    (VOID **) &mHeci
                    );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "AtGetTimerInfo: Locating for HECI Driver Failed!, Status = %r\n", Status));
      return Status;
    }
  }

  HeciLength = sizeof (ATHI_GET_TIMER_INFO_CMD);

  Status = mHeci->SendMsg (
                  (UINT32 *) &AtGetTimerInfoCmd,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_AT_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AtGetTimerInfo failed to send message over HECI!(SendMsg), Status = %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (ATHI_GET_TIMER_INFO_RSP);

  DEBUG ((EFI_D_ERROR, "AT::AtGetTimerInfo response message length  = %x\n", HeciLength));

  Status = mHeci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &AtGetTimerInfoRsp,
                  &HeciLength
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AtGetTimerInfoRsp failed to receive message over HECI!(ReadMsg), Status = %r\n", Status));
    return Status;
  }

  DEBUG ((EFI_D_ERROR, "AT::AtGetTimerInfo response message length = %x\n", HeciLength));
  DEBUG ((EFI_D_ERROR, "AtGetTimerInfoRsp.CompletionCode = %x\n", AtGetTimerInfoRsp.CompletionCode));
  DEBUG ((EFI_D_ERROR, "AtGetTimerInfoRsp.TimerInfo.Interval = %x\n", AtGetTimerInfoRsp.TimerInfo.Interval));
  DEBUG ((EFI_D_ERROR, "AtGetTimerInfoRsp.TimerInfo.TimeLeft = %x\n", AtGetTimerInfoRsp.TimerInfo.TimeLeft));

  *Interval = AtGetTimerInfoRsp.TimerInfo.Interval;
  *TimeLeft = AtGetTimerInfoRsp.TimerInfo.TimeLeft;

  return EFI_SUCCESS;

}

/**
  This gets the ME nonce
  @param[in] This                 The address of protocol
  @param[in][out] Nonce           The return value of the 16 Byte nonce received from ME

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetNonce (
  IN     EFI_AT_PROTOCOL          *This,
  IN OUT UINT8                    *Nonce
  )
{

  UINT32              HeciLength;
  EFI_STATUS          Status;

  ATHI_GET_NONCE_CMD  AtGetNonceCmd;
  ATHI_GET_NONCE_RSP  AtGetNonceRsp;

  ZeroMem ((VOID *) &AtGetNonceCmd, sizeof (ATHI_GET_NONCE_CMD));
  ZeroMem ((VOID *) &AtGetNonceRsp, sizeof (ATHI_GET_NONCE_RSP));

  AtGetNonceCmd.Version.Minor       = ATHI_PROTOCOL_VERSION_MINOR;
  AtGetNonceCmd.Version.Major       = ATHI_PROTOCOL_VERSION_MAJOR;
  AtGetNonceCmd.Command.Category    = ATHI_CMD_GROUP_GENERAL;
  AtGetNonceCmd.Command.IsResponse  = AT_COMMAND;
  AtGetNonceCmd.Command.Code        = ATHI_GENERAL_GRP_GET_NONCE_CMD;

  ///
  /// 0- Length 0 only header with command is send as message
  ///
  AtGetNonceCmd.Length       = sizeof (ATHI_GET_STATE_CMD) - sizeof (ATHI_HEADER);

  if (mHeci == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiHeciProtocolGuid,
                    NULL,
                    (VOID **) &mHeci
                    );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "AtGetNonce: Locating for HECI Driver Failed!, Status = %r\n", Status));
      return Status;
    }
  }

  HeciLength = sizeof (ATHI_GET_NONCE_CMD);

  Status = mHeci->SendMsg (
                  (UINT32 *) &AtGetNonceCmd,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_AT_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AtGetNonce failed to send message over HECI!(SendMsg), Status = %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (ATHI_GET_NONCE_RSP);
  Status = mHeci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &AtGetNonceRsp,
                  &HeciLength
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AtGetNonce failed to receive message over HECI!(ReadMsg), Status = %r\n", Status));
    return Status;
  }

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR, "AT::AtGetNonce response message length %x\n", HeciLength));
  DEBUG ((EFI_D_ERROR, "AtGetNonceRsp.CompletionCode = %x\n", AtGetNonceRsp.CompletionCode));
  DEBUG ((EFI_D_ERROR, "AtGetNonceRsp.Nonce = %x\n", AtGetNonceRsp.Nonce));
  DEBUG_CODE (
    ShowBuffer ((UINT8 *) &AtGetNonceRsp.Nonce, AT_NONCE_LENGTH);
  );
#endif

  if (AtGetNonceRsp.CompletionCode == ATHI_COMPCODE_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "AtGetNonceRsp.CompletionCode = %x\n", AtGetNonceRsp.CompletionCode));
  }

  CopyMem (Nonce, &AtGetNonceRsp.Nonce, AT_NONCE_LENGTH);

  return EFI_SUCCESS;

}

/**
  This retrives the ISV String stored by AT Server that BIOS will display during Platform lock state

  @param[in] This                 The address of protocol
  @param[in] StringId             The String buffer ID to retrive the ISV String
  @param[out] IsvString           256 Bytes of ISV string array, the
  @param[out] IsvStringLength     The String length

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetRecoveryString (
  IN  EFI_AT_PROTOCOL             *This,
  IN  UINT32                      *StringId,
  OUT UINT8                       *IsvString,
  OUT UINT32                      *IsvStringLength
  )
{

  UINT32                          HeciLength;
  EFI_STATUS                      Status;
  ATHI_GET_VENDOR_STRING_CMD      AtIsvStringCmd;
  ATHI_GET_VENDOR_STRING_RSP      AtIsvStringRsp;

  ZeroMem ((VOID *) &AtIsvStringCmd, sizeof (ATHI_GET_VENDOR_STRING_CMD));
  ZeroMem ((VOID *) &AtIsvStringRsp, sizeof (ATHI_GET_VENDOR_STRING_RSP));
  ///
  /// Setting the length of IsvString to 0 here.
  //
  *IsvStringLength                          = 0;

  AtIsvStringCmd.Header.Version.Minor       = ATHI_PROTOCOL_VERSION_MINOR;
  AtIsvStringCmd.Header.Version.Major       = ATHI_PROTOCOL_VERSION_MAJOR;
  AtIsvStringCmd.Header.Command.Category    = ATHI_CMD_GROUP_DATA_STORAGE;
  AtIsvStringCmd.Header.Command.IsResponse  = AT_COMMAND;
  AtIsvStringCmd.Header.Command.Code        = ATHI_DATA_STORE_GRP_GET_VENDOR_STRING_CMD;
  AtIsvStringCmd.Header.Length              = sizeof (ATHI_GET_VENDOR_STRING_CMD) - sizeof (ATHI_HEADER);
  AtIsvStringCmd.Id                         = (UINT8) *StringId;

  if (mHeci == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiHeciProtocolGuid,
                    NULL,
                    (VOID **) &mHeci
                    );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "AtIsvStringCmd: Locating for HECI Driver Failed!, Status = %r\n", Status));
      return Status;
    }
  }

  HeciLength = sizeof (ATHI_GET_VENDOR_STRING_CMD);

  PERF_START_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8200);
  Status = mHeci->SendMsg (
                  (UINT32 *) &AtIsvStringCmd,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_AT_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AtIsvStringCmd failed to send message over HECI!(SendMsg), Status = %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (ATHI_GET_VENDOR_STRING_RSP);
  Status = mHeci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &AtIsvStringRsp,
                  &HeciLength
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AtIsvStringRsp failed to receive message over HECI!(ReadMsg), Status = %r\n", Status));
    PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8202);
    return Status;
  }

  PERF_END_EX (NULL, EVENT_REC_TOK, NULL, AsmReadTsc(), 0x8201);

#ifdef EFI_DEBUG
  DEBUG ((EFI_D_ERROR, "AT::AtIsvStringRsp response message length = %x\n", HeciLength));
  DEBUG ((EFI_D_ERROR, "AtIsvStringRsp.CompletionCode = %x\n", AtIsvStringRsp.CompletionCode));
  DEBUG ((EFI_D_ERROR, "AtIsvStringRsp.String.Length = %x\n", AtIsvStringRsp.String.Length));
  DEBUG_CODE (
    ShowBuffer ((UINT8 *) &AtIsvStringRsp.String.Value, (UINT32) AtIsvStringRsp.String.Length);
  );
#endif

  if ((AtIsvStringRsp.CompletionCode == 0) &&
      (AtIsvStringRsp.String.Length > 0 && AtIsvStringRsp.String.Length < 256)
      ) {

    CopyMem (IsvString, &AtIsvStringRsp.String.Value, AtIsvStringRsp.String.Length);
    *IsvStringLength = AtIsvStringRsp.String.Length;
    return EFI_SUCCESS;
  }

  return EFIERR (AtIsvStringRsp.CompletionCode);;
}

/**
  This send an AssertStolen Message to ME when OEM has set the AllowAssertStolen bit to be accepted by BIOS.

  @param[in] This                 The address of protocol
  @param[out] CompletionCode      The return ME Firmware return code for this request

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
SendAssertStolen (
  IN     EFI_AT_PROTOCOL          *This,
  OUT    UINT8                    *CompletionCode
  )
{

  UINT32                          HeciLength;
  EFI_STATUS                      Status;
  ATHI_ASSERT_STOLEN_CMD          AtAssertStolenCmd;
  ATHI_ASSERT_STOLEN_RSP          AtAssertStolenRsp;

  ///
  /// Initialize Variables
  ///
  ZeroMem ((VOID *) &AtAssertStolenCmd, sizeof (ATHI_ASSERT_STOLEN_CMD));
  ZeroMem ((VOID *) &AtAssertStolenRsp, sizeof (ATHI_ASSERT_STOLEN_RSP));

  ///
  /// Populate AtAssertStolenCmd
  ///
  AtAssertStolenCmd.Header.Version.Minor        = ATHI_PROTOCOL_VERSION_MINOR;
  AtAssertStolenCmd.Header.Version.Major        = ATHI_PROTOCOL_VERSION_MAJOR;
  AtAssertStolenCmd.Header.Command.Category     = ATHI_CMD_GROUP_THEFT_DETECTION;
  AtAssertStolenCmd.Header.Command.IsResponse   = AT_COMMAND;
  AtAssertStolenCmd.Header.Command.Code         = ATHI_THEFT_DETECT_GRP_UNSIGNED_ASSERT_STOLEN_CMD;
  AtAssertStolenCmd.Header.Length               = sizeof (ATHI_ASSERT_STOLEN_CMD) - sizeof (ATHI_HEADER);

  if (mHeci == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiHeciProtocolGuid,
                    NULL,
                    (VOID **) &mHeci
                    );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "SendAssertStolen: Locating for HECI Driver Failed!, Status = %r\n", Status));
      return Status;
    }
  }

  ///
  /// Send AtAssertStolenCmd Request
  ///
  HeciLength = sizeof (ATHI_ASSERT_STOLEN_CMD);

  Status = mHeci->SendMsg (
                  (UINT32 *) &AtAssertStolenCmd,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_AT_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AtAssertStolenCmd failed to send message over HECI!(SendMsg), Status = %r\n", Status));
    return Status;
  }
  ///
  /// Receive AtAssertStolenCmd Response
  ///
  HeciLength = sizeof (ATHI_ASSERT_STOLEN_RSP);
  Status = mHeci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &AtAssertStolenRsp,
                  &HeciLength
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AtAssertStolenRsp failed to receive message over HECI!(ReadMsg), Status = %r\n", Status));
    return Status;
  }

  DEBUG ((EFI_D_ERROR, "ATAM: AtAssertStolen response message length %x\n", HeciLength));
  DEBUG ((EFI_D_ERROR, "ATAM: AtAssertStolenRsp.CompletionCode = %x\n", AtAssertStolenRsp.CompletionCode));

  *CompletionCode = AtAssertStolenRsp.CompletionCode;

  return EFI_SUCCESS;

}

/**
  This receives the ISV ID from ME and display the ID, when the platform is in stolen state

  @param[in] This                 The address of protocol
  @param[out] IsvId               The pointer to 4 byte ISV ID

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
GetIsvId (
  IN     EFI_AT_PROTOCOL          *This,
  OUT    UINT32                   *IsvId
  )
{

  UINT32              HeciLength;
  EFI_STATUS          Status;

  ATHI_GET_ISVID_CMD  AtGetIsvIdCmd;
  ATHI_GET_ISVID_RSP  AtGetIsvIdRsp;

  ZeroMem ((VOID *) &AtGetIsvIdCmd, sizeof (ATHI_GET_ISVID_CMD));
  ZeroMem ((VOID *) &AtGetIsvIdRsp, sizeof (ATHI_GET_ISVID_RSP));

  AtGetIsvIdCmd.Version.Minor       = ATHI_PROTOCOL_VERSION_MINOR;
  AtGetIsvIdCmd.Version.Major       = ATHI_PROTOCOL_VERSION_MAJOR;
  AtGetIsvIdCmd.Command.Category    = ATHI_CMD_GROUP_RECOVERY;
  AtGetIsvIdCmd.Command.IsResponse  = AT_COMMAND;
  AtGetIsvIdCmd.Command.Code        = ATHI_RECOVERY_GRP_GET_ISVID_CMD;
  AtGetIsvIdCmd.Length              = sizeof (ATHI_GET_ISVID_CMD) - sizeof (ATHI_HEADER);

  if (mHeci == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiHeciProtocolGuid,
                    NULL,
                    (VOID **) &mHeci
                    );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "GetIsvId: Locating for HECI Driver Failed!, Status = %r\n", Status));
      return Status;
    }
  }

  HeciLength = sizeof (ATHI_GET_ISVID_CMD);

  Status = mHeci->SendMsg (
                  (UINT32 *) &AtGetIsvIdCmd,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_AT_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "GetIsvId failed to send message over HECI!(SendMsg), Status = %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (ATHI_GET_ISVID_RSP);
  Status = mHeci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &AtGetIsvIdRsp,
                  &HeciLength
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "GetIsvId failed to receive message over HECI!(ReadMsg), Status = %r\n", Status));
    return Status;
  }

  DEBUG ((EFI_D_ERROR, "AT::AtGetIsvIdRsp response message length = %x\n", HeciLength));
  DEBUG ((EFI_D_ERROR, "AtGetIsvIdRsp.CompletionCode = %x\n", AtGetIsvIdRsp.CompletionCode));
  DEBUG ((EFI_D_ERROR, "AtGetIsvIdRsp.IsvId = %x\n", AtGetIsvIdRsp.IsvId));

  if (!AtGetIsvIdRsp.CompletionCode) {
    *IsvId = AtGetIsvIdRsp.IsvId;
  }

  return EFI_SUCCESS;

}

/**
  This requests FW to enter or exit Suspend mode based on user input

  @param[in] This                 The address of protocol
  @param[in] TransitionState      0: Exit Suspend Mode
                                  1: Enter Suspend Mode
  @param[in] Token                SSTK generated Token

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_OUT_OF_RESOURCES    Unable to allocate necessary data structures.
**/
EFI_STATUS
EFIAPI
SetSuspendState (
  IN     EFI_AT_PROTOCOL          *This,
  IN     UINT32                   TransitionState,
  IN     UINT8                    *Token
  )
{
  UINT32                      HeciLength;
  EFI_STATUS                  Status;
  EFI_HECI_PROTOCOL           *Heci;

  ATHI_SET_SUSPEND_STATE_CMD  *AtSetSuspendStateCmd;
  ATHI_SET_SUSPEND_STATE_RSP  AtSetSuspendStateRsp;

  AtSetSuspendStateCmd = AllocateZeroPool (sizeof (ATHI_SET_SUSPEND_STATE_CMD) + AT_USR_PASS_HASH_LENGTH_MAX);
  if (AtSetSuspendStateCmd == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  ZeroMem ((VOID *) &AtSetSuspendStateRsp, sizeof (ATHI_SET_SUSPEND_STATE_RSP));

  AtSetSuspendStateCmd->Header.Version.Minor        = ATHI_PROTOCOL_VERSION_MINOR;
  AtSetSuspendStateCmd->Header.Version.Major        = ATHI_PROTOCOL_VERSION_MAJOR;
  AtSetSuspendStateCmd->Header.Command.Category     = ATHI_CMD_GROUP_GENERAL;
  AtSetSuspendStateCmd->Header.Command.IsResponse   = AT_COMMAND;
  AtSetSuspendStateCmd->Header.Command.Code         = ATHI_GENERAL_GRP_SET_SUSPEND_CMD;
  AtSetSuspendStateCmd->TransitionState             = TransitionState;

  ///
  /// AT_CREDENTIAL has extra UINT8 (can't have zero length array like FW) that must be subtracted
  ///
  AtSetSuspendStateCmd->Header.Length = (sizeof (ATHI_SET_SUSPEND_STATE_CMD) - sizeof (UINT8)) - (sizeof (ATHI_HEADER)) + AT_USR_PASS_HASH_LENGTH_MAX;
  AtSetSuspendStateCmd->Credential.Type    = AT_CREDENTIAL_TYPE_SSTK;
  AtSetSuspendStateCmd->Credential.Length  = AT_USR_PASS_HASH_LENGTH_MAX;
  CopyMem (AtSetSuspendStateCmd->Credential.Value, Token, AT_USR_PASS_HASH_LENGTH_MAX);

  Status = gBS->LocateProtocol (
                 &gEfiHeciProtocolGuid,
                 NULL,
                 (VOID **) &Heci
                 );

  if (EFI_ERROR (Status)) {
   DEBUG ((EFI_D_ERROR, "SetSuspendState: Locating for HECI Driver Failed!, Status = %r\n", Status));
   FreePool (AtSetSuspendStateCmd);
   return Status;
  }

  HeciLength = sizeof (ATHI_SET_SUSPEND_STATE_CMD) - sizeof (UINT8) + AT_USR_PASS_HASH_LENGTH_MAX;

  Status = Heci->SendMsg (
                 (UINT32 *) AtSetSuspendStateCmd,
                 HeciLength,
                 BIOS_FIXED_HOST_ADDR,
                 HECI_AT_MESSAGE_ADDR
                 );

  if (EFI_ERROR (Status)) {
   DEBUG ((EFI_D_ERROR, "SetSuspendState failed to send message over HECI!(SendMsg), Status = %r\n", Status));
   FreePool (AtSetSuspendStateCmd);
   return Status;
  }

  FreePool (AtSetSuspendStateCmd);

  HeciLength = sizeof (ATHI_SET_SUSPEND_STATE_RSP);
  Status = Heci->ReadMsg (
                 BLOCKING,
                 (UINT32 *) &AtSetSuspendStateRsp,
                 &HeciLength
                 );

  if (EFI_ERROR (Status)) {
   DEBUG ((EFI_D_ERROR, "SetSuspendState failed to receive message over HECI!(ReadMsg), Status = %r\n", Status));
   return Status;
  }

  DEBUG ((EFI_D_ERROR, "AT::SetSuspendState response message length %x\n", HeciLength));
  DEBUG ((EFI_D_ERROR, "AtSetSuspendStateRsp.CompletionCode = %x\n", AtSetSuspendStateRsp.CompletionCode));

  if (AtSetSuspendStateRsp.CompletionCode) {
   return EFIERR (AtSetSuspendStateRsp.CompletionCode);
  }

  return EFI_SUCCESS;
}

/**
  This instructs FW that a WWAN recovery is desired and thus the Radio needs to be initialized.

  This command in not supported.

  @param[in] This                 The address of protocol
**/
EFI_STATUS
InitWWANREcov (
  IN     EFI_AT_PROTOCOL          *This
  )
{
  ///
  /// This command in not supported.
  ///
  return EFI_UNSUPPORTED;
}

/**
  This queries FW of the NIC Radio Status

  This command in not supported.

  @param[in] This                 The address of protocol
  @param[in] RadioStatus          Radio status
  @param[in] NetworkStatus        Network status
**/
EFI_STATUS
GetWWANNicStatus (
  IN     EFI_AT_PROTOCOL          *This,
  IN OUT UINT8                    *RadioStatus,
  IN OUT UINT8                    *NetworkStatus
  )
{
  ///
  /// This command in not supported.
  ///
  return EFI_UNSUPPORTED;
}

/**
  This queries FW of the AT Status in Unsigned mode

  @param[in] This                 The address of protocol
  @param[out] StateUnsigned       Structure retrieved from ME describing current AT state

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
GetStateUnsigned (
  IN    EFI_AT_PROTOCOL           *This,
  OUT   AT_STATE_INFO             *StateUnsigned
  )
{
  UINT32                          HeciLength;
  EFI_STATUS                      Status;

  ATHI_GET_STATE_CMD  AtGetStateCmd;
  ATHI_GET_STATE_RSP  AtGetStateRsp;

  ZeroMem ((VOID *) &AtGetStateCmd, sizeof (ATHI_GET_STATE_CMD));
  ZeroMem ((VOID *) &AtGetStateRsp, sizeof (ATHI_GET_STATE_RSP));

  AtGetStateCmd.Version.Major       = ATHI_PROTOCOL_VERSION_MAJOR;
  AtGetStateCmd.Version.Minor       = ATHI_PROTOCOL_VERSION_MINOR;
  AtGetStateCmd.Command.Code        = ATHI_THEFT_DETECT_GRP_GET_STATE_UNSIGNED;
  AtGetStateCmd.Command.Category    = ATHI_CMD_GROUP_THEFT_DETECTION;
  AtGetStateCmd.Command.IsResponse  = AT_COMMAND;
  AtGetStateCmd.Length              = sizeof (ATHI_GET_STATE_CMD) - sizeof (ATHI_HEADER);

  if (mHeci == NULL) {
    Status = gBS->LocateProtocol (
                    &gEfiHeciProtocolGuid,
                    NULL,
                    (VOID **) &mHeci
                    );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "AtGetStateUnsigned: Locating HECI Protocol failed!, Status = %r\n", Status));
      return Status;
    }
  }

  HeciLength = sizeof (ATHI_GET_STATE_CMD);

  Status = mHeci->SendMsg (
                  (UINT32 *) &AtGetStateCmd,
                  HeciLength,
                  BIOS_FIXED_HOST_ADDR,
                  HECI_AT_MESSAGE_ADDR
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AtGetStateUnsigned failed to send message over HECI!(SendMsg), Status = %r\n", Status));
    return Status;
  }

  HeciLength = sizeof (ATHI_GET_STATE_RSP);
  Status = mHeci->ReadMsg (
                  BLOCKING,
                  (UINT32 *) &AtGetStateRsp,
                  &HeciLength
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AtGetStateUnsigned failed to receive message over HECI!(ReadMsg), Status = %r\n", Status));
    return Status;
  }

  if (AtGetStateRsp.CompletionCode == ATHI_COMPCODE_SUCCESS) {
    DEBUG ((EFI_D_ERROR, "AtGetStateRsp.CompletionCode = %x\n", AtGetStateRsp.CompletionCode));
    CopyMem (StateUnsigned, &(AtGetStateRsp.StateInfo), sizeof (AT_STATE_INFO));
  }

  return EFI_SUCCESS;
}
