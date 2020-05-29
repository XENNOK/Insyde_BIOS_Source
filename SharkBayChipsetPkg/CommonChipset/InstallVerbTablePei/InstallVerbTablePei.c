/** @file
  InstallVerbTablePei peim will produces the InstallVerbTable Ppi for Pei driver.

;******************************************************************************
;* Copyright (c) 2012-2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <InstallVerbTablePei.h>

//
// Ppi Definition
//
STATIC
PEI_VERB_TABLE_PPI mVerbTablePpi = {
  InstallVerbTable
  };

STATIC
EFI_PEI_PPI_DESCRIPTOR mInstallVerbTablePpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeiInstallVerbTablePpiGuid,
  &mVerbTablePpi
  };

/**
  Install InstallVerbTable PPI.

  @param      PeiServices             Pointer's pointer to EFI_PEI_SERVICES.

  @retval     EFI_SUCCESS 

**/
EFI_STATUS
InstallVerbTablePpi (
  IN CONST EFI_PEI_SERVICES                      **PeiServices
  )
{
  EFI_STATUS  Status;

  Status = (**PeiServices).InstallPpi (PeiServices, &mInstallVerbTablePpi);

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_INFO, "\n\nInstall VerbTable PPI : %r \n", Status));
  }
  
  return Status;
}

/**
  A PEIM for install InstallVerbTable PPI.

  @param      FileHandle              Handle of the file being invoked.
  @param      PeiServices             Pointer to PEI Services table.

  @retval     EFI_SUCCESS 

**/
EFI_STATUS
PeimInstallVerbTableInitEntry (
  IN EFI_PEI_FILE_HANDLE        FileHandle,
  IN CONST EFI_PEI_SERVICES     **PeiServices
  )
{
  EFI_STATUS                             Status;
  PEI_VERB_TABLE_PPI                     *VerbTablePpi;
  EFI_PEI_PPI_DESCRIPTOR                 *VerbTablePpiDescriptor;

  //
  // Register so it will be automatically shadowed to memory
  //
  Status = PeiServicesRegisterForShadow (FileHandle);

  if (Status == EFI_ALREADY_STARTED) {
    //
    // Great!! Now that module is in memory,update the PPI 
    // that decribes verb table installation.
    //
    Status = (**PeiServices).LocatePpi (
                               PeiServices, 
                               &gPeiInstallVerbTablePpiGuid,
                               0,
                               &VerbTablePpiDescriptor,
                               (VOID **)&VerbTablePpi
                               );

    //
    // Reinstall the PPI Interface using the memory-based descriptor
    //
    if (!EFI_ERROR (Status)) {
      //
      // Only reinstall the "VerbTable" PPI
      //
      Status = (**PeiServices).ReInstallPpi (
                                 PeiServices,
                                 VerbTablePpiDescriptor,
                                 &mInstallVerbTablePpi
                                 );

      if (EFI_ERROR (Status)) {
        DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Reinstall Verb Table PPI To Memory is failed !\n"));
      }

      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Reinstall Verb Table PPI To Memory is successful !\n"));
    }

    return Status;
  } else if (!EFI_ERROR (Status)) {  

    Status = InstallVerbTablePpi (PeiServices);

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Publish the notification of Verb Table PPI is failed !\n"));
    }
    
  }

  return Status;
}

