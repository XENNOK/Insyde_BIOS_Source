//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _BOOT_SYSTEM_INFORMATION_H_
#define _BOOT_SYSTEM_INFORMATION_H_

#include "Tiano.h"
#include "OemDataHubSubClassMemory.h"
#include "GenericBdsLib.h"
#include "String.h"

#include EFI_PROTOCOL_CONSUMER (DataHub)
#include EFI_PROTOCOL_CONSUMER (DiskInfo)
#include EFI_PROTOCOL_CONSUMER (IdeControllerInit)

#include EFI_GUID_DEFINITION (DataHubRecords)

EFI_STATUS
ShowSystemInfo (
  IN OUT UINT8                        *PrintLine
  );

EFI_STATUS
GetBootStringFromToken (
  IN  EFI_HII_PROTOCOL                  *Hii,
  IN  EFI_GUID                          *ProducerGuid,
  IN  STRING_REF                        Token,
  OUT CHAR16                            **String
  );

#endif
