/** @file
  This is the driver that initializes the CPU Audio device.

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

#include "AudioInit.h"

extern SYSTEM_AGENT_GLOBAL_NVS_AREA_PROTOCOL  mSaGlobalNvsAreaProtocol;

///
/// Global variables
///
UINT32            *mVerbCodecSaveAddress = NULL;
UINT8             mVerbCount = 0;

UINT32            mSaHdaVerbTableDataDefault[] = {
  ///
  /// Audio Verb Table - 0x80862807
  ///
  ///
  /// Pin Widget 5 - PORT B
  ///
  0x00571C10,
  0x00571D00,
  0x00571E56,
  0x00571F18,
  ///
  /// Pin Widget 6 - PORT C
  ///
  0x00671C20,
  0x00671D00,
  0x00671E56,
  0x00671F18,
  ///
  /// Pin Widget 7 - PORT D
  ///
  0x00771C30,
  0x00771D00,
  0x00771E56,
  0x00771F18
};

SA_HDA_VERB_TABLE mSaHdaVerbTableDefault[] = {
  {
    ///
    ///  VerbTable:
    ///  Revision ID = 0xFF
    ///  Codec Vendor: 0x80862807
    ///
    {
      0x80862807, ///< Vendor ID/Device ID
      0x0000,     ///< SubSystem ID
      0xFF,       ///< Revision ID
      0x02,       ///< Front panel support (1=yes, 2=no)
      0x0003,     ///< Number of Rear Jacks
      0x0000      ///< Number of Front Jacks
    },
    0             ///< Pointer to verb table data, need to be inited in the code.
  }
};

/**
  Polling the Status bit

  @param[in] StatusReg               The regsiter address to read the status
  @param[in] PollingBitMap           The bit mapping for polling
  @param[in] PollingData             The Data for polling

  @retval EFI_SUCCESS                The function completed successfully
  @retval EFI_TIMEOUT                Polling the bit map time out
**/
EFI_STATUS
StatusPolling (
  IN      UINT32          StatusReg,
  IN      UINT16          PollingBitMap,
  IN      UINT16          PollingData
  )
{
  UINT32  LoopTime;

  for (LoopTime = 0; LoopTime < SA_HDA_MAX_LOOP_TIME; LoopTime++) {
    if ((MmioRead16 (StatusReg) & PollingBitMap) == PollingData) {
      break;
    } else {
      PchPmTimerStall (SA_HDA_WAIT_PERIOD);
    }
  }

  if (LoopTime >= SA_HDA_MAX_LOOP_TIME) {
    DEBUG ((EFI_D_ERROR, "Polling StatusReg 0x%X BitMap 0x%x TimeOut\n", StatusReg, PollingBitMap));
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

/**
  Send the command to the codec via the Immediate Command mechanism is written
  to the IC register

  @param[in] HdaBar                  Base address of Intel HD Audio memory mapped configuration registers
  @param[in, out] CodecCommandData   The Codec Command to be sent to the codec
  @param[in] ReadBack                Whether to get the response received from the codec

  @exception EFI_DEVICE_ERROR        Device status error, operation failed
  @retval EFI_SUCCESS                The function completed successfully
**/
EFI_STATUS
SendCodecCommand (
  IN      UINT32          HdaBar,
  IN OUT  UINT32          *CodecCommandData,
  IN      BOOLEAN         ReadBack
  )
{
  EFI_STATUS  Status;

  ///
  /// Wait for Command Busy (ICB) bit to be cleared
  ///
  Status = StatusPolling (HdaBar + R_HDA_IRS, (UINT16) B_HDA_IRS_ICB, (UINT16) 0);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "ICB bit is not zero before SendCodecCommand! \n"));
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Clear Result Valid (IRV) bit (by writing a one to it) before issuing a new command
  ///
  MmioOr16 ((UINTN) (HdaBar + R_HDA_IRS), (UINT16) (B_HDA_IRS_IRV));
  ///
  /// Send command to codec
  ///
  MmioWrite32 (HdaBar + R_HDA_IC, *CodecCommandData);
  ///
  /// Set ICB bit to issue the command currently stored in IC to the codec. 
  ///
  MmioOr16 ((UINTN) (HdaBar + R_HDA_IRS), (UINT16) (B_HDA_IRS_ICB));
  ///
  /// Wait for Command Busy (ICB) bit to be cleared
  ///
  Status = StatusPolling (HdaBar + R_HDA_IRS, (UINT16) B_HDA_IRS_ICB, (UINT16) 0);
  if (EFI_ERROR (Status)) {
    MmioAnd16 ((UINTN) (HdaBar + R_HDA_IRS), (UINT16)~(B_HDA_IRS_ICB));
    return Status;
  }

  ///
  /// Save Codec command for runtime code
  ///
  if (mVerbCodecSaveAddress != NULL) {
    if ((mVerbCount * 4) < AUDIO_CODEC_MAX_SIZE) {
      *(mVerbCodecSaveAddress + mVerbCount) = *CodecCommandData;
      mVerbCount++;
      DEBUG ((EFI_D_ERROR, "SendCodecCommand(0x%x): 0x%x \n",(mVerbCodecSaveAddress + (mVerbCount)), *CodecCommandData));
    } else {
      DEBUG ((EFI_D_ERROR, "Codec Command Save Area Overflow max value of %d\n", AUDIO_CODEC_MAX_SIZE));
    }
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

  @param[in] HdaBar                  Base address of Intel HD Audio memory mapped configuration registers

  @retval EFI_SUCCESS                The function completed successfully
**/
EFI_STATUS
SendCodecCommandS3Item (
  IN      UINT32          HdaBar
  )
{
  UINT16  BitMask;
  UINT16  BitValue;
  UINT16  Data16And;
  UINT16  Data16Or;

  ///
  /// Wait for Command Busy (ICB) bit to be cleared
  ///
  BitMask   = (UINT16) B_HDA_IRS_ICB;
  BitValue  = (UINT16) 0;
  SCRIPT_MEM_POLL (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    HdaBar + R_HDA_IRS,
    &BitMask,
    &BitValue,
    SA_HDA_WAIT_PERIOD,
    SA_HDA_MAX_LOOP_TIME
    );
  ///
  /// Clear Result Valid (IRV) bit (by writing a one to it) before issuing a new command
  ///
    Data16And = 0xFFFF;
    Data16Or  = (UINT16) (B_HDA_IRS_IRV);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint16,
      (UINTN) (HdaBar + R_HDA_IRS),
      &Data16Or,  /// Data to be ORed
      &Data16And  /// Data to be ANDed
      );

  ///
  /// Send command to codec
  ///
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (HdaBar + R_HDA_IC),
    1,
    (VOID *) (UINTN) (HdaBar + R_HDA_IC)
    );

  ///
  /// Set ICB bit to issue the command currently stored in IC to the codec. 
  ///
  Data16And = 0xFFFF;
  Data16Or  = (UINT16) (B_HDA_IRS_ICB );
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (HdaBar + R_HDA_IRS),
    &Data16Or,  /// Data to be ORed
    &Data16And  /// Data to be ANDed
    );

  ///
  /// Wait for Command Busy (ICB) bit to be cleared
  ///
  SCRIPT_MEM_POLL (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    HdaBar + R_HDA_IRS,
    &BitMask,
    &BitValue,
    SA_HDA_WAIT_PERIOD,
    SA_HDA_MAX_LOOP_TIME
    );

  return EFI_SUCCESS;
}

