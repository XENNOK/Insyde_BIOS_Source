/** @file
 H2O IPMI Interface library implement code.

 This c file contains H2O IPMI interface library instance for PEI phase.

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


#include <Library/PeiServicesLib.h>
#include <Library/IpmiInterfaceLib.h>
#include <Ppi/H2OIpmiInterfacePpi.h>


/**
 Send request, include Network Function, LUN, and command of IPMI, with/without
 additional data to BMC.

 @param[in]        NetFn                Network Function of the IPMI command want to send.
 @param[in]        Cmd                  Command code of the IPMI command want to send.
 @param[in]        SendData             Optional arguments, if an IPMI command is required to
                                        send with data, this argument is a pointer to the data buffer.
                                        If no data is required, set this argument as NULL.
 @param[in]        SendLength           When sending command with data, this argument is the length of the data,
                                        otherwise set this argument as 0.
 @param[out]       RecvData             Data buffer to put the data read from BMC.
 @param[out]       RecvLength           Length of Data readed from BMC.

 @retval EFI_SUCCESS                    Execute command successfully.
 @retval EFI_NO_MAPPING                 The request Network Function and the response Network Function
                                        does not match.
 @retval EFI_LOAD_ERROR                 Execute command successfully, but the completion code return
                                        from BMC is not 00h.
 @retval EFI_ABORTED                    For KCS system interface:
                                        1. When writing to BMC, BMC cannot enter "Write State",
                                        the error processing make BMC to enter "Idle State" successfully.
                                        2. When finishing reading from BMC, BMC cannot enter "Idle State",
                                        the error processing make BMC to enter "Idle State" successfully.
                                        For SMIC system interface: Unspecified error or abort.
                                        For BT system interface: It won't have this error.
 @retval EFI_TIMEOUT                    For KCS system interface:
                                        1. Output buffer is not full in a given time.
                                        2. Iutput buffer is not empty in a given time.
                                        For SMIC system interface:
                                        1. BUSY bit is not cleared or TX_DATA_RDY bit is not set in a given time.
                                        2. BUSY bit is not cleared or RX_DATA_RDY bit is not set in a given time.
                                        For BT system interface:
                                        1. B_BUSY and H2B_ATN bit are not cleared in a given time.
                                        2. B2H_ATN bit is not cleared in a given time.
 @retval EFI_DEVICE_ERROR               For KCS system interface:
                                        1. When writing to BMC, BMC cannot enter "Write State",
                                        the error processing cannot make BMC to enter "Idle State".
                                        2. When finishing reading from BMC, BMC cannot enter "Idle State",
                                        the error processing cannot make BMC to enter "Idle State".
                                        For SMIC system interface:
                                        Buffer full, attemp to write too many bytes to the BMC.
                                        For BT system interface: It won't have this error.
 @retval EFI_INVALID_PARAMETER          For all system interfaces:
                                        This or RecvData or RecvLength is NULL.
                                        For SMIC system interface:
                                        Illegal or unexpected control code, or illegal command.
*/
EFI_STATUS
EFIAPI
IpmiLibExecuteIpmiCmd (
  IN  UINT8                             NetFn,
  IN  UINT8                             Cmd,
  IN  VOID                              *SendData OPTIONAL,
  IN  UINT8                             SendLength,
  OUT VOID                              *RecvData,
  OUT UINT8                             *RecvLength
  )
{
  EFI_STATUS                            Status;
  H2O_IPMI_CMD_HEADER                   Request;
  H2O_IPMI_INTERFACE_PPI                *IpmiPpi;


  Status = PeiServicesLocatePpi (&gH2OIpmiInterfacePpiGuid, 0, NULL, (VOID **)&IpmiPpi);
  if (EFI_ERROR (Status)) {
    return Status;
  }


  Request.NetFn = NetFn;
  Request.Cmd = Cmd;
  Request.Lun = H2O_IPMI_BMC_LUN;

  Status = IpmiPpi->ExecuteIpmiCmd (
                      IpmiPpi,
                      Request,
                      SendData,
                      SendLength,
                      RecvData,
                      RecvLength,
                      NULL
                      );

  return Status;

}

