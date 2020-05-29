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
//; Module Name:
//;
//;   ExitFunc.h
//;
//; Abstract:
//;
//;   Header file for Exit Menu relative function
//;


#ifndef _EXIT_FUNC_H_
#define _EXIT_FUNC_H_
#include "SetupUtilityLibCommon.h"
#include "EfiPrintLib.h"

EFI_STATUS
LoadCustomOption (
  IN EFI_FORM_CALLBACK_PROTOCOL         *This
  );

EFI_STATUS
SaveCustomOption (
  IN EFI_FORM_CALLBACK_PROTOCOL         *This
  );

EFI_STATUS
DiscardChange (
  IN EFI_FORM_CALLBACK_PROTOCOL         *This
  );

#endif