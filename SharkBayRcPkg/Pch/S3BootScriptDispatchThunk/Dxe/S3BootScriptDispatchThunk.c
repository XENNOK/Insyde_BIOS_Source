/** @file
  Implementation for S3 Boot Script dispatch thunk driver.
  This thunk driver consumes PI S3SaveState protocol to produce framework S3BootScriptSave Protocol 
  
  Copyright (c) 2010 - 2011, Intel Corporation. All rights reserved.<BR>
  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/
#include <S3BootScriptDispatchThunk.h>


EFI_GUID mS3BootScriptThunkGuid = {
  0x1f84f9d8, 0xb672, 0x433b, {0x95, 0x8e, 0x59, 0xa7, 0x68, 0xd7, 0x9c, 0xd4}
};

EFI_HANDLE                    mHandle;
EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL mS3ScriptDispatch = {
                                 S3BootScriptDispatch
                                 };

EFI_S3_SAVE_STATE_PROTOCOL    *mS3SaveState;

/**
  Wrapper for a thunk  to transition from long mode to compatibility mode to execute 32-bit code and then transit back to
  long mode.
  
  @param  Function     The 32bit code entry to be executed.
  @param  Param1       The first parameter to pass to 32bit code
  @param  Param2       The second parameter to pass to 32bit code
  @retval EFI_SUCCESS  Execute 32bit code successfully.
  @retval other        Something wrong when execute the 32bit code 
              
**/  
EFI_STATUS
Execute32BitCode (
  IN UINT64      Function,
  IN UINT64      Param1,
  IN UINT64      Param2
  );

/**
  A stub to convert framework boot script dispatch to PI boot script dispatch.
  
  @param  ImageHandle  It should be is NULL.
  @param  Context      The first parameter to pass to 32bit code

  @return dispatch value.
              
**/  
EFI_STATUS
EFIAPI
FrameworkBootScriptDispatchStub (
  IN EFI_HANDLE ImageHandle,
  IN VOID       *Context
  )
{
  EFI_STATUS                Status;
  DISPATCH_ENTRYPOINT_FUNC  EntryFunc;
  VOID                      *PeiServices;
  IA32_DESCRIPTOR           Idtr;

//[-start-130315-IB06690222-remove]//
//
// This will cause system reboot when s3 resume ...
//
//  DEBUG ((EFI_D_ERROR, "FrameworkBootScriptDispatchStub - 0x%08x\n", (UINTN)Context));
//[-end-130315-IB06690222-remove]//

  EntryFunc = (DISPATCH_ENTRYPOINT_FUNC) (UINTN) (Context);
  AsmReadIdtr (&Idtr);
  PeiServices = (VOID *)(UINTN)(*(UINT32 *)(Idtr.Base - sizeof (UINT32)));

  //
  // ECP assumes first parameter is NULL, and second parameter is PeiServices.
  //
  Status = Execute32BitCode ((UINT64)(UINTN)EntryFunc, 0, (UINT64)(UINTN)PeiServices);

  return Status;
}

/**
  Internal function to add Save jmp address according to DISPATCH_OPCODE. 
  We ignore "Context" parameter.
  We need create thunk stub to convert PEI entrypoint (used in Framework version)
  to DXE entrypoint (defined in PI spec).

  @param  Marker                The variable argument list to get the opcode
                                and associated attributes.

  @retval EFI_OUT_OF_RESOURCES  Not enough resource to do operation.
  @retval EFI_SUCCESS           Opcode is added.

**/
EFI_STATUS
S3BootScriptDispatch (
  IN EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL    *This,
  IN VOID                                    *EntryPoint
  )
{
  VOID           *Context;
  
  //
  // Register callback
  //
  Context    = EntryPoint;
  EntryPoint = (VOID *)(UINTN)FrameworkBootScriptDispatchStub;
  return mS3SaveState->Write (
                         mS3SaveState,
                         EFI_BOOT_SCRIPT_DISPATCH_2_OPCODE,
                         EntryPoint,
                         Context
                         );
}

