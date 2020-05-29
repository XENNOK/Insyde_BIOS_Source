/** @file

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
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to  
  the additional terms of the license agreement               
**/

/**

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

@file:

  RapidStartPeiLib.h

@brief:

  Defines and prototypes for the library module.
  
**/
#ifndef _RAPID_START_PEI_LIB_H_
#define _RAPID_START_PEI_LIB_H_
#include <Ppi/RapidStart.h>
#ifndef RAPID_START_NO_SMRAM_INTEGRITY_CHECK
//
// Secure hash definitions for SMRAM integrity check
//
#include <Library/BaseCryptLib.h>

#define RAPID_START_SECURE_HASH_LENGTH                SHA256_DIGEST_SIZE
#define IfsSecureHashGetContextSize()                 (Sha256GetContextSize ())
#define RapidStartSecureHashInit(state)               (Sha256Init (state))
#define RapidStartSecureHashUpdate(state, data, len)  (Sha256Update ((state), (data), (len)))
#define RapidStartSecureHashFinal(state, output)      (Sha256Final ((state), (output)))

/**
  Saves SMRAM Secure Hash to non-volatile secure location.

  Only available when "RAPID_START_NO_SMRAM_INTEGRITY_CHECK" not defined. This
  function requires porting to support SMRAM Hash algorithm.

  This function will save SMRAM Hash value into non-volatile memory, platform code
  has to provide the available non-volatile memory space and modify this function
  to save SMRAM Hash value into that space. Reference code by default uses SHA256
  hashing algorithm in UDK2010 "Crypto library". See section "Porting
  Recommendation" for more information. The hash needs to be as strong as the
  one used to produce security measurements stored in the TPM, so as to not weaken
  the overall security level provided by the platform.
  "RAPID_START_SECURE_HASH_LENGTH" is defined as "SHA256_DIGEST_SIZE",
  which is defined as 32 in BaseCryptLib.h See "Porting Recommendation"
  section in this document for SMRAM Hashing implementation.

  @param[in] Hash[]   - hash value

  @retval EFI_SUCCESS   - Hash value has been saved successfully
  @retval EFI_NOT_FOUND - Hash value not found

**/
EFI_STATUS
RapidStartSaveSecureHash (
  IN  UINT8 Hash[RAPID_START_SECURE_HASH_LENGTH]
  );

/**
  Retrieves and removes SMRAM hash from non-volatile memory

  Only available when "RAPID_START_NO_SMRAM_INTEGRITY_CHECK" not defined. This
  function requires porting to support SMRAM Hash algorithm.

  This function will retrieve SMRAM Hash value from non-volatile memory and then
  remove it from non-volatile memory. Platform code has to provide the available
  non-volatile memory space and modify this function to retrieve SMRAM Hash value
  from that space. After hash value retrieved, the hash value has to be scrubbed
  from non-volatile memory immediately. Reference code by default uses SHA256
  hashing algorithm in UDK2010 "Crypto library". See section "Porting
  Recommendation" for more information. The hash needs to be as strong as the
  one used to produce security measurements stored in the TPM, so as to not weaken
  the overall security level provided by the platform.
  "RAPID_START_SECURE_HASH_LENGTH" is defined as "SHA256_DIGEST_SIZE",
  which is defined as 32 in BaseCryptLib.h

  @param[out] Hash[]   - hash value

  @retval EFI_SUCCESS   - Hash value has been restored and cleared successfully
  @retval EFI_NOT_FOUND - Hash value not found
**/
EFI_STATUS
RapidStartRestoreAndClearSecureHash (
  OUT UINT8 Hash[RAPID_START_SECURE_HASH_LENGTH]
  );

#endif /* RAPID_START_NO_SMRAM_INTEGRITY_CHECK */

/**
  Enables ACPI mode after RapidStart resume.

  This function will do the platform specific initialization for switching to ACPI
  mode by issuing ACPI_ENABLE SWSMI in the end of Rapid Start Exit transition. OS
  will treat Rapid Start Exit as same as regular S3 resume so it will not
  re-initialize certain tasks, like ACPI_ENABLE SMI will be skipped by OS, but
  those tasks still required for Rapid Start Exit because it could be resume from
  S4 or G3 state. This function may need some porting to fit platform design.

  @param[in] PeiServices   - Pointer to PEI Services Table.

  @retval EFI_SUCCESS   - enabled ACPI mode
**/
EFI_STATUS
RapidStartEnableAcpi (
  IN      CONST EFI_PEI_SERVICES    **PeiServices
  );

