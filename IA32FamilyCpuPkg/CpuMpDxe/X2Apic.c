/*++
  This file contains an 'Intel Peripheral Driver' and is        
  licensed for Intel CPUs and chipsets under the terms of your  
  license agreement with Intel or your vendor.  This file may   
  be modified by the user, subject to additional terms of the   
  license agreement                                             
--*/
/** @file

  Code for x2APIC feature.

  Copyright (c) 2011 - 2012, Intel Corporation. All rights reserved.<BR>
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  Module Name:  X2Apic.c

**/

#include "X2Apic.h"

BOOLEAN mEnableX2Apic                 = FALSE;
BOOLEAN mEnableSocketIdReassignment   = FALSE;
GLOBAL_REMOVE_IF_UNREFERENCED UINT32  mNewApicId[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];
GLOBAL_REMOVE_IF_UNREFERENCED UINT8   mNewSocketId[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];
GLOBAL_REMOVE_IF_UNREFERENCED BOOLEAN mSocketIdAssigned[FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber)];

/**
  Detect if the processor is x2APIC capable.

  @retval TRUE the processor supports x2APIC.
  @retval FALSE  the processor does not support x2APIC.
**/
BOOLEAN
IsX2ApicSupported (
  VOID
  )
{
  UINT32 RegEcx;

  AsmCpuid (EFI_CPUID_VERSION_INFO, NULL, NULL, &RegEcx, NULL);
  if ((RegEcx & BIT21) != 0) {
    return TRUE;
  } else {
    return FALSE;
  }
}
 
/**
  Produces entry for x2APIC feature in Register Table for specified processor.
  
  This function produces entry for x2APIC feature in Register Table for specified processor.

  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
X2ApicReg (
  IN UINTN      ProcessorNumber
  )
{
  if (mEnableX2Apic) {
    WritePreSmmInitRegisterTable (ProcessorNumber, Msr, EFI_MSR_IA32_APIC_BASE, N_MSR_ENABLE_X2APIC_MODE, 1, 1);
  }
}

/**
  Get the version information of current processor.
  
  This function gets the version information of current processor,
  including family ID, model ID, stepping ID and processor type.

  @param  DisplayedFamily   Pointer to family ID for output
  @param  DisplayedModel    Pointer to model ID for output
  @param  SteppingId        Pointer to stepping ID for output
  @param  ProcessorType     Pointer to processor type for output

**/
VOID
EFIAPI
GetProcessorId (
  OUT UINT32              *DisplayedFamily OPTIONAL,
  OUT UINT32              *DisplayedModel  OPTIONAL,
  OUT UINT32              *SteppingId      OPTIONAL,
  OUT UINT32              *ProcessorType   OPTIONAL
  )
{
  UINT32                  RegEax;
  UINT32                  FamilyId;
  UINT32                  ExtendedFamilyId;
  UINT32                  ExtendedModelId;

  //
  // Get CPUID(1).EAX
  //
  AsmCpuid (EFI_CPUID_VERSION_INFO, &RegEax, NULL, NULL, NULL);

  //
  // Processor Type is CPUID(1).EAX[12:13]
  //
  if (ProcessorType != NULL) {
    *ProcessorType = BitFieldRead32 (RegEax, 12, 13);
  }

  //
  // Stepping ID is CPUID(1).EAX[0:3]
  //
  if (SteppingId != NULL) {
    *SteppingId = BitFieldRead32 (RegEax, 0, 3);
  }

  //
  // The Extended Family ID needs to be examined only when the Family ID is 0FH.
  // If Family ID is 0FH, Displayed Family ID = Family ID + Extended Family ID.
  // Otherwise, Displayed Family ID is Family ID
  //
  FamilyId = BitFieldRead32 (RegEax, 8, 11);
  if (DisplayedFamily != NULL) {
    *DisplayedFamily = FamilyId;
    if (FamilyId == 0x0f) {
      ExtendedFamilyId = BitFieldRead32 (RegEax, 20, 27);
      *DisplayedFamily += ExtendedFamilyId;
    }
  }

  //
  // The Extended Model ID needs to be examined only when the Family ID is 06H or 0FH.
  // If Family ID is 06H or 0FH, Displayed Model ID = Model ID + (Extended Model ID << 4).
  // Otherwise, Displayed Model ID is Model ID.
  //
  if (DisplayedModel != NULL) {
    *DisplayedModel = BitFieldRead32 (RegEax, 4, 7);
    if (FamilyId == 0x06 || FamilyId == 0x0f) {
      ExtendedModelId = BitFieldRead32 (RegEax, 16, 19);
      *DisplayedModel += (ExtendedModelId << 4);
    }
  }
}

