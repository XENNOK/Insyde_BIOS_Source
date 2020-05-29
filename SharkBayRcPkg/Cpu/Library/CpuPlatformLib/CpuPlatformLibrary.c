/**
  This file contains an 'Intel Peripheral Driver' and uniquely
  identified as "Intel Reference Module" and is
  licensed for Intel CPUs and chipsets under the terms of your
  license agreement with Intel or your vendor.  This file may
  be modified by the user, subject to additional terms of the
  license agreement

@copyright
  Copyright (c) 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  CpuPlatformLib.c

@brief
  CPU Platform Lib implementation.
**/
#include "CpuPlatformLibrary.h"

/**
  Return CPU Family ID

  @retval CPU_FAMILY              CPU Family ID
**/
CPU_FAMILY
EFIAPI
GetCpuFamily (
  VOID
  )
{
  EFI_CPUID_REGISTER Cpuid;
  ///
  /// Read the CPUID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  return ((CPU_FAMILY) (Cpuid.RegEax & CPUID_FULL_FAMILY_MODEL));
}

/**
  Return Cpu stepping type

  @retval UINT8                   Cpu stepping type
**/
CPU_STEPPING
EFIAPI
GetCpuStepping (
  VOID
  )
{
  EFI_CPUID_REGISTER Cpuid;
  ///
  /// Read the CPUID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  return ((CPU_STEPPING) (Cpuid.RegEax & CPUID_FULL_STEPPING));
}

/**
  Determine if CPU is supported

  @retval TRUE                    CPU is supported
  @retval FALSE                   CPU is not supported
**/
BOOLEAN
IsCpuSupported (
  VOID
  )
{
  if (GetCpuFamily() == EnumCpuMax) {
    return FALSE;
  }
  return TRUE;
}

/**
  Return CPU Sku

  @retval UINT8              CPU Sku
**/
UINT8
EFIAPI
GetCpuSku (
  VOID
  )
{
  UINT8              CpuType;
  EFI_CPUID_REGISTER Cpuid;
  ///
  /// Read the CPUID information
  ///
  AsmCpuid (CPUID_VERSION_INFO, &Cpuid.RegEax, &Cpuid.RegEbx, &Cpuid.RegEcx, &Cpuid.RegEdx);
  switch (Cpuid.RegEax & CPUID_FULL_FAMILY_MODEL) {
    case CPUID_FULL_FAMILY_MODEL_HASWELL_ULT:
      CpuType = EnumCpuUlt;
      break;

    case CPUID_FULL_FAMILY_MODEL_HASWELL:
    case CPUID_FULL_FAMILY_MODEL_CRYSTALWELL:
      CpuType = EnumCpuTrad;
      break;

    default:
      CpuType = EnumCpuUnknown;
      DEBUG ((EFI_D_ERROR, "Unsupported CPU SKU, CpuFamilyId: 0x%08X!\n", (Cpuid.RegEax & CPUID_FULL_FAMILY_MODEL)));
      ASSERT (FALSE);
      break;
  }

  return CpuType;
}

EFI_STATUS
EFIAPI
MailboxWrite (
  IN UINT32  MailboxType,
  IN UINT32  MailboxCommand,
  IN UINT32  MailboxData,
  OUT UINT32 *MailboxStatus
  )
