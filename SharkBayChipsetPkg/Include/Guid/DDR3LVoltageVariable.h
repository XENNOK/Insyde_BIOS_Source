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

#ifndef _DDR3L_VOLTAGE_VARIABLE_H_
#define _DDR3L_VOLTAGE_VARIABLE_H_

#define DDR3L_VOLTAGE_VARIABLE \
  { \
    0xEF5F1F9F, 0x735E, 0x4671, 0xB6, 0x8B, 0x4C, 0x80, 0xCF, 0x01, 0x13, 0x28 \
  }
     
extern EFI_GUID gDDR3LVoltageVariableGuid;

//[-start-121019-IB05330384-modify]//
typedef struct {
  UINT8              GPIOPingSelect[3];
  UINT8              GPIOPingSetting;    
} DDR3L_VOLTAGE_SETTING;
//[-end-121019-IB05330384-modify]//

#endif
