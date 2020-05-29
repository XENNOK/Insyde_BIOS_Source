/** @file
 H2O IPMI Interface DXE/SMM common header file.

 This header file contains common definitions and structures for DXE/SMM module.

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

#ifndef _IPMI_INTERFACE_COMMON_H_
#define _IPMI_INTERFACE_COMMON_H_


#include <Guid/HobList.h>

#include <Protocol/H2OIpmiInterfaceProtocol.h>

#include <Library/IoLib.h>
#include <Library/HobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiLib.h>
#include <Library/IpmiHobLib.h>
#include <Library/DebugLib.h>


extern H2O_IPMI_INTERFACE_ADDRESS  mAllBaseAddressTable[H2O_IPMI_MAX_BASE_ADDRESS_NUM];


//
// Private data signature
//
#define H2O_IPMI_PRIVATE_DATA_SIGNATURE       SIGNATURE_32 ('h','d','i','m')
#define H2O_IPMI_PRIVATE_DATA_FROM_THIS(a)    BASE_CR(a, H2O_IPMI_PRIVATE_DATA, H2OIpmiProtocol)

//
// Private data structure
//
typedef struct {
  UINTN                                  Signature;
  H2O_IPMI_INTERFACE_PROTOCOL            H2OIpmiProtocol;
  EFI_HANDLE                             ProtocolHandle;
  UINT16                                 IpmiBaseAddress;
  UINT16                                 IpmiBaseAddressOffset;
  H2O_IPMI_INTERFACE_ADDRESS_TYPE        IpmiAddressType;
  UINT16                                 BmcFirmwareVersion;
  UINT8                                  IpmiVersion;
  H2O_IPMI_BMC_STATUS                    BmcStatus;
  UINTN                                  TotalTimeTicks;
  UINTN                                  DelayTimeTicks;
  UINT8                                  Sequence;
  UINT16                                 TimerAddress;
  UINT32                                 TimerMaxValue;
  UINT8                                  RetryAfterIdle;
} H2O_IPMI_PRIVATE_DATA ;


/**
 Fill private data content

 @param[in]         IpmiHob             A pointer to H2O IPMI Hob
 @param[in]         InSmm               In SMM or not. It is possible to use different base address in SMM.
 @param[out]        IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA
*/
VOID
InitialIpmiInterfaceContent (
  IN  H2O_IPMI_HOB                      *IpmiHob,
  IN  BOOLEAN                           InSmm,
  OUT H2O_IPMI_PRIVATE_DATA             *IpmiData
  );


/**
 Initial function pointer of BT system interface to corrsponding function.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA
*/
VOID
InitialBtSystemInterface (
  IN  H2O_IPMI_PRIVATE_DATA             *IpmiData
  );


/**
 Initial function pointer of KCS system interface to corrsponding function.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA
*/
VOID
InitialKcsSystemInterface (
  IN  H2O_IPMI_PRIVATE_DATA             *IpmiData
  );


/**
 Initial function pointer of SMIC system interface to corrsponding function.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA
*/
VOID
InitialSmicSystemInterface (
  IN  H2O_IPMI_PRIVATE_DATA             *IpmiData
  );


/**
 Output lowest level rad data for debugging.

 @param[in]        Request              TRUE for send request data to BMC;
                                        FALSE for receive response data from BMC;
 @param[in]        Header               Request/Response common header.
                                        and various IPMI command, to send to BMC.
 @param[in]        Data                 Data buffer that contains request/response data.
 @param[in]        DataSize             Data buffer size.
 @param[out]       Status               Status of read/write transaction.
 @param[out]       CompletionCode       Completion code that BMC returned.
                                        Ignore when Request is set to TRUE.

*/
VOID
SystemInterfaceDebug (
  IN  BOOLEAN                  Request,
  IN  H2O_IPMI_CMD_HEADER      *Header,
  IN  VOID                     *Data,
  IN  UINT8                    DataSize,
  IN  EFI_STATUS               Status,
  IN  UINT8                    CompletionCode
  );


#endif

