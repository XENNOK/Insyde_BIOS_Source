/** @file
  File to contain all the hardware specific stuff for the Smm Sx dispatch protocol.

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
#include "PchSmmHelpers.h"

extern EFI_PHYSICAL_ADDRESS         mResvMmioBaseAddr;
///
/// Maximum loop time for GbE status check
///
#define GBE_MAX_LOOP_TIME 4000

const PCH_SMM_SOURCE_DESC SX_SOURCE_DESC = {
  PCH_SMM_NO_FLAGS,
  {
    {
      {
        ACPI_ADDR_TYPE,
        R_PCH_SMI_EN
      },
      S_PCH_SMI_EN,
      N_PCH_SMI_EN_ON_SLP_EN
    },
    NULL_BIT_DESC_INITIALIZER
  },
  {
    {
      {
        ACPI_ADDR_TYPE,
        R_PCH_SMI_STS
      },
      S_PCH_SMI_STS,
      N_PCH_SMI_STS_ON_SLP_EN
    }
  }
};

/**
  Get the Sleep type

  @param[in] Record               No use
  @param[out] Context             The context that includes SLP_TYP bits to be filled

  @retval None
**/
VOID
EFIAPI
SxGetContext (
  IN  DATABASE_RECORD    *Record,
  OUT PCH_SMM_CONTEXT    *Context
  )
{
  UINT32  Pm1Cnt;

  Pm1Cnt = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT));

  ///
  /// By design, the context phase will always be ENTRY
  ///
  Context->Sx.Phase = SxEntry;

  ///
  /// Map the PM1_CNT register's SLP_TYP bits to the context type
  ///
  switch (Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) {
  case V_PCH_ACPI_PM1_CNT_S0:
    Context->Sx.Type = SxS0;
    break;

  case V_PCH_ACPI_PM1_CNT_S1:
    Context->Sx.Type = SxS1;
    break;

  case V_PCH_ACPI_PM1_CNT_S3:
    Context->Sx.Type = SxS3;
    break;

  case V_PCH_ACPI_PM1_CNT_S4:
    Context->Sx.Type = SxS4;
    break;

  case V_PCH_ACPI_PM1_CNT_S5:
    Context->Sx.Type = SxS5;
    break;

  default:
    ASSERT (FALSE);
    break;
  }
}

/**
  Check whether sleep type of two contexts match

  @param[in] Context1             Context 1 that includes sleep type 1
  @param[in] Context2             Context 2 that includes sleep type 2

  @retval FALSE                   Sleep types match
  @retval TRUE                    Sleep types don't match
**/
BOOLEAN
EFIAPI
SxCmpContext (
  IN PCH_SMM_CONTEXT     *Context1,
  IN PCH_SMM_CONTEXT     *Context2
  )
{
  return (BOOLEAN) (Context1->Sx.Type == Context2->Sx.Type);
}

/**
  Check ready flag to see if writing to MDIC is done.

  @param[in] GbEBar               GbE Memory Base Address Register

  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_TIMEOUT             Checking flag time out.
**/
EFI_STATUS
CheckReadyFlag (
  UINT32          GbEBar
  )
{
  UINT32  ReadyFlag;
  UINT32  LoopTime;

  ReadyFlag = 0;

  for (LoopTime = 0; LoopTime < GBE_MAX_LOOP_TIME; LoopTime++) {
    ReadyFlag = MmioRead32 (GbEBar + R_PCH_MBARA_GBECSR3) & B_PCH_MBARA_GBECSR3_RB;

    if (ReadyFlag) {
      break;
    }

    PchPmTimerStall (10);
  }

  if (LoopTime >= GBE_MAX_LOOP_TIME) {
    return EFI_TIMEOUT;
  }

  return EFI_SUCCESS;
}

