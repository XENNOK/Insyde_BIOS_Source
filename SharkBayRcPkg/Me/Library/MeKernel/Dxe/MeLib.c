/** @file

  This is a library to get Intel ME information

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

//[-start-120808-IB10820098-modify]//
#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PerformanceLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <MeLib.h>
#include <MeAccess.h>
#include <Protocol/ExitPmAuth.h>
//[-end-120808-IB10820098-modify]//

CHAR16                gEfiMeBiosExtensionSetupName[] = EFI_ME_BIOS_EXTENSION_SETUP_VARIABLE_NAME;

/**
  Check if Me is enabled

  @param[in] None.

  @retval None
**/
EFI_STATUS
MeLibInit (
  VOID
  )
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  return Status;
}

/**
  Send Get Firmware SKU Request to ME

  @param[in] FwCapsSku            Return Data from Get Firmware Capabilities MKHI Request

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
HeciGetFwCapsSku (
  MEFWCAPS_SKU                    *FwCapsSku
  )
{
  EFI_STATUS                      Status;
  GEN_GET_FW_CAPSKU               MsgGenGetFwCapsSku;
  GEN_GET_FW_CAPS_SKU_ACK         MsgGenGetFwCapsSkuAck;

  Status = HeciGetFwCapsSkuMsg (&MsgGenGetFwCapsSku, &MsgGenGetFwCapsSkuAck);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (((MsgGenGetFwCapsSkuAck.MKHIHeader.Fields.Command) == FWCAPS_GET_RULE_CMD) &&
      ((MsgGenGetFwCapsSkuAck.MKHIHeader.Fields.IsResponse) == 1) &&
      (MsgGenGetFwCapsSkuAck.MKHIHeader.Fields.Result == 0)
      ) {
    *FwCapsSku = MsgGenGetFwCapsSkuAck.Data.FWCapSku;
  }

  return EFI_SUCCESS;
}

/**
  This message is sent by the BIOS or IntelR MEBX prior to the End of Post (EOP)
  on the boot where host wants to get Ibex Peak platform type.
  One of usages is to utilize this command to determine if the platform runs in
  4M or 8M size firmware.

  @param[in] RuleData             PlatformBrand,
                                  IntelMeFwImageType,
                                  SuperSku,
                                  PlatformTargetMarketType,
                                  PlatformTargetUsageType

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
HeciGetPlatformType (
  OUT PLATFORM_TYPE_RULE_DATA     *RuleData
  )
{
  EFI_STATUS  Status;

  Status = HeciGetPlatformTypeMsg (RuleData);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return EFI_SUCCESS;
}

VOID
MbpGiveUp (
  IN  VOID
  )
{
  UINT32                              HECI_BASE_ADDRESS;
  HECI_HOST_CONTROL_REGISTER          HeciRegHCsr;
  volatile HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;

  HECI_BASE_ADDRESS = PciRead32 (PCI_LIB_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, R_HECIMBAR)) & 0xFFFFFFF0;
  HeciRegHCsrPtr    = (VOID *) (UINTN) (HECI_BASE_ADDRESS + H_CSR);

  //
  // Set MBP_GIVE_UP bit
  //
  HeciPciOr32(R_ME_H_GS2, B_ME_MBP_GIVE_UP);

  //
  // Set H_RST and H_IG bits to reset HECI
  //
  HeciRegHCsr.ul      = HeciRegHCsrPtr->ul;
  HeciRegHCsr.r.H_RST  = 1;
  HeciRegHCsr.r.H_IG  = 1;
  HeciRegHCsrPtr->ul  = HeciRegHCsr.ul;
}

/**
  Routine checks whether MBP buffer has been cleared form HECI buffer or not.
  BIOS must check this before executing any 3rd paty code or Oprom

  @param[in] Event                The event that triggered this notification function
  @param[in] Context              Pointer to the notification functions context
**/
VOID
EFIAPI
MbpCleared (
  IN EFI_EVENT                    Event,
  IN VOID                         *Context
  )
{

  HECI_GS_SHDW_REGISTER           MeFwsts2;
  HECI_FWS_REGISTER               MeFirmwareStatus;
  UINT32                          Timeout;

  gBS->CloseEvent (Event);

//[-start-130808-IB05400439-remove]//
//
// Do not uninstall DXE_MBP_DATA_PROTOCOL since we will use it in Setup Utility and Front Page
//
//  Status = gBS->LocateProtocol (&gMeBiosPayloadDataProtocolGuid, NULL, (VOID **) &MbpData);
//  if (!EFI_ERROR (Status)) {
//    //
//    // UnInstall the MBP protocol
//    //
//    Status = gBS->UninstallMultipleProtocolInterfaces (
//                    MbpData->Handle,
//                    &gMeBiosPayloadDataProtocolGuid,
//                    MbpData,
//                    NULL
//                    );
//    if (EFI_ERROR (Status)) {
//      DEBUG ((EFI_D_ERROR, "Unable to Uninstall Mbp protocol and Status is %r\n", Status));
//    }
//  }
//[-end-130808-IB05400439-remove]//

  //
  // Check for MEI1 PCI device availability
  //
  if (HeciPciRead32 (R_VENDORID) == 0xFFFFFFFF) {
    DEBUG ((EFI_D_ERROR, "MEI1 PCI device does not exist\n"));
    return;
  }

  //
  // BIOS polls on FWSTS.MbpCleared until it is set or 1s timeout reached
  //
  Timeout = 0;

  do {
    MeFwsts2.ul = HeciPciRead32 (R_ME_GS_SHDW);

    if (MeFwsts2.r.MbpCleared) {
      break;
    }

    gBS->Stall (FIVE_MS_TIMEOUT);
    Timeout += FIVE_MS_TIMEOUT;
  } while (Timeout <= HECI_MBP_CLR_TIMEOUT);

  DEBUG ((EFI_D_ERROR, "MbpCleared = %x\n", MeFwsts2.r.MbpCleared));

  if (!MeFwsts2.r.MbpCleared) {
    MbpGiveUp();
  }

  MeFirmwareStatus.ul = HeciPciRead32 (R_FWSTATE);
  if (MeFirmwareStatus.r.ErrorCode == ME_ERROR_CODE_UNKNOWN || MeFirmwareStatus.r.ErrorCode == ME_ERROR_CODE_IMAGE_FAILURE) {
    DisableAllMEDevices ();
  }

  return;
}

