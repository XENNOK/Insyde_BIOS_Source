/** @file
  This file contains an implementation of the function call interfaces
  required by the main TXT DXE file.  Hopefully, future platform porting
  tasks will be mostly limited to modifying the functions in this file.
  
;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
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

Copyright (c)  1999 - 2012 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

**/

#include <Txt.h>
#include "TxtDxeLib.h"
//[-start-110216-IB06690094-add]//
#include <TxtAcmLib.h>
//[-end-110216-IB06690094-add]//

#include <Library/S3BootScriptLib.h>
#include <Library/HobLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/DxeServicesTableLib.h>

#include <Protocol/PciIo.h>
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/FirmwareVolume.h>

UINT64                  mMcuAddr;
UINT64                  mAcmBase;
UINT64                  mApMtrrTab[2 * (IA32_MTRR_PHYSMASK9 - IA32_MTRR_PHYSBASE0 + 1) + 1];
UINT64                  mApIdt[2];
UINT64                  mApCr4;
UINT64                  mApSavedIa32ThermInterruptMSR[2];
UINT32                  mApSavedApicThermalValue;

/**
  This routine initializes and collects all Protocols and data required
  by the routines in this file.

  @param[in] ImageHandle   - A pointer to the Image Handle for this file.
  @param[in] SystemTable   - A pointer to the EFI System Table
  @param[in] TxtDxeCtx     - A pointer to a caller allocated data structure that contains
                  all of the Protocols and data required by the routines
                  in this file.

  @retval EFI_SUCCESS       - Return EFI_SUCCESS if no error happen
  @retval EFI_UNLOAD_IMAGE  - If TxtInfoHob is not found
**/
EFI_STATUS
InitializeTxtDxeLib (
  IN EFI_HANDLE              ImageHandle,
  IN EFI_SYSTEM_TABLE        *SystemTable,
  IN OUT TXT_DXE_LIB_CONTEXT *TxtDxeCtx
  )
{
  EFI_STATUS   Status;
  VOID         *HobList;
  TXT_INFO_HOB *TxtInfoHob;
  UINTN        tmp;

  //INITIALIZE_SCRIPT (ImageHandle, SystemTable);

  TxtDxeCtx->ImageHandle  = ImageHandle;
  TxtDxeCtx->SystemTable  = SystemTable;

  ///
  /// Find the TxtPolicyprotocol
  ///
  Status = gBS->LocateProtocol (
                  &gDxeCpuPlatformPolicyProtocolGuid,
                  NULL,
                   (VOID **)&(TxtDxeCtx->CpuPlatformPolicy)
                  );

  if (EFI_ERROR (Status)) {
    TxtDxeCtx->CpuPlatformPolicy = NULL;
  }
  ///
  /// Find the CpuIo protocol
  ///
  Status = gBS->LocateProtocol (
                  &gEfiCpuIoProtocolGuid,
                  NULL,
                  (VOID **) &(TxtDxeCtx->CpuIo)
                  );

  ASSERT_EFI_ERROR (Status);
  ///
  /// Find the MpService Protocol
  ///
  Status = gBS->LocateProtocol (
                  &gFrameworkEfiMpServiceProtocolGuid,
                  NULL,
                   (VOID **)&(TxtDxeCtx->MpService)
                  );

  ASSERT_EFI_ERROR (Status);
  ///
  /// Initialize CpuCount info. Current implemetation of
  /// GetGeneralMPInfo doesn't honor optionality of arguments. Don't use
  /// NULL pointers.
  ///
  Status = TxtDxeCtx->MpService->GetGeneralMPInfo (
                                   TxtDxeCtx->MpService,
                                   &(TxtDxeCtx->CpuCount),
                                   &tmp,
                                   &tmp,
                                   &tmp,
                                   &tmp
                                   );
  ASSERT_EFI_ERROR (Status);

  ///
  /// Find TxtInfoHob
  ///
  HobList = GetFirstGuidHob (&gTxtInfoHobGuid);
  if (HobList == NULL) {
    return EFI_UNLOAD_IMAGE;
  }

  TxtInfoHob              = (TXT_INFO_HOB *) HobList;
  TxtDxeCtx->TxtInfoData  = &(TxtInfoHob->Data);

  ///
  /// Print out the TxtInfo HOB
  ///
  DEBUG ((EFI_D_INFO, "TXTDXE: TxtInfoHob passed from platform as:\n"));
  DEBUG ((EFI_D_INFO, "TXTDXE: ChipsetIsTxtCapable = %x\n", TxtDxeCtx->TxtInfoData->ChipsetIsTxtCapable));
  DEBUG ((EFI_D_INFO, "TXTDXE: TxtMode             = %x\n", TxtDxeCtx->TxtInfoData->TxtMode));
  DEBUG ((EFI_D_INFO, "TXTDXE: PmBase              = %x\n", TxtDxeCtx->TxtInfoData->PmBase));
  DEBUG ((EFI_D_INFO, "TXTDXE: SinitMemorySize     = %x\n", TxtDxeCtx->TxtInfoData->SinitMemorySize));
  DEBUG ((EFI_D_INFO, "TXTDXE: TxtHeapMemorySize   = %x\n", TxtDxeCtx->TxtInfoData->TxtHeapMemorySize));
  DEBUG ((EFI_D_INFO, "TXTDXE: TxtDprMemoryBase    = %x\n", TxtDxeCtx->TxtInfoData->TxtDprMemoryBase));
  DEBUG ((EFI_D_INFO, "TXTDXE: TxtDprMemorySize    = %x\n", TxtDxeCtx->TxtInfoData->TxtDprMemorySize));
  DEBUG ((EFI_D_INFO, "TXTDXE: BiosAcmBase         = %x\n", TxtDxeCtx->TxtInfoData->BiosAcmBase));
  DEBUG ((EFI_D_INFO, "TXTDXE: BiosAcmSize         = %x\n", TxtDxeCtx->TxtInfoData->BiosAcmSize));
  DEBUG ((EFI_D_INFO, "TXTDXE: McuUpdateDataAddr   = %x\n", TxtDxeCtx->TxtInfoData->McuUpdateDataAddr));
  DEBUG ((EFI_D_INFO, "TXTDXE: SinitAcmBase        = %x\n", TxtDxeCtx->TxtInfoData->SinitAcmBase));
  DEBUG ((EFI_D_INFO, "TXTDXE: SinitAcmSize        = %x\n", TxtDxeCtx->TxtInfoData->SinitAcmSize));
  DEBUG ((EFI_D_INFO, "TXTDXE: TgaSize             = %x\n", TxtDxeCtx->TxtInfoData->TgaSize));
  DEBUG ((EFI_D_INFO, "TXTDXE: TxtLcpPdBase        = %x\n", TxtDxeCtx->TxtInfoData->TxtLcpPdBase));
  DEBUG ((EFI_D_INFO, "TXTDXE: TxtLcpPdSize        = %x\n", TxtDxeCtx->TxtInfoData->TxtLcpPdSize));
  DEBUG ((EFI_D_INFO, "TXTDXE: Flags               = %x\n", TxtDxeCtx->TxtInfoData->Flags));
  return EFI_SUCCESS;
}

