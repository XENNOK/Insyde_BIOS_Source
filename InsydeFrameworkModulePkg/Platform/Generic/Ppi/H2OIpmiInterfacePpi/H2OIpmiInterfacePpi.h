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

  H2OIpmiInterfacePpi.h

Abstract:

  H2O IPMI Interface Ppi Definition

--*/

#ifndef _H2O_IPMI_INTERFACE_PPI_H_
#define _H2O_IPMI_INTERFACE_PPI_H_
#include EFI_GUID_DEFINITION (H2OIpmiHob)

EFI_FORWARD_DECLARATION (H2O_IPMI_INTERFACE_PPI);


// {D7C7D1FE-2548-46e0-96F9-82B1E41AFD4D}
#define H2O_IPMI_INTERFACE_PPI_GUID \
  {0xd7c7d1fe, 0x2548, 0x46e0, 0x96, 0xf9, 0x82, 0xb1, 0xe4, 0x1a, 0xfd, 0x4d}



//
// Functions
//
typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_INTERFACE_PPI_EXECUTE_IPMI_CMD) (
  IN  EFI_PEI_SERVICES           **PeiServices,
  IN  H2O_IPMI_INTERFACE_PPI      *This,
  IN  H2O_IPMI_CMD_HEADER          Request,
  IN  VOID                        *SendData OPTIONAL,
  IN  UINT8                        SendLength,
  OUT VOID                        *RecvData,
  OUT UINT8                       *RecvLength,
  OUT UINT16                      *StatusCodes OPTIONAL
);
/*++

Routine Description:

  Send request, include Network Function, LUN, and command of IPMI,
  with/without additional data to BMC.

Arguments:

   PeiServices - A pointer to PEI Services.
   
   This        - A pointer to H2O_IPMI_INTERFACE_PPI structure.
   
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


//
// PPI data structure
//
typedef struct _H2O_IPMI_INTERFACE_PPI {
  H2O_IPMI_INTERFACE_PPI_EXECUTE_IPMI_CMD ExecuteIpmiCmd;
} H2O_IPMI_INTERFACE_PPI;

extern EFI_GUID gH2OIpmiInterfacePpiGuid;

#endif 

