/** @file
  Provide functions to initialize SATA controller and perform ACHI commands

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

@copyright
  Copyright (c) 1999 - 2012 Intel Corporation. All rights reserved
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

#include <PchAccess.h>
#include <RapidStartAhci.h>
#include <RapidStartData.h>

#include <Library/PcdLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PchPlatformLib.h>
#include <Library/RapidStartPeiLib.h>

typedef struct {
  UINT64  DataByteAddr;
  UINT32  Reserved;
  UINT32  ByteCountI;
} AHCI_PRDT;

#ifndef EFI_DEBUG
#define DumpPortStatus(a)
#else
/**
  Send serial out debug message for AHCI port status

  @param[in] Ahci  - AHCI controller information structure
**/
STATIC
VOID
DumpPortStatus (
  IN     AHCI_CONTEXT          *Ahci
  )
{
  UINTN PxBase;
  ASSERT (Ahci->Port <= 31);
  PxBase = AHCI_PORT_BASE (Ahci);

  DEBUG (
    (EFI_D_ERROR,
    " CLB:%08x   FB:%08x\n CMD:%08x  TFD:%08x  IS:%08x\nSSTS:%08x SERR:%08x\n  CI:%08x SACT:%08x SIG:%08x\n",
    MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCLB),
    MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXFB),
    MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCMD),
    MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXTFD),
    MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXIS),
    MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXSSTS),
    MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXSERR),
    MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCI),
    MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXSACT),
    MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXSIG))
    );
}
#endif

/**
  Returns SATA Port Control and Status register value.

  @retval   PCS value
**/
STATIC
UINT16
AhciGetPortControlStatus (
  VOID
  )
{
  UINTN PciD31F2RegBase;
  PciD31F2RegBase = MmPciAddress (
                      0,
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_SATA,
                      PCI_FUNCTION_NUMBER_PCH_SATA,
                      0
                      );
  return MmioRead16 (PciD31F2RegBase + R_PCH_SATA_PCS);
}

/**
  Returns SATA enabled port bitmap basing on PCS value.

  @retval Enabled ports map.
**/
UINT32
AhciGetEnabledPorts (
  VOID
  )
{
  return AhciGetPortControlStatus () & 0x7F;
}

/**
  Returns SATA present port bitmap basing on PCS value.

  @retval Present ports map.
**/
UINT32
AhciGetPresentPorts (
  VOID
  )
{
  return (AhciGetPortControlStatus () >> 8) & 0x7F;
}

/**
  This is a work-around to make sure Port Implemented bits (RWO) are set.
  PI is set according to PCS value restored from previous boot

  @param[in] Ahci            - SATA controller information structure
**/
STATIC
VOID
AhciConfigurePortsImplemented (
  IN     AHCI_CONTEXT          *Ahci
  )
{
  ASSERT (Ahci->Port <= 31);
  if ((MmioRead32 (Ahci->Abar + R_PCH_SATA_AHCI_PI) & (1 << Ahci->Port)) == 0) {
    MmioWrite32 (Ahci->Abar + R_PCH_SATA_AHCI_PI, AhciGetEnabledPorts ());
    //
    // Two reads required (C-spec ch. 25 - 1.9.14.1)
    //
    MmioRead32 (Ahci->Abar + R_PCH_SATA_AHCI_PI);
    MmioRead32 (Ahci->Abar + R_PCH_SATA_AHCI_PI);
    ASSERT (MmioRead32 (Ahci->Abar + R_PCH_SATA_AHCI_PI) & (1 << Ahci->Port));
  }
}

/**
  Initialize SATA controller and enable decode

  @param[in] Ahci  - SATA controller information structure

  @retval EFI_SUCCESS - SATA controller has been initialized successfully
**/
EFI_STATUS
AhciInit (
  IN     AHCI_CONTEXT          *Ahci
  )
{
  UINTN PciD31F2RegBase;

  DEBUG ((EFI_D_INFO, "AhciInit()\n"));

  Ahci->State    = AHCI_STATE_INIT;
  Ahci->Identify = NULL;

  ASSERT ((Ahci->Abar &~(~0 << N_PCH_SATA_AHCI_BAR_ALIGNMENT)) == 0);

  PciD31F2RegBase = MmPciAddress (
                      0,
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_SATA,
                      PCI_FUNCTION_NUMBER_PCH_SATA,
                      0
                      );

  MmioWrite32 (PciD31F2RegBase + R_PCH_SATA_AHCI_BAR, Ahci->Abar);
  ///
  /// Enable decode
  ///
  MmioWrite8 (PciD31F2RegBase + R_PCH_SATA_COMMAND, 0x6);

  DEBUG ((EFI_D_INFO, "FFS SATA BAR: %x\n", MmioRead32 (PciD31F2RegBase + R_PCH_SATA_AHCI_BAR)));
  DEBUG ((EFI_D_INFO, "FFS SATA ID: %x\n", MmioRead32 (PciD31F2RegBase + 0)));
  DEBUG ((EFI_D_INFO, "FFS SATA CMD: %x\n", MmioRead16 (PciD31F2RegBase + R_PCH_SATA_COMMAND)));
  DEBUG ((EFI_D_INFO, "FFS SATA STS: %x\n", MmioRead16 (PciD31F2RegBase + R_PCH_SATA_PCISTS)));
  DEBUG ((EFI_D_INFO, "FFS SATA PI: %x\n", MmioRead8 (PciD31F2RegBase + R_PCH_SATA_PI_REGISTER)));
  DEBUG ((EFI_D_INFO, "FFS SATA CC: %x\n", MmioRead8 (PciD31F2RegBase + R_PCH_SATA_SUB_CLASS_CODE)));

  ///
  /// Enable SATA ports if not enabled already.
  ///
  AhciConfigurePortsImplemented (Ahci);

  ///
  /// Enable AHCI by setting AE bit
  ///
  MmioWrite32 (Ahci->Abar + R_PCH_SATA_AHCI_GHC, B_PCH_SATA_AHCI_GHC_AE);

  DEBUG ((EFI_D_INFO, "FFS AHCI VS: %x\n", MmioRead32 (Ahci->Abar + R_PCH_SATA_AHCI_VS)));
  DEBUG ((EFI_D_INFO, "FFS AHCI GHC: %x\n", MmioRead32 (Ahci->Abar + R_PCH_SATA_AHCI_GHC)));
  DEBUG ((EFI_D_INFO, "FFS AHCI CAP: %x\n", MmioRead32 (Ahci->Abar + R_PCH_SATA_AHCI_CAP)));
  DEBUG ((EFI_D_INFO, "FFS AHCI  PI: %x\n", MmioRead32 (Ahci->Abar + R_PCH_SATA_AHCI_PI)));

  return EFI_SUCCESS;
}

