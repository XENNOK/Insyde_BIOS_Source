/** @file

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
--*/
/*++

Copyright (c)  1999 - 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

 LegacySpeaker.c

Abstract:

  This file implements PEIM for Legacy Speaker. This file is valid for platforms both
  on IA32 and Itanium Product Family

--*/

#include "LegacySpeakerPei.h"

EFI_BEEP_TABLE mBeepTable[NUMBERS_OF_BEEP_DATA] = {
  //BeepDuration   TimerInterval
   { 750000,        750000,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0},  //single tone

   {1000000,        250000,
    1000000,        250000,
    1000000,        250000,
     250000,         50000,
     250000,        750000,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0},  //three long and two short beep

   { 250000,        100000,
    1500000,        750000,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0,
        0x0,           0x0}  //a short and a long beep
};

//
// Module globals
//
static PEI_SPEAKER_IF_PPI     mSpeakerInterfacePpi = {
  ProgramToneFrequency,
  GenerateBeepTone,
  MemoryErrorBeep
};

static EFI_PEI_PPI_DESCRIPTOR mPpiListSpeakerInterface = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiSpeakerInterfacePpiGuid,
  &mSpeakerInterfacePpi
};


//[-start-130903-IB12360022-remove]//
///**
// Waits for at least the given number of microseconds.
//
// @param [in]   Microseconds     Desired length of time to wait
//
// @retval EFI_SUCCESS            If the desired amount of time passed.
//
//**/
//EFI_STATUS
//EFIAPI
//Stall (
//  IN  UINTN             Microseconds
//  )
//{
//  UINTN     Ticks;
//  UINTN     Counts;
//  UINT32    CurrentTick;
//  UINT32    OriginalTick;
//  UINT32    RemainingTick;
//
//  if ( Microseconds == 0 ) {
//    return EFI_SUCCESS;
//  }
//
//  OriginalTick = IoRead32 ( ACPI_TIMER_ADDR );
//
//  CurrentTick  = OriginalTick;
//
//  //
//  // The timer frequency is 3.579545 MHz, so 1 ms corresponds 3.58 clocks
//  //
//  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;
//
//  //
//  // The loops needed by timer overflow
//  //
//  Counts = Ticks / ACPI_TIMER_MAX_VALUE;
//
//  //
//  // Remaining clocks within one loop
//  //
//  RemainingTick = Ticks % ACPI_TIMER_MAX_VALUE;
//
//  //
//  // Not intend to use TMROF_STS bit of register PM1_STS, because this adds extra
//  // one I/O operation, and maybe generate SMI
//  //
//  while ( Counts != 0 ) {
//    CurrentTick = IoRead32 ( ACPI_TIMER_ADDR );
//
//    if ( CurrentTick <= OriginalTick ) {
//      Counts = Counts - 1;
//    }
//
//    OriginalTick = CurrentTick;
//  }
//
//  while ( ( RemainingTick > CurrentTick ) && ( OriginalTick <= CurrentTick ) ) {
//    OriginalTick = CurrentTick;
//    CurrentTick  = IoRead32 ( ACPI_TIMER_ADDR );
//  }
//
//  return EFI_SUCCESS;
//}
//[-end-130903-IB12360022-remove]//

/**
 This function will enable the speaker to generate beep


 @retval EFI_SUCCESS            Success

**/
EFI_STATUS
TurnOnSpeaker (
  )
{
  UINT8           Data;

  Data = IoRead8 ( EFI_SPEAKER_CONTROL_PORT );
  Data = Data | 0x03;
  IoWrite8 ( EFI_SPEAKER_CONTROL_PORT, Data );

  return EFI_SUCCESS;
}

/**
 This function will stop beep from speaker.


 @retval EFI_SUCCESS            Success

**/
EFI_STATUS
TurnOffSpeaker (
  )
{
  UINT8           Data;

  Data = IoRead8 ( EFI_SPEAKER_CONTROL_PORT );
  Data = Data & 0xFC;
  IoWrite8 ( EFI_SPEAKER_CONTROL_PORT, Data );

  return EFI_SUCCESS;
}

