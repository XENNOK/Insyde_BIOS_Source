/** @file
  When user enable PxeToLan, install one protocol to indicate Native driver load or not

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _NETWORK_LOCKER_PROTOCOL_H
#define _NETWORK_LOCKER_PROTOCOL_H

//
// Network Locker Protocol GUID value
//
#define NETWORK_LOCKER_PROTOCOL_GUID \
{ 0xd5e6bad, 0xc9b5, 0x47c2, 0xb7, 0x88, 0xc7, 0x56, 0xbe, 0x54, 0xea, 0xdd }

#define NETWORK_LOCKER_SIGNATURE       SIGNATURE_32 ('N', 'E', 'T', 'L')
#define DHCP6_DUID_SELECTOR_SIGNATURE  SIGNATURE_32 ('D', 'U', 'S', 'L')
#define NETWORK_LOCKER_VERSION         0x1000

typedef struct _NETWORK_LOCKER_PROTOCOL NETWORK_LOCKER_PROTOCOL;

typedef enum {
  NetworkStackSnpCallback,
  NetworkStackIp4Callback,
  NetworkStackIp6Callback,
  NetworkStackUdp4Callback,
  NetworkStackUdp6Callback,
  NetworkStackTcp4Callback,
  NetworkStackTcp6Callback,
  NetworkStackDhcp4Callback,
  NetworkStackDhcp6Callback,
  NetworkStackMtftp4Callback,
  NetworkStackMtftp6Callback,
  NetworkStackPxe4Callback,
  NetworkStackPxe6Callback,
  NetworkStackMax
} NETWORK_STACK_CALLBACK_IDENTIFIER;

//
// Enumeration of Duid type recorded by IANA, refers to section-24.5 of rfc-3315.
//
typedef enum _DHCP6_DUID_TYPE{
  Dhcp6DuidTypeLlt              = 1,
  Dhcp6DuidTypeEn               = 2,
  Dhcp6DuidTypeLl               = 3,
  Dhcp6DuidTypeUuid             = 4
} DHCP6_DUID_TYPE;

typedef
EFI_STATUS
(*NETWORK_STACK_CALLBACK) (
  NETWORK_STACK_CALLBACK_IDENTIFIER    Identifier
  );

#pragma pack(1)
typedef struct _NETWORK_STACK_DHCP6_DUID_SELECTOR {
  UINT32                               Signature;
  UINTN                                SelectorCnt;
  UINT8                                Pool[1];
} NETWORK_STACK_DHCP6_DUID_SELECTOR;

struct _NETWORK_LOCKER_PROTOCOL {
  NETWORK_STACK_CALLBACK               NetworkStackCallback;
  UINT32                               Signature;
  UINT32                               Version;
  NETWORK_STACK_DHCP6_DUID_SELECTOR    *NetworkStackDhcp6DuidSelector;
};
#pragma pack()

extern EFI_GUID gNetworkLockerProtocolGuid;

#endif //_NETWORK_LOCKER_PROTOCOL_H
