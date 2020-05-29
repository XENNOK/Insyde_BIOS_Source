/** @file
  This file contains function definitions that can determine
  the TXT capabilities of a platform during DXE and perform
  certain specific platform tasks that are required for TXT
  during DXE.
  
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
  This file contains an 'Intel Peripheral Driver' and uniquely  
  identified as "Intel Reference Module" and is                 
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
**/

/**

Copyright (c)  1999 - 2011 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  TxtDxeLib.h

@brief:

  This file contains function definitions that can determine
  the TXT capabilities of a platform during DXE and perform
  certain specific platform tasks that are required for TXT
  during DXE.

**/
#ifndef _TXT_DXE_LIB_H_
#define _TXT_DXE_LIB_H_

#include <CpuAccess.h>
#include <Library/BaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DebugLib.h>
#include <Library/IoLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/PcdLib.h>
#include <Library/PciLib.h>
#include <Protocol/CpuIo.h>
#include <Protocol/CpuPlatformPolicy.h>
#include <Guid/TxtInfoHob.h>

///
/// The following switch is used in EfiScriptLib.h file. If enabled -
/// forces linking to EfiScriptLib library
///
#define BASE_ADDR_MASK                      0xFFFFF000
#define TEST_PATTERN                        0x5A5A5A5A5A5A5A5A
#define MP_TIMEOUT_FOR_STARTUP_ALL_APS      0           /// Set 0 for BSP always wait for APs

///
/// Chispet register
///
#define TXT_OPT_IN_VMX_AND_SMX_MSR_VALUE    0xFF03

#define IA32_MTRR_PHYSBASE0                 0x200
#define IA32_MTRR_PHYSMASK9                 0x213

#define LOCAL_APIC_THERMAL_DEF              0x330
#define B_INTERRUPT_MASK                  (1 << 16)
#define B_DELIVERY_MODE                   (0x07 << 8)
#define V_MODE_SMI                        (0x02 << 8)
#define B_VECTOR                          (0xFF << 0)


#define TIANO_ERROR(a)          (MAX_2_BITS | (a))
#if (EFI_SPECIFICATION_VERSION < 0x00020000)
#define EFI_UNLOAD_IMAGE                  EFIERR (29)
#else
#define EFI_UNLOAD_IMAGE                  TIANO_ERROR (1)
#endif

#define EFI_DEADLOOP()    { volatile UINTN __iii; __iii = 1; while (__iii); }


#pragma pack(push, 1)
typedef struct _TXT_DXE_LIB_CONTEXT_ {
  EFI_HANDLE                          ImageHandle;
  EFI_SYSTEM_TABLE                    *SystemTable;
  EFI_CPU_IO_PROTOCOL                 *CpuIo;
  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL  *MpService;

  UINTN                               CpuCount;
  TXT_INFO_DATA                       *TxtInfoData;
  DXE_CPU_PLATFORM_POLICY_PROTOCOL    *CpuPlatformPolicy;
} TXT_DXE_LIB_CONTEXT;
#pragma pack(pop)

/**

  This routine initializes and collects all Protocols and data required
  by the routines in this file.

  @param[in] ImageHandle   - A pointer to the Image Handle for this file.
  @param[in] SystemTable   - A pointer to the EFI System Table
  @param[in] TxtDxeCtx      - A pointer to a caller allocated data structure that contains
                  all of the Protocols and data required by the routines 
                  in this file.

  @retval EFI_SUCCESS       - Return EFI_SUCCESS if no error happen
  @retval EFI_UNLOAD_IMAGE  - If TxtInfoHob is not found
  
**/
EFI_STATUS
InitializeTxtDxeLib (
  IN      EFI_HANDLE            ImageHandle,
  IN      EFI_SYSTEM_TABLE      *SystemTable,
  IN OUT  TXT_DXE_LIB_CONTEXT   *TxtDxeCtx
  );

/**

  Determines whether or not the current processor is TXT Capable.

  @param[in] None

  @retval TRUE          - If the current processor supports TXT
  @retval FALSE         - If the current processor does not support TXT

**/
BOOLEAN
IsTxtProcessor (
  VOID
  );

/**

  Determines whether or not the platform has executed an TXT launch by
  examining the TPM Establishment bit.

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval TRUE          - If the TPM establishment bit is asserted.
  @retval FALSE         - If the TPM establishment bit is unasserted.

**/
BOOLEAN
IsTxtEstablished (
  IN TXT_DXE_LIB_CONTEXT      *TxtDxeCtx
  );

