//;******************************************************************************
//;* Copyright (c) 1983-2013, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _OEM_SMBIOS_GET_INFO_H_
#define _OEM_SMBIOS_GET_INFO_H_


typedef struct {
  UINTN           Length;
  EFI_GUID        Uuid;
  CHAR16          *StringData;
} OEM_SMBIOS_INFORMATION;


#endif // _OEM_SMBIOS_GET_INFO_H_
