//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

/*++

Copyright (c) 2006, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

  SmmDriverLib.h

Abstract:

     SMM lib to support EFI drivers.

--*/

#ifndef _SMM_DRIVER_LIB_H_
#define _SMM_DRIVER_LIB_H_
#include EFI_PROTOCOL_DEFINITION (DEBUGMASK)

EFI_STATUS
InsydeEfiInitializeSmmDriverLib (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable,
  IN OUT BOOLEAN          *InSmm
  )
/*++

Routine Description:

  Intialize Smm Driver Lib if it has not yet been initialized.

Arguments:

  ImageHandle     - The firmware allocated handle for the EFI image.

  SystemTable     - A pointer to the EFI System Table.

  InSmm           - Currently in SMM mode or not?

Returns:

  EFI_STATUS always returns EFI_SUCCESS

--*/
;

typedef struct {
  EFI_PHYSICAL_ADDRESS        SmmRegionBase1;
  EFI_PHYSICAL_ADDRESS        SmmRegionBase2;
  UINT32                      SmmRegionSize1;
  UINT32                      SmmRegionSize2;
} SMM_REGION_BASE_INFO;

#endif