/**
  Search Codec and Initialize them by verb table installation.

  @param   PeiServices             Pointer's pointer to EFI_PEI_SERVICES
  @param   HdaBar                  The MMIO base address of High Definition Audio (Azalia) Controller.
  @param   OemVerbTableAddress     A Pointer to OEM defined verb table data.
  
  @retval  EFI_SUCCESS 
  
**/
EFI_STATUS
InstallVerbTable (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN COMMON_CHIPSET_AZALIA_VERB_TABLE      *OemVerbTableAddress
  )
{
//[-start-130410-12360005-modify]//
  EFI_STATUS                               Status;
  UINT16                                   SdinWake;
//[-start-130307-12360001-add]//
  UINT16                                   WaitTimerAfterHdaInit;
//[-end-130307-12360001-add]//
  UINT16                                   SdinWakeCompleted;
  EFI_PEI_STALL_PPI                        *StallPpi;
  COMMON_CHIPSET_AZALIA_VERB_TABLE         *VerbTableHeaderDataAddress;

  SdinWake = SdinWakeCompleted = HDA_NO_CODEC;
  VerbTableHeaderDataAddress   = NULL;
//[-end-130410-12360005-modify]//
//[-start-130315-12360001-add]//
  WaitTimerAfterHdaInit      = PcdGet16 (PcdHdaInitWaitTime);
//[-end-130315-12360001-add]//

  //
  // If no verb table data updated, it will load default Verb table data.
  //
  if (OemVerbTableAddress != NULL) {
    VerbTableHeaderDataAddress = OemVerbTableAddress;
  } else {
    //
    // OemServices
    //
    Status = OemSvcGetVerbTable (
               &VerbTableHeaderDataAddress
               );

    if (!EFI_ERROR (Status)) {
      return EFI_SUCCESS;
    }
  }
  if (VerbTableHeaderDataAddress == NULL) {
    return EFI_UNSUPPORTED;
  }

  if (VerbTableHeaderDataAddress[0].VerbTableHeader == NULL) {
    return EFI_UNSUPPORTED;
  }

  Status = (**PeiServices).LocatePpi (
                             (CONST EFI_PEI_SERVICES **)PeiServices, 
                             &gEfiPeiStallPpiGuid,
                             0,
                             NULL,
                             (VOID **)&StallPpi
                             );
  
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "To locate EFI_PEI_STALL_PPI is failed !\n"));
    return Status;
  }
  
  //
  // Oem set combo card delay time
  //
//[-start-130307-12360001-add]//
  OemSvcGetWaitTimerAfterHdaInit (&WaitTimerAfterHdaInit);
//[-end-130307-12360001-add]//
  
  if (!(MmioRead32 (HdaBar + HDA_REG_GCTL) & HDA_REG_GCTL_BIT_CRST)) {
    //
    // If Hda had been initialized, system don't need to init the controller.
    //
    
    //
    // To initialize HDA.
    //
    Status = HdaInitialize (PeiServices, HdaBar, StallPpi);

    if (EFI_ERROR (Status)) {
      //
      // Can't initialize HDA controller.
      //
      goto ExitCodecInitialize;
    }

    Status = PeiCsSvcHdaInitHook (HdaBar);
    
    if (EFI_ERROR (Status)) {
      //
      // Can't initialize HDA controller.
      //
      goto ExitCodecInitialize;
    }

    //
    // Clear State Change Status (STATESTS reg.)
    //
    MmioOr16 (HdaBar + HDA_REG_STATESTS, HDA_MAX_SDIN_FLG);
    
    //
    // To reset HDA.
    //
    Status = HdaReset (PeiServices, HdaBar, StallPpi);
  
    if (EFI_ERROR (Status)) {
      //
      // Can't reset HDA controller.
      //
      goto ExitCodecInitialize;
    }
  
    //
    // To initialize HDA controller.
    //
//[-start-130315-12360001-add]//
    Status = StallPpi->Stall ((CONST EFI_PEI_SERVICES **)PeiServices, StallPpi, (UINTN)WaitTimerAfterHdaInit);
    if (EFI_ERROR(Status)) {
      goto ExitCodecInitialize;
    }
//[-end-130315-12360001-add]//
    Status = HdaInitialize (PeiServices, HdaBar, StallPpi);
  
    if (EFI_ERROR (Status)) {
      //
      // Can't initialize HDA controller.
      //
      goto ExitCodecInitialize;
    }
  }

  if (MmioRead32 (HdaBar + HDA_REG_GCTL) & HDA_REG_GCTL_BIT_CRST) {
    Status = EFI_SUCCESS;
  } else {
    //
    // Retry to init HDA.
    //
    MmioOr32 (HdaBar + HDA_REG_GCTL, HDA_REG_GCTL_BIT_CRST);
    Status = StallPpi->Stall ((CONST EFI_PEI_SERVICES **)PeiServices, StallPpi, HDA_INIT_FAIL_RETRY_STALL_TIME);
  }
  
  //
  // To find out all codec through identification of the state change status
  // "STATESTS" bits.
  //
