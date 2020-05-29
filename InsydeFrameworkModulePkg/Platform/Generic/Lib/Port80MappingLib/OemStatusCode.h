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
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to
// the additional terms of the license agreement
//
/*++

Copyright (c)  2006 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:
 
  OemStatusCode.h
 
Abstract:

  Library to map status code to P80 code for the platform

--*/

#ifndef _EFI_OEM_STATUS_CODE_H_
#define _EFI_OEM_STATUS_CODE_H_

#include "EfiStatusCode.h"

#define EFI_SW_PS_PC_DISPATCH                     (EFI_OEM_SPECIFIC | 0x00000000)

//
//DXE core
//
#define EFI_SW_DXE_CORE_PC_ARCH_READY             (EFI_OEM_SPECIFIC | 0x00000000)

//
//memory
//
#define EFI_CU_MEMORY_PC_INIT_DONE                (EFI_OEM_SPECIFIC | 0x00000000)

//
//Boot Services
//
#define EFI_SW_BS_PC_VAR_RECLAIM                  (EFI_OEM_SPECIFIC | 0x00000000)

//
//BDS subclass
//
#define EFI_SOFTWARE_BDS                          (EFI_SOFTWARE | 0x00130000)

//
//BDS phase
//
#define EFI_SW_BDS_ENTRY_POINT                    (EFI_OEM_SPECIFIC | 0x00000000)
#define EFI_SW_BDS_RETURN_TO_LAST                 (EFI_OEM_SPECIFIC | 0x00000001)
#define EFI_SW_BDS_SETUP_POLICY                   (EFI_OEM_SPECIFIC | 0x00000002)
#define EFI_SW_BDS_CONNECT_CONTROLLER             (EFI_OEM_SPECIFIC | 0x00000003)
#define EFI_SW_BDS_FLASH_UPDATE                   (EFI_OEM_SPECIFIC | 0x00000004)
#define EFI_SW_BDS_HANDOFF_TO_NEXT                (EFI_OEM_SPECIFIC | 0x00000005)

//
//Chipset
//
#define EFI_CU_CHIPSET_PC_SOUTH_INIT              (EFI_OEM_SPECIFIC | 0x00000000)
#define EFI_CU_CHIPSET_NORTH_INIT                 (EFI_OEM_SPECIFIC | 0x00000001)
#define EFI_CU_CHIPSET_SIO_INIT                   (EFI_OEM_SPECIFIC | 0x00000002)

//
//USB bus
//
#define EFI_IOB_USB_BUS_ENUM                      (EFI_OEM_SPECIFIC | 0x00000000)

//
//ATA bus
//
#define EFI_IOB_ATA_BUS_ENUM                      (EFI_OEM_SPECIFIC | 0x00000000)


//
//Runtime
//
#define EFI_SW_RT_PC_ENTER_SLEEP                  (EFI_OEM_SPECIFIC | 0x00000000)
#define EFI_SW_RT_PC_EXIT_SLEEP                   (EFI_OEM_SPECIFIC | 0x00000001)



#endif