/**
  Determines whether or not the platform has executed an TXT launch by
  examining the TPM Establishment bit.

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval TRUE          - If the TPM establishment bit is asserted.
  @retval FALSE         - If the TPM establishment bit is unasserted.
**/
BOOLEAN
IsTxtEstablished (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx
  )
{
  EFI_STATUS Status;
  UINT8      AccessReg;
  UINT16     TimeOutCount;

  ///
  /// TO-DO: We might locate TCG protocol to access TPM status
  ///
  ///
  /// Set TPM.ACCESS polling timeout about 750ms
  ///
  TimeOutCount = TPM_TIME_OUT;
  do {
    ///
    /// Read TPM status register
    ///
    Status = TxtDxeCtx->CpuIo->Mem.Read (
                    TxtDxeCtx->CpuIo,
                    EfiCpuIoWidthUint8,
                    (UINT64) TPM_STATUS_REG_ADDRESS,
                    1,
                    &AccessReg
                    );
    ASSERT_EFI_ERROR (Status);
    ///
    /// if TPM.Access == 0xFF, TPM is not present
    ///
    if (AccessReg == 0xFF) {
      return FALSE;
    }
    ///
    /// Check tpmRegValidSts bit before checking establishment bit
    ///
    if ((AccessReg & 0x80) != 0x80) {
      ///
      /// Delay 1ms and read again
      ///
      gBS->Stall (1000);
    } else {
      ///
      /// tpmRegValidSts set, we can check establishment bit now.
      ///
      break;
    }

    TimeOutCount--;
  } while (TimeOutCount != 0);

  ///
  /// if tpmRegValidSts is not set, TPM is not usable
  ///
  if ((AccessReg & 0x80) != 0x80) {
    DEBUG ((EFI_D_ERROR, "TXTDXE: TPM Valid Status is not set!! TPM.ACCESS=%x\n", AccessReg));
    ASSERT (TRUE);
    EFI_DEADLOOP ();
  }
  ///
  /// The bit we're interested in uses negative logic:
  /// If bit 0 == 1 then return False
  /// Else return True
  ///
  return (AccessReg & 0x1) ? FALSE : TRUE;
}

/**
  Determines whether or not the platform has executed an TXT launch by
  examining the TPM Establishment bit.

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval TRUE          - If the TPM establishment bit is asserted.
  @retval FALSE         - If the TPM establishment bit is unasserted.
**/
BOOLEAN
IsTxtResetSet (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx
  )
{
  EFI_STATUS Status;
  UINT8      EstsReg;

  ///
  /// TO-DO: We might locate TCG protocol to access TPM status
  ///
  ///
  /// Read TPM status register
  ///
  Status = TxtDxeCtx->CpuIo->Mem.Read (
                  TxtDxeCtx->CpuIo,
                  EfiCpuIoWidthUint8,
                  (UINT64) TXT_PUBLIC_BASE + TXT_ERROR_STATUS_REG_OFF,
                  1,
                  &EstsReg
                  );
  ASSERT_EFI_ERROR (Status);

  return (EstsReg & 0x1) ? TRUE : FALSE;
}

