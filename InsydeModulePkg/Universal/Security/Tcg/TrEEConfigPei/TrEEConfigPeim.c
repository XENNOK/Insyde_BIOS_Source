/** @file
  TrEE configuration module.

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

#include <Guid/TpmInstance.h>

#include <Library/BaseLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/Tpm12DeviceLib.h>
#include <Library/Tpm12CommandLib.h>
#include <Library/KernelConfigLib.h>

#include <Ppi/MasterBootMode.h>

#include <IndustryStandard/Tpm12.h>
#include <KernelSetupConfig.h>
#include <TpmPolicy.h>

#define TPM_INSTANCE_ID_LIST  {                           \
  {TPM_DEVICE_INTERFACE_NONE,           TPM_DEVICE_NULL}, \
  {TPM_DEVICE_INTERFACE_TPM12,          TPM_DEVICE_1_2},  \
  {TPM_DEVICE_INTERFACE_TPM20_DTPM,     TPM_DEVICE_2_0},  \
}

typedef struct {
  GUID                                  TpmInstanceGuid;
  UINT8                                 TpmDevice;
} TPM_INSTANCE_ID;


TPM_INSTANCE_ID  mTpmInstanceId[] = TPM_INSTANCE_ID_LIST;

CONST EFI_PEI_PPI_DESCRIPTOR gTpmSelectedPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiTpmDeviceSelectedGuid,
  NULL
};

EFI_STATUS
EFIAPI
BootModePpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  );

EFI_PEI_NOTIFY_DESCRIPTOR mBootModeNotifyList[] = {
  (EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiPeiMasterBootModePpiGuid,
  BootModePpiNotifyCallback
};

/**
  Send TPM_ContinueSelfTest command to TPM.

  @retval EFI_SUCCESS           Operation completed successfully.
  @retval EFI_TIMEOUT           The register can't run into the expected status in time.
  @retval EFI_BUFFER_TOO_SMALL  Response data buffer is too small.
  @retval EFI_DEVICE_ERROR      Unexpected device behavior.

**/
EFI_STATUS
TpmContinueSelfTest (
  VOID
  )
{
  EFI_STATUS                        Status;
  UINT32                            TpmRecvSize;
  UINT32                            TpmSendSize;
  TPM_RQU_COMMAND_HDR               SendBuffer;
  UINT8                             RecvBuffer[20];

  //
  // send Tpm command TPM_ORD_ContinueSelfTest
  //
  TpmRecvSize           = 20;
  TpmSendSize           = sizeof (TPM_RQU_COMMAND_HDR);
  SendBuffer.tag        = SwapBytes16 (TPM_TAG_RQU_COMMAND);
  SendBuffer.paramSize  = SwapBytes32 (TpmSendSize);  
  SendBuffer.ordinal    = SwapBytes32 (TPM_ORD_ContinueSelfTest);
  
  Status = Tpm12SubmitCommand (TpmSendSize, (UINT8 *)&SendBuffer, &TpmRecvSize, RecvBuffer);
  return Status;
}

/**
  This routine return if dTPM (1.2 or 2.0) present.

  @retval TRUE  dTPM present
  @retval FALSE dTPM not present
**/
BOOLEAN
IsDtpmPresent (
  VOID
  )
{
  UINT8                             RegRead;
  
  RegRead = MmioRead8 ((UINTN)PcdGet64 (PcdTpmBaseAddress));
  if (RegRead == 0xFF) {
    DEBUG ((EFI_D_ERROR, "DetectTpmDevice: Dtpm not present\n"));
    return FALSE;
  } else {
    DEBUG ((EFI_D_ERROR, "DetectTpmDevice: Dtpm present\n"));
    return TRUE;
  }
}

