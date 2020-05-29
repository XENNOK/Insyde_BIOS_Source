/** @file
  This OemService provides OEM to decide the method of recovery request. 
  When DXE loader found that the DXE-core of firmware volume is corrupt, it will force system to restart. 
  This service will be called to set the recovery requests before system restart. 
  To design the recovery requests according to OEM specification.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Library/IoLib.h>
#include <Library/PeiOemSvcKernelLib.h>
//[-start-120920-IB11410006-add]//
#include <Library/PcdLib.h>
//[-end-120920-IB11410006-add]//

//[-start-130124-IB10130023-add]//
#include <Library/CmosLib.h>
#include <Library/HobLib.h>
#include <ChipsetCmos.h>
#include <Ppi/BootInRecoveryMode.h>

#define CMOS_ISRECOVERY                           0x55
//[-end-130124-IB10130023-add]//

//
// data type definitions
//
//[-start-120920-IB11410006-remove]//
//#define PCH_GPIO_BASE_ADDRESS   0x0500
//[-end-120920-IB11410006-remove]//
#define R_GPIO_LVL              0x0C

//
// helper function prototypes 
//
extern
UINT32
EFIAPI
IoRead32 (
  IN      UINTN                 Port
  );

/**
  This OemService provides OEM to detect the recovery mode. 
  OEM designs the rule to detect that boot mode is recovery mode or not, 
  The rule bases on the recovery request which defined in OemService "OemSetRecoveryRequest".

  @param[in, out]  *IsRecovery        If service detects that the current system is recovery mode.
                                      This parameter will return TRUE, else return FALSE.

  @retval          EFI_MEDIA_CHANGED  The Recovery is detected. (*IsRecovery is set)
**/
EFI_STATUS
OemSvcDetectRecoveryRequest (
  IN OUT BOOLEAN                        *IsRecovery
  )
{
//[-start-130124-IB10130023-add]//
  UINT8                                 CmosValue;
	VOID                                  *Hob;
//[-end-130124-IB10130023-add]//
    *IsRecovery = FALSE;
	
//[-start-130124-IB10130023-add]//
 
	if (*IsRecovery == FALSE) {
	  CmosValue = ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, RecoveryFlag);
	  if (CmosValue == CMOS_ISRECOVERY) {
	    *IsRecovery = TRUE;
			BuildGuidDataHob (
        &gEfiPeiBootInRecoveryModePpiGuid,
        IsRecovery,
        sizeof (BOOLEAN)
        );
		  CmosValue = 0xFF;
	    WriteExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, RecoveryFlag, CmosValue);
	  }
	}
	
  Hob = GetFirstGuidHob (&gEfiPeiBootInRecoveryModePpiGuid);
	if (Hob != NULL) {
		*IsRecovery = TRUE;
	}
//[-end-130124-IB10130023-add]//
  return EFI_MEDIA_CHANGED;
} 
