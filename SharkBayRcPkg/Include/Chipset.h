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

#ifndef _OEM_SERVICES_EX_CHIPSET_SUPPORT_INC_H_
#define _OEM_SERVICES_EX_CHIPSET_SUPPORT_INC_H_

//[-start-121019-IB05330384-remove]//
//#define  COMMON_SET_PEI_DDR3_VOLTAGE_ARG_COUNT         OEM_SET_PEI_DDR3_VOLTAGE_ARG_COUNT
//#define  OEM_SET_PEI_DDR3_VOLTAGE_ARG_COUNT            5
//[-end-121019-IB05330384-remove]//

typedef enum {
//[-start-121019-IB05330384-remove]//
//  OEM_SET_PEI_DDR3_VOLTAGE,
//[-end-121019-IB05330384-remove]//
  PEI_MAX_NUM_CHIPSET_EXTENDABLE_SUPPORT
} OEM_SERVICES_PEI_CHIPSET_EXTENDABLE_SUPPORT;

typedef enum {
  RUNTIME_MAX_NUM_CHIPSET_EXTENDABLE_SUPPORT
} OEM_SERVICES_RUNTIME_CHIPSET_EXTENDABLE_SUPPORT;

typedef enum {
  SMM_MAX_NUM_CHIPSET_EXTENDABLE_SUPPORT        
} OEM_SERVICES_SMM_CHIPSET_EXTENDABLE_SUPPORT;


#endif
