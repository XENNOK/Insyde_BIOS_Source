//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _PEG_DATA_VARIABLE_H_
#define _PEG_DATA_VARIABLE_H_

#define PEG_DATA_VARIABLE \
  { \
    0x28cf6d27, 0x694, 0x45d0, 0xa0, 0x6a, 0x15, 0x26, 0x6f, 0x1e, 0x83, 0x46 \
  }
//
// {28CF6D27-0694-45d0-A06A-15266F1E8346}
//

extern EFI_GUID gPegDataVariableGuid;

#define PEG_DATA_VARIABLE_NAME     L"PegDataVar"

#endif