/**
  Generic Mailbox function for mailbox write commands. This function will
  poll the mailbox interface for control, issue the write request, poll
  for completion, and verify the write was succussful.

  @param[IN]  MailboxType,
  @param[IN]  MailboxCommand,
  @param[IN]  MailboxData,
  @param[OUT] *MailboxStatus

  @retval EFI_STATUS
**/
{
  EFI_STATUS         Status;
  UINT64             MsrData;
  UINT32             MchBar;
  OC_MAILBOX_FULL    OcMailboxFull;
  OC_MAILBOX_FULL    OcMailboxFullVerify;
  PCODE_MAILBOX_FULL PcodeMailboxFull;
  PCODE_MAILBOX_FULL PcodeMailboxFullVerify;

  ///
  ///  Poll the run/busy to ensure the interface is available
  ///
  Status = PollMailboxReady(MailboxType);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DEBUG ((EFI_D_INFO, "(MAILBOX) Mailbox Write Command = %2X\n", (UINT8)MailboxCommand));

  switch (MailboxType)
  {
    case MAILBOX_TYPE_PCODE:
      ///
      /// Copy in Mailbox data and write the PCODE mailbox DATA field
      ///
      PcodeMailboxFull.Interface.InterfaceData = MailboxCommand;
      PcodeMailboxFull.Data = MailboxData;
      MchBar = (MmioRead32 (MmPciAddress (0, 0, 0, 0, MCHBAR_OFFSET)) &~BIT0);

      MmioWrite32 ( (MchBar + PCODE_MAILBOX_DATA_OFFSET), PcodeMailboxFull.Data);

      ///
      /// Set the Run/Busy bit to signal mailbox data is ready to process
      ///
      PcodeMailboxFull.Interface.Fields.RunBusy = 1;
      MmioWrite32 ( (MchBar + PCODE_MAILBOX_INTERFACE_OFFSET), PcodeMailboxFull.Interface.InterfaceData);

      ///
      /// Poll run/busy to indicate the completion of write request
      ///
      PollMailboxReady(MailboxType);

      ///
      /// Read the BIOS PCODE mailbox to verify write completion success.
      /// Mailbox protocol requires software to read back the interface twice
      /// to ensure the read results are consistent.
      ///
      PcodeMailboxFull.Interface.InterfaceData = MmioRead32 (MchBar + PCODE_MAILBOX_INTERFACE_OFFSET);
      PcodeMailboxFull.Data = MmioRead32 (MchBar + PCODE_MAILBOX_DATA_OFFSET);

      PchPmTimerStall(MAILBOX_READ_TIMEOUT);

      ///
      /// Read twice to verify data is consitent
      ///
      PcodeMailboxFullVerify.Interface.InterfaceData = MmioRead32 (MchBar + PCODE_MAILBOX_INTERFACE_OFFSET);
      PcodeMailboxFullVerify.Data = MmioRead32 (MchBar + PCODE_MAILBOX_DATA_OFFSET);

      ///
      /// If the data is inconsistent, we cannot trust the results
      ///
      if (PcodeMailboxFull.Interface.InterfaceData != PcodeMailboxFullVerify.Interface.InterfaceData) {
        if (PcodeMailboxFull.Data  != PcodeMailboxFullVerify.Data) {
          DEBUG ((EFI_D_ERROR, "(MAILBOX) Mailbox read data is corrupted.\n"));
          return EFI_INVALID_PARAMETER;
        }
      }

      ///
      ///  Copy PCODE mailbox completion code
      ///
      *MailboxStatus = (UINT32) PcodeMailboxFull.Interface.Fields.Command;
      break;

    case MAILBOX_TYPE_OC:
      ///
      /// Set the Run/Busy bit to signal mailbox data is ready to process
      ///
      OcMailboxFull.Interface.InterfaceData = MailboxCommand;
      OcMailboxFull.Data = MailboxData;
      OcMailboxFull.Interface.Fields.RunBusy = 1;
      CopyMem (&MsrData, &OcMailboxFull, sizeof(MsrData));

      ///
      /// Write mailbox command to OC mailbox
      ///
      AsmWriteMsr64 (OC_MAILBOX_MSR, MsrData);

      ///
      /// Poll run/busy to indicate the completion of write request
      ///
      PollMailboxReady(MailboxType);

      ///
      /// Read the mailbox command from OC mailbox. Read twice to ensure data.
      ///
      MsrData = AsmReadMsr64 (OC_MAILBOX_MSR);
      CopyMem (&OcMailboxFull, &MsrData, sizeof(OcMailboxFull));

      PchPmTimerStall(MAILBOX_READ_TIMEOUT);

      MsrData = AsmReadMsr64 (OC_MAILBOX_MSR);
      CopyMem (&OcMailboxFullVerify, &MsrData, sizeof(OcMailboxFullVerify));

      ///
      /// If the data is inconsistent, we cannot trust the results
      ///
      if (OcMailboxFull.Interface.InterfaceData != OcMailboxFullVerify.Interface.InterfaceData) {
        if (OcMailboxFull.Data != OcMailboxFullVerify.Data) {
          DEBUG ((EFI_D_ERROR, "(MAILBOX) Mailbox read data is corrupted.\n"));
          return EFI_INVALID_PARAMETER;
        }
      }

      ///
      ///  Copy Overclocking mailbox completion code and read results
      ///
      *MailboxStatus = OcMailboxFull.Interface.Fields.CommandCompletion;
      break;

    default:
      DEBUG ((EFI_D_ERROR, "(MAILBOX) Unrecognized Mailbox Type.\n"));
      Status = EFI_UNSUPPORTED;
      break;
  }

  DEBUG ((EFI_D_INFO, "(MAILBOX) Mailbox Status = %2X\n", *MailboxStatus));
  return Status;
}

