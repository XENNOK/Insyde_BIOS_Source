/** @file

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

#ifndef _OEM_SERVICES_OEM_SUPPORT_INC_H_
#define _OEM_SERVICES_OEM_SUPPORT_INC_H_

//[-start-120419-IB10820038-modify]//
//[-start-120622-IB03530373-modify]//
#include <Chipset.h>
//[-end-120622-IB03530373-modify]//
//[-end-120419-IB10820038-modify]//

typedef enum {
  FIRST_PEI_OEM_EXTENDABLE_SUPPORT      =  PEI_MAX_NUM_CHIPSET_EXTENDABLE_SUPPORT,
  PEI_OEM_EX_SAMPLE,
  PEI_MAX_NUM_OEM_EXTENDABLE_SUPPORT
} OEM_SERVICES_PEI_OEM_EXTENDABLE_SUPPORT;

typedef enum {
  FIRST_RUNTIME_OEM_EXTENDABLE_SUPPORT  =  RUNTIME_MAX_NUM_CHIPSET_EXTENDABLE_SUPPORT,
  DXE_OEM_EX_SAMPLE,
  RUNTIME_MAX_NUM_OEM_EXTENDABLE_SUPPORT        
} OEM_SERVICES_RUNTIME_OEM_EXTENDABLE_SUPPORT;

typedef enum {
  FIRST_SMM_OEM_EXTENDABLE_SUPPORT      =  SMM_MAX_NUM_CHIPSET_EXTENDABLE_SUPPORT,
  SMM_OEM_EX_SAMPLE,      
  SMM_MAX_NUM_OEM_EXTENDABLE_SUPPORT        
} OEM_SERVICES_SMM_OEM_EXTENDABLE_SUPPORT;

#endif