/**
  PCH BIOS Spec Rev 0.5.0 Section 10.5
  Additional Internal GbE Controller special cases WOL Support

  @param[in] None

  @retval None
**/
VOID
GbES02SxWorkaround (
  VOID
  )
{
  UINTN       PciD25F0RegBase;
  UINTN       PciD31F0RegBase;
  UINT32      GbEBar;
  UINT32      GbEBarB;
  UINT16      CmdReg;
  UINT32      RAL0;
  UINT32      RAH0;
  UINT32      PhyCtrl;
  UINT32      ExtCnfCtrl;
  UINT32      Buffer;
  UINT32      LoopTime;
  UINT32      RootComplexBar;
  UINT32      PchGpioBase;
  EFI_STATUS  Status;

  PciD25F0RegBase = MmPciAddress (
                      0,
                      PCI_BUS_NUMBER_PCH_LAN,
                      PCI_DEVICE_NUMBER_PCH_LAN,
                      PCI_FUNCTION_NUMBER_PCH_LAN,
                      0
                      );
  PciD31F0RegBase = MmPciAddress (
                      0,
                      0,
                      PCI_DEVICE_NUMBER_PCH_LPC,
                      PCI_FUNCTION_NUMBER_PCH_LPC,
                      0
                      );
  RootComplexBar  = PCH_RCRB_BASE;
  PchGpioBase     = (MmioRead32 (PciD31F0RegBase + R_PCH_LPC_GPIO_BASE)) &~BIT0;
  GbEBar          = 0;
  GbEBarB         = 0;
  CmdReg          = 0;
  Buffer          = 0;

  if (((MmioRead16 (RootComplexBar + R_PCH_RCRB_BUC)) & BIT5) == 0) {
    ///
    /// System BIOS requires to program the registers listed below for internal GbE to function upon S0 to S3,4,5 transition
    /// (When ME off and GbE device in D0)
    ///
    /// Note: Time out should be applied for MBARA + Offset 20h[28] verification to avoid non respond loop. Upon time out,
    ///       system BIOS is required to clear MBARA + Offset F00h [5] = 0b before exiting the WA.
    ///
    /// Check if GbE device is in D0 state
    ///
    if ((MmioRead16 (PciD25F0RegBase + R_PCH_LAN_PMCS) & (UINT16) B_PCH_LAN_PMCS_PS) == (UINT16) V_PCH_LAN_PMCS_PS0) {
      GbEBar = MmioRead32 (PciD25F0RegBase + R_PCH_LAN_MEM_BASE_A);
      ///
      /// Step 1
      /// If MBARA + Offset 5800h [0] = 1b then proceed the steps below
      ///
      if (MmioRead32 (GbEBar + R_PCH_MBARA_GBECSR9) & B_PCH_MBARA_GBECSR9_APME) {
        ///
        /// Step 2
        /// System BIOS perform read to MBARA + Offset 5400h [31:0], MBARA + Offset 5404h [31:0]
        /// and MBARA + Offset F00h [31:0]
        ///
        RAL0        = MmioRead32 (GbEBar + R_PCH_MBARA_GBECSR7);
        RAH0        = MmioRead32 (GbEBar + R_PCH_MBARA_GBECSR8);
        ExtCnfCtrl  = MmioRead32 (GbEBar + R_PCH_MBARA_GBECSR5);
        ///
        /// Step 3
        /// Ensure that MBARA + Offset F00h [5] = 1b
        /// a. Set MBARA + Offset F00h [31:0] value with the value read in step 2 or with 0x20 (set bit 5)
        /// b. Read MBARA + Offset F00h
        /// c. If MBARA + Offset F00h [5] = 1b (true) continue else wait X Sec and go back to step 3.b for Y times
        ///    (X*Y  totals to ~200mSec) if false - exit flow by jumping to step 32.
        ///
        MmioWrite32 (GbEBar + R_PCH_MBARA_GBECSR5, ExtCnfCtrl | B_PCH_MBARA_GBECSR5_SWFLAG);

        for (LoopTime = 0; LoopTime < GBE_MAX_LOOP_TIME; LoopTime++) {
          ExtCnfCtrl = MmioRead32 (GbEBar + R_PCH_MBARA_GBECSR5);

          if (ExtCnfCtrl & B_PCH_MBARA_GBECSR5_SWFLAG) {
            break;
          }

          PchPmTimerStall (50);
        }

        if (LoopTime >= GBE_MAX_LOOP_TIME) {
          goto ExitGbEWa;
        }
        ///
        /// Step 4
        /// If MBARA + Offset 5B54h [15] = 1b then jump to Step 10
        ///
        if ((MmioRead32 (GbEBar + 0x5B54) & BIT15) != BIT15) {
          ///
          /// Step 5
          /// If MBARA + Offset F10h [2] = 1b, then set MBARA + Offset F10h[1] = 1b. Else clear MBARA + Offset F10h[1] = 0b
          ///
          if (MmioRead32 (GbEBar + R_PCH_MBARA_GBECSR6) & B_PCH_MBARA_GBECSR6_LPLUND) {
            MmioOr32 (GbEBar + R_PCH_MBARA_GBECSR6, (UINT32) B_PCH_MBARA_GBECSR6_LPLUD);
          } else {
            MmioAnd32 (GbEBar + R_PCH_MBARA_GBECSR6, (UINT32)~B_PCH_MBARA_GBECSR6_LPLUD);
          }
          ///
          /// Step 6
          /// Set MBARA + Offset 20h = 0x043f0000. Verify MBARA + Offset 20h[28] = 1b
          ///
          MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x043f0000);

          Status = CheckReadyFlag (GbEBar);
          if (EFI_ERROR (Status)) {
            goto ExitGbEWa;
          }
          ///
          /// Step 7
          /// Wait 4 mSec
          ///
          PchPmTimerStall (4 * 1000);
          ///
          /// Step 8
          /// Set MBARA + Offset 20h = 0x04390000 or with 0x400 or with 0x40 if MBARA + Offset F10h [3] = 1b
          /// or with 0x04 if MBARA + Offset F10h [2] = 1b
          ///
          Buffer = 0x04390000 | 0x400;
          if (MmioRead32 (GbEBar + R_PCH_MBARA_GBECSR6) & B_PCH_MBARA_GBECSR6_GbE_DIS) {
            Buffer |= 0x40;
          }

          if (MmioRead32 (GbEBar + R_PCH_MBARA_GBECSR6) & B_PCH_MBARA_GBECSR6_LPLUND) {
            Buffer |= 0x04;
          }

          MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), Buffer);
          ///
          /// Step 9
          /// Verify MBARA + Offset 20h[28] = 1b
          ///
          Status = CheckReadyFlag (GbEBar);
          if (EFI_ERROR (Status)) {
            goto ExitGbEWa;
          }
        }
        ///
        /// Step 10
        /// Set MBARA + Offset 20h = 0x043f6400
        ///
        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x043f6400);
        ///
        /// Step 11
        /// Wait 4 mSec
        ///
        PchPmTimerStall (4 * 1000);
        ///
        /// Step 12
        /// Set MBARA + Offset F10h [6] = 1b (read modify write)
        ///
        PhyCtrl = MmioRead32 (GbEBar + R_PCH_MBARA_GBECSR6);
        MmioWrite32 (GbEBar + R_PCH_MBARA_GBECSR6, PhyCtrl | B_PCH_MBARA_GBECSR6_GGD);
        ///
        /// Step 13
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x4310010
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x4310010);
        ///
        /// Step 14
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x4320000 or with
        /// the least significant word of MBARA + offset 5400 that read in step 2
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), (0x4320000 | (RAL0 & 0x0000FFFF)));
        ///
        /// Step 15
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x4310011
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x4310011);
        ///
        /// Step 16
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x4320000 or with
        /// the most significant word of MBARA + offset 5400 that read in step 2
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), (0x4320000 | (RAL0 >> 16)));
        ///
        /// Step 17
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x4310012
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x4310012);
        ///
        /// Step 18
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x4320000 or with
        /// the least significant word of MBARA + offset 5404 that read in step 2
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), (0x4320000 | (RAH0 & B_PCH_MBARA_GBECSR8_RAH)));
        ///
        /// Step 19
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x4310013
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x4310013);
        ///
        /// Step 20
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x4328000
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x4328000);
        ///
        /// Step 21
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x4310001
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x4310001);
        ///
        /// Step 22
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x8320000
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x8320000);
        ///
        /// Step 23
        /// Verify MBARA + Offset 20h[28] = 1b, TEMP[15:0] = MBARA + Offset 20h [15:0]
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        Buffer = MmioRead32 (GbEBar + R_PCH_MBARA_GBECSR3) & B_PCH_MBARA_GBECSR3_DATA;
        ///
        /// Step 24
        /// Set MBARA + Offset 20h = 0x4320000 or TEMP[15:0] or 0x0001
        ///
        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x4320000 | Buffer | 0x0001);
        ///
        /// Step 25
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x43f6460
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x43f6460);
        ///
        /// Step 26
        /// Wait 4 mSec
        ///
        PchPmTimerStall (4 * 1000);
        ///
        /// Step 27
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x4310042
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x4310042);
        ///
        /// Step 28
        /// Verify MBARA + Offset 20h[28] = 1b.
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x43F6020);

        ///
        /// Step 29
        /// Wait 4 mSec
        ///
        PchPmTimerStall (4 * 1000);

        ///
        /// Step 30
        /// Verify MBARA + Offset 20h[28] = 1b, set MBARA + Offset 20h = 0x8310000
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x8310000);
        ///
        /// Step 31
        /// Verify MBARA + Offset 20h[28] = 1b, TEMP[15:0] = MBARA + 20[15:0]
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

        Buffer = MmioRead32 (GbEBar + R_PCH_MBARA_GBECSR3) & 0x0000FFFF;

        ///
        /// Step 32
        /// Verify MBARA + 20h[28] = 1b, set MBARA + 20h = 4310000h or with the TEMP[15:0] or with 10h
        ///
        Status = CheckReadyFlag (GbEBar);
        if (EFI_ERROR (Status)) {
          goto ExitGbEWa;
        }

