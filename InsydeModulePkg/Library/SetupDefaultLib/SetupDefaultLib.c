/** @file
  This module provides default Setup variable data if Setupvariable is not found
  and also provides a Setup variable cache mechanism in PEI phase

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/KernelConfigLib.h>

extern UINT8  BootVfrSystemConfigDefault0000[];
extern UINT8  PowerVfrSystemConfigDefault0000[];
extern UINT8  SecurityVfrSystemConfigDefault0000[];
extern UINT8  AdvanceVfrSystemConfigDefault0000[];
extern UINT8  MainVfrSystemConfigDefault0000[];

/**
  Extract default Setup variable data from VFR forms

  @param[in,out]  SetupData     A pointer to the Setup variable data buffer
  @param[in]  VfrDefault        The VFR default data of a single VFR form
  @param[in]  SetupDataSize     Data size in bytes of the Setup variable
*/
VOID
ExtractVfrDefault (
    UINT8 *SetupData,
    UINT8 *VfrDefault,
    UINTN SetupDataSize
  )
{
   UINTN   VfrBufSize;
   UINTN   DataSize;
   UINTN   VfrIndex;
   UINTN   NvDataIndex;

   VfrBufSize = (UINTN)( *(UINT32 *)VfrDefault );
   VfrIndex = sizeof (UINT32);
   do {
     NvDataIndex  = *(UINT16 *)(VfrDefault + VfrIndex);
     if (NvDataIndex > SetupDataSize) {
       ASSERT (NvDataIndex < SetupDataSize );
       return;
     }
     VfrIndex    += 2;
     DataSize     = *(UINT16 *)(VfrDefault + VfrIndex);
     VfrIndex    += 2;
     CopyMem(SetupData + NvDataIndex, VfrDefault + VfrIndex, DataSize);
     VfrIndex += DataSize;
   } while (VfrIndex < VfrBufSize);
}

/*
  Update legacy boot type order from PCD setting to kernel configuration

  @param[out]  KernelConfig     A pointer to kernel configuration
*/
VOID
UpdateDefaultLegacyBootTypeOrder (
  OUT KERNEL_CONFIGURATION        *KernelConfig
  )
{
  UINT8                           *DefaultLegacyBootTypeOrder;
  UINTN                           Index;

  if (KernelConfig == NULL) {
    return;
  }

  ZeroMem (KernelConfig->BootTypeOrder, MAX_BOOT_ORDER_NUMBER);

  DefaultLegacyBootTypeOrder = (UINT8 *) PcdGetPtr (PcdLegacyBootTypeOrder);
  if (DefaultLegacyBootTypeOrder == NULL) {
    return;
  }

  Index = 0;
  while (DefaultLegacyBootTypeOrder[Index] != 0 && Index < MAX_BOOT_ORDER_NUMBER) {
    KernelConfig->BootTypeOrder[Index] = DefaultLegacyBootTypeOrder[Index];
    Index++;
  }
}

/**
  Extract default Setup variable data from VFR forms

  @param[in,out]  SetupData     A pointer to the Setup variable data buffer
  @param[in]  SetupDataSize     Data size in bytes of the Setup variable
*/
VOID
ExtractSetupDefault (
  UINT8 *SetupData
  )
{
  UINTN                 Index;
  UINT8                 *VfrDefaults[] = {
                           BootVfrSystemConfigDefault0000,
                           PowerVfrSystemConfigDefault0000,
                           SecurityVfrSystemConfigDefault0000,
                           AdvanceVfrSystemConfigDefault0000,
                           MainVfrSystemConfigDefault0000,
                           NULL
                           };
  UINTN                  SetupDataSize;

  SetupDataSize = GetSetupVariableSize();
  Index = 0;

  while (VfrDefaults[Index] != NULL) {
    ExtractVfrDefault (
      SetupData,
      VfrDefaults[Index],
      SetupDataSize
    );
    Index++;
  }

  UpdateDefaultLegacyBootTypeOrder ((KERNEL_CONFIGURATION *) SetupData);
}