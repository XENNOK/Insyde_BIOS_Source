/** @file
  Initializes the PCH Azalia codec.

@copyright
  Copyright (c) 1999 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

**/
#include "PchInit.h"

/**
  Polling the Status bit

  @param[in] StatusReg            The regsiter address to read the status
  @param[in] PollingBitMap        The bit mapping for polling
  @param[in] PollingData          The Data for polling

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_TIMEOUT             Polling the bit map time out
**/
EFI_STATUS
StatusPolling (
  IN      UINT32          StatusReg,
  IN      UINT16          PollingBitMap,
  IN      UINT16          PollingData
  )
{
  UINT32  LoopTime;

  for (LoopTime = 0; LoopTime < AZALIA_MAX_LOOP_TIME; LoopTime++) {
    if ((MmioRead16 (StatusReg) & PollingBitMap) == PollingData) {
      break;
    } else {
      PchPmTimerStall (AZALIA_WAIT_PERIOD);
    }
  }

  if (LoopTime >= AZALIA_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

/**
  Send the command to the codec via the Immediate Command mechanism is written
  to the IC register

  @param[in] HdaBar                     Base address of Intel HD Audio memory mapped configuration registers
  @param[in, out] CodecCommandData      The Codec Command to be sent to the codec
  @param[in] ReadBack                   Whether to get the response received from the codec

  @retval EFI_DEVICE_ERROR              Device status error, operation failed
  @retval EFI_SUCCESS                   The function completed successfully
**/
EFI_STATUS
SendCodecCommand (
  IN      UINT32          HdaBar,
  IN OUT  UINT32          *CodecCommandData,
  IN      BOOLEAN         ReadBack
  )
{
  EFI_STATUS  Status;

  Status = StatusPolling (HdaBar + R_HDA_IRS, (UINT16) B_HDA_IRS_ICB, (UINT16) 0);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "ICB bit is not zero before SendCodecCommand! \n"));
    return EFI_DEVICE_ERROR;
  }

  MmioWrite32 (HdaBar + R_HDA_IC, *CodecCommandData);
  MmioOr16 ((UINTN) (HdaBar + R_HDA_IRS), (UINT16) ((B_HDA_IRS_IRV | B_HDA_IRS_ICB)));

  Status = StatusPolling (HdaBar + R_HDA_IRS, (UINT16) B_HDA_IRS_ICB, (UINT16) 0);
  if (EFI_ERROR (Status)) {
    MmioAnd16 ((UINTN) (HdaBar + R_HDA_IRS), (UINT16)~(B_HDA_IRS_ICB));
    return Status;
  }

  if (ReadBack == TRUE) {
    if ((MmioRead16 (HdaBar + R_HDA_IRS) & B_HDA_IRS_IRV) != 0) {
      *CodecCommandData = MmioRead32 (HdaBar + R_HDA_IR);
    } else {
      DEBUG ((EFI_D_ERROR, "SendCodecCommand: ReadBack fail! \n"));
      return EFI_DEVICE_ERROR;
    }
  }

  return EFI_SUCCESS;
}

