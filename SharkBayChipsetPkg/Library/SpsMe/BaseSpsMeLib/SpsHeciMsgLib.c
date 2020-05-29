/** @file

  Implementation file for SPS ME Heci Message functionality
  Note: Only for SPS.

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

#include <Uefi.h>

#include <SpsMe.h>

#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>

#include <Library/SpsMeLib.h>
#include <Library/SpsHeciLib.h>

/**
  Initialize SPS ME HECI library.

  @param None.

**/
EFI_STATUS
SpsHeciMsgLibInit (
  VOID
  )
{
  ///
  /// Do not thing In Dxe Phase.
  ///
  return EFI_SUCCESS;
}

/**
  Initilize HECI.

  @param None.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Heci initial succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
SpsHeciInit (
  VOID
  )
{
  EFI_STATUS            Status;

  Status = SpsHeciInitialize ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: SPS HECI Initialize failed! (%r)\n", Status));
    return Status;
  }

  return EFI_SUCCESS;
}

/**
  Send Get Firmware Version Request to ME

  @param[out] GetFwVersionRsp     Return themessage of FW version

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too smallfor the Acknowledge
**/
EFI_STATUS
SpsHeciGetFwVersion (
  OUT VOID                             *FwVersion
  )
{
  EFI_STATUS            Status;
  UINT32                HeciLength;
  SPS_GET_FW_VER_REQ    GetFwVersionReq;
  UINT32                MeMode;
  SPS_GET_FW_VER_RSP    TempGetFwVersionRsp;

  if (FwVersion == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = SpsGetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (!IS_SPS_ME_OPMODE_SPS (MeMode))) {
    return EFI_UNSUPPORTED;
  }
  ZeroMem (&TempGetFwVersionRsp, sizeof (SPS_GET_FW_VER_RSP));

  ///
  /// Send Get Firmware Version Request to ME
  ///
  /// HECI Header : 0x80040007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x04
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x000002FF
  ///   |- [ 7: 0] Group    = 0xFF (SPS_MKHI_GRP_GEN)
  ///   |- [14: 8] Command  = 0x02 (SPS_MKHI_CMD_GET_FW_VERSION_REQ)
  ///   -- [15]    Response = 0
  ///
  GetFwVersionReq.MkhiHeader.Data              = 0;
  GetFwVersionReq.MkhiHeader.Fields.GroupId    = SPS_MKHI_GRP_GEN;
  GetFwVersionReq.MkhiHeader.Fields.Command    = SPS_MKHI_CMD_GET_FW_VERSION_REQ;
  GetFwVersionReq.MkhiHeader.Fields.IsResponse = 0;

  HeciLength = sizeof (SPS_GET_FW_VER_REQ);
  Status = SpsHeciSendMsg (
             (UINT32 *) &GetFwVersionReq,
             HeciLength,
             SPS_HECI_CLIENT_HOST,
             SPS_HECI_CLIENT_CORE
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Send [Get Firmware Version] request message => (%r)\n", Status));
    return Status;
  }

  ///
  /// Read Get Firmware Version response from ME
  ///
  /// HECI Header : 0x80xx0007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x20 or 0x36
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x000002FF
  ///   |- [ 7: 0] Group           = 0xFF (SPS_MKHI_GRP_GEN)
  ///   |- [14: 8] Command         = 0x02 (SPS_MKHI_CMD_GET_FW_VERSION_REQ)
  ///   -- [15]    Response        = 1
  ///
  /// Byte[ 1: 0] : Active firmware version minor number
  /// Byte[ 3: 2] : Active firmware version major number
  /// Byte[ 5: 4] : Active firmware version build number
  /// Byte[ 7: 6] : Active firmware version patch number
  /// Byte[ 9: 8] : Recovery firmware version minor number
  /// Byte[11:10] : Recovery firmware version major number
  /// Byte[13:12] : Recovery firmware version build number
  /// Byte[15:14] : Recovery firmware version patch number
  /// Byte[17:16] : Backup firmware version minor number
  /// Byte[19:18] : Backup firmware version major number
  /// Byte[21:20] : Backup firmware version build number
  /// Byte[23:22] : Backup firmware version patch number
  ///
  HeciLength = sizeof (SPS_GET_FW_VER_RSP);
  Status = SpsHeciReadMsg (
             BLOCKING,
             (UINT32 *) &TempGetFwVersionRsp,
             &HeciLength
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Read [Get Firmware Version] response message => (%r)\n", Status));
    return Status;
  }

  if (FwVersion != NULL) {
    CopyMem (FwVersion , &(TempGetFwVersionRsp.Data), sizeof (SPS_GET_FW_VER_RSP_DATA));
  } else {
    return EFI_INVALID_PARAMETER;
  }

  return Status;
}

