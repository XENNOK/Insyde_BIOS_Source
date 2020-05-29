/** @file

  Framework PEIM to provide WdttPei.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c) 2008 Intel Corporation. All rights reserved
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/
#include "WdttPei.h"
#include <PchRegsLpc.h>
#include <BaseAddrDef.h>
#include <PchRegsRcrb.h>
#include <ChipsetCmos.h>
#include <Library/CmosLib.h>
#include <Library/DebugLib.h>

UINT32             mRcbaAddress = PCH_RCBA_ADDRESS;
UINT16             mTCOBase = ACPI_IO_BASE_ADDRESS + PCH_TCO_BASE;
UINT16             mPmBase = ACPI_IO_BASE_ADDRESS;

//
// Function Implementations
//

/**
  
  Init TCO Timer and preserve original setting.
  
  @param  PeiServices              General purpose services available to every PEIM.
  @param  This                        Wdtt Ppi
  @param  TimerInitValue         Init Value for TCO Timer

  @retval EFI_SUCCESS             The function completed successfully.
  @retval !EFI_SUCCESS            The PEIM failed to execute.

**/
EFI_STATUS
PrepareTCO (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     UINT16                     TimerInitValue    OPTIONAL
  )
{
  PEI_WATT_PRIVATE_DATA       *PrivateData;
  UINT16                      InitValue = 0;
  UINT16                      Data16;
  UINT32                      Data32;

  PrivateData = PEI_WATT_PRIVATE_DATA_FROM_THIS (This);
  PrivateData->CpuIo = (**PeiServices).CpuIo;

  if ( TimerInitValue == 0 || TimerInitValue < MIN_TIMER_COUNT || TimerInitValue > MAX_TIMER_COUNT ) {
    InitValue = DEFAULT_TIMER_COUNT;
  } else {
    InitValue = TimerInitValue;
  }

  //
  // 1.init  TCO_TMR   : TCO_BASE offset   12h[9:0]
  // 2.close TCO_EN    : PM_BASE  offset   30h[13]
  // 3.close NO_REBOOT : RCBA     offset 3410h[5]
  //
  Data16 = PrivateData->CpuIo->IoRead16 ( PeiServices, PrivateData->CpuIo, mTCOBase + R_PCH_TCO_TMR );
  PrivateData->TcoPreserveRegs.TcoTmr = Data16 & 0x3FF;
  Data16 = (Data16 & ~((UINT16)0x3FF)) | (InitValue & 0x3FF);
  PrivateData->CpuIo->IoWrite16 ( PeiServices, PrivateData->CpuIo, mTCOBase + R_PCH_TCO_TMR, Data16 );

  Data16 = PrivateData->CpuIo->IoRead16 ( PeiServices, PrivateData->CpuIo, mPmBase + R_PCH_SMI_EN );
  PrivateData->TcoPreserveRegs.TcoEn = (Data16 & B_PCH_SMI_EN_TCO) >> N_PCH_SMI_EN_TCO;
  Data16 = (Data16 & ~((UINT16)B_PCH_SMI_EN_TCO));
  PrivateData->CpuIo->IoWrite16 ( PeiServices, PrivateData->CpuIo, mPmBase + R_PCH_SMI_EN, Data16 );

  Data32 = PrivateData->CpuIo->MemRead32 ( PeiServices, PrivateData->CpuIo, mRcbaAddress + R_PCH_RCRB_GCS );
  PrivateData->TcoPreserveRegs.NoReboot = (Data32 & B_PCH_RCRB_GCS_NR) >> 5;
  Data32 = (Data32 & ~((UINT32)B_PCH_RCRB_GCS_NR));
  PrivateData->CpuIo->MemWrite32 ( PeiServices, PrivateData->CpuIo, mRcbaAddress + R_PCH_RCRB_GCS, Data32 );
  
  return EFI_SUCCESS;
}

