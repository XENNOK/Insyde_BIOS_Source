/** @file
 H2O IPMI Interface Protocol header file.

 H2O IPMI Interface Protocol provides an interface to send IPMI command to BMC
 in DXE and SMM environment.

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

#ifndef _H2O_IPMI_INTERFACE_PROTOCOL_H_
#define _H2O_IPMI_INTERFACE_PROTOCOL_H_


#include <H2OIpmi.h>


//
// Protocol Guid
//
#define H2O_IPMI_INTERFACE_PROTOCOL_GUID \
  {0xbb46188c, 0x7157, 0x4fec, 0xa1, 0xd8, 0xc0, 0xbb, 0xfc, 0xe0, 0x3b, 0xc7}

#define H2O_SMM_IPMI_INTERFACE_PROTOCOL_GUID \
  {0xFF2184AF, 0x1367, 0x477a, 0x84, 0x7B, 0xFF, 0x09, 0x65, 0x92, 0x6C, 0xFC}

typedef struct _H2O_IPMI_INTERFACE_PROTOCOL H2O_IPMI_INTERFACE_PROTOCOL;


/**
 Send request, include Network Function, LUN, and command of IPMI, with/without
 additional data to BMC.

 @param[in]         This                A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         Request             H2O_IPMI_CMD_HEADER structure, storing Network Function, LUN
                                        and various IPMI command, to send to BMC.
 @param[in]         SendData            Optional arguments, if an IPMI command is required to
                                        send with data, this argument is a pointer to the data buffer.
                                        If no data is required, set this argument as NULL.
 @param[in]         SendLength          When sending command with data, this argument is the length of the data,
                                        otherwise set this argument as 0.
 @param[out]        RecvData            Data buffer to put the data read from BMC.
 @param[out]        RecvLength          Length of Data readed from BMC.
 @param[out]        StatusCodes         The bit 15 of StatusCodes means this argument is valid or not:
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
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_EXECUTE_IPMI_CMD) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This,
  IN  H2O_IPMI_CMD_HEADER               Request,
  IN  VOID                              *SendData OPTIONAL,
  IN  UINT8                             SendLength,
  OUT VOID                              *RecvData,
  OUT UINT8                             *RecvLength,
  OUT UINT16                            *StatusCodes OPTIONAL
  );


/**
 Return system interface type that BMC currently use.

 @param[in]         This                A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.

 @retval H2O_IPMI_KCS                   If current system interface type is KCS.
 @retval H2O_IPMI_SMIC                  If current system interface type is SMIC.
 @retval H2O_IPMI_BT                    If current system interface type is BT.
*/
typedef
H2O_IPMI_INTERFACE_TYPE
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_INTERFACE_TYPE) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This
  );


/**
 Return system interface name that BMC currently use.

 @param[in]         This                A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[out]        InterfaceName       The buffer storing the name string. It is the caller's
                                        responsibility to provide enough space to store the name string.
                                        The longest name is five CHAR16, that is, four for interface
                                        name string and one for NULL character.

 @retval EFI_INVALID_PARAMETER          InterfaceName is NULL.
 @retval EFI_SUCCESS                    Return interface name string successfully.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_INTERFACE_NAME) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This,
  OUT CHAR16                            *InterfaceName
  );


/**
 Return system interface address that BMC currently use.

 @param[in]         This                A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         Index               Type of base address, one of three types:
                                        "H2O_IPMI_OS_BASE_ADDRESS_INDEX"
                                        "H2O_IPMI_SMM_BASE_ADDRESS_INDEX"
                                        "H2O_IPMI_FW_BASE_ADDRESS_INDEX"

 @retval The base address that BMC current use.
*/
typedef
UINT16
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This,
  IN  UINT8                             Index
  );


/**
 Return system interface offset that BMC currently use.

 @param[in]         This                A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.
 @param[in]         Index               Type of base address, one of three types:
                                        "H2O_IPMI_OS_BASE_ADDRESS_INDEX"
                                        "H2O_IPMI_SMM_BASE_ADDRESS_INDEX"
                                        "H2O_IPMI_FW_BASE_ADDRESS_INDEX"

 @retval The base address offset that BMC current use.
*/
typedef
UINT16
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS_OFFSET) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This,
  IN  UINT8                             Index
  );


/**
 Return base address type that BMC currently use.

 @param[in]         This                A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.

 @retval H2O_IPMI_MEMORY                The base address is in memory space.
 @retval H2O_IPMI_IO                    The base address is in I/O space.
*/
typedef
H2O_IPMI_INTERFACE_ADDRESS_TYPE
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS_TYPE) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This
  );


/**
 Return IPMI version that BMC current supports.

 @param[in]         This                A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.

 @retval IPMI version. Upper four bits are msb, lower four bits are lsb. For example, if
         IPMI version is 2.0, this function return 0x20h.
*/
typedef
UINT8
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_VERSION) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This
  );


/**
 Return BMC firmware version.

 @param[in]         This                A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.

 @retval Major revision is in high byte, minor revision is in low byte.
*/
typedef
UINT16
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_BMC_FIRMWARE_VERSION) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This
  );


/**
 Return BMC status.

 @param[in]         This                A pointer to H2O_IPMI_INTERFACE_PROTOCOL structure.

 @retval The enum H2O_IPMI_BMC_STATUS of BMC current status.
*/
typedef
H2O_IPMI_BMC_STATUS
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_BMC_STATUS) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL       *This
  );


//
// Structure of H2O_IPMI_INTERFACE_PROTOCOL
//
struct _H2O_IPMI_INTERFACE_PROTOCOL {
  H2O_IPMI_INTERFACE_PROTOCOL_EXECUTE_IPMI_CMD             ExecuteIpmiCmd;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_INTERFACE_TYPE      GetIpmiInterfaceType;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_INTERFACE_NAME      GetIpmiInterfaceName;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS        GetIpmiBaseAddress;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS_TYPE   GetIpmiBaseAddressType;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_VERSION             GetIpmiVersion;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_BMC_FIRMWARE_VERSION     GetBmcFirmwareVersion;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS_OFFSET GetIpmiBaseAddressOffset;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_BMC_STATUS               GetBmcStatus;
};


extern EFI_GUID gH2OIpmiInterfaceProtocolGuid;
extern EFI_GUID gH2OSmmIpmiInterfaceProtocolGuid;


#endif

