/** @file
 H2O IPMI SEL Info library implement code.

 This c file contains SEL Info library instance in SMM phase.

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


#include <Library/SmmServicesTableLib.h>
#include <Library/IpmiSelInfoLib.h>


H2O_IPMI_SEL_INFO_PROTOCOL  *mIpmiSelInfo = NULL;


/**
 Convert SEL entry to string based SEL entry read from "Get SEL Entry" IPMI Command.

 @param[in]         SelEntry            SEL entry to be coverted.
 @param[out]        SelInfo             Structure contains SEL entry information description strings.
                                        Callers should pass in structure buffer.

 @retval EFI_SUCCESS                    Convert success.
 @retval EFI_UNSUPPORTED                Event type is unspecified or sensor type is out of Range.
 @retval EFI_INVALID_PARAMETER          If one of arguments is NULL pointer.
*/
EFI_STATUS
IpmiLibGetSelInfo (
  IN  VOID                              *SelEntry,
  OUT SEL_INFO_STRUCT                   *SelInfo
  )
{
  EFI_STATUS                            Status;

  if (mIpmiSelInfo == NULL) {
    Status = gSmst->SmmLocateProtocol (&gH2OSmmIpmiSelInfoProtocolGuid, NULL, (VOID **)&mIpmiSelInfo);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return mIpmiSelInfo->GetSelInfo (mIpmiSelInfo, SelEntry, SelInfo);

}

