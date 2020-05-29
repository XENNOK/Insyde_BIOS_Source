/** @file

  SPS Me Chipset Lib implementation.
  Implementation file for SPS ME Chipset functionality
  Note: Only for SPS.

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


#include <Uefi.h>

#include <SpsMe.h>

#include <Library/S3BootScriptLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/SpsMeChipsetLib.h>

#include <SpsMeAccess.h>
#include <SpsHeciRegs.h>

#include <SpsChipset.h>

/**
  Enable/Disable Me devices

  @param[in] WhichDevice          Select of Me device
  @param[in] DeviceFuncCtrl       Function control

  @retval None
**/
VOID
SpsMeDeviceControl (
  IN SPS_ME_DEVICE                     WhichDevice,
  IN SPS_ME_DEVICE_FUNC_CTRL           DeviceFuncCtrl
  )
{
  switch (WhichDevice) {
    case SpsHECI1:
    case SpsHECI2:
    case SpsIDER:
    case SpsSOL:
      if (DeviceFuncCtrl) {
        SpsRcrbAnd16 (R_SPS_RCRB_FD2, ((UINT16)~(BIT0 << WhichDevice)));
      } else {
        SpsRcrbOr16 (R_SPS_RCRB_FD2, ((UINT16) BIT0 << WhichDevice));
      }
      SpsRcrbRead16 (R_SPS_RCRB_FD2);
      break;

    case SpsUSBR1:
      if (DeviceFuncCtrl) {
        SpsHeciPciOr16 (0x7A, ((UINT16) (0x100)));
      } else {
        SpsHeciPciAnd16 (0x7A, ((UINT16) (~0x100)));
      }
      break;

    ///
    /// Function Disable SUS well lockdown
    ///
    case SpsFDSWL:
      if (DeviceFuncCtrl) {
        SpsRcrbOr16 (R_SPS_RCRB_FDSW, ((UINT16) B_SPS_RCRB_FDSW_FDSWL));
      } else {
        SpsRcrbAnd16 (R_SPS_RCRB_FDSW, ((UINT16)~(B_SPS_RCRB_FDSW_FDSWL)));
      }
      SpsRcrbRead16 (R_SPS_RCRB_FDSW);
      break;
  }
}