/**
  Calculate if the circular buffer has overflowed.
  Corresponds to HECI HPS (part of) section 4.2.1

  @param[in] ReadPointer          Location of the read pointer.
  @param[in] WritePointer         Location of the write pointer.

  @retval UINT8                   Number of filled slots.
**/
UINT8
FilledSlots2 (
  IN UINT32                       ReadPointer,
  IN UINT32                       WritePointer
  )
{
  UINT8 FilledSlots;

  ///
  /// Calculation documented in HECI HPS 0.68 section 4.2.1
  ///
  FilledSlots = (((INT8) WritePointer) - ((INT8) ReadPointer));

  return FilledSlots;
}

/**
  This routine returns ME-BIOS Payload information.

  @param[out] MbpPtr              ME-BIOS Payload information.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        Failed to consume MBP
**/
EFI_STATUS
PrepareMeBiosPayload (
  OUT ME_BIOS_PAYLOAD             *MbpPtr
  )
{
  EFI_STATUS                          Status;
  UINT32                              *Ptr;
  UINT32                              MbpItemId;
  UINT32                              MbpItemSize;
  UINT32                              i;
  UINT32                              Index;
  UINT32                              Timeout;
  UINT8                               MbpReadAttempts;
  MBP_HEADER                          MbpHeader;
  MBP_ITEM_HEADER                     MbpItemHeader;
  VOID                                *Registration;
  EFI_EVENT                           Event;

  HECI_GS_SHDW_REGISTER               MeFwsts2;
  UINT32                              HECI_BASE_ADDRESS;
  HECI_ME_CONTROL_REGISTER            HeciRegMeCsrHa;
  HECI_HOST_CONTROL_REGISTER          HeciRegHCsr;
  volatile HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;
  volatile HECI_ME_CONTROL_REGISTER   *HeciRegMeCsrHaPtr;
  volatile UINT32                     *HeciRegMeCbrwPtr;
  VOID                                *DestPtr;
  UINTN                               DestSize;
  BOOLEAN                             MbpReadError;

  Ptr      = NULL;
  Status   = EFI_SUCCESS;

  //
  // Initialize memory mapped register pointers
  //
  HECI_BASE_ADDRESS = PciRead32 (PCI_LIB_ADDRESS (ME_BUS, ME_DEVICE_NUMBER, HECI_FUNCTION_NUMBER, R_HECIMBAR)) & 0xFFFFFFF0;
  HeciRegHCsrPtr    = (VOID *) (UINTN) (HECI_BASE_ADDRESS + H_CSR);
  HeciRegMeCsrHaPtr = (VOID *) (UINTN) (HECI_BASE_ADDRESS + ME_CSR_HA);
  HeciRegMeCbrwPtr  = (VOID *) (UINTN) (HECI_BASE_ADDRESS + ME_CB_RW);

  MeFwsts2.ul = HeciPciRead32 (R_ME_GS_SHDW);

  for (MbpReadAttempts = 0; MbpReadAttempts <= HECI_MBP_READ_MAX_RETRIES; MbpReadAttempts++) {

    DEBUG ((EFI_D_INFO, "MbpPresent = %x\n", MeFwsts2.r.MbpRdy));

    if (MeFwsts2.r.MbpRdy) {

      MbpReadError = FALSE;
      HeciRegMeCsrHa.ul = HeciRegMeCsrHaPtr->ul;

      if (FilledSlots2 (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA) == 0) {
        //
        // Exit if the circular buffer is empty
        //
        DEBUG ((EFI_D_ERROR, "MBP is present but circular buffer is empty, exit.\n"));
        MbpReadError = TRUE;
      } else {

        MbpHeader.Data = *HeciRegMeCbrwPtr;

        DEBUG ((EFI_D_INFO, "MBP header: %x\n", MbpHeader.Data));
        DEBUG ((EFI_D_INFO, "MbpSize: %x\n", MbpHeader.Fields.MbpSize));
        DEBUG ((EFI_D_INFO, "No. of Mbp Entries: %x\n", MbpHeader.Fields.NumEntries));

        MbpPtr->FwCapsSku.Available               = FALSE;
        MbpPtr->FwPlatType.Available              = FALSE;
        MbpPtr->HwaRequest.Available              = FALSE;
        MbpPtr->PlatBootPerfData.Available        = FALSE;
        MbpPtr->HwaRequest.Data.Fields.MediaTablePush = FALSE;
        MbpPtr->NfcSupport.Available = FALSE;

        for (i = 0; i < MbpHeader.Fields.NumEntries; i++) {
          //
          // If MBP Data Item is available but, RD and WR pointer are same, it indicates an error
          // ME FW indication of number of MBP entries and size are wrong
          //
          HeciRegMeCsrHa.ul = HeciRegMeCsrHaPtr->ul;
          if (FilledSlots2 (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA) == 0) {
            //
            // Exit if the circular buffer is empty
            //
            DEBUG ((EFI_D_ERROR, "MBP Item is present but circular buffer is empty, exit.\n"));
            MbpReadError = TRUE;
            break;
          }
          //
          // Consume MBP Item Header
          //
          MbpItemHeader.Data = *HeciRegMeCbrwPtr;
          DEBUG ((EFI_D_INFO, "MBP Item %x header: %x\n", i + 1, MbpItemHeader.Data));

          MbpItemId = (MbpItemHeader.Fields.AppId << 4) | MbpItemHeader.Fields.ItemId;
          //
          // Copy data from HECI buffer per size of each MBP item
          //
          MbpItemSize = MbpItemHeader.Fields.Length - 1;
          Ptr         = AllocateZeroPool ((MbpItemSize) * 4);
          ASSERT (Ptr);
          if (Ptr == NULL) {
            goto MbpGiveup;
          }

          for (Index = 0; Index < MbpItemSize; Index++) {
            Ptr[Index] = *HeciRegMeCbrwPtr;
            DEBUG ((EFI_D_INFO, "MBP Item Data: %x\n", Ptr[Index]));
          }

          switch (MbpItemId) {
          case 0x11:
            //
            // FW Version Name
            //
            DestSize = sizeof (MbpPtr->FwVersionName);
            DestPtr = &MbpPtr->FwVersionName;
            break;

          case 0x12:
            //
            // FW Capabilities
            //
            DestSize = sizeof (MbpPtr->FwCapsSku.FwCapabilities);
            DestPtr = &MbpPtr->FwCapsSku.FwCapabilities;
            MbpPtr->FwCapsSku.Available = TRUE;
            break;

          case 0x13:
            //
            // ROM BIST Data
            //
            DestSize = sizeof (MbpPtr->RomBistData);
            DestPtr = &MbpPtr->RomBistData;
            break;

          case 0x14:
            //
            // Platform Key
            //
            DestSize = sizeof (MbpPtr->PlatformKey);
            DestPtr = &MbpPtr->PlatformKey;
            break;

          case 0x15:
            //
            // ME Platform TYpe
            //
            DestSize = sizeof (MbpPtr->FwPlatType.RuleData);
            DestPtr = &MbpPtr->FwPlatType.RuleData;
            MbpPtr->FwPlatType.Available = TRUE;
            break;

          case 0x16:
            //
            // ME MFS status
            //
            DestSize = sizeof (MbpPtr->MFSIntegrity);
            DestPtr = &MbpPtr->MFSIntegrity;
            break;

          case 0x17:
            ///
            /// Platform timing information
            ///
            DestSize = sizeof (MbpPtr->PlatBootPerfData.MbpPerfData);
            DestPtr = &MbpPtr->PlatBootPerfData.MbpPerfData;
            MbpPtr->PlatBootPerfData.Available = TRUE;
            break;

          case 0x31:
            //
            // AT State
            //
            DestSize = sizeof (MbpPtr->AtState);
            DestPtr = &MbpPtr->AtState;
            break;

          case 0x41:
            //
            // HWA Request
            //
            DestSize = sizeof (MbpPtr->HwaRequest.Data);
            DestPtr = &MbpPtr->HwaRequest.Data;
            MbpPtr->HwaRequest.Available = TRUE;
            break;

          case 0x51:
            //
            // ICC Profile
            //
            DestSize = sizeof (MbpPtr->IccProfile);
            DestPtr = &MbpPtr->IccProfile;
            break;

          case 0x61:
            //
            // NFC Data
            //
            DestSize = sizeof (MbpPtr->NfcSupport.NfcData);
            DestPtr = &MbpPtr->NfcSupport.NfcData;
            MbpPtr->NfcSupport.Available = TRUE;
            break;

          default:
            //
            // default case
            //
            DestSize = 0;
            DestPtr = NULL;
            break;
          }
          if (DestPtr != NULL) {
            if ((MbpItemSize * 4) <= DestSize) {
              CopyMem (DestPtr, Ptr, (MbpItemSize * 4));
            } else {
              DEBUG ((EFI_D_INFO, "Data size is larger than destination buffer. This item is not copied.\n"));
            }
            //
            // Clear buffer
            //
            ZeroMem (Ptr, MbpItemSize * 4);
            FreePool (Ptr);
          }
        }

        //
        // Check if after consuming MBP R/W counters are ok:
        // ME_CBRP_HRA == ME_CBWP_HRA != 0
        //
        HeciRegMeCsrHa.ul = HeciRegMeCsrHaPtr->ul;

        if (FilledSlots2 (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA) != 0
          || HeciRegMeCsrHa.r.ME_CBWP_HRA == 0) {
          DEBUG ((EFI_D_INFO, "R/W pointers mismatch after MBP consume: ME_CBRP_HRA = %x, ME_CBWP_HRA = %x\n",
            HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA));
          MbpReadError = TRUE;
        }

        if (!MbpReadError) {
          //
          // Interrupt ME FW so FW can clear HECI buffer after MBP is consumed
          //
          HeciRegHCsr.ul      = HeciRegHCsrPtr->ul;
          HeciRegHCsr.r.H_IG  = 1;
          HeciRegHCsrPtr->ul  = HeciRegHCsr.ul;

          //
          // Create a callback event to check MBP FWSTS bit
          //
          Status = gBS->CreateEvent (
                          EVT_NOTIFY_SIGNAL,
                          TPL_CALLBACK,
                          MbpCleared,
                          NULL,
                          &Event
                          );
          ASSERT_EFI_ERROR (Status);
          //
          // Register EXIT_PM_AUTH_PROTOCOL notify function
          //
          Status = gBS->RegisterProtocolNotify (
                          &gExitPmAuthProtocolGuid,
                          Event,
                          &Registration
                          );
          ASSERT_EFI_ERROR (Status);

          goto Done;
        }
      }
    } else {
      //
      // If MBP_RDY not set, give up reading MBP and continue
      //
      goto MbpGiveup;
    }

    //
    // If MAX_RETRIES exceeded, give up
    //
    if (MbpReadError && MbpReadAttempts == HECI_MBP_READ_MAX_RETRIES) {
      goto MbpGiveup;
    }

    //
    // Interrupt ME FW so FW knows we're done with this MBP read attempt
    //
    HeciRegHCsr.ul      = HeciRegHCsrPtr->ul;
    HeciRegHCsr.r.H_IG  = 1;
    HeciRegHCsrPtr->ul  = HeciRegHCsr.ul;

    //
    // Wait for MBP_RDY after failed read attempt
    //
    Timeout  = 0;

    do {
      MeFwsts2.ul = HeciPciRead32 (R_ME_GS_SHDW);

      if (MeFwsts2.r.MbpRdy) {
        break;
      }

      gBS->Stall (FIVE_MS_TIMEOUT);
      Timeout += FIVE_MS_TIMEOUT;
    } while (Timeout <= HECI_MBP_RDY_TIMEOUT);
  }

MbpGiveup:
  MbpGiveUp();
  Status = EFI_DEVICE_ERROR;

Done:
  return Status;
}

