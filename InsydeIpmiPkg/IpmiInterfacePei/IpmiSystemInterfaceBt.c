/** @file
 H2O IPMI BT interface implement code.

 This c file provides functions to communication with BMC using BT interface in
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
 Read BT Control Register

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.

 @retval BT Control Register value.
*/
UINT8
BtReadControl (
  IN  H2O_IPMI_PRIVATE_DATA    *IpmiData
  )
{
  return IoRead8 (IpmiData->IpmiBaseAddress + BT_REG_CTRL);
}


/**
 Write BT Control Register

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.
 @param[in]         Control             Value that will be written to Control Register.
*/
VOID
BtWriteControl (
  IN  H2O_IPMI_PRIVATE_DATA     *IpmiData,
  IN  UINT8                     Control
  )
{
  IoWrite8 (IpmiData->IpmiBaseAddress + BT_REG_CTRL, Control);
}


/**
 Read BT BMC2HOST Register

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.

 @retval Data from BMC to Host
*/
UINT8
BtReadData (
  IN  H2O_IPMI_PRIVATE_DATA    *IpmiData
  )
{
  return IoRead8 (IpmiData->IpmiBaseAddress + BT_REG_BMC2HOST);
}


/**
 Write BT HOST2BMC Register

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.
 @param[in]         Data                Data from Host to BMC.
*/
VOID
BtWriteData (
  IN  H2O_IPMI_PRIVATE_DATA     *IpmiData,
  IN  UINT8                     Data
  )
{
  IoWrite8 (IpmiData->IpmiBaseAddress + BT_REG_HOST2BMC, Data);
}


