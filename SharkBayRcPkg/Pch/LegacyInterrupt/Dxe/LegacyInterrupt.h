/** @file
  This code supports a the private implementation of the Legacy Interrupt protocol

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

/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  LegacyInterrupt.h

@brief
  This code supports a the private implementation 
  of the Legacy Interrupt protocol

**/
#ifndef LEGACY_INTERRUPT_H_
#define LEGACY_INTERRUPT_H_

#include <Protocol/LegacyInterrupt.h>
#include <PchAccess.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>

#define PIRQN           0x00  ///< PIRQ Null
#define PIRQA           0x60
#define PIRQB           0x61
#define PIRQC           0x62
#define PIRQD           0x63
#define PIRQE           0x68
#define PIRQF           0x69
#define PIRQG           0x6A
#define PIRQH           0x6B

#define MAX_PIRQ_NUMBER 8

/**
  Return the number of PIRQs supported by this chipset.

  @param[in] This                 Pointer to LegacyInterrupt Protocol
  @param[out] NumberPirqs         The pointer which point to the max IRQ number supported by this PCH.

  @retval EFI_SUCCESS             Legacy BIOS protocol installed
**/
EFI_STATUS
EFIAPI
GetNumberPirqs (
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL  *This,
  OUT UINT8                          *NumberPirqs
  );

/**
  Return PCI location of this device. $PIR table requires this info.

  @param[in] This                 Protocol instance pointer.
  @param[out] Bus                 PCI Bus
  @param[out] Device              PCI Device
  @param[out] Function            PCI Function

  @retval EFI_SUCCESS             Bus/Device/Function returned
**/
EFI_STATUS
EFIAPI
GetLocation (
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL  *This,
  OUT UINT8                          *Bus,
  OUT UINT8                          *Device,
  OUT UINT8                          *Function
  );

/**
  Read the given PIRQ register

  @param[in] This                 Pointer to LegacyInterrupt Protocol
  @param[in] PirqNumber           The Pirq register 0 = A, 1 = B etc
  @param[out] PirqData            Value read

  @retval EFI_SUCCESS             Decoding change affected.
  @retval EFI_INVALID_PARAMETER   Invalid PIRQ number
**/
EFI_STATUS
EFIAPI
ReadPirq (
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL  *This,
  IN  UINT8                          PirqNumber,
  OUT UINT8                          *PirqData
  );

/**
  Read the given PIRQ register

  @param[in] This                 Pointer to LegacyInterrupt Protocol
  @param[in] PirqNumber           The Pirq register 0 = A, 1 = B etc
  @param[in] PirqData             Value read

  @retval EFI_SUCCESS             Decoding change affected.
  @retval EFI_INVALID_PARAMETER   Invalid PIRQ number
**/
EFI_STATUS
EFIAPI
WritePirq (
  IN  EFI_LEGACY_INTERRUPT_PROTOCOL  *This,
  IN  UINT8                          PirqNumber,
  IN  UINT8                          PirqData
  );

#endif
