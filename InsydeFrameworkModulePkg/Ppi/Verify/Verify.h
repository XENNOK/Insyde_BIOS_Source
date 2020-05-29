//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
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

  Verify.h

Abstract:

  Verification PPI as defined in EFI 2.0

  Used to verify modules in PEI

--*/

#ifndef _PEI_VERIFY_H_
#define _PEI_VERIFY_H_

#define SEC_PEI_VERIFY_GUID \
  { \
    0x5faacd88, 0x660c, 0x4a75, 0xa8, 0x8a, 0x9a, 0x18, 0x3, 0xfa, 0xe0, 0x3 \
  }

typedef
EFI_STATUS
(EFIAPI *SEC_VERIFY_SECTION) (
  IN EFI_PEI_SERVICES                **PeiServices,
  IN EFI_PHYSICAL_ADDRESS            SecuritySectionAddress
  );

typedef struct {
  SEC_VERIFY_SECTION  VerifySection;
} SEC_VERIFY_SECTION_PPI;

extern EFI_GUID gPeiVerifyPpiGuid;

#endif
