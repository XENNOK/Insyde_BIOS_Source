/** @file
 H2O IPMI SMIC interface implement code.

 This c file provides functions to communication with BMC using SMIC interface in
 PEI phase.
 
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include "IpmiInterfacePei.h"



/**
 Read SMIC Flags Register.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.
 
 @retval Value of register. 
*/
UINT8
SmicReadFlags (
  IN  H2O_IPMI_PRIVATE_DATA *IpmiData
  )
{
  return IoRead8 (IpmiData->IpmiBaseAddress + SMIC_REG_FLAGS);
}


/**
 Write SMIC Flags Register.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.
 @param[in]         Data                Data will be written to register.
*/
VOID
SmicWriteFlags (
  IN  H2O_IPMI_PRIVATE_DATA *IpmiData,
  IN  UINT8                 Data
  )
{
  IoWrite8 (IpmiData->IpmiBaseAddress + SMIC_REG_FLAGS, Data);
}


/**
 Read SMIC Status Register.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.
 
 @retval Value of register.
*/
UINT8
SmicReadStatus (
  IN  H2O_IPMI_PRIVATE_DATA *IpmiData
  )
{
  return IoRead8 (IpmiData->IpmiBaseAddress + SMIC_REG_CONTROL_STATUS);
}


/**
 Write SMIC Control Register.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.
 @param[in]         Data                Data will be written to register.
*/
VOID
SmicWriteControl (
  IN  H2O_IPMI_PRIVATE_DATA *IpmiData,
  IN  UINT8                 Data
  )
{
  IoWrite8 (IpmiData->IpmiBaseAddress + SMIC_REG_CONTROL_STATUS, Data);
}


/**
 Read SMIC Data Register.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.
 
 @retval Value of register.
*/
UINT8
SmicReadData (
  IN  H2O_IPMI_PRIVATE_DATA *IpmiData
  )
{
  return IoRead8 (IpmiData->IpmiBaseAddress + SMIC_REG_DATA);
}


/**
 Write SMIC Data Register.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.
 @param[in]         Data                Data will be written to register.
*/
VOID
SmicWriteData (
  IN  H2O_IPMI_PRIVATE_DATA *IpmiData,
  IN  UINT8                 Data
  )
{
  IoWrite8 (IpmiData->IpmiBaseAddress + SMIC_REG_DATA, Data);
}


/**
 Wait Flags Register's BUSY bit to be cleared .

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA

 @retval EFI_SUCCESS                    BUSY bit is cleared in a given time.
 @retval EFI_TIMEOUT                    BUSY bit is not cleared in a given time.
 @retval EFI_NOT_FOUND                  Flags Register is not be decode.
*/
EFI_STATUS
SmicWaitBusyBitClear (
  IN  H2O_IPMI_PRIVATE_DATA             *IpmiData
  )
{
  UINT8                                 Reg;
  UINT32                                StartTick;
  UINT32                                EndTick;

  StartTick = IoRead32 (IpmiData->TimerAddress);
  Reg = 0xFF;

  while (IpmiData->TotalTimeTicks > IpmiData->DelayTimeTicks) {
    Reg = SmicReadFlags (IpmiData);

    if (((SMIC_FLAGS_REGISTER*)&Reg)->Busy == 0) {
      return EFI_SUCCESS;
    }

    EndTick = IoRead32 (IpmiData->TimerAddress);

    if (EndTick > StartTick) {
      IpmiData->DelayTimeTicks += (EndTick - StartTick);
    } else {
      IpmiData->DelayTimeTicks += (EndTick + IpmiData->TimerMaxValue - StartTick);
    }

    StartTick = EndTick;
  }

  if (Reg == 0xFF) {
    return EFI_NOT_FOUND;
  } else {
    return EFI_TIMEOUT;
  }
}


