/** @file
  Defines and prototypes for the TDT driver.
  This driver implements the TDT protocol for Theft Deterrence Technology.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _EFI_SMM_TDT_PROTOCOL_H_
#define _EFI_SMM_TDT_PROTOCOL_H_
//
// Define the protocol GUID
//
#define EFI_SMM_TDT_PROTOCOL_GUID \
  { 0xf512d5dd, 0x5298, 0x4d4f, 0xa4, 0xd0, 0x83, 0xbc, 0x8c, 0xda, 0x79, 0xbd }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID gEfiSmmTdtProtocolGuid;
//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_SMM_TDT_PROTOCOL  EFI_SMM_TDT_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *EFI_SMM_TDT_GET_TIMER_INFO) (
  IN OUT UINT32     *Interval,
  IN OUT UINT32     *TimeLeft
  );

typedef
EFI_STATUS
(EFIAPI *EFI_SMM_TDT_GET_TDT_MERULE) (
  IN OUT UINT8      *TdtState,
  IN OUT UINT8      *TdtLastTheftTrigger,
  IN OUT UINT16     *TdtLockState,
  IN OUT UINT16     *TdtAmPref
  );

//
// Protocol definition
//

typedef struct _EFI_SMM_TDT_PROTOCOL {
  EFI_SMM_TDT_GET_TIMER_INFO                SmmGetTimerInfo;
  EFI_SMM_TDT_GET_TDT_MERULE                SmmGetTdtMeRule;
} EFI_SMM_TDT_PROTOCOL;


#endif
