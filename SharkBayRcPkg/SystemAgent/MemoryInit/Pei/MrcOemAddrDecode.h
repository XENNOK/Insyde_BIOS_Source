/**
This file contains an 'Intel Peripheral Driver' and uniquely
identified as "Intel Reference Module" and is
licensed for Intel CPUs and chipsets under the terms of your
license agreement with Intel or your vendor.  This file may
be modified by the user, subject to additional terms of the
license agreement.

Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved.
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:
  MrcOemAddrDecode.h

@brief:
  Wrapper file for AddrDecode files.

**/
#ifndef __MrcOemAddrDecode_h__
#define __MrcOemAddrDecode_h__

#include "MrcTypes.h"

//
// Defines used to get code in AddrDecode folder to compile
//
#define inline

typedef struct
{
  U8  ChannelNumber;
  U8  DIMMNumber;
  U8  Rank;
  U8  Bank;
  U16 CAS;
  U16 RAS;
} ADDRESS_DECODE;

/**
@brief
  Address decode function
  Converts system address to DRAM address

  @param[in]      SystemAddress  - The 39-bit system address to convert.
  @param[out]     DramAddress    - The dram address struct that the system address decodes to.

  @retval Returns 1 if successful, 0 otherwise.

**/
unsigned char
MrcMcAddressDecode (
  IN     unsigned long long SystemAddress,
  OUT    ADDRESS_DECODE     *DramAddress
  );

/**
@brief
  Address encode function (reverse address decode)
  DRAM address to system address conversion

  @param[in]      DramAddress    - The dram address that is converted.
  @param[out]     SystemAddress  - The 39-bit system address to convert to.

  @retval Returns 1 if successful, 0 otherwise.

**/
unsigned char
MrcMcAddressEncode (
  IN     ADDRESS_DECODE     *DramAddress,
  OUT    unsigned long long *SystemAddress
  );

#endif // __MrcOemAddrDecode_h__
