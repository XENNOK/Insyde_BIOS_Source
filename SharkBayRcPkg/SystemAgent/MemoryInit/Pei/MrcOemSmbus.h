/** @file
  This file contains SMBus related functions.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved.
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement.
**/
#ifndef _MrcOemSmbus_h_
#define _MrcOemSmbus_h_


#pragma pack (push, 1)
#include <MrcTypes.h>
#include <MrcApi.h>
#include <MrcOemDebugPrint.h>
#include <Library/PeiSmbusLibSmbus2Ppi/InternalSmbusLib.h>
/**
@brief
  Perform a byte read from the specified SMBus device address.

  @param[in] SmbusBaseAddress   - The SMBus base address.
  @param[in] SmBusDeviceAddress - SMBus device address to read from.
  @param[in] Offset             - Byte offset to read from.
  @param[out] Value             - Location to store read value.

  @retval: mrcSuccess if the read is successful, otherwise mrcFail and the read data is set to zero.
**/
extern
MrcStatus
MrcOemSmbusRead8 (
  IN const U32  SmbusBaseAddress,
  IN const U8   SmBusDeviceAddress,
  IN const U8   Offset,
  OUT U8 *const Value
  );

/**
@brief
  Perform a byte write to the specified SMBus device address.

  @param[in] SmbusBaseAddress   - The SMBus base address.
  @param[in] SmBusDeviceAddress - SMBus device address to write to.
  @param[in] Offset             - Byte offset to write to.
  @param[in] Value              - The value to write.

  @retval: mrcSuccess if the write is successful, otherwise mrcFail.
**/
extern
MrcStatus
MrcOemSmbusWrite8 (
  IN const U32  SmbusBaseAddress,
  IN const U8   SmBusDeviceAddress,
  IN const U8   Offset,
  IN U8         Value
  );

/**
@brief
  Perform a word read from the specified SMBus device address.

  @param[in] SmbusBaseAddress   - The SMBus base address.
  @param[in] SmBusDeviceAddress - SMBus device address to read from.
  @param[in] Offset             - Offset to read from.
  @param[out] Value             - Location to store read value.

  @retval:  mrcSuccess if the read is successful, otherwise mrcFail and the read data is set to zero.
**/
extern
MrcStatus
MrcOemSmbusRead16 (
  IN const U32   SmbusBaseAddress,
  IN const U8    SmBusDeviceAddress,
  IN const U8    Offset,
  OUT U16 *const Value
  );

/**
@brief
  Perform a word write to the specified SMBus device address.

  @param[in] SmbusBaseAddress   - The SMBus base address.
  @param[in] SmBusDeviceAddress - SMBus device address to write to.
  @param[in] Offset             - Offset to write to.
  @param[in] Value              - The value to write.

  @retval:  mrcSuccess if the write is successful, otherwise mrcFail.
**/
extern
MrcStatus
MrcOemSmbusWrite16 (
  IN const U32  SmbusBaseAddress,
  IN const U8   SmBusDeviceAddress,
  IN const U8   Offset,
  IN U16        Value
  );

#pragma pack(pop)
#endif // _MrcOemSmbus_h_