/**

  Determines whether or not the platform has executed an TXT launch by
  examining the TPM Establishment bit.

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval TRUE          - If the TPM establishment bit is asserted.
  @retval FALSE         - If the TPM establishment bit is unasserted.

**/
BOOLEAN
IsTxtResetSet (
  IN      TXT_DXE_LIB_CONTEXT      *TxtDxeCtx
  );

/**

  Determines whether or not the platform requires initialization for TXT use.

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval TRUE          - If the the platoform should be configured for TXT.
  @retval FALSE         - If TXT is not to be used.

**/
BOOLEAN
IsTxtEnabled (
  IN TXT_DXE_LIB_CONTEXT      *TxtDxeCtx
  );

/**

  Adds and allocates architecturally defined TXT Configuration Space memory
  region to GCD.                                                          
                                                                          
  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
ReserveTxtConfigSpace (
  IN TXT_DXE_LIB_CONTEXT      *TxtDxeCtx
  );

/**

  Allocates 1 MB of 1MB-aligned memory for use as TXT Device Memory.  Records
  the location of TXT Device Memory in TXT Chipset registers and then adds
  programming instructions for these registers into BootScript.

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - TXT Device memory has been successfully initialized.
  @retval EFI_ERROR     - TXT Device memory not awailable.

**/
EFI_STATUS
SetupTxtDeviceMemory (
  IN TXT_DXE_LIB_CONTEXT      *TxtDxeCtx
  );

/**

  Invokes TxtDxeLibLaunchBiosAcm to execute the SCHECK function.

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
DoScheck (
  IN TXT_DXE_LIB_CONTEXT      *TxtDxeCtx
  );

/**

  Invokes TxtDxeLibLaunchBiosAcm to reset the TPM's establishment bit.

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
ResetTpmEstBit (
  IN TXT_DXE_LIB_CONTEXT      *TxtDxeCtx
  );

/**

  Sets up the system and then launches the TXT BIOS ACM to run the function
  requested by AcmFunction. 

  @param[in] AcmBase  - Base address of BIOS ACM location
  @param[in] Funct    - Function number of BIOS ACM to be executed

  @retval EFI_SUCCESS   - Always.

**/
VOID
LaunchBiosAcm (
  IN UINT64 AcmBase,
  IN UINT64 Funct
  );

/**

  Sets up the system and then launches the TXT BIOS ACM to run the function
  requested by AcmFunction. 

  @param[in] AcmFunction   - Constant that represents the function from the BIOS ACM
                  that should be executed.
  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - Always.

**/
EFI_STATUS
TxtDxeLibLaunchBiosAcm (
  IN TXT_DXE_LIB_CONTEXT    *TxtDxeCtx,
  IN UINT64                 AcmFunction
  );

/**

  AP initial routine executed through MP service for TXT SCHECK

  @param[in] None


**/
VOID
DoApInit (
  VOID
  );

/**

  Save AP configuration routine executed through MP service for TXT SCHECK

  @param[in] None


**/
VOID
ApSaveConfig (
  VOID
  );

/**

  Restore AP configuration routine executed through MP service for TXT SCHECK

  @param[in] None


**/
VOID
ApRestoreConfig (
  VOID
  );

/**

  Disable or restore possible SMI sources before or after POST SCHECK

  @param[in] TxtDxeCtx - A pointer to an initialized TXT DXE Context data structure
  @param[in] Operation - Boolean value telling what operation is requested:
              TRUE  - to save and then disable possible SMI sources
              FALSE - to restore original SMI settings

  @retval EFI_SUCCESS - always return EFI_SUCCESS

**/
EFI_STATUS
DisableSmiSources (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx,
  IN BOOLEAN             Operation
  );

/**

  This function gets called before/after run the SCHECK function, intend to avoid platform specific SMIs to interfere in BIOS POST
  if BIOS SMM MP services or synchonization code is not well considered that some of APs are not waken up from Wait-for-SIPI state.
  Function should preserve original SMI enabling setting in augument is TRUE, and then restore it in augurment is FALSE.
  The caller is DisableSmiSources() in TxtDxeLib.c

  @param[in] Operation = TRUE    - Calling before SCHECK to saved and disable platform specific SMIs setting
                   = FALSE  - Calling after SCHECK to restore platform specific SMIs setting

  @retval EFI_SUCCESS   - Always.

**/
extern
EFI_STATUS
EFIAPI
TxtDxeOemDisableSmi (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx,
  IN BOOLEAN Operation
  );

/**

  Read policy protocol to reset AUX content

  @param[in] TxtDxeCtx - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - No error happend
  @retval EFI_NOT_FOUND - TxtPolicyProtocol is not found

**/
EFI_STATUS
ResetTpmAux (
  IN TXT_DXE_LIB_CONTEXT      *TxtDxeCtx
);

#endif