//[-start-130410-12360005-modify]//
  if (MmioRead32 (HdaBar + HDA_REG_GCTL) & HDA_REG_GCTL_BIT_CRST) {
    Status = FindOutCodec (PeiServices, HdaBar, StallPpi, &SdinWake);
  } else {
    Status = EFI_DEVICE_ERROR;
  }
    
  if (EFI_ERROR (Status)) {
    //
    // When Status is EFI_DEVICE_ERROR, HDA initialization is failed.
    // When Status is others error state, no codec be found.
    //
    goto ExitCodecInitialize;
  }
  
  do {
    SdinWake &= (~SdinWakeCompleted);
    //
    // Start to configure Verb Table for all existed codecs.
    //
    Status = ConfigureVerbTable (PeiServices, HdaBar, StallPpi, SdinWake, VerbTableHeaderDataAddress);
    if (EFI_ERROR (Status)) {
      //
      // Configure Verb Table has problem to stop procedure.
      //
      break;
    }

    SdinWakeCompleted |= SdinWake;
    //
    // To Check the codec number if it has changed or not.
    // If YES, system must install verb table for these codecs those 
    // are not be found at first time.
    //
    // If NOT, system doesn't need to install verb table again.
    //  
    if (MmioRead32 (HdaBar + HDA_REG_GCTL) & HDA_REG_GCTL_BIT_CRST) {
      Status = FindOutCodec (PeiServices, HdaBar, StallPpi, &SdinWake);
    } else {
      Status = EFI_DEVICE_ERROR;
    }

    if (EFI_ERROR (Status)) {
      //
      // When Status is EFI_DEVICE_ERROR, HDA initialization is failed.
      // When Status is others error state, no codec be found.
      //
      break;
    }
  } while ((SdinWake | SdinWakeCompleted) != SdinWakeCompleted);
    
  if (EFI_ERROR (Status)) {
    goto ExitCodecInitialize;
  }
//[-end-130410-12360005-modify]//
  return Status;

ExitCodecInitialize:
  MmioAnd32 (HdaBar + HDA_REG_GCTL, (UINTN)~HDA_REG_GCTL_BIT_CRST);
  
  return Status;
}


