/** @file
 The defination, structure, and enmuration consumed in Chipset Services.

***************************************************************************
* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#ifndef _CHIPSET_SVC_H_
#define _CHIPSET_SVC_H_

//*******************************************************
// IO_DECODE_TYPE
//*******************************************************
typedef enum {
  IoDecodeFdd,
  IoDecodeLpt,
  IoDecodeComA,
  IoDecodeComB,
  IoDecodeCnf2,
  IoDecodeCnf1,
  IoDecodeMc,
  IoDecodeKbc,
  IoDecodeGameh,
  IoDecodeGamel,
  IoDecodeGenericIo,
  IoDecodeTypeMaximum
} IO_DECODE_TYPE;

//*******************************************************
// IO_DECODE_ADDRESS
//*******************************************************
typedef enum {
  AddressFdd3F0,
  AddressFdd370,
  AddressLpt378,
  AddressLpt278,
  AddressLpt3BC,
  AddressCom3F8,
  AddressCom2F8,
  AddressCom220,
  AddressCom228,
  AddressCom238,
  AddressCom2E8,
  AddressCom338,
  AddressCom3E8,
  AddressCnf24E4F,
  AddressCnf12E2F,
  AddressMc6266,
  AddressKbc6064,
  AddressGameh208,
  AddressGamel200,
  IoDecodeAddressMaximum
} IO_DECODE_ADDRESS;

//*******************************************************
// MMIO_DECODE_ADDRESS
//*******************************************************
typedef enum {
  Address_FFF8_FFFF_FFB8_FFBF_000E_000F,
  Address_FFF0_FFF7_FFB0_FFB7,
  Address_FFE8_FFEF_FFA8_FFAF,
  Address_FFE0_FFE7_FFA0_FFA7,
  Address_FFD8_FFDF_FF98_FF9F,
  Address_FFD0_FFD7_FF90_FF97,
  Address_FFC8_FFCF_FF88_FF8F,
  Address_FFC0_FFC7_FF80_FF87,
  Address_FF70_FF7F_FF30_FF3F,
  Address_FF60_FF6F_FF20_FF2F,
  Address_FF50_FF5F_FF10_FF1F,
  Address_FF40_FF4F_FF00_FF0F,
  MmioDecodeAddressMaximum
} MMIO_DECODE_ADDRESS;

//*******************************************************
// Legacy Region Access Mode
//*******************************************************
#define LEGACY_REGION_ACCESS_UNLOCK            0
#define LEGACY_REGION_ACCESS_LOCK              1

#endif
