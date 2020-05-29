/** @file
  Initialize TPM2 device and measure FVs before handing off control to DXE.

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

#include <IndustryStandard/UefiTcgPlatform.h>

#include <Ppi/FirmwareVolumeInfo.h>
#include <Ppi/FirmwareVolumeInfo2.h>
#include <Ppi/LockPhysicalPresence.h>
#include <Ppi/TpmInitialized.h>
#include <Ppi/FirmwareVolume.h>
#include <Ppi/EndOfPeiPhase.h>
#include <Guid/TcgEventHob.h>
#include <Guid/TpmInstance.h>
#include <Guid/MeasuredFvHob.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/Tpm2CommandLib.h>
#include <Library/Tpm2DeviceLib.h>
#include <Library/HashLib.h>
#include <Library/HobLib.h>
#include <Library/PcdLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PerformanceLib.h>
#include <Library/BaseLib.h>

#include <TpmPolicy.h>
#include <Protocol/TrEEProtocol.h>

BOOLEAN                 mImageInMemory = FALSE;

EFI_PEI_PPI_DESCRIPTOR  mTpmInitializedPpiList = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeiTpmInitializedPpiGuid,
  NULL
};

EFI_PLATFORM_FIRMWARE_BLOB mMeasuredBaseFvInfo[FixedPcdGet32 (PcdPeiCoreMaxFvSupported)];
UINT32 mMeasuredBaseFvIndex = 0;

EFI_PLATFORM_FIRMWARE_BLOB mMeasuredChildFvInfo[FixedPcdGet32 (PcdPeiCoreMaxFvSupported)];
UINT32 mMeasuredChildFvIndex = 0;

/**
  Measure and record the Firmware Volum Information once FvInfoPPI install.

  @param[in] PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param[in] NotifyDescriptor  Address of the notification descriptor data structure.
  @param[in] Ppi               Address of the PPI that was installed.

  @retval EFI_SUCCESS          The FV Info is measured and recorded to TPM.
  @return Others               Fail to measure FV.

**/
EFI_STATUS
EFIAPI
FirmwareVolmeInfoPpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