/**
  Hda initialize itself.
  
  @param      PeiServices    Pointer's pointer to EFI_PEI_SERVICES
  @param      HdaBar         The MMIO base address of High Definition Audio Controller.
  @param      StallPpi       A pointer to PEI_STALL_PPI.
  
  @retval     EFI_SUCCESS 

**/
EFI_STATUS
HdaInitialize (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi
  )
{
  UINTN       LoopCounter;
  EFI_STATUS  Status;

  LoopCounter = TIME_OUT_LOOP_INIT;
  
  //
  // Let controller initializes itself. Turn on link and let state machines initialize themselves.
  // Until CRST bit be read back as 1, controller has initialized itself and registers accept to 
  // be writen.
  //
  MmioOr32 (HdaBar + HDA_REG_GCTL, HDA_REG_GCTL_BIT_CRST);

  do {
    if (MmioRead32 (HdaBar + HDA_REG_GCTL) & HDA_REG_GCTL_BIT_CRST) {
      break;
    } else{
      Status = StallPpi->Stall ((CONST EFI_PEI_SERVICES **)PeiServices, StallPpi, STALL_TIME);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      LoopCounter++;
    }
  } while (LoopCounter != TIME_OUT_MAX_LOOP);

  if (LoopCounter == TIME_OUT_MAX_LOOP) {
    //
    // Can't initialize HDA.
    //
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}


/**
  Hda reset itself.
  
  @param      PeiServices    Pointer's pointer to EFI_PEI_SERVICES
  @param      HdaBar         The MMIO base address of High Definition Audio Controller.
  @param      StallPpi       A pointer to PEI_STALL_PPI.
  
  @retval     EFI_SUCCESS 

**/
EFI_STATUS
HdaReset (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi
  )
{
  UINTN       LoopCounter;
  EFI_STATUS  Status;

  LoopCounter = TIME_OUT_LOOP_INIT;
  
  //
  // Let controller in reset and turn off link.This step is a goal to reset HDA controller.
  //
  MmioAnd32 (HdaBar + HDA_REG_GCTL, (UINTN)~HDA_REG_GCTL_BIT_CRST);

  do {
    if (MmioRead32 (HdaBar + HDA_REG_GCTL) & HDA_REG_GCTL_BIT_CRST) {
      Status = StallPpi->Stall ((CONST EFI_PEI_SERVICES **)PeiServices, StallPpi, STALL_TIME);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      LoopCounter++;
    } else{
      break;
    }
  } while (LoopCounter != TIME_OUT_MAX_LOOP);

  if (LoopCounter == TIME_OUT_MAX_LOOP) {
    //
    // Can't reset HDA.
    //
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}


/**
  To find out all Codec.

  @param      PeiServices    Pointer's pointer to EFI_PEI_SERVICES
  @param      HdaBar         The MMIO base address of High Definition Audio Controller.
  @param      StallPpi       A pointer to PEI_STALL_PPI.
  @param      SdinWake       A pointer to the data of register "STATESTS"
  
  @retval     EFI_SUCCESS 

**/
EFI_STATUS
FindOutCodec (
  IN     EFI_PEI_SERVICES                  **PeiServices,
  IN     UINT32                            HdaBar,
  IN     EFI_PEI_STALL_PPI                 *StallPpi,
  IN OUT UINT16                            *SdinWake
  )
{
  UINTN       LoopCounter;
  EFI_STATUS  Status;

  LoopCounter = TIME_OUT_LOOP_INIT;
  
  do {
    *SdinWake = MmioRead16 (HdaBar + HDA_REG_STATESTS) & HDA_MAX_SDIN_FLG;
    if (*SdinWake != HDA_NO_CODEC) {
      break;
    } else{
      Status = StallPpi->Stall ((CONST EFI_PEI_SERVICES **)PeiServices, StallPpi, DETECT_CODEC_STALL_TIME);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      LoopCounter++;
    }
  } while (LoopCounter != DETECT_CODEC_TIME_OUT_MAX_LOOP);

  if ((LoopCounter == DETECT_CODEC_TIME_OUT_MAX_LOOP) || (*SdinWake == HDA_NO_CODEC)) {
    //
    // No Codec be found.
    //
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}


/**
  To deliver command data in ICW register to deliver to Codec.

  @param      PeiServices  -  Pointer's pointer to EFI_PEI_SERVICES
  @param      HdaBar       -  The MMIO base address of High Definition Audio Controller.
  @param      StallPpi     -  A pointer to PEI_STALL_PPI.
  @param      CommandData  -  The command data.

  @retval     EFI_SUCCESS 

**/
EFI_STATUS
DeliverCommandToCodec (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi,
  IN UINT32                                CommandData
  )
{
  UINTN       LoopCounter;
  EFI_STATUS  Status;

  LoopCounter = TIME_OUT_LOOP_INIT;

  do {
    if (MmioRead16 (HdaBar + HDA_REG_ICS) & HDA_REG_ICS_BIT_ICB) {
      Status = StallPpi->Stall ((CONST EFI_PEI_SERVICES **)PeiServices, StallPpi, STALL_TIME);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      LoopCounter++;
    } else {
      break;
    }
  } while (LoopCounter != TIME_OUT_MAX_LOOP);

  if (LoopCounter == TIME_OUT_MAX_LOOP) {
    return EFI_TIMEOUT;
  }

  MmioOr16 (HdaBar + HDA_REG_ICS, HDA_REG_ICS_BIT_IRV);
  MmioWrite32 ((HdaBar + HDA_REG_ICW), CommandData);
  MmioOr16 (HdaBar + HDA_REG_ICS, HDA_REG_ICS_BIT_ICB);
  
  return EFI_SUCCESS;
}


/**
  To receive response data from Codec.

  @param      FPeiServices   -  Pointer's pointer to EFI_PEI_SERVICES
  @param      PHdaBar        -  The MMIO base address of High Definition Audio Controller.
  @param      StallPpi      -  A pointer to PEI_STALL_PPI.
  @param      ResponseData  -  data be received from Codec.

  @retval     EFI_SUCCESS 

**/
EFI_STATUS
ReceiveCodecData (
  IN     EFI_PEI_SERVICES                  **PeiServices,
  IN     UINT32                            HdaBar,
  IN     EFI_PEI_STALL_PPI                 *StallPpi,
  IN OUT UINT32                            *ResponseData
  )
{
  UINTN       LoopCounter;
  EFI_STATUS  Status;

  LoopCounter = TIME_OUT_LOOP_INIT;

  do {
    if (MmioRead16 (HdaBar + HDA_REG_ICS) & HDA_REG_ICS_BIT_IRV) {
      break;
    } else {
      Status = StallPpi->Stall ((CONST EFI_PEI_SERVICES **)PeiServices, StallPpi, STALL_TIME);
      if (EFI_ERROR (Status)) {
        return Status;
      }
      LoopCounter++;
    }
  } while (LoopCounter != TIME_OUT_MAX_LOOP);

  if (LoopCounter == TIME_OUT_MAX_LOOP) {
    return EFI_TIMEOUT;
  }

  *ResponseData = MmioRead32 (HdaBar + HDA_REG_IRR);

  return EFI_SUCCESS;
}

//[-start-130410-12360005-add]//
/**
  To install verb table for all existed codecs.

  @param      PeiServices            Pointer's pointer to EFI_PEI_SERVICES
  @param      HdaBar                 The MMIO base address of High Definition Audio (Azalia) Controller.
  @param      StallPpi               A pointer to PEI_STALL_PPI.
  @param      SdinWake               The data of register "STATESTS"
  @param      OemVerbTableAddress    A Pointer to OEM defined verb table data.

  @retval  EFI_SUCCESS 
  
**/
EFI_STATUS
ConfigureVerbTable (
  IN EFI_PEI_SERVICES                      **PeiServices,
  IN UINT32                                HdaBar,
  IN EFI_PEI_STALL_PPI                     *StallPpi,
  IN UINT16                                SdinWake,
  IN COMMON_CHIPSET_AZALIA_VERB_TABLE      *VerbTableHeaderDataAddress
  )
{
  EFI_STATUS                               Status;
  UINT8                                    Index;
  UINT8                                    CodecNum;
  UINT32                                   SdinBit;
  UINT32                                   CodecAddress;
  UINT32                                   DeliverCommandData;
  UINT32                                   ResponseData;
  UINT32                                   VendorDeviceId;
  UINT32                                   RevisionId;
  UINT32                                   JackIndex;
  UINT32                                   TotalJackNum;
  UINT32                                   *VerbTableDataBuffer;
  COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  *VerbTableHeaderBuffer;

  Status  = EFI_SUCCESS;
  SdinBit = HDA_SDIN_INIT;

  for (CodecNum = 0; CodecNum < HDA_MAX_SDIN_NUM; CodecNum++, SdinBit <<= 1) {
    //
    // To confirm every SDIN if there is a codec on link.
    //
    if ((SdinWake & SdinBit) != SdinBit) {
      //
      // To search the next SDIN to detect Codec.
      //
      continue;
    }

    // Verb Layout Format:
    //
    //  31    28 27 26      20 19                           0
    //  -----------------------------------------------------
    //  |  Cad  | I |   NID   |     Verb & Command data     |
    //  -----------------------------------------------------
    //           ^^^
    //            0 = Direct NID reference
    //            1 = Indirect NID reference
    // Note : when to use controls and parameters through this Format.

    //
    // To read the Codec's vendor and device ID.
    //
    CodecAddress = (UINT32)CodecNum;
    DeliverCommandData = ((CONTROL_GET_PARAMETER << 8) | PARAMETER_VENDOR_DEVICE_ID) | (CodecAddress << 28);
    Status = DeliverCommandToCodec (PeiServices, HdaBar, StallPpi, DeliverCommandData);
    Status = ReceiveCodecData (PeiServices, HdaBar, StallPpi, &ResponseData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    VendorDeviceId = ResponseData;

    //
    // To read the Codec's revision ID.
    //
    DeliverCommandData = ((CONTROL_GET_PARAMETER << 8) | PARAMETER_REVISION_ID) | (CodecAddress << 28);
    Status = DeliverCommandToCodec (PeiServices, HdaBar, StallPpi, DeliverCommandData);
    Status = ReceiveCodecData (PeiServices, HdaBar, StallPpi, &ResponseData);
    if (EFI_ERROR (Status)) {
      return Status;
    }

    RevisionId = (ResponseData >> 8) & 0xFF;

    //
    // Start to program codec with verb table.
    //

    for (Index = 0; VerbTableHeaderDataAddress[Index].VerbTableHeader != NULL; Index++) {
      VerbTableHeaderBuffer = VerbTableHeaderDataAddress[Index].VerbTableHeader;

      if ((VerbTableHeaderBuffer->VendorDeviceId == VendorDeviceId) && 
          ((VerbTableHeaderBuffer->RevisionId == RevisionId) || 
           (VerbTableHeaderBuffer->RevisionId == 0xFF))) {
        TotalJackNum = VerbTableHeaderBuffer->NumberOfFrontJacks + 
                       VerbTableHeaderBuffer->NumberOfRearJacks;

        VerbTableDataBuffer = VerbTableHeaderDataAddress[Index].VerbTableData;

        for (JackIndex = 0; JackIndex < (TotalJackNum * 4); JackIndex++) {
          //
          // To send command data to Codec for all Jacks.(Front and Rear)
          //
          DeliverCommandData = (VerbTableDataBuffer[JackIndex] & CLEAN_CODEC_ADDRESS_MASK) | (CodecAddress << 28);
          Status = DeliverCommandToCodec (PeiServices, HdaBar, StallPpi, DeliverCommandData);
          if (EFI_ERROR (Status)) {
            return Status;
          }
        }
      }
    }

    Status = EFI_SUCCESS;
  }

  return Status;
}
//[-end-130410-12360005-add]//