ExitGbEWa:

        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR3), 0x4310000 | Buffer | 0x10);
        ///
        /// Step 33
        /// Verify MBARA + Offset 20h[28] = 1b
        ///
        Status = CheckReadyFlag (GbEBar);

        ///
        /// Step 34
        /// Clear MBARA + Offset F00h [5] = 0b (read modify write)
        ///
        MmioWrite32 ((GbEBar + R_PCH_MBARA_GBECSR5), (ExtCnfCtrl & (UINT32) (~BIT5)));

      }
    }
  }
}

typedef struct {
  UINT8 Device;
  UINT8 Function;
} USB_CONTROLLER;

/**
  PCH BIOS Spec Rev 0.5.0, Section 12.10.1
  Additional Programming Requirements prior to enter
  S4/S5

  @param[in] None

  @retval None
**/
VOID
UsbS02SxWorkaround (
  VOID
  )
{
  UINT8           Index;
  UINTN           EhciPciRegBase;
  UINT32          UsbBar;
  UINT16          CmdReg;
  UINT16          PowerState;
  USB_CONTROLLER  EhciControllersMap[PchEhciControllerMax] = {
    {
      PCI_DEVICE_NUMBER_PCH_USB,
      PCI_FUNCTION_NUMBER_PCH_EHCI
    },
    {
      PCI_DEVICE_NUMBER_PCH_USB_EXT,
      PCI_FUNCTION_NUMBER_PCH_EHCI2
    }
  };

  ///
  /// System BIOS must execute the following steps prior to enter S4/S5.
  ///
  for (Index = 0; Index < GetPchEhciMaxControllerNum (); Index++) {
    ///
    /// Step 1
    /// Read "Memory Base Address (MEM_BASE) Register" of D26/D29:F0
    ///
    EhciPciRegBase  = MmPciAddress (0, 0, EhciControllersMap[Index].Device, EhciControllersMap[Index].Function, 0);
    UsbBar          = MmioRead32 (EhciPciRegBase + R_PCH_EHCI_MEM_BASE);
    CmdReg          = MmioRead16 (EhciPciRegBase + R_PCH_EHCI_COMMAND_REGISTER);
    PowerState      = MmioRead16 (EhciPciRegBase + R_PCH_EHCI_PWR_CNTL_STS);

    if (UsbBar != 0xFFFFFFFF) {
      ///
      /// Check if the Ehci device is in D3 power state
      ///
      if ((PowerState & B_PCH_EHCI_PWR_CNTL_STS_PWR_STS) == V_PCH_EHCI_PWR_CNTL_STS_PWR_STS_D3) {
        ///
        /// Step 2
        /// Set "Power State" bit of PWR_CNTL_STS register, D26/D29:F0:54h [1:0] = 0h
        ///
        MmioWrite16 (EhciPciRegBase + R_PCH_EHCI_PWR_CNTL_STS, (PowerState &~B_PCH_EHCI_PWR_CNTL_STS_PWR_STS));
        ///
        /// Step 3
        /// Write back the value from step 1 to the "Memory Base Address (MEM_BASE) Register" of D26/D29:F0
        ///
        MmioWrite32 (EhciPciRegBase + R_PCH_EHCI_MEM_BASE, UsbBar);
        ///
        /// Step 4
        /// Enable "Memory Space Enable (MSE)" bit, set D26/D29:F0:04h [1] = 1b.
        ///
        MmioOr16 (
          EhciPciRegBase + R_PCH_EHCI_COMMAND_REGISTER,
          (UINT16) (B_PCH_EHCI_COMMAND_MSE)
          );
      }
      ///
      /// Step 5
      /// Clear "Asynchronous Schedule Enable" and "Periodic Schedule Enable" bits, if "Run/Stop (RS)" bit, MEM_BASE + offset 20h [0] = 1b.
      /// Proceed to steps below if "Run/Stop (RS)" bit, MEM_BASE + offset 20h [0] = 0b.
      ///
      if (!(MmioRead32 (UsbBar + R_PCH_EHCI_USB2CMD) & B_PCH_EHCI_USB2CMD_RS)) {
        MmioAnd32 (UsbBar + R_PCH_EHCI_USB2CMD, (UINT32)~(B_PCH_EHCI_USB2CMD_ASE | B_PCH_EHCI_USB2CMD_PSE));
        MmioOr32 (UsbBar + R_PCH_EHCI_USB2CMD, B_PCH_EHCI_USB2CMD_RS);
      }
      ///
      /// Step 6
      /// If "Port Enabled/Disabled" bit of Port N Status and Control (PORTSC) Register is set, MEM_BASE + 64h [2] = 1b,
      /// proceed steps below else continue with S4/S5.
      ///
      if ((MmioRead32 (UsbBar + R_PCH_EHCI_PORTSC0) & R_PCH_EHCI_PORTSC0_PORT_EN_DIS)) {
        ///
        /// Step 7
        /// Ensure that "Suspend" bit of Port N Status and Control (PORTSC) Register is set, MEM_BASE + 64h [7] = 1b.
        ///
        if (!(MmioRead32 (UsbBar + R_PCH_EHCI_PORTSC0) & R_PCH_EHCI_PORTSC0_SUSPEND)) {
          MmioOr32 (UsbBar + R_PCH_EHCI_PORTSC0, R_PCH_EHCI_PORTSC0_SUSPEND);
        }
        ///
        /// Step 8
        /// Set delay of 25ms
        ///
        PchPmTimerStall (25 * 1000);
        ///
        /// Step 9
        /// Clear "Run/Stop (RS)" bit, MEM_BASE + offset 20h [0] = 0b.
        ///
        MmioAnd32 (UsbBar + R_PCH_EHCI_USB2CMD, (UINT32)~(B_PCH_EHCI_USB2CMD_RS));
      }
      ///
      /// If the EHCI device is in D3 power state before executing this WA
      ///
      if ((PowerState & B_PCH_EHCI_PWR_CNTL_STS_PWR_STS) == V_PCH_EHCI_PWR_CNTL_STS_PWR_STS_D3) {
        ///
        /// Restore PCI Command Register
        ///
        MmioWrite16 (EhciPciRegBase + R_PCH_EHCI_COMMAND_REGISTER, CmdReg);
        ///
        /// Set "Power State" bit of PWR_CNTL_STS register to D3 state, D26/D29:F0:54h [1:0] = 3h
        ///
        MmioWrite16 (EhciPciRegBase + R_PCH_EHCI_PWR_CNTL_STS, PowerState);
      }
      ///
      /// Step 10
      /// Continue with S4/S5
      ///
    }
  }
}

