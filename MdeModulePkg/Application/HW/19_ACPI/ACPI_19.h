/** @file
  ACPI_19 H Source File

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

#ifndef _ACPI_19_H_
#define _ACPI_19_H_

#include <Uefi.h>

#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>

#include <Guid/Acpi.h>

VOID
ShowAllSignature(
  VOID
  );

VOID
ShowFadtData(
  VOID
  );

VOID
Print12Byte (
  UINT64 Address
  );

VOID
ShowMainPage (
  VOID
  );

VOID
SetMainPageAppearance (
  VOID
  );

VOID
Press2Continue (
  VOID  
  );

#endif