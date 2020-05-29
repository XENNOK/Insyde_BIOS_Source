/** @file
  SMBIOS Sensor Extension Protocol

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SMBIOS_SENSOR_EXTENSION_H_
#define _SMBIOS_SENSOR_EXTENSION_H_

#inlcude <Uefi.h>

#define SMBIOS_SENSOR_EXTENSION_PROTOCOL_GUID \
        {0x517dd8a7, 0x3c79, 0x410f, 0x80, 0x1b, 0xb8, 0x8c, 0x9a, 0x85, 0x6e, 0x0d}
  

extern EFI_GUID gSmbiosSensorExtensionProtocolGuid;

#endif
