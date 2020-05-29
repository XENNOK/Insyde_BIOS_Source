/** @file
  Header file for the PCH Init PEIM

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

/**

@copyright
  Copyright (c) 2004 - 2012 Intel Corporation. All rights reserved
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
#ifndef _PCH_INIT_PEIM_H_
#define _PCH_INIT_PEIM_H_

#include <PchInitVar.h>
#include <PchAccess.h>
#include <PchPlatformLib.h>
#include <PchUsbCommon.h>
#include <IobpDefinitions.h>
#include <PchHSIO.h>
#include <Ppi/PchDmiTcVcMap.h>
#include <Ppi/MemoryDiscovered.h>
#include <Ppi/PchUsbPolicy.h>
#include <Ppi/PchPlatformPolicy.h>
#include <Ppi/PchReset.h>
#include <Ppi/PchPeiInitDone.h>
#include <Ppi/PchInit.h>
#include <Ppi/ReadOnlyVariable.h>
#include <Ppi/EndOfPeiPhase.h>
#include <PchInitVar.h>

//
// ChipsetInit settings defines
//
#define H2M_HSIO_MESSAGE        (0x7 << 28)///< Master type for all H2M Hsio messages
#define H2M_HSIO_CMD_GETHSIOVER 1          ///< Triggers Hsio version to be sent through ME/Host FW Status registers
#define H2M_HSIO_CMD_CLOSE      0          ///< Triggers H2M Hsio interface to close and revert FW Status registers
#define M2H_HSIO_MSG_ACK        0x7        ///< Ack sent in response to any H2M Hsio messages
#define MAX_ME_MSG_ACK_TIMEOUT  0x186A0    // Wait max of 100ms for FW to acknowledge.

/**
  Internal function performing SATA init needed in PEI phase

  @param[in] PeiServices            General purpose services available to every PEIM.
  @param[in] PchPlatformPolicyPpi   The PCH Platform Policy PPI instance

  @retval None
**/
EFI_STATUS
EFIAPI
PchSataInit (
  IN  CONST EFI_PEI_SERVICES            **PeiServices,
  IN  PCH_PLATFORM_POLICY_PPI     *PchPlatformPolicyPpi
  );

/**
  This function may trigger platform reset depending on the current GbE status,
  the intended GbE enabling, and current ME status. (When ME is enabled, this function
  may trigger a Global reset.)
  This function may not return if it triggers an platform reset and the BIOS boot flow
  restarts.
  If this function returns EFI_SUCCESS it indicates there is no need for platform
  reset in this boot, and boot flow continues.
  If this function returns EFI_DEVICE_ERROR, something error happens.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] PchPlatformPolicyPpi The PCH Platform Policy PPI instance

  @retval EFI_SUCCESS             No platform reset action is taken. System can continue boot flow.
  @retval Others                  Won't return if platform reset action is taken
**/
EFI_STATUS
EFIAPI
PchGbeMandatedReset (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN  PCH_PLATFORM_POLICY_PPI     *PchPlatformPolicyPpi
  );

/**
  Perform Thermal Management Support initialization

  @param[in] PchPlatformPolicyPpi The PCH Platform Policy PPI instance

  @retval EFI_SUCCESS             Succeeds.
  @retval EFI_DEVICE_ERROR        Device error, aborts abnormally.
**/
EFI_STATUS
PchThermalInit (
  IN  PCH_PLATFORM_POLICY_PPI     *PchPlatformPolicyPpi
  );

/**
  Initialize IOAPIC according to IoApicConfig policy of the PCH
  Platform Policy PPI

  @param[in] PchPlatformPolicyPpi The PCH Platform Policy PPI instance

  @retval EFI_SUCCESS             Succeeds.
  @retval EFI_DEVICE_ERROR        Device error, aborts abnormally.
**/
EFI_STATUS
PchIoApicInit (
  IN  PCH_PLATFORM_POLICY_PPI     *PchPlatformPolicyPpi
  );

