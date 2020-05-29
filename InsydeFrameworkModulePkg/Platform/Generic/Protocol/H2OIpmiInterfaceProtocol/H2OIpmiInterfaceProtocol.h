//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*--
Module Name:

  H2OIpmiInterfaceProtocol.h

Abstract:

  H2O IPMI Interface Protocol definition file

--*/

#ifndef _H2O_IPMI_INTERFACE_PROTOCOL_H_
#define _H2O_IPMI_INTERFACE_PROTOCOL_H_

#include "Tiano.h"
#include EFI_GUID_DEFINITION (H2OIpmiHob)

EFI_FORWARD_DECLARATION (H2O_IPMI_INTERFACE_PROTOCOL);
//
// Variable Guid
//
#define H2O_IPMI_INTERFACE_PROTOCOL_GUID \
  { 0xbb46188c, 0x7157, 0x4fec, 0xa1, 0xd8, 0xc0, 0xbb, 0xfc, 0xe0, 0x3b, 0xc7}

typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_EXECUTE_IPMI_CMD) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL   *This,
  IN  H2O_IPMI_CMD_HEADER            Request,
  IN  VOID                          *SendData OPTIONAL,
  IN  UINT8                          SendLength,
  OUT VOID                          *RecvData,
  OUT UINT8                         *RecvLength,
  OUT UINT16                        *StatusCodes OPTIONAL
);
/*++

Routine Description:

  Send request, include Network Function, LUN, and command of IPMI,
  with/without additional data to BMC.

Arguments:

   This        - Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance
   
   Request     - H2O_IPMI_CMD_HEADER structure, storing Network Function, LUN
                 and various IPMI command, to send to BMC.
                 
   SendData    - Optional arguments, if an IPMI command is required to
                 send with data, this argument is a pointer to the data buffer.
                 If no data is required, set this argument as NULL.
                 
   SendLength  - When sending command with data, this argument is the length of the data,
                 otherwise set this argument as 0.
                 
   RecvData    - Data buffer to put the data read from BMC.
   
   RecvLength  - Length of Data readed from BMC.
   
   StatusCodes - The bit 15 of StatusCodes means this argument is valid or not:
 	               1. If bit 15 set to 1, this is a valid Status Code,
 	                  and the Status Code is in low byte.
 	               2. If bit 15 set to 0, there is no Status Code
 	               
                 If the system interface type is KCS, StatusCodes is valid 
                 when return value is EFI_ ABORTED. If the system interface type
                 is KCS and the return value is EFI_DEVICE_ERROR,it does not 
                 guarantee StatusCodes is valid, the caller must check bit 15.

Returns:

  EFI_SUCCESS      - Execute command successfully.
  
  EFI_ABORTED      - If the system interface type is KCS, this will happen when:
                     1.	When writing to BMC, BMC cannot enter "Write State",
                        the error processing make BMC to enter "Idle State" successfully.
                     2.	When finishing reading from BMC, BMC cannot enter "Idle State",
                        the error processing make BMC to enter "Idle State" successfully.

  EFI_TIMEOUT      - 1.	Wait BMC output buffer full timeout.
                     2.	Wait BMC input buffer empty timeout.
                     
  EFI_DEVICE_ERROR - If the system interface type is KCS, this will happen when:
                     1.	When writing to BMC, BMC cannot enter "Write State",
                        the error processing cannot make BMC to enter "Idle State".
                     2.	When finishing reading from BMC, BMC cannot enter "Idle State",
                        the error processing cannot make BMC to enter "Idle State".

  EFI_NO_MAPPING   - The request Network Function and the response Network Function
                     does not match.
                     
  EFI_LOAD_ERROR   - Execute command successfully, but the completion code return
                     from BMC is not 00h.
  
--*/

typedef
H2O_IPMI_INTERFACE_TYPE
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_INTERFACE_TYPE) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL   *This
);
/*++

Routine Description:

  Return system interface type that BMC currently use.

Arguments:

   This        - Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance

Returns:

   H2O_IPMI_KCS - KCS System Interface Type
  
--*/

typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_INTERFACE_NAME) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL   *This,
  OUT CHAR16                        *InterfaceName
);
/*++

Routine Description:

  Return system interface name that BMC currently use.

Arguments:

   This          - Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance
   
   InterfaceName - The buffer storing the name string. It is the caller's
                   responsibility to provide enough space to store the name string.
                   The longest name is five CHAR16, that is, four for interface
                   name string and one for NULL character.

Returns:

   EFI_INVALID_PARAMETER - InterfaceName is NULL.

   EFI_SUCCESS           - Return interface name string successfully.
  
--*/

typedef
UINT16
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL    *This,
  IN  UINT8                          Index
);
/*++

Routine Description:

  Return system interface base address that BMC currently use.

Arguments:

   This          - Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance
   Index         - Type of base address, one of three types:
                   "H2O_IPMI_OS_BASE_ADDRESS_INDEX"
                   "H2O_IPMI_SMM_BASE_ADDRESS_INDEX"
                   "H2O_IPMI_FW_BASE_ADDRESS_INDEX"

Returns:

   The base address that BMC current using
  
--*/

typedef
UINT16
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS_OFFSET) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL    *This,
  IN  UINT8                          Index
);
/*++

Routine Description:

  Return system interface offset that BMC currently use.

Arguments:

   This          - Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance
   Index         - Type of base address, one of three types:
                   "H2O_IPMI_OS_BASE_ADDRESS_INDEX"
                   "H2O_IPMI_SMM_BASE_ADDRESS_INDEX"
                   "H2O_IPMI_FW_BASE_ADDRESS_INDEX"

Returns:

   The base address offset that BMC current using
  
--*/

typedef
H2O_IPMI_INTERFACE_ADDRESS_TYPE
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS_TYPE) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL      *This
);
/*++

Routine Description:

  Return system interface type that BMC currently use.

Arguments:

   This          - Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance

Returns:

   MEMORY - The base address is in memory space.

   IO     - The base address is in I/O space.
  
--*/

typedef
UINT8
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_VERSION) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL      *This
);
/*++

Routine Description:

  Return IPMI version that BMC current supports.

Arguments:

   This          - Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance

Returns:

   Upper four bits are msb, lower four bits are lsb. For example, if
   IPMI version is 2.0, this function return 0x20h.
  
--*/

typedef
UINT16
(EFIAPI *H2O_IPMI_INTERFACE_PROTOCOL_GET_FIRMWARE_VERSION) (
  IN  H2O_IPMI_INTERFACE_PROTOCOL      *This
);
/*++

Routine Description:

  Return BMC firmware version

Arguments:

   This          - Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance

Returns:

   Major revision is in high byte, minor revision is in low byte.
  
--*/

typedef struct _H2O_IPMI_INTERFACE_PROTOCOL {
  H2O_IPMI_INTERFACE_PROTOCOL_EXECUTE_IPMI_CMD             ExecuteIpmiCmd;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_INTERFACE_TYPE      GetIpmiInterfaceType;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_INTERFACE_NAME      GetIpmiInterfaceName;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS        GetIpmiBaseAddress;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS_TYPE   GetIpmiBaseAddressType;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_VERSION             GetIpmiVersion;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_FIRMWARE_VERSION         GetFirmwareVersion;
  H2O_IPMI_INTERFACE_PROTOCOL_GET_IPMI_BASE_ADDRESS_OFFSET GetIpmiBaseAddressOffset;
} H2O_IPMI_INTERFACE_PROTOCOL;

extern EFI_GUID gH2OIpmiInterfaceProtocolGuid;

#endif
