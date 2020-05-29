/** @file

  Provide the interface for Variable Editor to add/remove Hii database

;******************************************************************************
;* Copyright (c) 1983 - 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _VARIABLE_EDIT_PROTOCOL_H_
#define _VARIABLE_EDIT_PROTOCOL_H_

#define VARIABLE_EDIT_PROTOCOL_GUID \
  { \
    0xd289eacc, 0x06b7, 0x4cc0, { 0xa8, 0x44, 0x98, 0xfd, 0xb0, 0x89, 0x58, 0xdd } \
  }

typedef struct _VARIABLE_EDIT_PROTOCOL VARIABLE_EDIT_PROTOCOL;

//
// Member functions
//
typedef
EFI_STATUS
(EFIAPI *INSTALL_SETUP_HII) (
  IN  EFI_SETUP_UTILITY_PROTOCOL            *This,
  IN  BOOLEAN                               InstallHii
  );

#pragma pack (1)
typedef struct _VARIABLE_EDIT_PROTOCOL {
  INSTALL_SETUP_HII                   InstallSetupHii;
};
#pragma pack()

extern EFI_GUID gVariableEditProtocolGuid;

#endif
