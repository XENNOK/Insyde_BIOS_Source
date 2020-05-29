/** @file
 DXE IPMI BDS library header file.

 This file contains functions prototype that can be implemented by OEM to fit
 their requirements.

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


#ifndef _DXE_IPMI_BDS_LIB_H_
#define _DXE_IPMI_BDS_LIB_H_


#include <Uefi.h>

#include <Library/MemoryAllocationLib.h>


/**
 This function should be called after IpmiMiscDxe process boot options so that
 it can return correct BootNext value.

 @param[in]         BootNext            Boot device number for Boot#### that will boot at this time.

 @retval EFI_SUCCESS                    Update boot device number success.
 @retval EFI_UNSUPPORTED                Boot device number is not set.
*/
EFI_STATUS
IpmiSetBootNext (
  IN  UINT16         **BootNext
  );


#endif

