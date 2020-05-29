/** @file
  Header file for HddSpindown Driver..

//;******************************************************************************
//;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
*/

#ifndef _HDD_SPINDOWN_H
#define _HDD_SPINDOWN_H

#include <PortNumberMap.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DevicePathLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/SmmServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/DevicePathLib.h>
#include <Protocol/DiskInfo.h>
#include <Protocol/AtaPassThru.h>
#include <Protocol/HddSpindown.h>

#define STANDBY_IMMEDIATE              0xE0

#define EFI_HDD_SPINDOWN_SIGNATURE     SIGNATURE_32 ('H', 'D', 'D', 'S')

#define HDD_SPINDOWN_FROM_HDDSPINDOWN(a) CR (a, HDD_SPINDOWN_PRIVATE_DATA, HddSpindown, EFI_HDD_SPINDOWN_SIGNATURE)

typedef struct _HDD_SPINDOWN_PRIVATE_DATA {
  UINT32                     Signature;
  UINT8                      ControllerMode;
  HDD_SPINDOWN_PROTOCOL      HddSpindown;
} HDD_SPINDOWN_PRIVATE_DATA;

#endif
