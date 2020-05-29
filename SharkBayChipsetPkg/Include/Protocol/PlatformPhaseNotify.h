/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PLATFORM_PHASE_NOTIFY_H_
#define _PLATFORM_PHASE_NOTIFY_H_

//
// { BA2E7D85-F570-0361-822E-265DFDB7CF01 }
//
#define EFI_PCI_HOST_BRIDGE_END_RESOURCE_ALLOCATION_NOTIFY_GUID \
  { \
    0xBA2E7D85, 0xF570, 0x0361, 0x82, 0x2E, 0x26, 0x5D, 0xFD, 0xB7, 0xCF, 0x01 \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID        gEfiPciHostBridgeEndResourceAllocationNotifyGuid;

#endif
