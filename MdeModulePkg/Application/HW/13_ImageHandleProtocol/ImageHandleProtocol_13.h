/** @file
  ImageHandleProtocol_13 H Source File

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _Image_Handle_Protocol_13_H_
#define _Image_Handle_Protocol_13_H_

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Library/ShellLib.h>
#include <Library/HandleParsingLib.h>

typedef struct {
  INT16 column;         /// cursor column
  INT16 row;            /// cursor row
} POSITION;

typedef struct {
  UINT64    value;      /// write value
  POSITION  position;   /// cursor position
} WRITE_BUFFER;

EFI_STATUS 
SearchAllProtocol (
  VOID
  );

EFI_STATUS 
SearchProtocolByGuid (
  EFI_GUID *ProtocolGuid
  );

EFI_STATUS 
SearchProtocolGuidByName (
  CHAR16 *Str
  );

UINT32 
InputValue (
  WRITE_BUFFER *WriteBuffer,
  POSITION *CursorPosition
  );

UINT32 
InputWord (
  WRITE_BUFFER *WriteBuffer, 
  POSITION *CursorPosition
  );

EFI_GUID 
ParseGuid (
  WRITE_BUFFER *WriteBuffer
  );

VOID 
SetMainPageAppearance (
  VOID
  );

VOID 
InitializeArray(
  WRITE_BUFFER *WriteBuffer
  );

#endif