/**
 Wait Flags Register's TX_DATA_RDY bit to be set 

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA
 
 @retval EFI_SUCCESS                    TX_DATA_RDY bit is set in a given time.
 @retval EFI_TIMEOUT                    TX_DATA_RDY bit is not set in a given time.
*/
EFI_STATUS
SmicWaitTxDataRdySet (
  IN  H2O_IPMI_PRIVATE_DATA *IpmiData
  )
{
  UINT8  Reg;
  UINT32 StartTick;
  UINT32 EndTick;


  StartTick = IoRead32 (IpmiData->TimerAddress);
  
  while (IpmiData->TotalTimeTicks > IpmiData->DelayTimeTicks) {
    Reg = SmicReadFlags (IpmiData);

    if (((SMIC_FLAGS_REGISTER*)&Reg)->TxDataRdy == 1) {
      return EFI_SUCCESS;
    }
    
    EndTick = IoRead32 (IpmiData->TimerAddress);

    if (EndTick > StartTick) {
      IpmiData->DelayTimeTicks += (EndTick - StartTick);
    } else {
      IpmiData->DelayTimeTicks += (EndTick + IpmiData->TimerMaxValue - StartTick);
    }

    StartTick = EndTick;
    
  }

  return EFI_TIMEOUT;
  
}


/**
 Wait Flags Register's RX_DATA_RDY bit to be set.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA
 
 @retval EFI_SUCCESS                    RX_DATA_RDY bit is set in a given time.
 @retval EFI_TIMEOUT                    RX_DATA_RDY bit is not set in a given time.
*/
EFI_STATUS
SmicWaitRxDataRdySet (
  IN  H2O_IPMI_PRIVATE_DATA *IpmiData
  )
{
  UINT8  Reg;
  UINT32 StartTick;
  UINT32 EndTick;


  StartTick = IoRead32 (IpmiData->TimerAddress);

  while (IpmiData->TotalTimeTicks > IpmiData->DelayTimeTicks) {
    Reg = SmicReadFlags (IpmiData);

    if (((SMIC_FLAGS_REGISTER*)&Reg)->RxDataRdy == 1) {
      return EFI_SUCCESS;
    }
    
    EndTick = IoRead32 (IpmiData->TimerAddress);

    if (EndTick > StartTick) {
      IpmiData->DelayTimeTicks += (EndTick - StartTick);
    } else {
      IpmiData->DelayTimeTicks += (EndTick + IpmiData->TimerMaxValue - StartTick);
    }

    StartTick = EndTick;
    
  }

  return EFI_TIMEOUT;
  
}


/**
 Convert SMIC error code to EFI_STATUS.

 @param[in]         ErrorCode           SMIC error code
 
 @retval EFI_SUCCESS                    No error.
 @retval EFI_ABORTED                    Unspecified error or abort.
 @retval EFI_NO_RESPONSE                Response timeout.
 @retval EFI_INVALID_PARAMETER          Illegal or unexpected control code, or illegal command.
 @retval EFI_DEVICE_ERROR               Buffer full, attemp to write too many bytes to the BMC.
*/
EFI_STATUS
SmicErrorCodeReturn (
  IN UINT8 ErrorCode
  )
{

  switch (ErrorCode) {
  case SMIC_EC_NO_ERROR:
    return EFI_SUCCESS;
    break;

  case SMIC_EC_ABORTED:
    return EFI_ABORTED;
    break;

  case SMIC_EC_NO_RESPONSE:
    return EFI_NO_RESPONSE;
    break;

  case SMIC_EC_ILLEGAL_CMD:
  case SMIC_EC_ILLEGAL_CC:
    return EFI_INVALID_PARAMETER;
    break;

  case SMIC_EC_BUFFER_FULL:
    return EFI_DEVICE_ERROR;
    break;
      
  }

  return EFI_SUCCESS;
}