/**
  Disable SATA controller after all AHCI commands have completed

  @param[in] Ahci  - SATA controller information structure
**/
VOID
AhciDone (
  IN     AHCI_CONTEXT          *Ahci
  )
{
  UINTN PciD31F2RegBase;

  DEBUG ((EFI_D_INFO, "AhciDone()\n"));
  ASSERT (Ahci->State == AHCI_STATE_INIT);
  Ahci->State = AHCI_STATE_UNKNOWN;

  PciD31F2RegBase = MmPciAddress (
                      0,
                      DEFAULT_PCI_BUS_NUMBER_PCH,
                      PCI_DEVICE_NUMBER_PCH_SATA,
                      PCI_FUNCTION_NUMBER_PCH_SATA,
                      0
                      );

  ///
  /// Disable AHCI
  ///
  MmioWrite32 (Ahci->Abar + R_PCH_SATA_AHCI_GHC, 0);

  ///
  /// Unconfigure SATA
  ///
  MmioWrite32 (PciD31F2RegBase + R_PCH_SATA_AHCI_BAR, 0);
  MmioWrite8 (PciD31F2RegBase + R_PCH_SATA_COMMAND, 0);
}

/**
  Spin up Ahci port

  @param[in] Ahci  - SATA controller information structure
  @param[in] Port  - SATA port number

  @retval EFI_SUCCESS            - AHCI Port has been spun up successfully
**/
EFI_STATUS
AhciSpinUpPort (
  IN     AHCI_CONTEXT          *Ahci,
  IN     UINTN                 Port
  )
{
  UINTN PxBase;

  DEBUG ((EFI_D_INFO, "AhciPortSpinUpPort(%d)\n", Port));
  ASSERT (Ahci->State == AHCI_STATE_INIT);
  ASSERT (Port <= 31);
  ASSERT (MmioRead32 (Ahci->Abar + R_PCH_SATA_AHCI_GHC) & B_PCH_SATA_AHCI_GHC_AE);

  PxBase = AHCI_PORT_BASE_X (Ahci, Port);

  ASSERT ((MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCMD) & B_PCH_SATA_AHCI_PxCMD_ST) == 0);

  ///
  /// Clear errors
  ///
  MmioWrite32 (PxBase + R_PCH_SATA_AHCI_PXSERR, (UINT32)~0u);

  ///
  /// Spin Up
  ///
  MmioOr32 (PxBase + R_PCH_SATA_AHCI_PXCMD, B_PCH_SATA_AHCI_PxCMD_SUD);

  return EFI_SUCCESS;
}

