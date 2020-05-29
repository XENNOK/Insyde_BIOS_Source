/** @file

  AMT Platform Policy for AMT PEIMs

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

#ifndef _PEI_AMT_PLATFORM_POLICY_PPI_H_
#define _PEI_AMT_PLATFORM_POLICY_PPI_H_

///
/// Include other definitions
///
///
/// Intel AMT Platform Policy PPI GUID
/// This PPI provides an interface to get the current Intel AMT SKU information
///
#define PEI_AMT_PLATFORM_POLICY_PPI_GUID \
  { 0xB4A1208E, 0x4D9A, 0x4EA2, { 0x9D, 0x6B, 0xE4, 0x1A, 0x61, 0xE6, 0xC5, 0xAC }}

///
/// Extern the GUID for PPI users.
///
extern EFI_GUID gPeiAmtPlatformPolicyPpiGuid;

///
/// Revision
///
#define PEI_AMT_PLATFORM_POLICY_PPI_REVISION_1  1
///
/// Add AsfEnabled, and ManageabilityMode
///
#define PEI_AMT_PLATFORM_POLICY_PPI_REVISION_2  2
///
/// Added FWProgress
///
#define PEI_AMT_PLATFORM_POLICY_PPI_REVISION_3  3
///
/// Cleanup
///
#define PEI_AMT_PLATFORM_POLICY_PPI_REVISION_4  4
///
/// Cleanup
///
#define PEI_AMT_PLATFORM_POLICY_PPI_REVISION_5  5

///
/// Intel AMT Platform Policy PPI
/// The Intel AMT Platform Policy PPI returns the Intel ME feature set in PEI phase
///
typedef struct _PEI_AMT_PLATFORM_POLICY_PPI {
  UINT8   Revision;               ///< Policy structure revision number
  UINT8   iAmtEnabled : 1;        ///< Intel AMT features enabled/disable
  UINT8   WatchDog : 1;           ///< Asf Watch Dog timer message enabled/disable
  UINT8   Reserved : 1;
  UINT8   AsfEnabled : 1;         ///< Asf features enable/disable
  UINT8   ManageabilityMode : 1;  ///< Manageability Mode, 0: Off, 1:On
  UINT8   Reserved1 : 1;
  UINT16  WatchDogTimerOs;        ///< Watch Dog timeout value for OS
  UINT16  WatchDogTimerBios;      ///< Watch Dog timeout value for BIOS
  UINT8   FWProgress;             ///< Progress Event option enable/disable
} PEI_AMT_PLATFORM_POLICY_PPI;

#endif
