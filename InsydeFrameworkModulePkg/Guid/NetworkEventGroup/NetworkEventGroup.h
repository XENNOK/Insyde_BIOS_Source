//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; Module Name:
//;
//;   NetworkEventGroup.h
//;
//; Abstract:
//;
//;   Defination GUID of Network event
//;

#ifndef _NETWORK_EVENT_GROUP_H_
#define _NETWORK_EVENT_GROUP_H_

//
// UEFI event group, PXE boot failed event GUID value
//
#define EFI_NETWORK_EVENT_GROUP_PXE_FAILED \
{ 0xd5e6bad, 0xc9b5, 0x47c2, 0xb7, 0x88, 0xc7, 0x56, 0xbe, 0x54, 0xea, 0xdd }

//
// UEFI event group, Network Stack IPv4 Support
//
#define EFI_NETWORK_STACK_IPv4_SUPPORT_GUID \
{ 0xcad4ecda, 0xc857, 0x4ff9, 0x81, 0xab, 0xfd, 0xda, 0xfc, 0x70, 0xbb, 0x39 }

//
// UEFI event group, Network Stack IPv6 Support
//
#define EFI_NETWORK_STACK_IPv6_SUPPORT_GUID \
{ 0x9da16e07, 0x3b52, 0x453d, 0xa7, 0xf0, 0x1a, 0xc0, 0x5d, 0x50, 0x49, 0x80 }

//
// UEFI event group, DHCP4 and DHCP6 support for Microsoft Windows 8 BitLocker Network Unlock
//
#define EFI_NETWORK_STACK_DHCP_SUPPORT_GUID \
{ 0x7f48caf0, 0x996e, 0x4bd8, 0xb0, 0x1d, 0xf2, 0x9f, 0x8c, 0x72, 0x92, 0xbc }

//
// UEFI event group, UNDI driver dependency GUID
//
#define EFI_NETWORK_STACK_UNDI_DEPENDENCY_GUID \
{ 0x45d54528, 0x70f9, 0x467c, 0xba, 0xdc, 0xb1, 0xe9, 0x75, 0x26, 0x5e, 0x90 }

extern EFI_GUID gEfiNetworkStackDHCPSupportGuid;
extern EFI_GUID gEfiNetworkStackIPv4SupportGuid;
extern EFI_GUID gEfiNetworkStackIPv6SupportGuid;
extern EFI_GUID gEfiNetworkEventPxeFailed;
extern EFI_GUID gEfiNetworkStackUndiDependencyGuid;

#endif //_NETWORK_EVENT_GROUP_H_
