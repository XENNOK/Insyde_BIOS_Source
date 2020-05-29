//;******************************************************************************
//;* Copyright (c) 1983-2014, Insyde Software Corp. All Rights Reserved.
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
//;   DhcpDummy.h
//;
//; Abstract:
//;
//;    Refer DhcpDummy.c
//;


#ifndef _DHCP_DUMMY_H_
#define _DHCP_DUMMY_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/OemGraphicsLib.h>

#include <IndustryStandard/Pci22.h>

#include <Framework/FrameworkInternalFormRepresentation.h>

#include <Protocol/ServiceBinding.h>
#include <Protocol/Dhcp6.h>
#include <Protocol/Dhcp4.h>
#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/SimpleNetwork.h>
#include <Protocol/DevicePath.h>
#include <Protocol/H2ODialog.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/DevicePath.h>
#include <Protocol/NetworkLocker.h>

#include <Guid/NetworkEventGroup.h>
#include <Guid/EventGroup.h>

#define PCI_CLASS_CODE_OFFSET          0x08

#define DHCP4                          FALSE
#define DHCP6                          TRUE

EFI_STATUS
DhcpDummyCommonCall (
  VOID
  );

//
// The alignment do not change, the DUMMY_DHCP_SIGNATURE maybe locate failed.
//
#pragma pack(1)
typedef struct _DUMMY_DHCP {
  UINT32                               Signature;
  EFI_SERVICE_BINDING_PROTOCOL         Dhcp4DummySB;
  EFI_SERVICE_BINDING_PROTOCOL         Dhcp6DummySB;
  EFI_HANDLE                           UndiHandle;
  EFI_DEVICE_PATH_PROTOCOL             *PciDevicePath;
  EFI_SERVICE_BINDING_PROTOCOL         *Dhcp4SB;
  EFI_SERVICE_BINDING_PROTOCOL         *Dhcp6SB;
} DUMMY_DHCP;
#pragma pack()

#define DUMMY_DHCP_SIGNATURE           SIGNATURE_32 ('D', 'U', 'D', 'H')
#define LOCATE_DUMMY_DHCP4_INSTANCE(This)  \
          ((DUMMY_DHCP *) (((UINTN) (This)) - sizeof (UINT32)))
#define LOCATE_DUMMY_DHCP6_INSTANCE(This)  \
          ((DUMMY_DHCP *) (((UINTN) (This)) - sizeof (EFI_SERVICE_BINDING_PROTOCOL) - sizeof (UINT32)))

extern EFI_DHCP4_PROTOCOL Dhcp4Dummy;
extern EFI_DHCP6_PROTOCOL Dhcp6Dummy;

#endif //_DHCP_DUMMY_H_
