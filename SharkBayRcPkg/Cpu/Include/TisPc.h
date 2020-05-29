/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  TisPc.h

@brief
  Definitions and function prototypes shared by all TPM components

**/
#ifndef _TIS_PC_H_
#define _TIS_PC_H_

///
/// Set structure alignment to 1-byte
///
#pragma pack(push)
#pragma pack(1)
///
/// TPM Base Address Definitions
///
#define TPM_BASE_ADDRESS  0xFED40000

#define VOLATILE  volatile

///
/// Register set map as specified in TIS Chapter 10
///
typedef struct tdTIS_PC_REGISTERS {
  INT8    access;               ///< 0
  UINT8   reserved1[7];         ///< 1
  UINT32  intEnable;            ///< 8
  UINT8   intVector;            ///< 0ch
  UINT8   reserved2[3];         ///< 0dh
  UINT32  intSts;               ///< 10h
  UINT32  intfCapability;       ///< 14h
  INT8    status;               ///< 18h
  UINT16  burstCount;           ///< 19h
  UINT8   reserved3[9];
  UINT32  dataFifo;             ///< 24
  UINT8   reserved4[0xed8];     ///< 28h
  UINT16  vid;                  ///< 0f00h
  UINT16  did;                  ///< 0f02h
  UINT8   rid;                  ///< 0f04h
  UINT8   tcgDefined[0x7b];     ///< 0f05h
  UINT32  legacyAddress1;       ///< 0f80h
  UINT32  legacyAddress1Ex;     ///< 0f84h
  UINT32  legacyAddress2;       ///< 0f88h
  UINT32  legacyAddress2Ex;     ///< 0f8ch
  UINT8   vendorDefined[0x70];  ///< 0f90h
} TIS_PC_REGISTERS;

///
/// Define pointer types used to access TIS registers on PC
///
typedef VOLATILE TIS_PC_REGISTERS *TIS_PC_REGISTERS_PTR;

#pragma pack(pop)
#endif
