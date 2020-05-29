//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  XtuPolicy.h

--*/

#ifndef _XTU_POLICY_H_
#define _XTU_POLICY_H_


#define DXE_PLATFORM_XTU_POLICY_GUID \
  { \
    0x565245c9, 0x9eb7, 0x468e, 0xbd, 0x51, 0x00, 0x85, 0x7c, 0x45, 0x35, 0x96 \
  }

extern EFI_GUID gDxePlatformXtuPolicyGuid;

typedef struct {
  BOOLEAN         EnableRealtimeDevicesSptt;
  BOOLEAN         EanbleRealtimeWdttAcpiTable;
  BOOLEAN         EnableSwSmiRealtimeCommunicationsInterface;
} DXE_XTU_POLICY_PROTOCOL;


#endif 

