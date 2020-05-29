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

    Smm.h

Abstract:

  GUIDs used for SMM drivers

--*/

#ifndef _SMM_GUID_H_
#define _SMM_GUID_H_

#define EFI_SMM_START_IMAGE_GUID \
  { \
    0xe5d02959, 0xc65c, 0x44eb, 0xa5, 0x57, 0xfa, 0xed, 0xe7, 0x6c, 0x1d, 0x5b \
  }

#define EFI_SMM_CORE_IMAGE_GUID \
  { \
    0xc1bdd34e, 0x9ec0, 0x48aa, 0x80, 0x6a, 0x6c, 0x2e, 0xba, 0x0d, 0xc4, 0x45 \
  }

#define EFI_SMM_CORE_DISPATCHER_GUID \
  { \
    0x240612B5, 0xA063, 0x11d4, 0x9A, 0x3A, 0x0, 0x90, 0x27, 0x3F, 0xC1, 0x8E \
  }

#define EFI_SMM_AP_IMAGE_GUID \
  { \
    0xe2887c7, 0x9925, 0x4951, 0xba, 0x5c, 0xc3, 0x27, 0x4a, 0x4e, 0xaa, 0x25 \
  }

extern EFI_GUID gEfiSmmStartImageGuid;
extern EFI_GUID gEfiSmmCoreImageGuid;
extern EFI_GUID gEfiSmmCoreDispatcherGuid;
extern EFI_GUID gEfiSmmApImageGuid;

#endif