/**
  This function performs basic initialization for PCH in PEI phase.
  If any of the base address arguments is zero, this function will disable the corresponding
  decoding, otherwise this function will enable the decoding.
  This function locks down the PMBase.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] NotifyDescriptor     The notification structure this PEIM registered on install.
  @param[in] Ppi                  The memory discovered PPI.  Not used.

  @retval EFI_SUCCESS             Succeeds.
  @retval EFI_DEVICE_ERROR        Device error, aborts abnormally.
**/
EFI_STATUS
EFIAPI
PchInitialize (
  IN EFI_PEI_SERVICES             **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor,
  IN VOID                         *Ppi
  );

/**
  The function performing USB init in PEI phase. This could be used by USB recovery
  or debug features that need USB initialization during PEI phase.
  Note: Before executing this function, please be sure that PCH_INIT_PPI.Initialize
  has been done and PchUsbPolicyPpi has been installed.

  @param[in] PeiServices    General purpose services available to every PEIM

  @retval EFI_SUCCESS       The function completed successfully
  @retval Others            All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
PchUsbInit (
  IN CONST EFI_PEI_SERVICES             **PeiServices
  );

/**
  The function performing TC/VC mapping program, and poll all PCH Virtual Channel
  until negotiation completion

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval EFI_SUCCESS             The function completed successfully
  @retval Others                  All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
EFIAPI
PchDmiTcVcProgPoll (
  IN  CONST EFI_PEI_SERVICES            **PeiServices
  );

/**
  The function set the Target Link Speed in PCH to DMI GEN 2.

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval None
**/
VOID
EFIAPI
PchDmiGen2Prog (
  IN  CONST EFI_PEI_SERVICES            **PeiServices
  );

/**
  The function program DMI miscellaneous registers.

  @param[in] PeiServices          General purpose services available to every PEIM
  @param[in] PchPlatformPolicyPpi The PCH Platform Policy PPI instance

  @retval EFI_SUCCESS             The DMI required settings programmed correctly
**/
EFI_STATUS
EFIAPI
PchDmiMiscProg (
  IN  EFI_PEI_SERVICES                   **PeiServices,
  IN  PCH_PLATFORM_POLICY_PPI           *PchPlatformPolicyPpi
  );

/**
  The function is used while doing CPU Only Reset, where PCH may be required
  to initialize strap data before soft reset.

  @param[in] PeiServices          General purpose services available to every PEIM
  @param[in] Operation            Get/Set Cpu Strap Set Data
  @param[in, out] CpuStrapSet     Cpu Strap Set Data

  @retval EFI_SUCCESS             The function completed successfully.
  @exception EFI_UNSUPPORTED      The function is not supported.
**/
EFI_STATUS
EFIAPI
PchCpuStrapSet (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN      CPU_STRAP_OPERATION         Operation,
  IN OUT  UINT16                      *CpuStrapSet
  );

/**
  The function performing USB init in PEI phase. This could be
  used by USB recovery ,debug features or usb precondition
  enabled case that need USB initialization during PEI phase.
  Please be sure the function should not be executed in if the
  boot mode is S3 resume.

  @param[in] PeiServices    General purpose services available to every PEIM

  @retval EFI_SUCCESS       The function completed successfully
  @retval Others            All other error conditions encountered result in an ASSERT.
**/
EFI_STATUS
PchStartUsbInit (
  IN PCH_USB_CONFIG               *UsbConfig,
  IN UINT32                       EhciMemBaseAddr,
  IN UINT32                       XhciMemBaseAddr,
  IN UINT8                        Revision
  );

/**
  This function handles Pch S3 resume task

  @param[in] PeiServices   - Pointer to PEI Services Table.
  @param[in] NotifyDesc    - Pointer to the descriptor for the Notification event that 
                             caused this function to execute.
  @param[in] Ppi           - Pointer to the PPI data associated with this function.

  @retval EFI_STATUS       - Always return EFI_SUCCESS
**/
EFI_STATUS
PchS3ResumeAtEndOfPei (
  IN      EFI_PEI_SERVICES          **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR *NotifyDesc,
  IN      VOID                      *Ppi
  );

#endif
