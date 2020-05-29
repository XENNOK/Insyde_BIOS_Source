//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; ----------------------------------------------------------------------------
//;
//; Abstract :
//;
//;   Module name : Dhcp6.c
//;   Follow EDK2 Dhcp6 defination
//;
/** @file
  UEFI Dynamic Host Configuration Protocol 6 Definition, which is used to get IPv6
  addresses and other configuration parameters from DHCPv6 servers.

  Copyright (c) 2008 - 2010, Intel Corporation                                                         
  All rights reserved. This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            
                                                                                            
  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED. 

  @par Revision Reference:          
  This Protocol is introduced in UEFI Specification 2.2

**/

#include "EfiSpec.h"
#include EFI_PROTOCOL_DEFINITION (Dhcp6)

EFI_GUID gEfiDhcp6ServiceBindingProtocolGuid
           = EFI_DHCP6_SERVICE_BINDING_PROTOCOL_GUID;

EFI_GUID gEfiDhcp6ProtocolGuid = EFI_DHCP6_PROTOCOL_GUID;

EFI_GUID_STRING (
  &gEfiDhcp6ServiceBindingProtocolGuid,
  "DHCP6 Service Binding Protocol",
  "DHCP6 Service Binding Protocol"
  );

EFI_GUID_STRING (
  &gEfiDhcp6ProtocolGuid,
  "DHCP6 Protocol",
  "DHCP6 Protocol"
  );
