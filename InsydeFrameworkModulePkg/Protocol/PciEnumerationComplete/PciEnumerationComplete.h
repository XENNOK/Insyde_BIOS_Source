//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
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
//;   PciEnumerationComplete.h
//; 
//; Abstract:
//; 
//;   This Protocol is defined in UEFI Platform Initialization Specification 1.3
//;   Volume 5: Standards
//; 

#ifndef _PCI_ENUMERATION_COMPLETE_H_
#define _PCI_ENUMERATION_COMPLETE_H_

#define EFI_PCI_ENUMERATION_COMPLETE_GUID \
  {  \
   0x30cfe3e7, 0x3de1, 0x4586, { 0xbe, 0x20, 0xde, 0xab, 0xa1, 0xb3, 0xb7, 0x93  } \
  }

extern EFI_GUID gEfiPciEnumerationCompleteProtocolGuid;

#endif
