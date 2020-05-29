/** @file

  This file contains the SMI handler for the XTU specific SMI Commands
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
;* Copyright (c) 2008 Intel Corporation. All rights reserved
;* This software and associated documentation (if any) is furnished
;* under a license and may only be used or copied in accordance
;* with the terms of the license. Except as permitted by such
;* license, no part of this software or documentation may be
;* reproduced, stored in a retrieval system, or transmitted in any
;* form or by any means without the express written consent of
;* Intel Corporation.
;******************************************************************************

*/

#include "PerfTuneDxe.h"


EFI_GUID  mPerfTuneSwChildGuid = {0x53a3edac, 0xc718, 0x4522, 0x8b, 0x3a, 0x3b, 0xd3, 0x4a, 0x70, 0xca, 0x56};

/**
  
  Initializes the SMM Handler Driver
  
  @param  ImageHandle              
  @param  SystemTable             

  @retval EFI_SUCCESS             The function completed successfully.
  @retval !EFI_SUCCESS            The PEIM failed to execute.

**/
EFI_STATUS
InitializePerfTuneServices (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
   UINT8                           *Buffer;
   UINTN                           BufferSize;
   UINT64                          PowerDivisor;
   UINT64                          MsrValue;
   EFI_HANDLE                      Handle;
   EFI_STATUS                      Status;
   EFI_SMM_BASE_PROTOCOL           *SmmBase;
   DXE_XTU_POLICY_PROTOCOL         *XtuPlatformPolicyInstance;
   EFI_SETUP_UTILITY_PROTOCOL      *SetupUtility;
   
   CHIPSET_CONFIGURATION            *SystemConfiguration;
   
//[-start-120217-IB03780424-remove]//
//   MEM_INFO_PROTOCOL               *MemInfoHob;
//   EFI_GUID                        MemInfoProtocolGuid = MEM_INFO_PROTOCOL_GUID;
//[-end-120217-IB03780424-remove]//

  //
  // Clear XTU Posting flag, Set B_XTU_FLAG_SAFE_RECOVERY to 1, or XtuPei will detect as CMOS clear
  //
  WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus, (ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, XtuWdtStatus) & ~B_XTU_FLAG_POSTING) | B_XTU_FLAG_SAFE_RECOVERY);
  //
  // Clear XTU crash counter
  //
  WriteExtCmos8 (R_XCMOS_INDEX, R_XCMOS_DATA, XtuCrashCounter, 0);


  //
  // supported only when Enable OverClocking
  //
  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  SystemConfiguration = (CHIPSET_CONFIGURATION *)SetupUtility->SetupNvData;
  if (EFI_ERROR(Status) || !SystemConfiguration->OverClocking) {
    return EFI_UNSUPPORTED;
  }


  Status = gBS->LocateProtocol (&gEfiSmmBaseProtocolGuid, NULL, (VOID **)&SmmBase);
 
  if (EFI_ERROR (Status)) {
  	  DEBUG(( EFI_D_ERROR, "Locate  SmmBaseProtocol error: %r\n", Status));
     return Status;
  }

  Status = gBS->LocateProtocol (&gDxePlatformXtuPolicyGuid, NULL, (VOID **)&XtuPlatformPolicyInstance);

  if (EFI_ERROR (Status)) {
    return Status;
  }

  if (! XtuPlatformPolicyInstance->EnableSwSmiRealtimeCommunicationsInterface) {
    return EFI_ABORTED;
  }

//[-start-120224-IB03780425-remove]//
//  mCurrentProfile = SystemConfiguration->XmpProfileSetting;
//[-end-120224-IB03780425-remove]//

