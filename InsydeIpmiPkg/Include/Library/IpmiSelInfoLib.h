/** @file
 H2O IPMI SEL Info library header file.

 This file contains functions prototype that can easily retrieve SEL Info via
 using H2O IPMI SEL Data protocol.

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

#ifndef _IPMI_SEL_INFO_H_
#define _IPMI_SEL_INFO_H_


#include <Protocol/H2OIpmiSelInfoProtocol.h>


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
  );


#endif

