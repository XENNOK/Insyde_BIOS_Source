/** @file
  TIS (TPM Interface Specification) functions to access discrete TPM module.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <PiPei.h>

#include <Library/DebugLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PeimEntryPoint.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PerformanceLib.h>
#include <Library/BaseLib.h>
#include <Library/Tpm2TisCommLib.h>

#include <Ppi/Tpm2InterfacePei.h>

EFI_STATUS
TisPeiCommand (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This,
  IN     UINT8                            *BufferIn,
  IN     UINT32                           SizeIn,
  IN OUT UINT8                            *BufferOut,
  IN OUT UINT32                           *SizeOut
  )
{
  return TisTpmCommand (
           (TIS_PC_REGISTERS_PTR) TPM_BASE_ADDRESS,
           TIS_TIMEOUT_B,
           BufferIn,
           SizeIn,
           BufferOut,
           SizeOut
           );
}

EFI_STATUS
TisPeiRequestUseTpm (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This
  )
{
  return TisPcRequestUseTpm ((TIS_PC_REGISTERS_PTR) TPM_BASE_ADDRESS);
}

EFI_STATUS
TisPeiSend (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This,
  IN     UINT8                            *BufferIn,
  IN     UINT32                           SizeIn
  )
{
  return TisSend (
           (TIS_PC_REGISTERS_PTR) TPM_BASE_ADDRESS,
           BufferIn,
           SizeIn
           );
}

EFI_STATUS
TisPeiReceive (
  IN     EFI_PEI_SERVICES                 **PeiServices,
  IN     PEI_TPM2_INTERFACE_PPI           *This,
  IN OUT UINT8                            *BufferOut,
  IN OUT UINT32                           *SizeOut
  )
{
  return TisReceive (
           (TIS_PC_REGISTERS_PTR) TPM_BASE_ADDRESS,
           TIS_TIMEOUT_B,
           BufferOut,
           SizeOut
           );
}

PEI_TPM2_INTERFACE_PPI         mPeiTpm2InterfacePpi = {
  TisPeiCommand,
  TisPeiRequestUseTpm,
  TisPeiSend,
  TisPeiReceive
};

EFI_PEI_PPI_DESCRIPTOR   mInstallPeiTpm2InterfacePpi = {
  EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST,
  &gPeiTpm2InterfacePpiGuid,
  &mPeiTpm2InterfacePpi
};

/**
  Entry point of this module.

  @param[in] FileHandle   Handle of the file being invoked.
  @param[in] PeiServices  Describes the list of possible PEI Services.

  @return Status.

**/
EFI_STATUS
EFIAPI
PeimEntry (
  IN       EFI_PEI_FILE_HANDLE            FileHandle,
  IN CONST EFI_PEI_SERVICES               **PeiServices
  )
{
  EFI_STATUS                              Status;

  if (!TisPcPresenceCheck ((TIS_PC_REGISTERS_PTR) TPM_BASE_ADDRESS)) {
    DEBUG ((EFI_D_ERROR, "TPM not detected!\n"));
    return EFI_SUCCESS;
  }

  //
  // Install PPI
  //
  Status = PeiServicesInstallPpi (&mInstallPeiTpm2InterfacePpi);
  ASSERT_EFI_ERROR (Status);

  return EFI_SUCCESS;
}