/**
  Determines whether or not the platform requires initialization for TXT use.

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval TRUE          - If the the platoform should be configured for TXT.
  @retval FALSE         - If TXT is not to be used.
**/
BOOLEAN
IsTxtEnabled (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx
  )
{

  UINT64        Ia32FeatureControl;
  TXT_INFO_DATA *TxtInfoData;

  TxtInfoData = TxtDxeCtx->TxtInfoData;

  ///
  /// If TxtInfoHob reported TXT disabled, return FALSE to indicate TXT should be be used
  ///
  if (TxtInfoData->TxtMode == 0) {
    return FALSE;
  }

  Ia32FeatureControl = AsmReadMsr64 (MSR_IA32_FEATURE_CONTROL);

  return ((Ia32FeatureControl & TXT_OPT_IN_VMX_AND_SMX_MSR_VALUE) == TXT_OPT_IN_VMX_AND_SMX_MSR_VALUE) ? TRUE : FALSE;
}

/**
  Determines whether or not the current processor is TXT Capable.

  @retval TRUE          - If the current processor supports TXT
  @retval FALSE         - If the current processor does not support TXT
**/
BOOLEAN
IsTxtProcessor (
  VOID
  )
{
  EFI_CPUID_REGISTER CpuidRegs;

  AsmCpuid (1, &CpuidRegs.RegEax, &CpuidRegs.RegEbx, &CpuidRegs.RegEcx, &CpuidRegs.RegEdx);

  return (CpuidRegs.RegEcx & B_CPUID_VERSION_INFO_ECX_SME) ? TRUE : FALSE;
}

/**
  Add extened elements to BiosOsData

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure
  @param[in] Type        - The element's type
  @param[in] Buffer      - A pointer to buffer which need to append the element
  @param[in] Size        - return the size of the appened element.

  @retval None
**/
VOID
AppendElement (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx,
  IN UINT32              Type,
  IN VOID                *Buffer,
  OUT UINT32             *Size
  )
{
  VOID   *Element;
  UINT32 NumberOfAcm;
  UINT64 *AcmBase;

  NumberOfAcm = 1;
  AcmBase     = NULL;
  Element     = NULL;
  *Size       = 0;

  switch (Type) {
    case HEAP_EXTDATA_TYPE_BIOS_SPEC_VER:
      ///
      /// Fill BIOS spec ver element
      ///
      Element = AllocatePool (sizeof (HEAP_BIOS_SPEC_VER_ELEMENT));

      if (Element != NULL) {
        *Size = sizeof (HEAP_BIOS_SPEC_VER_ELEMENT);
        ((HEAP_BIOS_SPEC_VER_ELEMENT *) Element)->Header.Type     = HEAP_EXTDATA_TYPE_BIOS_SPEC_VER;
        ((HEAP_BIOS_SPEC_VER_ELEMENT *) Element)->Header.Size     = sizeof (HEAP_BIOS_SPEC_VER_ELEMENT);
        ((HEAP_BIOS_SPEC_VER_ELEMENT *) Element)->SpecVerMajor    = TXT_BIOS_SPEC_VER_MAJOR;
        ((HEAP_BIOS_SPEC_VER_ELEMENT *) Element)->SpecVerMinor    = TXT_BIOS_SPEC_VER_MINOR;
        ((HEAP_BIOS_SPEC_VER_ELEMENT *) Element)->SpecVerRevision = TXT_BIOS_SPEC_VER_REVISION;
      }
      break;

    case HEAP_EXTDATA_TYPE_BIOSACM:
      ///
      /// Fill BIOS ACM element
      ///
      Element = AllocatePool (sizeof (HEAP_BIOSACM_ELEMENT) + NumberOfAcm * sizeof (UINT64));

      if (Element != NULL) {
        *Size = sizeof (HEAP_BIOSACM_ELEMENT) + sizeof (UINT64) * NumberOfAcm;
        ((HEAP_BIOSACM_ELEMENT *) Element)->Header.Type = HEAP_EXTDATA_TYPE_BIOSACM;
        ((HEAP_BIOSACM_ELEMENT *) Element)->Header.Size = sizeof (HEAP_BIOSACM_ELEMENT) + NumberOfAcm * sizeof (UINT64);
        ((HEAP_BIOSACM_ELEMENT *) Element)->NumAcms     = NumberOfAcm;
        AcmBase = (UINT64 *) ((UINTN) Element + sizeof (HEAP_BIOSACM_ELEMENT));
        *AcmBase = TxtDxeCtx->TxtInfoData->BiosAcmBase;
      }
      break;

    case HEAP_EXTDATA_TYPE_END:
      ///
      /// Fill end type element
      ///
      Element = AllocatePool (sizeof (HEAP_EXT_DATA_ELEMENT));

      if (Element != NULL) {
        *Size = sizeof (HEAP_EXT_DATA_ELEMENT);
        ((HEAP_EXT_DATA_ELEMENT *) Element)->Type = HEAP_EXTDATA_TYPE_END;
        ((HEAP_EXT_DATA_ELEMENT *) Element)->Size = sizeof (HEAP_EXT_DATA_ELEMENT);
      }
      break;

    default:
      break;
  }

  if (Element != NULL) {
    CopyMem (Buffer, Element, *Size);
    FreePool (Element);
  }
}

