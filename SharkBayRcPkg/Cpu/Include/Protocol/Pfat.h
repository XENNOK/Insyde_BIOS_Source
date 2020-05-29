/**
  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement

@copyright
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  Pfat.h

@brief
  This file defines the PFAT Protocol which implements the
  Intel(R) PFAT Host Controller Compatibility Interface.
**/
#ifndef _PFAT_H_
#define _PFAT_H_

#if !defined(EDK_RELEASE_VERSION) || (EDK_RELEASE_VERSION < 0x00020000)
///
/// GUID to locate PFAT SMM Protocol
///
#define SMM_PFAT_PROTOCOL_GUID \
  { \
    0xc3e156e4, 0x27b3, 0x4dff, 0xb8, 0x96, 0xfb, 0x11, 0x3b, 0x2e, 0x68, 0xb5 \
  }
#else
///
/// GUID to locate PFAT SMM Protocol
///
#define SMM_PFAT_PROTOCOL_GUID \
  { \
    0xc3e156e4, 0x27b3, 0x4dff, \
    { \
      0xb8, 0x96, 0xfb, 0x11, 0x3b, 0x2e, 0x68, 0xb5 \
    } \
  }
#endif

///
/// Extern the GUID for protocol users.
///
extern EFI_GUID gSmmPfatProtocolGuid;

//
// Forward reference for ANSI C compatibility
//
typedef struct _PFAT_PROTOCOL PFAT_PROTOCOL;

/**
  This service will write PFAT_DIRECTORY MSR and invoke the PFAT Module by writing to PLAT_FRMW_PROT_TRIGGER MSR for writing/erasing to flash.
  BIOS should invoke PFAT_PROTOCOL.Write() or PFAT_PROTOCOL.Erase() function prior to calling PFAT_PROTOCOL.Execute() for flash writes/erases (except for BiosUpdate).
  Write()/Erase() function will render PFAT script during execution.
  Execute() function will implement the following steps:
  1. Update PFAT directory with address of PUP.
  2. All the AP's except the master thread are put to sleep.
  3. PFAT module is invoked from BSP to execute desired operation.
  If BiosUpdate flag is set to true, PUP (PUP Header + PFAT Script + Update data) is part of data that is passed to SMI Handler. SMI Handler invokes PFAT module to process the update.
  This function would be called by runtime driver, please do not use any MMIO macro here.

  @param[in] This                Pointer to the PFAT_PROTOCOL instance.
  @param[in] BiosUpdate          Flag to indicate flash update is requested by the Tool

  @retval EFI_SUCCESS            Successfully completed flash operation.
  @retval EFI_INVALID_PARAMETER  The parameters specified are not valid.
  @retval EFI_UNSUPPORTED        The CPU or SPI memory is not supported.
  @retval EFI_DEVICE_ERROR       Device error, command aborts abnormally.
**/
typedef
EFI_STATUS
(EFIAPI *PFAT_EXECUTE)(
  IN PFAT_PROTOCOL *This,
  IN BOOLEAN       BiosUpdate
  );

/**
  This service fills PFAT script buffer for flash writes.
  BIOS should invoke this function prior to calling PFAT_PROTOCOL.Execute() with all the relevant data required for flash write.
  This function will not invoke PFAT Module, only create script required for writing to flash.
  This function would be called by runtime driver, please do not use any MMIO macro here.

  @param[in] This           Pointer to the PFAT_PROTOCOL instance.
  @param[in] Address        This value specifies the offset from the start of the SPI Flash component where BIOS Image is located.
  @param[in] DataByteCount  Number of bytes in the data portion.
  @param[in] Buffer         Pointer to caller-allocated buffer containing the dada sent.
**/
typedef
VOID
(EFIAPI *PFAT_WRITE)(
  IN PFAT_PROTOCOL *This,
  IN UINTN         Address,
  IN UINT32        DataByteCount,
  IN OUT UINT8     *Buffer
  );

/**
  This service fills PFAT script buffer for erasing blocks in flash.
  BIOS should invoke this function prior to calling PFAT_PROTOCOL.Execute() with all the relevant data required for flash erase.
  This function will not invoke PFAT module, only create script required for erasing each block in the flash.
  This function would be called by runtime driver, please do not use any MMIO macro here.

  @param[in] This     Pointer to the PFAT_PROTOCOL instance.
  @param[in] Address  This value specifies the offset from the start of the SPI Flash component where BIOS Image is located.
**/
typedef
VOID
(EFIAPI *PFAT_ERASE)(
  IN PFAT_PROTOCOL *This,
  IN UINTN         Address
  );

/**
  This protocol provides all the services required for flash writes/erases via PFAT
  PFAT Module can only be launched from SMM, this means that all flash writes & erases
  that BIOS needs to do must flow thru SMI Handler and so dependency on SMM_BASE_PROTOCOL
  for installing PFAT Protocol. Prior to PFAT SMM Protocol being installed there should
  be no writes/erases to flash.
**/
struct _PFAT_PROTOCOL {
  PFAT_WRITE   Write;   ///< Invoked to fill up PFAT script buffer for flash writes
  PFAT_ERASE   Erase;   ///< Invoked to fill up PFAT script buffer for flash erases
  PFAT_EXECUTE Execute; ///< Will trigger invocation of PFAT module
};

#endif
