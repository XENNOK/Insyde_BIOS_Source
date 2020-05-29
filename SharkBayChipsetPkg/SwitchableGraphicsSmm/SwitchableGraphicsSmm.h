/** @file
  This header file is for Switchable Graphics Feature SMM driver.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SWITCHABLE_GRAPHICS_SMM_H_
#define _SWITCHABLE_GRAPHICS_SMM_H_

#include <Library/UefiBootServicesTableLib.h>
#include <PchRegs.h>
#include <Protocol/LegacyBios.h>
#include <Protocol/SmmInt15Service.h>
#include <Protocol/SmmBase2.h>
#include <Protocol/SwitchableGraphicsInfo.h>
#include <SwitchableGraphicsDefine.h>

//
// Int15 Function number
//
#define SWITCHABLE_GRAPHICS_INT15_FUN_NUM     0x5F70
#define MXM_INT15_FUN_NUM                     0x5F80

#endif
