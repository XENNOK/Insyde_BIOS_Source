/**
  This file contains an 'Intel Peripheral Driver' and uniquely        
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your   
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2009 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  IobpAccess.c

@brief
  Program IOBP register.

**/
#include "PchPlatformLibrary.h"

/**
  Configures PCH IOBP

  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] Address              Address of the IOBP register block
  @param[in] AndMask              Mask to AND with the register
  @param[in] OrMask               Mask to OR with the register

  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_DEVICE_ERROR        Transaction fail
**/
EFI_STATUS
EFIAPI
ProgramIobp (
  IN UINT32                              RootComplexBar,
  IN UINT32                              Address,
  IN UINT32                              AndMask,
  IN UINT32                              OrMask
  )
{
  UINT32      Data32;
  UINT8       ResponseStatus;
  EFI_STATUS  Status;

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 7.1.4 IOSF SBI Programming
  /// Step 1 to Step 8
  ///
  Status = ReadIobp (RootComplexBar, Address, &Data32);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Step 9
  /// Update the SBI data accordingly
  ///
  Data32 &= AndMask;
  Data32 |= OrMask;
  ///
  /// Step 10
  /// Set RCBA + 2338h[15:8] = 00000111b
  ///
  MmioAndThenOr16 (
    (RootComplexBar + R_PCH_RCRB_IOBPS),
    (UINT16) (~B_PCH_RCRB_IOBPS_IOBPIA),
    (UINT16) V_PCH_RCRB_IOBPS_IOBPIA_W
    );
  ///
  /// Step 11
  /// Write RCBA + 2334h[31:0] with updated SBI data
  ///
  MmioWrite32 ((RootComplexBar + R_PCH_RCRB_IOBPD), Data32);

  ///
  /// Step 12
  /// Set RCBA + 233Ah[15:0] = F000h
  ///
  MmioWrite16 (
    (RootComplexBar + 0x233A),
    0xF000
    );

  ///
  /// Step 13
  /// Set RCBA + 2338h[0] = 1b
  ///
  MmioOr16 (
    (RootComplexBar + R_PCH_RCRB_IOBPS),
    (UINT16) BIT0
    );

  ///
  /// Step 14
  /// Poll RCBA + Offset 2338h[0] = 0b
  ///
  while (MmioRead8 (RootComplexBar + R_PCH_RCRB_IOBPS) & B_PCH_RCRB_IOBPS_BUSY);

  ///
  /// Step 15
  /// Check if RCBA + 2338h[2:1] = 00b for successful transaction
  ///
  ResponseStatus = MmioRead8 (RootComplexBar + R_PCH_RCRB_IOBPS) & B_PCH_RCRB_IOBPS;
  if (ResponseStatus == V_PCH_RCRB_IOBPS_SUCCESS) {
    return EFI_SUCCESS;
  } else {
    return EFI_DEVICE_ERROR;
  }
}

/**
  Read data from PCH IOBP register block

  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] Address              Address of the IOBP register block
  @param[out] Data                Data contain in the IOBP register block

  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_DEVICE_ERROR        Transaction fail
**/
EFI_STATUS
EFIAPI
ReadIobp (
  IN UINT32                              RootComplexBar,
  IN UINT32                              Address,
  OUT UINT32                             *Data
  )
{
  UINT8 ResponseStatus;

  ///
  /// Step 1 Poll RCBA + 2338[0] = 0b
  ///
  while (MmioRead8 (RootComplexBar + R_PCH_RCRB_IOBPS) & B_PCH_RCRB_IOBPS_BUSY);

  ///
  /// Step 2
  /// Write RCBA + Offset 2330h[31:0] with IOBP register address
  ///
  MmioWrite32 ((RootComplexBar + R_PCH_RCRB_IOBPIRI), Address);
  ///
  /// Step 3
  /// Set RCBA + 2338h[15:8] = 00000110b
  ///
  MmioAndThenOr16 (
    (RootComplexBar + R_PCH_RCRB_IOBPS),
    (UINT16) (~B_PCH_RCRB_IOBPS_IOBPIA),
    (UINT16) V_PCH_RCRB_IOBPS_IOBPIA_R
    );
  ///
  /// Step 4
  /// Set RCBA + 233Ah[15:0] = F000h
  ///
  MmioWrite16 (
    (RootComplexBar + 0x233A),
    0xF000
    );
  ///
  /// Step 5
  /// Set RCBA + 2338h[0] = 1b
  ///
  MmioOr16 (
    (RootComplexBar + R_PCH_RCRB_IOBPS),
    (UINT16) BIT0
    );

  ///
  /// Step 6
  /// Poll RCBA + Offset 2338h[0] = 0b, Polling for Busy bit
  ///
  while (MmioRead8 (RootComplexBar + R_PCH_RCRB_IOBPS) & B_PCH_RCRB_IOBPS_BUSY);

  ///
  /// Step 7
  /// Check if RCBA + 2338h[2:1] = 00b for successful transaction
  ///
  ResponseStatus = MmioRead8 (RootComplexBar + R_PCH_RCRB_IOBPS) & B_PCH_RCRB_IOBPS;
  if (ResponseStatus == V_PCH_RCRB_IOBPS_SUCCESS) {
    ///
    /// Step 8
    /// Read RCBA + 2334h[31:0] for IOBP data
    ///
    *Data = MmioRead32 (RootComplexBar + R_PCH_RCRB_IOBPD);
    return EFI_SUCCESS;
  } else {
    return EFI_DEVICE_ERROR;
  }
}

