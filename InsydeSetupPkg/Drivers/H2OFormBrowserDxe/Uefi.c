/** @file
 General function.
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

#include "InternalH2OFormBrowser.h"

/**
 Formbrowser free buffer.

 @param [in] Buffer         Buffer which free.

**/
VOID
FBFreePool (
  IN VOID **Buffer
  )
{
  if (Buffer != NULL && *Buffer != NULL) {
    gBS->FreePool (*Buffer);
    *Buffer = NULL;
  }
}