/** @file

  Header file for the SA Init PEIM

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _SA_INIT_PEIM_H_
#define _SA_INIT_PEIM_H_

///===============================================
///  MdePkg/Include/
///===============================================
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include <Library/HobLib.h>
#include <Library/IoLib.h>

#include <Ppi/MemoryDiscovered.h>
#include <Ppi/EndOfPeiPhase.h>

///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Chipset/IntelMch/SystemAgent/
///===============================================
#include <SaAccess.h>
#include <CpuRegs.h>

#include <Library/CpuPlatformLib.h>
#include <Guid/SaDataHob.h>
#include <Ppi/SaPeiInit.h>
#include <Ppi/SaPlatformPolicy.h>

///===============================================
///  $(CHIPSET_REF_CODE_PKG)/Cpu/Haswell/
///===============================================
#include <Ppi/CpuPlatformPolicy.h>

#include "SaOcInit.h"

///
/// Data definitions & structures
///

EFI_GUID  gMemoryInitHobGuid = EFI_PEI_PERMANENT_MEMORY_INSTALLED_PPI_GUID;

///
/// Functions
///
/**
  SA PEI Initialization.

  @param [in] FileHandle          Handle of the file being invoked. 
  @param [in] PeiServices         Describes the list of possible PEI Services.

  @retval EFI_SUCCESS

**/
EFI_STATUS
EFIAPI
SaInitPeiEntryPoint (
  IN       EFI_PEI_FILE_HANDLE         FileHandle,
  IN CONST EFI_PEI_SERVICES            **PeiServices
  );

/**
  Programs SA Bars

  @param [in] SaPlatformPolicyPpi Instance of SA_PLATFORM_POLICY_PPI

**/
VOID
ProgramSaBars (
  IN SA_PLATFORM_POLICY_PPI            *SaPlatformPolicyPpi
  );

/**
  SwitchableGraphicsInit: Initialize the Switchable Graphics if enabled

  @param [in] PeiServices         Pointer to the PEI services table
  @param [in] SaPlatformPolicyPpi SaPlatformPolicyPpi to access the SgConfig related information


**/
VOID
SwitchableGraphicsInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  );

/**
  Do Early BIOS POST Programming

  @param [in] SaPlatformPolicyPpi Instance of SA_PLATFORM_POLICY_PPI

**/
VOID
EarlyBiosPostProgramming (
  IN SA_PLATFORM_POLICY_PPI            *SaPlatformPolicyPpi
  );

/**
  GraphicsInit: Initialize the IGD if no other external graphics is present

  @param [in] PeiServices         Pointer to the PEI services table
  @param [in] SaPlatformPolicyPpi SaPlatformPolicyPpi to access the GtConfig related information


**/
VOID
GraphicsInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  );

/**
  Initialize DMI Tc/Vc mapping through SA-PCH.

  @param [in] PeiServices         General purpose services available to every PEIM.
  @param [in] SaPlatformPolicyPpi SaPlatformPolicyPpi to access the GtConfig related information

  @retval EFI_SUCCESS

**/
EFI_STATUS
SaDmiTcVcInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  );

#ifdef DMI_FLAG
/**
  Initialize DMI.

  @param [in] PeiServices         General purpose services available to every PEIM.
  @param [in] SaPlatformPolicyPpi SaPlatformPolicyPpi to access the GtConfig related information

  @retval EFI_SUCCESS

**/
EFI_STATUS
DmiInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  );
#endif /// DMI_FLAG

#ifdef PEG_FLAG
/**
  GraphicsInit: Initialize the IGD if no other external graphics is present

  @param [in] PeiServices         Pointer to the PEI services table
  @param [in] SaPlatformPolicyPpi SaPlatformPolicyPpi to access the GtConfig related information

**/
VOID
PciExpressInit (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  );
#endif /// PEG_FLAG

/**
  Init and Install SA Hob

  @param [in] PeiServices         General purpose services available to every PEIM.
  @param [in] SaPlatformPolicyPpi SaPlatformPolicyPpi to access the GtConfig related information

  @retval EFI_SUCCESS

**/
EFI_STATUS
InstallSaHob (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SA_PLATFORM_POLICY_PPI      *SaPlatformPolicyPpi
  );

/**
  Report the SA PCIe initialization code version.

  @param [in] SaPlatformPolicyPpi Instance of SA_PLATFORM_POLICY_PPI

  @retval EFI_SUCCESS

**/
EFI_STATUS
ReportPcieVersion (
  IN SA_PLATFORM_POLICY_PPI            *SaPlatformPolicyPpi
  );

/**
  BIOS_RESET_CPL bit is set for processor to activate the power and thermal management
  features on the platform.

  @param [in] PeiServices         Pointer to PEI Services Table.
  @param [in] NotifyDesc          Pointer to the descriptor for the Notification event that
                                  caused this function to execute.
  @param [in] Ppi                 Pointer to the PPI data associated with this function.

  @retval EFI_SUCCESS

**/
STATIC
EFI_STATUS
SaResetComplete (
  IN       EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDesc,
  IN       VOID                        *Ppi
  );

/**
  This function prints the PEI phase platform policy.

  @param [in] SaPlatformPolicyPpi Instance of SA_PLATFORM_POLICY_PPI

**/
VOID
SaPeiPolicyDump (
  IN SA_PLATFORM_POLICY_PPI            *SaPlatformPolicyPpi
  );

/**
  This function handles SA S3 resume task

  @param [in] PeiServices         Pointer to PEI Services Table.
  @param [in] NotifyDesc          Pointer to the descriptor for the Notification event that
                                  caused this function to execute.
  @param [in] Ppi                 Pointer to the PPI data associated with this function.

  @retval EFI_STATUS              Always return EFI_SUCCESS

**/
static
EFI_STATUS
SaS3ResumeAtEndOfPei (
  IN       EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDesc,
  IN       VOID                        *Ppi
  );

#ifdef RAPID_START_FLAG
/**
  This function will check Rapid Start mode and install SaS3Resume callback notify if it was Rapid Start Resume

  @param [in] PeiServices         Pointer to PEI Services Table.
  @param [in] NotifyDesc          Pointer to the descriptor for the Notification event that
                                  caused this function to execute.
  @param [in] Ppi                 Pointer to the PPI data associated with this function.

  @retval EFI_STATUS              Always return EFI_SUCCESS

**/
static
EFI_STATUS
SaCheckRapidStartMode (
  IN       EFI_PEI_SERVICES            **PeiServices,
  IN       EFI_PEI_NOTIFY_DESCRIPTOR   *NotifyDesc,
  IN       VOID                        *Ppi
  );
#endif /// RAPID_START_FLAG

#endif
