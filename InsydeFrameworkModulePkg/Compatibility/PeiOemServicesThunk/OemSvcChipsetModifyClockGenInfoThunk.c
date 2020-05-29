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

#include "PeiOemServicesThunk.h"
#include <Library/PeiOemSvcKernelLib.h>

/**
  When PEI begins to initialize clock generator, 
  this OemService sends the clock generator information to ProgClkGen driver. 
  According to the OEM specification, add the clock generator information to this service.
 
  @param  *SlaveAddress         SlaveAddress of Clock generator.
  @param  *StartOffset          The command of Smbus for this clock generator.
  @param  *BigTable             Clock generator information table. The detail of this 
                                table refers to the document "ClkGen Porting Guide_V0.7".
  @param  *RealPlatformId       Real platform ID, if no platform ID that need to set 
                                NounPlatformId (0xFF).
  @param  *SizeInfo             Clock generator information table size.

  @retval EFI_SUCCESS           If ChipsetModifyClockGenInfo run success.
**/
EFI_STATUS
OemSvcChipsetModifyClockGenInfoThunk (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINT8                                 *SlaveAddress;
  UINT8                                 *StartOffset;
  CLOCK_GEN_UNION_INFO                  **BigTable;
  UINT16                                *RealPlatformId;
  UINT16                                *SizeInfo;
  EFI_STATUS                            Status;
  
  VA_START (Marker, NumOfArgs);
  SlaveAddress   = VA_ARG (Marker, UINT8 *);
  StartOffset    = VA_ARG (Marker, UINT8 *);
  BigTable       = VA_ARG (Marker, CLOCK_GEN_UNION_INFO **);
  RealPlatformId = VA_ARG (Marker, UINT16 *);
  SizeInfo       = VA_ARG (Marker, UINT16 *);
  VA_END (Marker); 

  //
  // make a call to PeiOemSvcKernelLib internally
  //
  Status = OemSvcChipsetModifyClockGenInfo (
             SlaveAddress,
             StartOffset,
             BigTable,
             RealPlatformId,
             SizeInfo
             );

  return Status;
}

