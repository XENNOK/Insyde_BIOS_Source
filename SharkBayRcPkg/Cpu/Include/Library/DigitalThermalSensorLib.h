/** @file

  Defines and prototypes for the Digital Thermal Sensor SMM driver                                    

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/


#ifndef _DIGITAL_THERMAL_SENSOR_LIB_H_
#define _DIGITAL_THERMAL_SENSOR_LIB_H_

/**
  Prepare data and protocol for Dts Hooe Lib

  @param[in] None

  @retval EFI_SUCCESS - Initialize complete

*/
EFI_STATUS
InitializeDtsHookLib (
  VOID
  );

/**
  Platform may need to register some data to private data structure before generate 
  software SMI or SCI.

  @param[in] None


*/
VOID
PlatformHookBeforeGenerateSCI (
  VOID
  );

/**
  Read CPU temperature from platform diode

  @param[in] None

  @retval TemperatureOfDiode   -  Return the CPU temperature of platform diode

*/
UINT8
ReadPlatformThermalDiode (
  VOID
  );

/**
  When system temperature out of specification, do platform specific programming to prevent 
  system damage.

  @param[in] None

*/

VOID
PlatformEventOutOfSpec (
  VOID
  );

#endif
