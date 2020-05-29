/** @file
  CrHookDxe driver for memory allocation module header

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

#ifndef _CR_BDA_MEM_MANAGER_H
#define _CR_BDA_MEM_MANAGER_H

#include <PiDxe.h>

#define BDA_CR_EBDA_PRIVATE_DATA_OFFSET     0xF2
#define BDA_CR_EBDA_CRINFO_OFFSET           0xF4
#define BDA_EBDA_OFFSET                     0x40E
#define EBDA_SIZE_OF_KBYTE                  0x00
#define BIOS_PHASE                          0x047
#define SIZE_OF_PARA                        16
#define CR_INITIAL_EBDA_SIZE                1
#define BDA_EBDA_SEG                        0x0E
#define BDA_MEMORY_SIZE                     0x13
#define MAX_CONVENTIONAL_MEMORY             0xA000

#define SIGNATURE(a, b, c, d)               (UINT32)((UINT8)a | (UINT16)b << 8 | (UINT32)c << 16 | (UINT32)d << 24)
#define BDA_DATA(Offset, type)              (*(type *)(UINTN)(0x400+Offset))
#define EBDA_DATA(Offset, type)             (*(type*)(UINTN)(((*(UINT16*)(UINTN)0x40e) << 4) + (Offset)))

extern UINTN    mCrMemStart;

#pragma pack (push)
#pragma pack (1)

typedef struct {
  UINT32    Signature;
  UINT16    EndOfMemory;
  UINT16    UsedOfMemory;
} CR_EFI_MEM_MANAGER;

#pragma pack (pop)

EFI_STATUS
InitCrEfiMemManager (
  VOID
  );
    
EFI_STATUS
FreeCrEfiMemorySpace (
  VOID
  );
  
VOID*
CrEfiMemAlloc (
  IN UINTN     Size
  );

#endif

