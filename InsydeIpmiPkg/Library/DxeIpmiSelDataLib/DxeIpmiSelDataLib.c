/** @file
 H2O IPMI SEL Data library implement code.

 This c file contains SEL Data library instance in DXE phase.

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


#include <Library/UefiBootServicesTableLib.h>
#include <Library/IpmiSelDataLib.h>


H2O_IPMI_SEL_DATA_PROTOCOL  *mIpmiSelData = NULL;


/**
 Get Sel Data of SelId.

 @param[in]         SelId               The SEL record ID to get which sel data.
 @param[out]        SelData             A buffer to H2O_IPMI_SEL_ENTRY.

 @retval EFI_SUCCESS                    Get SEL data success.
 @retval EFI_UNSUPPORTED                Get SEL data fail or speificed Id does not exist.
*/
EFI_STATUS
IpmiLibGetSelData (
  IN  UINT16                            SelId,
  OUT H2O_IPMI_SEL_ENTRY                *SelData
  )
{
  EFI_STATUS                            Status;

  if (mIpmiSelData == NULL) {
    Status = gBS->LocateProtocol (&gH2OIpmiSelDataProtocolGuid, NULL, (VOID **)&mIpmiSelData);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return mIpmiSelData->GetSelData (mIpmiSelData, SelId, SelData);

}


/**
 Get All Sel entry data.

 @param[out]        SelCount            Number of log entries in SEL.
 @param[out]        SelData             A H2O_IPMI_SEL_ENTRY array to store all H2O_IPMI_SEL_ENTRY.

 @retval EFI_SUCCESS                    Get SEL data success.
 @retval EFI_UNSUPPORTED                Get SEL data fail.
*/
EFI_STATUS
IpmiLibGetAllSelData (
  OUT UINT16                            *SelCount,
  OUT H2O_IPMI_SEL_ENTRY                **SelData
  )
{
  EFI_STATUS                            Status;

  if (mIpmiSelData == NULL) {
    Status = gBS->LocateProtocol (&gH2OIpmiSelDataProtocolGuid, NULL, (VOID **)&mIpmiSelData);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return mIpmiSelData->GetAllSelData (mIpmiSelData, SelCount, SelData);

}

