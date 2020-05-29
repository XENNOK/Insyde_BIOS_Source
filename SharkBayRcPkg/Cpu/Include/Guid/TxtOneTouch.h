/** @file
  
  This file contains definitions required for One-Touch function.
  
;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _TXT_ONE_TOUCH_H_
#define _TXT_ONE_TOUCH_H_

#define TXT_ONE_TOUCH_GUID \
  { 0x3D989471, 0xCFAC, 0x46B7, { 0x9B, 0x1C, 0x8, 0x43, 0x1, 0x9, 0x40, 0x2D } }

extern EFI_GUID gTxtOneTouchGuid;

//
// Operation
//
#define ENABLE_VT                             128
#define DISABLE_VT_TXT                        129
#define ENABLE_VTD                            130
#define DISABLE_VTD_TXT                       131
#define ENABLE_ACTTPM_VT_VTD_TXT_DISABLE_STM  132
#define ENABLE_ACTTPM_VT_VTD_TXT_STM          133
#define DISABLE_STM                           134
#define DISABLE_TXT_STM                       135
#define DISABLE_SENTER_VMX                    136 ///< optional
#define ENABLE_VMX_SMX_ONLY                   137 ///< optional
#define ENABLE_VMX_OUTSIDE_SMX                138 ///< optional
#define ENABLE_VMX                            139 ///< optional
#define ENABLE_SENTER_ONLY                    140 ///< optional
#define ENABLE_SENTER_VMX_IN_SMX              141 ///< optional
#define ENABLE_SENTER_VMX_OUTSIDE_SMX         142 ///< optional
#define ENABLE_SENTER_VMX                     143 ///< optional
#define SET_NO_TXT_MAINTENANCE_FALSE          159
#define SET_NO_TXT_MAINTENANCE_TRUE           160

#endif
