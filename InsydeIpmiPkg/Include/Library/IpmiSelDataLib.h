/** @file
 H2O IPMI SEL Data library header file.

 This file contains functions prototype that can easily retrieve SEL Data via
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

#ifndef _IPMI_SEL_DATA_H_
#define _IPMI_SEL_DATA_H_


#include <Protocol/H2OIpmiSelDataProtocol.h>


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
  );


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
  );


#endif