/**
  Send Get Firmware Version Request to ME

  @param[in] MsgGenGetFwVersionAckData  Return themessage of FW version

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
HeciGetFwVersion (
  IN OUT GEN_GET_FW_VER_ACK_DATA  *MsgGenGetFwVersionAckData
  )
{
  EFI_STATUS                      Status;
  GEN_GET_FW_VER_ACK              MsgGenGetFwVersionAck;

  Status = HeciGetFwVersionMsg (&MsgGenGetFwVersionAck);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  if ((MsgGenGetFwVersionAck.MKHIHeader.Fields.Command == GEN_GET_FW_VERSION_CMD) &&
      (MsgGenGetFwVersionAck.MKHIHeader.Fields.IsResponse == 1) &&
      (MsgGenGetFwVersionAck.MKHIHeader.Fields.Result == 0)
      ) {
    *MsgGenGetFwVersionAckData = MsgGenGetFwVersionAck.Data;
  }

  return EFI_SUCCESS;
}

/**
  Host client gets Firmware update info from ME client

  @param[in] MECapability         Structure of FirmwareUpdateInfo

  @exception EFI_UNSUPPORTED      No MBP Data Protocol available
**/
EFI_STATUS
HeciGetMeFwInfo (
  IN OUT ME_CAP                   *MECapability
  )
{
  EFI_STATUS                      Status;
  DXE_MBP_DATA_PROTOCOL           *MbpData;
  MEFWCAPS_SKU                    FwCapsSku;

  //
  // Get the MBP Data.
  //
  Status = gBS->LocateProtocol (&gMeBiosPayloadDataProtocolGuid, NULL, (VOID **) &MbpData);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "HeciGetMeFwInfo: No MBP Data Protocol available\n"));
    return EFI_UNSUPPORTED;
  }

  MECapability->MeEnabled = 1;

  FwCapsSku.Data          = MbpData->MeBiosPayload.FwCapsSku.FwCapabilities.Data;
  if (FwCapsSku.Fields.IntelAT) {
    MECapability->AtSupported = 1;
  }

  if (FwCapsSku.Fields.KVM) {
    MECapability->IntelKVM = 1;
  }

  switch (MbpData->MeBiosPayload.FwPlatType.RuleData.Fields.PlatformBrand) {
  case INTEL_AMT_BRAND:
    MECapability->IntelAmtFw        = 1;
    MECapability->LocalWakeupTimer  = 1;
    break;

  case INTEL_STAND_MANAGEABILITY_BRAND:
    MECapability->IntelAmtFwStandard = 1;
    break;

  case INTEL_SMALL_BUSINESS_TECHNOLOGY_BRAND:
    MECapability->IntelSmallBusiness = 1;
    break;
  }

  MECapability->MeMajorVer  = MbpData->MeBiosPayload.FwVersionName.MajorVersion;
  MECapability->MeMinorVer  = MbpData->MeBiosPayload.FwVersionName.MinorVersion;
  MECapability->MeBuildNo   = MbpData->MeBiosPayload.FwVersionName.BuildVersion;
  MECapability->MeHotFixNo  = MbpData->MeBiosPayload.FwVersionName.HotfixVersion;

  return Status;
}

