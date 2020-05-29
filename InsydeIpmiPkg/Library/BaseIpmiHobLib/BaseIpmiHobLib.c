/** @file
 H2O IPMI HOB library implement code.

 This c file contains H2O IPMI HOB library instance for all phase.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include <Uefi.h>
#include <Guid/HobList.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#include <Library/HobLib.h>
#include <Library/IpmiHobLib.h>


/**
 Get H2OIpmiHob created in H2O IPMI PEIM.

 @param[out]        IpmiHob             A pointer to H2O IPMI HOB.

 @retval EFI_SUCCESS                    Get Hob success.
 @retval !EFI_SUCCESS                   Cannot get HOB.
*/
EFI_STATUS
IpmiLibGetIpmiHob (
  OUT H2O_IPMI_HOB                      **IpmiHob
  )
{
  VOID                                  *HobList;
  UINT8                                 *Temp;


  HobList = GetHobList ();
  if (HobList == NULL) {
    return EFI_UNSUPPORTED;
  }

  *IpmiHob = NULL;

  //
  // If there is an IPMI Hob then put in context, otherwise leave
  //
  Temp = GetNextGuidHob (&gH2OIpmiHobGuid, HobList);
  if (Temp == NULL) {
   return EFI_NOT_FOUND;
  }

  *IpmiHob = (H2O_IPMI_HOB*)(Temp + sizeof (EFI_HOB_GUID_TYPE));

  return EFI_SUCCESS;

}