/**
  Initialize AHCI port for reading/writing RapidStart Store

  @param[in] Ahci  - SATA controller information structure

  @retval EFI_DEVICE_ERROR       - AHCI port initialization failed
  @retval EFI_SUCCESS            - AHCI port initialized successfully and RapidStart Store is ready for reading/writing
  @retval EFI_SECURITY_VIOLATION - Drive is already in SECURITY FREEZE state and will not accept UNLOCK command
  @retval EFI_ACCESS_DENIED      - Drive is in LOCKED state and need to execute UNLOCK command before accessing
**/
EFI_STATUS
AhciPortInit (
  IN     AHCI_CONTEXT          *Ahci
  )
{
  UINTN       PxBase;
  UINT32      Timer;
  UINT32      Count;

  DEBUG ((EFI_D_INFO, "AhciPortInit()\n"));
  ASSERT (Ahci->State == AHCI_STATE_INIT);
  ASSERT (Ahci->Port <= 31);
  ASSERT (MmioRead32 (Ahci->Abar + R_PCH_SATA_AHCI_GHC) & B_PCH_SATA_AHCI_GHC_AE);
  DEBUG ((EFI_D_INFO, "Mem used: %d\n", AHCI_MEM_MAX (Ahci) - Ahci->PortBase));
  DEBUG ((EFI_D_INFO, "AHCI_MEM_MAX_SIZE: %d\n", AHCI_MEM_MAX_SIZE));

  ASSERT (AHCI_MEM_MAX (Ahci) <= (Ahci->PortBase + Ahci->PortSize));
  ASSERT ((AHCI_MEM_MAX (Ahci) - Ahci->PortBase) == AHCI_MEM_MAX_SIZE);

  PxBase = AHCI_PORT_BASE (Ahci);

  ASSERT ((MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCMD) & B_PCH_SATA_AHCI_PxCMD_ST) == 0);

  ZeroMem ((VOID *) Ahci->PortBase, Ahci->PortSize);

  DEBUG ((EFI_D_INFO, "AHCI_CMD_LIST_BASE: %08lx\n", (UINT64) AHCI_CMD_LIST_BASE (Ahci)));
  DEBUG ((EFI_D_INFO, "AHCI_RXFIS_BASE: %08lx\n", (UINT64) AHCI_RXFIS_BASE (Ahci)));
  DEBUG ((EFI_D_INFO, "AHCI_CMD_TABLE_BASE: %08lx\n", (UINT64) AHCI_CMD_TABLE_BASE (Ahci)));
  DEBUG ((EFI_D_INFO, "AHCI_MEM_MAX: %08lx\n", (UINT64) AHCI_MEM_MAX (Ahci)));

  ASSERT ((AHCI_CMD_LIST_BASE (Ahci) & 0x3FF) == 0);
  ASSERT ((AHCI_RXFIS_BASE (Ahci) & 0xFF) == 0);

  MmioWrite32 (PxBase + R_PCH_SATA_AHCI_PXCLB, (UINT32) AHCI_CMD_LIST_BASE (Ahci));
  MmioWrite32 (PxBase + R_PCH_SATA_AHCI_PXCLBU, (UINT32) RShiftU64 (AHCI_CMD_LIST_BASE (Ahci), 32));

  MmioWrite32 (PxBase + R_PCH_SATA_AHCI_PXFB, (UINT32) AHCI_RXFIS_BASE (Ahci));
  MmioWrite32 (PxBase + R_PCH_SATA_AHCI_PXFBU, (UINT32) RShiftU64 (AHCI_RXFIS_BASE (Ahci), 32));

  MmioOr32 (PxBase + R_PCH_SATA_AHCI_PXCMD, B_PCH_SATA_AHCI_PxCMD_FRE);

  ASSERT (MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCMD) & B_PCH_SATA_AHCI_PxCMD_SUD);

  ///
  /// Clear errors
  ///
  MmioWrite32 (PxBase + R_PCH_SATA_AHCI_PXSERR, (UINT32)~0u);

  for (Count = 0; Count < AHCI_INIT_RETRY_COUNT; ++Count) {
    ///
    /// Reset port if in error state
    ///
    if (Count > 0 || MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXSERR) != 0) {
      DEBUG ((EFI_D_INFO, "Resetting AHCI port.\n"));
      MmioWrite32 (PxBase + R_PCH_SATA_AHCI_PXSCTL, V_PCH_SATA_AHCI_PXSCTL_DET_1);
      PchPmTimerStall (1000);
      MmioWrite32 (PxBase + R_PCH_SATA_AHCI_PXSCTL, 0);
    }

    Timer = 0;

    ///
    /// Wait till device detected
    ///
    while
    (
      Timer < AHCI_INIT_TIMEOUT &&
      ((MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXSSTS) & B_PCH_SATA_AHCI_PXSSTS_DET) != B_PCH_SATA_AHCI_PXSSTS_DET_3)
    ) {
      PchPmTimerStall (AHCI_INIT_WAIT);
      Timer += AHCI_INIT_WAIT;
    }
    ///
    /// Clear errors
    ///
    MmioWrite32 (PxBase + R_PCH_SATA_AHCI_PXSERR, (UINT32)~0u);
    MmioWrite32 (PxBase + R_PCH_SATA_AHCI_PXIS, (UINT32)~0u);

    ///
    /// Wait for device ready
    ///
    while (Timer < AHCI_INIT_TIMEOUT) {
      if ((
            MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXTFD) & (B_PCH_SATA_AHCI_PXTFD_STS_DRQ | B_PCH_SATA_AHCI_PXTFD_STS_BSY)
        ) == 0
          ) {
        goto complete;
      }

      PchPmTimerStall (AHCI_INIT_WAIT);
      Timer += AHCI_INIT_WAIT;
    }
  }

complete:

  if (Count == AHCI_INIT_RETRY_COUNT) {

    DEBUG ((EFI_D_ERROR, "AHCI port initialization timeout.\n"));
    DumpPortStatus (Ahci);

    ///
    /// Clear FRE
    ///
    MmioAnd32 (PxBase + R_PCH_SATA_AHCI_PXCMD, (UINT32)~B_PCH_SATA_AHCI_PxCMD_FRE);

    ///
    /// Wait for FR to be cleared
    ///
    while (MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCMD) & B_PCH_SATA_AHCI_PxCMD_FR) {
    }

    return EFI_DEVICE_ERROR;
  }

  ASSERT ((MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCMD) & (B_PCH_SATA_AHCI_PxCMD_CLO | B_PCH_SATA_AHCI_PxCMD_CR)) == 0);

  ///
  /// Set Start bit
  ///
  MmioOr32 (PxBase + R_PCH_SATA_AHCI_PXCMD, B_PCH_SATA_AHCI_PxCMD_ST);

  Ahci->State = AHCI_STATE_INUSE;
  return EFI_SUCCESS;
}

/**
  Check whether device is password locked.

  @param[in] Ahci  - SATA controller information structure

  @retval EFI_SUCCESS            - Device not locked
  @retval EFI_ACCESS_DENIED      - Drive is in LOCKED state and need to execute UNLOCK command before accessing
  @retval EFI_SECURITY_VIOLATION - Drive is already in SECURITY FREEZE state and will not accept UNLOCK command
  @retval EFI_TIMEOUT            - Timeout occured
  @retval EFI_DEVICE_ERROR       - Error occurred on device side.
**/
EFI_STATUS
AhciGetLockStatus (
  IN     AHCI_CONTEXT          *Ahci
)
{
  UINT16      SecurityStatus;
  EFI_STATUS  Status;
  VOID        *Buffer;

  DEBUG ((EFI_D_INFO, "AhciGetLockStatus()\n"));
  ASSERT (Ahci->State == AHCI_STATE_INUSE);

  if (Ahci->Identify == 0) {
    Buffer = (VOID*) AHCI_ID_BLOCK (Ahci);
    Status = AhciIdentifyDevice (Ahci, Buffer);
    if (Status == EFI_SUCCESS) {
      Ahci->Identify  = (UINT16 *) Buffer;
    } else {
      DEBUG ((EFI_D_ERROR, "Error: Identifying device failed!\n"));
      return Status;
    }
  }
  SecurityStatus = Ahci->Identify[ATA_ID_DEV_SECURITY_STATUS];
  if ((SecurityStatus & (B_ATA_ID_DEV_SEC_SUPPORTED | B_ATA_ID_DEV_SEC_ENABLED | B_ATA_ID_DEV_SEC_LOCKED)
      ) == (B_ATA_ID_DEV_SEC_SUPPORTED | B_ATA_ID_DEV_SEC_ENABLED | B_ATA_ID_DEV_SEC_LOCKED)
  ) {
    if (SecurityStatus & (B_ATA_ID_DEV_SEC_FROZEN | B_ATA_ID_DEV_SEC_COUNT_EXP)) {
      DEBUG ((EFI_D_ERROR, "AHCI device lock freeze!\n"));
      return EFI_SECURITY_VIOLATION;
    }
    DEBUG ((EFI_D_WARN, "AHCI device locked!\n"));
    return EFI_ACCESS_DENIED;
  }
  return EFI_SUCCESS;
}