//[-start-110215-IB06690094-add]//
EFI_STATUS
FindAndFillSinitAcm (
  IN OUT  TXT_DXE_LIB_CONTEXT *TxtDxeCtx
  )
{
  EFI_STATUS                    Status;
  UINTN                         i;
  UINT16                        TxtDeviceId;
  UINT64                        TxtSinitMemoryBase;
  UINT64                        TxtSinitMemorySize;

  UINTN                         NoHandles;
  EFI_HANDLE                    *Buffer;
  UINT8                         *SinitBuffer;
  UINTN                         Size;
  UINT32                        FvStatus;
  EFI_FV_FILETYPE               FileType;
  EFI_FIRMWARE_VOLUME_PROTOCOL  *FwVol;
  EFI_FV_FILE_ATTRIBUTES        Attributes;

  DEBUG ((EFI_D_INFO, "FindAndFillSinitAcm () start.\n"));

  //
  // Initialize local variables
  //
  Status              = EFI_SUCCESS;
  TxtDeviceId         = *(UINT16 *)(UINTN) (TXT_PUBLIC_BASE + TXT_DEVICE_ID_REG_OFF);
  TxtSinitMemoryBase  = *(UINT64 *)(UINTN) (TXT_PUBLIC_BASE + TXT_SINIT_BASE_REG_OFF);
  TxtSinitMemorySize  = *(UINT64 *)(UINTN) (TXT_PUBLIC_BASE + TXT_SINIT_SIZE_REG_OFF);
  Buffer              = NULL;
  SinitBuffer         = NULL;
  Size                = 0;
  FvStatus            = 0;
  FileType            = 0;
  FwVol               = NULL;

  //
  // TXT.SINIT.BASE/TXT.SINIT.SIZE registers should be programmed before call this function
  //
  if (TxtSinitMemoryBase == 0 || TxtSinitMemorySize == 0) {
    DEBUG ((EFI_D_ERROR, "TXT.SINIT.BASE/TXT.SINIT.SIZE device memory not assigned!!"));
    return EFI_OUT_OF_RESOURCES;
  }

  //
  // SINIT ACM should be 4K aligned.
  //
  if ((TxtSinitMemoryBase & (EFI_PAGE_SIZE - 1)) != 0) {
    DEBUG ((EFI_D_ERROR, "TXT.SINIT.BASE not 4k alignment!!"));
    return EFI_UNSUPPORTED;
  }

  //
  // If no information about placement of TXT BIOS ACM has been
  // passed from PEI - find it and load into memory dynamically.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolumeProtocolGuid,
                  NULL,
                  &NoHandles,
                  &Buffer
                  );

  if (EFI_ERROR(Status)) {
    return Status;
  }

  for (i = 0; i < NoHandles; i++) {

    Status = gBS->HandleProtocol (
                    Buffer[i],
                    &gEfiFirmwareVolumeProtocolGuid,
                    (VOID **)&FwVol
                    );

    ASSERT_EFI_ERROR (Status);

    //
    // No output buffer - get size only
    //
    Status = FwVol->ReadFile (
                      FwVol,
                      &gTxtSinitAcmPeiFileGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );
    if (Status == EFI_SUCCESS) {
      break;
    }
  }

  ASSERT (Size);
  //
  // Assert if file not found.
  //
  FreePool (Buffer);

  if (FwVol == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // SINIT ACM larger than SINIT memory size
  //
  if (Size > TxtSinitMemorySize) {
    DEBUG ((EFI_D_ERROR, "SINIT ACM file size more than TXT.SINIT.SIZE device memory size!!"));
    return EFI_OUT_OF_RESOURCES;
  }

  Status = (gBS->AllocatePool) (EfiBootServicesCode, Size, &SinitBuffer);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Read SINIT ACM into DPR.
  //
  Status = FwVol->ReadFile (
                    FwVol,
                    &gTxtSinitAcmPeiFileGuid,
                    &((VOID *) SinitBuffer),
                    &Size,
                    &FileType,
                    &Attributes,
                    &FvStatus
                    );

  if ((Status == EFI_SUCCESS)) {
    if (GetSinitAcmChipsetId (SinitBuffer, Size) == TxtDeviceId) {
      // Copy SINIT ACM into DPR.
      CopyMem ((VOID *)(UINTN) TxtSinitMemoryBase, SinitBuffer, Size);

      TxtDxeCtx->TxtInfoData->SinitAcmBase  = TxtSinitMemoryBase;
      TxtDxeCtx->TxtInfoData->SinitAcmSize  = Size;
    } else {
      Status = EFI_NOT_FOUND;
    }
  }
  
  if (EFI_ERROR (Status)) {
    TxtDxeCtx->TxtInfoData->SinitAcmBase  = 0;
    TxtDxeCtx->TxtInfoData->SinitAcmSize  = 0;
  }

  DEBUG ((EFI_D_INFO, "FindAndFillSinitAcm () end.\n"));

  FreePool (SinitBuffer);

  return Status;
}
//[-end-110215-IB06690094-add]//
EFI_STATUS
SetupTxtDeviceMemory (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx
  )
{
  EFI_PHYSICAL_ADDRESS TopAddr;
  UINT64               *Ptr64;
  UINT64               Value64;
  UINT32               Value32;
  UINT64               TxtHeapMemoryBase;
  UINT64               TxtSinitMemoryBase;
  BOOLEAN              Locked;
  BIOS_OS_DATA_REGION  *BiosOsDataRegion;
  TXT_INFO_DATA        *TxtInfoData;
  UINT8                *Ptr8;
//[-start-130120-IB06690094-add]//
  EFI_STATUS           Status;
//[-end-130120-IB06690094-add]//

  TxtHeapMemoryBase   = 0;
  TxtSinitMemoryBase  = 0;
  Locked              = FALSE;
  Ptr8                = NULL;
  Value32             = 0;

  TxtInfoData         = TxtDxeCtx->TxtInfoData;

  if ((TxtInfoData == 0) ||
      (TxtInfoData->TxtDprMemoryBase == 0) ||
      (TxtInfoData->TxtDprMemorySize == 0) ||
      (TxtInfoData->TxtHeapMemorySize == 0) ||
      (TxtInfoData->SinitMemorySize == 0)
      ) {
    return EFI_UNSUPPORTED;
  } else {
    ///
    /// Use address passed from PEI
    ///
    TopAddr             = TxtInfoData->TxtDprMemoryBase + TxtInfoData->TxtDprMemorySize;

    TxtHeapMemoryBase   = (UINT64) (TopAddr - TxtInfoData->TxtHeapMemorySize);

    TxtSinitMemoryBase  = TxtHeapMemoryBase - TxtInfoData->SinitMemorySize;
  }
  ///
  /// Program TXT Device Memory Chipset Registers and record them in
  /// BootScript so they will be saved and restored on S3
  ///
  ASSERT ((TopAddr & 0x0FFFFF) == 0);

  ///
  /// DPR registers
  ///
  Ptr64   = (UINT64 *) (UINTN) (TXT_PUBLIC_BASE + TXT_DPR_SIZE_REG_OFF);
  Value64 = RShiftU64 (TxtInfoData->TxtDprMemorySize, 16) | 1;
  *Ptr64 = Value64 | TopAddr;
  ///
  /// Assert error if programmed value is different from requested. This
  /// means error is requested size.
  ///
  Value64 = *Ptr64;
  ASSERT ((LShiftU64 ((Value64 & 0xFFE), 16)) == TxtInfoData->TxtDprMemorySize);

  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINT64) (UINTN) (Ptr64),
    2,
    &Value64
    );

  ///
  /// HEAP Registers
  /// Program size register first
  ///
  Ptr64 = (UINT64 *) (UINTN) (TXT_PUBLIC_BASE + TXT_HEAP_SIZE_REG_OFF);

  ///
  /// Test register locked status. If locked, skip programming since
  /// this will be done by BIOS ACM
  ///
  *Ptr64  = TEST_PATTERN;
  Value64 = *Ptr64;
  if (Value64 != TEST_PATTERN) {
    Locked = TRUE;
  } else {
    ///
    /// To be safe invert pattern and try again
    ///
    *Ptr64  = (UINT64) ~TEST_PATTERN;
    Value64 = *Ptr64;
    if (Value64 != (UINT64) ~TEST_PATTERN) {
      Locked = TRUE;
    }
  }

  if (!Locked) {

    *Ptr64 = TxtInfoData->TxtHeapMemorySize;
    ///
    /// Assert error if programmed value is different from requested. This
    /// means error is requested size.
    ///
    Value64 = *Ptr64;
    ASSERT (Value64 == TxtInfoData->TxtHeapMemorySize);

    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINT64) (UINTN) (Ptr64),
      2,
      &Value64
      );

    ///
    /// Program base register.
    ///
    Ptr64   = (UINT64 *) (UINTN) (TXT_PUBLIC_BASE + TXT_HEAP_BASE_REG_OFF);
    *Ptr64  = TxtHeapMemoryBase;

    ///
    /// Assert error if programmed value is different from requested. This
    /// means error is requested size.
    ///
    Value64 = *Ptr64;
    ASSERT (Value64 == TxtHeapMemoryBase);

    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      (UINT64) (UINTN) (Ptr64),
      2,
      &Value64
      );
  }
  ///
  /// SINIT Registers
  /// Program size register first
  ///
  Ptr64   = (UINT64 *) (UINTN) (TXT_PUBLIC_BASE + TXT_SINIT_SIZE_REG_OFF);
  *Ptr64  = TxtInfoData->SinitMemorySize;
  ///
  /// Assert error if programmed value is different from requested. This
  /// means error is requested size.
  ///
  Value64 = *Ptr64;
  ASSERT (Value64 == TxtInfoData->SinitMemorySize);

  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINT64) (UINTN) (Ptr64),
    2,
    &Value64
    );

  ///
  /// Program base register
  ///
  Ptr64   = (UINT64 *) (UINTN) (TXT_PUBLIC_BASE + TXT_SINIT_BASE_REG_OFF);
  *Ptr64  = TxtSinitMemoryBase;
  ///
  /// Assert error if programmed value is different from requested. This
  /// means error is requested size.
  ///
  Value64 = *Ptr64;
  ASSERT (Value64 == TxtSinitMemoryBase);

  S3BootScriptSaveMemWrite (
    S3BootScriptWidthUint32,
    (UINT64) (UINTN) (Ptr64),
    2,
    &Value64
    );

  ///
  /// Make sure TXT Device Memory has been zeroed
  ///
  ZeroMem (
    (VOID *) ((UINTN) TxtSinitMemoryBase),
    (UINTN) (TopAddr - TxtSinitMemoryBase)
    );

  if (TxtInfoData->TgaSize) {
    ;
    ///
    /// Placeholder for Trusted graphics support
    ///
  }

  Ptr64   = (UINT64 *) TxtHeapMemoryBase;
  *Ptr64  = sizeof (BIOS_OS_DATA_REGION);
  ///
  /// BiosOsDataSize plus sizew of data size feld itself
  ///
  BiosOsDataRegion                          = (BIOS_OS_DATA_REGION *) (Ptr64 + 1);
  BiosOsDataRegion->Version                 = BIOS_OS_DATAREGION_VERSION;