/**
  Detect if the processor supports socket ID reassignment.

  @retval TRUE   the processor supports socket ID reassignment.
  @retval FALSE  the processor does not socket ID reassignment.
**/
BOOLEAN
IsSocketIdReassignmentSupported (
  VOID
  )
{
  UINT32 FamilyId;
  UINT32 ModelId;

  GetProcessorId (&FamilyId, &ModelId, NULL, NULL);

  if ((FeaturePcdGet (PcdCpuNehalemFamilyFlag) && IS_NEHALEM_SERVER_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuSandyBridgeFamilyFlag) && IS_SANDYBRIDGE_SERVER_PROC (FamilyId, ModelId)) ||
      (FeaturePcdGet (PcdCpuIvyBridgeFamilyFlag) && IS_IVYBRIDGE_SERVER_PROC (FamilyId, ModelId))) {
      return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Construct new APIC IDs based on current APIC IDs and new socket ID configuration.

**/
VOID
ConstructNewApicId (
  VOID
  )
{
  CPU_SOCKET_ID_INFO        *SocketIdInfo;
  UINTN                     Index1;
  UINTN                     Index2;
  EFI_CPU_PHYSICAL_LOCATION Location;
  UINT32                    ApicId;
  UINTN                     ThreadIdBits;
  UINTN                     CoreIdBits;
  UINTN                     PackageIdBitOffset;
  UINTN                     SocketIdCount;
  
  if (FeaturePcdGet (PcdCpuSocketIdReassignmentFlag) && IsSocketIdReassignmentSupported ()) {
    PcdSet32 (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_SOCKET_ID_REASSIGNMENT_BIT);
    if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_SOCKET_ID_REASSIGNMENT_BIT) != 0) {
      //
      // If any socket ID is to be re-assigned, then validate if there is any conflict.
      //
      SocketIdInfo = (CPU_SOCKET_ID_INFO *)PcdGetPtr (PcdCpuSocketId);
      ASSERT (SocketIdInfo != NULL);
      SocketIdCount = LibPcdGetSize (PcdToken (PcdCpuSocketId)) / sizeof (CPU_SOCKET_ID_INFO);

      if (SocketIdCount != 0) {
        mEnableSocketIdReassignment = TRUE;

        for (Index1 = 0; Index1 < SocketIdCount; Index1++) {
          for (Index2 = Index1 + 1; Index2 < SocketIdCount; Index2++) {
            if (SocketIdInfo[Index1].DefaultSocketId == SocketIdInfo[Index2].DefaultSocketId ||
              SocketIdInfo[Index1].NewSocketId == SocketIdInfo[Index2].NewSocketId) {
              break;
            }
          }
          if (Index2 < Index1) {
            DEBUG ((EFI_D_WARN, "Duplicate socket ID found in socket ID configuration!\n"));
            DEBUG ((EFI_D_WARN, "Socket ID reassignment not enabled!\n"));
            mEnableSocketIdReassignment = FALSE;
            break;
          }
        }
      }

      for (Index1 = 0; mEnableSocketIdReassignment && Index1 < mCpuConfigConextBuffer.NumberOfProcessors; Index1++) {
        ApicId = mExchangeInfo->BistBuffer[Index1].ApicId;

        //
        // Assume all processors have same APIC ID topology. So use BSP's CPUID value
        // to extract processor location for all processors.
        //
        ExtractProcessorLocation (ApicId, &Location, &ThreadIdBits, &CoreIdBits);
        PackageIdBitOffset = ThreadIdBits + CoreIdBits;

        for (Index2 = 0; Index2 < SocketIdCount; Index2++) {
          if (Location.Package == SocketIdInfo[Index2].DefaultSocketId) {
            break;
          }
        }
        if (Index2 < SocketIdCount && SocketIdInfo[Index2].DefaultSocketId != SocketIdInfo[Index2].NewSocketId) {
          mSocketIdAssigned[Index1] = TRUE;
          mNewSocketId[Index1] = SocketIdInfo[Index2].NewSocketId;
          mNewApicId[Index1] = BitFieldWrite32 (
                                 ApicId,
                                 PackageIdBitOffset,
                                 31,
                                 SocketIdInfo[Index2].NewSocketId
                                 );
        } else {
          mSocketIdAssigned[Index1] = FALSE;
          mNewApicId[Index1] = ApicId;
        }

        //
        // Make sure new APIC ID is unique
        //
        for (Index2 = 0; Index2 < Index1; Index2++) {
          if (mNewApicId[Index1] == mNewApicId[Index2]) {
            DEBUG ((EFI_D_WARN, "APIC ID is not unique after socket ID reassignment!\n"));
            DEBUG ((EFI_D_WARN, "Socket ID reassignment not enabled!\n"));
            mEnableSocketIdReassignment = FALSE;
            break;
          }
        }
      }

      if (mEnableSocketIdReassignment) {
        PcdSet32 (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_SOCKET_ID_REASSIGNMENT_BIT);
      }
    }
  }
}

