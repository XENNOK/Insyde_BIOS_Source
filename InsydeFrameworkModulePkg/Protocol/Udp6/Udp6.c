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
//;   Module name : Udp6.c
//;   Follow EDK2 Udp6 defination
//;
/** @file
  The EFI UDPv6 (User Datagram Protocol version 6) Protocol Definition, which is built upon
  the EFI IPv6 Protocol and provides simple packet-oriented services to transmit and receive
  UDP packets.

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
#include EFI_PROTOCOL_DEFINITION (Udp6)

EFI_GUID gEfiUdp6ServiceBindingProtocolGuid = EFI_UDP6_SERVICE_BINDING_PROTOCOL_GUID;
EFI_GUID gEfiUdp6ProtocolGuid = EFI_UDP6_PROTOCOL_GUID;

EFI_GUID_STRING (
  &gEfiUdp6ServiceBindingProtocolGuid,
  "UDP6 Service Binding Protocol",
  "UDP6 Service Binding Protocol"
  );

EFI_GUID_STRING (
  &gEfiUdp6ProtocolGuid,
  "UDP6 Protocol",
  "UDP6 Protocol"
  );