//[-start-110215-IB06690094-modify]//
//  BiosOsDataRegion->BiosSinitSize           = 0;
  if (TxtInfoData->SinitAcmBase == 0 || TxtInfoData->SinitAcmSize == 0) {
    Status = FindAndFillSinitAcm (TxtDxeCtx);
    DEBUG ((EFI_D_ERROR, "FindAndFillSinitAcm Status (%r)\n", Status));
  }
  BiosOsDataRegion->BiosSinitSize           = (UINT32) TxtInfoData->SinitAcmSize;
//[-end-110215-IB06690094-modify]//
  BiosOsDataRegion->LcpPdBase               = TxtInfoData->TxtLcpPdBase;
  BiosOsDataRegion->LcpPdSize               = TxtInfoData->TxtLcpPdSize;
  BiosOsDataRegion->NumOfLogicalProcessors  = (UINT32) (TxtDxeCtx->CpuCount);
  BiosOsDataRegion->Flags                   = TxtInfoData->Flags;
  Ptr8 = (UINT8 *) (UINTN) &(BiosOsDataRegion->ExtData);
  AppendElement (TxtDxeCtx, HEAP_EXTDATA_TYPE_BIOS_SPEC_VER, Ptr8, &Value32);
  Ptr8 += Value32;
  AppendElement (TxtDxeCtx, HEAP_EXTDATA_TYPE_BIOSACM, Ptr8, &Value32);
  Ptr8 += Value32;
  AppendElement (TxtDxeCtx, HEAP_EXTDATA_TYPE_END, Ptr8, &Value32);
  Value64 = (UINT64) Ptr8 - TxtHeapMemoryBase + Value32;
  *Ptr64  = Value64;

  return EFI_SUCCESS;
}

