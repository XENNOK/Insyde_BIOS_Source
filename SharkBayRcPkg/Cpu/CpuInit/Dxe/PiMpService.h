/** @file
  some definitions for PI MP services Protocol.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
**/
#ifndef _PI_MP_SERVICE_H_
#define _PI_MP_SERVICE_H_

#include "MpService.h"

///
/// Driver Produced Protocol.
///
/**
  Implementation of GetNumberOfProcessors() service of MP Services Protocol.

  This service retrieves the number of logical processor in the platform
  and the number of those logical processors that are enabled on this boot.
  This service may only be called from the BSP.

  @param[in] This                      - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] NumberOfProcessors        - Pointer to the total number of logical processors in the system,
                              including the BSP and disabled APs.
  @param[in] NumberOfEnabledProcessors - Pointer to the number of enabled logical processors that exist
                              in system, including the BSP.

  @retval EFI_SUCCESS            - Number of logical processors and enabled logical processors retrieved..
  @retval EFI_DEVICE_ERROR       - Caller processor is AP.
  @retval EFI_INVALID_PARAMETER  - NumberOfProcessors is NULL.
  @retval EFI_INVALID_PARAMETER  - NumberOfEnabledProcessors is NULL.
**/
EFI_STATUS
EFIAPI
GetNumberOfProcessors (
  IN  EFI_MP_SERVICES_PROTOCOL     *This,
  OUT UINTN                      *NumberOfProcessors,
  OUT UINTN                      *NumberOfEnabledProcessors
  );

/**
  Implementation of GetNumberOfProcessors() service of MP Services Protocol.

  Gets detailed MP-related information on the requested processor at the
  instant this call is made. This service may only be called from the BSP.

  @param[in] This                - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] ProcessorNumber     -  The handle number of processor.
  @param[in] ProcessorInfoBuffer - A pointer to the buffer where information for the requested processor is deposited.

  @retval EFI_SUCCESS           - Processor information successfully returned.
  @retval EFI_DEVICE_ERROR      - Caller processor is AP.
  @retval EFI_INVALID_PARAMETER - ProcessorInfoBuffer is NULL
  @retval EFI_NOT_FOUND         - Processor with the handle specified by ProcessorNumber does not exist.
**/
EFI_STATUS
EFIAPI
GetProcessorInfo (
  IN       EFI_MP_SERVICES_PROTOCOL     *This,
  IN UINTN                       ProcessorNumber,
  OUT EFI_PROCESSOR_INFORMATION  *ProcessorInfoBuffer
  );

/**
  Implementation of StartupThisAP() service of MP Services Protocol.

  This service lets the caller get one enabled AP to execute a caller-provided function.
  This service may only be called from the BSP.

  @param[in] This                  - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] Procedure             - A pointer to the function to be run on the designated AP.
  @param[in] ProcessorNumber       - The handle number of AP..
  @param[in] WaitEvent             - The event created by the caller.
                                     If it is NULL, then execute in blocking mode.
                                     If it is not NULL, then execute in non-blocking mode.
  @param[in] TimeoutInMicroseconds - The time limit in microseconds for this AP to finish the function.
                                     Zero means infinity.
  @param[in] ProcedureArgument     - Pointer to the optional parameter of the assigned function.
  @param[in] Finished              - Indicates whether AP has finished assigned function.
                                     In blocking mode, it is ignored.

  @retval EFI_SUCCESS           - In blocking mode, specified AP has finished before the timeout expires.
  @retval EFI_SUCCESS           - In non-blocking mode, function has been dispatched to specified AP.
  @retval EFI_DEVICE_ERROR      - Caller processor is AP.
  @retval EFI_TIMEOUT           - In blocking mode, the timeout expires before specified AP has finished.
  @retval EFI_NOT_READY         - Specified AP is busy.
  @retval EFI_NOT_FOUND         - Processor with the handle specified by ProcessorNumber does not exist.
  @retval EFI_INVALID_PARAMETER - ProcessorNumber specifies the BSP or disabled AP.
  @retval EFI_INVALID_PARAMETER - Procedure is NULL.
**/
EFI_STATUS
EFIAPI
PiStartupThisAP (
  IN  EFI_MP_SERVICES_PROTOCOL               * This,
  IN EFI_AP_PROCEDURE            Procedure,
  IN UINTN                       ProcessorNumber,
  IN EFI_EVENT WaitEvent         OPTIONAL,
  IN UINTN                       TimeoutInMicroseconds,
  IN VOID *ProcedureArgument     OPTIONAL,
  OUT BOOLEAN *Finished          OPTIONAL
  );