/**
  Get HECI-1 BAR

  @param[out] Heci1Bar            HECI 1 BAR.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetHeci1Bar (
  OUT UINT64                           *Heci1Bar
  )
{
  UINT64                TempHeci1Bar;
  UINT16                FunDis2Value;

  if (Heci1Bar == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Temporarily Enable Heci 1, if HECI-1 disabled.
  ///
  FunDis2Value = SpsRcrbRead16 (R_SPS_RCRB_FD2);
  if ((FunDis2Value & B_SPS_RCRB_FD2_MEI1D) == B_SPS_RCRB_FD2_MEI1D) {
    SpsRcrbWrite16 (R_SPS_RCRB_FD2, (FunDis2Value & ~(B_SPS_RCRB_FD2_MEI1D)));
  }

  if (SpsHeciPciRead32 (SPS_R_HECIMBAR) & 0x4) {
    TempHeci1Bar = (UINT64) SpsHeciPciRead32 (SPS_R_HECIMBAR) & 0xFFFFFFF0;
    TempHeci1Bar |= (UINT64) LShiftU64 (SpsHeciPciRead32 (SPS_R_HECIMBAR + 4), 32);
  } else {
    TempHeci1Bar = (UINT64) SpsHeciPciRead32 (SPS_R_HECIMBAR) & 0xFFFFFFF0;
  }

  ///
  /// Restore Function Disable Value
  ///
  SpsRcrbWrite16 (R_SPS_RCRB_FD2, (FunDis2Value));

  *Heci1Bar = TempHeci1Bar;
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: HECI #1 BAR: (0x%0lx)\n", *Heci1Bar));

  return EFI_SUCCESS;
}

/**
  Get HECI-2 BAR

  @param[out] Heci2Bar            HECI 2 BAR.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetHeci2Bar (
  OUT UINT64                           *Heci2Bar
  )
{
  UINT64                TempHeci2Bar;
  UINT16                FunDis2Value;

  if (Heci2Bar == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Temporarily Enable Heci 2, if HECI-2 disabled.
  ///
  FunDis2Value = SpsRcrbRead16 (R_SPS_RCRB_FD2);
  if ((FunDis2Value & B_SPS_RCRB_FD2_MEI2D) == B_SPS_RCRB_FD2_MEI2D) {
    SpsRcrbWrite16 (R_SPS_RCRB_FD2, (FunDis2Value & ~(B_SPS_RCRB_FD2_MEI2D | B_SPS_RCRB_FD2_MEI1D)));
  }

  if (SpsHeci2PciRead32 (SPS_R_HECIMBAR) & 0x4) {
    TempHeci2Bar = (UINT64) SpsHeci2PciRead32 (SPS_R_HECIMBAR) & 0xFFFFFFF0;
    TempHeci2Bar |= (UINT64) LShiftU64 (SpsHeci2PciRead32 (SPS_R_HECIMBAR + 4), 32);
  } else {
    TempHeci2Bar = (UINT64) SpsHeci2PciRead32 (SPS_R_HECIMBAR) & 0xFFFFFFF0;
  }

  ///
  /// Restore Function Disable Value
  ///
  SpsRcrbWrite16 (R_SPS_RCRB_FD2, (FunDis2Value));

  *Heci2Bar = TempHeci2Bar;
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: HECI #2 BAR: (0x%0lx)\n", *Heci2Bar));

  return EFI_SUCCESS;
}

/**
  Get HECI 1 Firmware Status # 1

  @param[out] MeFs1               HECI 1 Firmware Status # 1

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsGetMeFs1 (
  OUT UINT32                           *MeFs1
  )
{
  UINT16                          FunDis2Value;

  if (MeFs1 == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Temporarily Enable Heci 1, if HECI-1 disabled.
  ///
  FunDis2Value = SpsRcrbRead16 (R_SPS_RCRB_FD2);
  if ((FunDis2Value & B_SPS_RCRB_FD2_MEI1D) == B_SPS_RCRB_FD2_MEI1D) {
    SpsRcrbWrite16 (R_SPS_RCRB_FD2, (FunDis2Value & ~(B_SPS_RCRB_FD2_MEI1D)));
  }

  *MeFs1 = SpsHeciPciRead32 (SPS_R_MEFS1);
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: ME Firmware Status #1: 0x%08x\n", *MeFs1));

  ///
  /// Restore Function Disable Value
  ///
  SpsRcrbWrite16 (R_SPS_RCRB_FD2, (FunDis2Value));

  return EFI_SUCCESS;
}

/**
  Get HECI 1 Firmware Status #2 

  @param[out] MeFs2               HECI 1 Firmware Status # 2 

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsGetMeFs2 (
  OUT UINT32                           *MeFs2
  )
{
  UINT16                          FunDis2Value;

  if (MeFs2 == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Temporarily Enable Heci 1, if HECI-1 disabled.
  ///
  FunDis2Value = SpsRcrbRead16 (R_SPS_RCRB_FD2);
  if ((FunDis2Value & B_SPS_RCRB_FD2_MEI1D) == B_SPS_RCRB_FD2_MEI1D) {
    SpsRcrbWrite16 (R_SPS_RCRB_FD2, (FunDis2Value & ~(B_SPS_RCRB_FD2_MEI1D)));
  }

  *MeFs2 = SpsHeciPciRead32 (SPS_R_MEFS2);
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: ME Firmware Status #2: 0x%08x\n", *MeFs2));

  ///
  /// Restore Function Disable Value
  ///
  SpsRcrbWrite16 (R_SPS_RCRB_FD2, (FunDis2Value));

  return EFI_SUCCESS;
}

///
/// SPS Node Manager
///
/**
  Get HECI 2 Firmware Status # 1 

  @param[out] Nmfs                HECI 2 Firmware Status # 1 

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsGetNmFs (
  OUT UINT32                           *Nmfs
  )
{
  UINT16                          FunDis2Value;

  if (Nmfs == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  ///
  /// Temporarily Enable Heci 2, if HECI-2 disabled.
  ///
  FunDis2Value = SpsRcrbRead16 (R_SPS_RCRB_FD2);
  if ((FunDis2Value & B_SPS_RCRB_FD2_MEI2D) == B_SPS_RCRB_FD2_MEI2D) {
    SpsRcrbWrite16 (R_SPS_RCRB_FD2, (FunDis2Value & ~(B_SPS_RCRB_FD2_MEI2D | B_SPS_RCRB_FD2_MEI1D)));
  }

  *Nmfs = SpsHeci2PciRead32 (SPS_R_NMFS);
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: NMFS : 0x%08x\n", *Nmfs));

  ///
  /// Restore Function Disable Value
  ///
  SpsRcrbWrite16 (R_SPS_RCRB_FD2, (FunDis2Value));

  ///
  /// If NMFS[31] = 0, Node Manager is Disabled.
  ///
  if (!(*Nmfs & BIT31)) {
    DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Node Manager is Dislabed.\n"));
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Initialize PCH power management by clearing the PCH wake reason.

  @param None

  @retval EFI_SUCCESS             Heci initialization completed successfully.
**/
VOID
SpsMeChipsetClearWakeStatus (
  VOID
  )
{
  ///
  /// Clear PCH wake reason in ME_WAKE_STS in the PRSTS configuration register.
  /// This bit must be cleared to allow platform shutdown.
  ///
  SpsRcrbOr32 (R_SPS_RCRB_PRSTS, ((UINT32) B_SPS_RCRB_PRSTS_ME_WAKE_STS));

  SpsRcrbAnd32 (R_SPS_RCRB_FD2, ((UINT32) ~(B_SPS_RCRB_FD2_MEI1D)));

  return;
}