/**
  Sort the APIC ID of all processors.

  This function sorts the APIC ID of all processors so that processor number is assigned in the
  ascending order of APIC ID which eases MP debugging. SMBIOS logic also depends on this assumption.

**/
VOID
SortApicId (
  VOID
  )
{
  UINTN   Index1;
  UINTN   Index2;
  UINTN   Index3;
  UINT32  ApicId;
  UINT32  Bist;
  UINT8   SocketId;
  BOOLEAN SocketIdAssigned;

  if (mExchangeInfo->ApCount != 0) {
    mCpuConfigConextBuffer.NumberOfProcessors += mExchangeInfo->ApCount;

    ConstructNewApicId ();

    for (Index1 = 0; Index1 < mExchangeInfo->ApCount; Index1++) {
      Index3 = Index1;
      if (mEnableSocketIdReassignment) {
        //
        // Sort key is the new APIC ID
        //
        ApicId = mNewApicId[Index1];
        for (Index2 = Index1 + 1; Index2 <= mExchangeInfo->ApCount; Index2++) {
          if (ApicId > mNewApicId[Index2]) {
            Index3 = Index2;
            ApicId = mNewApicId[Index2];
          }
        }
      } else {
        //
        // Sort key is the hardware default APIC ID
        //
        ApicId = mExchangeInfo->BistBuffer[Index1].ApicId;
        for (Index2 = Index1 + 1; Index2 <= mExchangeInfo->ApCount; Index2++) {
          if (ApicId > mExchangeInfo->BistBuffer[Index2].ApicId) {
            Index3 = Index2;
            ApicId = mExchangeInfo->BistBuffer[Index2].ApicId;
          }
        }
      }
      if (Index3 != Index1) {
        if (mEnableSocketIdReassignment) {
          mNewApicId[Index3] = mNewApicId[Index1];
          mNewApicId[Index1] = ApicId;

          SocketIdAssigned = mSocketIdAssigned[Index3];
          mSocketIdAssigned[Index3] = mSocketIdAssigned[Index1];
          mSocketIdAssigned[Index1] = SocketIdAssigned;

          SocketId = mNewSocketId[Index3];
          mNewSocketId[Index3] = mNewSocketId[Index1];
          mNewSocketId[Index1] = SocketId;

          ApicId = mExchangeInfo->BistBuffer[Index3].ApicId;
        }
        mExchangeInfo->BistBuffer[Index3].ApicId = mExchangeInfo->BistBuffer[Index1].ApicId;
        mExchangeInfo->BistBuffer[Index1].ApicId = ApicId;
        Bist = mExchangeInfo->BistBuffer[Index3].Bist;
        mExchangeInfo->BistBuffer[Index3].Bist = mExchangeInfo->BistBuffer[Index1].Bist;
        mExchangeInfo->BistBuffer[Index1].Bist = Bist;
      }
    }

    //
    // Get the processor number for the BSP
    //
    ApicId = GetInitialApicId ();
    for (Index1 = 0; Index1 < mCpuConfigConextBuffer.NumberOfProcessors; Index1++) {
      if (mExchangeInfo->BistBuffer[Index1].ApicId == ApicId) {
        mCpuConfigConextBuffer.BspNumber = Index1;
        break;
      }
    }
  }
}

