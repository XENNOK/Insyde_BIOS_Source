/** @file
  Define Cmos debug code data structure.

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

#ifndef _CMOS_DEBUG_CODE_H_
#define _CMOS_DEBUG_CODE_H_

typedef struct _DEBUG_CODE_DATA{
  UINT8         NameGuid[16];
  UINT8         DebugGroup;
  UINT8         DebugCode;
} DEBUG_CODE_DATA; 

#endif
