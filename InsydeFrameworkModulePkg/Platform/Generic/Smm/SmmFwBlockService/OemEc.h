//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _OEM_EC_RT_SERVICE_H_
#define _OEM_EC_RT_SERVICE_H_

#include "Tiano.h"
#include "Pci22.h"
#include "EfiRuntimeLib.h"

EFI_STATUS
EcIdle (
  IN  BOOLEAN   EnableWrites
  );

#endif