/**
  Stops AHCI port.

  @param[in] Ahci  - SATA controller information structure
**/
VOID
AhciPortDone (
  IN     AHCI_CONTEXT          *Ahci
  )
{
  UINTN PxBase;

  DEBUG ((EFI_D_INFO, "AhciPortDone()\n"));
  Ahci->State = AHCI_STATE_INIT;

  PxBase = AHCI_PORT_BASE (Ahci);
  ASSERT (MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCI) == 0);

  MmioAnd32 (PxBase + R_PCH_SATA_AHCI_PXCMD, (UINT32)~B_PCH_SATA_AHCI_PxCMD_ST);

  ///
  /// Wait for CR to be cleared
  ///
  while (MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCMD) & B_PCH_SATA_AHCI_PxCMD_CR) {
  }

  MmioAnd32 (PxBase + R_PCH_SATA_AHCI_PXCMD, (UINT32)~B_PCH_SATA_AHCI_PxCMD_FRE);

  ///
  /// Wait for FR to be cleared
  ///
  while (MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCMD) & B_PCH_SATA_AHCI_PxCMD_FR) {
  }
}

/**
  Check whether there are available command slots.

  @param[in] Ahci      - SATA controller information structure

  @retval TRUE if there is an available slot, FALSE otherwise.
**/
BOOLEAN
AhciHasFreeCmdSlot (
  IN     AHCI_CONTEXT          *Ahci
  )
{
  UINTN   PxBase;
  UINT32  ci;

  ASSERT (AHCI_MAX_CMD <= 32);

  PxBase  = AHCI_PORT_BASE (Ahci);
  ci      = MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCI);

  return ci != ((UINT32)~0u >> (32 - AHCI_MAX_CMD));
}

/**
  Finds or waits for available command slot

  @param[in] Ahci       - SATA controller information structure
  @param[out] CmdIndex  - Available command slot index

  @retval EFI_SUCCESS      - Available command slot index found
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Error occurred on device side.
  @retval EFI_NOT_STARTED  - The RapidStart Entry flow should be canceled and do S3 resume back to OS
**/
STATIC
EFI_STATUS
AhciFindCmdSlot (
  IN     AHCI_CONTEXT          *Ahci,
  OUT    UINTN                 *CmdIndex
  )
{
  UINTN   PxBase;
  UINT32  ci;
  UINT32  Timer;
  UINT32  Index;

  PxBase  = AHCI_PORT_BASE (Ahci);

  Timer   = 0;
  while (Timer < AHCI_CMD_TIMEOUT) {
    ci = MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCI);

    for (Index = 0; Index < AHCI_MAX_CMD; ++Index) {
      if ((ci & (1u << Index)) == 0) {
        *CmdIndex = Index;
        return EFI_SUCCESS;
      }
    }

    if (Ahci->PollCancellation && RapidStartShouldCancelEntry ()) {
      return EFI_NOT_STARTED;
    }

    if (AHCI_ERROR (PxBase)) {
      DEBUG ((EFI_D_ERROR, "AHCI error!\n"));
      DumpPortStatus (Ahci);
      return EFI_DEVICE_ERROR;
    }

    PchPmTimerStall (AHCI_CMD_WAIT);
    Timer += AHCI_CMD_WAIT;
  }

  DEBUG ((EFI_D_ERROR, "AHCI command timeout!\n"));
  DumpPortStatus (Ahci);
  return EFI_TIMEOUT;
}

/**
  Waits until given command(s) complete.

  @param[in] Ahci      - SATA controller information structure
  @param[in] CmdMask   - Command(s) mask

  @retval EFI_SUCCESS      - Command complete
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Error occurred on device side.
  @retval EFI_NOT_STARTED  - The RapidStart Entry flow should be canceled and do S3 resume back to OS
**/
EFI_STATUS
AhciWaitComplete (
  IN     AHCI_CONTEXT          *Ahci,
  IN     UINT32                CmdMask
  )
{
  UINTN   PxBase;
  UINT32  ci;
  UINT32  Timer;

  DEBUG ((EFI_D_INFO, "AhciWaitComplete(%d)\n", CmdMask));
  ASSERT (Ahci->State == AHCI_STATE_INUSE);

  PxBase  = AHCI_PORT_BASE (Ahci);

  Timer   = 0;
  while (Timer < AHCI_CMD_TIMEOUT) {
    ci = MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCI);

    if ((ci & CmdMask) == 0) {
      return EFI_SUCCESS;
    }

    if (Ahci->PollCancellation && RapidStartShouldCancelEntry ()) {
      return EFI_NOT_STARTED;
    }

    if (AHCI_ERROR (PxBase)) {
      DEBUG ((EFI_D_ERROR, "AHCI error!\n"));
      DumpPortStatus (Ahci);
      return EFI_DEVICE_ERROR;
    }

    PchPmTimerStall (AHCI_CMD_WAIT);
    Timer += AHCI_CMD_WAIT;
  }

  DEBUG ((EFI_D_ERROR, "AHCI command timeout!\n"));
  DumpPortStatus (Ahci);
  return EFI_TIMEOUT;
}