/**
  Additional xHCI Controller Configurations Prior to Entering S3/S4/S5

  @param[in] None

  @retval None
**/
VOID
XhciSxWorkaround (
  VOID
  )
{
  UINT32      RootComplexBar;
  UINTN       XhciPciMmBase;
  UINT8       OrgCmdByte;
  UINT32      OrgMmioAddr;
  UINT32      OrgMmioHAddr;
  UINT8       OrgPwrSts;
  PCH_SERIES  PchSeries;
  UINT32      XhciMmioBase;
  UINT32      PortScOffset[4];
  UINT32      Data32;
  UINT32      Index;
  UINT32      ResetMask;

  PchSeries       = GetPchSeries();

  RootComplexBar  = PCH_RCRB_BASE;
  ///
  /// Check if XHCI controller is enabled
  ///
  if ((MmioRead32 (RootComplexBar + R_PCH_RCRB_FUNC_DIS) & (UINT32) B_PCH_RCRB_FUNC_DIS_XHCI) != 0) {
    return;
  }
  XhciPciMmBase = MmPciAddress (
                      0,
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_XHCI,
                      PCI_FUNCTION_NUMBER_PCH_XHCI,
                      0
                      );

  //
  // Save Cmd and XhciBar and PwrSts registers
  //
  OrgCmdByte     = MmioRead8  (XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER);
  OrgMmioAddr    = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE) & 0xFFFF0000;
  OrgMmioHAddr   = MmioRead32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE + 4);
  OrgPwrSts      = MmioRead8  (XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS);
  //
  // Bring device back to D0
  //
  MmioAnd8 (XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS, (UINT8)~(B_PCH_XHCI_PWR_CNTL_STS_PWR_STS));
  //
  // Use the reserved MMIO
  // Clear MSE before changing MMIO address
  //
  MmioAnd8 (XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER, (UINT8)~(B_PCH_XHCI_COMMAND_BME | B_PCH_XHCI_COMMAND_MSE));
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE, (UINT32)mResvMmioBaseAddr);
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE + 4, 0);
  XhciMmioBase = (UINT32)mResvMmioBaseAddr;
  //
  // Set MSE
  //
  MmioOr8 (XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER, (B_PCH_XHCI_COMMAND_BME | B_PCH_XHCI_COMMAND_MSE));

  //
  // XHC W/A for LPT-LP
  //   Clear PCI CFG offset 0xB0[14:13] for LPT-LP
  //   Clear MMIO Offset 0x816C[14]
  //   Clear MMIO Offset 0x816C[2]
  //   Wait until all SS ports are out of polling
  //   For each SS port which is disconnected (i.e. PORTS.PLS=5h) and CSC=0
  //     Issue Warm Port Reset
  //   Wait 101ms
  //   Write '1' to all Port Change Status bits if reset
  //   Set MMIO Offset 0x80E0[15]
  //
  // For PCH H and LP
  //   Clear MMIO Offset 0x8154[31]
  //
  if (PchSeries == PchLp) {
    //
    // Clear PCI CFG offset 0xB0[14:13] for LPT-LP
    //
    MmioAnd32 (XhciPciMmBase + 0xB0, (UINT32)~(BIT14 | BIT13));
    //
    // Clear MMIO Offset 0x816C[14]
    // Clear MMIO Offset 0x816C[2]
    //
    MmioAnd32 (XhciMmioBase + 0x816C, (UINT32)~(BIT14 | BIT2));

    PortScOffset[0] = R_PCH_LP_XHCI_PORTSC1USB3;
    PortScOffset[1] = R_PCH_LP_XHCI_PORTSC2USB3;
    PortScOffset[2] = R_PCH_LP_XHCI_PORTSC3USB3;
    PortScOffset[3] = R_PCH_LP_XHCI_PORTSC4USB3;
    //
    // Wait until all ports are out of polling (PP=1, PLS=7)
    //
    for (Index = 0; Index < 4; Index++) {
      Data32 = MmioRead32 (XhciMmioBase + PortScOffset[Index]);
      //
      // Check if PLS = 7, and PP = 1
      //
      while ((Data32 & B_PCH_XHCI_PORTSCXUSB3_PLS) == V_PCH_XHCI_PORTSCXUSB3_PLS_POLLING)
      {
        PchPmTimerStall (10);
        Data32 = MmioRead32 (XhciMmioBase + PortScOffset[Index]);
      }
    }
    ResetMask = 0;
    for (Index = 0; Index < 4; Index++) {
      Data32 = MmioRead32 (XhciMmioBase + PortScOffset[Index]);
      //
      // If Port X is Disconnected (i.e. PORTS.PLS=5h) AND PORTSC{X}.CSC=0
      // Check if PLS = 5, CSC = 0, and PP = 1
      //
      if (((Data32 & B_PCH_XHCI_PORTSCXUSB3_PLS) == V_PCH_XHCI_PORTSCXUSB3_PLS_RXDETECT) &&
          ((Data32 & B_PCH_XHCI_PORTSCXUSB3_CSC) == 0))
      {
        //
        // Issue Warm Port Reset
        //
        ResetMask |= (1 << Index);
        Data32 &= (UINT32)~(B_PCH_XHCI_PORTSCXUSB3_PED);
        Data32 |= B_PCH_XHCI_PORTSCXUSB3_WPR;
        MmioWrite32 (XhciMmioBase + PortScOffset[Index], Data32);
      }
    }

    if (ResetMask != 0) {
      //
      // Wait 101ms to ensure reset completed
      //
      PchPmTimerStall (101 * 1000);

      for (Index = 0; Index < 4; ++Index) {
        if (ResetMask & (1 << Index)) {
          Data32 = MmioRead32 (XhciMmioBase + PortScOffset[Index]);
          Data32 &= (UINT32)~(B_PCH_XHCI_PORTSCXUSB3_PED);
          //
          // Write '1' to All Port Change Status
          //
          Data32 |= (B_PCH_XHCI_PORTSCXUSB3_CEC |
                     B_PCH_XHCI_PORTSCXUSB3_PLC |
                     B_PCH_XHCI_PORTSCXUSB3_PRC |
                     B_PCH_XHCI_PORTSCXUSB3_OCC |
                     B_PCH_XHCI_PORTSCXUSB3_WRC |
                     B_PCH_XHCI_PORTSCXUSB3_PEC |
                     B_PCH_XHCI_PORTSCXUSB3_CSC);
          MmioWrite32 (XhciMmioBase + PortScOffset[Index], Data32);
        }
      }
    }

    //
    // Set MMIO Offset 0x80E0[15]
    //
    MmioOr32 (XhciMmioBase + 0x80E0, BIT15);
  }
  //
  // Clear MMIO Offset 0x8154[31]
  //
  MmioAnd32 (XhciMmioBase + 0x8154, (UINT32)~BIT31);

  //
  // Restore Cmd and XhciBar and PwrSts registers
  //
  MmioAnd8 (XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER, (UINT8)~(B_PCH_XHCI_COMMAND_BME | B_PCH_XHCI_COMMAND_MSE));
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE + 4, OrgMmioHAddr);
  MmioWrite32 (XhciPciMmBase + R_PCH_XHCI_MEM_BASE, OrgMmioAddr);
  MmioWrite8 (XhciPciMmBase + R_PCH_XHCI_COMMAND_REGISTER, OrgCmdByte);

  ///
  /// Set D3hot state - 11b
  ///
  MmioOr16 ((XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS), (UINT16) 0x3);

  ///
  /// Set "PME Enable" bit of PWR_CNTL_STS register, D20:F0:74h[8] = 1h
  ///
  MmioOr16 ((XhciPciMmBase + R_PCH_XHCI_PWR_CNTL_STS), (UINT16) (B_PCH_XHCI_PWR_CNTL_STS_PME_EN));

}