/**
  Send ME-BIOS Interface Version Request Message through HECI.

  @param[in]  PeiServices         General purpose services available to every PEIM.
  @param[out] Resul               ME response after Dynamic Fusing Request

  @retval EFI_SUCCESS             Command succeeded
**/
EFI_STATUS
SpsHeciGetMeBiosInterfaceVersion (
  OUT VOID                             *MeBiosInterfVer
  )
{
  EFI_STATUS                        Status;
  SPS_GET_ME_BIOS_INTERFACE_REQ     MeBiosInterfaceRequest;
  SPS_GET_ME_BIOS_INTERFACE_RSP     MeBiosInterfaceResponse;
  UINT32                            HeciLength;

  if (MeBiosInterfVer == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  Status = EFI_DEVICE_ERROR;
  ///
  /// Send SPS_GET_MEBIOS_INTERFACE request to ME
  ///
  /// HECI Header : 0x80010020
  ///   |- [ 7: 0] ME client ID    = 0x20 (SPS_HECI_CLIENT_SPS)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x01
  ///   -- [31]    MessageComplete = 1
  ///
  /// Command = 0x01 (SPS_CMD_GET_MEBIOS_INTERFACE_REQ)
  ///
  MeBiosInterfaceRequest.Command = SPS_CMD_GET_MEBIOS_INTERFACE_REQ;
  HeciLength = sizeof (SPS_GET_ME_BIOS_INTERFACE_REQ);
  Status = SpsHeciSendMsg (
             (UINT32 *) &MeBiosInterfaceRequest,
             HeciLength,
             SPS_HECI_CLIENT_HOST,
             SPS_HECI_CLIENT_SPS
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Send [SPS_GET_MEBIOS_INTERFACE] request message => (%r)\n", Status));
    return Status;
  }

  ///
  /// Read SPS_GET_MEBIOS_INTERFACE response from ME
  ///
  /// HECI Header : 0x80050020
  ///   |- [ 7: 0] ME client ID    = 0x20 (SPS_HECI_CLIENT_SPS)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x05
  ///   -- [31]    MessageComplete = 1
  ///
  /// Byte[0]   : Command = 0x81 (SPS_CMD_GET_MEBIOS_INTERFACE_RSP)
  /// Byte[1]   : Major Interface Version = 1 
  /// Byte[2]   : Minor Interface Version = 0 
  /// Byte[4:3] : Active Feature Set
  ///
  ZeroMem (&MeBiosInterfaceResponse, sizeof (SPS_GET_ME_BIOS_INTERFACE_RSP));
  HeciLength = sizeof (SPS_GET_ME_BIOS_INTERFACE_RSP);
  Status = SpsHeciReadMsg (
             BLOCKING,
             (UINT32 *) &MeBiosInterfaceResponse,
             &HeciLength
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Send [SPS_GET_MEBIOS_INTERFACE] response message => (%r)\n", Status));
    return Status;
  }

  if (MeBiosInterfVer != NULL) {
    CopyMem (MeBiosInterfVer, &MeBiosInterfaceResponse, HeciLength);
  } else {
    return EFI_INVALID_PARAMETER;
  }

  return Status;
}

/**
  Sends a message to ME to lock a specified SPI Flash region for writing and receiving a response message.

  @param[out] Nonce               Random number generated by Ignition ME FW. When BIOS
                                  want to unlock region it should use this value
                                  in HMRFPO_ENABLE Request Message
  @param[out] FactoryDefaultBase  The base of the factory default calculated from the start of the ME region.
                                  BIOS sets a Protected Range (PR) register!¦s "Protected Range Base" field with this value
                                  + the base address of the region.
  @param[out] FactoryDefaultLimit The length of the factory image.
                                  BIOS sets a Protected Range (PR) register!¦s "Protected Range Limit" field with this value
  @param[out] Result              Status report

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
SpsHeciHmrfpoLock (
  OUT UINT64                           *Nonce,
  OUT UINT32                           *FactoryDefaultBase,
  OUT UINT32                           *FactoryDefaultLimit,
  OUT UINT8                            *Result
  )
{
  EFI_STATUS                Status;
  SPS_HMRFPO_LOCK_REQ       HmrfpoLockRequest;
  SPS_HMRFPO_LOCK_RSP       HmrfpoLockResponse;
  UINT32                    HeciLength;
  UINT32                    SpsMeStatus;
  UINT32                    SpsMeMode;

  SpsMeStatus = 0xFF;
  SpsMeMode   = 0xFF; 

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1 - 3.10.1.1  HMRFPO_LOCK message
  ///
  ///  /* HMRFPO_LOCK message */
  ///  Before sending this message BIOS has to check ME Firmware Status
  ///  to make sure ME finished initialization (MEFS1.CurrentState is higher than 1,
  ///  see Table 3-4)
  ///
  Status = SpsGetMeStatus (&SpsMeStatus);
  if (EFI_ERROR (Status) || (SPS_ME_CURSTATE (SpsMeStatus) <= SPS_ME_CURSTATE_INIT)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: SPS ME FW in initialization(0x%x) => (%r)\n",SpsMeStatus, Status));
    return EFI_NOT_READY;
  }
  
  Status = SpsGetMeMode (&SpsMeMode);
  if (EFI_ERROR (Status) || (!IS_SPS_ME_OPMODE_SPS (SpsMeMode))) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// Send HMRFPO_LOCK Request to ME
  ///
  /// HECI Header : 0x80040007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x04
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x00000205
  ///   |- [ 7: 0] Group           = 0x05 (SPS_MKHI_GRP_HMRFPO)
  ///   |- [14: 8] Command         = 0x02 (SPS_HMRFPO_CMD_LOCK)
  ///   -- [15]    Response        = 0
  ///
  HmrfpoLockRequest.MkhiHeader.Data               = 0;
  HmrfpoLockRequest.MkhiHeader.Fields.GroupId     = SPS_MKHI_GRP_HMRFPO;
  HmrfpoLockRequest.MkhiHeader.Fields.Command     = SPS_HMRFPO_CMD_LOCK;
  HmrfpoLockRequest.MkhiHeader.Fields.IsResponse  = 0;

  HeciLength = sizeof (SPS_HMRFPO_LOCK_REQ);
  Status = SpsHeciSendMsg (
             (UINT32 *) &HmrfpoLockRequest,
             HeciLength,
             SPS_HECI_CLIENT_HOST,
             SPS_HECI_CLIENT_CORE
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Send [HMRFPO_LOCK] request message => (%r)\n", Status));
    return Status;
  }

  ///
  /// Read HMRFPO_LOCK response from ME
  ///
  /// HECI Header : 0x80180007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x18
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x00008205
  ///   |- [ 7: 0] Group           = 0x05 (SPS_MKHI_GRP_HMRFPO)
  ///   |- [14: 8] Command         = 0x02 (SPS_HMRFPO_CMD_LOCK)
  ///   -- [15]    Response        = 1
  ///
  /// Byte[ 7: 0]  : Nonce
  /// Byte[11: 8]  : Factory Base
  /// Byte[15: 12] : Factory Limit
  /// Byte[16]     : Status
  /// Byte[19:17]  : Reserved
  ///
  HeciLength = sizeof (SPS_HMRFPO_LOCK_RSP);
  Status = SpsHeciReadMsg (
             BLOCKING,
             (UINT32 *) &HmrfpoLockResponse,
             &HeciLength
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Read [HMRFPO_LOCK] response message => (%r)\n", Status));
    return Status;
  }

  if (Nonce != NULL) {
    *Nonce = HmrfpoLockResponse.Nonce;
  }
  if (FactoryDefaultBase != NULL) {
    *FactoryDefaultBase = HmrfpoLockResponse.FactoryDefaultBase;
  }
  if (FactoryDefaultLimit != NULL) {
    *FactoryDefaultLimit = HmrfpoLockResponse.FactoryDefaultLimit;
  }
  if (Result != NULL) {
    *Result = HmrfpoLockResponse.Status;
  }


  return Status;
}

