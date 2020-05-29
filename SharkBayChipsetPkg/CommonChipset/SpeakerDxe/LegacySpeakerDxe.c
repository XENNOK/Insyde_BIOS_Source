/** @file

;******************************************************************************
;* Copyright (c) 2012-2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/**
  This file contains 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
**/
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

 LegacySpeaker.c

@brief:

  Leagcy Speaker Interface Implementation. This code should be fine with both
  IA32 and Itanium Product family

**/

#include "LegacySpeakerDxe.h"

EFI_SPEAKER_IF_PROTOCOL mSpeakerInterface;


/**

  This function will enable the gate to produce beep

  @retval EFI_SUCCESS

**/
EFI_STATUS
TurnOnSpeaker (
  VOID
  )
{
  UINT8 Data;

  Data = IoRead8 (EFI_SPEAKER_CONTROL_PORT);
  Data |= 0x03;
  IoWrite8 (EFI_SPEAKER_CONTROL_PORT, Data);

  return EFI_SUCCESS;
}

/**

  This function will disable the gate to stop beep

  @retval EFI_SUCCESS
  
**/ 
EFI_STATUS
TurnOffSpeaker (
  VOID
  )
{

  UINT8 Data;

  Data = IoRead8 (EFI_SPEAKER_CONTROL_PORT);
  Data &= EFI_SPEAKER_OFF_MASK;
  IoWrite8 (EFI_SPEAKER_CONTROL_PORT, Data);

  return EFI_SUCCESS;
}

/**

  Generate beep sound based upon number of beeps and duration of the beep

  @param  NumberOfBeep                 Number of beep to be produced
  @param  BeepDuration                 Duration of each beep
  @param  TimeInterval                 Time gap between each beep
  
  @retval EFI_SUCCESS  
  
**/ 
EFI_STATUS
OutputBeep (
  IN UINTN            NumberOfBeep,
  IN UINTN            BeepDuration,
  IN UINTN            TimeInterval
  )
{
  UINTN Num;

  for (Num = 0; Num < NumberOfBeep; Num++) {
    TurnOnSpeaker ();
    gBS->Stall (BeepDuration);
    TurnOffSpeaker ();
    gBS->Stall (TimeInterval);
  }

  return EFI_SUCCESS;
}

/**

  This function will program the speaker tone frequency. The value should be with 64k
  boundary since it takes only 16 bit value which gets programmed in two step IO 
  opearattion.  

  @param  This           Pointer to EFI_SPEAKER_IF_PROTOCOL
  @param  Frequency      A value which should be 16 bit only.

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
ProgramToneFrequency (
  IN  EFI_SPEAKER_IF_PROTOCOL           *This,
  IN  UINT16                            Frequency
  )
{
  UINT8 Data;

  Data = 0xB6;
  IoWrite8 (EFI_TIMER_CONTROL_PORT, Data);

  Data = (UINT8) (Frequency & 0x00FF);
  IoWrite8 (EFI_TIMER_2_PORT, Data);

  Data = (UINT8) ((Frequency & 0xFF00) >> 8);
  IoWrite8 (EFI_TIMER_2_PORT, Data);

  return EFI_SUCCESS;
}

/**

  Generate the Beep Tone

  @param  this           Pointer to EFI_SPEAKER_IF_PROTOCOL
  @param  NumberOfBeeps  Number of beeps which user want to produce
  @param  BeepDuration   Duration for speaker gate need to be enabled
  @param  TimeInterval   Interval between each beep

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
GenerateBeepTone (
  IN  EFI_SPEAKER_IF_PROTOCOL           *this,
  IN  UINTN                             NumberOfBeeps,
  IN  UINTN                             BeepDuration,
  IN  UINTN                             TimeInterval
  )
{

  if (BeepDuration == 0) {
    BeepDuration = EFI_DEFAULT_SHORT_BEEP_DURATION;
  }

  if (TimeInterval == 0) {
    TimeInterval = EFI_DEFAULT_BEEP_TIME_INTERVAL;
  }

  OutputBeep (NumberOfBeeps, BeepDuration, TimeInterval);

  return EFI_SUCCESS;
}

/**

  Initialize the speaker interface protocol

  @param  ImageHandle           EImageHandle of the loaded driver
  @param  SystemTable           Pointer to the System Table

  @retval EFI_SUCCESS           thread can be successfully created
  @retval EFI_OUT_OF_RESOURCES  cannot allocate protocol data structure
  @retval EFI_DEVICE_ERROR      cannot create the timer service

**/
EFI_STATUS
EFIAPI
InitializeSpeakerInterfaceEntryPoint (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_HANDLE  Handle;
  EFI_STATUS  Status;

  //
  // Program Default Tone Frequency so that it can be used. Since
  // EFI_SPEAKER_IF_PROTOCOL is not installed, passing NULL for This.
  //
  ProgramToneFrequency (NULL, EFI_DEFAULT_BEEP_FREQUENCY);

  //
  // Initialize the interfaces
  //
  mSpeakerInterface.SetSpeakerToneFrequency = ProgramToneFrequency;
  mSpeakerInterface.GenerateBeep            = GenerateBeepTone;

  Handle = NULL;

  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiSpeakerInterfaceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mSpeakerInterface
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;

}