/**
  Set a "Send Codec Command" S3 dispatch item

  @param[in] HdaBar                     Base address of Intel HD Audio memory mapped configuration registers
  @param[in, out] CodecCommandData      The Codec Command to be sent to the codec

  @retval EFI_SUCCESS                   The function completed successfully
**/
EFI_STATUS
SendCodecCommandS3Item (
  IN      UINT32          HdaBar,
  IN OUT  UINT32          CodecCommandData
  )
{
  EFI_STATUS                                      Status;
#ifdef EFI_S3_RESUME
  STATIC EFI_PCH_S3_SUPPORT_PROTOCOL              *PchS3Support;
  STATIC EFI_PCH_S3_PARAMETER_SEND_CODEC_COMMAND  S3ParameterSendCodecCommand;
  STATIC EFI_PCH_S3_DISPATCH_ITEM                 S3DispatchItem = {
    PchS3ItemTypeSendCodecCommand,
    &S3ParameterSendCodecCommand
  };
  EFI_PHYSICAL_ADDRESS                            S3DispatchEntryPoint;

  if (!PchS3Support) {
    ///
    /// Get the PCH S3 Support Protocol
    ///
    Status = gBS->LocateProtocol (
                    &gEfiPchS3SupportProtocolGuid,
                    NULL,
                    (VOID **) &PchS3Support
                    );
    ASSERT_EFI_ERROR (Status);

    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  S3ParameterSendCodecCommand.HdaBar        = HdaBar;
  S3ParameterSendCodecCommand.CodecCmdData  = CodecCommandData;
  Status = PchS3Support->SetDispatchItem (
                          PchS3Support,
                          &S3DispatchItem,
                          &S3DispatchEntryPoint
                          );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Save the script dispatch item in the Boot Script
  ///
  SCRIPT_DISPATCH (EFI_ACPI_S3_RESUME_SCRIPT_TABLE, S3DispatchEntryPoint);
#else
  Status = EFI_SUCCESS;
#endif
  return Status;
}

/**
  Initialize the Intel High Definition Audio Codec(s) present in the system.
  For each codec, a predefined codec verb table should be programmed.
  The list contains 32-bit verbs to be sent to the corresponding codec.
  If it is not programmed, the codec uses the default verb table, which may or may not
  correspond to the platform jack information.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar address of this PCH device
  @param[in, out] AzaliaStarted   Whether Azalia is successfully started

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_INVALID_PARAMETER   Provided VerbTableData is null
**/
EFI_STATUS
DetectAndInitializeAzalia (
  IN      DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN      UINT32                              RootComplexBar,
  IN OUT  BOOLEAN                             *AzaliaStarted
  )
{
  EFI_STATUS                    Status;
  UINT32                        Index;
  UINT32                        VendorDeviceId;
  UINT32                        RevisionId;
  UINT8                         ByteReg;
  UINTN                         AzaliaBase;
  UINT8                         AzaliaSDINo;
  UINT32                        HdaBar;
  UINT32                        *VerbTable;
  UINT32                        LoopTime;
  PCH_AZALIA_VERB_TABLE_HEADER  *VerbHeaderTable;
  EFI_PHYSICAL_ADDRESS          BaseAddressBarMem;
  UINT8                         VerbTableNum;
  PCH_AZALIA_CONFIG             *AzaliaConfig;
  UINT32                        Data32And;
  UINT32                        Data32Or;
  UINT16                        Data16And;
  UINT16                        Data16Or;
  UINT8                         Data8And;
  UINT8                         Data8Or;
  UINT32                        CodecCmdData;
  UINTN                         PciD31F0RegBase;
  UINT16                        LpcDeviceId;
  UINT16                        Data16;
  UINT16                        BitMask;
  UINT16                        BitValue;
  PCH_SERIES                    PchSeries;

  PchSeries = GetPchSeries();
  AzaliaConfig = PchPlatformPolicy->AzaliaConfig;
  AzaliaBase = MmPciAddress (
                0,
                PchPlatformPolicy->BusNumber,
                PCI_DEVICE_NUMBER_PCH_AZALIA,
                PCI_FUNCTION_NUMBER_PCH_AZALIA,
                0
                );
  PciD31F0RegBase = MmPciAddress (
                      0,
                      PchPlatformPolicy->BusNumber,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC,
                      0
                      );
  LpcDeviceId = MmioRead16 (PciD31F0RegBase + R_PCH_LPC_DEVICE_ID);

  Data32And = 0xF8FFFF01;
  if ((MmioRead32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CIR2030)) & (UINT32) BIT31) != 0) {
    ///
    /// PCH BIOS Spec Rev 0.5.0 Section 9.4.2 High Definition Audio VCi Configuration
    /// For Sever
    /// Step 1
    /// Configure and enable Vcp on DMI, done on PchDmiPeim.c
    /// Step 2
    /// Assign a Vcp ID value of 2 to High Definition Audio VCi ID field of VCi Resource Control register
    /// D27:F0:Reg 120h[26:24] = 2
    /// Step 3
    /// Map Tcp to VCP. Set bit 2 of TC/VCi Map field of High Definition Audio VCi Resource Control register
    /// D27:F0:Reg 120h[7:1]
    ///
    Data32Or  = BIT25;
    Data32Or |= MmioRead32 ((UINTN) (RootComplexBar + R_PCH_RCRB_CIR2030)) & V_PCH_RCRB_V1CTL_TVM_MASK;
    MmioAndThenOr32 (
      (UINTN) (AzaliaBase + R_PCH_HDA_VCICTL),
      Data32And,  // Data to be ANDed
      Data32Or    // Data to be ORed
      );
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (AzaliaBase + R_PCH_HDA_VCICTL),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
    ///
    /// Step 4
    /// Avoid isochronous transfers to use VC1, Clear No Snoop Enable of Device Control Register
    /// D27:F0:Reg 78h[11] = 0b
    ///
    if (PchSeries == PchH) {
      MmioAnd16 (
        (UINTN) (AzaliaBase + R_PCH_HDA_DEVC),
        (UINT16) (~B_PCH_HDA_DEVC_NSNPEN)
        );
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (AzaliaBase + R_PCH_HDA_DEVC),
        1,
        (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_DEVC)
        );
    }
  }
  if ((MmioRead32 ((UINTN) (AzaliaBase + R_PCH_HDA_VCICTL)) & B_PCH_HDA_VCICTL_ID) != 0) {
    ///
    /// Step 5
    /// Clear the TC/VC0 Map field of VC0 Resource Control register
    /// D27:F0:Reg 114h[7:1] = 0
    ///
    MmioAnd32 (
      (UINTN) (AzaliaBase + R_PCH_HDA_VC0CTL),
      (UINT32) (~B_PCH_HDA_VC0CTL_TCVC0_MAP)
      );
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (AzaliaBase + R_PCH_HDA_VC0CTL),
      1,
      (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_VC0CTL)
      );
    ///
    /// Step 6
    /// For LPT-H, Set VCi Enable bit of VCi Resource Control register
    /// D27:F0:Reg 120h[31] = 1
    /// For LPT-LP, Clear VCi Enable bit of VCi Resource Control register
    /// D27:F0:Reg 120h[31] = 0
    ///
    if (PchSeries == PchH) {
      MmioOr32 ((UINTN) (AzaliaBase + R_PCH_HDA_VCICTL), (UINT32) (B_PCH_HDA_VCICTL_EN));
    }
    if (PchSeries == PchLp) {
      MmioAnd32 ((UINTN) (AzaliaBase + R_PCH_HDA_VCICTL), (UINT32)~(B_PCH_HDA_VCICTL_EN));
    }
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (AzaliaBase + R_PCH_HDA_VCICTL),
      1,
      (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_VCICTL)
      );
  }
  ///
  /// Firstly Initialize Azalia to be not started.
  ///
  *AzaliaStarted = FALSE;

  ///
  /// Allocate resource for HDBAR
  ///
  BaseAddressBarMem = 0x0FFFFFFFF;
  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateMaxAddressSearchBottomUp,
                  EfiGcdMemoryTypeMemoryMappedIo,
                  14,
                  V_PCH_HDA_HDBAR_SIZE,
                  &BaseAddressBarMem,
                  mImageHandle,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// System BIOS should ensure that the High Definition Audio HDBAR D27:F0:Reg 10-17h contains a valid address value
  /// and is enabled by setting D27:F0:Reg 04h[1].
  ///
  HdaBar = (UINT32) BaseAddressBarMem;
  MmioWrite32 (AzaliaBase + R_PCH_HDA_HDBARL, HdaBar);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (AzaliaBase + R_PCH_HDA_HDBARL),
    1,
    (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_HDBARL)
    );

  MmioWrite32 (AzaliaBase + R_PCH_HDA_HDBARU, 0);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (AzaliaBase + R_PCH_HDA_HDBARU),
    1,
    (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_HDBARU)
    );

  MmioOr16 ((UINTN) (AzaliaBase + R_PCH_HDA_COMMAND), (UINT16) B_PCH_HDA_COMMAND_MSE);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (AzaliaBase + R_PCH_HDA_COMMAND),
    1,
    (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_COMMAND)
    );

  ///
  /// PCH BIOS Spec Rev 0.5.0 Section 9.5
  /// Additional High Definition Audio Programming Steps
  ///
  if(PchSeries == PchH) {
    ///
    /// Step 1
    /// Set D27:F0:43h[4] = 1b
    ///
    Data8And = (UINT8) ~0;
    Data8Or  = BIT4;
    MmioOr8 ((UINTN) (AzaliaBase + 0x43), Data8Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (AzaliaBase + 0x43),
      &Data8Or,  // Data to be ORed
      &Data8And  // Data to be ANDed
      );
    ///
    /// Step 2
    /// Set D27:F0:C0h[17] = 1b
    ///
    Data32And = (UINT32) ~0;
    Data32Or  = BIT17;
    MmioOr32 ((UINTN) (AzaliaBase + 0xC0), Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (AzaliaBase + 0xC0),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
  }
  ///
  /// For LPT-LP, clear D27:F0:43h[6] = 0b
  ///  
  if(PchSeries == PchLp) {
    ///
    /// Step 1
    /// Set D27:F0:43h[6] = 0b
    ///
    MmioAnd8 ((UINTN) (AzaliaBase + 0x43), (UINT8) (~BIT6));
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (AzaliaBase + 0x43),
      1,
      (VOID *) (UINTN) (AzaliaBase + 0x43)
      );
  }

  ///
  /// Step 3
  /// For LPT-H, Set D27:F0:C4h[14] = 1b
  /// For LPT-LP, Set D27:F0:C4h[24] = 1b
  ///
  Data32And = (UINT32) ~0;
  Data32Or = 0;
  if (PchSeries == PchH) {
    Data32Or |= BIT14;
  }
  if (PchSeries == PchLp) {
    Data32Or |= BIT24;
  }
  MmioOr32 ((UINTN) (AzaliaBase + 0xC4), Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (AzaliaBase + 0xC4),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );

  if (PchSeries == PchH) {
    ///
    /// Step 4
    /// Set D27:F0:D0h[31] = 0b
    ///
    Data32And = ~BIT31;
    Data32Or  = (UINT32) 0;
    MmioAnd32 ((UINTN) (AzaliaBase + 0xD0), Data32And);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (AzaliaBase + 0xD0),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
  }

  if (AzaliaConfig->DS == PCH_DEVICE_DISABLE) {
    MmioAnd8 ((UINTN) (AzaliaBase + R_PCH_HDA_DCKSTS), (UINT8) (~B_PCH_HDA_DCKSTS_DS));
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint8,
      (UINTN) (AzaliaBase + R_PCH_HDA_DCKSTS),
      1,
      (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_DCKSTS)
      );
  } else if (AzaliaConfig->DA != PCH_DEVICE_DISABLE) {
    if ((MmioRead8 (AzaliaBase + R_PCH_HDA_DCKSTS) & B_PCH_HDA_DCKSTS_DM) == 0) {
      MmioOr8 ((UINTN) (AzaliaBase + R_PCH_HDA_DCKCTL), (UINT8) (B_PCH_HDA_DCKCTL_DA));
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint8,
        (UINTN) (AzaliaBase + R_PCH_HDA_DCKCTL),
        1,
        (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_DCKCTL)
        );
    }
  }

  if (PchSeries == PchLp) {
    ///
    /// @todo: Policy check to bypass for PO
    /// Set Hdabar + 0x0012h[0] to 1b
    ///
    Data16And = (UINT16)~BIT0;
    Data16Or  = (UINT16) (BIT0);
    MmioOr16 ((UINTN) (HdaBar + 0x0012), Data16Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (HdaBar + 0x0012),
      &Data16Or,  // Data to be ORed
      &Data16And  // Data to be ANDed
      );

      ///
      /// W/A: Azalia BCLK is not at full swing when operating in high voltage mode
      /// Set D27:F0:42h[2] = 1b - disabling Auto Voltage Detector.
      ///
      MmioOr8 ((UINTN)(AzaliaBase + R_PCH_HDA_AZIOBC), (UINT8)B_PCH_HDA_AZIOBC_AVDDIS);
      SCRIPT_MEM_WRITE (
         EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
         EfiBootScriptWidthUint8,
         (UINTN) (AzaliaBase + R_PCH_HDA_AZIOBC),
         1,
         (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_AZIOBC)
         );
  }
  ///
  /// PCH BIOS Spec Rev 0.5.0 Section 9.1.3 Codec Initialization Programming Sequence
  /// System BIOS should also ensure that the Controller Reset# bit of Global Control register
  /// in memory-mapped space (HDBAR+08h[0]) is set to 1 and read back as 1.
  /// Deassert the HDA controller RESET# to start up the link
  ///
  Data32And = 0xFFFFFFFF;
  Data32Or  = (UINT32) (B_HDA_GCTL_CRST);
  MmioOr32 ((UINTN) (HdaBar + R_HDA_GCTL), Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (HdaBar + R_HDA_GCTL),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );

  BitMask = (UINT16) B_HDA_GCTL_CRST;
  BitValue = (UINT16) B_HDA_GCTL_CRST;
  Status = StatusPolling (HdaBar + R_HDA_GCTL, BitMask, BitValue);
  SCRIPT_MEM_POLL (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    HdaBar + R_HDA_GCTL,
    &BitMask,
    &BitValue,
    AZALIA_WAIT_PERIOD,
    AZALIA_MAX_LOOP_TIME
    );
  ///
  /// PCH BIOS Spec Rev 0.5.0 Section 9.1.3 Codec Initialization Programming Sequence
  /// Read GCAP and write the same value back to the register once after Controller Reset# bit is set
  ///
  Data16  = MmioRead16 (HdaBar + R_HDA_GCAP);
  MmioWrite16 (HdaBar + R_HDA_GCAP, Data16);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (HdaBar + R_HDA_GCAP),
    1,
    (VOID *) (UINTN) (HdaBar + R_HDA_GCAP)
    );

  ///
  /// Clear the "State Change Status Register" STATESTS bits for
  /// each of the "SDIN Stat Change Status Flag"
  ///
  Data16 = AZALIA_MAX_SID_MASK_PCH_H;
  if (PchSeries == PchLp) {
    Data16 = AZALIA_MAX_SID_MASK_PCH_LP;
  }
  MmioOr8 ((UINTN) (HdaBar + R_HDA_STATESTS), (UINT8) (Data16));
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint8,
    (UINTN) (HdaBar + R_HDA_STATESTS),
    1,
    (VOID *) (UINTN) (HdaBar + R_HDA_STATESTS)
    );

  ///
  /// Turn off the link and poll RESET# bit until it reads back as 0 to get hardware reset report
  ///
  Data32And = (UINT32) (~B_HDA_GCTL_CRST);
  Data32Or  = (UINT32) 0;
  MmioAnd32 ((UINTN) (HdaBar + R_HDA_GCTL), Data32And);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (HdaBar + R_HDA_GCTL),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );

  BitMask = (UINT16) B_HDA_GCTL_CRST;
  BitValue = 0;
  Status = StatusPolling (HdaBar + R_HDA_GCTL, BitMask, BitValue);
  SCRIPT_MEM_POLL (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    HdaBar + R_HDA_GCTL,
    &BitMask,
    &BitValue,
    AZALIA_WAIT_PERIOD,
    AZALIA_MAX_LOOP_TIME
    );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Reset High Definition Audio (Azalia) Codec Time Out - 1! \n"));
    goto ExitInitAzalia;
  }
  ///
  /// Turn on the link and poll RESET# bit until it reads back as 1
  ///
  Data32And = 0xFFFFFFFF;
  Data32Or  = (UINT32) (B_HDA_GCTL_CRST);
  MmioOr32 ((UINTN) (HdaBar + R_HDA_GCTL), Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (HdaBar + R_HDA_GCTL),
    &Data32Or,  // Data to be ORed
    &Data32And  // Data to be ANDed
    );
  ///
  /// For some combo card that will need this delay because each codec has different latency to come out from RESET.
  /// This delay can make sure all codecs be recognized by BIOS after RESET sequence.
  /// Additional delay might be required to allow codec coming out of reset prior to subsequent operations,
  /// please contact your codec vendor for detail. When clearing this bit and setting it afterward,
  /// BIOS must ensure that minimum link timing requirements (minimum RESET# assertion time, etc.) are met..
  ///
  PchPmTimerStall (AzaliaConfig->ResetWaitTimer);
  SCRIPT_STALL (EFI_ACPI_S3_RESUME_SCRIPT_TABLE, AzaliaConfig->ResetWaitTimer);

  BitMask = (UINT16) B_HDA_GCTL_CRST;
  BitValue = (UINT16) B_HDA_GCTL_CRST;
  Status = StatusPolling (HdaBar + R_HDA_GCTL, BitMask, BitValue);
  SCRIPT_MEM_POLL (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    HdaBar + R_HDA_GCTL,
    &BitMask,
    &BitValue,
    AZALIA_WAIT_PERIOD,
    AZALIA_MAX_LOOP_TIME
    );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Reset High Definition Audio (Azalia) Codec Time Out - 2! \n"));
    goto ExitInitAzalia;
  }
  ///
  /// Read the "State Change Status Register" STATESTS bits twice to find out if any SDIN is connected
  /// to a codec.
  ///
  Data16 = AZALIA_MAX_SID_MASK_PCH_H;
  if (PchSeries == PchLp) {
    Data16 = AZALIA_MAX_SID_MASK_PCH_LP;
  }
  for (LoopTime = 0, ByteReg = 0, AzaliaSDINo = 0; LoopTime < AZALIA_MAX_LOOP_TIME; LoopTime++) {
    ByteReg = (UINT8)(MmioRead8 (HdaBar + R_HDA_STATESTS) & Data16);
    if (ByteReg != 0 && (ByteReg == AzaliaSDINo)) {
      break;
    } else {
      AzaliaSDINo = ByteReg;
    }

    PchPmTimerStall (AZALIA_WAIT_PERIOD);
  }
  ///
  /// BIT3(1000) -- SDI3
  /// BIT2(0100) -- SDI2
  /// BIT1(0010) -- SDI1
  /// BIT0(0001) -- SDI0
  ///
  if (ByteReg == 0) {
    ///
    /// No Azalia Detected
    ///
    ///
    /// Turn off the link
    ///
    DEBUG ((EFI_D_ERROR, "No Azalia device is detected.\n"));
    Data32And = (UINT32) (~B_HDA_GCTL_CRST);
    Data32Or  = (UINT32) 0;
    MmioAnd32 ((UINTN) (HdaBar + R_HDA_GCTL), Data32And);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (HdaBar + R_HDA_GCTL),
      &Data32Or,  // Data to be ORed
      &Data32And  // Data to be ANDed
      );
    Status = EFI_DEVICE_ERROR;
    goto ExitInitAzalia;
  }
  ///
  /// PME Enable for Audio controller, this bit is in the resume well
  ///
  if (AzaliaConfig->Pme == PCH_DEVICE_ENABLE) {
    MmioOr32 ((UINTN) (AzaliaBase + R_PCH_HDA_PCS), (UINT32) (B_PCH_HDA_PCS_PMEE));
#ifdef SUS_WELL_RESTORE
    ///
    /// To support RapidStart resume from G3 state, all resume well registers need to be saved
    /// into S3 Script table.
    ///
    SCRIPT_MEM_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (AzaliaBase + R_PCH_HDA_PCS),
      1,
      (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_PCS)
      );