/**
  Waits until all AHCI commands completed.

  @param[in] Ahci  - SATA controller information structure

  @retval EFI_SUCCESS      - All AHCI commands have completed
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Error occurred on device side.
  @retval EFI_NOT_STARTED  - The RapidStart Entry flow should be canceled and do S3 resume back to OS
**/
EFI_STATUS
AhciWaitAllComplete (
  IN     AHCI_CONTEXT          *Ahci
  )
{
  UINTN       PxBase;
  UINT32      Ci;
  UINT32      PrevCi;
  EFI_STATUS  Status;
  UINT32      Timer;

  DEBUG ((EFI_D_INFO, "AhciWaitAllComplete() "));
  ASSERT (Ahci->State == AHCI_STATE_INUSE);

  PxBase  = AHCI_PORT_BASE (Ahci);
  Timer   = 0;
  Status  = EFI_TIMEOUT;

  Ci      = MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCI);
  PrevCi  = Ci;

  while (Timer < AHCI_CMD_TIMEOUT) {
    if (Ci == 0) {
      Status = EFI_SUCCESS;
      break;
    }

    if (Ahci->PollCancellation && RapidStartShouldCancelEntry ()) {
      return EFI_NOT_STARTED;
    }

    if (AHCI_ERROR (PxBase)) {
      Status = EFI_DEVICE_ERROR;
      break;
    }

    if (Ci != PrevCi) {
      DEBUG ((EFI_D_INFO, "."));
      Timer = 0;
    }

    PchPmTimerStall (AHCI_CMD_WAIT);
    Timer += AHCI_CMD_WAIT;
    PrevCi  = Ci;
    Ci      = MmioRead32 (PxBase + R_PCH_SATA_AHCI_PXCI);
  }

  DEBUG ((EFI_D_INFO, "\n"));
  if (Status == EFI_TIMEOUT) {
    DEBUG ((EFI_D_ERROR, "AHCI command timeout!\n"));
  } else if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AHCI error!\n"));
    DumpPortStatus (Ahci);
  }

  return Status;
}

/**
  Enqueues ahci command for read/write and optionally with Zero filter

  @param[in] Ahci            - SATA controller information structure
  @param[in,out] AtaCmd     - ATA command structure
  @param[out] CmdMask        - If given a bit corresponding to allocated command slot is set
  @param[in] ZeroPageBitMap  - A pointer for ZeroPageTable
  @param[out] SmRamBufferLba - If given the drive LBA storing original SMRAM buffer content will be passed by it.

  @retval EFI_SUCCESS      - AHCI command successfully posted
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Command failed
**/
EFI_STATUS
AhciPostCommandWithZeroFilter (
  IN     AHCI_CONTEXT          *Ahci,
  IN OUT ATA_COMMAND           *AtaCmd,
  OUT    OPTIONAL UINT32                *CmdMask,
  IN     OPTIONAL UINT32                *ZeroPageBitMap,
  OUT    OPTIONAL UINT64                *SmRamBufferLba
  )
{
  UINTN       PxBase;
  UINTN       CmdIndex;
  UINTN       CmdTableAddr;
  UINT32      *CmdHeader;
  UINT32      *CmdFis;
  AHCI_PRDT   *Prdt;
  UINT16      PrdCount;
  UINT16      Index;
  UINT32      MemLeft;
  UINT32      MemChunk;
  UINT64      MemAddr;
  EFI_STATUS  Status;
  UINT32      TempBitMap;
  UINT32      *ZeroPagePointer;
  UINT8       count;
  UINT32      AhciMaxRegion;
  UINT32      NumberOfPages;
  UINT32      SectorCount;

  count           = 0;
  ZeroPagePointer = 0;
  TempBitMap      = 0;
  CmdIndex        = 0;

  DEBUG (
    (EFI_D_INFO,
    "AhciPostCommand port=%d cmd=%02x lba=%08x mem=%08lx ",
    Ahci->Port,
    AtaCmd->Command,
    (UINT32) AtaCmd->Lba,
    (UINT64) AtaCmd->MemAddr)
    );
  ASSERT (Ahci->State == AHCI_STATE_INUSE);

  MemAddr = AtaCmd->MemAddr;
  MemLeft = AtaCmd->SectorCount << SECTOR_SHIFT;
  PxBase  = AHCI_PORT_BASE (Ahci);

  ASSERT ((MemAddr & (ZeroPageBitMap ? 0xFFF : 1)) == 0);
  ASSERT (AtaCmd->SectorCount <= AHCI_MAX_SECTORS);

  Status = AhciFindCmdSlot (Ahci, &CmdIndex);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (CmdIndex < AHCI_MAX_CMD);

  CmdHeader     = (UINT32 *) AHCI_CMD_HEADER (Ahci, CmdIndex);
  CmdTableAddr  = AHCI_CMD_TABLE (Ahci, CmdIndex);
  CmdFis        = (UINT32 *) (UINTN) CmdTableAddr;
  Prdt          = (AHCI_PRDT *) (UINTN) (CmdTableAddr + AHCI_CMD_TABLE_HEADER_SIZE);

  ASSERT ((UINT32) CmdHeader >= AHCI_CMD_LIST_BASE (Ahci));
  ASSERT ((UINT32) CmdHeader + AHCI_CMD_HEADER_SIZE <= AHCI_CMD_LIST_BASE (Ahci) + AHCI_CMD_LIST_SIZE);
  ASSERT (CmdTableAddr >= Ahci->PortBase);
  ASSERT (CmdTableAddr + AHCI_CMD_TABLE_SIZE <= Ahci->PortBase + Ahci->PortSize);
  ASSERT (RShiftU64 (AtaCmd->Lba, 48) == 0);

  ///
  /// Create PRD table
  ///
  PrdCount      = 0;
  AhciMaxRegion = AHCI_REGION_MAX;
  SectorCount   = 0;
  ///
  /// Initialize ZeroPage Filter Parameters
  ///
  if (ZeroPageBitMap != NULL) {
    NumberOfPages   = (UINT32) NUMBER_OF_PAGES_IN_DWORD (MemAddr);
    ZeroPagePointer = ZeroPageBitMap + NumberOfPages;
    count           = (UINT8) (NUMBER_OF_PAGES (MemAddr) - (NumberOfPages * ZERO_BITMAP_UNIT));
    TempBitMap      = (UINT32) *ZeroPagePointer;
    AhciMaxRegion   = EFI_PAGE_SIZE;
  }

  while (MemLeft > 0) {
    ASSERT (PrdCount < AHCI_MAX_PRDT);
    ASSERT ((MemAddr & 1) == 0);

    if (MemLeft >= AhciMaxRegion) {
      MemChunk = AhciMaxRegion;
    } else {
      MemChunk = MemLeft;
    }

    ASSERT (((MemChunk - 1) &~(AhciMaxRegion - 1)) == 0);
    ASSERT ((MemChunk & 0x1FF) == 0);

    if (ZeroPageBitMap != NULL) {
      ///
      /// Get LBA which storing SMRAM buffer and later restore this buffer after SMRAM handled.
      ///
      if (SmRamBufferLba != NULL) {
        if (MemAddr == LEGACY_SMRAM_BUFFER) {
          *SmRamBufferLba = AtaCmd->Lba + SectorCount;
        }
      }

      if (((UINT32) (TempBitMap >> count) & 1) == 0) {
        Prdt[PrdCount].DataByteAddr = MemAddr;
        Prdt[PrdCount].Reserved     = 0;
        Prdt[PrdCount].ByteCountI   = MemChunk - 1;
        PrdCount++;
        SectorCount += 8;
      }
      ///
      /// Shift ZeroPage filter bitmap to next bit, if it is end of current DWORD, will shift to next DWORD
      ///
      if (count < (ZERO_BITMAP_UNIT - 1)) {
        count++;
      } else {
        count = 0;
        ZeroPagePointer++;
        TempBitMap = (UINT32) *ZeroPagePointer;
      }
    } else {
      Prdt[PrdCount].DataByteAddr = MemAddr;
      Prdt[PrdCount].Reserved     = 0;
      Prdt[PrdCount].ByteCountI   = MemChunk - 1;
      PrdCount++;
      SectorCount += MemChunk >> SECTOR_SHIFT;
    }

    MemAddr += MemChunk;
    MemLeft -= MemChunk;
  }

  AtaCmd->MemAddr += AtaCmd->SectorCount << SECTOR_SHIFT;
  AtaCmd->SectorCount = SectorCount;

  if (ZeroPageBitMap != NULL && PrdCount == 0) {
    DEBUG ((EFI_D_INFO, "skipped\n"));
    return EFI_SUCCESS;
  }
  ///
  /// Fill command header
  ///
  ASSERT ((CmdTableAddr & 0x7F) == 0);
  CmdHeader[0] = AHCI_CMD_PRDTL (PrdCount) | AHCI_CMD_CFL (5);
  if (AtaCmd->Direction == AHCI_DIR_HOST2DEV) {
    CmdHeader[0] |= AHCI_CMD_WRITE | AHCI_CMD_PREFETCHABLE;
  }

  CmdHeader[1]  = 0;
  CmdHeader[2]  = (UINT32) CmdTableAddr;
  CmdHeader[3]  = (UINT32) RShiftU64 (CmdTableAddr, 32);
  for (Index = 4; Index < 8; ++Index) {
    CmdHeader[Index] = 0;
  }
  ///
  /// Create FIS
  ///
  CmdFis[0] = SATA_FIS_FEAT (AtaCmd->Feature) | SATA_FIS_CMD (AtaCmd->Command) | SATA_FIS_C | SATA_FIS_HOST2DEVICE;
  CmdFis[1] = SATA_FIS_DEV_LBA | SATA_FIS_LBA (AtaCmd->Lba);
  CmdFis[2] = SATA_FIS_FEAT_EXP (AtaCmd->Feature) | SATA_FIS_LBA_EXP (AtaCmd->Lba);
  CmdFis[3] = SATA_FIS_SECT_COUNT (SectorCount);
  CmdFis[4] = AtaCmd->Auxiliary.Data;
  DEBUG ((EFI_D_INFO, "FIS[4] = %x\n", AtaCmd->Auxiliary.Data));

  DEBUG ((EFI_D_INFO, "actualcount=%d ", SectorCount));

  ///
  /// Issue the command to the controller
  ///
  MmioWrite32 (PxBase + R_PCH_SATA_AHCI_PXCI, (1u << CmdIndex));

  if (CmdMask != NULL) {
    *CmdMask |= (1u << CmdIndex);
  }

  AtaCmd->Lba += SectorCount;
  DEBUG ((EFI_D_INFO, "idx=%d\n", CmdIndex));

  return EFI_SUCCESS;
}

