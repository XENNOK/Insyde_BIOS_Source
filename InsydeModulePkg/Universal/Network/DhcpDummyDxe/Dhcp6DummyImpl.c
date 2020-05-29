// @file
//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corp. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   Dhcp6DummyImpl.c
//;
//; Abstract:
//;
//;    Implement dummy DHCP6 protocol
//;

#include "DhcpDummy.h"

EFI_STATUS 
EFIAPI
Dhcp6DummyGetModeData (
  IN EFI_DHCP6_PROTOCOL        *This,
  OUT EFI_DHCP6_MODE_DATA      *Dhcp6ModeData OPTIONAL,
  OUT EFI_DHCP6_CONFIG_DATA    *Dhcp6ConfigData OPTIONAL
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp6DummyConfigure (
  IN EFI_DHCP6_PROTOCOL        *This,
  IN EFI_DHCP6_CONFIG_DATA     *Dhcp6CfgData OPTIONAL
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp6DummyStart (
  IN EFI_DHCP6_PROTOCOL        *This
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp6DummyInfoRequest (
  IN EFI_DHCP6_PROTOCOL        *This,
  IN BOOLEAN                   SendClientId,
  IN EFI_DHCP6_PACKET_OPTION   *OptionRequest,
  IN UINT32                    OptionCount,
  IN EFI_DHCP6_PACKET_OPTION   *OptionList[] OPTIONAL,
  IN EFI_DHCP6_RETRANSMISSION  *Retransmission, 
  IN EFI_EVENT                 TimeoutEvent OPTIONAL,
  IN EFI_DHCP6_INFO_CALLBACK   ReplyCallback,
  IN VOID                      *CallbackContext OPTIONAL
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp6DummyRenewRebind (
  IN EFI_DHCP6_PROTOCOL        *This,
  IN BOOLEAN                   RebindRequest
  )
{
  return DhcpDummyCommonCall ();
}


EFI_STATUS
EFIAPI
Dhcp6DummyDecline (
  IN EFI_DHCP6_PROTOCOL        *This,
  IN UINT32                    AddressCount,
  IN EFI_IPv6_ADDRESS          *Addresses
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp6DummyRelease (
  IN EFI_DHCP6_PROTOCOL        *This,
  IN UINT32                    AddressCount,
  IN EFI_IPv6_ADDRESS          *Addresses
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp6DummyStop (
  IN EFI_DHCP6_PROTOCOL        *This
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp6DummyParse (
  IN EFI_DHCP6_PROTOCOL        *This,
  IN EFI_DHCP6_PACKET          *Packet,
  IN OUT UINT32                *OptionCount,
  OUT EFI_DHCP6_PACKET_OPTION  *PacketOptionList[] OPTIONAL
  )
{
  return DhcpDummyCommonCall ();
}

EFI_DHCP6_PROTOCOL Dhcp6Dummy = {
                     Dhcp6DummyGetModeData,
                     Dhcp6DummyConfigure,
                     Dhcp6DummyStart,
                     Dhcp6DummyInfoRequest,
                     Dhcp6DummyRenewRebind,
                     Dhcp6DummyDecline,
                     Dhcp6DummyRelease,
                     Dhcp6DummyStop,
                     Dhcp6DummyParse
                     };
