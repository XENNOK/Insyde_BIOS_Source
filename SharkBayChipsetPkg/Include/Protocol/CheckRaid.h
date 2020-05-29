//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _CHECK_RAID_H_
#define _CHECK_RAID_H_

#include <Protocol/DiskInfo.h>
#include <Protocol/AtaPassThru.h>
#include <Library/DevicePathLib.h>

//EFI_FORWARD_DECLARATION (CHECK_RAID_PROTOCOL);

// {932CF515-3189-400d-9AD9-40CE6EB731BC}
#define CHECK_RAID_PROTOCOL_GUID \
 { 0x932cf515, 0x3189, 0x400d, 0x9a, 0xd9, 0x40, 0xce, 0x6e, 0xb7, 0x31, 0xbc}

extern EFI_GUID gEfiCheckRaidProtocolGuid;

typedef struct _CHECK_RAID_PROTOCOL {
  EFI_HANDLE                  Controller;
  EFI_DEVICE_PATH_PROTOCOL    *AtaPassThruDevPath;
  EFI_DISK_INFO_PROTOCOL      DiskInfo;
  UINT16                      Port;
  UINT16                      PortMultiplierPort;
  EFI_ATA_PASS_THRU_PROTOCOL  *AtaPassThruPtr;
} CHECK_RAID_PROTOCOL;

#endif // _CHECK_RAID_H_
