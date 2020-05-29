/** @file
  Header file for Reference code Firmware Version Info Interface Lib implementation.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/
#ifndef _RC_FVI_LIBRARY_IMPLEMENTATION_H_
#define _RC_FVI_LIBRARY_IMPLEMENTATION_H_


#include "RcFviDxeLib.h"


/**
  Publish the Reference code version info as per Firmware Version Info (FVI) Interface Spec v0.7
  using MiscSubClass Data Hub.

  @param[in] Event                Event whose notification function is being invoked.
  @param[in] Context              Pointer to the notification functions context, which is implementation dependent.

  @retval None
**/
VOID
EFIAPI
DataHubCallback (
  IN EFI_EVENT  Event,
  IN VOID       *Context
  );
#endif