/**
  Save device setting to S3 script table.

  @param None.

**/
VOID
SpsDeviceStatusSave (
  VOID
  )
{
  UINT32  Data;

  ///
  /// Read RCBA register for saving
  ///
  Data = SpsRcrbRead16 (R_SPS_RCRB_FD2);

  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    (UINTN) (SPS_RCRB_BASE + R_SPS_RCRB_FD2),
    1,
    &Data
    );
  Data = SpsRcrbRead16 (R_SPS_RCRB_FDSW);

  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint16,
    (UINTN) (SPS_RCRB_BASE + R_SPS_RCRB_FDSW),
    1,
    &Data
    );

  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Add Function Disable Register to S3 Boot Script.\n"));
}

/**
  Initial HECI-2.

  @param None.

**/
VOID
SpsMeChipsetHeci2Init (
  VOID
  )
{
  UINT64                          Heci2BarAddr;
  SPS_HECI_HOST_CONTROL_REGISTER  *HostCtrlReg;

  ///
  /// Program to 01 if SCI handler is used.
  ///
  SpsHeci2PciWrite16 (SPS_R_HIDM, 0x01);

  ///
  /// Read MEI1 MMIO base address from MEI1_MBAR.
  ///
  SpsMeChipsetGetHeci2Bar (&Heci2BarAddr);

  HostCtrlReg = (SPS_HECI_HOST_CONTROL_REGISTER *)(UINTN)(Heci2BarAddr + H_CSR);

  ///
  /// Setup HECI-2 HIDM register to generate SCI interrupts.
  ///
//  HostCtrlReg->r.H_RST = 0; // clear reset if any
//  HostCtrlReg->r.H_RDY = 1; // host side is ready now
//  HostCtrlReg->r.H_IS = 1;  // clear interrupt if any
//  HostCtrlReg->r.H_IE = 1;  // enable interrupt for SMI
//  HostCtrlReg->r.H_IG = 1;  // trigger interrupt to ME
  HostCtrlReg->r.H_RST = 0; // clear reset if any
  HostCtrlReg->r.H_RDY = 0; // host side is ready now
  HostCtrlReg->r.H_IS = 1;  // clear interrupt if any
  HostCtrlReg->r.H_IG = 0;  // trigger interrupt to ME

}

/**
  Lock CF9.

  @param None.

**/
VOID
SpsMeChipsetCF9Lock (
  VOID
  )
{
  return;
}

/**
  Disable CF9.

  @param None.

**/
VOID
SpsMeChipsetCF9Disable (
  VOID
  )
{
  UINTN             Address;

  Address = SpsPchLpcAddress (R_SPS_LPC_PMIR);

  PciAndThenOr32 (
    Address,
    (UINT32) (~(B_SPS_LPC_PMIR_CF9LOCK | B_SPS_LPC_PMIR_CF9GR)),
    (UINT32) B_SPS_LPC_PMIR_CF9LOCK
    );

  return;
}