/**
  This routine check both SetupVariable and real TPM device, and return final TpmDevice configuration.

  @param  SetupTpmDevice  TpmDevice configuration in setup driver
  @param  ByPassTpmInit   If TRUE, tries TpmSelfTest to identify TPM.
                          If FALSE, tries TpmStartup to identify TPM.
  
  @return TpmDevice configuration
**/
UINT8
DetectTpmDevice (
  IN UINT8                          SetupTpmDevice,
  IN BOOLEAN                        ByPassTpmInit
  )
{
  EFI_STATUS                        Status;
  EFI_BOOT_MODE                     BootMode;
  UINT32                            TpmPolicy;

  Status = PeiServicesGetBootMode (&BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // In S3, we rely on normal boot Detection
  //
  if (BootMode == BOOT_ON_S3_RESUME) {
    DEBUG ((EFI_D_ERROR, "DetectTpmDevice: S3 mode\n"));
    DEBUG ((EFI_D_ERROR, "TpmDevice from DeviceDetection: %x\n", SetupTpmDevice));
    return SetupTpmDevice;
  }

  DEBUG ((EFI_D_ERROR, "DetectTpmDevice:\n"));
  if (!IsDtpmPresent ()) {
    //
    // dTPM not available
    //
    return TPM_DEVICE_NULL;
  }

  //
  // Check if it is TPM1.2 or TPM2.0 by tring TPM1.2 command at first
  //
  Status = Tpm12RequestUseTpm ();
  if (EFI_ERROR (Status)) {
    return TPM_DEVICE_2_0;
  }

  if (ByPassTpmInit) {
    Status = TpmContinueSelfTest ();
    TpmPolicy = PcdGet32 (PcdPeiTpmPolicy) | SKIP_TPM_SELF_TEST;
  } else {
    Status = Tpm12Startup (TPM_ST_CLEAR);
    TpmPolicy = PcdGet32 (PcdPeiTpmPolicy) | SKIP_TPM_STARTUP;
  }
  if (EFI_ERROR (Status)) {
    return TPM_DEVICE_2_0;
  }

  PcdSet32 (PcdPeiTpmPolicy, TpmPolicy);
  
  return TPM_DEVICE_1_2;
}

/**
  Callback for TrEE configuration driver.

  @param  FileHandle       Handle of the file being invoked.
  @param  NotifyDescriptor Describes the list of possible PEI Services.

  @retval EFI_SUCCES             Convert variable to PCD successfully.
  @retval Others                 Fail to convert variable to PCD.
**/
EFI_STATUS
EFIAPI
BootModePpiNotifyCallback (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR      *NotifyDescriptor,
  IN VOID                           *Ppi
  )
{
  EFI_STATUS                        Status;
  KERNEL_CONFIGURATION              KernelConfiguration;
  UINTN                             Size;
  UINTN                             Index;
  UINT8                             TpmDevice;
  BOOLEAN                           ByPassTpmInit;

  if (!CompareGuid (NotifyDescriptor->Guid, &gEfiPeiMasterBootModePpiGuid)) {
    return EFI_UNSUPPORTED;
  }
  
  Status = GetKernelConfiguration (&KernelConfiguration);

  if (EFI_ERROR (Status) ||
      KernelConfiguration.TpmDevice > TPM_DEVICE_MAX) {
    //
    // Variable not ready or incorrect, set default value
    //
    if (PcdGetBool (PcdH2OTpm2Supported)) {
      KernelConfiguration.TpmDevice   = TPM_DEVICE_2_0;
    } else {
      KernelConfiguration.TpmDevice   = TPM_DEVICE_1_2;
    }
    KernelConfiguration.TpmHide    = 0;
    KernelConfiguration.Tpm2Enable = 1;
  }

  PcdSetBool (PcdTpmHide, KernelConfiguration.TpmHide);
  PcdSetBool (PcdTpm2Enable, KernelConfiguration.Tpm2Enable);
    
  //
  // Although we have setup variable info, we still need detect TPM device manually.
  //
  DEBUG ((EFI_D_ERROR, "KernelConfiguration.TpmDevice from Setup: %x\n", KernelConfiguration.TpmDevice));

  if (PcdGetBool (PcdTpmAutoDetection)) {
    ByPassTpmInit = ((PcdGet32 (PcdPeiTpmPolicy) & SKIP_TPM_STARTUP) == 0) ? FALSE : TRUE;
    TpmDevice = DetectTpmDevice (KernelConfiguration.TpmDevice, ByPassTpmInit);
    DEBUG ((EFI_D_ERROR, "TpmDevice final: %x\n", TpmDevice));
    if (TpmDevice != TPM_DEVICE_NULL) {
      KernelConfiguration.TpmDevice = TpmDevice;
    }
  } else {
    TpmDevice = KernelConfiguration.TpmDevice;
  }

  //
  // Check whether the selected TPM is changed. 
  // Don't hide TPM if TPM is changed
  //
  switch (TpmDevice) {
   
  case TPM_DEVICE_2_0:
    if (KernelConfiguration.TpmDeviceOk == 1) {
        PcdSetBool (PcdTpmHide, FALSE);
    }
    break;
  
  case TPM_DEVICE_1_2:
    if (KernelConfiguration.Tpm2DeviceOk == 1) {
        PcdSetBool (PcdTpmHide, FALSE);
    }
    break;
  }
  
  //
  // Convert variable to PCD.
  //
  for (Index = 0; Index < sizeof (mTpmInstanceId) / sizeof (mTpmInstanceId[0]); Index++) {
    if (TpmDevice == mTpmInstanceId[Index].TpmDevice) {
      Size = sizeof (mTpmInstanceId[Index].TpmInstanceGuid);
      PcdSetPtr (PcdTpmInstanceGuid, &Size, &mTpmInstanceId[Index].TpmInstanceGuid);
      break;
    }
  }

  //
  // Selection done
  //
  Status = PeiServicesInstallPpi (&gTpmSelectedPpi);
  ASSERT_EFI_ERROR (Status);
  
  return Status;
}

/**
  The entry point for TrEE configuration driver.

  @param  FileHandle  Handle of the file being invoked.
  @param  PeiServices Describes the list of possible PEI Services.

  @retval EFI_SUCCES             Successfully registers BootMode service to be invoked
  @retval Others                 Fail to register a callback
**/
EFI_STATUS
EFIAPI
TrEEConfigPeimEntryPoint (
  IN       EFI_PEI_FILE_HANDLE          FileHandle,
  IN CONST EFI_PEI_SERVICES             **PeiServices
  )
{
  EFI_STATUS                            Status;
  
  Status = PeiServicesNotifyPpi (mBootModeNotifyList);
  ASSERT_EFI_ERROR (Status);

  return Status;
}