/**
  Configures PCH IOBP

  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] Address              Address of the IOBP register block
  @param[in] Opcode               Iobp Opcode
  @param[in] RouteId              Route Id
  @param[in, out] Data32          Read/Write data
  @param[out] Response            Response
  
  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_DEVICE_ERROR        Transaction fail
**/
EFI_STATUS
EFIAPI
PchIobpExecution (
  IN UINT32                              RootComplexBar,
  IN UINT32                              Address,
  IN PCH_IOBP_OPCODE                     Opcode,
  IN UINT8                               RouteId,
  IN OUT UINT32                          *Data32,
  OUT UINT8                              *Response
  )
{
  ///
  /// Step 1 Poll RCBA + 2338[0] = 0b
  ///
  while (MmioRead8 (RootComplexBar + R_PCH_RCRB_IOBPS) & B_PCH_RCRB_IOBPS_BUSY);
  ///
  /// Step 2
  /// Write RCBA + Offset 2330h[31:0] with IOBP register address
  ///
  MmioWrite32 ((RootComplexBar + R_PCH_RCRB_IOBPIRI), Address);
  ///
  /// Step 3
  /// Set RCBA + 2338h[15:8] to the opcode passed in
  ///
  MmioAndThenOr16 (
    (RootComplexBar + R_PCH_RCRB_IOBPS),
    (UINT16) (~B_PCH_RCRB_IOBPS_IOBPIA),
    (UINT16) (Opcode << 8)
    );
  ///
  /// Step 4
  /// Set RCBA + 233Ah[15:8] = F0h
  /// Set RCBA + 233Ah[7:0] = Route Id passed in
  ///
  MmioWrite16 (
    (RootComplexBar + 0x233A),
    (0xF000 | RouteId)
    );
  switch (Opcode) {
  case MemoryMapWrite:
  case IoMapWrite:
  case PciConfigWrite:
  case PrivateControlWrite:
    ///
    /// Step 5
    /// Write RCBA + 2334h[31:0] with updated SBI data
    ///
    MmioWrite32 ((RootComplexBar + R_PCH_RCRB_IOBPD), *Data32);
    break;
  default:
    break;
  }
  ///
  /// Step 6
  /// Set RCBA + 2338h[0] = 1b
  ///
  MmioOr16 (
    (RootComplexBar + R_PCH_RCRB_IOBPS),
    (UINT16) BIT0
    );

  ///
  /// Step 7
  /// Poll RCBA + Offset 2338h[0] = 0b, Polling for Busy bit
  ///
  while (MmioRead8 (RootComplexBar + R_PCH_RCRB_IOBPS) & B_PCH_RCRB_IOBPS_BUSY);

  ///
  /// Step 8
  /// Check if RCBA + 2338h[2:1] = 00b for successful transaction
  ///
  *Response = MmioRead8 (RootComplexBar + R_PCH_RCRB_IOBPS) & B_PCH_RCRB_IOBPS;
  if (*Response == V_PCH_RCRB_IOBPS_SUCCESS) {
    switch (Opcode) {
    case MemoryMapRead:
    case IoMapRead:
    case PciConfigRead:
    case PrivateControlRead:
      ///
      /// Step 9
      /// Read RCBA + 2334h[31:0] for IOBP data
      ///
      *Data32 = MmioRead32 (RootComplexBar + R_PCH_RCRB_IOBPD);
      break;
    default:
      break;
    }

    return EFI_SUCCESS;
  }
  return EFI_DEVICE_ERROR;
}
