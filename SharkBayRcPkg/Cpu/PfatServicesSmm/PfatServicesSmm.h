/** @file

  Header file for the PCH PFAT Driver.                                            

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

;******************************************************************************
;* Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
;*
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#ifndef _PFAT_SERVICES_H_
#define _PFAT_SERVICES_H_

#include <Uefi.h>
#include <PfatDefinitions.h>
#include <PchAccess.h>
#include <Library/PchPlatformLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/BaseLib/BaseLibInternals.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/AslUpdateLib.h>
#include <Library/HobLib.h>

///
/// Driver Dependency Protocols
///
#include <Protocol/Pfat.h>
//[-start-130121-IB05160402-modify]//
#include <Protocol/SmmBase2.h>
//[-end-130121-IB05160402-modify]//
#include <Protocol/FirmwareVolume.h>
#include <Protocol/SmmIoTrapDispatch.h>
#include <Protocol/GlobalNvsArea.h>
#include <Guid/SaDataHob.h>

UINT8                 mPfatUpdateData[PUP_BUFFER_SIZE];
static UINT16         mPfatUpdateCounter;
PUP                   *mPfatUpdatePackagePtr;
EFI_PHYSICAL_ADDRESS  mPupCertificate;
PFAT_LOG              *mPfatLogPtr;
PFAT_LOG              mPfatLogTemp;
EFI_PHYSICAL_ADDRESS  mPfatMemAddress;
UINT32                mPfatMemSize;
UINT64                mPfatFullStatus;

///
/// Private data structure definitions for the driver
///
#define PFAT_SIGNATURE SIGNATURE_32 ('P', 'F', 'A', 'T')

#define PFAT_DIRECTORY_MAX_SIZE               6
#define PFAT_DIRECTORY_PFAT_MODULE_ENTRY      0x00
#define PFAT_DIRECTORY_PPDT_ENTRY             0x01
#define PFAT_DIRECTORY_PUP_ENTRY              0x02
#define PFAT_DIRECTORY_PUP_CERTIFICATE_ENTRY  0x03
#define PFAT_DIRECTORY_PFAT_LOG_ENTRY         0x04
#define PFAT_DIRECTORY_UNDEFINED_ENTRY        0xFE
#define PFAT_DIRECTORY_END_MARKER             0xFF

typedef enum {
  EnumPfatModule        = 0,
  EnumPpdt,
  EnumPup,
  EnumPupCertificate,
  EnumPfatLog,
  EnumPfatDirectoryEnd
} PFAT_DIRECTORY;

typedef struct {
  UINTN                Signature;
  EFI_HANDLE           Handle;
  PFAT_PROTOCOL        PfatProtocol;
  EFI_PHYSICAL_ADDRESS PfatDirectory[PFAT_DIRECTORY_MAX_SIZE];
  UINT32               AddrMask;
  UINT64               MsrValue;
} PFAT_INSTANCE;

#define PFAT_INSTANCE_FROM_PFATPROTOCOL(a)  CR (a, PFAT_INSTANCE, PfatProtocol, PFAT_SIGNATURE)

///
/// Stall period in microseconds
///
#define PFAT_WAIT_PERIOD          0
#define PFAT_AP_SAFE_RETRY_LIMIT  1

/**
  Initialize PFAT protocol instance.

  @param[in] PfatInstance  Pointer to PfatInstance to initialize

  @retval EFI_SUCCESS      The protocol instance was properly initialized
**/
EFI_STATUS
PfatProtocolConstructor (
  PFAT_INSTANCE *PfatInstance
  );

/**
  Set MSR 0x115 with PFAT DIRECTORY Address.
  Trigger MSR 0x116 to invoke PFAT Binary.
  Read MSR 0x115 to get PFAT Binary Status.

  @param[in] PfatInstance              Pointer to PfatInstance to initialize
**/
VOID
EFIAPI
PfatModuleExecute (
  IN VOID          *PfatInstance
  );

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
EFI_STATUS
EFIAPI
PfatProtocolExecute (
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
VOID
EFIAPI
PfatProtocolWrite (
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
VOID
EFIAPI
PfatProtocolBlockErase (
  IN PFAT_PROTOCOL *This,
  IN UINTN         Address
  );

#endif
