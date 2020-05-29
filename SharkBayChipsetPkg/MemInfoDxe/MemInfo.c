/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

//[-start-120712-IB10820084-modify]//
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <MemInfoHob.h>
#include <Protocol/MemInfo.h>

VOID
DumpMemInfo (
  IN      MEM_INFO_PROTOCOL        *MemoryInfo
  );
//[-end-120712-IB10820084-modify]//


EFI_STATUS
EFIAPI
MemInfoEntry (
  IN      EFI_HANDLE              ImageHandle,
  IN      EFI_SYSTEM_TABLE        *SystemTable
  )
{
  EFI_STATUS                  Status;
  EFI_HANDLE                  Handle;
  EFI_HOB_GUID_TYPE           *GuidHob;
  HOB_SAVE_MEMORY_DATA        *HobSaveMemoryData;
  EFI_GUID                    MemRestoreDataGuid = EFI_MEMORY_RESTORE_DATA_GUID;
  MEM_INFO_PROTOCOL           *MemoryInfo;
  UINT8                       Index;
  UINT8                       Channel;
  UINT8                       DIMM;

  Handle            = NULL;
  GuidHob           = NULL;
  HobSaveMemoryData = NULL;
  MemoryInfo        = NULL;
  Index             = 0;
  Channel           = 0;
  DIMM              = 0;

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "\nMemInfo Entry\n" ) );

  //
  // Search for the Memory Restore Data GUID HOB.
  //
  GuidHob = NULL;
  GuidHob = GetFirstGuidHob ( &MemRestoreDataGuid );
//[-start-140625-IB05080432-modify]//
  if (GuidHob == NULL) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "Cannot find MemRestoreData HOB!!!\n" ) );
    ASSERT ( GuidHob != NULL );
    return EFI_NOT_FOUND;
  }
//[-end-140625-IB05080432-modify]//

  GuidHob = ( EFI_HOB_GUID_TYPE * )( UINTN )( GET_GUID_HOB_DATA ( GuidHob ) );
  HobSaveMemoryData = BASE_CR ( GuidHob, HOB_SAVE_MEMORY_DATA, MrcData );

  //
  // Allocate and set MemoryInfo structure to known value
  //
  MemoryInfo = NULL;
  MemoryInfo = AllocateZeroPool ( sizeof ( MEM_INFO_PROTOCOL ) );
//[-start-140625-IB05080432-modify]//
  if (MemoryInfo == NULL) {
    DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "Fail to allocate memory!!!\n" ) );
    ASSERT ( MemoryInfo != NULL );
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-140625-IB05080432-modify]//

  MemoryInfo->MemInfoData.memSize = 0;
  Index                           = 0;
//[-start-120404-IB05300309-modify]//
  for (Channel = 0; Channel < MAX_CH; Channel = Channel + 1) {
    for (DIMM = 0; DIMM < MAX_DIMM; DIMM = DIMM + 1) {
//[-start-140625-IB05080432-add]//
      if (Index >= 4) break;
//[-end-140625-IB05080432-add]//
      MemoryInfo->MemInfoData.dimmSize[Index]  = (UINT16) HobSaveMemoryData->MrcData.SysOut.Outputs.Controller[0].Channel[Channel].Dimm[DIMM].DimmCapacity;
      MemoryInfo->MemInfoData.memSize          = MemoryInfo->MemInfoData.memSize + MemoryInfo->MemInfoData.dimmSize[Index];
      MemoryInfo->MemInfoData.DimmExist[Index] = (BOOLEAN) (HobSaveMemoryData->MrcData.SysSave.Save.Data.Controller[0].Channel[Channel].Dimm[DIMM].Status == DIMM_PRESENT ? TRUE : FALSE);
      Index                                    = Index + 1;
    }
  }
  MemoryInfo->MemInfoData.ddrFreq = (UINT16)HobSaveMemoryData->MrcData.SysOut.Outputs.Frequency;
//[-end-120404-IB05300309-modify]//