/**
  
  Recover TCO Timer setting
  
  @param  PeiServices              General purpose services available to every PEIM.
  @param  This                        Wdtt Ppi

  @retval EFI_SUCCESS             The function completed successfully.
  @retval !EFI_SUCCESS            The PEIM failed to execute.

**/
EFI_STATUS
RecoverTCO (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This
  )
{
  PEI_WATT_PRIVATE_DATA       *PrivateData;
  UINT16                      Data16;
  UINT32                      Data32;

  PrivateData = PEI_WATT_PRIVATE_DATA_FROM_THIS (This);

  //
  // 1.TCO_TMR   : TCO_BASE offset   12h[9:0]
  // 2.TCO_EN    : PM_BASE  offset   30h[13]
  // 3.NO_REBOOT : RCBA     offset 3410h[5]
  //
  Data16 = PrivateData->CpuIo->IoRead16 ( PeiServices, PrivateData->CpuIo, mTCOBase + R_PCH_TCO_TMR );
  Data16 = (Data16 & ~((UINT16)0x3FF)) | (PrivateData->TcoPreserveRegs.TcoTmr & 0x3FF);
  PrivateData->CpuIo->IoWrite16 ( PeiServices, PrivateData->CpuIo, mTCOBase + R_PCH_TCO_TMR, Data16 );

  Data16 = PrivateData->CpuIo->IoRead16 ( PeiServices, PrivateData->CpuIo, mPmBase + R_PCH_SMI_EN );
  Data16 = (Data16 & ~((UINT16)B_PCH_SMI_EN_TCO)) | (PrivateData->TcoPreserveRegs.TcoEn << N_PCH_SMI_EN_TCO);
  PrivateData->CpuIo->IoWrite16 ( PeiServices, PrivateData->CpuIo, mPmBase + R_PCH_SMI_EN, Data16 );

  Data32 = PrivateData->CpuIo->MemRead32 ( PeiServices, PrivateData->CpuIo, mRcbaAddress + R_PCH_RCRB_GCS );
  Data32 = (Data32 & ~((UINT32)B_PCH_RCRB_GCS_NR)) | (PrivateData->TcoPreserveRegs.NoReboot << 5);
  PrivateData->CpuIo->MemWrite32 ( PeiServices, PrivateData->CpuIo, mRcbaAddress + R_PCH_RCRB_GCS, Data32 );
  
  return EFI_SUCCESS;
}

/**
  
  Start TCO Timer
  
  @param  PeiServices              General purpose services available to every PEIM.
  @param  This                        Wdtt Ppi
  @param  Flag                        Indicate which set of CMOS used to record watchdog status
  @param  TimerInitValue         Init Value for TCO Timer

  @retval EFI_SUCCESS                    The function completed successfully.
  @retval EFI_ALREADY_STARTED     The Watchdog is running now.

**/
EFI_STATUS
StartWDT (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     PEI_WDTT_WATCHDOG_FLAG     Flag,
  IN     UINT16                     TimerInitValue    OPTIONAL
  )
{
  PEI_WATT_PRIVATE_DATA       *PrivateData;
  UINT16                      Data16;
  UINT8                       CmosXTUFlag;

  PrivateData = PEI_WATT_PRIVATE_DATA_FROM_THIS (This);

  if ( PrivateData->WatchdogRunningFlag != 0 ) {
    return EFI_ALREADY_STARTED;
  }


  PrepareTCO ( PeiServices, This, TimerInitValue );

  //
  // 1.reset TCO_RLD   : TCO_BASE offset   00h[9:0] (write any value to it)
  // 2.start countdown : TCO_BASE offset   08h[11]  set to 0
  // 3.mark to CMOS    : Ex_CMOS  offset   59h
  // note : above setting must accord with WDTT action table "SetRunningState" in AcpiWatchDog.c
  //
  Data16 = PrivateData->CpuIo->IoRead16 ( PeiServices, PrivateData->CpuIo, mTCOBase + R_PCH_TCO_RLD );
  Data16 = (Data16 & ~((UINT16)0x3FF)) | DEFAULT_TIMER_COUNT;
  PrivateData->CpuIo->IoWrite16 ( PeiServices, PrivateData->CpuIo, mTCOBase + R_PCH_TCO_RLD, Data16 );

  Data16 = PrivateData->CpuIo->IoRead16 ( PeiServices, PrivateData->CpuIo, mTCOBase + R_PCH_TCO1_CNT );
  Data16 = (Data16 & ~((UINT16)B_PCH_TCO_CNT_TMR_HLT));
  PrivateData->CpuIo->IoWrite16 ( PeiServices, PrivateData->CpuIo, mTCOBase + R_PCH_TCO1_CNT, Data16 );

  CmosXTUFlag = ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus);
  switch ( Flag ) {
    case XtuWatchdogFlagPei1:
      CmosXTUFlag |= B_XTU_WATCH_DOG_FLAG_PEI_1;
      break;

    case XtuWatchdogFlagPei2:
      CmosXTUFlag |= B_XTU_WATCH_DOG_FLAG_PEI_2;
      break;
      
    case XtuWatchdogFlagXmp:
      CmosXTUFlag |= B_XTU_WATCH_DOG_FLAG_XMP;
      break;
  }
  WriteExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus, CmosXTUFlag );

  //
  // set up the flag
  //
  PrivateData->WatchdogRunningFlag = 1;
  
  return EFI_SUCCESS;
}

