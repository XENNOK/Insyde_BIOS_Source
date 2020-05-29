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

  CapsuleOnFatUsbDisk.h

Abstract:

  Capsule on Fat Usb Disk GUID.

  This is the contract between the recovery module and device recovery module
  in order to convey the name of a given recovery module type

--*/

#ifndef _PEI_CAPSULE_ON_FAT_USB_DISK_H
#define _PEI_CAPSULE_ON_FAT_USB_DISK_H

#define PEI_CAPSULE_ON_FAT_USB_DISK_GUID \
  { \
    0x0ffbce19, 0x324c, 0x4690, 0xa0, 0x09, 0x98, 0xc6, 0xae, 0x2e, 0xb1, 0x86 \
  };

extern EFI_GUID gPeiCapsuleOnFatUsbDiskGuid;

#endif
