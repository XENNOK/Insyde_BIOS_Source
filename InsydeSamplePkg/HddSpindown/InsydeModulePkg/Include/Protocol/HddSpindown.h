/** @file
  Protocol used to send HDD spin down command.
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

#ifndef _HDD_SPIN_DONW_PROTOCOL_H_
#define _HDD_SPIN_DONW_PROTOCOL_H_

#define HDD_SPINDOWN_PROTOCOL_GUID \
  {0xf754e402, 0xaee3, 0x47d6, 0x93, 0x7a, 0xdb, 0x75, 0xaa, 0xac, 0xd1, 0x43}

typedef struct _HDD_SPINDOWN_PROTOCOL   HDD_SPINDOWN_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *HDD_SPINDOWN_FUNCTION) (
  IN HDD_SPINDOWN_PROTOCOL     *This,
  IN UINT16                    TargetPort
);

typedef struct _HDD_SPINDOWN_PROTOCOL {
  HDD_SPINDOWN_FUNCTION          HddSpinDown;
} HDD_SPINDOWN_PROTOCOL;

extern EFI_GUID gHddSpindownProtocolGuid;

#endif 

