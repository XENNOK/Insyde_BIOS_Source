/** @file
  This file defines the BTS INIT PPI.
;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_BTS_INIT_H_
#define _PEI_BTS_INIT_H_

#include <Pi/PiPeiCis.h>
#include <Ppi/LoadFile.h>

#define PEI_BTS_INIT_PPI_GUID \
  { 0x1c7af2f2, 0xa8cd, 0x4dd1, {0x8c, 0xa4, 0xf7, 0x36, 0x1e, 0xf8, 0xd0, 0x70} }


#define	CPU_MODE16		0
#define	CPU_MODE32		1
#define	CPU_MODE64		2


typedef struct _PEI_BTS_INIT_PPI          PEI_BTS_INIT_PPI;
typedef struct _EFI_PEI_MY_LOAD_FILE_PPI  EFI_PEI_MY_LOAD_FILE_PPI;

struct _EFI_PEI_MY_LOAD_FILE_PPI {
  EFI_PEI_LOAD_FILE           LoadFile;
  EFI_PEI_PPI_DESCRIPTOR      *PeiCoreLoadFileDescriptor;
  EFI_PEI_LOAD_FILE_PPI       *PeiCoreLoadFilePpi;
};

typedef
EFI_STATUS
(EFIAPI *EFI_SEND_INFO) (
  IN PEI_BTS_INIT_PPI             *This,
  IN EFI_PHYSICAL_ADDRESS         ImageBase,
  IN UINT32                       ImageSize,
  IN CHAR8                        *TEImageHdr,
  IN EFI_PHYSICAL_ADDRESS         ImageEntry
  );
  
typedef
VOID
(EFIAPI *EFI_START_BTS) (
);

typedef
VOID
(EFIAPI *EFI_STOP_BTS) (
);

typedef struct _PEI_BTS_INIT_PPI {
  EFI_SEND_INFO             SendInfo;
  EFI_START_BTS             StartBts;
  EFI_STOP_BTS              StopBts;
  EFI_PHYSICAL_ADDRESS      BtsDsAreaAddr;
} PEI_BTS_INIT_PPI;


#define INSYDE_BTS_SEND_INFO(a, b, c, d, e) \
{ \
  EFI_STATUS                Status; \
  PEI_BTS_INIT_PPI          *BtsInit; \
  EFI_GUID                  PeiBtsInitPpiGuid = PEI_BTS_INIT_PPI_GUID; \
  Status = (**(a)).LocatePpi (a, &PeiBtsInitPpiGuid, 0, NULL, &BtsInit); \
  if (Status == EFI_SUCCESS) { \
    BtsInit->SendInfo(BtsInit, (EFI_PHYSICAL_ADDRESS)(b), (UINT32)(c), (CHAR8*)(d), (EFI_PHYSICAL_ADDRESS)(e)); \
  } \
}

#define INSYDE_BTS_START_BTS(a) \
{ \
  EFI_STATUS                Status; \
  PEI_BTS_INIT_PPI          *BtsInit; \
  EFI_GUID                  PeiBtsInitPpiGuid = PEI_BTS_INIT_PPI_GUID; \
  Status = (**(a)).LocatePpi (a, &PeiBtsInitPpiGuid, 0, NULL, &BtsInit); \
  if (Status == EFI_SUCCESS) { \
    BtsInit->StartBts(); \
  } \
}

#define INSYDE_BTS_STOP_BTS(a) \
{ \
  EFI_STATUS                Status; \
  PEI_BTS_INIT_PPI          *BtsInit; \
  EFI_GUID                  PeiBtsInitPpiGuid = PEI_BTS_INIT_PPI_GUID; \
  Status = (**(a)).LocatePpi (a, &PeiBtsInitPpiGuid, 0, NULL, &BtsInit); \
  if (Status == EFI_SUCCESS) { \
    BtsInit->StopBts(); \
  } \
}

extern EFI_GUID gPeiBtsInitPpiGuid;
#endif
