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
//; Module Name:
//; 
//;   PciEnumerationComplete.c
//; 
//; Abstract:
//; 
//;   This Protocol is defined in UEFI Platform Initialization Specification 1.3
//;   Volume 5: Standards
//; 

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (PciEnumerationComplete)

EFI_GUID  gEfiPciEnumerationCompleteProtocolGuid = EFI_PCI_ENUMERATION_COMPLETE_GUID;

EFI_GUID_STRING (&gEfiPciEnumerationCompleteProtocolGuid, "EFI Pci Enumeration Complete Protocol", "EFI PciEnumerationComplete");