/**
 Wait BT Control Register's B_BUSY and H2B_ATN bit to be cleared.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.

 @retval EFI_SUCCESS                    B_BUSY and H2B_ATN bit are cleared in a given time.
 @retval EFI_TIMEOUT                    B_BUSY and H2B_ATN bit are not cleared in a given time.
 @retval EFI_NOT_FOUND                  Control Register is not be decode.
*/
EFI_STATUS
BtWaitWrite (
  IN  H2O_IPMI_PRIVATE_DATA             *IpmiData
  )
{
  UINT8                                 Reg;
  UINT32                                StartTick;
  UINT32                                EndTick;

  StartTick = IoRead32 (IpmiData->TimerAddress);
  Reg = 0xFF;

  while (IpmiData->TotalTimeTicks > IpmiData->DelayTimeTicks) {
    Reg = BtReadControl (IpmiData);

    if (((BT_CONTROL_REGISTER*)&Reg)->BBusy == 0 && ((BT_CONTROL_REGISTER*)&Reg)->H2bAtn == 0) {
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
 Wait BT Control Register's B2H_ATN bit to be cleared.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.
 
 @retval EFI_SUCCESS                    B2H_ATN bit is cleared in a given time.
 @retval EFI_TIMEOUT                    B2H_ATN bit is not cleared in a given time.
*/
EFI_STATUS
BtWaitRead (
  IN  H2O_IPMI_PRIVATE_DATA     *IpmiData
  )
{
  UINT8  Reg;
  UINT32 StartTick;
  UINT32 EndTick;



  StartTick = IoRead32 (IpmiData->TimerAddress);

  while (IpmiData->TotalTimeTicks > IpmiData->DelayTimeTicks) {
    Reg = BtReadControl (IpmiData);

    if (((BT_CONTROL_REGISTER*)&Reg)->B2hAtn == 1) {
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
 BT System Interface Write Processing.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA
 @param[in]         Request             H2O_IPMI_CMD_HEADER sturcture, incoude NetFnLun and Cmd
 @param[in]         Sequence            Used for matching responses up with requests.
 @param[in]         SendData            Command data buffer to be written to BMC
 @param[in]         SendLength          Command data length of command data buffer
 
 @retval EFI_SUCCESS                    Write Transaction finish successfully.
 @retval EFI_TIMEOUT                    B_BUSY and H2B_ATN bit are not cleared in a given time.
*/
EFI_STATUS
BtWriteTransaction (
  IN  H2O_IPMI_PRIVATE_DATA   *IpmiData,
  IN  H2O_IPMI_CMD_HEADER     *Request,
  IN  UINT8                   Sequence,
  IN  VOID                    *SendData OPTIONAL,
  IN  UINT8                   SendLength
  )
{
  EFI_STATUS Status;
  UINT8      RegCtrl;
  UINT8      TotalLength;
  UINT8      Count;
  UINT8      *Data;

  //
  // Wait for B_BUSY clear & H2B_ATN clear
  //
  Status = BtWaitWrite (IpmiData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Write 1 to CLR_WR_ PTR bit in BT_CNTRL
  //
  RegCtrl = BtReadControl (IpmiData);
  RegCtrl |= BT_CTRL_CLR_WR_PTR;
  BtWriteControl (IpmiData, RegCtrl);

  //
  // Start to write all data to BMC
  //
  TotalLength = BT_CMD_MIN_LEN + SendLength;
  Data = (UINT8 *)Request;
  
  BtWriteData (IpmiData, TotalLength);
  
  for  (Count = 0; Count < TotalLength; ++Count) {
    
    switch (Count) {
    //
    // NetFn/Lun
    //
    case 0:
      BtWriteData (IpmiData, *Data);
      ++Data;
      break;

    //
    // Seq
    //
    case 1:
      BtWriteData (IpmiData, Sequence);
      break;

    //
    // Command
    //
    case 2:
      BtWriteData (IpmiData, *Data);
      Data = SendData;
      break;

    //
    // Other Data
    //
    default:
      BtWriteData (IpmiData, *Data);
      ++Data;
      break;

    }
  }

  //
  // Set H2B_ATN attention
  //
  RegCtrl = BtReadControl (IpmiData);
  RegCtrl |= BT_CTRL_H2B_ATN;
  BtWriteControl (IpmiData, RegCtrl);
  

  return EFI_SUCCESS;

}


/**
 BT System Interface Read Processing.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA.
 @param[out]        Response            H2O_IPMI_CMD_HEADER sturcture, incoude NetFnLun and Cmd.
 @param[out]        Sequence            Used for matching responses up with requests.
 @param[out]        CompletionCode      Completion code of this transaction.
 @param[out]        RecvData            Data buffer to put the data read from BMC (from completion code).
 @param[out]        RecvLength          Length of Data readed from BMC.
 
 @retval EFI_SUCCESS                    Read Transaction finish successfully.
 @retval EFI_TIMEOUT                    B2H_ATN bit is not cleared in a given time.
*/
EFI_STATUS
BtReadTransaction (
  IN  H2O_IPMI_PRIVATE_DATA   *IpmiData,
  OUT H2O_IPMI_CMD_HEADER     *Response,
  OUT UINT8                   *Sequence,
  OUT UINT8                   *CompletionCode,
  OUT VOID                    *RecvData,
  OUT UINT8                   *RecvLength
  )
{
  EFI_STATUS Status;
  UINT8      RegCtrl;
  UINT8      Count;
  UINT8      TotalLength;
  UINT8      *Data;
  UINT8      TempRecvLength;
  
  //
  // Wait for B2H_ATN attention to be set
  //
  Status = BtWaitRead (IpmiData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Set H_BUSY
  //
  RegCtrl = BtReadControl (IpmiData);
  RegCtrl |= BT_CTRL_H_BUSY;
  BtWriteControl (IpmiData, RegCtrl);

  //
  // Write 1 to clear bit;
  // Clear B2H_ATN
  //
  RegCtrl = BtReadControl (IpmiData);
  RegCtrl |= BT_CTRL_B2H_ATN;
  BtWriteControl (IpmiData, RegCtrl);

  //
  // Write 1 to CLR_RD_PTR
  //
  RegCtrl = BtReadControl (IpmiData);
  RegCtrl |= BT_CTRL_CLR_RD_PTR;
  BtWriteControl (IpmiData, RegCtrl);

  //
  // Read all data
  //
  TotalLength = BtReadData (IpmiData);
  Data = (UINT8*)Response;
  TempRecvLength = 0;

  for (Count = 0; Count < TotalLength; ++Count) {

    switch (Count) {
    //
    // NetFn/Lun
    //
    case 0:
      *Data = BtReadData (IpmiData);
      ++Data;
      break;

    //
    // Seq
    //
    case 1:
      *Sequence = BtReadData (IpmiData);
      break;

    //
    // Command
    //
    case 2:
      *Data = BtReadData (IpmiData);
      Data = RecvData;
      break;

    //
    // Completion Code
    //
    case 3:
      *CompletionCode = BtReadData (IpmiData);
      break;

    //
    // All other data
    //
    default:
      *Data = BtReadData (IpmiData);
      ++Data;
      TempRecvLength++;
      break;
    }
  }

  //
  // Clear H_BUSY
  //
  RegCtrl = BtReadControl (IpmiData);
  RegCtrl &= ~BT_CTRL_H_BUSY;
  BtWriteControl (IpmiData, RegCtrl);
  *RecvLength = TempRecvLength;
  
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
 @param[out]       StatusCodes          It is no function in BT system interface

 @retval EFI_SUCCESS                    Execute command successfully.
 @retval EFI_TIMEOUT                    1. B_BUSY and H2B_ATN bit are not cleared in a given time.
                                        2. B2H_ATN bit is not cleared in a given time.
 @retval EFI_NO_MAPPING                 The request Network Function and the response Network Function
                                        does not match.
 @retval EFI_LOAD_ERROR                 Execute command successfully, but the completion code return
                                        from BMC is not 00h.
 @retval EFI_INVALID_PARAMETER          This or RecvData or RecvLength is NULL.                                        
*/
EFI_STATUS
EFIAPI
BtExecuteIpmiCmd (
  IN    H2O_IPMI_INTERFACE_PPI         *This,
  IN    H2O_IPMI_CMD_HEADER            Request,
  IN    VOID                           *SendData OPTIONAL,
  IN    UINT8                          SendLength,
  OUT   VOID                           *RecvData,
  OUT   UINT8                          *RecvLength,
  OUT   UINT16                         *StatusCodes OPTIONAL
  )
{
  UINT8                   CompletionCode;
  EFI_STATUS              Status;
  UINT8                   Sequence;
  UINT8                   SequenceResponse;
  H2O_IPMI_CMD_HEADER     Response;
  H2O_IPMI_PRIVATE_DATA   *IpmiData;


  DEBUG ((DEBUG_VERBOSE, "\n[IpmiInterfacePei] BtExecuteIpmiCmd Start:\n"));


  if (This == NULL || RecvData == NULL || RecvLength == NULL) {
    Status = EFI_INVALID_PARAMETER;
    goto Exit;
  }


  IpmiData = H2O_IPMI_PRIVATE_DATA_FROM_THIS (This);
  Sequence = IpmiData->Sequence++;
  IpmiData->DelayTimeTicks ^= IpmiData->DelayTimeTicks;


  Status = BtWriteTransaction (IpmiData, &Request, Sequence, SendData, SendLength);

  DEBUG_CODE (SystemInterfaceDebug (TRUE, &Request, SendData, SendLength, Status, 0););
  
  if (EFI_ERROR (Status)) {
    goto Exit;
  }

  CompletionCode = H2O_IPMI_COMPLETE_CODE_NO_ERROR;
  Status = BtReadTransaction (
             IpmiData,
             &Response,
             &SequenceResponse,
             &CompletionCode,
             RecvData,
             RecvLength
             );

  DEBUG_CODE (SystemInterfaceDebug (FALSE, &Response, RecvData, *RecvLength, Status, CompletionCode););

  //
  // If EFI_TIMEOUT
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
  DEBUG ((DEBUG_VERBOSE, "[IpmiInterfacePei] BtExecuteIpmiCmd End. Status: %r\n", Status));
  
  return Status;

}


/**
 Function to wait & detect corresponding register is decoded by BMC.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA struct.

 @retval EFI_SUCCESS                    Corresponding register is decoded by BMC success.
 @retval EFI_NOT_FOUND                  Corresponding register not be decoded.
*/
EFI_STATUS
BtWaitBmcDecode (
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

  Reg = BtReadControl (IpmiData);
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

    Reg = BtReadControl (IpmiData);
    if (Reg != 0xFF) {
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}


/**
 Initial function pointer of BT system interface to corrsponding function.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA
*/
VOID
InitialBtSystemInterface (
  IN  H2O_IPMI_PRIVATE_DATA   *IpmiData
  )
{
  IpmiData->IpmiPpi.ExecuteIpmiCmd = BtExecuteIpmiCmd;
  IpmiData->IpmiWaitBmcDecode = BtWaitBmcDecode;
}

