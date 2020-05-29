/** @file

  Define the variable data structures used for TrEE physical presence.
  The TPM2 request from firmware or OS is saved to variable. And it is
  cleared after it is processed in the next boot cycle. The TPM2 response
  is saved to variable.

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

#ifndef __TREE_PHYSICAL_PRESENCE_DATA_GUID_H__
#define __TREE_PHYSICAL_PRESENCE_DATA_GUID_H__

#define EFI_TREE_PHYSICAL_PRESENCE_DATA_GUID \
  { \
    0xF24643C2, 0xC622, 0x494E, { 0x8A, 0x0D, 0x46, 0x32, 0x57, 0x9C, 0x2D, 0x5B }}

#define TREE_PHYSICAL_PRESENCE_VARIABLE  L"TrEEPhysicalPresence"

typedef struct {
  UINT8   PPRequest;      ///< Physical Presence request command.
  UINT8   LastPPRequest;
  UINT32  PPResponse;
  UINT8   Flags;
} EFI_TREE_PHYSICAL_PRESENCE;

///
/// The definition bit of the flags
///
#define TREE_FLAG_NO_PPI_CLEAR                        0x2
#define TREE_FLAG_RESET_TRACK                         0x8

///
/// The definition of physical presence operation actions
///
#define TREE_PHYSICAL_PRESENCE_NO_ACTION                               0
#define TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR                     5
#define TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_2                   14
#define TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_FALSE                  17
#define TREE_PHYSICAL_PRESENCE_SET_NO_PPI_CLEAR_TRUE                   18
#define TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_3                   21
#define TREE_PHYSICAL_PRESENCE_CLEAR_CONTROL_CLEAR_4                   22

#define TREE_PHYSICAL_PRESENCE_NO_ACTION_MAX                           20

extern EFI_GUID  gEfiTrEEPhysicalPresenceGuid;

#endif

