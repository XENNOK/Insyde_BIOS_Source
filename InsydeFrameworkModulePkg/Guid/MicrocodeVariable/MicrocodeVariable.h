//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _EFI_MICROCODE_VARIABLE_H_
#define _EFI_MICROCODE_VARIABLE_H_

#include "Tiano.h"

//
// Variable Name
//
#define EFI_MICROCODE_VARIABLE_NAME     L"Microcode"

#define EFI_MICROCODE_VARIABLE_GUID \
  { \
    0x155828cf, 0x8b3f, 0x4c1e, 0xab, 0x8c, 0x59, 0x7b, 0xd0, 0x7c, 0x97, 0x36 \
  }

#define EFI_MICROCODE_VARIABLE_SIZE     0x200
#define EFI_MICROCODE_STANDARD_SIZE     0x800
#define EFI_MICROCODE_STANDARD_VAR_NUM  EFI_MICROCODE_STANDARD_SIZE / EFI_MICROCODE_VARIABLE_SIZE

extern EFI_GUID gEfiMicrocodeVariableGuid;


#endif
