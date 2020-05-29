/** @file

  Interface definition between MRC and SaInitPeim driver.

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

#ifndef _SA_PEI_INIT_H_
#define _SA_PEI_INIT_H_

///
/// Define the SA PEI Init PPI GUID
///
#define SA_PEI_INIT_PPI_GUID \
  { 0x09EA8911, 0xBE0D, 0x4230, { 0xA0, 0x03, 0xED, 0xC6, 0x93, 0xB4, 0x8E, 0x11 }}

///
/// Extern the GUID for PPI users.
///
extern EFI_GUID gSaPeiInitPpiGuid;

#endif
