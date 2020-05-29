/** @file

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

#ifndef _DEBUG_COMM_H_
#define _DEBUG_COMM_H_

#include <Uefi.h>

#define H2O_DEBUG_COMM_PPI_GUID \
         {0x8BE3CAA7, 0xF138, 0x41ce, 0x87, 0x71, 0xD3, 0xED, 0x87, 0x62, 0x65, 0xC9}


extern EFI_GUID gH2ODebugCommPpiGuid;

typedef struct _H2O_DEBUG_COMM_PPI H2O_DEBUG_COMM_PPI;

typedef
EFI_STATUS
(EFIAPI *H2O_DEBUG_SEND_INFO) (
  IN H2O_DEBUG_COMM_PPI           *This,
  IN EFI_PHYSICAL_ADDRESS         ImageBase,
  IN UINT32                       ImageSize,
  IN CHAR8                        *PdbPointer,
  IN EFI_PHYSICAL_ADDRESS         ImageEntry
  );

typedef
EFI_STATUS
(EFIAPI *H2O_DEBUG_PRINT) (
  IN H2O_DEBUG_COMM_PPI           *This,
  IN CHAR8                        *Format,
  IN VA_LIST                      Parameters
  );

struct _H2O_DEBUG_COMM_PPI {
  H2O_DEBUG_SEND_INFO             SendInfo;
  H2O_DEBUG_PRINT                 DDTPrint;
};


#endif