/**
  Adds and allocates architecturally defined TXT Configuration Space memory
  region to GCD.

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - Always.
**/
EFI_STATUS
ReserveTxtConfigSpace (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx
  )
{
  EFI_STATUS           Status;
  EFI_PHYSICAL_ADDRESS BaseAddr;

  BaseAddr = TXT_PRIVATE_BASE;
  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeReserved,
                  TXT_PRIVATE_BASE,
                  TXT_CONFIG_SPACE_LENGTH,
                  0
                  );

  ASSERT_EFI_ERROR (Status);

  Status = gDS->AllocateMemorySpace (
                  EfiGcdAllocateAddress,
                  EfiGcdMemoryTypeReserved,
                  12, ///< 4K Boundary
                  TXT_CONFIG_SPACE_LENGTH,
                  &BaseAddr,
                  TxtDxeCtx->ImageHandle,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}

/**
  Invokes TxtDxeLibLaunchBiosAcm to execute the SCHECK function.

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - Always.
**/
EFI_STATUS
DoScheck (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx
  )
{
  return TxtDxeLibLaunchBiosAcm (TxtDxeCtx, TXT_LAUNCH_SCHECK);
}

/**
  Invokes TxtDxeLibLaunchBiosAcm to reset the TPM's establishment bit.

  @param[in] TxtDxeCtx      - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - Always.
**/
EFI_STATUS
ResetTpmEstBit (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx
  )
{
  return TxtDxeLibLaunchBiosAcm (TxtDxeCtx, TXT_RESET_EST_BIT);
}

