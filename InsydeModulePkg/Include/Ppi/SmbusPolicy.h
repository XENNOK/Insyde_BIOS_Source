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

/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SmbusPolicy.h

Abstract:

  Smbus Policy PPI as defined in EFI 2.0

--*/

#ifndef _PEI_SMBUS_POLICY_PPI_H
#define _PEI_SMBUS_POLICY_PPI_H

#define PEI_SMBUS_POLICY_PPI_GUID \
  { \
    0x63b6e435, 0x32bc, 0x49c6, 0x81, 0xbd, 0xb7, 0xa1, 0xa0, 0xfe, 0x1a, 0x6c \
  }

typedef struct _PEI_SMBUS_POLICY_PPI  PEI_SMBUS_POLICY_PPI;

struct _PEI_SMBUS_POLICY_PPI {
  UINTN   BaseAddress;
  UINT32  PciAddress;
  UINT8   NumRsvdAddress;
  UINT8   *RsvdAddress;
};

extern EFI_GUID gPeiSmbusPolicyPpiGuid;

#endif
