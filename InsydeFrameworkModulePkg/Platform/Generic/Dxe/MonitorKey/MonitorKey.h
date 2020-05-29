//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _MONITORKEY_H
#define _MONITORKEY_H

#include "Tiano.h"
#include "EfiDriverLib.h"
#include "OemPostKey.h"
#include "EfiPrePostHotkey.h"

#include EFI_PROTOCOL_PRODUCER (MonitorKeyFilter)
#include EFI_PROTOCOL_DEFINITION (UsbLegacyPlatform)
#include EFI_PROTOCOL_DEFINITION (LegacyBios)
#include EFI_PROTOCOL_DEFINITION (SimpleTextIn)
#include EFI_PROTOCOL_DEFINITION (SimpleTextInputEx)

#include EFI_GUID_DEFINITION (GenericVariable)

#define MONITOR_KEY_FILTER_PRIVATE_SIGNATURE   EFI_SIGNATURE_32('M','t','k','f')

#define DEFINE_DESTINATION_MEMORY_ARRESS       0x06000
#define DEFINE_BDA_MEMORY_GET_EBDA_ADDRESS     0x40E
#define DEFINE_EBDA_MONITOR_KEY_BUFFER_ADDRESS 0x3B
#define DEFINE_TRANSFER_SEGMENT                0x04
#define DEFINE_KEY_MOINTOR_ENABGLE_SIGNATURE   0x55
#define DEFINE_MARK_OFFSET                     0x0000000f

#define SHIFT_KEY_ENABLE_FLAG                  0x01
#define ALT_KEY_ENABLE_FLAG                    0x02
#define CTRL_KEY_ENABLE_FLAG                   0x04
#define DEFINE_KEY_IS_SET                      1
#define DEFINE_MAX_KEY_LIST                    32
#pragma pack (1)
typedef struct {
  UINT8                               EnableFlag;
  UINT16                              BufferOffset;
  UINT16                              BufferSegment;
  UINT32                              StatusFlag;
} MONITOR_KEY_FILTER_BUFFER_HEAD_STRUCT;
#pragma pack ()

typedef struct {
  UINTN                               Signature;
  EFI_USB_LEGACY_PLATFORM_PROTOCOL    *UsbLegacyPlatform;
  EFI_MONITOR_KEY_FILTER_PROTOCOL     MonitorKeyInstance;
  KEY_ELEMENT                         *HotKeyList;
  UINT32                              HotKeyCount;
  UINT32                              KeyDetected;
} MONITOR_KEY_FILTER_PRIVATE;

#define MONITOR_KEY_FILTER_FROM_THIS(a) CR(a, MONITOR_KEY_FILTER_PRIVATE, MonitorKeyInstance, MONITOR_KEY_FILTER_PRIVATE_SIGNATURE)


#endif