/**
  Record all measured Firmware Volum Information into a Guid Hob

  @param[in] PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param[in] NotifyDescriptor  Address of the notification descriptor data structure.
  @param[in] Ppi               Address of the PPI that was installed.

  @retval EFI_SUCCESS          The FV Info is measured and recorded to TPM.
  @return Others               Fail to measure FV.

**/
EFI_STATUS
EFIAPI
EndofPeiSignalNotifyCallBack (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

EFI_PEI_NOTIFY_DESCRIPTOR           mNotifyList[] = {
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiFirmwareVolumeInfoPpiGuid,
    FirmwareVolmeInfoPpiNotifyCallback
  },
  {
    EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK,
    &gEfiPeiFirmwareVolumeInfo2PpiGuid,
    FirmwareVolmeInfoPpiNotifyCallback
  },
  {
    (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
    &gEfiEndOfPeiSignalPpiGuid,
    EndofPeiSignalNotifyCallBack
  }
};

/**
  Record all measured Firmware Volum Information into a Guid Hob
  Guid Hob payload layout is

     UINT32 *************************** FIRMWARE_BLOB number
     EFI_PLATFORM_FIRMWARE_BLOB******** BLOB Array

  @param[in] PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param[in] NotifyDescriptor  Address of the notification descriptor data structure.
  @param[in] Ppi               Address of the PPI that was installed.

  @retval EFI_SUCCESS          The FV Info is measured and recorded to TPM.
  @return Others               Fail to measure FV.

**/
EFI_STATUS
EFIAPI
EndofPeiSignalNotifyCallBack (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  MEASURED_HOB_DATA                 *MeasuredHobData;

  MeasuredHobData = NULL;
  //
  // Create a Guid hob to save all measured Fv
  //
  MeasuredHobData = BuildGuidHob (
                      &gMeasuredFvHobGuid,
                      sizeof (UINTN) + sizeof (EFI_PLATFORM_FIRMWARE_BLOB) * (mMeasuredBaseFvIndex + mMeasuredChildFvIndex)
                      );

  if (MeasuredHobData != NULL) {
    //
    // Save measured FV info enty number
    //
    MeasuredHobData->Num = mMeasuredBaseFvIndex + mMeasuredChildFvIndex;

    //
    // Save measured base Fv info
    //
    CopyMem (MeasuredHobData->MeasuredFvBuf, mMeasuredBaseFvInfo, sizeof (EFI_PLATFORM_FIRMWARE_BLOB) * (mMeasuredBaseFvIndex));

    //
    // Save measured child Fv info
    //
    CopyMem (&MeasuredHobData->MeasuredFvBuf[mMeasuredBaseFvIndex] , mMeasuredChildFvInfo, sizeof (EFI_PLATFORM_FIRMWARE_BLOB) * (mMeasuredChildFvIndex));
  }

  return EFI_SUCCESS;
}

/**
  This function get SHA1 digest from digest list.

  @param DigestList digest list
  @param Sha1Digest SHA1 digest

  @retval EFI_SUCCESS   Sha1Digest is found and returned.
  @retval EFI_NOT_FOUND Sha1Digest is not found.
**/
EFI_STATUS
Tpm2GetSha1FromDigestList (
  IN TPML_DIGEST_VALUES             *DigestList,
  IN TPM_DIGEST                     *Sha1Digest
  )
{
  UINTN                             Index;

  for (Index = 0; Index < DigestList->count; Index++) {
    if (DigestList->digests[Index].hashAlg == TPM_ALG_SHA1) {
      CopyMem (
        Sha1Digest,
        DigestList->digests[Index].digest.sha1,
        SHA1_DIGEST_SIZE
        );
      return EFI_SUCCESS;
    }
  }

  return EFI_NOT_FOUND;
}

/**
  Do a hash operation on a data buffer, extend a specific TPM PCR with the hash result,
  and build a GUIDed HOB recording the event which will be passed to the DXE phase and
  added into the Event Log.

  @param[in]      HashData      Physical address of the start of the data buffer
                                to be hashed, extended, and logged.
  @param[in]      HashDataLen   The length, in bytes, of the buffer referenced by HashData.
  @param[in]      NewEventHdr   Pointer to a TCG_PCR_EVENT_HDR data structure.
  @param[in]      NewEventData  Pointer to the new event data.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
HashLogExtendEvent (
  IN UINT8                          *HashData,
  IN UINTN                          HashDataLen,
  IN UINT64                         Flags,
  IN TCG_PCR_EVENT_HDR              *NewEventHdr,
  IN UINT8                          *NewEventData
  )
{
  EFI_STATUS                        Status;
  VOID                              *HobData;
  TPML_DIGEST_VALUES                DigestList;

  HobData = NULL;
  Status = HashAndExtend (
             NewEventHdr->PCRIndex,
             HashData,
             HashDataLen,
             &DigestList
             );
  if (!EFI_ERROR (Status)) {
    Status = Tpm2GetSha1FromDigestList (&DigestList, &NewEventHdr->Digest);
    if (!EFI_ERROR (Status)) {
      if ((Flags & TREE_EXTEND_ONLY) == 0) {
        HobData = BuildGuidHob (
                   &gTcgEventEntryHobGuid,
                   sizeof (*NewEventHdr) + NewEventHdr->EventSize
                   );
        if (HobData == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }

        CopyMem (HobData, NewEventHdr, sizeof (*NewEventHdr));
        HobData = (VOID *) ((UINT8*)HobData + sizeof (*NewEventHdr));
        CopyMem (HobData, NewEventData, NewEventHdr->EventSize);
      }
    }
  }
  return Status;
}

/**
  Measure CRTM version.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
EFIAPI
MeasureCRTMVersion (
  VOID
  )
{
  TCG_PCR_EVENT_HDR                 TcgEventHdr;

  if ((PcdGet32 (PcdPeiTpmPolicy) & SKIP_MEASURE_CRTM_VERSION) != 0) {
    return EFI_SUCCESS;
  }
  //
  // Use FirmwareVersion string to represent CRTM version.
  // OEMs should get real CRTM version string and measure it.
  //
  TcgEventHdr.PCRIndex  = 0;
  TcgEventHdr.EventType = EV_S_CRTM_VERSION;
  TcgEventHdr.EventSize = (UINT32)StrSize ((CHAR16*)PcdGetPtr (PcdFirmwareVersionString));

  return HashLogExtendEvent (
           (UINT8*)PcdGetPtr (PcdFirmwareVersionString),
           TcgEventHdr.EventSize,
           0, //TREE_EXTEND_ONLY
           &TcgEventHdr,
           (UINT8*)PcdGetPtr (PcdFirmwareVersionString)
           );
}

/**
  Measure FV image.
  Add it into the measured FV list after the FV is measured successfully.

  @param[in]  FvBase            Base address of FV image.
  @param[in]  FvLength          Length of FV image.

  @retval EFI_SUCCESS           Fv image is measured successfully
                                or it has been already measured.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
EFIAPI
MeasureFvImage (
  IN EFI_PHYSICAL_ADDRESS           FvBase,
  IN UINT64                         FvLength
  )
{
  UINT32                            Index;
  EFI_STATUS                        Status;
  EFI_PLATFORM_FIRMWARE_BLOB        FvBlob;
  TCG_PCR_EVENT_HDR                 TcgEventHdr;

  if ((PcdGet32 (PcdPeiTpmPolicy) & SKIP_MEASURE_FV_IMAGE) != 0) {
    return EFI_SUCCESS;
  }
  //
  // Check whether FV is in the measured FV list.
  //
  for (Index = 0; Index < mMeasuredBaseFvIndex; Index ++) {
    if (mMeasuredBaseFvInfo[Index].BlobBase == FvBase) {
      return EFI_SUCCESS;
    }
  }

  //
  // Measure and record the FV to the TPM
  //
  FvBlob.BlobBase   = FvBase;
  FvBlob.BlobLength = FvLength;

  DEBUG ((DEBUG_INFO, "The FV which is measured by TrEEPei starts at: 0x%x\n", FvBlob.BlobBase));
  DEBUG ((DEBUG_INFO, "The FV which is measured by TrEEPei has the size: 0x%x\n", FvBlob.BlobLength));

  TcgEventHdr.PCRIndex = 0;
  TcgEventHdr.EventType = EV_EFI_PLATFORM_FIRMWARE_BLOB;
  TcgEventHdr.EventSize = sizeof (FvBlob);

  Status = HashLogExtendEvent (
             (UINT8*) (UINTN) FvBlob.BlobBase,
             (UINTN) FvBlob.BlobLength,
             0, //TREE_EXTEND_ONLY
             &TcgEventHdr,
             (UINT8*) &FvBlob
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Add new FV into the measured FV list.
  //
  ASSERT (mMeasuredBaseFvIndex < FixedPcdGet32 (PcdPeiCoreMaxFvSupported));
  if (mMeasuredBaseFvIndex < FixedPcdGet32 (PcdPeiCoreMaxFvSupported)) {
    mMeasuredBaseFvInfo[mMeasuredBaseFvIndex].BlobBase   = FvBase;
    mMeasuredBaseFvInfo[mMeasuredBaseFvIndex].BlobLength = FvLength;
    mMeasuredBaseFvIndex++;
  }

  return Status;
}

/**
  Measure main BIOS.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
EFIAPI
MeasureMainBios (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINT32                            FvInstances;
  EFI_PEI_FV_HANDLE                 VolumeHandle;
  EFI_FV_INFO                       VolumeInfo;
  EFI_PEI_FIRMWARE_VOLUME_PPI       *FvPpi;

  FvInstances = 0;

  if ((PcdGet32 (PcdPeiTpmPolicy) & SKIP_MEASURE_MAIN_BIOS) != 0) {
    return EFI_SUCCESS;
  }

  while (TRUE) {
    //
    // Traverse all firmware volume instances of Static Core Root of Trust for Measurement
    // (S-CRTM), this firmware volume measure policy can be modified/enhanced by special
    // platform for special CRTM TPM measuring.
    //
    Status = PeiServicesFfsFindNextVolume (FvInstances, (VOID **)&VolumeHandle);
    if (Status != EFI_SUCCESS) {
      break;
    }

    //
    // Measure and record the firmware volume that is dispatched by PeiCore
    //
    Status = PeiServicesFfsGetVolumeInfo (VolumeHandle, &VolumeInfo);
    ASSERT_EFI_ERROR (Status);
    //
    // Locate the corresponding FV_PPI according to founded FV's format guid
    //
    Status = PeiServicesLocatePpi (
               &VolumeInfo.FvFormat,
               0,
               NULL,
               (VOID**)&FvPpi
               );
    if (!EFI_ERROR (Status)) {
      MeasureFvImage ((EFI_PHYSICAL_ADDRESS)(UINTN)VolumeInfo.FvStart, VolumeInfo.FvSize);
    }

    FvInstances++;
  }

  return EFI_SUCCESS;
}

/**
  Measure and record the Firmware Volum Information once FvInfoPPI install.

  @param[in] PeiServices       An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param[in] NotifyDescriptor  Address of the notification descriptor data structure.
  @param[in] Ppi               Address of the PPI that was installed.

  @retval EFI_SUCCESS          The FV Info is measured and recorded to TPM.
  @return Others               Fail to measure FV.

**/
EFI_STATUS
EFIAPI
FirmwareVolmeInfoPpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_STATUS                        Status;
  EFI_PEI_FIRMWARE_VOLUME_INFO2_PPI FvInfo2Ppi;
  EFI_PEI_FIRMWARE_VOLUME_PPI       *FvPpi;
  
  if (CompareGuid (NotifyDescriptor->Guid, &gEfiPeiFirmwareVolumeInfo2PpiGuid)) {
    //
    // It is FvInfo2PPI.
    //
    CopyMem (&FvInfo2Ppi, Ppi, sizeof (EFI_PEI_FIRMWARE_VOLUME_INFO2_PPI));
  } else {
    //
    // It is FvInfoPPI.
    //
    CopyMem (&FvInfo2Ppi, Ppi, sizeof (EFI_PEI_FIRMWARE_VOLUME_INFO_PPI));
    FvInfo2Ppi.AuthenticationStatus = 0;
  }

  //
  // The PEI Core can not dispatch or load files from memory mapped FVs that do not support FvPpi.
  //
  Status = PeiServicesLocatePpi (
             &FvInfo2Ppi.FvFormat,
             0,
             NULL,
             (VOID**)&FvPpi
             );
  if (EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  //
  // This is an FV from an FFS file, and the parent FV must have already been measured,
  // No need to measure twice, so just record the FV and return
  //
  if (FvInfo2Ppi.ParentFvName != NULL || FvInfo2Ppi.ParentFileName != NULL) {

    ASSERT (mMeasuredChildFvIndex < FixedPcdGet32 (PcdPeiCoreMaxFvSupported));
    if (mMeasuredChildFvIndex < FixedPcdGet32 (PcdPeiCoreMaxFvSupported)) {
      mMeasuredChildFvInfo[mMeasuredChildFvIndex].BlobBase   = (EFI_PHYSICAL_ADDRESS)(UINTN)FvInfo2Ppi.FvInfo;
      mMeasuredChildFvInfo[mMeasuredChildFvIndex].BlobLength = FvInfo2Ppi.FvInfoSize;
      mMeasuredChildFvIndex++;
    }
    return EFI_SUCCESS;
  }

  return MeasureFvImage ((EFI_PHYSICAL_ADDRESS)(UINTN)FvInfo2Ppi.FvInfo, FvInfo2Ppi.FvInfoSize);
}

/**
  Do measurement after memory is ready.

  @param[in]      PeiServices   Describes the list of possible PEI Services.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_OUT_OF_RESOURCES  No enough memory to log the new event.
  @retval EFI_DEVICE_ERROR      The command was unsuccessful.

**/
EFI_STATUS
EFIAPI
PeimEntryMP (
  IN      EFI_PEI_SERVICES          **PeiServices
  )
{
  EFI_STATUS                        Status;

  Status = MeasureCRTMVersion ();
  ASSERT_EFI_ERROR (Status);

  Status = MeasureMainBios ();

  //
  // Post callbacks:
  // for the FvInfoPpi services to measure and record
  // the additional Fvs to TPM
  //
  Status = PeiServicesNotifyPpi (&mNotifyList[0]);
  ASSERT_EFI_ERROR (Status);

  return Status;
}

/**
  Entry point of this module.

  @param[in] FileHandle   Handle of the file being invoked.
  @param[in] PeiServices  Describes the list of possible PEI Services.

  @return Status.

**/
EFI_STATUS
EFIAPI
PeimEntryMA (
  IN       EFI_PEI_FILE_HANDLE      FileHandle,
  IN CONST EFI_PEI_SERVICES         **PeiServices
  )
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     BootMode;

  if (CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceNoneGuid) ||
      CompareGuid (PcdGetPtr(PcdTpmInstanceGuid), &gEfiTpmDeviceInstanceTpm12Guid)){
    DEBUG ((EFI_D_ERROR, "No TPM2 instance required!\n"));
    return EFI_UNSUPPORTED;
  }

  if (PcdGetBool (PcdTpmHide)) {
    DEBUG((EFI_D_INFO, "TPM is hidden\n"));
    return EFI_UNSUPPORTED;
  }

  //
  // Update for Performance optimization
  //
  Status = Tpm2RequestUseTpm ();
  if (EFI_ERROR (Status)) {
    DEBUG ((DEBUG_ERROR, "TPM not detected!\n"));
    return Status;
  }

  //
  // Initialize TPM device
  //
  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // In S3 path, skip shadow logic. no measurement is required
  //
  if (BootMode != BOOT_ON_S3_RESUME) {
    Status = (**PeiServices).RegisterForShadow(FileHandle);
    if (Status == EFI_ALREADY_STARTED) {
      mImageInMemory = TRUE;
    } else if (Status == EFI_NOT_FOUND) {
      ASSERT_EFI_ERROR (Status);
    }
  }

  if (!mImageInMemory) {
    if ((PcdGet32 (PcdPeiTpmPolicy) & SKIP_TPM_STARTUP) == 0) {
      if (BootMode == BOOT_ON_S3_RESUME) {
        Status = Tpm2Startup (TPM_SU_STATE);
        if (EFI_ERROR (Status)) {
          Status = Tpm2Startup (TPM_SU_CLEAR);
        }
      } else {
        Status = Tpm2Startup (TPM_SU_CLEAR);
      }
      if (EFI_ERROR (Status)) {
        return Status;
      }
    }
    //
    // TpmSelfTest is optional on S3 path, skip it to save S3 time
    //
    if (BootMode != BOOT_ON_S3_RESUME) {
      if ((PcdGet32 (PcdPeiTpmPolicy) & SKIP_TPM_SELF_TEST) == 0) {    
        Status = Tpm2SelfTest (NO);
        if (EFI_ERROR (Status)) {
          return Status;
        }
      }
    }
    Status = PeiServicesInstallPpi (&mTpmInitializedPpiList);
    ASSERT_EFI_ERROR (Status);
  }

  if (mImageInMemory) {
    Status = PeimEntryMP ((EFI_PEI_SERVICES**)PeiServices);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return Status;
}
