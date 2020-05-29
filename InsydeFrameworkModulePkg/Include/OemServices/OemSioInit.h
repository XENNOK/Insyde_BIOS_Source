//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _OEM_SIO_INIT_H_
#define _OEM_SIO_INIT_H_
#include "Tiano.h"

typedef 
VOID
(EFIAPI *INIT_LPC_SIO) (
  VOID
  );

typedef struct {
  UINT8 Register;
  UINT8 Value;
} EFI_SIO_TABLE;

typedef struct {
  UINT16  IndexPort;
  UINT16  DataPort;
} SIO_BASE_ADDR;

typedef struct {
  UINT8 Register;
  UINT8 Value;
} EFI_SIO_GPIO_TABLE;

#endif
