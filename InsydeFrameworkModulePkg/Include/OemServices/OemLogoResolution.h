//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; ModuleName:
//;
//;   OemLogoResolution.h
//;
//;------------------------------------------------------------------------------
//;
//; Abstract:
//;
//;   

#ifndef _OEM_LOGO_RESOLUTION_DEFINE_H_
#define _OEM_LOGO_RESOLUTION_DEFINE_H_

typedef struct  {
  UINT32       LogoResolutionX;
  UINT32       LogoResolutionY;
  UINT32       SCUResolutionX;
  UINT32       SCUResolutionY;
} OEM_LOGO_RESOLUTION_DEFINITION;

#endif
