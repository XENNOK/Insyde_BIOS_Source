//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _OEM_PNP_UPDATE_GPNV_H_
#define _OEM_PNP_UPDATE_GPNV_H_

#include "Tiano.h"



#pragma pack (1)


typedef struct {
  UINT16                                 OemHandle;
} OEM_DEFINE_GPNV_HANDLE;


//
//  This structure is for H2O 5.0
//
typedef struct {
  UINT16                                Handle;
  UINTN                                 GPNVBaseAddress;
  UINTN                                 GPNVSize;
} OEM_GPNV_MAP;

#pragma pack ()

#endif