/**
 SMIC System Interface Write Processing.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA
 @param[in]         Request             H2O_IPMI_CMD_HEADER sturcture, incoude NetFnLun and Cmd
 @param[in]         SendData            Command data buffer to be written to BMC
 @param[in]         SendLength          Command data length of command data buffer
 @param[out]        StatusCodes         The bit 15 of StatusCodes means this argument is valid or not:
                                        1. If bit 15 set to 1, this is a valid Status Code,
                                        and the Status Code is in low byte.
                                        2. If bit 15 set to 0, there is no Status Code
 
 @retval EFI_SUCCESS                    No error.
 @retval EFI_ABORTED                    Unspecified error or abort.
 @retval EFI_NO_RESPONSE                Response timeout.
 @retval EFI_INVALID_PARAMETER          Illegal or unexpected control code, or illegal command.
 @retval EFI_DEVICE_ERROR               Buffer full, attemp to write too many bytes to the BMC.
 @retval EFI_TIMEOUT                    BUSY bit is not cleared or TX_DATA_RDY bit is not set
                                        in a given time..
*/
EFI_STATUS
SmicWriteTransaction (
  IN  H2O_IPMI_PRIVATE_DATA   *IpmiData,
  IN  H2O_IPMI_CMD_HEADER     *Request,
  IN  VOID                    *SendData OPTIONAL,
  IN  UINT8                   SendLength,
  OUT UINT16                  *StatusCodes OPTIONAL
  )
{
  EFI_STATUS Status;
  UINT8      ControlCode;
  UINT8      StatusCode;
  UINT8      ErrorCode;
  UINT8      Round;
  UINT8      Length;
  UINT8      *Buffer;

  Length = SendLength + sizeof (H2O_IPMI_CMD_HEADER);
  Round = Length;
  ControlCode = SMIC_CC_SMS_WR_START;
  Buffer = (UINT8 *)Request;

  
  while (Round > 0)
  {
    //
    // Check BUSY BIT
    //
    Status = SmicWaitBusyBitClear (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Issue Control Code
    //
    SmicWriteControl (IpmiData, ControlCode);

    //
    // If Control Code is WR_NEXT or WR_END,
    // SMS must check TX_DATA_RDY bit
    //
    if (ControlCode != SMIC_CC_SMS_WR_START) {
      Status = SmicWaitTxDataRdySet (IpmiData);
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }

    //
    // Write Data
    //
    SmicWriteData (IpmiData, *Buffer);

    //
    // Set BUSY bit, this will make BMC to read/write data
    //
    SmicWriteFlags (IpmiData, SMIC_FLAGS_REG_BUSY_BIT);

    //
    // When BMC processing data, it sets BUSY bit,
    // and when BMC finishing processing data, the BUSY bit will be cleared.
    //
    Status = SmicWaitBusyBitClear (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Check if the opeartion success or not.
    //
    StatusCode = SmicReadStatus (IpmiData);
    
    if (StatusCode == SMIC_SC_SMS_RDY || StatusCode == SMIC_SC_SMS_WR_END) {
      ErrorCode = SmicReadData (IpmiData);
      if (StatusCodes != NULL) {
        *StatusCodes = ErrorCode | STATUS_CODE_ENABLE;
      }
      return SmicErrorCodeReturn (ErrorCode);
    }

    //
    // Move data pointer to next byte
    //
    --Round;
    if (Round == Length - sizeof (H2O_IPMI_CMD_HEADER)) {
      Buffer = (UINT8*)SendData;
    } else {
      ++Buffer;
    }

    //
    // Check if the last byte or not
    //
    if (Round == 1) {
      ControlCode = SMIC_CC_SMS_WR_END;
    } else {
      ControlCode = SMIC_CC_SMS_WR_NEXT;
    }
    
  }


  return EFI_SUCCESS;

}


/**
 SMIC System Interface Read Processing.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.
 @param[out]        Response            H2O_IPMI_CMD_HEADER sturcture, incoude NetFnLun and Cmd
 @param[out]        CompletionCode      Completion code of this transaction
 @param[out]        RecvData            Data buffer to put the data read from BMC (from completion code)
 @param[out]        RecvLength          Length of Data readed from BMC
 @param[out]        StatusCodes         The bit 15 of StatusCodes means this argument is valid or not:
                                        1. If bit 15 set to 1, this is a valid Status Code,
                                        and the Status Code is in low byte.
                                        2. If bit 15 set to 0, there is no Status Code
 
 @retval EFI_SUCCESS                    Read Transaction finish successfully.
 @retval EFI_ABORTED                    Unspecified error or abort.
 @retval EFI_TIMEOUT                    BUSY bit is not cleared or RX_DATA_RDY bit is not set
                                        in a given time.
 @retval EFI_NO_RESPONSE                Response timeout.
 @retval EFI_INVALID_PARAMETER          Illegal or unexpected control code, or illegal command.
 @retval EFI_DEVICE_ERROR               Buffer full, attemp to write too many bytes to the BMC.
*/
EFI_STATUS
SmicReadTransaction (
  IN  H2O_IPMI_PRIVATE_DATA   *IpmiData,
  OUT H2O_IPMI_CMD_HEADER     *Response,
  OUT UINT8                   *CompletionCode,
  OUT VOID                    *RecvData,
  OUT UINT8                   *RecvLength,
  OUT UINT16                  *StatusCodes OPTIONAL
  )
{
  EFI_STATUS Status;
  BOOLEAN    Done;
  UINT8      ControlCode;
  UINT8      StatusCode;
  UINT8      ErrorCode;
  UINT8      RecvByte;
  UINT8      *Buffer;

  RecvByte = 0;
  ControlCode = SMIC_CC_SMS_RD_START;
  Buffer = (UINT8 *)Response;
  Done = FALSE;

  
  while (!Done)
  {
    //
    // Check BUSY BIT
    //
    Status = SmicWaitBusyBitClear (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Issue Control Code
    //
    SmicWriteControl (IpmiData, ControlCode);

    //
    // SMS must check RX_DATA_RDY bit when reading from BMC
    //

    Status = SmicWaitRxDataRdySet (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Set BUSY bit, this will make BMC to read/write data
    //
    SmicWriteFlags (IpmiData, SMIC_FLAGS_REG_BUSY_BIT);

    //
    // When BMC processing data, it sets BUSY bit,
    // and when BMC finishing processing data, the BUSY bit will be cleared.
    //
    Status = SmicWaitBusyBitClear (IpmiData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Check if the opeartion success or not.
    //
    StatusCode = SmicReadStatus (IpmiData);

    switch (StatusCode) {
    case SMIC_SC_SMS_RDY:
      ErrorCode = SmicReadData (IpmiData);
      
      if (StatusCodes != NULL) {
        *StatusCodes = ErrorCode | STATUS_CODE_ENABLE;
      }
      return SmicErrorCodeReturn (ErrorCode);
      break;

    default:
      *Buffer = SmicReadData (IpmiData);
      ++RecvByte;
      ControlCode = SMIC_CC_SMS_RD_NEXT;

      if (RecvByte == 2) {
        Buffer = CompletionCode;
      } else if (RecvByte == 3) {
        Buffer = RecvData;
      } else {
        ++Buffer;
      }

      if (StatusCode == SMIC_SC_SMS_RD_END) {
        Done = TRUE;
      }
      break;

    }
    
  }

  *RecvLength = RecvByte - sizeof(H2O_IPMI_CMD_HEADER) - 1;


  return EFI_SUCCESS;

}


/**
 Send request, include Network Function, LUN, and command of IPMI, with/without
 additional data to BMC.

 @param[in]        This                 A pointer to H2O_IPMI_INTERFACE_PPI structure.
 @param[in]        Request              H2O_IPMI_CMD_HEADER structure, storing Network Function, LUN
                                        and various IPMI command, to send to BMC.
 @param[in]        SendData             Optional arguments, if an IPMI command is required to
                                        send with data, this argument is a pointer to the data buffer.
                                        If no data is required, set this argument as NULL.
 @param[in]        SendLength           When sending command with data, this argument is the length of the data,
                                        otherwise set this argument as 0.
 @param[out]       RecvData             Data buffer to put the data read from BMC.
 @param[out]       RecvLength           Length of Data readed from BMC.
 @param[out]       StatusCodes          The bit 15 of StatusCodes means this argument is valid or not:
                                        1. If bit 15 set to 1, this is a valid Status Code,
                                        and the Status Code is in low byte.
                                        2. If bit 15 set to 0, there is no Status Code

 @retval EFI_SUCCESS                    Execute command successfully.
 @retval EFI_ABORTED                    Unspecified error or abort.
 @retval EFI_TIMEOUT                    1. BUSY bit is not cleared or TX_DATA_RDY bit is not set in a given time.
                                        2. BUSY bit is not cleared or RX_DATA_RDY bit is not set in a given time.
 @retval EFI_DEVICE_ERROR               Buffer full, attemp to write too many bytes to the BMC.
 @retval EFI_NO_MAPPING                 The request Network Function and the response Network Function
                                        does not match.
 @retval EFI_LOAD_ERROR                 Execute command successfully, but the completion code return
                                        from BMC is not 00h.
 @retval EFI_INVALID_PARAMETER          1. Illegal or unexpected control code, or illegal command.
                                        2. This or RecvData or RecvLength is NULL.                                       
*/
EFI_STATUS
EFIAPI
SmicExecuteIpmiCmd (
  IN    H2O_IPMI_INTERFACE_PPI        *This,
  IN    H2O_IPMI_CMD_HEADER           Request,
  IN    VOID                          *SendData OPTIONAL,
  IN    UINT8                         SendLength,
  OUT   VOID                          *RecvData,
  OUT   UINT8                         *RecvLength,
  OUT   UINT16                        *StatusCodes OPTIONAL
  )
{
  UINT8                    CompletionCode;
  EFI_STATUS               Status;
  H2O_IPMI_CMD_HEADER      Response;
  H2O_IPMI_PRIVATE_DATA    *IpmiData;


  DEBUG ((DEBUG_VERBOSE, "\n[IpmiInterfacePei] SmicExecuteIpmiCmd Start:\n"));

  if (This == NULL || RecvData == NULL || RecvLength == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }

  
  if (StatusCodes != NULL) {
    *StatusCodes = 0;
  }

  CompletionCode = H2O_IPMI_COMPLETE_CODE_NO_ERROR;
  IpmiData = H2O_IPMI_PRIVATE_DATA_FROM_THIS (This);
  IpmiData->DelayTimeTicks ^= IpmiData->DelayTimeTicks;
  
  Status = SmicWriteTransaction (IpmiData, &Request, SendData, SendLength, StatusCodes);

  DEBUG_CODE (SystemInterfaceDebug (TRUE, &Request, SendData, SendLength, Status, 0););
  
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  Status = SmicReadTransaction (
             IpmiData,
             &Response,
             &CompletionCode,
             RecvData,
             RecvLength,
             StatusCodes
             );

  DEBUG_CODE (SystemInterfaceDebug (FALSE, &Response, RecvData, *RecvLength, Status, CompletionCode););

  //
  // If EFI_ABORTED, EFI_TIMEOUT or EFI_DEVICE_ERROR
  //
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  //
  // The transaction complete, but CompletionCode is not 0x00
  //
  if (CompletionCode != H2O_IPMI_COMPLETE_CODE_NO_ERROR) {
    *((UINT8*)RecvData) = CompletionCode;
    *RecvLength = 1;

    Status = EFI_LOAD_ERROR;
    goto Exit;
  }
  
  //
  // Request Netfn and Response Netfn does not match
  //
  if (Response.NetFn != (Request.NetFn | H2O_IPMI_RESPONSE_NETFN_BIT)) {
    Status = EFI_NO_MAPPING;
    goto Exit;
  }

  Status = EFI_SUCCESS;

Exit:
  DEBUG ((DEBUG_VERBOSE, "[IpmiInterfacePei] SmicExecuteIpmiCmd End. Status: %r\n", Status));
  
  return Status;

}


/**
 Function to wait & detect corresponding register is decoded by BMC.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA struct.

 @retval EFI_SUCCESS                    Corresponding register is decoded by BMC success.
 @retval EFI_NOT_FOUND                  Corresponding register not be decoded.
*/
EFI_STATUS
SmicWaitBmcDecode (
  IN  H2O_IPMI_PRIVATE_DATA             *IpmiData
  )
{
  UINT8                                 Retry;
  UINT8                                 Interval;
  UINT8                                 Index;
  UINTN                                 TotalTimeTicks;
  UINTN                                 DelayTimeTicks;
  UINT8                                 Reg;
  UINT32                                StartTick;
  UINT32                                EndTick;

  Reg = SmicReadFlags (IpmiData);
  //
  // Detect status register is decoded by BMC or not (should not be 0xFF).
  //
  if (Reg != 0xFF) {
    return EFI_SUCCESS;
  }

  Retry = FixedPcdGet8 (PcdIpmiWaitBmcDecodeRetry);
  Interval = FixedPcdGet8 (PcdIpmiWaitBmcDecodeInterval);
  TotalTimeTicks = Interval * 1000000 * TICK_OF_TEN_MS / 10;

  for (Index = 0; Index < Retry; Index++) {

    StartTick = IoRead32 (IpmiData->TimerAddress);
    DelayTimeTicks = 0;

    while (TotalTimeTicks > DelayTimeTicks) {
      EndTick = IoRead32 (IpmiData->TimerAddress);

      if (EndTick > StartTick) {
        DelayTimeTicks += (EndTick - StartTick);
      } else {
        DelayTimeTicks += (EndTick + IpmiData->TimerMaxValue - StartTick);
      }

      StartTick = EndTick;
    }

    Reg = SmicReadFlags (IpmiData);
    if (Reg != 0xFF) {
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}


/**
 Initial function pointer of SMIC system interface to corrsponding function.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA 
*/
VOID
InitialSmicSystemInterface (
  IN  H2O_IPMI_PRIVATE_DATA *IpmiData
  )
{
  IpmiData->IpmiPpi.ExecuteIpmiCmd = SmicExecuteIpmiCmd;
  IpmiData->IpmiWaitBmcDecode = SmicWaitBmcDecode;
}