/**
  
  Stop TCO Timer
  
  @param  PeiServices              General purpose services available to every PEIM.
  @param  This                        Wdtt Ppi
  @param  Flag                        Indicate which set of CMOS used to record watchdog status

  @retval EFI_SUCCESS                    The function completed successfully.

**/
EFI_STATUS
StopWDT (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     PEI_WDTT_WATCHDOG_FLAG     Flag
  )
{
  PEI_WATT_PRIVATE_DATA       *PrivateData;
  UINT16                      Data16;
  UINT8                       CmosXTUFlag;

  PrivateData = PEI_WATT_PRIVATE_DATA_FROM_THIS (This);

  if ( PrivateData->WatchdogRunningFlag == 0 ) {
    return EFI_SUCCESS;
  }

  //
  // 1.stop countdown : TCO_BASE offset   08h[11]  set to 1
  // 2.clear CMOS     : Ex_CMOS  offset   59h
  // note : above setting must accord with WDTT action table "SetStoppedState" in AcpiWatchDog.c
  //
  Data16 = PrivateData->CpuIo->IoRead16 ( PeiServices, PrivateData->CpuIo, mTCOBase + R_PCH_TCO1_CNT );
  Data16 |= B_PCH_TCO_CNT_TMR_HLT;
  PrivateData->CpuIo->IoWrite16 ( PeiServices, PrivateData->CpuIo, mTCOBase + R_PCH_TCO1_CNT, Data16 );

  CmosXTUFlag = ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus);
  switch ( Flag ) {
    case XtuWatchdogFlagPei1:
      CmosXTUFlag &= (UINT8)~B_XTU_WATCH_DOG_FLAG_PEI_1;
      break;

    case XtuWatchdogFlagPei2:
      CmosXTUFlag &= (UINT8)~B_XTU_WATCH_DOG_FLAG_PEI_2;
      break;
      
    case XtuWatchdogFlagXmp:
      CmosXTUFlag &= (UINT8)~B_XTU_WATCH_DOG_FLAG_XMP;
      break;
  }
  WriteExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus, CmosXTUFlag );


  RecoverTCO ( PeiServices, This ) ;

  //
  // clear up the flag
  //
  PrivateData->WatchdogRunningFlag = 0;
  
  return EFI_SUCCESS;
}

/**
  
  Check Watchdog reboot has occured or not
  
  @param  PeiServices              General purpose services available to every PEIM.
  @param  This                        Wdtt Ppi
  @param  Flag                        Indicate which set of CMOS used to record watchdog status

  @retval WdttRunning             Watchdog is running.
  @retval WdttTriggered           Watchdog has reboot system in last POST.
  @retval WdttNormalBoot        Current POST is normally boot.

**/
PEI_WDTT_STATUS
CheckWdtStatus (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     PEI_WDTT_WATCHDOG_FLAG     Flag
  )
{
  PEI_WATT_PRIVATE_DATA       *PrivateData;
  UINT8                       CmosXTUFlag;

  PrivateData = PEI_WATT_PRIVATE_DATA_FROM_THIS (This);

  if ( PrivateData->WatchdogRunningFlag != 0 ) {
    return WdttRunning;
  }

  //
  // check CMOS : Ex_CMOS  offset   59h[0]
  // note : watchdog status here is "different" from WDTT action table "QueryWatchdogStatus" in AcpiWatchDog.c
  //
  CmosXTUFlag = ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus);
  switch ( Flag ) {
    case XtuWatchdogFlagPei1:
      CmosXTUFlag &= B_XTU_WATCH_DOG_FLAG_PEI_1;
      break;

    case XtuWatchdogFlagPei2:
      CmosXTUFlag &= B_XTU_WATCH_DOG_FLAG_PEI_2;
      break;

    case XtuWatchdogFlagXmp:
      CmosXTUFlag &= B_XTU_WATCH_DOG_FLAG_XMP;
      break;
  }
  if ( CmosXTUFlag != 0 ) {
    return WdttTriggered;
  } else {
    return WdttNormalBoot;
  }
}