/**
  Implementation of StartupAllAPs() service of MP Services Protocol.

  This service lets the caller get all enabled APs to execute a caller-provided function.
  This service may only be called from the BSP.

  @param[in] This                  - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] Procedure             - A pointer to the function to be run on enabled APs of the system.
  @param[in] SingleThread          - Indicates whether to execute the function simultaneously or one by one..
  @param[in] WaitEvent             - The event created by the caller.
                                     If it is NULL, then execute in blocking mode.
                                     If it is not NULL, then execute in non-blocking mode.
  @param[in] TimeoutInMicroSeconds - The time limit in microseconds for this AP to finish the function.
                                     Zero means infinity.
  @param[in] ProcedureArgument     - Pointer to the optional parameter of the assigned function.
  @param[in] FailedCpuList         - The list of processor numbers that fail to finish the function before
                                     TimeoutInMicrosecsond expires.

  @retval EFI_SUCCESS           - In blocking mode, all APs have finished before the timeout expired.
  @retval EFI_SUCCESS           - In non-blocking mode, function has been dispatched to all enabled APs.
  @retval EFI_DEVICE_ERROR      - Caller processor is AP.
  @retval EFI_NOT_STARTED       - No enabled AP exists in the system.
  @retval EFI_NOT_READY         - Any enabled AP is busy.
  @retval EFI_TIMEOUT           - In blocking mode, The timeout expired before all enabled APs have finished.
  @retval EFI_INVALID_PARAMETER - Procedure is NULL.
**/
EFI_STATUS
EFIAPI
PiStartupAllAPs (
  IN  EFI_MP_SERVICES_PROTOCOL                           * This,
  IN EFI_AP_PROCEDURE            Procedure,
  IN BOOLEAN                     SingleThread,
  IN EFI_EVENT WaitEvent         OPTIONAL,
  IN UINTN                       TimeoutInMicroSeconds,
  IN VOID *ProcedureArgument     OPTIONAL,
  OUT UINTN **FailedCpuList      OPTIONAL
  );

/**
  Implementation of SwitchBSP() service of MP Services Protocol.

  This service switches the requested AP to be the BSP from that point onward.
  This service may only be called from the current BSP.

  @param[in] This                 - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] ProcessorNumber      - The handle number of processor.
  @param[in] EnableOldBSP         - Whether to enable or disable the original BSP.

  @retval EFI_SUCCESS           - BSP successfully switched.
  @retval EFI_DEVICE_ERROR      - Caller processor is AP.
  @retval EFI_NOT_FOUND         - Processor with the handle specified by ProcessorNumber does not exist.
  @retval EFI_INVALID_PARAMETER - ProcessorNumber specifies the BSP or disabled AP.
  @retval EFI_NOT_READY         - Specified AP is busy.
**/
EFI_STATUS
EFIAPI
PiSwitchBSP (
  IN  EFI_MP_SERVICES_PROTOCOL     *This,
  IN UINTN                       ProcessorNumber,
  IN BOOLEAN                     EnableOldBSP
  );

/**
  Implementation of EnableDisableAP() service of MP Services Protocol.

  This service lets the caller enable or disable an AP.
  This service may only be called from the BSP.

  @param[in] This                  - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] ProcessorNumber       - The handle number of processor.
  @param[in] EnableAP              - Indicates whether the newstate of the AP is enabled or disabled.
  @param[in] HealthFlag            - Indicates new health state of the AP..

  @retval EFI_SUCCESS            - AP successfully enabled or disabled.
  @retval EFI_DEVICE_ERROR       - Caller processor is AP.
  @retval EFI_NOT_FOUND          - Processor with the handle specified by ProcessorNumber does not exist.
  @retval EFI_INVALID_PARAMETERS - ProcessorNumber specifies the BSP.
**/
EFI_STATUS
EFIAPI
PiEnableDisableAP (
  IN  EFI_MP_SERVICES_PROTOCOL     * This,
  IN UINTN                       ProcessorNumber,
  IN BOOLEAN                     EnableAP,
  IN UINT32 *HealthFlag          OPTIONAL
  );

/**
  Implementation of WhoAmI() service of MP Services Protocol.

  This service lets the caller processor get its handle number.
  This service may be called from the BSP and APs.

  @param[in] This                 - A pointer to the EFI_MP_SERVICES_PROTOCOL instance.
  @param[in] ProcessorNumber      - Pointer to the handle number of AP.

  @retval EFI_SUCCESS           - Processor number successfully returned.
  @retval EFI_INVALID_PARAMETER - ProcessorNumber is NULL
**/
EFI_STATUS
EFIAPI
PiWhoAmI (
  IN EFI_MP_SERVICES_PROTOCOL     *This,
  OUT UINTN                      *ProcessorNumber
  );

/**
  Checks APs' status periodically.

  This function is triggerred by timer perodically to check the
  state of APs for StartupAllAPs() and StartupThisAP() executed
  in non-blocking mode.

  @param[in] Event   - Event triggered.
  @param[in] Context - Parameter passed with the event.
**/
VOID
EFIAPI
CheckAPsStatus (
  IN EFI_EVENT Event,
  IN VOID      *Context
  );

#endif