/**
  Dummy return for Me signal event use

  @param[in] Event                The event that triggered this notification function
  @param[in] Context              Pointer to the notification functions context
**/
VOID
EFIAPI
MeEmptyEvent (
  IN  EFI_EVENT                   Event,
  IN  VOID                        *Context
  )
{
  return;
}

/**
  Get AT State Information From Stored ME platform policy

  @param[in] AtState              Pointer to AT State Information
  @param[in] AtLastTheftTrigger   Pointer to Variable holding the cause of last AT Stolen Stae
  @param[in] AtLockState          Pointer to variable indicating whether AT is locked or not
  @param[in] AtAmPref             Pointer to variable indicating whether ATAM or PBA should be used

  @retval EFI_SUCCESS             The function completed successfully
  @exception EFI_UNSUPPORTED      No MBP Data Protocol available
**/
EFI_STATUS
GetAtStateInfo (
  IN AT_STATE_INFO                *AtStateInfo
  )
{
  EFI_STATUS                      Status;
  DXE_MBP_DATA_PROTOCOL           *MbpData;

  //
  // Get the ME platform policy.
  //
  Status = gBS->LocateProtocol (&gMeBiosPayloadDataProtocolGuid, NULL, (VOID **) &MbpData);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "GetAtStateInfo: No MBP Data Protocol available\n"));
    return EFI_UNSUPPORTED;
  }
  AtStateInfo->State                         = MbpData->MeBiosPayload.AtState.State;
  AtStateInfo->LastTheftTrigger              = MbpData->MeBiosPayload.AtState.LastTheftTrigger;
  AtStateInfo->flags.LockState               = MbpData->MeBiosPayload.AtState.flags.LockState;
  AtStateInfo->flags.AuthenticateModule      = MbpData->MeBiosPayload.AtState.flags.AuthenticateModule;
  AtStateInfo->flags.S3Authentication        = MbpData->MeBiosPayload.AtState.flags.S3Authentication;
  AtStateInfo->flags.FlashVariableSecurity   = MbpData->MeBiosPayload.AtState.flags.FlashVariableSecurity;
  AtStateInfo->flags.FlashWearOut            = MbpData->MeBiosPayload.AtState.flags.FlashWearOut;

  return EFI_SUCCESS;
}