/**
  Update collected APIC ID related info after socket IDs were reassigned.

**/
VOID
UpdateApicId (
  VOID
  )
{
  UINTN  Index;

  if (FeaturePcdGet (PcdCpuSocketIdReassignmentFlag)) {
    //
    // Update APIC ID in Exchange Info
    //
    if (mEnableSocketIdReassignment) {
      for (Index = 0; Index < mCpuConfigConextBuffer.NumberOfProcessors; Index++) {
        if (mSocketIdAssigned[Index]) {
          mExchangeInfo->BistBuffer[Index].ApicId = mNewApicId[Index];
        }
      }
      //
      // Re-collect the basic processor data including APIC ID related data.
      //
      DispatchAPAndWait (
        TRUE,
        0,
        CollectBasicProcessorData
        );

      CollectBasicProcessorData (mCpuConfigConextBuffer.BspNumber);
    }
  }
}

/**
  Check if there is legacy APIC ID conflict among all processors.

  @retval EFI_SUCCESS      No coflict or conflict was resoved by force x2APIC mode
  @retval EFI_UNSUPPORTED  There is legacy APIC ID conflict and can't be rsolved in xAPIC mode
**/
EFI_STATUS
CheckApicId (
  VOID
  )
{
  UINTN               Index1;
  UINTN               Index2;
  BOOLEAN             X2ApicCapable;

  X2ApicCapable = FALSE; 
  if (FeaturePcdGet (PcdCpuX2ApicFlag) && IsX2ApicSupported ()) {
    X2ApicCapable = TRUE;
    PcdSet32 (PcdCpuProcessorFeatureCapabilityEx1, PcdGet32 (PcdCpuProcessorFeatureCapabilityEx1) | PCD_CPU_X2APIC_BIT);
    if ((PcdGet32 (PcdCpuProcessorFeatureUserConfigurationEx1) & PCD_CPU_X2APIC_BIT) != 0) {
      mEnableX2Apic = TRUE;
    }
  }

  //
  // If xAPIC mode is configured, then check if there is any APIC ID conflict.
  //
  if (!mEnableX2Apic) {
    ASSERT (mCpuConfigConextBuffer.NumberOfProcessors <= FixedPcdGet32 (PcdCpuMaxLogicalProcessorNumber));

    if (mCpuConfigConextBuffer.NumberOfProcessors > 255) {
      if (X2ApicCapable) {
        mEnableX2Apic = TRUE;
        DEBUG ((EFI_D_WARN, "Number of processors > 255, force x2APIC mode\n"));
      } else {
        DEBUG ((EFI_D_ERROR, "Number of processors > 255 but x2APIC mode is not supported!\n"));
        return EFI_UNSUPPORTED;
      }
    } else if (mEnableSocketIdReassignment) {
      //
      // Check if there is 8-bit legacy APIC ID conflict with assigned new socket IDs.
      //
      for (Index1 = 0; Index1 < mCpuConfigConextBuffer.NumberOfProcessors; Index1++) {
        for (Index2 = Index1 + 1; Index2 < mCpuConfigConextBuffer.NumberOfProcessors; Index2++) {
          if ((UINT8) mNewApicId[Index1] == (UINT8) mNewApicId[Index2]) {
            break;
          }
        }
        if (Index2 < mCpuConfigConextBuffer.NumberOfProcessors) {
          if (X2ApicCapable) {
            mEnableX2Apic = TRUE;
            DEBUG ((EFI_D_WARN, "Legacy APIC ID conflict with socket ID reassignment, force x2APIC mode\n"));
          } else {
            DEBUG ((EFI_D_ERROR, "Legacy APIC ID conflict with socket ID reassignment but x2APIC mode is not supported!\n"));
            return EFI_UNSUPPORTED;
          }
          break;
        }
      }
    } else {
      //
      // Check if there is 8-bit legacy APIC ID conflict with hardware default socket IDs.
      //
      for (Index1 = 0; Index1 < mCpuConfigConextBuffer.NumberOfProcessors; Index1++) {
       for (Index2 = Index1 + 1; Index2 < mCpuConfigConextBuffer.NumberOfProcessors; Index2++) {
         if ((UINT8) mExchangeInfo->BistBuffer[Index1].ApicId == (UINT8) mExchangeInfo->BistBuffer[Index2].ApicId) {
            break;
          }
        }
        if (Index2 < mCpuConfigConextBuffer.NumberOfProcessors) {
          if (X2ApicCapable) {
            mEnableX2Apic = TRUE;
            DEBUG ((EFI_D_WARN, "Legacy APIC ID conflict with hardware socket ID, force x2APIC mode\n"));
          } else {
            DEBUG ((EFI_D_ERROR, "Legacy APIC ID conflict with hardware socket ID but x2APIC mode is not supported!\n"));
            return EFI_UNSUPPORTED;
          }
          break;
        }
      }
    }
  }

  if (mEnableX2Apic) {
    PcdSet32 (PcdCpuProcessorFeatureSettingEx1, PcdGet32 (PcdCpuProcessorFeatureSettingEx1) | PCD_CPU_X2APIC_BIT);
  }
  return EFI_SUCCESS;
}

