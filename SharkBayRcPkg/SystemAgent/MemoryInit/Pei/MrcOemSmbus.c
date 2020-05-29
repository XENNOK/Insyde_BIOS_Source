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


#include <MrcOemSmbus.h>

#ifdef NOSMBUS_BUILD
///
/// DDR3 1600 2GB single rank
///
const U8 SPDData[] =
  {0x92, 0x11, 0x0B, 0x02, 0x03, 0x19, 0x02, 0x01, 0x03, 0x11, 0x01, 0x08, 0x0A, 0x00, 0xFE, 0x00, /// 00-15
   0x69, 0x78, 0x69, 0x30, 0x69, 0x11, 0x18, 0x81, 0x00, 0x05, 0x3C, 0x3C, 0x00, 0xF0, 0x83, 0x01, /// 16-31
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /// 32-47
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x11, 0x61, 0x00, /// 48-63
   0x00, 0x00, 0x00, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00, 0x80                                      /// 64-73
};
#endif

/**
@brief
  Perform a byte read from the specified SMBus device address.

  @param[in] SmbusBaseAddress   - The SMBus base address.
  @param[in] SmBusDeviceAddress - SMBus device address to read from.
  @param[in] Offset             - Byte offset to read from.
  @param[out] Value             - Location to store read value.

  @retval: mrcSuccess if the read is successful, otherwise mrcFail and the read data is set to zero.
**/
MrcStatus
MrcOemSmbusRead8 (
  IN const U32  SmbusBaseAddress,
  IN const U8   SmBusDeviceAddress,
  IN const U8   Offset,
  OUT U8 *const Value
  )
{
  MrcStatus     Status;
  RETURN_STATUS EfiStatus;

#ifdef NOSMBUS_BUILD
  if (SmBusDeviceAddress == 0xA0) {
    //
    // For CH0 - DIMM0 only
    //
    *Value  = SPDData[Offset];
    Status  = mrcSuccess;
  } else {
    Status = mrcFail;
  }

#else
  *Value = SmBusReadDataByte (SmBusDeviceAddress | ((U32) Offset << 8), &EfiStatus);
  if (EfiStatus == RETURN_SUCCESS) {
    Status = mrcSuccess;
  } else {
    Status = mrcFail;
  }
#endif // NOSMBUS_BUILD

  if (Status != mrcSuccess) {
    *Value = 0;
  }

  return Status;
}

/**
@brief
  Perform a byte write to the specified SMBus device address.

  @param[in] SmbusBaseAddress   - The SMBus base address.
  @param[in] SmBusDeviceAddress - SMBus device address to write to.
  @param[in] Offset             - Byte offset to write to.
  @param[in] Value              - The value to write.

  @retval: mrcSuccess if the write is successful, otherwise mrcFail.
**/
MrcStatus
MrcOemSmbusWrite8 (
  IN const U32  SmbusBaseAddress,
  IN const U8   SmBusDeviceAddress,
  IN const U8   Offset,
  IN U8         Value
  )
{

  MrcStatus     Status;
  RETURN_STATUS EfiStatus;

  SmBusWriteDataByte (SmBusDeviceAddress | ((U32) Offset << 8), Value, &EfiStatus);
  Status = (EfiStatus == RETURN_SUCCESS) ? mrcSuccess : mrcFail;
  return Status;
}

/**
@brief
  Perform a word read from the specified SMBus device address.

  @param[in] SmbusBaseAddress   - The SMBus base address.
  @param[in] SmBusDeviceAddress - SMBus device address to read from.
  @param[in] Offset             - Offset to read from.
  @param[out] Value             - Location to store read value.

  @retval:  mrcSuccess if the read is successful, otherwise mrcFail and the read data is set to zero.
**/
MrcStatus
MrcOemSmbusRead16 (
  IN const U32   SmbusBaseAddress,
  IN const U8    SmBusDeviceAddress,
  IN const U8    Offset,
  OUT U16 *const Value
  )
{
  MrcStatus     Status;
  RETURN_STATUS EfiStatus;

#ifdef NOSMBUS_BUILD
  if (SmBusDeviceAddress == 0xA0) {
    //
    // For CH0 - DIMM0 only
    //
    *Value  = SPDData[Offset];
    Status  = mrcSuccess;
  } else {
    Status = mrcFail;
  }

#else
  *Value = SmBusReadDataWord (SmBusDeviceAddress | ((U32) Offset << 8), &EfiStatus);
  if (EfiStatus == RETURN_SUCCESS) {
    Status = mrcSuccess;
  } else {
    Status = mrcFail;
  }
#endif // NOSMBUS_BUILD

  if (Status != mrcSuccess) {
    *Value = 0;
  }

  return Status;
}

/**
@brief
  Perform a word write to the specified SMBus device address.

  @param[in] SmbusBaseAddress   - The SMBus base address.
  @param[in] SmBusDeviceAddress - SMBus device address to write to.
  @param[in] Offset             - Offset to write to.
  @param[in] Value              - The value to write.

  @retval:  mrcSuccess if the write is successful, otherwise mrcFail.
**/
MrcStatus
MrcOemSmbusWrite16 (
  IN const U32  SmbusBaseAddress,
  IN const U8   SmBusDeviceAddress,
  IN const U8   Offset,
  IN U16        Value
  )
{
  MrcStatus     Status;
  RETURN_STATUS EfiStatus;

  SmBusWriteDataWord (SmBusDeviceAddress | ((U32) Offset << 8), Value, &EfiStatus);
  Status = (EfiStatus == RETURN_SUCCESS) ? mrcSuccess : mrcFail;
  return Status;
}
