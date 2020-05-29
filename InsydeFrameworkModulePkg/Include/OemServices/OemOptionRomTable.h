//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
#ifndef _OEM_OPTION_ROM_TABLE_H_
#define _OEM_OPTION_ROM_TABLE_H_

typedef struct {
  EFI_GUID  FileName;
  UINT16    VendorId;
  UINT16    DeviceId;
} PCI_OPTION_ROM_TABLE;

typedef struct {
  EFI_GUID   FileName;
  BOOLEAN    Valid;
  BOOLEAN    OptionRomSpace;
  UINTN      OpRomType;
} SERVICE_ROM_TABLE;
 
typedef struct {
  EFI_GUID   FileName;
  UINTN      Valid;
  UINTN      OpRomType;
} SYSTEM_ROM_TABLE;

#endif