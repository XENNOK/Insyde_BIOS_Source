/** @file
  Serial driver for standard UARTS on an ISA bus.

;******************************************************************************
;* Copyright (c) 2006 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/

#ifndef _FILE_TEXT_OUT_HOOK_H_
#define _FILE_TEXT_OUT_HOOK_H_



VOID
TextGetBackground (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  );

VOID
TextPutBackground (
  UINTN   X,
  UINTN   Y,
  UINTN   Width,
  UINTN   Height,
  VOID    *Buffer
  );

EFI_STATUS
TextGetScreenSize (
  OUT  UINTN                                      *SizeOfX,
  OUT  UINTN                                      *SizeOfY
  );
  
EFI_STATUS
InitializeTextOutHook (
  VOID
  );
	
EFI_STATUS
FinalizeTextOutHook (
  VOID
  );
  
#endif // _SNAP_TEXT_OUT_HOOK_H_