#endif
  }

  Data16 = AZALIA_MAX_SID_NUMBER_PCH_H;
  if (PchSeries == PchLp) {
    Data16 = AZALIA_MAX_SID_NUMBER_PCH_LP;
  }
  for (AzaliaSDINo = 0; AzaliaSDINo < Data16; AzaliaSDINo++, ByteReg >>= 1) {
    if ((ByteReg & 0x1) == 0) {
      ///
      /// SDIx has no Azalia Device
      ///
      DEBUG ((EFI_D_ERROR, "SDI%d has no Azalia device.\n", AzaliaSDINo));
      continue;
    }
    ///
    /// PME Enable for each existing codec, these bits are in the resume well
    ///
    if (AzaliaConfig->Pme != PCH_DEVICE_DISABLE) {
      MmioOr16 (
        (UINTN) (HdaBar + R_HDA_WAKEEN),
        (UINT16) ((B_HDA_WAKEEN_SDI_0 << AzaliaSDINo))
        );
#ifdef SUS_WELL_RESTORE
      ///
      /// To support RapidStart resume from G3 state, all resume well registers need to be saved
      /// into S3 Script table.
      ///
      SCRIPT_MEM_WRITE (
        EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
        EfiBootScriptWidthUint16,
        (UINTN) (HdaBar + R_HDA_WAKEEN),
        1,
        (VOID *) (UINTN) (HdaBar + R_HDA_WAKEEN)
        );
#endif
    }
    ///
    /// Verb:  31~28   27  26~20                   19~0
    ///         CAd    1    NID   Verb Command and data
    ///       0/1/2
    ///
    /// Read the Vendor ID/Device ID pair from the attached codec
    ///
    VendorDeviceId  = 0x000F0000 | (AzaliaSDINo << 28);
    Status          = SendCodecCommand (HdaBar, &VendorDeviceId, TRUE);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Read the Codec Vendor ID/Device ID fail! \n"));
      goto ExitInitAzalia;
    }
    ///
    /// Read the Revision ID from the attached codec
    ///
    RevisionId  = 0x000F0002 | (AzaliaSDINo << 28);
    Status      = SendCodecCommand (HdaBar, &RevisionId, TRUE);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Read the Codec Revision ID fail! \n"));
      goto ExitInitAzalia;
    }

    RevisionId = (RevisionId >> 8) & 0xFF;

    ///
    /// Get the match codec verb table, RevID of 0xFF applies to all steppings.
    ///
    for (VerbTableNum = 0, VerbHeaderTable = NULL, VerbTable = NULL;
         VerbTableNum < AzaliaConfig->AzaliaVerbTableNum;
         VerbTableNum++) {
      if ((VendorDeviceId == AzaliaConfig->AzaliaVerbTable[VerbTableNum].VerbTableHeader.VendorDeviceId) &&
          ((AzaliaConfig->AzaliaVerbTable[VerbTableNum].VerbTableHeader.RevisionId == 0xFF) ||
          ( RevisionId == AzaliaConfig->AzaliaVerbTable[VerbTableNum].VerbTableHeader.RevisionId))) {
        VerbHeaderTable = &(AzaliaConfig->AzaliaVerbTable[VerbTableNum].VerbTableHeader);
        VerbTable       = AzaliaConfig->AzaliaVerbTable[VerbTableNum].VerbTableData;
        if (VerbTable == 0) {
            DEBUG ((EFI_D_ERROR | EFI_D_INFO, "VerbTableData of VendorID:0x%X is null.\n", VendorDeviceId));
            Status = EFI_INVALID_PARAMETER;
            goto ExitInitAzalia;
        }
        DEBUG ((EFI_D_INFO, "Detected Azalia Codec with verb table, VendorID = 0x%X", VendorDeviceId));
        DEBUG ((EFI_D_INFO, " on SDI%d, revision = 0x%0x.\n", AzaliaSDINo, RevisionId));
        ///
        /// Send the entire list of verbs in the matching verb table one by one to the codec
        ///
        for (Index = 0;
             Index < (UINT32) ((VerbHeaderTable->NumberOfFrontJacks + VerbHeaderTable->NumberOfRearJacks) * 4);
             Index++) {
          ///
          /// Clear CAd Field
          ///
          CodecCmdData  = VerbTable[Index] & (UINT32) ~(BIT31 | BIT30 | BIT29 | BIT28);
          ///
          /// Program CAd Field per the SDI number got during codec detection
          ///
          CodecCmdData  |= (UINT32) (AzaliaSDINo << 28);
          Status        = SendCodecCommand (HdaBar, &CodecCmdData, FALSE);
          if (EFI_ERROR (Status)) {
            ///
            /// Skip the Azalia verb table loading when find the verb table content is not
            /// properly matched with the HDA hardware, though IDs match.
            ///
            DEBUG (
              (EFI_D_ERROR | EFI_D_INFO,
              "Detected Azalia Codec of VendorID:0x%X, error occurs during loading verb table.\n",
              VendorDeviceId)
              );
            goto ExitInitAzalia;
          }
          SendCodecCommandS3Item (HdaBar, CodecCmdData);
        }
        break;
      }
    }

    if (VerbTableNum >= AzaliaConfig->AzaliaVerbTableNum) {
      DEBUG (
        (EFI_D_ERROR,
        "Detected High Definition Audio (Azalia) Codec, VendorID = 0x%08x on SDI%d,",
        VendorDeviceId,
        AzaliaSDINo)
        );
      DEBUG ((EFI_D_ERROR, " but no matching verb table found.\n"));
    }
  }
  ///
  /// end of for
  ///
  *AzaliaStarted  = TRUE;
  Status          = EFI_SUCCESS;