/**
 Generate beep sound based upon number of beeps and duration of the beep

 @param [in]   NumberOfBeep     Number of beeps which user want to produce
 @param [in]   BeepDuration     Duration for speaker gate need to be enabled
 @param [in]   TimeInterval     Interval between each beep

 @retval EFI_SUCCESS            Success

**/
EFI_STATUS
OutputBeep (
  IN  UINTN                   NumberOfBeep,
  IN  UINTN                   BeepDuration,
  IN  UINTN                   TimeInterval
  )
{
//[-start-130902-IB12360022-add]//
  CONST EFI_PEI_SERVICES     **PeiServices;
  EFI_PEI_STALL_PPI          *PeiStall;
  EFI_STATUS                 Status;
//[-end-130902-IB12360022-add]//
  UINTN                      Num;

//[-start-130902-IB12360022-add]//
  PeiServices = NULL;
  PeiServices = GetPeiServicesTablePointer ();
  if (PeiServices == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  
  Status = PeiServicesLocatePpi (
             &gEfiPeiStallPpiGuid,
             0,
             NULL,
             (VOID **) &PeiStall
             );
  if(EFI_ERROR(Status)) {
    return Status;
  }
//[-end-130902-IB12360022-add]//

//[-start-130903-IB12360022-modify]//
  for (Num = 0; Num < NumberOfBeep; Num++) {
    TurnOnSpeaker ();
    PeiStall->Stall (PeiServices, PeiStall, BeepDuration);
    TurnOffSpeaker ();
    PeiStall->Stall (PeiServices, PeiStall, TimeInterval);
  }
//[-end-130903-IB12360022-modify]//

  return EFI_SUCCESS;
}

/**
 This function will program the speaker tone frequency. The value should be with 64k
 boundary since it takes only 16 bit value which gets programmed in two step IO opearattion

 @param [in]   Frequency        A value which should be 16 bit only.

 @retval EFI_SUCESS             Success

**/
EFI_STATUS
ProgramToneFrequency (
  IN  UINT16                            Frequency
  )
{
  UINT8           Data;

  Data = 0xB6;
  IoWrite8 ( EFI_TIMER_CONTROL_PORT, Data );

  Data = ( UINT8 )( Frequency & 0x00FF );
  IoWrite8 ( EFI_TIMER_2_PORT, Data );

  Data = ( UINT8 )( ( Frequency & 0xFF00 ) >> 8 );
  IoWrite8 ( EFI_TIMER_2_PORT, Data );

  return EFI_SUCCESS;
}

/**
 This function will generate the beep for specified duration.

 @param [in]   NumberOfBeeps    Number of beeps which user want to produce
 @param [in]   BeepDuration     Duration for speaker gate need to be enabled
 @param [in]   TimeInterval     Interval between each beep

 @retval EFI_STATUS
 @retval EFI_SUCCESS            Success

**/
EFI_STATUS
GenerateBeepTone (
  IN  UINTN                             NumberOfBeeps,
  IN  UINTN                             BeepDuration,
  IN  UINTN                             TimeInterval
  )
{
  if ( BeepDuration == 0 ) {
    BeepDuration = EFI_DEFAULT_SHORT_BEEP_DURATION;
  }

  if ( TimeInterval == 0 ) {
    TimeInterval = EFI_DEFAULT_BEEP_TIME_INTERVAL;
  }

  OutputBeep ( NumberOfBeeps, BeepDuration, TimeInterval );
  return EFI_SUCCESS;

}

/**
 This function will generate the beep for specified duration.


 @retval EFI_STATUS

**/
EFI_STATUS
MemoryErrorBeep (
  )
{
  UINTN           Count;
  UINTN           TimesOfBeep;
  UINT8           BeepDataIndex;
  UINT8           BeepDataCount;

  TimesOfBeep = TIMES_OF_MEMORY_ERROR_BEEP;
  BeepDataIndex = MEMORY_ERROR_BEEP_INDEX;

  if (BeepDataIndex < NUMBERS_OF_BEEP_DATA) {
    for (Count = 0x0; Count < TimesOfBeep; Count++) {
    	BeepDataCount = 0x0;
      while (mBeepTable[BeepDataIndex].BeepData[BeepDataCount].BeepDuration != 0x0){
      	OutputBeep (0x1,
      	            mBeepTable[BeepDataIndex].BeepData[BeepDataCount].BeepDuration,
      	            mBeepTable[BeepDataIndex].BeepData[BeepDataCount].TimerInterval );
      	BeepDataCount++;
      }
    }

    return EFI_SUCCESS;
  } else {

  	return EFI_INVALID_PARAMETER;
  }
}

/**
 Initialize the speaker interface protocol

 @param [in]   FileHandle
 @param [in]   PeiServices      PeiServices to locate various PPIs

 @retval Status
 @retval EFI_SUCCESS            thread can be successfully created
 @retval EFI_OUT_OF_RESOURCES   cannot allocate protocol data structure
 @retval EFI_DEVICE_ERROR       cannot create the timer service

**/
EFI_STATUS
EFIAPI
PeimInitializeSpeakerInterfaceEntryPoint (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS  Status;

  //
  // Register so it will be automatically shadowed to memory
  //
  Status = PeiServicesRegisterForShadow (FileHandle);

  if (Status == EFI_ALREADY_STARTED) {

    EFI_PEI_PPI_DESCRIPTOR        *OldDescriptor;
    VOID                          *OldPpi;
  
    //
    // Locate Old Ppi pointer
    //
    Status = PeiServicesLocatePpi (
             &gPeiSpeakerInterfacePpiGuid,
             0,
             &OldDescriptor,
             (VOID **)&OldPpi
             );
    ASSERT_EFI_ERROR (Status);

    //
    // Reinstall Ppi in memory
    //
    if (!EFI_ERROR (Status)) {
      Status = PeiServicesReInstallPpi (OldDescriptor, &mPpiListSpeakerInterface);
      ASSERT_EFI_ERROR (Status);
    }
    
  } else if (!EFI_ERROR (Status)) { 
    //
    // Program Default Tone Frequency so that it can be used
    //
    ProgramToneFrequency (EFI_DEFAULT_BEEP_FREQUENCY);
    Status = PeiServicesInstallPpi (&mPpiListSpeakerInterface);
    ASSERT_EFI_ERROR (Status);
  }
  return Status;
}