/**
  Do global reset by CF9.

  @param None.

**/
VOID
SpsMeChipsetCF9Reset (
  VOID
  )
{
  if ((SpsPchLpcRead32 (R_SPS_LPC_PMIR) & B_SPS_LPC_PMIR_CF9LOCK) != B_SPS_LPC_PMIR_CF9LOCK) {
    ///
    /// Set global reset bit
    ///

    SpsPchLpcOr32 (R_SPS_LPC_PMIR, (UINT32) (B_SPS_LPC_PMIR_CF9GR));
    SpsPchLpcRead32 (R_SPS_LPC_PMIR); /// Read it back for really take I/O transaction effect

    ///
    /// Reset system
    ///
    IoWrite8 (R_SPS_RST_CNT, V_SPS_RST_CNT_HARDSTARTSTATE);
    IoWrite8 (R_SPS_RST_CNT, V_SPS_RST_CNT_HARDRESET);
  } else {
    DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: PMIR already been locked, could not perform a global reset!\n"));
    DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Please perform a global reset/AC off the system.\n"));
  }

  return;
}

///
/// SPS ME Firmware upgrade library function
///
/**
  Enable HECI#1 Device.

  @param None.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetEnableHeciDevice (
  VOID
  )
{
  UINT32                Data32;

  // Enable HECI device if it is disabled
  Data32 = SpsRcrbRead16 (R_SPS_RCRB_FD2);
  if ((Data32 & (UINT32)(B_SPS_RCRB_FD2_MEI1D)) == B_SPS_RCRB_FD2_MEI1D) {
    SpsRcrbAnd32 (R_SPS_RCRB_FD2, (~(UINT32) (B_SPS_RCRB_FD2_MEI1D)));
  }
  return EFI_SUCCESS;
}

/**
  Get ROM linear base address of system from descriptor region

  @param[out] RomBaseAddress      The pointer of ROM linear base address

  @retval EFI_SUCCESS             Get Rom Base Address Successfully.
**/
EFI_STATUS
SpsMeChipsetGetRomBaseAddress (
  OUT UINTN                            *RomBaseAddress
  )
{
  UINT32                Data32;
  UINTN                 NumberOfComponents;
  UINTN                 ComponentDensity[2];

  Data32              = 0;
  NumberOfComponents  = 0;
  ComponentDensity[0] = 0;
  ComponentDensity[1] = 0;

  if (RomBaseAddress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  /// Check does the system used the descripor mode
  SpsRcrbWrite32 (R_SPS_SPI_FDOC, (V_SPS_SPI_FDOC_FDSS_FSDM | R_SPS_SPI_FDBAR_FLVALSIG));
  Data32 = SpsRcrbRead32 (R_SPS_SPI_FDOD);

  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Flash Valud Signature Register : 0x%08x\n", Data32));
  if (Data32 != V_SPS_SPI_FDBAR_FLVALSIG) {
    DEBUG ((DEBUG_ERROR, "[SPS ME CHIPSET LIB] ERROR: Not config in descriptor mode, unsupported!\n"));
    return EFI_NOT_FOUND;
  }

  /// Get # of ROM from descriptor region
  SpsRcrbWrite32 (R_SPS_SPI_FDOC, (V_SPS_SPI_FDOC_FDSS_FSDM | R_SPS_SPI_FDBAR_FLASH_MAP0));
  Data32 = SpsRcrbRead32 (R_SPS_SPI_FDOD);
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Flash Map 0 Register : 0x%08x\n", Data32));
  if ((Data32 & B_SPS_SPI_FDBAR_NC) == V_SPS_SPI_FDBAR_NC_1) {
    NumberOfComponents = 1;
  } else if ((Data32 & B_SPS_SPI_FDBAR_NC) == V_SPS_SPI_FDBAR_NC_2) {
    NumberOfComponents = 2;
  } else {
    DEBUG ((DEBUG_ERROR, "[SPS ME CHIPSET LIB] ERROR: Could not recognize the # of flash components!\n"));
    return EFI_NOT_FOUND;
  }
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Number Of Components : %d\n", NumberOfComponents));

  /// Calculate the each ROM's density
  SpsRcrbWrite32 (R_SPS_SPI_FDOC, (V_SPS_SPI_FDOC_FDSS_COMP | R_SPS_SPI_FCBA_FLCOMP));
  Data32 = SpsRcrbRead32 (R_SPS_SPI_FDOD);
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Flash Components Record Register : 0x%08x\n", Data32));

  switch (Data32 & B_SPS_SPI_FLCOMP_COMP1_MASK) {
  case V_SPS_SPI_FLCOMP_COMP1_512KB:
    ComponentDensity[0] = SIZE_512KB;
    break;

  case V_SPS_SPI_FLCOMP_COMP1_1MB:
    ComponentDensity[0] = SIZE_1MB;
    break;

  case V_SPS_SPI_FLCOMP_COMP1_2MB:
    ComponentDensity[0] = SIZE_2MB;
    break;

  case V_SPS_SPI_FLCOMP_COMP1_4MB:
    ComponentDensity[0] = SIZE_4MB;
    break;

  case V_SPS_SPI_FLCOMP_COMP1_8MB:
    ComponentDensity[0] = SIZE_8MB;
    break;

  case V_SPS_SPI_FLCOMP_COMP1_16MB:
    ComponentDensity[0] = SIZE_16MB;
    break;

  default:
    DEBUG ((DEBUG_ERROR, "[SPS ME CHIPSET LIB] ERROR: Could not recognize the density of flash component 1, unsupported!\n"));
    DEBUG ((DEBUG_ERROR, "[SPS ME CHIPSET LIB] ERROR: Flash Components Record Register : 0x%08x\n", Data32));
    return EFI_UNSUPPORTED;
    break;
  }

  if (NumberOfComponents == 2) {
    switch (Data32 & B_SPS_SPI_FLCOMP_COMP2_MASK) {
    case V_SPS_SPI_FLCOMP_COMP2_512KB:
      ComponentDensity[1] = SIZE_512KB;
      break;

    case V_SPS_SPI_FLCOMP_COMP2_1MB:
      ComponentDensity[1] = SIZE_1MB;
      break;

    case V_SPS_SPI_FLCOMP_COMP2_2MB:
      ComponentDensity[1] = SIZE_2MB;
      break;

    case V_SPS_SPI_FLCOMP_COMP2_4MB:
      ComponentDensity[1] = SIZE_4MB;
      break;

    case V_SPS_SPI_FLCOMP_COMP2_8MB:
      ComponentDensity[1] = SIZE_8MB;
      break;

    case V_SPS_SPI_FLCOMP_COMP2_16MB:
      ComponentDensity[1] = SIZE_16MB;
      break;

    default:
      DEBUG ((DEBUG_ERROR, "[SPS ME CHIPSET LIB] ERROR: Could not recognize the density of flash component 2, unsupported!\n"));
      DEBUG ((DEBUG_ERROR, "[SPS ME CHIPSET LIB] ERROR: Flash Components Record Register : 0x%08x\n", Data32));
      return EFI_UNSUPPORTED;
      break;
    }
  }

  /// Transfer to linear address
  (*RomBaseAddress)  = (UINTN) (SIZE_4GB - ComponentDensity[0] - ComponentDensity[1]);
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: ROM Base Address : 0x%08x\n", *RomBaseAddress));

  return EFI_SUCCESS;
}

/**
  Get descriptor region Base and Limit.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region Limit.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetDescRegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  )
{
  UINT32                Data32;

  if ((Base == NULL) || (Limit == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FREG0_FLASHD);
  *Base  = (UINTN) ((Data32 & B_SPS_SPI_FREG0_BASE_MASK) << 12);
  *Limit = (UINTN) (((Data32 & B_SPS_SPI_FREG0_LIMIT_MASK) >> 4) | 0x0fff);

  DEBUG ( (DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Flash Descriptor Region Base  : 0x%08x\n", *Base));
  DEBUG ( (DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Flash Descriptor Region Limit : 0x%08x\n", *Limit));

  return EFI_SUCCESS;
}

/**
  Get Me region Base and Limit.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region Limit.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetMeRegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  )
{
  UINT32                Data32;

  if ((Base == NULL) || (Limit == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FREG2_ME);
  *Limit = (UINTN) (((Data32 & B_SPS_SPI_FREG2_LIMIT_MASK) >> 4) | 0x0fff);
  *Base  = (UINTN) ((Data32 & B_SPS_SPI_FREG2_BASE_MASK) << 12);

  DEBUG ( (DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Me Region Base  : 0x%08x\n", *Base));
  DEBUG ( (DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Me Region Limit : 0x%08x\n", *Limit));

  return EFI_SUCCESS;
}

/**
  Get GBE region Base and Limit.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region Limit.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetGBERegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  )
{
  UINT32                Data32;

  if ((Base == NULL) || (Limit == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FREG3_GBE);
  *Base  = (UINTN) ((Data32 & B_SPS_SPI_FREG3_BASE_MASK) << 12);
  *Limit = (UINTN) (((Data32 & B_SPS_SPI_FREG3_LIMIT_MASK) >> 4) | 0x0fff);

  DEBUG ( (DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: GBE Base  : 0x%08x\n", *Base));
  DEBUG ( (DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: GBE Limit : 0x%08x\n", *Limit));

  return EFI_SUCCESS;
}

/**
  Get Platform Data Regione(PDR) region Base and Limit.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region Limit.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetPDRRegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  )
{
  UINT32                Data32;

  if ((Base == NULL) || (Limit == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FREG4_PLATFORM_DATA);
  *Base  = (UINTN) ((Data32 & B_SPS_SPI_FREG4_BASE_MASK) << 12);
  *Limit = (UINTN) (((Data32 & B_SPS_SPI_FREG4_LIMIT_MASK) >> 4) | 0x0fff);

  DEBUG ( (DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Platform Data Regione(PDR) Base  : 0x%08x\n", *Base));
  DEBUG ( (DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Platform Data Regione(PDR) Limit : 0x%08x\n", *Limit));

  return EFI_SUCCESS;
}

/**
  Get Device Expansion Region(DER) region Base and Limit.

  @param[out] Base                Point to Region Base.
  @param[out] Limit               Point to Region Limit.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetGetDERRegion (
  OUT UINTN                            *Base,
  OUT UINTN                            *Limit
  )
{
  UINT32                Data32;

  if ((Base == NULL) || (Limit == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FREG5_DER);
  *Base  = (UINTN) ((Data32 & B_SPS_SPI_FREG5_BASE_MASK) << 12);
  *Limit = (UINTN) (((Data32 & B_SPS_SPI_FREG5_LIMIT_MASK) >> 4) | 0x0fff);

  DEBUG ( (DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Device Expansion Region(DER) Base  : 0x%08x\n", *Base));
  DEBUG ( (DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: Device Expansion Region(DER) Limit : 0x%08x\n", *Limit));

  return EFI_SUCCESS;
}

/**
  Check Region write access for Region 0 Flash Descriptor.

  @param None.

  @retval TRUE                    BIOS have write access.
  @retval FALSE                   BIOS have not write access.
**/
BOOLEAN
SpsMeChipsetCheckDescBRWA (
  VOID
  )
{
  UINT32                Data32;

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FRAP);
  if ((Data32 & B_SPS_SPI_FRAP_BRWA_FLASHD) == B_SPS_SPI_FRAP_BRWA_FLASHD) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check Region write access for Region 1 BIOS.

  @param None.

  @retval TRUE                    BIOS have write access.
  @retval FALSE                   BIOS have not write access.
**/
BOOLEAN
SpsMeChipsetCheckBiosBRWA (
  VOID
  )
{
  UINT32                Data32;

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FRAP);
  if ((Data32 & B_SPS_SPI_FRAP_BRWA_BIOS) == B_SPS_SPI_FRAP_BRWA_BIOS) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check Region write access for Region 2 Management Engine Firmware(ME).

  @param None.

  @retval TRUE                    BIOS have write access.
  @retval FALSE                   BIOS have not write access.
**/
BOOLEAN
SpsMeChipsetCheckMeBRWA (
  VOID
  )
{
  UINT32                Data32;

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FRAP);
  if ((Data32 & B_SPS_SPI_FRAP_BRWA_ME) == B_SPS_SPI_FRAP_BRWA_ME) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check Region write access for Region 3 Location for Integrated LAN firmware and MAC address(GBE).

  @param None.

  @retval TRUE                    BIOS have write access.
  @retval FALSE                   BIOS have not write access.
**/
BOOLEAN
SpsMeChipsetCheckGbeBRWA (
  VOID
  )
{
  UINT32                Data32;

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FRAP);
  if ((Data32 & B_SPS_SPI_FRAP_BRWA_GBE) == B_SPS_SPI_FRAP_BRWA_GBE) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check Region write access for Region 4 Platform Data Region(PDR).

  @param None.

  @retval TRUE                    BIOS have write access.
  @retval FALSE                   BIOS have not write access.
**/
BOOLEAN
SpsMeChipsetCheckPDRBRWA (
  VOID
  )
{
  UINT32                Data32;

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FRAP);
  if ((Data32 & B_SPS_SPI_FRAP_BRWA_PDR) == B_SPS_SPI_FRAP_BRWA_PDR) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check Region write access for Region 5 Device Expansion Region(DER) (SPS FW Only).

  @param None.

  @retval TRUE                    BIOS have write access.
  @retval FALSE                   BIOS have not write access.
**/
BOOLEAN
SpsMeChipsetCheckDERBRWA (
  VOID
  )
{
  UINT32                Data32;

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FRAP);
  if ((Data32 & B_SPS_SPI_FRAP_BRWA_DER) == B_SPS_SPI_FRAP_BRWA_DER) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check Region read access for Region 0 Flash Descriptor.

  @param None.

  @retval TRUE                    BIOS have read access.
  @retval FALSE                   BIOS have not read access.
**/
BOOLEAN
SpsMeChipsetCheckDescBRRA (
  VOID
  )
{
  UINT32                Data32;

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FRAP);
  if ((Data32 & B_SPS_SPI_FRAP_BRRA_FLASHD) == B_SPS_SPI_FRAP_BRRA_FLASHD) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check Region read access for Region 1 Region 1 BIOS.

  @param None.

  @retval TRUE                    BIOS have read access.
  @retval FALSE                   BIOS have not read access.
**/
BOOLEAN
SpsMeChipsetCheckBiosBRRA (
  VOID
  )
{
  UINT32                Data32;

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FRAP);
  if ((Data32 & B_SPS_SPI_FRAP_BRRA_BIOS) == B_SPS_SPI_FRAP_BRRA_BIOS) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check Region read access for Region 2 Management Engine Firmware(ME).

  @param None.

  @retval TRUE                    BIOS have read access.
  @retval FALSE                   BIOS have not read access.
**/
BOOLEAN
SpsMeChipsetCheckMeBRRA (
  VOID
  )
{
  UINT32                Data32;

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FRAP);
  if ((Data32 & B_SPS_SPI_FRAP_BRRA_ME) == B_SPS_SPI_FRAP_BRRA_ME) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check Region read access for Region 3 Location for Integrated LAN firmware and MAC address(GBE).

  @param None.

  @retval TRUE                    BIOS have read access.
  @retval FALSE                   BIOS have not read access.
**/
BOOLEAN
SpsMeChipsetCheckGbeBRRA (
  VOID
  )
{
  UINT32                Data32;

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FRAP);
  if ((Data32 & B_SPS_SPI_FRAP_BRRA_GBE) == B_SPS_SPI_FRAP_BRRA_GBE) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check Region read access for Region 4 Platform Data Region(PDR).

  @param None.

  @retval TRUE                    BIOS have read access.
  @retval FALSE                   BIOS have not read access.
**/
BOOLEAN
SpsMeChipsetCheckPDRBRRA (
  VOID
  )
{
  UINT32                Data32;

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FRAP);
  if ((Data32 & B_SPS_SPI_FRAP_BRRA_PDR) == B_SPS_SPI_FRAP_BRRA_PDR) {
    return TRUE;
  }
  return FALSE;
}