/**
  Detect And Initialize SA Audio Codec

  @param[in] ImageHandle             Handle for the image of this driver
  @param[in] DxePlatformSaPolicy     SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS - Legacy Region protocol installed
  @retval Other       - No protocol installed, unload driver.
**/
EFI_STATUS
DetectAndInitializeCodec (
  IN EFI_HANDLE                      mImageHandle,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
{
  EFI_STATUS                Status;
  UINT32                    Index;
  UINT32                    VendorDeviceId;
  UINT32                    RevisionId;
  UINT8                     ByteReg;
  UINTN                     AudioBase;
  UINT8                     AudioSDINo;
  UINT32                    HdaBar;
  UINT32                    *VerbTable;
  UINT32                    LoopTime;
  SA_HDA_VERB_TABLE_HEADER  *VerbHeaderTable;
  EFI_PHYSICAL_ADDRESS      BaseAddressBarMem;
  UINT8                     VerbTableNum;
  UINT32                    Data32And;
  UINT32                    Data32Or;
  UINT32                    Data32;
  UINT32                    CodecCmdData;
  UINT16                    AudioDeviceId;
  UINT16                    Data16;
  UINT16                    BitMask;
  UINT16                    BitValue;
  CPU_STEPPING              CpuSteppingId;
  CPU_FAMILY                CpuFamilyId;
  UINT16                    IsUlx;

  CpuFamilyId   = GetCpuFamily();
  CpuSteppingId = GetCpuStepping();

  AudioBase     = MmPciAddress (0, SA_HDA_BUS_NUM, SA_HDA_DEV_NUM, SA_HDA_FUN_NUM, 0);
  AudioDeviceId = MmioRead16 (AudioBase + PCI_DEVICE_ID_OFFSET);

  ///
  /// Allocate resource for HDBAR
  ///
  BaseAddressBarMem = 0x0FFFFFFFF;
  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateMaxAddressSearchBottomUp,
                  EfiGcdMemoryTypeMemoryMappedIo,
                  14,
                  SA_HDA_HDBAR_SIZE,
                  &BaseAddressBarMem,
                  mImageHandle,
                  NULL
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// System BIOS should ensure that the High Definition Audio HDBAR D27:F0:Reg 10-17h contains a valid address value
  /// and is enabled by setting D3:F0:Reg 04h[1].
  ///
  HdaBar = (UINT32) BaseAddressBarMem;
  MmioWrite32 (AudioBase + SA_HDA_HDBARL, HdaBar);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (AudioBase + SA_HDA_HDBARL),
    1,
    (VOID *) (UINTN) (AudioBase + SA_HDA_HDBARL)
    );

  MmioWrite32 (AudioBase + SA_HDA_HDBARU, 0);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (AudioBase + SA_HDA_HDBARU),
    1,
    (VOID *) (UINTN) (AudioBase + SA_HDA_HDBARU)
    );

  MmioOr16 ((UINTN) (AudioBase + PCI_COMMAND_OFFSET), (UINT16) BIT1);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (AudioBase + PCI_COMMAND_OFFSET),
    1,
    (VOID *) (UINTN) (AudioBase + PCI_COMMAND_OFFSET)
    );

  ///
  /// AudioWA: Apply until C0, program Chicken bit: set Dev3 mmio 101C bit 29
  ///
  if (((CpuFamilyId == EnumCpuHsw) && (CpuSteppingId < EnumHswC0)) ||
      ((CpuFamilyId == EnumCpuCrw) && (CpuSteppingId < EnumCrwC0))) {
    Data32And = 0xFFFFFFFF;
    Data32Or  = (UINT32) (BIT29);
    MmioOr32 ((UINTN) (HdaBar + 0x101C), Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (HdaBar + 0x101C),
      &Data32Or,  /// Data to be ORed
      &Data32And  /// Data to be ANDed
      );
  }
  if (DxePlatformSaPolicy->Revision >=4) {
    IsUlx = 0;
    Data16 = McD2PciCfg16 (0x2);
    if ((Data16 == 0xA0E) || (Data16 == 0xA1E)) {
      IsUlx = 1;
    }
    Data32Or = 4;
    Data32 = 75;
    if (DxePlatformSaPolicy->IgdConfig->CdClkVar != 0) {
      if (DxePlatformSaPolicy->IgdConfig->CdClk == 0) {
        Data32Or = 16;
        Data32 = 225;
      }
      if (DxePlatformSaPolicy->IgdConfig->CdClk == 2) {
        Data32Or = 4;
        Data32 = 90;
      }
    }
    if(IsUlx == 1) {
      Data32Or = 16;
      Data32 = 225;
    }
    ///
    /// Program Dev3 EM4 and EM5 MMIO registers [17:00] based on Cd Clk frequency
    ///
    Data32And = 0xFFFC0000;
    Mmio32AndThenOr ((UINTN) HdaBar, 0x100c, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (HdaBar + 0x100c),
      &Data32Or,  /// Data to be ORed
      &Data32And  /// Data to be ANDed
      );

    Data32And = 0xFFFC0000;
    Data32Or  = Data32;
    Mmio32AndThenOr ((UINTN) HdaBar, 0x1010, Data32And, Data32Or);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (HdaBar + 0x1010),
      &Data32Or,  /// Data to be ORed
      &Data32And  /// Data to be ANDed
      );
  }

  mSaGlobalNvsAreaProtocol.Area->AudioWaA = MmioRead32(HdaBar + 0x1010);
  mSaGlobalNvsAreaProtocol.Area->AudioWaB = MmioRead32(HdaBar + 0x101C);
  mSaGlobalNvsAreaProtocol.Area->AudioWaC = MmioRead32(HdaBar + 0x100C);

  ///
  /// Allocate an ACPI NVS memory buffer for Saving Codec value for Adapter Power on restore
  /// , zero initialize, and set the pointer in the SA NVS area structure.
  ///
  Status = (gBS->AllocatePool) (EfiACPIMemoryNVS, AUDIO_CODEC_MAX_SIZE, (VOID **) &mVerbCodecSaveAddress);
  if (EFI_ERROR(Status)) {
	mSaGlobalNvsAreaProtocol.Area->AudioCodecSaveAddress = 0;
    ASSERT_EFI_ERROR (Status);  
  } else {
	ZeroMem ((VOID *) mVerbCodecSaveAddress, AUDIO_CODEC_MAX_SIZE);
    mSaGlobalNvsAreaProtocol.Area->AudioCodecSaveAddress = (UINT32) (UINTN) (mVerbCodecSaveAddress);
  }
  
  ///
  /// Codec Initialization Programming Sequence
  /// System BIOS should also ensure that the Controller Reset# bit of Global Control register
  /// in memory-mapped space (HDBAR+08h[0]) is set to 1 and read back as 1.
  /// Deassert the HDA controller RESET# to start up the link
  ///
  DEBUG ((EFI_D_INFO, "Codec Initialization...\n"));
  Data32And = 0xFFFFFFFF;
  Data32Or  = (UINT32) (B_HDA_GCTL_CRST);
  MmioOr32 ((UINTN) (HdaBar + R_HDA_GCTL), Data32Or);
  SCRIPT_MEM_READ_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (HdaBar + R_HDA_GCTL),
    &Data32Or,  /// Data to be ORed
    &Data32And  /// Data to be ANDed
    );

  BitMask   = (UINT16) B_HDA_GCTL_CRST;
  BitValue  = (UINT16) B_HDA_GCTL_CRST;
  Status    = StatusPolling (HdaBar + R_HDA_GCTL, BitMask, BitValue);
  SCRIPT_MEM_POLL (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    HdaBar + R_HDA_GCTL,
    &BitMask,
    &BitValue,
    SA_HDA_WAIT_PERIOD,
    SA_HDA_MAX_LOOP_TIME
    );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Reset SA dHDA Codec - Set Controller Reset# bit and Poll: Time Out - 0! \n"));
    goto ExitInitAudio;
  }
  ///
  /// Read GCAP and write the same value back to the register once after Controller Reset# bit is set
  ///
  Data16 = MmioRead16 (HdaBar + R_HDA_GCAP);
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
  MmioOr8 ((UINTN) (HdaBar + R_HDA_STATESTS), (UINT8) (SA_HDA_MAX_SID_MASK));
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
    &Data32Or,  /// Data to be ORed
    &Data32And  /// Data to be ANDed
    );

  BitMask   = (UINT16) B_HDA_GCTL_CRST;
  BitValue  = 0;
  Status    = StatusPolling (HdaBar + R_HDA_GCTL, BitMask, BitValue);
  SCRIPT_MEM_POLL (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    HdaBar + R_HDA_GCTL,
    &BitMask,
    &BitValue,
    SA_HDA_WAIT_PERIOD,
    SA_HDA_MAX_LOOP_TIME
    );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Reset SA dHDA Codec - Turn off the link and Poll: Time Out - 1! \n"));
    goto ExitInitAudio;
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
    &Data32Or,  /// Data to be ORed
    &Data32And  /// Data to be ANDed
    );
  ///
  /// For some combo card that will need this delay because each codec has different latency to come out from RESET.
  /// This delay can make sure all codecs be recognized by BIOS after RESET sequence.
  /// Additional delay might be required to allow codec coming out of reset prior to subsequent operations,
  /// please contact your codec vendor for detail. When clearing this bit and setting it afterward,
  /// BIOS must ensure that minimum link timing requirements (minimum RESET# assertion time, etc.) are met..
  ///
  PchPmTimerStall (SA_HDA_WAIT_PERIOD);
  SCRIPT_STALL (EFI_ACPI_S3_RESUME_SCRIPT_TABLE, 300 * STALL_ONE_MICRO_SECOND);

  BitMask   = (UINT16) B_HDA_GCTL_CRST;
  BitValue  = (UINT16) B_HDA_GCTL_CRST;
  Status    = StatusPolling (HdaBar + R_HDA_GCTL, BitMask, BitValue);
  SCRIPT_MEM_POLL (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    HdaBar + R_HDA_GCTL,
    &BitMask,
    &BitValue,
    SA_HDA_WAIT_PERIOD,
    SA_HDA_MAX_LOOP_TIME
    );

  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Reset SA dHDA Codec - Turn on the link and Poll: Time Out - 2! \n"));
    goto ExitInitAudio;
  }
  ///
  /// Read the "State Change Status Register" STATESTS bits twice to find out if any SDIN is connected
  /// to a codec.
  ///
  for (LoopTime = 0, ByteReg = 0, AudioSDINo = 0; LoopTime < SA_HDA_MAX_LOOP_TIME; LoopTime++) {
    ByteReg = MmioRead8 (HdaBar + R_HDA_STATESTS) & SA_HDA_MAX_SID_MASK;
    if (ByteReg != 0 && (ByteReg == AudioSDINo)) {
      break;
    } else {
      AudioSDINo = ByteReg;
    }

    PchPmTimerStall (SA_HDA_WAIT_PERIOD);
  }
  ///
  /// BIT3(1000) -- SDI3
  /// BIT2(0100) -- SDI2
  /// BIT1(0010) -- SDI1
  /// BIT0(0001) -- SDI0
  ///
  if (ByteReg == 0) {
    ///
    /// No Codec Detected, Turn off the link
    ///
    DEBUG ((EFI_D_INFO, "No Codec device is detected.\n"));
    Data32And = (UINT32) (~B_HDA_GCTL_CRST);
    Data32Or  = (UINT32) 0;
    MmioAnd32 ((UINTN) (HdaBar + R_HDA_GCTL), Data32And);
    SCRIPT_MEM_READ_WRITE (
      EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
      EfiBootScriptWidthUint32,
      (UINTN) (HdaBar + R_HDA_GCTL),
      &Data32Or,  /// Data to be ORed
      &Data32And  /// Data to be ANDed
      );
    Status = EFI_DEVICE_ERROR;
    goto ExitInitAudio;
  }

  for (AudioSDINo = 0; AudioSDINo < SA_HDA_MAX_SID_NUMBER; AudioSDINo++, ByteReg >>= 1) {
    if ((ByteReg & 0x1) == 0) {
      ///
      /// SDIx has no Codec Device
      ///
      DEBUG ((EFI_D_INFO, "SDI%d has no Codec device.\n", AudioSDINo));
      continue;
    }
    ///
    /// Verb:  31~28   27  26~20                   19~0
    ///         CAd    1    NID   Verb Command and data
    ///       0/1/2
    ///
    /// Read the Vendor ID/Device ID pair from the attached codec
    ///
    VendorDeviceId  = 0x000F0000 | (AudioSDINo << 28);
    Status          = SendCodecCommand (HdaBar, &VendorDeviceId, TRUE);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Read the Codec Vendor ID/Device ID fail! \n"));
      goto ExitInitAudio;
    }
    ///
    /// Read the Revision ID from the attached codec
    ///
    RevisionId  = 0x000F0002 | (AudioSDINo << 28);
    Status      = SendCodecCommand (HdaBar, &RevisionId, TRUE);
    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_ERROR, "Read the Codec Revision ID fail! \n"));
      goto ExitInitAudio;
    }

    RevisionId = (RevisionId >> 8) & 0xFF;

    ///
    /// Get the match codec verb table, RevID of 0xFF applies to all steppings.
    ///
    if (DxePlatformSaPolicy->MiscConfig->SaHdaVerbTableNum == 0) {
      ///
      /// Init Verb Table Data
      ///
      DxePlatformSaPolicy->MiscConfig->SaHdaVerbTable[0].VerbTableData = mSaHdaVerbTableDataDefault;
      DxePlatformSaPolicy->MiscConfig->SaHdaVerbTableNum   = sizeof (mSaHdaVerbTableDefault) / sizeof (SA_HDA_VERB_TABLE);
    }
    for (VerbTableNum = 0, VerbHeaderTable = NULL, VerbTable = NULL;
         VerbTableNum < DxePlatformSaPolicy->MiscConfig->SaHdaVerbTableNum;
         VerbTableNum++
        ) {
      if ((VendorDeviceId == DxePlatformSaPolicy->MiscConfig->SaHdaVerbTable[VerbTableNum].VerbTableHeader.VendorDeviceId) &&
          (
            (DxePlatformSaPolicy->MiscConfig->SaHdaVerbTable[VerbTableNum].VerbTableHeader.RevisionId == 0xFF) ||
          (RevisionId == DxePlatformSaPolicy->MiscConfig->SaHdaVerbTable[VerbTableNum].VerbTableHeader.RevisionId)
        )
          ) {
        VerbHeaderTable = &(DxePlatformSaPolicy->MiscConfig->SaHdaVerbTable[VerbTableNum].VerbTableHeader);
        VerbTable       = DxePlatformSaPolicy->MiscConfig->SaHdaVerbTable[VerbTableNum].VerbTableData;
        if (VerbTable == 0) {
          DEBUG ((EFI_D_ERROR, "VerbTableData of VendorID:0x%X is null.\n", VendorDeviceId));
          Status = EFI_INVALID_PARAMETER;
          goto ExitInitAudio;
        }

        DEBUG ((EFI_D_INFO, "Detected SA HDA Codec with verb table, VendorID = 0x%X", VendorDeviceId));
        DEBUG ((EFI_D_INFO, " on SDI%d, revision = 0x%0x.\n", AudioSDINo, RevisionId));
        ///
        /// Enable 3rd Pin and Converter Widget
        /// Clear CAd Field
        ///
        CodecCmdData = SA_HDA_ENABLE_3RD_PIN_WIDGET;
        ///
        /// Program CAd Field per the SDI number got during codec detection
        ///
        CodecCmdData |= (UINT32) (AudioSDINo << 28);
        Status = SendCodecCommand (HdaBar, &CodecCmdData, FALSE);
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "Error occurs at loading Verb 0x00878101"));
          ///
          /// Skip the verb table loading when find the verb table content is not
          /// properly matched with the HDA hardware, though IDs match.
          ///
          DEBUG (
            (EFI_D_ERROR,
            "Detected Codec of VendorID:0x%X, error occurs during loading verb table.\n",
            VendorDeviceId)
            );
          goto ExitInitAudio;
        }

        SendCodecCommandS3Item (HdaBar);
        ///
        /// Send the entire list of verbs in the matching verb table one by one to the codec
        ///
        for (Index = 0; Index < (UINT32) ((VerbHeaderTable->NumberOfFrontJacks + VerbHeaderTable->NumberOfRearJacks) * 4); Index++) {
          ///
          /// Clear CAd Field
          ///
          CodecCmdData = VerbTable[Index] & (UINT32)~(BIT31 | BIT30 | BIT29 | BIT28);
          ///
          /// Program CAd Field per the SDI number got during codec detection
          ///
          CodecCmdData |= (UINT32) (AudioSDINo << 28);
          Status = SendCodecCommand (HdaBar, &CodecCmdData, FALSE);
          if (EFI_ERROR (Status)) {
            DEBUG ((EFI_D_ERROR, "Error occurs at loading Command Index:%x\n", Index));
            ///
            /// Skip the verb table loading when find the verb table content is not
            /// properly matched with the HDA hardware, though IDs match.
            ///
            DEBUG (
              (EFI_D_ERROR,
              "Detected Codec of VendorID:0x%X, error occurs during loading verb table.\n",
              VendorDeviceId)
              );
            goto ExitInitAudio;
          }

          SendCodecCommandS3Item (HdaBar);
        }
        ///
        /// Disable 3rd Pin and Converter Widget
        /// Clear CAd Field
        ///
        CodecCmdData = SA_HDA_DISABLE_3RD_PIN_WIDGET;
        ///
        /// Program CAd Field per the SDI number got during codec detection
        ///
        CodecCmdData |= (UINT32) (AudioSDINo << 28);
        Status = SendCodecCommand (HdaBar, &CodecCmdData, FALSE);
        if (EFI_ERROR (Status)) {
          DEBUG ((EFI_D_ERROR, "Error occurs at loading verb 0x00878100"));
          ///
          /// Skip the verb table loading when find the verb table content is not
          /// properly matched with the HDA hardware, though IDs match.
          ///
          DEBUG (
            (EFI_D_ERROR,
            "Detected Codec of VendorID:0x%X, error occurs during loading verb table.\n",
            VendorDeviceId)
            );
          goto ExitInitAudio;
        }

        SendCodecCommandS3Item (HdaBar);

        DEBUG ((EFI_D_INFO, "Verb Table loading complete to Codec on SDI%d\n", AudioSDINo));
        break;
      }
    }

    if (VerbTableNum >= DxePlatformSaPolicy->MiscConfig->SaHdaVerbTableNum) {
      DEBUG (
        (EFI_D_INFO,
        "Detected SA High Definition Audio Codec, VendorID = 0x%08x on SDI%d,",
        VendorDeviceId,
        AudioSDINo)
        );
      DEBUG ((EFI_D_INFO, " but no matching verb table found.\n"));
    }
  } // end of for
  Status = EFI_SUCCESS;

