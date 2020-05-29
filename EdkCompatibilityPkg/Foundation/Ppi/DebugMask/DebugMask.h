/*++

Copyright (c) 2010, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

  DebugMask.h

Abstract:

  This PPI is used to abstract the Debug Mask services for 
  the specific PEIM.

--*/

#ifndef _DEBUG_MASK_PPI_H_
#define _DEBUG_MASK_PPI_H_

//
//3bd930fd-f823-4948-8691-98e6fe36ace2
//
#define EFI_DEBUG_MASK_PPI_GUID \
  { 0x3bd930fd, 0xf823, 0x4948, 0x86, 0x91, 0x98, 0xe6, 0xfe, 0x36, 0xac, 0xe2 }

//
// Forward reference for pure ANSI compatability
//
EFI_FORWARD_DECLARATION (EFI_DEBUG_MASK_PPI);

//
// DebugMask PPI definition
//
typedef struct _EFI_DEBUG_MASK_PPI {
  UINTN                             ImageDebugMask;
} EFI_DEBUG_MASK_PPI;

extern EFI_GUID gEfiDebugMaskPpiGuid;

#endif