/**
  Sends a message to ME to unlock a specified SPI Flash region for writing and receiving a response message.
  It is recommended that HMRFPO_ENABLE MEI message needs to be sent after all OROMs finish their initialization.

  @param[in] Nonce                Nonce received in previous HMRFPO_ENABLE Response Message
  @param[in] Result               HMRFPO_ENABLE response

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
**/
EFI_STATUS
SpsHeciHmrfpoEnable (
  IN  UINT64                           Nonce,
  OUT UINT32                           *FactoryDefaultBase,
  OUT UINT32                           *FactoryDefaultLimit,
  OUT UINT8                            *Result
  )
{
  EFI_STATUS                  Status;
  SPS_HMRFPO_ENABLE_REQ       HmrfpoEnableRequest;
  SPS_HMRFPO_ENABLE_RSP       HmrfpoEnableResponse;
  UINT32                      HeciLength;
  UINT32                      MeMode;

  Status = SpsGetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (!IS_SPS_ME_OPMODE_SPS (MeMode))) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// Send HMRFPO_ENABLE Request to ME
  ///
  /// HECI Header : 0x800C0007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x0C
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x00000105
  ///   |- [ 7: 0] Group           = 0x05 (SPS_MKHI_GRP_HMRFPO)
  ///   |- [14: 8] Command         = 0x01 (SPS_HMRFPO_CMD_ENABLE)
  ///   -- [15]    Response        = 0
  ///
  /// Byte[ 7: 0]  : Nonce
  ///
  HmrfpoEnableRequest.MkhiHeader.Data               = 0;
  HmrfpoEnableRequest.MkhiHeader.Fields.GroupId     = SPS_MKHI_GRP_HMRFPO;
  HmrfpoEnableRequest.MkhiHeader.Fields.Command     = SPS_HMRFPO_CMD_ENABLE;
  HmrfpoEnableRequest.MkhiHeader.Fields.IsResponse  = 0;
  HmrfpoEnableRequest.Nonce                         = Nonce;
  HeciLength = sizeof (SPS_HMRFPO_ENABLE_REQ);
  Status = SpsHeciSendMsg (
             (UINT32 *) &HmrfpoEnableRequest,
             HeciLength,
             SPS_HECI_CLIENT_HOST,
             SPS_HECI_CLIENT_CORE
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Send [HMRFPO_ENABLE] request message => (%r)\n", Status));
    return Status;
  }

  ///
  /// Read HMRFPO_ENABLE response from ME
  ///
  /// HECI Header : 0x80100007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x10
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x00008105
  ///   |- [ 7: 0] Group           = 0x05 (SPS_MKHI_GRP_HMRFPO)
  ///   |- [14: 8] Command         = 0x01 (SPS_HMRFPO_CMD_ENABLE)
  ///   -- [15]    Response        = 1
  ///
  /// Byte[ 3: 0] : Factory Base
  /// Byte[ 7: 4] : Factory Limit
  /// Byte[8]     : Status
  /// Byte[11: 9] : Reserved
  ///
  HeciLength = sizeof (SPS_HMRFPO_ENABLE_RSP);
  Status = SpsHeciReadMsg (
             BLOCKING,
             (UINT32 *) &HmrfpoEnableResponse,
             &HeciLength
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Read [HMRFPO_ENABLE] response message => (%r)\n", Status));
    return Status;
  }

  if (FactoryDefaultBase != NULL) {
    *FactoryDefaultBase = HmrfpoEnableResponse.FactoryDefaultBase;
  }
  if (FactoryDefaultLimit != NULL) {
    *FactoryDefaultLimit = HmrfpoEnableResponse.FactoryDefaultLimit;
  }
  if (Result != NULL) {
    *Result = HmrfpoEnableResponse.Status;
  }

  return Status;
}