/**
  Executes AHCI command and waits until command completes.

  @param[in] Ahci          - SATA controller information structure
  @param[in,out] AtaCmd   - ATA command structure

  @retval EFI_SUCCESS      - AHCI command successfully posted
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Command failed
**/
EFI_STATUS
AhciExecCommand (
  IN     AHCI_CONTEXT          *Ahci,
  IN OUT ATA_COMMAND           *AtaCmd
  )
{
  EFI_STATUS  Status;
  UINTN       CmdMask;

  CmdMask = 0;
  Status  = AhciPostCommand (Ahci, AtaCmd, &CmdMask);
  if (Status == EFI_SUCCESS) {
    Status = AhciWaitComplete (Ahci, CmdMask);
  }

  return Status;
}

/**
  Issues ATA command with no data transfer.

  @param[in] Ahci    - SATA controller information structure
  @param[in] Command - Command to be issued

  @retval EFI_SUCCESS      - fucntion executed successfully
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Command failed
**/
EFI_STATUS
AhciSimpleCommand (
  IN     AHCI_CONTEXT          *Ahci,
  IN     UINT8                 Command
  )
{
  ATA_COMMAND AtaCmd;

  ZeroMem (&AtaCmd, sizeof (AtaCmd));
  AtaCmd.Command = Command;

  return AhciExecCommand (Ahci, &AtaCmd);
}