/**
  Sets up the system and then launches the TXT BIOS ACM to run the function
  requested by AcmFunction.

  @param[in] AcmFunction   - Constant that represents the function from the BIOS ACM
                  that should be executed.
  @param[in] TxtDxeCtx     - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS            - BIOS ACM is set up.
  @retval EFI_INVALID_PARAMETER  - Wrong data in TxtInfoHob.
  @retval EFI_NOT_FOUND          - BIOS ACM is not found
**/
EFI_STATUS
TxtDxeLibLaunchBiosAcm (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx,
  IN UINT64              AcmFunction
  )
{
  EFI_STATUS               Status;
  EFI_TPL                  OldTpl;
  UINTN                    CpuCount;
  UINTN                    MyCpuNumber;
  UINTN                    Index;
  UINTN                    i;
  FRAMEWORK_EFI_MP_SERVICES_PROTOCOL      *MpService;

  UINTN                        NoHandles;
  EFI_HANDLE                   *Buffer;
  UINTN                        Size;
  UINT32                       FvStatus;
  EFI_FV_FILETYPE              FileType;
  EFI_FIRMWARE_VOLUME_PROTOCOL *FwVol;
  EFI_FV_FILE_ATTRIBUTES       Attributes;
  UINTN                        NumPages;
  EFI_PHYSICAL_ADDRESS         Addr;
  EFI_PHYSICAL_ADDRESS         AlignedAddr;

  FwVol = NULL;
  ///
  /// Initialize local variables
  ///
  CpuCount  = TxtDxeCtx->CpuCount;
  MpService = TxtDxeCtx->MpService;
  Size      = 0;
  FvStatus  = 0;
  NumPages  = 0;
  Addr      = 0;

  if (TxtDxeCtx->TxtInfoData == NULL) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Get current running CPU number
  ///
  Status = MpService->WhoAmI (
                  MpService,
                  &MyCpuNumber
                  );
  ASSERT_EFI_ERROR (Status);

  if ((TxtDxeCtx->TxtInfoData->BiosAcmBase == 0) || (TxtDxeCtx->TxtInfoData->BiosAcmSize == 0)) {
    ///
    /// If no information about placement of TXT BIOS ACM has been
    /// passed from PEI - find it and load into memory dynamically.
    ///
    Status = gBS->LocateHandleBuffer (
                    ByProtocol,
                    &gEfiFirmwareVolumeProtocolGuid,
                    NULL,
                    &NoHandles,
                    &Buffer
                    );

    if (EFI_ERROR (Status)) {
      return Status;
    }

    for (i = 0; i < NoHandles; i++) {

      Status = gBS->HandleProtocol (
                      Buffer[i],
                      &gEfiFirmwareVolumeProtocolGuid,
                      (VOID **) &FwVol
                      );

      ASSERT_EFI_ERROR (Status);

      ///
      /// No output buffer - get size only
      ///
      Status = FwVol->ReadFile (
                      FwVol,
                      &gTxtBiosAcmPeiFileGuid,
                      NULL,
                      &Size,
                      &FileType,
                      &Attributes,
                      &FvStatus
                      );
      if (Status == EFI_SUCCESS) {
        break;
      }
    }

    ASSERT (Size);
    ///
    /// Assert if file not found.
    ///
    FreePool (Buffer);

    if (FwVol == NULL) {
      return EFI_NOT_FOUND;
    }
    ///
    /// Allocate 4K aligned memory to load BIOS ACM.  For this allocate
    /// 1 page more than BIOS ACM size.
    ///
    NumPages = (Size + EFI_PAGE_SIZE - 1) / EFI_PAGE_SIZE;

    ///
    /// Allocate buffer for BIOS ACM
    ///
    Status = (gBS->AllocatePages) (AllocateAnyPages, EfiRuntimeServicesData, NumPages, &Addr);

    ASSERT_EFI_ERROR (Status);

    AlignedAddr = Addr &~(EFI_PAGE_SIZE - 1);
    AlignedAddr = AlignedAddr < Addr ? (AlignedAddr + EFI_PAGE_SIZE) : AlignedAddr;

    ///
    /// Read BIOS ACM into prepared buffer.
    ///
    Status = FwVol->ReadFile (
                    FwVol,
                    &gTxtBiosAcmPeiFileGuid,
                    ((VOID **) &AlignedAddr),
                    &Size,
                    &FileType,
                    &Attributes,
                    &FvStatus
                    );
  } else {
    ///
    /// Use address passed from PEI
    ///
    AlignedAddr = TxtDxeCtx->TxtInfoData->BiosAcmBase;
  }
  ///
  /// Save AP configuration. Run on one AP since all must be programmed
  /// identically
  ///
  MpService->StartupThisAP (
          MpService,
          (EFI_AP_PROCEDURE) ApSaveConfig,
          1,
          NULL,
          0,
          NULL
          );

  ///
  /// Initialize APs. Prepare data for DoApInit
  ///
  mAcmBase  = AlignedAddr;
  mMcuAddr  = TxtDxeCtx->TxtInfoData->McuUpdateDataAddr;

  DisableSmiSources (TxtDxeCtx, TRUE);

#if defined(TXT_RLP_INIT) && (TXT_RLP_INIT == 1)
  ///
  /// Execute DoApInit on every AP
  ///
  MpService->StartupAllAPs (
          MpService,
          (EFI_AP_PROCEDURE) DoApInit,
          TRUE,
          NULL,
          MP_TIMEOUT_FOR_STARTUP_ALL_APS,
          NULL,
          NULL
          );
#endif
  ///
  /// Disable every AP and put in WFS state
  ///
  for (Index = 0; Index < CpuCount; Index++) {
    if (Index != MyCpuNumber) {
      ///
      /// Halt CPU otherwise it will not be re-enabled
      ///
      Status = MpService->EnableDisableAP (
                      MpService,
                      Index,
                      FALSE,
                      NULL
                      );
      ASSERT_EFI_ERROR (Status);

      Status = MpService->SendIPI (
                      MpService,
                      Index,
                      0,
                      DELIVERY_MODE_INIT
                      );
      ASSERT_EFI_ERROR (Status);
    }
  }
  ///
  /// Launch the BIOS ACM to run the requested function
  ///
  OldTpl = gBS->RaiseTPL (TPL_HIGH_LEVEL);

  LaunchBiosAcm (AlignedAddr, AcmFunction);

  gBS->RestoreTPL (OldTpl);

  ///
  /// Free memory only if it was allocated
  ///
  if (Addr != 0) {
    (gBS->FreePages)(Addr, NumPages);
  }
  ///
  /// Restart APs
  ///
  for (Index = 0; Index < CpuCount; Index++) {
    if (Index != MyCpuNumber) {
      Status = MpService->EnableDisableAP (
                      MpService,
                      Index,
                      TRUE,
                      NULL
                      );
      ASSERT_EFI_ERROR (Status);
    }
  }
  ///
  /// Restore AP configuration
  ///
  Status = MpService->StartupAllAPs (
                  MpService,
                  (EFI_AP_PROCEDURE) ApRestoreConfig,
                  TRUE,
                  NULL,
                  MP_TIMEOUT_FOR_STARTUP_ALL_APS,
                  NULL,
                  NULL
                  );

  ASSERT_EFI_ERROR (Status);

  DisableSmiSources (TxtDxeCtx, FALSE);

  return EFI_SUCCESS;
}

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
  )
{
  EFI_STATUS    Status;
  UINT64        GlobalSmiControlIoAddr;
  UINT32        LocalApicBaseAddr;
  static UINT64 SavedIa32ThermInterruptMSR;
  static UINT32 SavedSmiControl;
  static UINT32 SavedApicThermalValue;
  BOOLEAN       x2ApicEnabled;

  x2ApicEnabled           = (BOOLEAN) (((AsmReadMsr64 (MSR_IA32_APIC_BASE)) & (BIT11 + BIT10)) == BIT11 + BIT10);
  GlobalSmiControlIoAddr  = TxtDxeCtx->TxtInfoData->PmBase + 0x30;
  LocalApicBaseAddr       = ((UINT32) AsmReadMsr64 (MSR_IA32_APIC_BASE)) & BASE_ADDR_MASK;

  if (Operation == TRUE) {
    ///
    /// Save IA32_THERMAL_INTERRUPT MSR and disable the interrupts
    ///
    SavedIa32ThermInterruptMSR = AsmReadMsr64 ((UINT32) IA32_THERM_INTERRUPT);
    AsmWriteMsr64 (
            (UINT32) IA32_THERM_INTERRUPT,
            (UINT64) (SavedIa32ThermInterruptMSR &~(BIT0 + BIT1 + BIT2 + BIT4 + BIT15 + BIT23))
            );
    ///
    /// Save THERMAL LVT in local APIC and mask THERMAL LVT
    ///
    if (x2ApicEnabled) {
      SavedApicThermalValue = (UINT32) AsmReadMsr64 (MSR_EXT_XAPIC_LVT_THERM);
      AsmWriteMsr64 (
              MSR_EXT_XAPIC_LVT_THERM,
              ((SavedApicThermalValue &~(B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) | (B_INTERRUPT_MASK | V_MODE_SMI))
              );
    } else {
      SavedApicThermalValue = *(UINT32 *) (UINTN) (LocalApicBaseAddr + LOCAL_APIC_THERMAL_DEF);
      *(UINT32 *) (UINTN) (LocalApicBaseAddr + LOCAL_APIC_THERMAL_DEF) = (UINT32) ((SavedApicThermalValue &~(B_INTERRUPT_MASK | B_DELIVERY_MODE | B_VECTOR)) | (B_INTERRUPT_MASK | V_MODE_SMI));
    }
    ///
    /// Save SMI control register and try to disable all SMIs indivitually.
    ///
    SavedSmiControl = IoRead32 ((UINTN) GlobalSmiControlIoAddr);
    ///
    /// We can not disable Global SMI since it should be locked after SCHECK. we can only disable SMI sources indivitually.
    /// Call to TxtDxeOemDisableSmi() for platform specific SMIs.
    ///
    Status = TxtDxeOemDisableSmi (TxtDxeCtx, TRUE);
    ASSERT_EFI_ERROR (Status);

    IoWrite32 (
            (UINTN) GlobalSmiControlIoAddr,
            (UINT32) (SavedSmiControl & 0x01)
            );
  } else {
    ///
    /// We can not disable Global SMI since it should be locked after SCHECK. we can only disable SMI sources indivitually.
    /// Restore original SMI setting after SCHECK
    /// Call to TxtDxeOemDisableSmi() for platform specific SMIs.
    ///
    Status = TxtDxeOemDisableSmi (TxtDxeCtx, FALSE);
    ASSERT_EFI_ERROR (Status);

    IoWrite32 (
            (UINTN) GlobalSmiControlIoAddr,
            (UINT32) (SavedSmiControl)
            );
    ///
    /// Restore IA32_THERMAL_INTERRUPT MSR
    ///
    AsmWriteMsr64 (
            (UINT32) IA32_THERM_INTERRUPT,
            (UINT64) SavedIa32ThermInterruptMSR
            );
    if (x2ApicEnabled) {
      AsmWriteMsr64 (MSR_EXT_XAPIC_LVT_THERM, SavedApicThermalValue);
    } else {
      *(UINT32 *) (UINTN) (LocalApicBaseAddr + LOCAL_APIC_THERMAL_DEF) = (UINT32) SavedApicThermalValue;

    }
  }

  return EFI_SUCCESS;
}

/**
  Read policy protocol to reset AUX content

  @param[in] TxtDxeCtx - A pointer to an initialized TXT DXE Context data structure

  @retval EFI_SUCCESS   - No error happend
  @retval EFI_NOT_FOUND - TxtPolicyProtocol is not found
**/
EFI_STATUS
ResetTpmAux (
  IN TXT_DXE_LIB_CONTEXT *TxtDxeCtx
  )
{
  EFI_STATUS Status;

  Status = EFI_SUCCESS;

  ///
  /// Check if TxtPolicy protocol installed
  ///
  if (TxtDxeCtx->CpuPlatformPolicy == NULL) {
    return EFI_NOT_FOUND;
  }
  ///
  ///
  ///
  if (TxtDxeCtx->CpuPlatformPolicy->SecurityConfig->TxtFunctionConfig->ResetAux == 1) {
    DEBUG ((EFI_D_INFO, "TXTDXE: Reset AUX content\n"));
    Status = TxtDxeLibLaunchBiosAcm (TxtDxeCtx, TXT_RESET_AUX);
  }

  return Status;
}