/**
  System BIOS sends this message to get status for HMRFPO_GET_STATUS message.

  @param[out] Result              HMRFPO_GET_STATUS response

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
SpsHeciHmrfpoGetStatus (
  OUT UINT8                            *Result
  )
{
  EFI_STATUS                      Status;
  SPS_HMRFPO_GET_STATUS_REQ       HmrfpoGetStatusRequest;
  SPS_HMRFPO_GET_STATUS_RSP       HmrfpoGetStatusResponse;
  UINT32                          HeciLength;
  UINT32                          MeMode;

  Status = SpsGetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (!IS_SPS_ME_OPMODE_SPS (MeMode))) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// Send HMRFPO_GET_STATUS Request to ME
  ///
  /// HECI Header : 0x80040007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x04
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x00000305
  ///   |- [ 7: 0] Group           = 0x05 (SPS_MKHI_GRP_HMRFPO)
  ///   |- [14: 8] Command         = 0x03 (SPS_HMRFPO_CMD_GET_STATUS)
  ///   -- [15]    Response        = 0
  ///
  HmrfpoGetStatusRequest.MkhiHeader.Data              = 0;
  HmrfpoGetStatusRequest.MkhiHeader.Fields.GroupId    = SPS_MKHI_GRP_HMRFPO;
  HmrfpoGetStatusRequest.MkhiHeader.Fields.Command    = SPS_HMRFPO_CMD_GET_STATUS;
  HmrfpoGetStatusRequest.MkhiHeader.Fields.IsResponse = 0;
  HeciLength = sizeof (SPS_HMRFPO_GET_STATUS_REQ);
  Status = SpsHeciSendMsg (
             (UINT32 *) &HmrfpoGetStatusRequest,
             HeciLength,
             SPS_HECI_CLIENT_HOST,
             SPS_HECI_CLIENT_CORE
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Send [HMRFPO_GET_STATUS] request message => (%r)\n", Status));
    return Status;
  }

  ///
  /// Read HMRFPO_GET_STATUS response from ME
  ///
  /// HECI Header : 0x80080007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x08
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x00008305
  ///   |- [ 7: 0] Group           = 0x05 (SPS_MKHI_GRP_HMRFPO)
  ///   |- [14: 8] Command         = 0x03 (SPS_HMRFPO_CMD_GET_STATUS)
  ///   -- [15]    Response        = 1
  ///
  /// Byte[0]   : Status
  /// Byte[3:1] : Reserved
  ///
  HeciLength = sizeof (SPS_HMRFPO_GET_STATUS_RSP);
  Status = SpsHeciReadMsg (
             BLOCKING,
             (UINT32 *) &HmrfpoGetStatusResponse,
             &HeciLength
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Read [HMRFPO_GET_STATUS] response message => (%r)\n", Status));
    return Status;
  }

  if (Result != NULL) {
    *Result = HmrfpoGetStatusResponse.Status;
  }

  return Status;
}

/**
  Enables/disables clocks. Used to turn off clocks in unused pci/pcie slots.
  BIOS use this command when it enumerates PCI slots. When PCI slot is found unpopulated, the
  BIOS can disable its clock through this MEI message. It is the BIOS requirement to know which
  slot is controlled by which control bit.

  @param[in] Enables              each bit means corresponding clock should be turned on (1) or off (0)
  @param[in] EnablesMask          each bit means corresponding enable bit is valid (1) or should be ignored (0)
  @param[in] ResponseMode         [0] wait for response, [1] skip

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           ME is not ready
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
SpsHeciSetIccClockEnables (
  IN UINT32                            Enables,
  IN UINT32                            EnablesMask,
  IN UINT8                             ResponseMode
  )
{
  EFI_STATUS                 Status;
  SPS_ICC_SET_CLOCK_ENABLES  IccSetClockEnables;
  UINT32                     HeciLength;
  UINT32                     MeMode;
  UINT32                     SpsMeStatus;

  Status = SpsGetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (!IS_SPS_ME_OPMODE_SPS (MeMode))) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Get SPS Me mode. => (%r)\n", Status));
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: ME not in Normal mode. ME Mode => (%x)\n", MeMode));
    return EFI_UNSUPPORTED;
  }

  Status = SpsGetMeStatus (&SpsMeStatus);
  if (EFI_ERROR (Status) || (!IS_SPS_ME_CURSTATE_NORMAL(SpsMeStatus))) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: ME NOT READY. ME Status => (%x)\n", SpsMeStatus));
    return EFI_NOT_READY;
  }

  ///
  /// Send ICC_SET_CLOCK_ENABLES Request to ME
  ///
  /// HECI Header : 0x80200008
  ///   |- [ 7: 0] ME client ID    = 0x08 (SPS_HECI_CLIENT_ICC)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x20
  ///   -- [31]    MessageComplete = 1
  ///
  /// ICC Header
  ///   |- [ 3: 0] API Version     = 0x00030000 (SPS_ICC_API_VERSION)
  ///   |- [ 7: 4] ICC Command     = 0x03 (SPS_SET_CLOCK_ENABLES)
  ///   |- [11: 8] ICC Status      = 0x00 (SPS_ICC_STATUS_SUCCESS)
  ///   |- [15:12] Buffer Length   = 0x0C
  ///
  /// Byte[ 3: 0] : Clock Enables
  /// Byte[ 7: 4] : Clock Enables Mask
  /// Byte[11: 8] : Params
  ///
  HeciLength = sizeof (SPS_ICC_SET_CLOCK_ENABLES_REQ);

  IccSetClockEnables.Req.Header.ApiVersion    = SPS_ICC_API_VERSION;   /// API version for Lynx Point platforms is 3.0
  IccSetClockEnables.Req.Header.IccCommand    = SPS_SET_CLOCK_ENABLES; /// Set Spread Spectrum Config
  IccSetClockEnables.Req.Header.IccStatus     = SPS_ICC_STATUS_SUCCESS;
  IccSetClockEnables.Req.Header.BufferLength  = HeciLength - sizeof (SPS_ICC_HEADER);
  IccSetClockEnables.Req.Header.Reserved      = 0;
  IccSetClockEnables.Req.ClockEnables         = Enables;
  IccSetClockEnables.Req.ClockEnablesMask     = EnablesMask;
  IccSetClockEnables.Req.Params               = ResponseMode;

  Status = SpsHeciSendMsg (
             (UINT32 *) &IccSetClockEnables,
             HeciLength,
             SPS_HECI_CLIENT_HOST,
             SPS_HECI_CLIENT_ICC
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Send [ICC_SET_CLOCK_ENABLES] request message => (%r)\n", Status));
    return Status;
  }

  if (ResponseMode == SPS_ICC_RESPONSE_MODE_SKIP) {
    DEBUG ((DEBUG_INFO, "[SPS HECI MSG LIB] INFO: Skip Read [ICC_SET_CLOCK_ENABLES] response message => %r\n", Status));
    return Status;
  }

  ///
  /// Read ICC_SET_CLOCK_ENABLES response from ME
  ///
  /// HECI Header : 0x80140008
  ///   |- [ 7: 0] ME client ID    = 0x08 (SPS_HECI_CLIENT_ICC)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x14
  ///   -- [31]    MessageComplete = 1
  ///
  /// ICC Header
  ///   |- [ 3: 0] API Version     = 0x00030000 (SPS_ICC_API_VERSION)
  ///   |- [ 7: 4] ICC Command     = 0x03 (SPS_SET_CLOCK_ENABLES)
  ///   |- [11: 8] ICC Status      =
  ///   |- [15:12] Buffer Length   =
  ///
  HeciLength = sizeof (SPS_ICC_SET_CLOCK_ENABLES_RSP);
  Status = SpsHeciReadMsg (
             BLOCKING,
             (UINT32 *) &IccSetClockEnables,
             &HeciLength
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Read [ICC_SET_CLOCK_ENABLES] response message => (%r)\n", Status));
    return Status;
  }

  if (IccSetClockEnables.Rsp.Header.IccStatus != SPS_ICC_STATUS_SUCCESS) {
    DEBUG ((
      DEBUG_ERROR,
      "[SPS HECI MSG LIB] ERROR: [ICC_SET_CLOCK_ENABLES] Message Wrong response! ICC Status = 0x%x\n",
      IccSetClockEnables.Rsp.Header.IccStatus
      ));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Enabling/disabling 0.5% default spectrum spread on specified clock source.

  @param[in] IccSSMSelect         Selects spread spectrum mode for SSC[n] outputs
  @param[in] ResponseMode         [0] wait for response, [1] skip

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_DEVICE_ERROR        Wrong response
  @retval EFI_NOT_READY           ME is not ready
  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
SpsHeciSetIccSpreadSpectrum (
  IN UINT32                            IccSSMSelect,
  IN UINT8                             ResponseMode
  )
{
  EFI_STATUS                      Status;
  SPS_ICC_SET_SPREAD_SPECTRUM     IccSpreadSpectrum;
  UINT32                          HeciLength;

  ///
  /// Send ICC_SET_SPREAD_SPECTRUM Request to ME
  ///
  ///
  /// HECI Header : 0x801C0008
  ///   |- [ 7: 0] ME client ID    = 0x08 (SPS_HECI_CLIENT_ICC)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x1C
  ///   -- [31]    MessageComplete = 1
  ///
  /// ICC Header
  ///   |- [ 3: 0] API Version     = 0x00030000 (SPS_ICC_API_VERSION)
  ///   |- [ 7: 4] ICC Command     = 0x0C (SPS_SET_SPREAD_SPECTRUM_CONFIG)
  ///   |- [11: 8] ICC Status      = 0x00 (SPS_ICC_STATUS_SUCCESS)
  ///   |- [15:12] Buffer Length   = 0x08
  ///
  /// Byte[ 3: 0] : Spread Spectrum Mode Select
  /// Byte[ 7: 4] : Misc Params
  ///
  HeciLength = sizeof (SPS_ICC_SET_SPREAD_SPECTRUM_REQ);

  IccSpreadSpectrum.Req.Header.ApiVersion    = SPS_ICC_API_VERSION;            /// API version for Lynx Point platforms is 3.0
  IccSpreadSpectrum.Req.Header.IccCommand    = SPS_SET_SPREAD_SPECTRUM_CONFIG; /// Set Spread Spectrum Config
  IccSpreadSpectrum.Req.Header.IccStatus     = SPS_ICC_STATUS_SUCCESS;
  IccSpreadSpectrum.Req.Header.BufferLength  = HeciLength - sizeof (SPS_ICC_HEADER);
  IccSpreadSpectrum.Req.Header.Reserved      = 0;
  IccSpreadSpectrum.Req.IccSSMSelect         = IccSSMSelect;
  IccSpreadSpectrum.Req.Params               = ResponseMode;

  Status = SpsHeciSendMsg (
             (UINT32 *) &IccSpreadSpectrum,
             HeciLength,
             SPS_HECI_CLIENT_HOST,
             SPS_HECI_CLIENT_ICC
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Send [ICC_SET_SPREAD_SPECTRUM] request message => (%r)\n", Status));
    return Status;
  }

  if (ResponseMode == SPS_ICC_RESPONSE_MODE_SKIP) {
    DEBUG ((DEBUG_INFO, "[SPS HECI MSG LIB] INFO: Skip Read [ICC_SET_SPREAD_SPECTRUM] response message => %r\n", Status));
    return Status;
  }

  ///
  /// Read ICC_SET_SPREAD_SPECTRUM response from ME
  ///
  /// HECI Header : 0x80140008
  ///   |- [ 7: 0] ME client ID    = 0x08 (SPS_HECI_CLIENT_ICC)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x14
  ///   -- [31]    MessageComplete = 1
  ///
  /// ICC Header
  ///   |- [ 3: 0] API Version     = 0x00030000 (SPS_ICC_API_VERSION)
  ///   |- [ 7: 4] ICC Command     = 0x0C (SPS_SET_SPREAD_SPECTRUM_CONFIG)
  ///   |- [11: 8] ICC Status      =
  ///   |- [15:12] Buffer Length   =
  ///
  HeciLength = sizeof (SPS_ICC_SET_SPREAD_SPECTRUM_RSP);
  Status = SpsHeciReadMsg (
             BLOCKING,
             (UINT32 *) &IccSpreadSpectrum,
             &HeciLength
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Read [ICC_SET_SPREAD_SPECTRUM] response message => (%r)\n", Status));
    return Status;
  }

  if (IccSpreadSpectrum.Rsp.Header.IccStatus != SPS_ICC_STATUS_SUCCESS) {
    DEBUG ((
      DEBUG_ERROR,
      "[SPS HECI MSG LIB] ERROR: [ICC_SET_SPREAD_SPECTRUM] Message Wrong response! ICC Status = 0x%x\n",
      IccSpreadSpectrum.Rsp.Header.IccStatus
      ));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Send the END_OF_POST to ME FW.

  @param None.

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
SpsHeciEndOfPost (
  VOID
  )
{
  EFI_STATUS                 Status;
  SPS_END_OF_POST_MESSAGE    EndOfPostMsg;
  UINT32                     HeciLength;
  UINT32                     MeMode;

  Status = SpsGetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (!IS_SPS_ME_OPMODE_SPS (MeMode))) {
    return EFI_UNSUPPORTED;
  }

  ///
  /// Send END_OF_POST Request to ME
  ///
  /// HECI Header : 0x80040007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x04
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x00000CFF
  ///   |- [ 7: 0] Group           = 0xFF (SPS_MKHI_GRP_GEN)
  ///   |- [14: 8] Command         = 0x0C (SPS_MKHI_CMD_END_OF_POST_REQ)
  ///   -- [15]    Response        = 0
  ///
  EndOfPostMsg.Req.MkhiHeader.Data              = 0;
  EndOfPostMsg.Req.MkhiHeader.Fields.GroupId    = SPS_MKHI_GRP_GEN;
  EndOfPostMsg.Req.MkhiHeader.Fields.Command    = SPS_MKHI_CMD_END_OF_POST_REQ;
  EndOfPostMsg.Req.MkhiHeader.Fields.IsResponse = 0;
  EndOfPostMsg.Rsp.Action                       = 0;

  HeciLength = sizeof (SPS_END_OF_POST_REQ);
  Status = SpsHeciSendMsg (
             (UINT32 *) &EndOfPostMsg,
             HeciLength,
             SPS_HECI_CLIENT_HOST,
             SPS_HECI_CLIENT_CORE
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Send [END_OF_POST] request message => (%r)\n", Status));
    return Status;
  }

  if (EFI_ERROR(Status)) {
    return Status;
  }

  ///
  /// Read END_OF_POST response from ME
  ///
  /// HECI Header : 0x800x0007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x04 or 0x08
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x00008CFF
  ///   |- [ 7: 0] Group           = 0xFF (SPS_MKHI_GRP_GEN)
  ///   |- [14: 8] Command         = 0x0C (SPS_MKHI_CMD_END_OF_POST_REQ)
  ///   -- [15]    Response        = 1
  ///
  /// Byte[3:0] : Action
  ///
  HeciLength = sizeof (SPS_END_OF_POST_RSP);
  Status = SpsHeciReadMsg (
             BLOCKING,
             (UINT32 *) &EndOfPostMsg,
             &HeciLength
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Read [END_OF_POST] response message => (%r)\n", Status));
    return Status;
  }

  if (EndOfPostMsg.Rsp.Action == SPS_END_OF_POST_GLOBAL_RESET) {
    DEBUG ((DEBUG_ERROR, "HeciSendEndOfPostMessage(): Reset requested by FW EOP response\n"));
    SpsMeCF9Configure (GlobalReset);
  }

  return EFI_SUCCESS;
}

/**
  Send the required system ChipsetInit Table to ME FW.

  @param[in] ChipsetInitTable     The required system ChipsetInit Table.
  @param[in] ChipsetInitTableLen  Length of the table in bytes

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
SpsHeciWritemPhySurvivabilityTable (
  IN UINT32                            ChipsetInitTableLen,
  IN UINT8                             *ChipsetInitTable
  )
{
  EFI_STATUS                 Status;
  UINT32                     HeciLength;
  UINT32                     MeMode;
  UINT32                     MeStatus;
  SPS_ICC_WRITE_MPHY_SETTINGS   mPhyWriteMessage;

  Status = SpsGetMeMode (&MeMode);
  if (EFI_ERROR (Status) || (!IS_SPS_ME_OPMODE_SPS (MeMode))) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Get SPS Me mode. => (%r)\n", Status));
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: ME not in Normal mode. ME Mode => (%x)\n", MeMode));
    return EFI_UNSUPPORTED;
  }

  Status = SpsGetMeStatus (&MeStatus);
  if (EFI_ERROR (Status) || (!IS_SPS_ME_CURSTATE_NORMAL(MeStatus))) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: ME NOT READY. ME Status => (%x)\n", MeStatus));
    return EFI_NOT_READY;
  }

  HeciLength = sizeof (SPS_ICC_WRITE_MPHY_SETTINGS_REQ);
  HeciLength = OFFSET_OF (SPS_ICC_WRITE_MPHY_SETTINGS_REQ, TableData) + ChipsetInitTableLen;

  mPhyWriteMessage.Req.Header.ApiVersion    = SPS_ICC_API_VERSION;       /// API version for Lynx Point platforms is 3.0
  mPhyWriteMessage.Req.Header.IccCommand    = SPS_WRITE_MPHY_SETTINGS;   /// Set Write mPhy settings message
  mPhyWriteMessage.Req.Header.IccStatus     = SPS_ICC_STATUS_SUCCESS;
  mPhyWriteMessage.Req.Header.BufferLength  = HeciLength - sizeof (SPS_ICC_HEADER);
  mPhyWriteMessage.Req.Header.Reserved      = 0;
  mPhyWriteMessage.Req.Flags                = 0;

  CopyMem (&mPhyWriteMessage.Req.TableData[0], ChipsetInitTable, ChipsetInitTableLen);

  ///
  /// Send WRITE_MPHY_SETTINGS Request to ME
  ///
  Status = SpsHeciSendMsg (
             (UINT32 *) &mPhyWriteMessage,
             HeciLength,
             SPS_HECI_CLIENT_HOST,
             SPS_HECI_CLIENT_ICC
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Send [WRITE_MPHY_SETTINGS] request message => (%r)\n", Status));
    return Status;
  }

  ///
  /// Read WRITE_MPHY_SETTINGS response from ME
  ///
  HeciLength = sizeof (SPS_ICC_WRITE_MPHY_SETTINGS_RSP);
  Status = SpsHeciReadMsg (
             BLOCKING,
             (UINT32 *) &mPhyWriteMessage,
             &HeciLength
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Read [WRITE_MPHY_SETTINGS] response message => (%r)\n", Status));
    return Status;
  }

  if (mPhyWriteMessage.Rsp.Header.IccStatus != SPS_ICC_STATUS_SUCCESS) {
    DEBUG ((
      DEBUG_ERROR,
      "[SPS HECI MSG LIB] ERROR: [WRITE_MPHY_SETTINGS] Message Wrong response! ICC Status = 0x%x\n",
      mPhyWriteMessage.Rsp.Header.IccStatus
      ));
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}


/**
  Send CPU Dynamic Fusing Request Message through HECI.

  @param[out] Resul               ME response after Dynamic Fusing Request

  @retval EFI_SUCCESS             Command succeeded
**/
EFI_STATUS
SpsHeciSendDynamicFusing (
  OUT UINT8                            *Result
  )
{
  EFI_STATUS              Status;
  SPS_DYNAMIC_FUSING_RSP  DynamicFusingMsg;
  UINT32                  HeciLength;

  ///
  /// Server Platform Services(SPS) Firmware ME-BIOS Interface
  ///  for Denlow Revision 1.0.1 - 3.13.2  Dynamic fusing HECI messages
  ///  /* Dynamic fusing HECI messages */
  ///

  ///
  /// Send Dynamic Fusing request to ME
  ///
  /// HECI Header : 0x80xx0007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x20 or 0x36
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x00000CFF
  ///   |- [ 7: 0] Group           = 0x13 (?)
  ///   |- [14: 8] Command         = 0x01 (?)
  ///   -- [15]    Response        = 0
  ///
  DynamicFusingMsg.MkhiHeader.Data               = 0;
  DynamicFusingMsg.MkhiHeader.Fields.Command     = 1;
  DynamicFusingMsg.MkhiHeader.Fields.IsResponse  = 0;
  DynamicFusingMsg.MkhiHeader.Fields.GroupId     = 0x13;
  DynamicFusingMsg.MkhiHeader.Fields.Reserved    = 0;
  DynamicFusingMsg.MkhiHeader.Fields.Result      = 0;

  HeciLength = sizeof (SPS_DYNAMIC_FUSING_REQ);
  Status = SpsHeciSendMsg (
             (UINT32 *) &DynamicFusingMsg,
             HeciLength,
             SPS_HECI_CLIENT_HOST,
             SPS_HECI_CLIENT_CORE
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Send [Dynamic Fusing] request message => (%r)\n", Status));
    return Status;
  }

  ///
  /// Read Dynamic Fusing response from ME
  ///
  /// HECI Header : 0x80050007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x05
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x00008113
  ///   |- [ 7: 0] Group           = 0x13 (?)
  ///   |- [14: 8] Command         = 0x01 (?)
  ///   -- [15]    Response        = 1
  ///
  /// Byte[0] : Result
  ///
  DynamicFusingMsg.Result = 0;
  HeciLength = sizeof (SPS_DYNAMIC_FUSING_RSP);
  Status = SpsHeciReadMsg (
             BLOCKING,
             (UINT32 *) &DynamicFusingMsg,
             &HeciLength
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Read [Dynamic Fusing] response message => (%r)\n", Status));
    return Status;
  }

  if (Result != NULL) {
    *Result = DynamicFusingMsg.Result;
  }

  return Status;
}

