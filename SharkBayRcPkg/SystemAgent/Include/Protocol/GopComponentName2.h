/** @file
  Protocol to retrieve the GOP driver version

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
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
  license agreement
**/

#ifndef _GOP_COMPONENT_NAME2_H_
#define _GOP_COMPONENT_NAME2_H_
//
// Global ID for the Component Name Protocol
//
#define GOP_COMPONENT_NAME2_PROTOCOL_GUID \
  { \
    0x651b7ebd, 0xce13, 0x41d0, 0x82, 0xe5, 0xa0, 0x63, 0xab, 0xbe, 0x9b, 0xb6 \
  }

typedef struct _GOP_COMPONENT_NAME2_PROTOCOL  GOP_COMPONENT_NAME2_PROTOCOL;

typedef
EFI_STATUS
(EFIAPI *GOP_COMPONENT_NAME2_GET_DRIVER_NAME) (
  IN  GOP_COMPONENT_NAME2_PROTOCOL * This,
  IN  CHAR8                           *Language,
  OUT CHAR16                          **DriverName
  );

typedef
EFI_STATUS
(EFIAPI *GOP_COMPONENT_NAME2_GET_CONTROLLER_NAME) (
  IN  GOP_COMPONENT_NAME2_PROTOCOL          * This,
  IN  EFI_HANDLE                               ControllerHandle,
  IN  EFI_HANDLE                               ChildHandle OPTIONAL,
  IN  CHAR8                                    *Language,
  OUT CHAR16                                   **ControllerName
  );

typedef
EFI_STATUS
(EFIAPI *GOP_COMPONENT_NAME2_GET_DRIVER_VERSION) (
  IN  GOP_COMPONENT_NAME2_PROTOCOL          * This,
  IN  CHAR8                                    *Language,
  OUT CHAR16                                   **DriverVersion
  );

struct _GOP_COMPONENT_NAME2_PROTOCOL {
  GOP_COMPONENT_NAME2_GET_DRIVER_NAME      GetDriverName;
  GOP_COMPONENT_NAME2_GET_DRIVER_VERSION   GetDriverVersion;
  GOP_COMPONENT_NAME2_GET_CONTROLLER_NAME  GetControllerName;
  CHAR8                                       *SupportedLanguages;
};

extern EFI_GUID gGopComponentName2ProtocolGuid;

#endif
