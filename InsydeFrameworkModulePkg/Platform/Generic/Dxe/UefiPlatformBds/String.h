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

Copyright (c) 2004, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  string.h

Abstract:
  
  String support

Revision History

--*/

#ifndef _STRING_H_
#define _STRING_H_

#include "UefiIfrLibrary.h"

extern EFI_HII_HANDLE gStringPackHandle;

//
// This is the VFR compiler generated header file which defines the
// string identifiers.
//
#include "BdsStrDefs.h"
#include "BvdtLib.h"

extern UINT8  BdsStrings[];

//
// String Definition Guid for BDS Platform
//
#define EFI_BDS_PLATFORM_GUID \
  { \
    0x7777E939, 0xD57E, 0x4DCB, 0xA0, 0x8E, 0x64, 0xD7, 0x98, 0x57, 0x1E, 0x0F \
  }

CHAR16            *
GetStringById (
  IN  EFI_STRING_ID   Id
  );

EFI_STATUS
InitializeStringSupport (
  VOID
  );

EFI_STATUS
CallFrontPage (
  VOID
  );

VOID
UpdateBvdtToHii (
  IN EFI_HII_HANDLE     HiiHandle
  );

EFI_STATUS
GetProducerString (
  IN      EFI_GUID                  *ProducerGuid,
  IN      EFI_STRING_ID             Token,
  OUT     CHAR16                    **String
  );

#endif // _STRING_H_
