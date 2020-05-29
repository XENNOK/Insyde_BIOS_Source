/** @file

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

#ifndef _ICC_PLATFORM_POLICY_H_
#define _ICC_PLATFORM_POLICY_H_

//
// ICC Policy provided by platform for DXE phase { 35DD551A-7AB4-0361-852C-3E3AFE52041A }
//
#define DXE_PLATFORM_ICC_POLICY_GUID \
  { \
    0x35DD551A, 0x7AB4, 0x0361, 0x85, 0x2C, 0x3E, 0x3A, 0xFE, 0x52, 0x04, 0x1A \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID        gDxePlatformICCPolicyGuid;

//
// Constant
//
#define ICC_ENABLE         1
#define ICC_DISABLE        0

typedef struct _ICC_FUNCTION_ENABLING {
  UINT32        FCIM     : 1;
  UINT32        Reserved : 31;
} ICC_FUNCTION_ENABLING;

typedef union _ICC_CLOCK_ENABLING {
  UINT32        Dword;
  struct {
    UINT32        Flex0      : 1;
    UINT32        Flex1      : 1;
    UINT32        Flex2      : 1;
    UINT32        Flex3      : 1;
    UINT32        Reserved1  : 3;
    UINT32        PCI_Clock0 : 1;
    UINT32        PCI_Clock1 : 1;
    UINT32        PCI_Clock2 : 1;
    UINT32        PCI_Clock3 : 1;
    UINT32        PCI_Clock4 : 1;
    UINT32        Reserved2  : 4;
    UINT32        SRC0       : 1;
    UINT32        SRC1       : 1;
    UINT32        SRC2       : 1;
    UINT32        SRC3       : 1;
    UINT32        SRC4       : 1;
    UINT32        SRC5       : 1;
    UINT32        SRC6       : 1;
    UINT32        SRC7       : 1;
    UINT32        CSI_SRC8   : 1;
    UINT32        CSI_DP     : 1;
    UINT32        PEG_A      : 1;
    UINT32        PEG_B      : 1;
    UINT32        DMI        : 1;
    UINT32        Reserved3  : 3;
  } Fields;
} ICC_CLOCK_ENABLING;

typedef union _ICC_CLOCK_ENABLES_MASK {
  UINT32        Dword;
  struct {
    UINT32        Flex0      : 1;
    UINT32        Flex1      : 1;
    UINT32        Flex2      : 1;
    UINT32        Flex3      : 1;
    UINT32        Reserved1  : 3;
    UINT32        PCI_Clock0 : 1;
    UINT32        PCI_Clock1 : 1;
    UINT32        PCI_Clock2 : 1;
    UINT32        PCI_Clock3 : 1;
    UINT32        PCI_Clock4 : 1;
    UINT32        Reserved2  : 4;
    UINT32        SRC0       : 1;
    UINT32        SRC1       : 1;
    UINT32        SRC2       : 1;
    UINT32        SRC3       : 1;
    UINT32        SRC4       : 1;
    UINT32        SRC5       : 1;
    UINT32        SRC6       : 1;
    UINT32        SRC7       : 1;
    UINT32        CSI_SRC8   : 1;
    UINT32        CSI_DP     : 1;
    UINT32        PEG_A      : 1;
    UINT32        PEG_B      : 1;
    UINT32        DMI        : 1;
    UINT32        Reserved3  : 3;
  } Fields;
} ICC_CLOCK_ENABLES_MASK;

typedef
EFI_STATUS
( EFIAPI *ICC_SET_BACKUP_PROFILE ) (
     OUT  UINT8        *ICCBackupProfile
  );

//
// ICC DXE Platform Policiy
//
typedef struct _DXE_PLATFORM_ICC_POLICY_PROTOCOL {
  ICC_FUNCTION_ENABLING         *FunctionEnabling;
  ICC_CLOCK_ENABLING            *ClockEnabling;
  ICC_CLOCK_ENABLES_MASK        *ClockEnablesMask;
  ICC_SET_BACKUP_PROFILE        SetBackupProfile;
//[-start-120315-IB06460374-add]//
  UINT8                         IccDefaultProfile;
//[-end-120315-IB06460374-add]//
} DXE_PLATFORM_ICC_POLICY_PROTOCOL;

#endif
