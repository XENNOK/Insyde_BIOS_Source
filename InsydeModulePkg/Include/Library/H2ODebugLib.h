/** @file
  H2ODebugLib include file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _H2ODEBUG_LIB_H_
#define _H2ODEBUG_LIB_H_

#include <Uefi.h>


VOID
DDTPrint (
  IN CHAR8                        *Format,
  ...
  );
  
#endif