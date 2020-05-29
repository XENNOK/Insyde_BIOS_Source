//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Abstract:
//;

#ifndef _OEM_SG_LIB_H_
#define _OEM_SG_LIB_H_

#include "Tiano.h"


UINT32* 
OemGetDeviceIdArray (
  );

VOID
OemSelectDiscreteVGA (
  BOOLEAN   DiscreteVideoOn
  );

#endif
