/** @file

  ME Platform Policy for ME PEIMs

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

#ifndef _PEI_ME_PLATFORM_POLICY_PPI_H_
#define _PEI_ME_PLATFORM_POLICY_PPI_H_

///
/// Include other definitions
///
///
/// ME policy provided by platform for PEI phase
/// The Platform Policy PPI returns the Intel ME feature set in PEI phase
///
#define PEI_ME_PLATFORM_POLICY_PPI_GUID \
  { 0x7AE3CEB7, 0x2EE2, 0x48FA, { 0xAA, 0x49, 0x35, 0x10, 0xBC, 0x83, 0xCA, 0xBF }}

///
/// Extern the GUID for PPI users.
///
extern EFI_GUID gPeiMePlatformPolicyPpiGuid;

//
// Revision
//
#define PEI_ME_PLATFORM_POLICY_PPI_REVISION_1 1
#define PEI_ME_PLATFORM_POLICY_PPI_REVISION_2 2

///
///
/// ME policy provided by platform for PEI phase
/// The Platform Policy PPI returns the Intel ME feature set in PEI phase
///
typedef struct _PEI_ME_PLATFORM_POLICY_PPI {
  UINT8 Revision;                 ///< Revision for the protocol structure
  UINT8 Reserved1 : 1;            ///< Reserved for Intel internal use
  UINT8 Reserved2 : 1;            ///< Reserved for Intel internal use
  UINT8 Reserved : 6;             ///< Reserved for Intel internal use
  UINT8 FTpmSwitch;
} PEI_ME_PLATFORM_POLICY_PPI;

#endif
