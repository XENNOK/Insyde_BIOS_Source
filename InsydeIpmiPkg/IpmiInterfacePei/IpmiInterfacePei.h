/** @file
 H2O IPMI Interface PEI module common header file.

 This header file contains common definitions and structures for PEI module.

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

#ifndef _IPMI_INTERFACE_PEI_H_
#define _IPMI_INTERFACE_PEI_H_


#include <Ppi/H2OIpmiInterfacePpi.h>
#include <Ppi/MasterBootMode.h>
#include <Ppi/ReadOnlyVariable2.h>

#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/IpmiCommonLib.h>
#include <Library/IpmiHobLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiIpmiPackageLib.h>
#include <Library/PeiIpmiLpcLib.h>
#include <Library/PeiIpmiSioLib.h>
#include <Library/PeiOemIpmiPackageLib.h>

#include <IpmiDmConfigForm.h>


//
// TimeStamp use
//
#define CMOS_INDEX           0x70
#define CMOS_DATA            0x71

#define CMOS_YEAR            0x09
#define CMOS_MONTH           0x08
#define CMOS_DAY             0x07
#define CMOS_HOUR            0x04
#define CMOS_MINUTE          0x02
#define CMOS_SECOND          0x00


#define H2O_IPMI_PRIVATE_DATA_SIGNATURE     SIGNATURE_32('h', 'p', 'i', 'm')
#define H2O_IPMI_PRIVATE_DATA_FROM_THIS(a)  BASE_CR(a, H2O_IPMI_PRIVATE_DATA, IpmiPpi)


typedef struct _H2O_IPMI_PRIVATE_DATA H2O_IPMI_PRIVATE_DATA;


//
// Private wait BMC decode function
//
/**
 Function to wait & detect corresponding register is decoded by BMC.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA struct.

 @retval EFI_SUCCESS                    Corresponding register is decoded by BMC success.
 @retval EFI_NOT_FOUND                  Corresponding register not be decoded.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_WAIT_BMC_DECODE) (
  IN  H2O_IPMI_PRIVATE_DATA             *IpmiData
  );


struct _H2O_IPMI_PRIVATE_DATA {
  UINTN                    Signature;
  H2O_IPMI_INTERFACE_PPI   IpmiPpi;
  EFI_PEI_PPI_DESCRIPTOR   IpmiPpiDescriptor;
  UINT16                   IpmiBaseAddress;
  UINT16                   IpmiBaseAddressOffset;
  UINTN                    DelayTimeTicks;
  UINTN                    TotalTimeTicks;
  UINT8                    Sequence;
  UINT16                   TimerAddress;
  UINT32                   TimerMaxValue;
  H2O_IPMI_WAIT_BMC_DECODE IpmiWaitBmcDecode;
};


/**
 Initial function pointer of BT system interface to corrsponding function.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA struct.
*/
VOID
InitialBtSystemInterface (
  IN  H2O_IPMI_PRIVATE_DATA             *IpmiData
  );


/**
 Initial function pointer of KCS system interface to corrsponding function.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA struct.
*/
VOID
InitialKcsSystemInterface (
  IN  H2O_IPMI_PRIVATE_DATA             *IpmiData
  );


/**
 Initial function pointer of SMIC system interface to corrsponding function.

 @param[in]         IpmiData            A pointer to H2O_IPMI_PRIVATE_DATA struct.
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

