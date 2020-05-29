//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
#ifndef _OEMDATAHUB_SUBCLASS_CACHE_H_
#define _OEMDATAHUB_SUBCLASS_CACHE_H_
#include EFI_GUID_DEFINITION (DataHubRecords)

typedef enum {  
  //
  // SMBIOS 2.7.1+
  //
  EfiCacheAssociativity20Way        = 0x0E
} EFI_CACHE_ASSOCIATIVITY_DATA_PLUS;

#endif
