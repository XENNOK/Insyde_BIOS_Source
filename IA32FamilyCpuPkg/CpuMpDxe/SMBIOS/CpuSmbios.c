/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file
  Code to log processor and cache subclass data to smbios record.

  Copyright (c) 2006 - 2011, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

**/

#include "Processor.h"
#include "Cache.h"

EFI_SMBIOS_PROTOCOL     *mSmbios;
EFI_HII_HANDLE          mStringHandle;
UINT32                  mPopulatedSocketCount;

/**
  Add SMBIOS Processor Type and Cache Type tables for the CPU.
**/
VOID
AddCpuSmbiosTables (
  VOID
  )
{
  EFI_STATUS            Status;
  EFI_SMBIOS_HANDLE     L1CacheHandle;
  EFI_SMBIOS_HANDLE     L2CacheHandle;
  EFI_SMBIOS_HANDLE     L3CacheHandle;
  UINT32                PreviousPackageNumber;
  UINT32                PackageNumber;
  UINTN                 ProcessorIndex;
  UINTN                 *SocketProcessorNumberTable;
  UINT32                SocketIndex;

  L1CacheHandle = 0xFFFF;
  L2CacheHandle = 0xFFFF;
  L3CacheHandle = 0xFFFF;

  //
  // Initialize the mSmbios to contain the SMBIOS protocol,
  //
  Status = gBS->LocateProtocol (&gEfiSmbiosProtocolGuid, NULL, (VOID **) &mSmbios);
  ASSERT_EFI_ERROR (Status);

  //
  // Initialize strings to HII database
  //
  mStringHandle = HiiAddPackages (
                    &gEfiCallerIdGuid,
                    NULL,
                    CpuMpDxeStrings,
                    NULL
                    );
  ASSERT (mStringHandle != NULL);

  SocketProcessorNumberTable = AllocateZeroPool (mCpuConfigConextBuffer.NumberOfProcessors * sizeof (UINTN));
  ASSERT (SocketProcessorNumberTable != NULL);

  //
  // Detect populated sockets (comparing the processors' PackangeNumber) and record their ProcessorNumber.
  // For example:
  //   ProcessorNumber: 0 1 2 3 (PackageNumber 0) 4 5 6 7 (PackageNumber 1)
  //   And then, populated socket count will be 2 and record ProcessorNumber 0 for Socket 0, ProcessorNumber 4 for Socket 1
  //

  //
  // System has 1 populated socket at least, initialize mPopulatedSocketCount to 1 and record ProcessorNumber 0 for it.
  //
  mPopulatedSocketCount = 1;
  SocketProcessorNumberTable[0] = 0;
  GetProcessorLocation (0, &PreviousPackageNumber, NULL, NULL);

  //
  // Scan and compare the processors' PackageNumber to find the populated sockets.
  //
  for (ProcessorIndex = 1; ProcessorIndex < mCpuConfigConextBuffer.NumberOfProcessors; ProcessorIndex++) {
    GetProcessorLocation (ProcessorIndex, &PackageNumber, NULL, NULL);
    if (PackageNumber != PreviousPackageNumber) {
      //
      // Found a new populated socket.
      //
      PreviousPackageNumber = PackageNumber;
      mPopulatedSocketCount++;
      SocketProcessorNumberTable[mPopulatedSocketCount - 1] = ProcessorIndex;
    }
  }

  //
  // Add SMBIOS tables for populated sockets.
  //
  for (SocketIndex = 0; SocketIndex < mPopulatedSocketCount; SocketIndex++) {
    AddSmbiosCacheTypeTable (SocketProcessorNumberTable[SocketIndex], &L1CacheHandle, &L2CacheHandle, &L3CacheHandle);
    AddSmbiosProcessorTypeTable (SocketProcessorNumberTable[SocketIndex], L1CacheHandle, L2CacheHandle, L3CacheHandle);
  }
  FreePool (SocketProcessorNumberTable);

  //
  // Register notification functions for Smbios Processor Type.
  //
  SmbiosProcessorTypeTableCallback ();
}

