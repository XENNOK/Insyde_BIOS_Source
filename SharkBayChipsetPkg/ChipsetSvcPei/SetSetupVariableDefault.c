/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcSetSetupVariableDefault().
 The function PeiCsSvcSetSetupVariableDefault() use chipset services to set defaul
 variables when variable not found

***************************************************************************
* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Library/CmosLib.h>
#include <ChipsetSetupConfig.h>
#include <ChipsetCmos.h>

/**
 When variable not found, to set default variables.

 @param[in]         SystemConfiguration A pointer to setup variables

 @return            EFI_SUCCESS         Always return successfully
*/
EFI_STATUS
SetSetupVariableDefault (
  IN VOID                      *SystemConfiguration
  )
{
  CHIPSET_CONFIGURATION         *SystemConfig;
  
  SystemConfig = (CHIPSET_CONFIGURATION *)SystemConfiguration;
  if (SystemConfig->SetupVariableInvalid) {
    //
    // Please sync with default settings in VFR files.
    //

//[-start-120316-IB06460376-remove]//
//    SystemConfiguration->AtPbaSupport                 = 1;
//[-end-120316-IB06460376-remove]//
//[-start-120731-IB10820094-modify]//
  if (FeaturePcdGet(PcdAntiTheftSupported)) {
    SystemConfig->AtState                      = 0;
  }
//[-start-120316-IB06460376-remove]//
//#ifdef ANTI_THEFT_TESTMENU_SUPPORT
//    SystemConfiguration->AtAssertStolen               = 0;
//#endif
//[-end-120316-IB06460376-remove]//
//[-start-121128-IB03780468-remove]//
//  if (FeaturePcdGet (PcdXtuSupported)) {
//    SystemConfig->XECap = 0;
//  }
//[-end-121128-IB03780468-remove]//
//[-end-120731-IB10820094-modify]//
    WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, SetupNVFlag, NV_NOT_FOUND);
  } else {
    WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, SetupNVFlag, NV_AVAILABLE);
  }
  return EFI_SUCCESS;
}
