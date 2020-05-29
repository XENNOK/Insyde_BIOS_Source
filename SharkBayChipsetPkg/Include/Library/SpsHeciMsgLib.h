/** @file

  Header file for Heci Message functionality
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

#ifndef _SPS_HECI_MESSAGE_LIB_H_
#define _SPS_HECI_MESSAGE_LIB_H_

/**
  Initialize SPS ME HECI library.

  @param None.

**/
EFI_STATUS
SpsHeciMsgLibInit (
  VOID
  );

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
  );

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
  );

/**
  Send ME-BIOS Interface Version Request Message through HECI.

  @param[in]  PeiServices         General purpose services available to every PEIM.
  @param[out] Resul               ME response after Dynamic Fusing Request

  @retval EFI_SUCCESS             Command succeeded
**/
EFI_STATUS
SpsHeciGetMeBiosInterfaceVersion (
  OUT VOID                             *MeBiosInterfVer
  );

/**
  Sends a message to ME to lock a specified SPI Flash region for writing and receiving a response message.

  @param[out] Nonce               Random number generated by Ignition ME FW. When BIOS
                                  want to unlock region it should use this value
                                  in HMRFPO_ENABLE Request Message
  @param[out] FactoryDefaultBase  The base of the factory default calculated from the start of the ME region.
                                  BIOS sets a Protected Range (PR) register!�s "Protected Range Base" field with this value
                                  + the base address of the region.
  @param[out] FactoryDefaultLimit The length of the factory image.
                                  BIOS sets a Protected Range (PR) register!�s "Protected Range Limit" field with this value
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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

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
  );

/**
  Send CPU Dynamic Fusing Request Message through HECI.

  @param[out] Resul               ME response after Dynamic Fusing Request

  @retval EFI_SUCCESS             Command succeeded
**/
EFI_STATUS
SpsHeciSendDynamicFusing (
  OUT UINT8                            *Result
  );

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
  );

#endif