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
//;   Dhcp4DummyImpl.c
//;
//; Abstract:
//;
//;    Implement dummy DHCP4 protocol
//;

#include "DhcpDummy.h"

EFI_STATUS 
EFIAPI
Dhcp4DummyGetModeData (
  IN EFI_DHCP4_PROTOCOL     *This,
  OUT EFI_DHCP4_MODE_DATA   *Dhcp4ModeData
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp4DummyConfigure (
  IN EFI_DHCP4_PROTOCOL     *This,
  IN EFI_DHCP4_CONFIG_DATA  *Dhcp4CfgData  OPTIONAL
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp4DummyStart (
  IN EFI_DHCP4_PROTOCOL     *This,
  IN EFI_EVENT              CompletionEvent    OPTIONAL
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp4DummyRenewRebind (
  IN EFI_DHCP4_PROTOCOL     *This,
  IN BOOLEAN                RebindRequest,
  IN EFI_EVENT              CompletionEvent    OPTIONAL
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp4DummyRelease (
  IN EFI_DHCP4_PROTOCOL     *This
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp4DummyStop (
  IN EFI_DHCP4_PROTOCOL     *This
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp4DummyBuild (
  IN EFI_DHCP4_PROTOCOL       *This,
  IN EFI_DHCP4_PACKET         *SeedPacket,
  IN UINT32                   DeleteCount,
  IN UINT8                    *DeleteList        OPTIONAL,
  IN UINT32                   AppendCount,
  IN EFI_DHCP4_PACKET_OPTION  *AppendList[]      OPTIONAL,
  OUT EFI_DHCP4_PACKET        **NewPacket
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp4DummyTransmitReceive (
  IN EFI_DHCP4_PROTOCOL                *This,
  IN EFI_DHCP4_TRANSMIT_RECEIVE_TOKEN  *Token
  )
{
  return DhcpDummyCommonCall ();
}

EFI_STATUS
EFIAPI
Dhcp4DummyParse (
  IN EFI_DHCP4_PROTOCOL        *This,
  IN EFI_DHCP4_PACKET          *Packet,
  IN OUT UINT32                *OptionCount,
  OUT EFI_DHCP4_PACKET_OPTION  *PacketOptionList[]  OPTIONAL
  )
{
  return DhcpDummyCommonCall ();
}

EFI_DHCP4_PROTOCOL Dhcp4Dummy = {
                     Dhcp4DummyGetModeData,
                     Dhcp4DummyConfigure,
                     Dhcp4DummyStart,
                     Dhcp4DummyRenewRebind,
                     Dhcp4DummyRelease,
                     Dhcp4DummyStop,
                     Dhcp4DummyBuild,
                     Dhcp4DummyTransmitReceive,
                     Dhcp4DummyParse
                     };