/**
  Assign a package scope BSP responsible for package scope programming.

  This functions assigns the processor with the least APIC ID within a processor package as
  the package BSP.

**/
VOID
AssignPackageBsp (
  VOID
  )
{
  UINTN  Index;
  UINT32 PackageId;

  mCpuConfigConextBuffer.CollectedDataBuffer[0].PackageBsp = TRUE;
  PackageId = mCpuConfigConextBuffer.CollectedDataBuffer[0].ProcessorLocation.Package;
  for (Index = 1; Index < mCpuConfigConextBuffer.NumberOfProcessors; Index++) {
    if (PackageId != mCpuConfigConextBuffer.CollectedDataBuffer[Index].ProcessorLocation.Package) {
      PackageId = mCpuConfigConextBuffer.CollectedDataBuffer[Index].ProcessorLocation.Package;
      mCpuConfigConextBuffer.CollectedDataBuffer[Index].PackageBsp = TRUE;
    }
  }
}

/**
  Generate entry for socket ID reassignment in pre-SMM-init S3 register table.
  
  @param  ProcessorNumber   The handle number of specified processor.

**/
VOID
SocketIdReg (
  IN UINTN      ProcessorNumber
  )
{
  if (mEnableSocketIdReassignment) {
    if (mSocketIdAssigned[ProcessorNumber]) {
      //
      // Disable APIC
      //
      WritePreSmmInitRegisterTable (ProcessorNumber, Msr, EFI_MSR_IA32_APIC_BASE, N_MSR_APIC_GLOBAL_ENABLE, 1, 0);
      //
      // Writes Socket ID MSR with package ID value. This must be done on the NBSP 
      // or the same value must be written to all APs in the package.
      //
      WritePreSmmInitRegisterTable (ProcessorNumber, Msr, MSR_SOCKET_ID, 0, 8, mNewSocketId[ProcessorNumber]);
      //
      // Re-enables APIC to make the Socket ID update
      //
      WritePreSmmInitRegisterTable (ProcessorNumber, Msr, EFI_MSR_IA32_APIC_BASE, N_MSR_APIC_GLOBAL_ENABLE, 1, 1);
    }
  }
}

/**
  Re-program Local APIC for virtual wire mode after socket ID for the BSP has been changed.

**/
VOID
ReprogramVirtualWireMode (
  VOID
  )
{
  if (FeaturePcdGet (PcdCpuSocketIdReassignmentFlag)) {
    if (mEnableSocketIdReassignment && mSocketIdAssigned[mCpuConfigConextBuffer.BspNumber]) {
      ProgramVirtualWireMode ();
    }
  }
}