/**
  Sends Identify Device ATA command to port

  @param[in] Ahci     - SATA controller information structure
  @param[out] Buffer  - Buffer to store device information

  @retval EFI_SUCCESS      - function executed successfully
  @retval EFI_TIMEOUT      - Timeout occured
  @retval EFI_DEVICE_ERROR - Command failed
**/
EFI_STATUS
AhciIdentifyDevice (
  IN     AHCI_CONTEXT          *Ahci,
  OUT    VOID                  *Buffer
  )
{
  ATA_COMMAND AtaCmd;

  DEBUG ((EFI_D_INFO, "AhciIdentifyDevice()\n"));
  ASSERT (Buffer != NULL);

  AtaCmd.Command      = ATA_CMD_IDENTIFY_DEVICE;
  AtaCmd.Direction    = AHCI_DIR_DEV2HOST;
  AtaCmd.Lba          = 0;
  AtaCmd.SectorCount  = 1;
  AtaCmd.MemAddr      = (UINTN) Buffer;
  AtaCmd.Feature      = 0;

  return AhciExecCommand (Ahci, &AtaCmd);
}

/**
  Performs TRIM command on given LBA range.

  @param[in] Ahci  - SATA controller information structure
  @param[in] Lba   - First block to be trimmed
  @param[in] Count - Number of blocks to trim

  @retval EFI_SUCCESS      - fucntion executed successfully
  @retval EFI_TIMEOUT      - Timeout occured
  @exception EFI_UNSUPPORTED  - TRIM is not supported by the device
  @retval EFI_DEVICE_ERROR - Command failed
**/
EFI_STATUS
AhciTrimRange (
  IN     AHCI_CONTEXT          *Ahci,
  IN     UINT64                Lba,
  IN     UINT32                Count
  )
{
  UINT64      *RangeEntries;
  UINTN       Index;
  UINT16      Blocks;
  ATA_COMMAND AtaCmd;
  EFI_STATUS  Status;

  DEBUG ((EFI_D_INFO, "AhciTrimRange(lba=%lx count=%x)\n", Lba, Count));
  ASSERT (Ahci->Identify != NULL);

  if (!((Ahci->Identify[ATA_ID_DEV_DATA_SET_MGMNT_SUPPORT] & B_ATA_ID_DEV_DATA_SET_TRIM) &&
        (Ahci->Identify[ATA_ID_DEV_DATA_SET_MGMNT_BLOCKS] > 0))
      ) {
    DEBUG ((EFI_D_WARN, "TRIM is not supported!\n"));
    return EFI_UNSUPPORTED;
  }

  RangeEntries        = (UINT64 *) AHCI_TMP_BLOCK (Ahci);

  AtaCmd.Command      = ATA_CMD_DATA_SET_MANAGEMENT;
  AtaCmd.Direction    = AHCI_DIR_HOST2DEV;
  AtaCmd.Feature      = V_ATA_TRIM_FEATURE;
  AtaCmd.SectorCount  = 1;

  while (Count > 0) {
    for (Index = 0; Index < (AHCI_TMP_BLOCK_SIZE / sizeof (UINT64)); ++Index) {
      if (Count <= 0xFFFF) {
        Blocks = (UINT16) Count;
      } else {
        Blocks = 0xFFFF;
      }

      if (Blocks != 0) {
        RangeEntries[Index] = LShiftU64 (Blocks, 48) | Lba;
      } else {
        RangeEntries[Index] = 0;
      }

      Count -= Blocks;
      Lba += Blocks;
    }

    AtaCmd.Lba      = 0;
    AtaCmd.MemAddr  = (UINTN) RangeEntries;

    Status          = AhciExecCommand (Ahci, &AtaCmd);
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

/**
  Unlocks ATA device.

  @param[in] Ahci     - SATA controller information structure
  @param[in] Password - security credential

  @retval EFI_SUCCESS            - Command executed successfully
  @retval EFI_SECURITY_VIOLATION - Device security lock freeze
  @exception EFI_UNSUPPORTED        - Security is not supported by the device
  @retval EFI_TIMEOUT            - Timeout occured
  @retval EFI_DEVICE_ERROR       - Command failed
**/
EFI_STATUS
AhciSecurityUnlock (
  IN     AHCI_CONTEXT          *Ahci,
//[-start-111024-IB04770172-modify]//
  IN     CHAR8                 *Password,
  IN     UINT16                PasswordType
//[-end-111024-IB04770172-modify]//
  )
{
  UINT16      *Block;
  ATA_COMMAND AtaCmd;
  EFI_STATUS  Status;

  DEBUG ((EFI_D_INFO, "AhciSecurityUnlock()\n"));
  ASSERT (Ahci->Identify != NULL);

  if ((Ahci->Identify[ATA_ID_DEV_SECURITY_STATUS] & B_ATA_ID_DEV_SEC_SUPPORTED) == 0) {
    DEBUG ((EFI_D_ERROR, "AHCI security not supported!\n"));
    return EFI_UNSUPPORTED;
  }

  if ((Ahci->Identify[ATA_ID_DEV_SECURITY_STATUS] & (B_ATA_ID_DEV_SEC_ENABLED | B_ATA_ID_DEV_SEC_LOCKED)) !=
        (B_ATA_ID_DEV_SEC_ENABLED | B_ATA_ID_DEV_SEC_LOCKED)
        ) {
    DEBUG ((EFI_D_WARN, "AHCI device already unlocked!\n"));
    return EFI_SUCCESS;
  }

  if (Ahci->Identify[ATA_ID_DEV_SECURITY_STATUS] & (B_ATA_ID_DEV_SEC_FROZEN | B_ATA_ID_DEV_SEC_COUNT_EXP)) {
    DEBUG ((EFI_D_ERROR, "AHCI device lock freeze!\n"));
    return EFI_SECURITY_VIOLATION;
  }

  Block     = (UINT16 *) AHCI_TMP_BLOCK (Ahci);
//[-start-111021-IB04770172-modify]//
  Block[0] = PasswordType;
//[-end-111021-IB04770172-modify]//
  CopyMem (&Block[1], Password, ATA_PASSWORD_LEN);

  AtaCmd.Command      = ATA_CMD_SECURITY_UNLOCK;
  AtaCmd.Direction    = AHCI_DIR_HOST2DEV;
  AtaCmd.Feature      = 0;
  AtaCmd.Lba          = 0;
  AtaCmd.MemAddr      = (UINTN) Block;
  AtaCmd.SectorCount  = 1;

  Status              = AhciExecCommand (Ahci, &AtaCmd);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "AHCI unlock failed!\n"));
  }

  return Status;
}