/**
  This callback is provided to support drive password locking interoperability with RapidStart.
  It is called before RapidStart transition is to occur. If SSD partiotion is located on password
  locked drive this function must copy the password to the output buffer.
  Size of the buffer equals ATA_PASSWORD_LEN.

  Normally the drive password is kept in SMRAM so it can be used to unlock the drive at S3 resume,
  thus during RapidStartEntry the password from SMRAM shall be returned. However at RapidStartExit the content
  of SMRAM is not yet available so the password has to be preserved in a non-volatile memory
  during RapidStartEntry (in RapidStartAfterTransition callback) and retrieved by this function.

  For security reasons the password must be scrubbed from non-volatile memory on RapidStartExit before
  returning from this function. On RapidStartEntry the password must not be stored to non-volatile memory
  sooner than RapidStartAfterTransition is called (that is after RapidStart non-volatile flag is set).

  @param[in] PeiServices   - Pointer to PEI Services Table
  @param[in] Transition    - RapidStart transition being performed
  @param[in] SataPort      - locked SATA port
  @param[out] Password     - output buffer to place the password in
  @param[out] FreezeLock   - set to TRUE to freeze drive lock, set to FALSE otherwise

  @retval EFI_SUCCESS   - Unlock password copied to the buffer
  @retval EFI_NOT_FOUND - No password provided (will break the transition)
**/
EFI_STATUS
RapidStartGetDriveUnlockPassword (
  IN      CONST EFI_PEI_SERVICES    **PeiServices,
  IN      RAPID_START_TRANSITION    Transition,
  IN      UINT8                     SataPort,
  OUT     UINT8                     *Password,
  OUT     BOOLEAN                   *FreezeLock,
  OUT     UINT16                    *PasswordType
  );

/**
  This callback is executed before RapidStart transition is performed, but after RapidStart transition conditions has been met.

  This callback is executed before Rapid Start transition is performed, but after
  Rapid Start transition conditions have been met. This callback provides
  capability to cancel Rapid Start transition if required by platform specific
  condition.

  @param[in] PeiServices   - Pointer to PEI Services Table
  @param[in] Transition    - RapidStart transition being performed
  @param[in] SataPort      - SATA port of RapidStart storage drive

  @retval EFI_SUCCESS   - RapidStart transition will follow
  @retval EFI_ABORTED   - RapidStart transition will not be performed
**/
EFI_STATUS
RapidStartBeforeTransition (
  IN      CONST EFI_PEI_SERVICES    **PeiServices,
  IN      RAPID_START_TRANSITION    Transition,
  IN      UINT8                     SataPort
  );

/**
  This callback is executed at the end of RapidStart transition whether succesfull ot not.

  If RapidStart partition is located on password locked drive the drive password must be
  copied to non-volatile memory during RapidStartEntry in this function, so it is available
  on RapidStartExit (see RapidStartGetDriveUnlockPassword).

  @param[in] PeiServices      - Pointer to PEI Services Table
  @param[in] Transition       - RapidStart transition being performed
  @param[in] TransitionStatus - Status of the RapidStart transition
  @param[in] SataPort         - SATA port of RapidStart storage drive
**/
VOID
RapidStartAfterTransition (
  IN      CONST EFI_PEI_SERVICES    **PeiServices,
  IN      RAPID_START_TRANSITION    Transition,
  IN      EFI_STATUS                TransitionStatus,
  IN      UINT8                     SataPort
  );

/**
  Provide a hook for OEM to cancel RapidStart flow.
  This function may impact RapidStart Entry performance since it's polled frequently.

  This routine is a callback for platform designer to cancel the Rapid Start Entry
  process and do S3 resume back to OS immediately (e.g. Lid opening). Since the
  time for Rapid Start Entry might be long and user might want to cancel it for
  any urgent scenario. This required condition check for when the Rapid Start
  Entry should be canceled. Note: This callback will be polled frequently during
  Rapid Start Entry process, so the implementation may impact Rapid Start Entry
  performance.

  A callback for platform designer to cancel Rapid Start Entry process when
  specific condition happened (e.g. Lid opening). This is optional, required
  porting and might impact Rapid Start Entry performance.

  @retval FALSE     - Do not cancel RapidStart flow
  @retval TRUE      - The RapidStart Entry flow should be canceled and do S3 resume back to OS
**/
BOOLEAN
RapidStartShouldCancelEntry (
  VOID
  );

/**
  This callback is executed to clear all PEM events before Rapid Start enter S4.

  @retval EFI_SUCCESS           - Operation successfully performed
**/
EFI_STATUS
RapidStartClearOemPmeEvent (
  VOID
  );

//
// Max uCode time out is 100 Milli second
//
#define MAX_UCODE_TIMEOUT    100000
#define STALL_1_MICROSECONDS 1

/**
  This function is work around for NetDetect and WakeOnLan when RapidStart enabled.

  @param EFI_SUCCESS           - Operation successfully performed
**/
EFI_STATUS
RapidStartWANetDetect (
  IN      CONST EFI_PEI_SERVICES    **PeiServices,
  IN      UINT32                    WlanMmioSpace
  );

/**
  Provide a hook for OEM to cancel RapidStart Instant-On function.
  This function may impact RapidStart Resume performance since it's polled frequently.

  @retval FALSE     - Do not cancel RapidStart Instant-On
  @retval TRUE      - The RapidStart Resume will have no Instant-On function
**/
BOOLEAN
RapidStartUnattendedWake (
  VOID
  );
#endif
