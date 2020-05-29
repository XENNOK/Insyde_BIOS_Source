/** @file
  Header file for RapidStart PPI

@copyright
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Mobile Silicon Support Module" and is
  licensed for Intel Mobile CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#ifndef PPI_RAPID_START_H_
#define PPI_RAPID_START_H_

#define RAPID_START_PPI_GUID \
  { \
    0x3d0e663a, 0xdc72, 0x4489, 0x87, 0xc5, 0xe4, 0x9e, 0xe7, 0x73, 0xa4, 0x52 \
  }

#define ATA_PASSWORD_LEN  32

extern EFI_GUID                 gRapidStartPpiGuid;

typedef struct _RAPID_START_PPI RAPID_START_PPI;

///
/// PPI revision number
/// Any backwards compatible changes to this PPI will result in an update in the revision number
/// Major changes will require publication of a new PPI
///
/// Revision 1:  Original version
///
#define RAPID_START_PPI_REVISION_1  1

typedef enum {
  RapidStartNone,
  RapidStartEntry,
  RapidStartExit,
  RapidStartTransitionMax
} RAPID_START_TRANSITION;

/**
  Detremines RapidStart transition.

  This PPI function will determine which Rapid Start transition state should be
  performed. It will check current Rapid Start enable/disable by calling
  RapidStartIsEnabled(), current boot mode, wakeup source and Rapid Start Entry
  Flag. If Rapid Start enabled, current boot mode is S3 Resume and wakeup source
  is one of the supported sources, the Rapid Start Entry will be performed. Once
  Rapid Start Entry completed successfully the Rapid Start Entry Flag will be set
  and next boot GetMode() will check this flag and perform Rapid Start Resume as
  requested.

  GetMode () function will do the following check:
    - Rapid Start function is enabled or not
    - Current boot mode is S3 resume or not
    - If it is S3 resume, check wakeup source is one of enabled Rapid Start Entry
      conditions to perform Rapid Start Entry.
    - If it is not S3 resume, the Rapid Start transition mode should already be
      determined in the RapidStartPeiEntryPoint ().

  After Rapid Start Transition state determined TransitionEntryPoint () PPI
  function will do the necessary Rapid Start transition.

  @param[in] This - A pointer to RAPID_START_PPI itself.

  @retval RAPID_START_TRANSITION - The RapidStart transition being performed
**/
typedef
RAPID_START_TRANSITION
(EFIAPI *RAPID_START_GET_MODE) (
  IN      RAPID_START_PPI      *This
  );

/**
  Executes or schedules RapidStart transition if appropriate.

  This PPI function will perform or schedule an Rapid Start transition (by
  registering either EFI_PEI_PERMANENT_MEMORY_INSTALLED_PPI_GUID or
  PEI_RAPID_START_TRANSITION_START_PPI_GUID callback function). It includes Rapid
  Start Entry or Rapid Start Exit transition basing on Transition mode. If
  Transition mode is RapidStartTransitionMax which means unknown transition mode,
  GetMode() will be called again to determine current Rapid Start transition mode.
  In Rapid Start Entry transition, the memory content will be saved in "Rapid
  Start Store" and Rapid Start Entry Flag will be set then system will be
  switched to Rapid Start state. If error occurred during Rapid Start Entry
  transition, it will switch system back to S3 state. In Rapid Start Exit
  transition, Rapid Start Entry Flag will be cleared and the memory content will
  be restored from "Rapid Start Store". If no error occurred, it will boot
  from S3 resume path back to OS, otherwise it will reset system.

  TransitionEntryPoint () function will do the following check:
    - Rapid Start function is enabled or not
    - If current Rapid Start transition mode is not determined yet, call
      GetMode () PPI function.
    - If current Transition state is "RapidStartNone", do nothing and
      return to caller.
    - If transition state is "Rapid Start Entry", save memory content to
      Rapid Start Store. If no error occurring, set Rapid Start Entry Flag and
      put system to Rapid Start state, otherwise it will re-enter S3 state as
      Rapid Start Entry failed.
    - If transition state is "RapidStartExit", restore memory content from
      Rapid Start Store and override boot mode to "S3 Resume" so that
      system will perform S3 resume back to OS. If error occurred during Rapid
      Start Exit transition it will reset system.
    - When "RapidStartDoTransition ()" is executed in
      EFI_PEI_PERMANENT_MEMORY_INSTALLED_PPI_GUID or
      PEI_RAPID_START_TRANSITION_START_PPI_GUID callback function, platform
      code should ensure the NEM mode has been disabled and cache
      configuration basing on memory map has been completed prior to execute
      RapidStartDoTransition ().
    - SMRAM hashing algorithm in reference code uses SHA256 algorithm in
      "Crypto library" from UDK2010 source code. Those SMRAM hashing
      relevant functions are not included in this document since they are no
      porting required. Refer to Crypto library and BaseCryptLib.h for detail
      information regarding those functions. See section Porting
      Recommendation" for more information.

  @param[in] This       - Pointer to RapidStart PPI
  @param[out] BootMode  - Boot mode that may be overridden

  @retval EFI_SUCCESS   - RapidStart transition performed/scheduled successfully
  @retval EFI_ABORTED   - RapidStart transition aborted
**/
typedef
EFI_STATUS
(EFIAPI *RAPID_START_TRANSITION_ENTRY_POINT) (
  IN      RAPID_START_PPI      * This,
  OUT     EFI_BOOT_MODE        * BootMode
  );

/**
  Check if RapidStart support is enabled.

  This PPI function will return current Rapid Start enable/disable state.

  IsEnabled () function will return current Rapid Start enable/disable state.

  @param[in] This - Pointer to RapidStart PPI

  @retval TRUE if RapidStart support is enabled FALSE otherwise
**/
typedef
BOOLEAN
(EFIAPI *RAPID_START_IS_ENABLED) (
  IN      RAPID_START_PPI      * This
  );

/**
  This PPI provides main functions required for Rapid Start Entry and Exit flow.

  This PPI provides major functions required for Rapid Start transition. Both
  GetMode() and TransitionEntryPoint() should be executed after memory is ready
  for accessing since they will save, restore or check memory content. Besides,
  SATA controller must be initialized to AHCI mode before executing
  TransitionEntryPoint() PPI so Rapid Start Store can be accessed.
**/
struct _RAPID_START_PPI {
  ///
  /// Indicate PPI structure revision.
  ///
  UINT8                               Revision;
  ///
  /// GetMode function will base on current Rapid Start configurations and boot mode
  /// to determine which Rapid Start transition flow should be performed.
  ///
  RAPID_START_GET_MODE                GetMode;
  ///
  /// This function will perform or schedule Rapid Start transition if appropriate.
  /// This includes Rapid Start Entry, Rapid Start Exit or does nothing basing on the
  /// Rapid Start transition mode.
  ///
  RAPID_START_TRANSITION_ENTRY_POINT  TransitionEntryPoint;
  ///
  /// This function returns Rapid Start feature current enable/disable state.
  ///
  RAPID_START_IS_ENABLED              IsEnabled;
};

#endif
