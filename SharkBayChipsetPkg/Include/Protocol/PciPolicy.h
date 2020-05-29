/** @file
  
  Pci Policy Protocol Guid
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _PCI_POLICY_H_
#define _PCI_POLICY_H_

#define PCI_POLICY_PROTOCOL_GUID  \
  { \
    0xe60e8aad, 0x9345, 0x4074, 0x86, 0xd, 0xbc, 0x3e, 0xea, 0x6b, 0x2a, 0xba \
  }

typedef struct {
  UINT8   Enable;
} PCIE_PAYLOAD;

typedef struct {
  UINT8   Enable;
  UINT8   LatencyTime;
} PCI_LATENCYTIME;

typedef struct {
  PCI_LATENCYTIME LatencyTimePolicy;
  PCIE_PAYLOAD    ProgramPciePayLoadPolicy;
} PCI_POLICY;

extern EFI_GUID gPciPolicyProtocolGuid;

#endif