EFI_STATUS
EFIAPI
MailboxRead (
  IN UINT32  MailboxType,
  IN UINT32  MailboxCommand,
  OUT UINT32 *MailboxDataPtr,
  OUT UINT32 *MailboxStatus
  )
/**
  Generic Mailbox function for mailbox read commands. This function will write
  the read request, and populate the read results in the output data.

  @param[IN]  MailboxType,
  @param[IN]  MailboxCommand,
  @param[OUT] *MailboxDataPtr,
  @param[OUT] *MailboxStatus

  @retval EFI_STATUS
**/
{
  EFI_STATUS         Status;
  UINT64             MsrData;
  UINT32             MchBar;
  PCODE_MAILBOX_FULL PcodeMailboxFull;
  PCODE_MAILBOX_FULL PcodeMailboxFullVerify;
  OC_MAILBOX_FULL    OcMailboxFull;
  OC_MAILBOX_FULL    OcMailboxFullVerify;

  ///
  ///  Poll the run/busy to ensure the interface is available
  ///
  Status = PollMailboxReady(MailboxType);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  DEBUG ((EFI_D_INFO, "(MAILBOX) Mailbox Read Command = %2X\n", (UINT8)MailboxCommand));

  switch (MailboxType)
  {
    case MAILBOX_TYPE_PCODE:
      ///
      /// Write the PCODE mailbox read request.
      /// Read requests only require a write to the PCODE interface mailbox.
      /// The read results will be updated in the data mailbox.
      ///
      PcodeMailboxFull.Interface.InterfaceData = MailboxCommand;
      PcodeMailboxFull.Interface.Fields.RunBusy = 1;
      MchBar = (MmioRead32 (MmPciAddress (0, 0, 0, 0, MCHBAR_OFFSET)) &~BIT0);

      MmioWrite32 ( (MchBar + PCODE_MAILBOX_INTERFACE_OFFSET), PcodeMailboxFull.Interface.InterfaceData);

      ///
      /// Poll run/busy to indicate the completion of read request
      ///
      PollMailboxReady(MailboxType);

      ///
      /// Read the BIOS PCODE mailbox to verify read completion success.
      /// Mailbox protocol requires software to read back the interface twice
      /// to ensure the read results are consistent.
      ///
      PcodeMailboxFull.Interface.InterfaceData = MmioRead32 (MchBar + PCODE_MAILBOX_INTERFACE_OFFSET);
      PcodeMailboxFull.Data = MmioRead32 (MchBar + PCODE_MAILBOX_DATA_OFFSET);

      PchPmTimerStall(MAILBOX_READ_TIMEOUT);

      ///
      /// Read twice to verify data is consitent
      ///
      PcodeMailboxFullVerify.Interface.InterfaceData = MmioRead32 (MchBar + PCODE_MAILBOX_INTERFACE_OFFSET);
      PcodeMailboxFullVerify.Data = MmioRead32 (MchBar + PCODE_MAILBOX_DATA_OFFSET);

      ///
      /// If the data is inconsistent, we cannot trust the results
      ///
      if (PcodeMailboxFull.Interface.InterfaceData != PcodeMailboxFullVerify.Interface.InterfaceData) {
        if (PcodeMailboxFull.Data  != PcodeMailboxFullVerify.Data) {
          DEBUG ((EFI_D_ERROR, "(MAILBOX) Mailbox read data is corrupted.\n"));
          return EFI_INVALID_PARAMETER;
        }
      }

      ///
      ///  Copy PCODE mailbox completion code and read results
      ///
      *MailboxStatus = (UINT32) PcodeMailboxFull.Interface.InterfaceData;
      CopyMem(MailboxDataPtr, &PcodeMailboxFull.Data, sizeof(UINT32));
      break;

    case MAILBOX_TYPE_OC:
      ///
      /// Set the Run/Busy bit to signal mailbox data is ready to process
      ///
      OcMailboxFull.Interface.InterfaceData = MailboxCommand;
      OcMailboxFull.Data = *MailboxDataPtr;
      OcMailboxFull.Interface.Fields.RunBusy = 1;
      CopyMem (&MsrData, &OcMailboxFull, sizeof(MsrData));

      ///
      /// Write mailbox command to OC mailbox
      ///
      AsmWriteMsr64 (OC_MAILBOX_MSR, MsrData);

      ///
      /// Poll run/busy to indicate the completion of write request
      ///
      PollMailboxReady(MailboxType);

      ///
      /// Read the OC mailbox to verify read completion success.
      /// Mailbox protocol requires software to read back the interface twice
      /// to ensure the read results are consistent.
      ///
      MsrData = AsmReadMsr64 (OC_MAILBOX_MSR);
      CopyMem (&OcMailboxFull, &MsrData, sizeof(OcMailboxFull));

      PchPmTimerStall(MAILBOX_READ_TIMEOUT);

      MsrData = AsmReadMsr64 (OC_MAILBOX_MSR);
      CopyMem (&OcMailboxFullVerify, &MsrData, sizeof(OcMailboxFullVerify));

      ///
      /// If the data is inconsistent, we cannot trust the results
      ///
      if (OcMailboxFull.Interface.InterfaceData != OcMailboxFullVerify.Interface.InterfaceData) {
        if (OcMailboxFull.Data != OcMailboxFullVerify.Data) {
          DEBUG ((EFI_D_ERROR, "(MAILBOX) Mailbox read data is corrupted.\n"));
          return EFI_INVALID_PARAMETER;
        }
      }

      ///
      ///  Copy Overclocking mailbox completion code and read results
      ///
      *MailboxStatus = OcMailboxFull.Interface.Fields.CommandCompletion;
      CopyMem(MailboxDataPtr, &OcMailboxFull.Data, sizeof(UINT32));
      break;

    default:
      DEBUG ((EFI_D_ERROR, "(MAILBOX) Unrecognized Mailbox Type.\n"));
      Status = EFI_UNSUPPORTED;
      break;
  }

  DEBUG ((EFI_D_INFO, "(MAILBOX) Mailbox Status = %2X\n", *MailboxStatus));

  return Status;
}