/**
  Internal function to add Save jmp address according to DISPATCH_OPCODE2. 
  The "Context" parameter is not ignored.

  @param  Marker                The variable argument list to get the opcode
                                and associated attributes.

  @retval EFI_OUT_OF_RESOURCES  Not enough resource to do operation.
  @retval EFI_SUCCESS           Opcode is added.

**/
EFI_STATUS
S3BootScriptDispatch2 (
  IN EFI_S3_BOOT_SCRIPT_DISPATCH_PROTOCOL    *This,
  IN VOID                                    *EntryPoint,
  IN VOID                                    *Context
  )
{
 return mS3SaveState->Write (
                          mS3SaveState,
                          EFI_BOOT_SCRIPT_DISPATCH_2_OPCODE,
                          EntryPoint, 
                          Context
                         );
}

/**
  This routine is entry point of ScriptSave driver.

  @param  ImageHandle           Handle for this drivers loaded image protocol.
  @param  SystemTable           EFI system table.

  @retval EFI_OUT_OF_RESOURCES  No enough resource
  @retval EFI_SUCCESS           Succesfully installed the ScriptSave driver.
  @retval other                 Errors occured.

**/
EFI_STATUS
EFIAPI
InitializeS3BootScriptDispatch (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                     Status;  
  UINT8                          *Buffer;
  UINTN                          BufferSize;
  PE_COFF_LOADER_IMAGE_CONTEXT   ImageContext;
//[-start-130401-IB06690222-remove]//
//  BOOT_SCRIPT_THUNK_DATA         *BootScriptThunkData;
//[-end-130401-IB06690222-remove]//
  VOID                           *DevicePath;
  EFI_PHYSICAL_ADDRESS           MemoryAddress;
  UINTN                          PageNumber;
  EFI_HANDLE                     NewImageHandle;

  //
  // Test if the gEfiCallerIdGuid of this image is already installed. if not, the entry
  // point is loaded by DXE code which is the first time loaded. or else, it is already
  // be reloaded be itself.This is a work-around
  //
  Status = gBS->LocateProtocol (&gEfiCallerIdGuid, NULL, (VOID **)&DevicePath);
  if (EFI_ERROR (Status)) {
    //
    // This is the first-time loaded by DXE core. reload itself to NVS mem
    //
    //
    // A workarouond: Here we install a dummy handle
    //
    NewImageHandle = NULL;
    Status = gBS->InstallProtocolInterface (
                    &NewImageHandle,
                    &gEfiCallerIdGuid,
                    EFI_NATIVE_INTERFACE,
                    NULL
                    );

    Status = GetSectionFromAnyFv  (
               &gEfiCallerIdGuid,
               EFI_SECTION_PE32,
               0,
               (VOID **) &Buffer,
               &BufferSize
               );
    ImageContext.Handle    = Buffer;
    ImageContext.ImageRead = PeCoffLoaderImageReadFromMemory;
    //
    // Get information about the image being loaded
    //
    Status = PeCoffLoaderGetImageInfo (&ImageContext);
    if (EFI_ERROR (Status)) {
//[-start-130401-IB06690222-add]//
      gBS->FreePool (Buffer);  // Free the buffer allocated by ReadSection
//[-end-130401-IB06690222-add]//
      return Status;
    }

    MemoryAddress = SIZE_4GB - 1;
    PageNumber    = EFI_SIZE_TO_PAGES (BufferSize + ImageContext.SectionAlignment);
    Status  = gBS->AllocatePages (
                     AllocateMaxAddress,
                     EfiACPIMemoryNVS,
                     PageNumber,
                     &MemoryAddress
                     );
    if (EFI_ERROR (Status)) {
//[-start-130401-IB06690222-add]//
      gBS->FreePool (Buffer);  // Free the buffer allocated by ReadSection
//[-end-130401-IB06690222-add]//
      return EFI_OUT_OF_RESOURCES;
    }
    ImageContext.ImageAddress = (PHYSICAL_ADDRESS)(UINTN)MemoryAddress;
    //
    // Align buffer on section boundry
    //
    ImageContext.ImageAddress += ImageContext.SectionAlignment - 1;
    ImageContext.ImageAddress &= ~(ImageContext.SectionAlignment - 1);
    //
    // Load the image to our new buffer
    //
    Status = PeCoffLoaderLoadImage (&ImageContext);
    if (EFI_ERROR (Status)) {
//[-start-130401-IB06690222-add]//
      gBS->FreePool (Buffer);  // Free the buffer allocated by ReadSection
//[-end-130401-IB06690222-add]//
      gBS->FreePages (MemoryAddress, PageNumber);
      return Status;
    }

    //
    // Relocate the image in our new buffer
    //
    Status = PeCoffLoaderRelocateImage (&ImageContext);

    if (EFI_ERROR (Status)) {
//[-start-130401-IB06690222-add]//
      gBS->FreePool (Buffer); // Free the buffer allocated by ReadSection
//[-end-130401-IB06690222-add]//
      PeCoffLoaderUnloadImage (&ImageContext);
      gBS->FreePages (MemoryAddress, PageNumber);
      return Status;
    }

//[-start-130401-IB06690222-add]//
    //
    // Free the buffer allocated by ReadSection since the image has been relocated in the new buffer
    //
    gBS->FreePool (Buffer);
//[-end-130401-IB06690222-add]//

    //
    // Flush the instruction cache so the image data is written before we execute it
    //
    InvalidateInstructionCacheRange ((VOID *)(UINTN)ImageContext.ImageAddress, (UINTN)ImageContext.ImageSize);
    Status = ((EFI_IMAGE_ENTRY_POINT)(UINTN)(ImageContext.EntryPoint)) (NewImageHandle, SystemTable);
    if (EFI_ERROR (Status)) {
//[-start-130401-IB06690222-add]//
      PeCoffLoaderUnloadImage (&ImageContext);
//[-end-130401-IB06690222-add]//
      gBS->FreePages (MemoryAddress, PageNumber);
      return Status;
    }

//[-start-130401-IB06690222-remove]//
//
// Useless ...
//
//    //
//    // Additional step for BootScriptThunk integrity
//    //
//
//    //
//    // Allocate BootScriptThunkData
//    //
//    BootScriptThunkData = AllocatePool (sizeof (BOOT_SCRIPT_THUNK_DATA));
//    if (BootScriptThunkData == NULL) {
//      return EFI_OUT_OF_RESOURCES;
//    }
//
//    BootScriptThunkData->BootScriptThunkBase   = ImageContext.ImageAddress;
//    BootScriptThunkData->BootScriptThunkLength = ImageContext.ImageSize;
//    //
//    // Set BootScriptThunkData
//    //
//    //PcdSet64 (BootScriptThunkDataPtr, (UINT64)(UINTN)BootScriptThunkData); 
//
//    Status = SaveLockBox (
//               &mS3BootScriptThunkGuid,
//               (VOID *)(UINTN)BootScriptThunkData->BootScriptThunkBase,
//               (UINTN)BootScriptThunkData->BootScriptThunkLength
//               );
//    ASSERT_EFI_ERROR (Status);
//
//    Status = SetLockBoxAttributes (&mS3BootScriptThunkGuid, LOCK_BOX_ATTRIBUTE_RESTORE_IN_PLACE);
//    ASSERT_EFI_ERROR (Status);
//[-end-130401-IB06690222-remove]//

    return EFI_SUCCESS;
  } else {
    //
    // the entry point is invoked after reloading. following code only run in  ACPI NVS
    //

    //
    // Locate and cache PI S3 Save State Protocol.
    //
    Status = gBS->LocateProtocol (
                    &gEfiS3SaveStateProtocolGuid, 
                    NULL, 
                    (VOID **) &mS3SaveState
                    );
    ASSERT_EFI_ERROR (Status);

    return gBS->InstallProtocolInterface (
                  &mHandle,
                  &gEfiS3BootScriptDispatchProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mS3ScriptDispatch
                  );
  }
}
