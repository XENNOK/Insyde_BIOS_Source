/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  PchSmbusLibrary.h

@brief
  Header file for Pch Smbus Lib.

**/
#ifndef _PCH_SMBUS_LIBRARY_H_
#define _PCH_SMBUS_LIBRARY_H_

/**
  This function provides a standard way to execute Smbus sequential
  I2C Read. This function allows the PCH to perform block reads to
  certain I2C devices, such as serial E2PROMs. Typically these data
  bytes correspond to an offset (address) within the serial memory
  chips.

  @param[in] SmBusAddress         Address that encodes the SMBUS Slave Address,
                                  SMBUS Command, SMBUS Data Length, and PEC.
  @param[out] Buffer              Pointer to the buffer to store the bytes read
                                  from the SMBUS
  @param[out] Status              eturn status for the executed command.

  @retval UINTN                   The number of bytes read
**/
UINTN
EFIAPI
SmBusSeqI2CRead (
  IN  UINTN          SmBusAddress,
  OUT VOID           *Buffer,
  OUT RETURN_STATUS  * Status OPTIONAL
  );
#endif