/**
  Hybrid Hard Disk Support.

  @param[in] Ahci  - SATA controller information structure

  @retval EFI_SUCCESS      - fucntion executed successfully
  @retval EFI_TIMEOUT      - Timeout occured
  @exception EFI_UNSUPPORTED  - HHD is not supported by the device
  @retval EFI_DEVICE_ERROR - Command failed
**/
EFI_STATUS
AhciHybridHardDiskSupport (
  IN     AHCI_CONTEXT          *Ahci
  )
{
  EFI_STATUS            Status;
  ATA_COMMAND           AtaCmd;
  UINT8                 *HybridLogRangeEntries;
  UINT8                 *HybridInfoHeader;
  UINT8                 *HybridInfoDescriptor;
  UINT16                Index, HybridInfoDescriptorNumber;
  UINT64                X, Y;
  UINT64                NvmSize;

  DEBUG ((EFI_D_INFO, "Ahci Hybrid Hard Device Support Start\n"));
  ASSERT (Ahci->Identify != NULL);

  if (!((Ahci->Identify[ATA_ID_DEV_HYBRID_FEATURE_SUPPORT] & B_ATA_ID_DEV_HYBRID_FEATURE_SUPPORT) &
        (Ahci->Identify[ATA_ID_DEV_HYBRID_FEATURE_ENABLE] & B_ATA_ID_DEV_HYBRID_FEATURE_ENABLE))
     ) {
    DEBUG ((EFI_D_ERROR, "Hybrid is not supported!\n"));
    DEBUG ((EFI_D_ERROR, "Hybrid Feature support = %x\n", Ahci->Identify[ATA_ID_DEV_HYBRID_FEATURE_SUPPORT]));
    DEBUG ((EFI_D_ERROR, "Hybrid Feature Enable = %x\n", Ahci->Identify[ATA_ID_DEV_HYBRID_FEATURE_ENABLE]));

    return EFI_UNSUPPORTED;
  }

  HybridLogRangeEntries = (UINT8 *) AHCI_TMP_BLOCK (Ahci);

  AtaCmd.Command        = ATA_CMD_READ_LOG_EXT;
  AtaCmd.Direction      = AHCI_DIR_DEV2HOST;
  AtaCmd.Lba            = 0x14;
  AtaCmd.SectorCount    = 1;
  AtaCmd.MemAddr        = (UINTN) HybridLogRangeEntries;
  AtaCmd.Feature        = 0;

  Status = AhciExecCommand (Ahci, &AtaCmd);
  if (EFI_ERROR (Status)) {
    DEBUG ((EFI_D_ERROR, "Hybrid can't be supported, READ_LOG_EXT fail and Status is %r\n", Status));
    return Status;
  }

  HybridInfoHeader = HybridLogRangeEntries;

  if (HybridInfoHeader[2] != 0xFF) {
    DEBUG ((EFI_D_ERROR, "Hybrid Information is Disabled\n"));
    return EFI_DEVICE_ERROR;
  }

  DEBUG ((EFI_D_INFO, "Maximum Hybrid Priority is %x\n", HybridInfoHeader[7]));
  if (HybridInfoHeader[7] == 0 || HybridInfoHeader[7] > 16) {
    DEBUG ((EFI_D_ERROR, "Maximum Hybrid Priority Level is invalid !!\n"));
    return EFI_DEVICE_ERROR;
  }

  Ahci->HybridInfo.HybridInfoValid = HybridInfoHeader[2];
  Ahci->HybridInfo.HybridPriority  = HybridInfoHeader[7] - 1;
  DEBUG ((EFI_D_INFO, "RapidStart Hybrid Priority is %x\n", Ahci->HybridInfo.HybridPriority));

  X                          = 0;
  Y                          = 0;
  HybridInfoDescriptorNumber = *(UINT16 *)HybridInfoHeader;
  DEBUG ((EFI_D_INFO, "HybridInfoDescriptorNumber is %x\n", HybridInfoDescriptorNumber));
  for (Index = 0; Index < HybridInfoDescriptorNumber; Index++) {
    HybridInfoDescriptor = HybridLogRangeEntries + 64;
    HybridInfoDescriptor += (16 * Index);
    DEBUG ((EFI_D_INFO, "%x - Hybrid Priority is %x\n", Index, HybridInfoDescriptor[0]));
    DEBUG ((EFI_D_INFO, "   - Consumed NVM Size Fraction is %x\n", HybridInfoDescriptor[1]));
    DEBUG ((EFI_D_INFO, "   - Consumed Mapping Resources Fraction is %x\n", HybridInfoDescriptor[2]));
    DEBUG ((EFI_D_INFO, "   - Consumed NVM Size for Dirty Data Fraction is %x\n", HybridInfoDescriptor[3]));
    DEBUG ((EFI_D_INFO, "   - Consumed Mapping Resources for Dirty Data Fraction is %x\n", HybridInfoDescriptor[4]));

    X+= HybridInfoDescriptor[3];
    Y+= HybridInfoDescriptor[4];
  }

  DEBUG ((EFI_D_INFO, "Total Consumed Capacity For Dirty Data Fraction is %x\n", X));

  NvmSize = *(UINT64 *)(HybridInfoHeader + 16);
  DEBUG ((EFI_D_INFO, "NVM Size is %lx\n", NvmSize));
  X = MultU64x64 (X, NvmSize);
  X = DivU64x32 (X, 0xFF);

  Ahci->TotalRemainingCacheCapacityInSector = (UINT32)(NvmSize - X);
  DEBUG ((EFI_D_INFO, "TotalRemainingCacheCapacityInSector = %x\n", Ahci->TotalRemainingCacheCapacityInSector));

  DEBUG ((EFI_D_INFO, "Ahci Hybrid Hard Device Support Successfully\n"));

  return EFI_SUCCESS;
}