/**
  
  Clear Watchdog status
  
  @param  PeiServices              General purpose services available to every PEIM.
  @param  This                        Wdtt Ppi
  @param  Flag                        Indicate which set of CMOS used to record watchdog status

  @retval EFI_SUCCESS                   The function completed successfully.
  @retval EFI_ACCESS_DENIED        Access denied, because of watchdog is running.

**/
EFI_STATUS
ClearWdtStatus (
  IN     CONST EFI_PEI_SERVICES     **PeiServices,
  IN     PEI_WDTT_PPI               *This,
  IN     PEI_WDTT_WATCHDOG_FLAG     Flag
  )
{
  PEI_WATT_PRIVATE_DATA       *PrivateData;
  UINT8                       CmosXTUFlag;

  PrivateData = PEI_WATT_PRIVATE_DATA_FROM_THIS (This);

  if ( PrivateData->WatchdogRunningFlag != 0 ) {
    return EFI_ACCESS_DENIED;
  }

  //
  // clear CMOS : Ex_CMOS  offset   59h[0]
  // note : watchdog status here is "different" from WDTT action table "ClearWatchdogStatus" in AcpiWatchDog.c
  //
  CmosXTUFlag = ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus);
  switch ( Flag ) {
    case XtuWatchdogFlagPei1:
      CmosXTUFlag &= (UINT8)~B_XTU_WATCH_DOG_FLAG_PEI_1;
      break;

    case XtuWatchdogFlagPei2:
      CmosXTUFlag &= (UINT8)~B_XTU_WATCH_DOG_FLAG_PEI_2;
      break;

    case XtuWatchdogFlagXmp:
      CmosXTUFlag &= (UINT8)~B_XTU_WATCH_DOG_FLAG_XMP;
      break;
  }
  WriteExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus, CmosXTUFlag );

  return EFI_SUCCESS;
}

/**
  
  Do memory initialization.
  
  @param  FfsHeader              Not used.
  @param  PeiServices             General purpose services available to every PEIM.

  @retval EFI_SUCCESS          Memory initialization completed successfully.
  All other error conditions encountered result in an ASSERT.

**/
EFI_STATUS
WdttEntryPoint (
  IN EFI_PEI_FILE_HANDLE       FileHandle,
  IN CONST EFI_PEI_SERVICES    **PeiServices
  )
{
  EFI_STATUS                  Status;
  PEI_WATT_PRIVATE_DATA       *PrivateData;

  //
  // Allocate Private Data
  //
  Status = (**PeiServices).AllocatePool (
                            PeiServices,
                            sizeof (PEI_WATT_PRIVATE_DATA),
                            (VOID **)&PrivateData
                            );
  if (EFI_ERROR (Status)) {
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // Initialize Private Data (to zero, as is required by subsequent operations)
  //
  (*PeiServices)->SetMem ((UINT8 *) PrivateData, sizeof (PEI_WATT_PRIVATE_DATA), 0);
  PrivateData->Signature    = PEI_WATT_PRIVATE_DATA_SIGNATURE;
  PrivateData->CpuIo = (**PeiServices).CpuIo;

  PrivateData->WdttPpi.StartWDT       = StartWDT;
  PrivateData->WdttPpi.StopWDT        = StopWDT;
  PrivateData->WdttPpi.CheckWdtStatus = CheckWdtStatus;
  PrivateData->WdttPpi.ClearWdtStatus = ClearWdtStatus;

  PrivateData->PpiDescriptor.Flags = EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
  PrivateData->PpiDescriptor.Guid  = &gPeiWdttPpiGuid;
  PrivateData->PpiDescriptor.Ppi   = &(PrivateData->WdttPpi);

  Status = (**PeiServices).InstallPpi (PeiServices, &(PrivateData->PpiDescriptor));
  
  DEBUG ((EFI_D_INFO, "Wdtt Ppi Install Complete.\n"));
  ASSERT_EFI_ERROR (Status);
  
  return Status;
}
