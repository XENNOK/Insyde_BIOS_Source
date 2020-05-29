/** @file
  This file defines the RapidStart Info Protocol.

@copyright
  Copyright (c) 2011 - 2014 Intel Corporation. All rights reserved
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
#ifndef _RAPID_START_INFO_H_
#define _RAPID_START_INFO_H_

///
/// GUID to locate Rapid Start Info Protocol
///
/// EDK and EDKII have different GUID formats
///
/// @see _RAPID_START_INFO_PROTOCOL
///

#define RAPID_START_INFO_PROTOCOL_GUID \
  { \
    0xd231db5b, 0x4a9c, 0x4092, \
    { \
      0xa8, 0xc5, 0x9c, 0xa0, 0xbc, 0x7d, 0x6a, 0xa6 \
    } \
  }

///
/// Extern the GUID for protocol users.
///
/// @see _RAPID_START_INFO_PROTOCOL
///
extern EFI_GUID                           gRapidStartInfoProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _RAPID_START_INFO_PROTOCOL RAPID_START_INFO_PROTOCOL;

///
/// Protocol revision number
/// Any backwards compatible changes to this protocol will result in an update in the revision number
/// Major changes will require publication of a new protocol
///
/// Revision 1:  Original version
///
#define RAPID_START_INFO_PROTOCOL_REVISION_1  1

///
/// RapidStart Reference Code formats 0xAABBCCDD
///   DD - Build Number
///   CC - Reference Code Revision
///   BB - Reference Code Minor Version
///   AA - Reference Code Major Version
/// Example: RapidStart Reference Code 0.7.1 should be 00 07 01 00 (0x00070100)
///
#define RAPID_START_RC_VERSION          0x01080000
#define RAPID_START_FVI_STRING          "Reference Code - Intel Rapid Start"
#define RAPID_START_FVI_SMBIOS_TYPE     0xDD
#define RAPID_START_FVI_SMBIOS_INSTANCE 0x05
/**
  A protocol to indicate Rapid Start reference code revision.

  This protocol will be initialized by Rapid Start reference code to indicate the reference code revision.
  It will be installed by reference code even Rapid Start is disabled.

  @see RAPID_START_INFO_PROTOCOL_GUID
  @see gRapidStartInfoProtocolGuid
  @see RapidStartInstallInfoProtocol()
**/
struct _RAPID_START_INFO_PROTOCOL {
  UINT8   Revision;  ///< Revision of this protocol
  UINT32  RCVersion; ///< Rapid Start reference code revision
};

#endif
