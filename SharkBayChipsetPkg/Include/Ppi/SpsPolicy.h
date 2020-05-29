/** @file

  Server Platform Services (SPS) Policy for SPS ME PEIMs.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SPS_POLICY_PPI_H_
#define _SPS_POLICY_PPI_H_

#include <SpsPolicy.h>

///
/// Define the PPI GUID
///
/// {C5AF3434-9906-4b81-BDAE-9DA153D014ED}
#define SPS_POLICY_PPI_GUID \
  { 0xC5AF3434, 0x9906, 0x4B81, { 0xBD, 0xAE, 0x9D, 0xA1, 0x53, 0xD0, 0x14, 0xED }}

typedef struct _SPS_POLICY             SPS_POLICY_PPI;

///
/// Extern the GUID for PPI users.
///
extern EFI_GUID gSpsPolicyPpiGuid;

#endif
