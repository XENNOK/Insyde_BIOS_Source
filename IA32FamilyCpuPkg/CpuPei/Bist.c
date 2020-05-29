/*++
  This file contains an 'Intel Pre-EFI Module' and is licensed  
  for Intel CPUs and Chipsets under the terms of your license   
  agreement with Intel or your vendor.  This file may be        
  modified by the user, subject to additional terms of the      
  license agreement                                             
--*/
/** @file
Implementation of CPU driver for PEI phase.

  Copyright (c) 2006, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.                                       

Module Name: Bist.c

**/

#include "CpuPei.h"
#include "Bist.h"

EFI_SEC_PLATFORM_INFORMATION_PPI mSecPlatformInformationPpi = {
  SecPlatformInformation
};

EFI_PEI_PPI_DESCRIPTOR mPeiSecPlatformInformationPpi = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gEfiSecPlatformInformationPpiGuid,
  &mSecPlatformInformationPpi
};

/**

  Implementation of the PlatformInformation service
  
  Implementation of the PlatformInformation service in EFI_SEC_PLATFORM_INFORMATION_PPI.
  This function will parse SecPlatform Information from BIST Hob.

  @param  PeiServices                Pointer to the PEI Services Table.
  @param  StructureSize              Pointer to the variable describing size of the input buffer.
  @param  PlatformInformationRecord  Pointer to the EFI_SEC_PLATFORM_INFORMATION_RECORD.
  
  @retval EFI_SUCCESS                The data was successfully returned.
  @retval EFI_BUFFER_TOO_SMALL       The buffer was too small.
  
**/
EFI_STATUS
EFIAPI
SecPlatformInformation (
  IN CONST EFI_PEI_SERVICES                  **PeiServices,
  IN OUT UINT64                              *StructureSize,
     OUT EFI_SEC_PLATFORM_INFORMATION_RECORD *PlatformInformationRecord
  )
{
  EFI_HOB_GUID_TYPE       *GuidHob;
  VOID                    *DataInHob;
  UINTN                   DataSize;

  GuidHob = GetFirstGuidHob (&gEfiHtBistHobGuid);
  if (GuidHob == NULL) {
    *StructureSize = 0;
    return EFI_SUCCESS;
  }

  DataInHob = GET_GUID_HOB_DATA (GuidHob);
  DataSize  = GET_GUID_HOB_DATA_SIZE (GuidHob);

  //
  // return the information from BistHob
  //
  if ((*StructureSize) < (UINT64) DataSize) {
    *StructureSize = (UINT64) DataSize;
    return EFI_BUFFER_TOO_SMALL;
  }

  *StructureSize = (UINT64) DataSize;
   
  CopyMem (PlatformInformationRecord, DataInHob, DataSize);

  return EFI_SUCCESS;
}

/**
  A callback function to build CPU(only BSP) BIST. 

  This function is a callback function to build bsp's BIST by calling SecPlatformInformationPpi

  @param  PeiServices      Pointer to PEI Services Table      
  @param  NotifyDescriptor Address if the notification descriptor data structure 
  @param  Ppi              Address of the PPI that was installed     
  @retval EFI_SUCCESS      Retrieve of the BIST data successfully 
  @retval EFI_SUCCESS      No sec platform information ppi export   
  @retval EFI_SUCCESS      The boot mode is S3 path   
**/
EFI_STATUS
EFIAPI
BuildBistAndCpuInfoHob (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
{
  EFI_STATUS                            Status;
  EFI_BOOT_MODE                         BootMode;
  EFI_SEC_PLATFORM_INFORMATION_PPI      *SecPlatformInformationPpi;
  UINT64                                InformationSize;
  EFI_SEC_PLATFORM_INFORMATION_RECORD   *SecPlatformInformation;
  UINT32                                RegEax;
  UINT8                                 PhysicalAddressBits;
  EFI_PEI_PPI_DESCRIPTOR                *SecInformationDescriptor;

  Status = PeiServicesGetBootMode(&BootMode);
  ASSERT_EFI_ERROR (Status);

  //
  // locate the SEC platform information PPI
  //
  Status = PeiServicesLocatePpi (
             &gEfiSecPlatformInformationPpiGuid, // GUID
             0,                                  // INSTANCE
             &SecInformationDescriptor,          // EFI_PEI_PPI_DESCRIPTOR
             (VOID **)&SecPlatformInformationPpi // PPI
             );

  if (Status == EFI_NOT_FOUND) {
    return EFI_SUCCESS;
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // We do not build CPU Hobs on S3, because the data are only needed by DXE drivers.
  // But we still reinstall the Ppi to prevent system hang issue if someone want to locate it.
  //
  if (BootMode != BOOT_ON_S3_RESUME) {
    //
    // Get the size of the sec platform information(BIST data)
    //
    InformationSize         = 0;
    SecPlatformInformation  = NULL;
    Status = SecPlatformInformationPpi->PlatformInformation (
                                          (CONST EFI_PEI_SERVICES **) PeiServices,
                                          &InformationSize,
                                          SecPlatformInformation
                                          );
    if (Status == EFI_BUFFER_TOO_SMALL) {
      Status = PeiServicesAllocatePool (
                                (UINTN) InformationSize,
                                (VOID **) &SecPlatformInformation
                                );

      if (EFI_ERROR (Status)) {
        return Status;
      }
      //
      // Retrieve BIST data
      //
      Status = SecPlatformInformationPpi->PlatformInformation (
                                            (CONST EFI_PEI_SERVICES **) PeiServices,
                                            &InformationSize,
                                            SecPlatformInformation
                                            );
    }

    if (EFI_ERROR (Status)) {
      return Status;
    }

    BuildGuidDataHob (
      &gEfiHtBistHobGuid,
      SecPlatformInformation,
      (UINTN) InformationSize
    );

    BuildGuidDataHob (
      &gEfiSecPlatformInformationPpiGuid,
      SecPlatformInformation,
      (UINTN) InformationSize
    );

    //
    //Local APIC range
    //
    BuildResourceDescriptorHob (
      EFI_RESOURCE_MEMORY_MAPPED_IO,
      (EFI_RESOURCE_ATTRIBUTE_PRESENT    |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE),
      0xFEC80000,
      SIZE_512KB
      );
    BuildMemoryAllocationHob (
      0xFEC80000,
      SIZE_512KB,
      EfiMemoryMappedIO
      );

    //
    // build CPU memory space and IO space hob
    //
    AsmCpuid (0x80000000, &RegEax, NULL, NULL, NULL);
    if (RegEax >= 0x80000008) {
      AsmCpuid (0x80000008, &RegEax, NULL, NULL, NULL); 
      PhysicalAddressBits = (UINT8) RegEax;
    } else {
      PhysicalAddressBits  = 36;
    }
    //
    // Create a CPU hand-off information
    // 
    BuildCpuHob (PhysicalAddressBits, 16);
  }

  //
  // The old SecPlatformInformation Ppi is on CAR.
  // After memory discovered, we should never get it from CAR, or the data will be crashed.
  // So, we reinstall SecPlatformInformation PPI here.
  //
  Status = PeiServicesReInstallPpi (
                             SecInformationDescriptor,
                             &mPeiSecPlatformInformationPpi
                             );
  return Status;
}
