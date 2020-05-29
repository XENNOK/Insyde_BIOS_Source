//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//; 
//; Module Name:
//; 
//;   DriverHealth.c
//; 
//; Abstract:
//; 
//;   EFI_DRIVER_HEALTH_PROTOCOL as defined in UEFI 2.3.1
//; 
/** @file
  EFI Driver Health Protocol definitions.

  When installed, the Driver Health Protocol produces a collection of services that allow
  the health status for a controller to be retrieved. If a controller is not in a usable
  state, status messages may be reported to the user, repair operations can be invoked,
  and the user may be asked to make software and/or hardware configuration changes.
  
  The Driver Health Protocol is optionally produced by a driver that follows the 
  EFI Driver Model.  If an EFI Driver needs to report health status to the platform, 
  provide warning or error messages to the user, perform length repair operations, 
  or request the user to make hardware or software configuration changes, then the 
  Driver Health Protocol must be produced.
 
  A controller that is managed by driver that follows the EFI Driver Model and 
  produces the Driver Health Protocol must report the current health of the 
  controllers that the driver is currently managing.  The controller can initially 
  be healthy, failed, require repair, or require configuration.  If a controller 
  requires configuration, and the user make configuration changes, the controller 
  may then need to be reconnected or the system may need to be rebooted for the 
  configuration changes to take affect. 

  Copyright (c) 2009 - 2011, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials                          
  are licensed and made available under the terms and conditions of the BSD License         
  which accompanies this distribution.  The full text of the license may be found at        
  http://opensource.org/licenses/bsd-license.php                                            

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

  @par Revision Reference:
  This Protocol is defined in UEFI Specification 2.3d 

**/

#include "Tiano.h"

#include EFI_PROTOCOL_DEFINITION (DriverHealth)

EFI_GUID  gEfiDriverHealthProtocolGuid = EFI_DRIVER_HEALTH_PROTOCOL_GUID;

EFI_GUID_STRING (&gEfiDriverHealthProtocolGuid, "Driver Health", "");

