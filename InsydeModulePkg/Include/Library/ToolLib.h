/** @file
  Tool Library Class definitions

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

#ifndef _TOOL_LIB_H_
#define _TOOL_LIB_H_

#include <Uefi.h>

#if defined(PcdGetBool)
#undef PcdGetBool
#endif

#if defined(FeaturePcdGet)
#undef FeaturePcdGet
#endif

#if defined(PcdGet8)
#undef PcdGet8
#endif

#if defined(PcdGet16)
#undef PcdGet16
#endif

#if defined(PcdGet32)
#undef PcdGet32
#endif

#if defined(PcdGet64)
#undef PcdGet64
#endif

#if defined(FixedPcdGet8)
#undef FixedPcdGet8
#endif

#if defined(FixedPcdGet16)
#undef FixedPcdGet16
#endif

#if defined(FixedPcdGet32)
#undef FixedPcdGet32
#endif

#if defined(FixedPcdGet64)
#undef FixedPcdGet64
#endif

#define PcdGetBool(PcdName)     (BOOLEAN)GetPcdValue(#PcdName)
#define PcdGet8(PcdName)        (UINT8)GetPcdValue(#PcdName)
#define PcdGet16(PcdName)       (UINT16)GetPcdValue(#PcdName)
#define PcdGet32(PcdName)       (UINT32)GetPcdValue(#PcdName)
#define PcdGet64(PcdName)       (UINT64)GetPcdValue(#PcdName)

#define FeaturePcdGet           PcdGetBool
#define FixedPcdGet8            PcdGet8   
#define FixedPcdGet16           PcdGet16
#define FixedPcdGet32           PcdGet32
#define FixedPcdGet64           PcdGet64

UINTN 
GetPcdValue (
  CHAR8 *PcdName
  );
  
#endif

