/** @file
  When PEI begins to initialize clock generator, 
  this OemService sends the clock generator information to ProgClkGen driver. 
  According to the OEM specification, add the clock generator information to this service.

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

#include <Library/PeiOemSvcKernelLib.h>

/**
  When PEI begins to initialize clock generator, 
  this OemService sends the clock generator information to ProgClkGen driver. 
  According to the OEM specification, add the clock generator information to this service.
 
  @param[out]  *SlaveAddress         SlaveAddress of Clock generator.
  @param[out]  *StartOffset          The command of Smbus for this clock generator.
  @param[out]  *BigTable             Clock generator information table. The detail of this 
                                     table refers to the document "ClkGen Porting Guide_V0.7".
  @param[out]  *RealPlatformID       Real platform ID, if no platform ID that need to set 
                                     NounPlatformId (0xFF).
  @param[out]  *SizeInfo             Clock generator information table size.

  @retval      EFI_UNSUPPORTED       Returns unsupported by default.
  @retval      EFI_SUCCESS           The service is customized in the project.
  @retval      EFI_MEDIA_CHANGED     The value of IN OUT parameter is changed. 
  @retval      Others                Depends on customization.
**/
EFI_STATUS
OemSvcChipsetModifyClockGenInfo (
  OUT UINT8                                 *SlaveAddress,
  OUT UINT8                                 *StartOffset,
  OUT CLOCK_GEN_UNION_INFO                  **BigTable,
  OUT UINT16                                *RealPlatformId,
  OUT UINT16                                *SizeInfo
  )
{
  /*++
    Todo:
      Get all of arguments by Macro VA_ARG.
  __*/
  
  return EFI_UNSUPPORTED;
}