//[-start-130531-IB10930028-add]//
  MemoryInfo->MemInfoData.EccSupport  = (BOOLEAN) (HobSaveMemoryData->MrcData.SysOut.Outputs.EccSupport);
//[-end-130531-IB10930028-add]//
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "Install MemInfo Protocol\n" ) );
  //
  // Install MemInfo Protocol.
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gMemInfoProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  MemoryInfo
                  );
  ASSERT_EFI_ERROR ( Status );

  //
  // Dump MemInfo
  //
  DumpMemInfo ( MemoryInfo );

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "MemInfo Exit\n" ) );

  return Status;
}

VOID
DumpMemInfo (
  IN      MEM_INFO_PROTOCOL        *MemoryInfo
  )
{
  UINTN        Count;

  Count = 0;

  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "\n" ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "MemoryInfo ( Address : 0x%x )\n", MemoryInfo ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, " |-MemInfoData ( Address : 0x%x )\n", &MemoryInfo->MemInfoData ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-memSize               : %x\n", MemoryInfo->MemInfoData.memSize ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-ddrFreq               : %x\n", MemoryInfo->MemInfoData.ddrFreq ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-EccSupport            : %x\n", MemoryInfo->MemInfoData.EccSupport ) );
  for ( Count = 0 ; Count < ( NODE_NUM * CH_NUM * DIMM_NUM ) ; Count = Count + 1 ) {
    if ( Count < 10 ) {
      DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-dimmSize[%d]           : %x\n", Count, MemoryInfo->MemInfoData.dimmSize[Count] ) );
    } else {
      DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-dimmSize[%d]          : %x\n", Count, MemoryInfo->MemInfoData.dimmSize[Count] ) );
    }
  }
  for ( Count = 0 ; Count < ( NODE_NUM * CH_NUM * DIMM_NUM ) ; Count = Count + 1 ) {
    if ( Count < 10 ) {
      DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-DimmExist[%d]          : %x\n", Count, MemoryInfo->MemInfoData.DimmExist[Count] ) );
    } else {
      DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-DimmExist[%d]         : %x\n", Count, MemoryInfo->MemInfoData.DimmExist[Count] ) );
    }
  }
  for ( Count = 0 ; Count < ( NODE_NUM * CH_NUM * DIMM_NUM ) ; Count = Count + 1 ) {
    if ( Count < 10 ) {
      DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-RankInDimm[%d]         : %x\n", Count, MemoryInfo->MemInfoData.RankInDimm[Count] ) );
    } else {
      DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-RankInDimm[%d]        : %x\n", Count, MemoryInfo->MemInfoData.RankInDimm[Count] ) );
    }
  }
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-tCL                   : %x\n", MemoryInfo->MemInfoData.Timing[0].tCL ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-tRCD                  : %x\n", MemoryInfo->MemInfoData.Timing[0].tRCD ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-tRP                   : %x\n", MemoryInfo->MemInfoData.Timing[0].tRP ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-tRAS                  : %x\n", MemoryInfo->MemInfoData.Timing[0].tRAS ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-tWR                   : %x\n", MemoryInfo->MemInfoData.Timing[0].tWR ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-tRFC                  : %x\n", MemoryInfo->MemInfoData.Timing[0].tRFC ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-tRRD                  : %x\n", MemoryInfo->MemInfoData.Timing[0].tRRD ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-tWTR                  : %x\n", MemoryInfo->MemInfoData.Timing[0].tWTR ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-tRTP                  : %x\n", MemoryInfo->MemInfoData.Timing[0].tRTP ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-tRC                   : %x\n", MemoryInfo->MemInfoData.Timing[0].tRC ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-tFAW                  : %x\n", MemoryInfo->MemInfoData.Timing[0].tFAW ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "    |-NMode                 : %x\n", MemoryInfo->MemInfoData.Timing[0].NMode ) );
  DEBUG ( ( EFI_D_INFO | EFI_D_ERROR, "\n" ) );

  return;
}
