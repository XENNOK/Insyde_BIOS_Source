/** @file
  This file defines the ACPI Info Protocol.
  
;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 2012 - 2014 Intel Corporation. All rights reserved
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
#ifndef _ACPI_INFO_H_
#define _ACPI_INFO_H_

#define EFI_ACPI_INFO_PROTOCOL_GUID \
  { 0xfcdc1aa3, 0x1f1e, 0x4d47, { 0xbf, 0x61, 0x3c, 0x3b, 0xd2, 0x65, 0x5d, 0x1e } }


///
/// Extern the GUID for protocol users.
///
extern EFI_GUID                       gEfiAcpiInfoProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _EFI_ACPI_INFO_PROTOCOL EFI_ACPI_INFO_PROTOCOL;

///
/// Protocol revision number
/// Any backwards compatible changes to this protocol will result in an update in the revision number
/// Major changes will require publication of a new protocol
///
/// Revision 1:  Original version
///
#define ACPI_INFO_PROTOCOL_REVISION_1  1
#define ACPI_RC_VERSION          0x01080000

///
/// Protocol definition
///
struct _EFI_ACPI_INFO_PROTOCOL {
  UINT8   Revision;
  UINT32  RCVersion;
};

#endif
