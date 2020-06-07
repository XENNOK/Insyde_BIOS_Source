/** @file

 SIO_HW Header

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/


#ifndef _SMMAP_HW_H_
#define _SMMAP_HW_H_

#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>

#include <Protocol/SmmControl2.h>


#define EFI_HYDRA_ENABLE_SW_SMI 0xCA

#define DEF_ROWS    31
#define DEF_COLS    100

#define HUGE_TITLE_OFFSET   3
#define HUGE_TAIL_OFFSET    25
#endif
