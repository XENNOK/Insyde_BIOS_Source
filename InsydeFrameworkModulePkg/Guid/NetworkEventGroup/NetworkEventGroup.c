//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//#; 
//#;  Module Name:
//#;
//#;    NetworkEventGroup.c
//#;
//#;  Abstract:
//#;
//#;    Defination event GUID of network stack
//#;

#include "Tiano.h"
#include EFI_GUID_DEFINITION (NetworkEventGroup)

EFI_GUID  gEfiNetworkEventPxeFailed = EFI_NETWORK_EVENT_GROUP_PXE_FAILED;

EFI_GUID  gEfiNetworkStackIPv4SupportGuid = EFI_NETWORK_STACK_IPv4_SUPPORT_GUID;
EFI_GUID  gEfiNetworkStackIPv6SupportGuid = EFI_NETWORK_STACK_IPv6_SUPPORT_GUID;
EFI_GUID  gEfiNetworkStackDHCPSupportGuid = EFI_NETWORK_STACK_DHCP_SUPPORT_GUID;
EFI_GUID  gEfiNetworkStackUndiDependencyGuid = EFI_NETWORK_STACK_UNDI_DEPENDENCY_GUID;

EFI_GUID_STRING ( \
  &gEfiNetworkStackIPv4SupportGuid, \
  "Network Event Group, UEFI NetworkStack IPv4 support", \
  "UEFI Network Stack IPv4 support GUID" \
  );

EFI_GUID_STRING ( \
  &gEfiNetworkStackIPv6SupportGuid, \
  "Network Event Group, UEFI NetworkStack IPv6 support", \
  "UEFI Network Stack IPv6 support GUID" \
  );

EFI_GUID_STRING (&gEfiNetworkEventPxeFailed, "Network Event Group, UEFI PXE failed event", "UEFI PXE Failed event GUID");