//[-start-120217-IB03780424-remove]//
//  Status = gBS->LocateProtocol (&MemInfoProtocolGuid, NULL, &MemInfoHob);
//  if (EFI_ERROR (Status)) {
//    return Status;
//  }
//
//  switch (MemInfoHob->MemInfoData.ddrFreq) {
//    case 1067:
//      mMemoryFrequency = 4;
//      break;
//
//    case 1333:
//      mMemoryFrequency = 5;
//      break;
//
//    case 1600:
//      mMemoryFrequency = 6;
//      break;
//
//    case 1867:
//      mMemoryFrequency = 7;
//      break;
//
//    case 2133:
//      mMemoryFrequency = 8;
//      break;
//
//    default:
//      mMemoryFrequency = 5;
//  }
//[-end-120217-IB03780424-remove]//

  MsrValue = AsmReadMsr64 (EFI_MSR_PKG_POWER_SKU_UNIT);
  PowerDivisor = LShiftU64(1, MsrValue & B_EFI_MSR_PKG_POWER_UNIT); 

  //
  // Load this driver's image to memory
  //
  Status = GetFvImage (&mPerfTuneSwChildGuid, &Buffer, &BufferSize);
  DEBUG(( EFI_D_ERROR, "GetFvImage Status: %r\n", Status));
  if (!EFI_ERROR (Status)) {
    //
    // Load the image in memory to SMRAM; it will automatically generate the
    // SMI.
    //
    SmmBase->Register (SmmBase, NULL, Buffer, BufferSize, &Handle, FALSE);
    gBS->FreePool(Buffer);
  }

   return EFI_SUCCESS;
}

/**
  
  This routine will take a GUIDed file name and provide a buffer of the data.
  
  NOTE:
  Need to remove this if DXE Core creating correct Loaded Image Handle!
  Dependencies: 
  
  @param  NameGuid             Pointer to a GUID of the file name.
  @param  Buffer                    Pointer to a buffer for the file contents.
  @param  Size                      Size of the buffer, in bytes

  @retval EFI_SUCCESS

**/
EFI_STATUS
GetFvImage (
  IN EFI_GUID   *NameGuid,
  IN OUT VOID   **Buffer,
  IN OUT UINTN  *Size
  )
{
   EFI_STATUS                    Status;
   EFI_HANDLE                    *HandleBuffer;
   UINTN                         HandleCount;
   UINTN                         Index;
   EFI_FIRMWARE_VOLUME_PROTOCOL  *Fv;
   EFI_FV_FILETYPE               FileType;
   EFI_FV_FILE_ATTRIBUTES        Attributes;
   EFI_SECTION_TYPE              SectionType;
   UINT32                        AuthenticationStatus;

   Fv = NULL;
   AuthenticationStatus = 0;

   Status = gBS->LocateHandleBuffer (
                                    ByProtocol,
                                    &gEfiFirmwareVolumeProtocolGuid,
                                    NULL,
                                    &HandleCount,
                                    &HandleBuffer
                                    );

   if (EFI_ERROR (Status) || HandleCount == 0) {
      return EFI_NOT_FOUND;
   }

   //
   // Find desired image in all Fvs
   //
   for (Index = 0; Index < HandleCount; Index++) {
      Status = gBS->HandleProtocol(
                                  HandleBuffer[Index],
                                  &gEfiFirmwareVolumeProtocolGuid,
                                  (VOID **)&Fv
                                  );

      if (EFI_ERROR (Status)) {
         return EFI_LOAD_ERROR;
      }

      *Buffer = NULL;
      *Size = 0;
      SectionType  = EFI_SECTION_PE32;
      Status = Fv->ReadSection (
                               Fv,
                               NameGuid,
                               SectionType,
                               0,
                               Buffer,
                               Size,
                               &AuthenticationStatus
                               );

      if (!EFI_ERROR (Status)) {
         break;
      }


      //
      // Try a raw file, since a PE32 SECTION does not exist
      //
      *Buffer = NULL;
      *Size = 0;
      Status = Fv->ReadFile (
                            Fv,
                            NameGuid,
                            Buffer,
                            Size,
                            &FileType,
                            &Attributes,
                            &AuthenticationStatus
                            );

      if (!EFI_ERROR (Status)) {
         break;
      }

   }
   //
   // not found image
   //
   if (Index >= HandleCount) {
      return EFI_NOT_FOUND;
   }

   return EFI_SUCCESS;
}
