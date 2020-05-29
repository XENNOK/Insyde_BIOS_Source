/** @file

  Header file for the SwitchableGraphics Pei driver.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SWITCHABLE_GRAPHICS_PEI_H_
#define _SWITCHABLE_GRAPHICS_PEI_H_

///===============================================
///  MdePkg/Include/
///===============================================
#include <CpuRegs.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>
#include <Library/CpuPlatformLib.h>
#include <Ppi/Stall.h>


///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelPch/LynxPoint/Include/
///===============================================
#include <PchRegs.h>
///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelMch/SystemAgent/
///===============================================
#include <Guid/SaDataHob.h>
#include <Ppi/SaPlatformPolicy.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef HIGH
#define HIGH  1
#endif

#ifndef LOW
#define LOW 0
#endif

#define SG_DELAY_HOLD_RST    100 * STALL_ONE_MILLI_SECOND
#define SG_DELAY_PWR_ENABLE  300 * STALL_ONE_MILLI_SECOND

/**
  SG GPIO Write

  @param[in] PeiServices - General purpose services available to every PEIM
  @param[in] CpuFamilyId - Specifies the CPU family
  @param[in] GpioAddress - GPIO base address
  @param[in] Value       - PCH GPIO number
  @param[in] Level       - Write SG GPIO value (0/1)

  @retval none
**/
VOID
GpioWrite (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  CPU_FAMILY                CpuFamilyId,
  IN       UINT16                      GpioAddress,
  IN       UINT8                       Value,
  IN       BOOLEAN                     Level
  );

/**
  SG GPIO Read

  @param[in] PeiServices - General purpose services available to every PEIM
  @param[in] CpuFamilyId - Specifies the CPU family
  @param[in] GpioAddress - GPIO base address
  @param[in] Value       - PCH GPIO number

  @retval GPIO read value (0/1)
**/
BOOLEAN
GpioRead (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  CPU_FAMILY                CpuFamilyId,
  IN       UINT16                      GpioAddress,
  IN       UINT8                       Value
  );
#endif
