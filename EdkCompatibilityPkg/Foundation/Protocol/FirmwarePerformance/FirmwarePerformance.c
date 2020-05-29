/*++

Copyright (c) 2011, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:
  
    FirmwarePerformance.c

Abstract:

    Firmware Performance Protocol

--*/

#include "Tiano.h"
#include EFI_PROTOCOL_DEFINITION (FirmwarePerformance)

EFI_GUID  gFirmwarePerformanceProtocolGuid = FIRMWARE_PERFORMANCE_PROTOCOL_GUID;

EFI_GUID_STRING(&gFirmwarePerformanceProtocolGuid, "Firmware Performance Protocol", "Firmware Performance Protocol");
