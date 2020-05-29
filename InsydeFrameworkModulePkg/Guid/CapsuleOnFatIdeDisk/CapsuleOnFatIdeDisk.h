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

  CapsuleOnFatIdeDisk.h

Abstract:

  Capsule on Fat Ide Disk GUID.

  This is the contract between the recovery module and device recovery module
  in order to convey the name of a given recovery module type

--*/

#ifndef _PEI_CAPSULE_ON_FAT_IDE_DISK_H
#define _PEI_CAPSULE_ON_FAT_IDE_DISK_H

#define PEI_CAPSULE_ON_FAT_IDE_DISK_GUID \
  { \
    0xb38573b6, 0x6200, 0x4ac5, 0xb5, 0x1d, 0x82, 0xe6, 0x59, 0x38, 0xd7, 0x83 \
  };

extern EFI_GUID gPeiCapsuleOnFatIdeDiskGuid;

#endif
