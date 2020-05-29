/** @file
 H2O IPMI hob library header file.

 This file contains functions prototype that can retrive H2O IPMI hob easily.

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

#ifndef _IPMI_HOB_LIB_H_
#define _IPMI_HOB_LIB_H_


#include <Guid/H2OIpmiHob.h>


/**
 Get H2OIpmiHob created in H2O IPMI PEIM.

 @param[out]        IpmiHob             A pointer to H2O IPMI HOB.

 @retval EFI_SUCCESS                    Get Hob success.
 @retval !EFI_SUCCESS                   Cannot get HOB.
*/
EFI_STATUS
IpmiLibGetIpmiHob (
  OUT H2O_IPMI_HOB                      **IpmiHob
  );


#endif

