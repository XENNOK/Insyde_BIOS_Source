/** @file

  Interface definition details for platform hook support to ME module.

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

#ifndef _PLATFORM_ME_HOOK_PPI_H_
#define _PLATFORM_ME_HOOK_PPI_H_

///
/// ME Hook provided by platform for PEI phase
/// This ppi provides an interface to hook reference code by OEM.
///
#define PLATFORM_ME_HOOK_PPI_GUID \
  { 0xe806424f, 0xd425, 0x4b1a, { 0xbc, 0x26, 0x5f, 0x69, 0x03, 0x89, 0xa1, 0x5a }}

extern EFI_GUID gPlatformMeHookPpiGuid;

typedef struct _PLATFORM_ME_HOOK_PPI PLATFORM_ME_HOOK_PPI;

///
/// Revision
///
#define PLATFORM_ME_HOOK_PPI_REVISION 1

/**
  Platform hook before BIOS sends Global Reset Heci Message to ME

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 Pointer to this PLATFORM_ME_HOOK_PPI

  @retval EFI Status Code
**/
typedef
EFI_STATUS
(EFIAPI *PLATFORM_ME_HOOK_PRE_GLOBAL_RESET) (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       PLATFORM_ME_HOOK_PPI        * This
  );

///
/// ME Hook provided by platform for PEI phase
/// This ppi provides an interface to hook reference code by OEM.
///
struct _PLATFORM_ME_HOOK_PPI {
  ///
  /// Revision for the ppi structure
  ///
  UINT8                             Revision;
  ///
  /// Function pointer for the hook called before BIOS sends Global Reset Heci Message to ME
  //
  PLATFORM_ME_HOOK_PRE_GLOBAL_RESET PreGlobalReset;
};

#endif