/**
  Check Region read access for Region 5 Device Expansion Region(DER) (SPS FW Only).

  @param None.

  @retval TRUE                    BIOS have read access.
  @retval FALSE                   BIOS have not read access.
**/
BOOLEAN
SpsMeChipsetCheckDERBRRA (
  VOID
  )
{
  UINT32                Data32;

  Data32 = SpsRcrbRead32 (R_SPS_SPI_FRAP);
  if ((Data32 & B_SPS_SPI_FRAP_BRRA_DER) == B_SPS_SPI_FRAP_BRRA_DER) {
    return TRUE;
  }
  return FALSE;
}

/**
  Protect descriptor region by PRx.

  @param None.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetProtectDescRegion (
  VOID
  )
{
  UINT32                Data32;
  UINT32                Reg;
  UINT32                PRxRegister;

  //
  // Protect Descriptor region if Bios write access was not permission.
  //
  if ((!SpsMeChipsetCheckDescBRWA ()) || (!SpsMeChipsetCheckDescBRRA ())) {
    Data32 = SpsRcrbRead32 (R_SPS_SPI_FREG0_FLASHD);
    PRxRegister = (Data32 & B_SPS_SPI_PR0_PRB_MASK);
    PRxRegister |= (Data32 & B_SPS_SPI_PR0_PRL_MASK);
    PRxRegister |= B_SPS_SPI_PR0_WPE;

    for (Reg = R_SPS_SPI_PR0; Reg <= R_SPS_SPI_PR4; Reg += sizeof (UINT32)) {
      Data32 = SpsRcrbRead32 (Reg);
//[-start-140120-IB10310043-add]//
      if (Data32 == PRxRegister) {
        ///
        /// Already protected.
        ///
        return EFI_SUCCESS;
      }
//[-end-140120-IB10310043-add]//
      if (Data32 == 0) {
        SpsRcrbWrite32 (Reg, PRxRegister);
        break;
      }
    }

    if (Reg > R_SPS_SPI_PR4) {
      return EFI_NOT_FOUND;
    }
  }
  return EFI_SUCCESS;
}

/**
  Protect ME region by PRx.

  @param[out] FactoryDefaultBase  ME Region Base.
  @param[out] FactoryDefaultBase  ME Region Limit.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeChipsetProtectMeRegion (
  IN UINT32                            FactoryDefaultBase,
  IN UINT32                            FactoryDefaultLimit
  )
{
  UINT32                Data32;
  UINT32                Reg;
  UINT32                PRxRegister;
  UINT32                RangeBase;
  UINT32                RangeLimit;

  ///
  /// Check if the interface has already been locked-down.
  ///
  Data32 = SpsRcrbRead32 (R_SPS_SPI_HSFS);
  if ((Data32 & B_SPS_SPI_HSFS_FLOCKDN) != 0) {
    DEBUG ((DEBUG_ERROR, "[SPS ME CHIPSET LIB] ERROR: SPI has been locked - Access Denied!\n"));
    return EFI_ACCESS_DENIED;
  }

  if (FactoryDefaultLimit == 0) {
    DEBUG ((DEBUG_ERROR, "[SPS ME CHIPSET LIB] ERROR: ME region size is 0, unsupported!\n"));
    return EFI_UNSUPPORTED;
  }

  ///
  /// Get ME region Base
  ///
  Data32 = SpsRcrbRead32 (R_SPS_SPI_FREG2_ME);
  RangeBase = Data32;
  RangeBase = RangeBase & B_SPS_SPI_FREG2_BASE_MASK;
  RangeBase = RangeBase + (FactoryDefaultBase >> 12);

  ///
  /// Mask Protect Limit
  /// Protected Range Limit - The filed PR0[28:16] corresponds to FLA address bits FREG2[24:12]
  ///
  RangeLimit = RangeBase << 16;
  RangeLimit = RangeLimit + ((FactoryDefaultLimit - 1) << 4);
  RangeLimit = RangeLimit & B_SPS_SPI_FREG2_LIMIT_MASK;
  ///
  /// Protect ME region by program PRx registers.
  ///
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: SPS ME Spi Protect ME Region\n"));
  PRxRegister = (RangeLimit | RangeBase | B_SPS_SPI_PR0_WPE);

  for (Reg = R_SPS_SPI_PR0; Reg <= R_SPS_SPI_PR4; Reg += sizeof (UINT32)) {
    Data32 = SpsRcrbRead32 (Reg);
    if (Data32 == PRxRegister) {
      ///
      /// Already protected.
      ///
      return EFI_SUCCESS;
    }
    if (Data32 == 0) {
      SpsRcrbWrite32 (Reg, PRxRegister);
      break;
    }
  }

  if (Reg > R_SPS_SPI_PR4) {
    return EFI_NOT_FOUND;
  }
  return EFI_SUCCESS;
}

/**
  Early BIOS message to SPS ME firmware.

  @param[in] Value                Message will send to SPS ME firmware.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsMeEarlyBiosMessage (
  IN UINT32                            Value
  )
{
  SpsHeciPciAndThenOr32 (SPS_R_HOST_H_GS, 0, Value);
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: ME H_GS written: 0x%08x\n", Value));

  return EFI_SUCCESS;
}

/**
  Early Get mPhy table version from SPS ME firmware.

  @param[out] Version             mPhy table version.
  @param[out] CRC16               mPhy table CRC16.

  @retval EFI_SUCCESS             Function completed.
**/
EFI_STATUS
SpsGetmPhySurvivabilityTableVesrion (
  OUT UINT16                           *Version,
  OUT UINT16                           *CRC16
  )
{
  UINT32                Data32;
  if ((Version == NULL) || (CRC16 == NULL)) {
    return EFI_INVALID_PARAMETER;
  }

  Data32 = SpsHeciPciRead32 (SPS_R_GS_SHDW4);
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: ME GS_SHDW4: 0x%08x\n", Data32));

  *CRC16   = Data32 &  0xFFFF;
  *Version = Data32 >> 16;
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: mPhy Survivability Table CRC16  : 0x%04x\n", *CRC16));
  DEBUG ((DEBUG_INFO, "[SPS ME CHIPSET LIB] INFO: mPhy Survivability Table Version: 0x%04x\n", *Version));

  return EFI_SUCCESS;
}

