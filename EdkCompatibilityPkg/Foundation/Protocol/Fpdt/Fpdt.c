/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
/*++

Copyright (c) 2011, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:
  
    Fpdt.c

Abstract:

    Fpdt Performance Protocol

--*/

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (Fpdt)

EFI_GUID  gFpdtPerformanceProtocolGuid = FPDT_PERFORMANCE_PROTOCOL_GUID;

EFI_GUID_STRING(&gFpdtPerformanceProtocolGuid, "FPDT Performance Protocol", "FPDT Performance Protocol");