///
/// SPS Node Manager
///
/**
  Send SPS ME NM Host configuration Request Message through HECI.

  @param[in] HostConfigureData    Host CPU configuration.

  @retval EFI_SUCCESS             Command succeeded
**/
EFI_STATUS
SpsHeciNmHostConfiguration (
  IN VOID                              *HostConfigureData
  )
{
  EFI_STATUS                           Status;
  SPS_NM_HOST_CONFIGURATION_MESSAGE    HostConfig;
  UINT32                               HostConfigMsgLength;

  if (HostConfigureData == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ZeroMem (&HostConfig, sizeof (SPS_NM_HOST_CONFIGURATION_MESSAGE));

  ///
  /// Send NM Host Configuration Request to ME
  ///
  /// HECI Header : 0x80340007
  ///   |- [ 7: 0] ME client ID    = 0x07 (SPS_HECI_CLIENT_CORE)
  ///   |- [15: 8] Host Client ID  = 0x00 (SPS_HECI_CLIENT_HOST)
  ///   |- [24:16] Length          = 0x34
  ///   -- [31]    MessageComplete = 1
  ///
  /// MKHI Header : 0x00000011
  ///   |- [ 7: 0] Group           = 0x11 (SPS_HECI_CLIENT_NM)
  ///   |- [14: 8] Command         = 0x00 (SPS_NM_CMD_PROCCFG)
  ///   -- [15]    Response        = 0
  ///
  /// Byte[ 1: 0] : Capabilities
  /// Byte[2]     : PStatesNumber
  /// Byte[3]     : TStatesNumber
  /// Byte[ 5: 4] : MaxPower
  /// Byte[ 7: 6] : MinPower
  /// Byte[8]     : ProcNumber
  /// Byte[9]     : ProcCoresNumber
  /// Byte[10]    : ProcCoresEnabled
  /// Byte[11]    : ProcThreadsEnabled
  /// Byte[19:12] : TurboRatioLimit
  /// Byte[27:20] : PlatformInfo
  /// Byte[31:28] : Altitude
  /// Byte[47:32] : PStatesRatio
  ///
  HostConfig.MkhiHeader.Fields.GroupId    = SPS_HECI_CLIENT_NM;
  HostConfig.MkhiHeader.Fields.Command    = SPS_NM_CMD_PROCCFG;
  HostConfig.MkhiHeader.Fields.IsResponse = 0;
  CopyMem (&(HostConfig.Config), HostConfigureData, sizeof (SPS_NM_HOST_CONFIGURATION_DATA));

  HostConfigMsgLength = sizeof (SPS_NM_HOST_CONFIGURATION_MESSAGE);
  Status = SpsHeciSendMsg (
             (UINT32 *) &HostConfig,
             HostConfigMsgLength,
             SPS_HECI_CLIENT_HOST,
             SPS_HECI_CLIENT_CORE
             );
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "[SPS HECI MSG LIB] ERROR: Send [NM Host Configuration] message => (%r)\n", Status));
    return Status;
  }

  return Status;
}

