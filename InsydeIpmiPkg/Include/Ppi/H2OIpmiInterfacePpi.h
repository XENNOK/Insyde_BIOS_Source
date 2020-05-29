/** @file
 H2O IPMI Interface PPI header file.

 H2O IPMI Interface PPI can be used to send IPMI command to BMC in PEI phase.

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

#ifndef _H2O_IPMI_INTERFACE_PPI_H_
#define _H2O_IPMI_INTERFACE_PPI_H_


#include <H2OIpmi.h>


#define H2O_IPMI_INTERFACE_PPI_GUID \
  {0xd7c7d1fe, 0x2548, 0x46e0, 0x96, 0xf9, 0x82, 0xb1, 0xe4, 0x1a, 0xfd, 0x4d}

typedef struct _H2O_IPMI_INTERFACE_PPI H2O_IPMI_INTERFACE_PPI;


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
                                        For KCS system interface:
                                        StatusCodes is valid when return value is EFI_ABORTED. If the return
                                        value is EFI_DEVICE_ERROR,it does not guarantee StatusCodes is valid,
                                        the caller must check bit 15.
                                        For SMIC system interface: It is always valid.
                                        For BT system interface: It is no function.

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
typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_INTERFACE_PPI_EXECUTE_IPMI_CMD) (
  IN  H2O_IPMI_INTERFACE_PPI            *This,
  IN  H2O_IPMI_CMD_HEADER               Request,
  IN  VOID                              *SendData OPTIONAL,
  IN  UINT8                             SendLength,
  OUT VOID                              *RecvData,
  OUT UINT8                             *RecvLength,
  OUT UINT16                            *StatusCodes OPTIONAL
  );


//
// PPI data structure
//
struct _H2O_IPMI_INTERFACE_PPI {
  H2O_IPMI_INTERFACE_PPI_EXECUTE_IPMI_CMD ExecuteIpmiCmd;
};


extern EFI_GUID gH2OIpmiInterfacePpiGuid;


#endif 

