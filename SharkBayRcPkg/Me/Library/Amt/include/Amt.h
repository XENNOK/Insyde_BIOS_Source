/** @file

  Header file for common Active Management Technology defines.

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

#ifndef _AMT_H_
#define _AMT_H_

///
/// Intel Internet Assigned Numbers Authority Manufacturer ID
/// (The firmware sends 0x57010000 for decimal value 343)
///
#define INTEL_IANA_SWAP32(x)      ((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | \
                                    (((x) & 0xff0000) >> 8) | (((x) & 0xff000000) >> 24))
#define ASF_INTEL_IANA            0x00000157
#define ASF_INTEL_CONVERTED_IANA  INTEL_IANA_SWAP32 (ASF_INTEL_IANA)  ///< 0X57010000, received from ME
///
/// Intel OEM Special Command
///
#define ASF_INTEL_OEM_CMD 0xC1

///
/// Intel OEM Parameters 16 bit OEM Parameter values
///
#define USE_SOL 0x0001  ///< 0000 0000 0000 0001 - bit 0, use SOL on the next boot
//
// Intel OEM Command 16 bit special command parameter values
//
#define USE_IDER                0x0001  ///< 0000 0000 0000 0001 - bit 0 Paramater 2 will be used to indicate the channel
#define ENFORCE_SECURE_BOOT     0x0002  ///< 0000 0000 0000 0010 - bit 1 Enforce secure boot over IDER
#define REFLASH_BIOS            0x0004  ///< 0000 0000 0000 0100 - bit 2
#define BIOS_SETUP              0x0008  ///< 0000 0000 0000 1000 - bit 3
#define BIOS_PAUSE              0x0010  ///< 0000 0000 0001 0000 - bit 4
#define USE_KVM                 0x0020  ///< 0000 0000 0010 0000 - bit 5
#define IDER_CD                 0x0100  ///< 0000 0001 0000 0000 - bit 8 Primary Slave Drive
#define IDER_PRIMARY_MASTER     0x0000
#define IDER_PRIMARY_SLAVE      0x0100
#define IDER_SECONDARY_MASTER   0x0200
#define IDER_SECONDARY_SLAVE    0x0300

#define IDER_BOOT_DEVICE_MASK   0x0300
#define IDER_BOOT_DEVICE_SHIFT  8

#endif