ExitInitAudio:
  ///
  /// Clear HdaBar and disable memory map access
  ///
  MmioAnd16 ((UINTN) (AudioBase + PCI_COMMAND_OFFSET), (UINT16) (~BIT1));
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint16,
    (UINTN) (AudioBase + PCI_COMMAND_OFFSET),
    1,
    (VOID *) (UINTN) (AudioBase + PCI_COMMAND_OFFSET)
    );

  MmioWrite32 (AudioBase + SA_HDA_HDBARL, 0);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (AudioBase + SA_HDA_HDBARL),
    1,
    (VOID *) (UINTN) (AudioBase + SA_HDA_HDBARL)
    );

  MmioWrite32 (AudioBase + SA_HDA_HDBARU, 0);
  SCRIPT_MEM_WRITE (
    EFI_ACPI_S3_RESUME_SCRIPT_TABLE,
    EfiBootScriptWidthUint32,
    (UINTN) (AudioBase + SA_HDA_HDBARU),
    1,
    (VOID *) (UINTN) (AudioBase + SA_HDA_HDBARU)
    );

  gDS->FreeMemorySpace (
        BaseAddressBarMem,
        SA_HDA_HDBAR_SIZE
        );
  ///
  /// Save the final count of Audio Codec Save data in SA NVS
  ///
  mSaGlobalNvsAreaProtocol.Area->AudioCodecSaveCount = mVerbCount;

  return Status;
}

/**
  Initialize SystemAgent Audio Device/Codec.

  @param[in] ImageHandle             Handle for the image of this driver
  @param[in] DxePlatformSaPolicy     SA DxePlatformPolicy protocol

  @retval EFI_SUCCESS - Audio/Codec initialization done
**/
EFI_STATUS
AudioInit (
  IN EFI_HANDLE                      ImageHandle,
  IN DXE_PLATFORM_SA_POLICY_PROTOCOL *DxePlatformSaPolicy
  )
{
  EFI_STATUS  Status;

  if ((DxePlatformSaPolicy->MiscConfig->AudioEnable == 0) || (McD2PciCfg16 (R_SA_IGD_VID) == 0xFFFF)) {
    DEBUG ((
      EFI_D_INFO, "Skip Audio Initialization when BIOS option set to disable Or iGfx is not enabled.\n"
      )
      );
    return EFI_SUCCESS;
  }

  Status = DetectAndInitializeCodec (ImageHandle, DxePlatformSaPolicy);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "SA Audio Codec initialization failure!\n"));
  }

  DEBUG ((EFI_D_INFO, "AudioInit() End\n"));
  return EFI_SUCCESS;
}
