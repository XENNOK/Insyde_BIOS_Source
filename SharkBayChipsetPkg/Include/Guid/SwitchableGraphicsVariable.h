/** @file
  SwitchableGraphicsVariable.h

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

#ifndef _SWITCHABLE_GRAPHICS_VARIABLE_GUID_H_
#define _SWITCHABLE_GRAPHICS_VARIABLE_GUID_H_

//[-start-130218-IB07250301-modify]//
#define SWITCHABLE_GRAPHICS_VARIABLE_GUID  \
  { \
    0xb2b7c21f, 0x1786, 0x4a64, 0xbe, 0x69, 0x16, 0xce, 0xf7, 0x64, 0x73, 0x31 \
  }

extern EFI_GUID                               gH2OSwitchableGraphicsVariableGuid;

#pragma pack (1)
typedef struct {
  UINT8                                       OptimusFlag;
  UINT8                                       MasterDgpuBus;
  UINT8                                       SlaveDgpuBus;
} OPTIMUS_CONFIGURATION;

typedef struct {
  OPTIMUS_CONFIGURATION                       OptimusVariable;
} SG_VARIABLE_CONFIGURATION;
#pragma pack ()
//[-end-130218-IB07250301-modify]//

#endif
