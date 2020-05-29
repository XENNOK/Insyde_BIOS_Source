/** @file

  Implementation file for Heci Message functionality

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PeiHeciMsgLib.h>

/**
  Start Watch Dog Timer HECI message

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] HeciPpi              The pointer to HECI PPI
  @param[in] HeciMemBar           HECI Memory BAR
  @param[in] WaitTimerBios        The value of waiting limit

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciAsfStartWatchDog (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       PEI_HECI_PPI                *HeciPpi,
  IN       UINT32                      HeciMemBar,
  IN       UINT16                      WaitTimerBios
  )
{
  EFI_STATUS    Status;
  ASF_START_WDT AsfStartWdt;
  UINT32        HeciLength;
  UINT32        MeMode;

  Status = HeciPpi->GetMeMode (PeiServices, &MeMode);
  if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Clear Start Watch Dog Timer HECI message
  ///
  ZeroMem ((VOID *) &AsfStartWdt, sizeof (ASF_START_WDT));

  AsfStartWdt.Command         = ASF_MANAGEMENT_CONTROL;
  AsfStartWdt.ByteCount       = START_WDT_BYTE_COUNT;
  AsfStartWdt.SubCommand      = ASF_SUB_COMMAND_START_WDT;
  AsfStartWdt.VersionNumber   = START_WDT_VERSION_NUMBER;
  AsfStartWdt.TimeoutLow      = (UINT8) WaitTimerBios;
  AsfStartWdt.TimeoutHigh     = (UINT8) (WaitTimerBios >> 8);
  AsfStartWdt.EventSensorType = START_WDT_EVENT_SENSOR_TYPE;
  AsfStartWdt.EventType       = START_WDT_EVENT_TYPE;
  AsfStartWdt.EventOffset     = START_WDT_EVENT_OFFSET;
  AsfStartWdt.EventSourceType = START_WDT_EVENT_SOURCE_TYPE_BIOS;
  AsfStartWdt.EventSeverity   = START_WDT_EVENT_SEVERITY;
  AsfStartWdt.SensorDevice    = START_WDT_SENSOR_DEVICE;
  AsfStartWdt.SensorNumber    = START_WDT_SENSOR_NUMBER;
  AsfStartWdt.Entity          = START_WDT_ENTITY;
  AsfStartWdt.EntityInstance  = START_WDT_ENTITY_INSTANCE;
  AsfStartWdt.EventData[0]    = START_WDT_EVENT_DATA_BYTE_0;
  AsfStartWdt.EventData[1]    = START_WDT_EVENT_DATA_BYTE_1_BIOS_TIMEOUT;

  HeciLength                  = ASF_START_WDT_LENGTH;

  Status = HeciPpi->SendMsg (
                      PeiServices,
                      HeciPpi,
                      (UINT32 *) &AsfStartWdt,
                      HeciMemBar,
                      HeciLength,
                      BIOS_FIXED_HOST_ADDR,
                      HECI_ASF_MESSAGE_ADDR
                      );

  return Status;
}

/**
  Send Core BIOS Reset Request Message through HECI to reset the system.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] ResetOrigin          Reset source
  @param[in] ResetType            Global or Host reset

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
**/
EFI_STATUS
PeiHeciSendCbmResetRequest (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       UINT8                       ResetOrigin,
  IN       UINT8                       ResetType
  )
{
  EFI_STATUS            Status;
  PEI_HECI_PPI          *HeciPpi;
  CBM_RESET_REQ         CbmResetRequest;
  UINT32                HeciLength;
  UINT32                HeciMemBar;
  PLATFORM_ME_HOOK_PPI  *PlatformMeHookPpi;
  WDT_PPI               *WdtPpi;

  Status = PeiServicesLocatePpi (
            &gPlatformMeHookPpiGuid,      /// GUID
            0,                            /// INSTANCE
            NULL,                         /// EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &PlatformMeHookPpi  /// PPI
            );
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Unable to Locate PlatformMeHook Protocol for Global Reset Hook, so skip instead.", Status));
  } else {
    PlatformMeHookPpi->PreGlobalReset (PeiServices, PlatformMeHookPpi);
  }

  Status = PeiServicesLocatePpi (
            &gEfiPeiHeciPpiGuid,      /// GUID
            0,                        /// INSTANCE
            NULL,                     /// EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &HeciPpi        /// PPI
            );
  ASSERT_EFI_ERROR (Status);

  Status = HeciPpi->InitializeHeci (PeiServices, HeciPpi, &HeciMemBar);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  CbmResetRequest.MKHIHeader.Data               = 0;
  CbmResetRequest.MKHIHeader.Fields.Command     = CBM_RESET_REQ_CMD;
  CbmResetRequest.MKHIHeader.Fields.IsResponse  = 0;
  CbmResetRequest.MKHIHeader.Fields.GroupId     = MKHI_CBM_GROUP_ID;
  CbmResetRequest.MKHIHeader.Fields.Reserved    = 0;
  CbmResetRequest.MKHIHeader.Fields.Result      = 0;
  CbmResetRequest.Data.RequestOrigin            = CBM_RR_REQ_ORIGIN_BIOS_MEMORY_INIT;
  CbmResetRequest.Data.ResetType                = CBM_HRR_GLOBAL_RESET;

  HeciLength = sizeof (CBM_RESET_REQ);

  Status = PeiServicesLocatePpi (
            &gWdtPpiGuid,             /// GUID
            0,                        /// INSTANCE
            NULL,                     /// EFI_PEI_PPI_DESCRIPTOR
            (VOID **) &WdtPpi         /// PPI
            );
  ASSERT_EFI_ERROR (Status);
  WdtPpi->AllowKnownReset ();

  Status = HeciPpi->SendMsg (
                      PeiServices,
                      HeciPpi,
                      (UINT32 *) &CbmResetRequest,
                      HeciMemBar,
                      HeciLength,
                      BIOS_FIXED_HOST_ADDR,
                      HECI_CORE_MESSAGE_ADDR
                      );

  return Status;
}
