/** @file

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

/*++
Module Name:

  OEMEfiStatusCode.h

Abstract:

  Status Code Definitions, according to Intel Platform Innovation Framework 
  for EFI Status Codes Specification
  Revision 0.60
  
  This file is used to add the extra definition which does not define in the EfiStatusCode.h
--*/


#ifndef _OEM_EfiStatusCode_H_
#define _OEM_EfiStatusCode_H_

#define EFI_SW_DXE_BS_PC_BOOT_OPTION_CHANGE           (EFI_SUBCLASS_SPECIFIC | 0x00000007)
#define EFI_SW_DXE_BS_PC_BOOT_OPTION_EXPECTED         (EFI_SUBCLASS_SPECIFIC | 0x00000008)

#endif