/**
  Poll the run/busy bit of the mailbox until available or timeout expires.

  @param[IN]  MailboxType,

  @retval EFI_STATUS
**/
EFI_STATUS
EFIAPI
PollMailboxReady (
  IN UINT32 MailboxType
  )
{
  EFI_STATUS              Status;
  UINT16                  StallCount;
  UINT8                   RunBusyBit;
  UINT64                  MsrData;
  UINT32                  MchBar;
  OC_MAILBOX_FULL         OcMailboxFull;
  PCODE_MAILBOX_INTERFACE PcodeMailboxInterface;

  Status = EFI_SUCCESS;
  StallCount = 0;
  RunBusyBit = 1;

  do {
    switch (MailboxType)
    {
      case MAILBOX_TYPE_PCODE:
        ///
        /// Read the MMIO run/busy state
        ///
        MchBar = (MmioRead32 (MmPciAddress (0, 0, 0, 0, MCHBAR_OFFSET)) &~BIT0);
        PcodeMailboxInterface.InterfaceData = MmioRead32 (MchBar + PCODE_MAILBOX_INTERFACE_OFFSET);
        RunBusyBit = (UINT8) PcodeMailboxInterface.Fields.RunBusy;
        break;

      case MAILBOX_TYPE_OC:
        ///
        /// Read the OC mailbox run/busy state
        ///
        MsrData = AsmReadMsr64(OC_MAILBOX_MSR);
        CopyMem(&OcMailboxFull.Data, &MsrData, sizeof(OcMailboxFull));
        RunBusyBit = OcMailboxFull.Interface.Fields.RunBusy;
        break;
    }
    //
    // Wait for 1us
    //
    PchPmTimerStall(MAILBOX_WAIT_STALL);
    StallCount++;
  }
  while ((RunBusyBit == 1) && (StallCount < MAILBOX_WAIT_TIMEOUT));

  if ((RunBusyBit == 1) && (StallCount == MAILBOX_WAIT_TIMEOUT)) {
    DEBUG ((EFI_D_ERROR, "(MAILBOX) Mailbox interface timed out.\n"));
    Status = EFI_TIMEOUT;
  }
  return Status;
}

