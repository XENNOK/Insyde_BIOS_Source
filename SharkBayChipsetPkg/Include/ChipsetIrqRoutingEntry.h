/** @file

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CHIPSET_IRQ_ROUTING_ENTRY_H_
#define _CHIPSET_IRQ_ROUTING_ENTRY_H_

#include <Protocol/LegacyBiosPlatform.h>

#pragma pack(1)
typedef struct {
  EFI_LEGACY_IRQ_ROUTING_ENTRY  LeagcyIrqRoutingEntry;
  UINT32                        DevIpRegValue;
  UINT8                         ProgrammableIrq;
} EFI_LEGACY_IRQ_ROUTING_ENTRY_AND_IP_REGISTER;
#pragma pack()

#endif 

