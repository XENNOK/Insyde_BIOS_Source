//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _OEM_SERVICES_PPI_H
#define _OEM_SERVICES_PPI_H

#define OEM_SERVICES_PPI_GUID \
  { \
  0xa8e61b3f, 0x7675, 0x4323, 0x87, 0xb5, 0x69, 0xa4, 0x9c, 0xd0, 0x3b, 0x55 \
  }

EFI_FORWARD_DECLARATION (OEM_SERVICES_PPI);

typedef
EFI_STATUS
(EFIAPI *OEM_SERVICES_FUNCTION) (
  IN  EFI_PEI_SERVICES                  **PeiServices,
  IN  OEM_SERVICES_PPI                  *This,
  IN  UINTN                             NumOfArgs,
  ...
  );

typedef struct _OEM_SERVICES_PPI {
  OEM_SERVICES_FUNCTION                 *Services;
  OEM_SERVICES_FUNCTION                 *ServicesEx;
} OEM_SERVICES_PPI;

extern EFI_GUID gOemServicesPpiGuid;

#endif