/**
  When we get an SMI that indicates that we are transitioning to a sleep state,
  we need to actually transition to that state.  We do this by disabling the
  "SMI on sleep enable" feature, which generates an SMI when the operating system
  tries to put the system to sleep, and then physically putting the system to sleep.

  @param[in] None

  @retval None.
**/
VOID
PchSmmSxGoToSleep (
  VOID
  )
{
  UINT32      Pm1Cnt;
  UINT32      RootComplexBar;
  PCH_SERIES  PchSeries;

  PchSeries       = GetPchSeries();
  RootComplexBar  = PCH_RCRB_BASE;

  ///
  /// Flush cache into memory before we go to sleep. It is necessary for S3 sleep
  /// because we may update memory in SMM Sx sleep handlers -- the updates are in cache now
  ///
  AsmWbinvd ();

  ///
  /// Disable SMIs
  ///
  PchSmmClearSource (&SX_SOURCE_DESC);
  PchSmmDisableSource (&SX_SOURCE_DESC);

  ///
  /// Get Power Management 1 Control Register Value
  ///
  Pm1Cnt = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT));

  ///
  /// PCH BIOS Spec Rev 0.5.0, Section 12.10.1
  /// Additional Programming Requirements prior to enter S4/S5
  ///
  if (((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) == V_PCH_ACPI_PM1_CNT_S4) ||
      ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) == V_PCH_ACPI_PM1_CNT_S5)) {
    UsbS02SxWorkaround ();
  }

  if (((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) == V_PCH_ACPI_PM1_CNT_S3) ||
      ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) == V_PCH_ACPI_PM1_CNT_S4) ||
      ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) == V_PCH_ACPI_PM1_CNT_S5)) {
    XhciSxWorkaround ();
    ///
    /// PCH BIOS Spec Rev 0.5.0 Section 10.6
    /// Additional Internal GbE Controller special cases WOL Support
    ///
    GbES02SxWorkaround ();
    ///
    /// PCH BIOS Spec Rev 0.5.0 Section 10.6 Additional GbE based wake events
    /// The GPIO[27] is used as a wake pin when the GbE controller is enabled.
    /// The System BIOS should enable this as wake event by setting the GPIO[27]
    /// Enable bit (GP27_EN PMBASE + 2Ch[3]). Unlike other wake events the System
    /// BIOS does not need to clear the corresponding GPIO[27] Status bit
    /// (GP27_STS PMBASE + 24h[3]) as the bit will be cleared by the hardware.
    ///
    /// RCBA + 0x3334 [0] will be 1b while PchDeepSx is enabled and GP27 is
    /// reuqired to wake up the system from PchDeepSx.
    ///
    if ((MmioRead32 (RootComplexBar + 0x3334) & (UINT32) BIT0) != 0) {
      if (PchSeries == PchLp) {
        IoOr32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_GPE0_EN_127_96), (UINT32) B_PCH_ACPI_GPE0_EN_127_96_GP27);
      } else if (PchSeries == PchH) {
        IoOr32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_GPE0b_EN), (UINT32) B_PCH_ACPI_GPE0b_EN_GP27);
      }
    }
  }

  ///
  /// Record S3 suspend performance data
  ///
  if ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) == V_PCH_ACPI_PM1_CNT_S3) {
    ///
    /// Report status code before goto S3 sleep
    ///
    REPORT_STATUS_CODE (EFI_PROGRESS_CODE, PcdGet32(PcdProgressCodeS3SuspendEnd));

    ///
    /// Flush cache into memory before we go to sleep.
    ///
    AsmWbinvd ();
  }

  ///
  /// Now that SMIs are disabled, write to the SLP_EN bit again to trigger the sleep
  ///
  Pm1Cnt |= B_PCH_ACPI_PM1_CNT_SLP_EN;

  IoWrite32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT), Pm1Cnt);

  ///
  /// Should only proceed if wake event is generated.
  ///
  if ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) == V_PCH_ACPI_PM1_CNT_S1) {
    while (((IoRead16 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_STS))) & B_PCH_ACPI_PM1_STS_WAK) == 0x0);
  } else {
    CpuDeadLoop ();
  }
  ///
  /// The system just went to sleep. If the sleep state was S1, then code execution will resume
  /// here when the system wakes up.
  ///
  Pm1Cnt = IoRead32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT));

  if ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SCI_EN) == 0) {
    ///
    /// An ACPI OS isn't present, clear the sleep information
    ///
    Pm1Cnt &= ~B_PCH_ACPI_PM1_CNT_SLP_TYP;
    Pm1Cnt |= V_PCH_ACPI_PM1_CNT_S0;

    IoWrite32 ((UINTN) (mAcpiBaseAddr + R_PCH_ACPI_PM1_CNT), Pm1Cnt);
  }

  PchSmmClearSource (&SX_SOURCE_DESC);
  PchSmmEnableSource (&SX_SOURCE_DESC);
}
