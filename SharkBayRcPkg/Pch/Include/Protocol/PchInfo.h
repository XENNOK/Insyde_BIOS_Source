/** @file
  This file defines the PCH Info Protocol.

@copyright
  Copyright (c) 2008 - 2014 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
#ifndef _PCH_INFO_H_
#define _PCH_INFO_H_

///
/// Define PCH INFO protocol GUID
///
/// EDK and EDKII have different GUID formats
///
#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
#include "UsbHcPortPrecondition.h"
#define EFI_PCH_INFO_PROTOCOL_GUID \
  { \
    0x984eb4e9, 0x5a95, 0x41de, 0xaa, 0xd0, 0x53, 0x66, 0x8c, 0xa5, 0x13, 0xc0 \
  }

#else
#define EFI_PCH_INFO_PROTOCOL_GUID \
  { \
    0x984eb4e9, 0x5a95, 0x41de, \
    { \
      0xaa, 0xd0, 0x53, 0x66, 0x8c, 0xa5, 0x13, 0xc0 \
    } \
  }
#endif
//
// Extern the GUID for protocol users.
//
extern EFI_GUID                       gEfiPchInfoProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_PCH_INFO_PROTOCOL EFI_PCH_INFO_PROTOCOL;

///
/// Protocol revision number
/// Any backwards compatible changes to this protocol will result in an update in the revision number
/// Major changes will require publication of a new protocol
///
/// Revision 1:  Original version
///
#define PCH_INFO_PROTOCOL_REVISION_1  1
///
/// Revision 2:  Add Preconditioned for Usb precondition support.
///
#define PCH_INFO_PROTOCOL_REVISION_2  2

///
/// RCVersion[7:0] is the release number.
/// For example:
/// LptFramework 0.6.0-01 should be 00 06 00 01 (0x00060001)
/// LptFramework 0.6.2    should be 00 06 02 00 (0x00060200)
/// LptFramework 0.7.0    should be 00 07 00 00 (0x00070000)
/// LptFramework 0.7.1    should be 00 07 01 00 (0x00070100)
/// LptFramework 0.8.0    should be 00 08 00 00 (0x00080000)
/// LptFramework 0.8.1    should be 00 08 01 00 (0x00080100)
/// LptFramework 0.9.0    should be 00 09 00 00 (0x00090000)
/// LptFramework 1.0.0    should be 01 00 00 00 (0x01000000)
/// LptFramework 1.1.0    should be 01 01 00 00 (0x01010000)
/// LptFramework 1.2.0    should be 01 02 00 00 (0x01020000)
/// LptFramework 1.3.0    should be 01 03 00 00 (0x01030000)
/// LptFramework 1.3.1    should be 01 03 01 00 (0x01030100)
/// LptFramework 1.4.0    should be 01 04 00 00 (0x01040000)
/// LptFramework 1.5.0    should be 01 05 00 00 (0x01050000)
/// LptFramework 1.6.0    should be 01 06 00 00 (0x01060000)
/// LptFramework 1.6.1    should be 01 06 01 00 (0x01060100)
/// LptFramework 1.6.2    should be 01 06 02 00 (0x01060200)
/// LptFramework 1.7.0    should be 01 07 00 00 (0x01070000)
/// LptFramework 1.8.0    should be 01 08 00 00 (0x01080000)
///
#define PCH_RC_VERSION                0x01080000

///
/// Protocol definition
///
/// This protocol is used to provide the information of PCH controller.
///
struct _EFI_PCH_INFO_PROTOCOL {
  ///
  /// This member specifies the revision of the PCH Info protocol. This field is used
  /// to indicate backwards compatible changes to the protocol. Platform code that
  /// consumes this protocol must read the correct revision value to correctly interpret
  /// the content of the protocol fields.
  ///
  UINT8   Revision;
  ///
  /// The actual bus number of the PCH devices.
  ///
  UINT8   BusNumber;
  ///
  /// The reference code package release number
  ///
  UINT32  RCVersion;
  ///
  /// Indicate the Usb precondition feature is working, and it links all the Usb HC
  /// precondition structures in the list.
  ///
  EFI_USB_HC_PORT_PRECONDITION  *Preconditioned;
};

#endif