ExitInitAzalia:
  ///
  /// Clear AZBAR and disable memory map access
  ///
  MmioAnd16 ((UINTN) (AzaliaBase + R_PCH_HDA_COMMAND), (UINT16) (~B_PCH_HDA_COMMAND_MSE));
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (AzaliaBase + R_PCH_HDA_COMMAND),
    1,
    (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_COMMAND)
    );

  MmioWrite32 (AzaliaBase + R_PCH_HDA_HDBARL, 0);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (AzaliaBase + R_PCH_HDA_HDBARL),
    1,
    (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_HDBARL)
    );

  MmioWrite32 (AzaliaBase + R_PCH_HDA_HDBARU, 0);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (AzaliaBase + R_PCH_HDA_HDBARU),
    1,
    (VOID *) (UINTN) (AzaliaBase + R_PCH_HDA_HDBARU)
    );

  gDS->FreeMemorySpace (
        BaseAddressBarMem,
        V_PCH_HDA_HDBAR_SIZE
        );

  return Status;
}

/**
  Detect and initialize the type of codec (AC'97 and HDA) present in the system.

  @param[in] PchPlatformPolicy    The PCH Platform Policy protocol instance
  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in, out] AzaliaEnable    Returned with TRUE if Azalia High Definition Audio codec
                                  is detected and initialized.

  @retval EFI_SUCCESS            Codec is detected and initialized.
  @retval EFI_OUT_OF_RESOURCES   Failed to allocate resources to initialize the codec.
**/
EFI_STATUS
ConfigureAzalia (
  IN      DXE_PCH_PLATFORM_POLICY_PROTOCOL    *PchPlatformPolicy,
  IN      UINT32                              RootComplexBar,
  IN OUT  BOOLEAN                             *AzaliaEnable
  )
{
  EFI_STATUS  Status;

  DEBUG ((EFI_D_INFO, "ConfigureAzalia() Start\n"));

  *AzaliaEnable = FALSE;

  ///
  /// If all codec devices are to be disabled, skip the detection code
  ///
  if (PchPlatformPolicy->DeviceEnabling->Azalia == PCH_DEVICE_DISABLE) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Skip Azalia Codec detection.\n"));
    return EFI_SUCCESS;
  }

  Status = DetectAndInitializeAzalia (PchPlatformPolicy, RootComplexBar, AzaliaEnable);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR | EFI_D_INFO, "Azalia detection / initialization failure!\n"));

    if (PchPlatformPolicy->DeviceEnabling->Azalia == PCH_DEVICE_ENABLE) {
      *AzaliaEnable = TRUE;
    }
  }

  DEBUG ((EFI_D_INFO, "ConfigureAzalia() End\n"));
  return EFI_SUCCESS;
}
