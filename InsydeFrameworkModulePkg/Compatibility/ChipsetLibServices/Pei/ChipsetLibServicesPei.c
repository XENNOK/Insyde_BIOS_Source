//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;* 
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Module Name:

  ChipsetLibServicesPei.c

Abstract:

  ChipsetLibServices Ppi provider.

--*/

#include "Tiano.h"
#include "Pei.h"
#include "EfiCommonLib.h"
#include "BootMode.h"
#include "SetupConfig.h"

#include EFI_PPI_PRODUCER (ChipsetLibServices)
#include EFI_PPI_DEFINITION (LoadFile)
#ifdef INSYDE_DEBUG
  #include EFI_PPI_DEFINITION (DebugComm)
#endif

//
// Module Local functions
//
EFI_STATUS
EFIAPI
FvFileLoaderPpiNotifyCallback (
  IN      EFI_PEI_SERVICES                **PeiServices,
  IN      EFI_PEI_NOTIFY_DESCRIPTOR       *NotifyDescriptor,
  IN      VOID                            *Ppi
);

//
// Chipset Lib external functions
//
EFI_STATUS
LegacyRegionAccessCtrl (
  IN      UINTN                           Start,
  IN      UINTN                           Length,
  IN      UINTN                           Mode  
);

EFI_STATUS
OnStartOfTcgPei (
  IN      VOID                            **PeiServices,
  IN OUT  BOOLEAN                         *NoNeedToInitTpm
);


EFI_STATUS
InitFlashMode (
  IN      UINT8                           *FlashMode
);

EFI_STATUS
EnableFvbWrites (
  IN      BOOLEAN                         EnableWrites
);

VOID
ResetSystem (
  IN      EFI_RESET_TYPE                  ResetType,
  IN      EFI_STATUS                      ResetStatus,
  IN      UINTN                           DataSize,
  IN      CHAR16                          *ResetData OPTIONAL
);

VOID
ProgramGpio (
  VOID
);

EFI_STATUS
ReadNBVID (
  IN      EFI_PEI_SERVICES                **PeiServices,
  OUT     UINT16                          *VenderID
);

EFI_STATUS
GetVgaTypeInfo (
  IN      EFI_PEI_SERVICES                **PeiServices,
  OUT     CLOCK_GEN_VGA_TYPE              *VgaType
);


//
// Module globals
//
static CHIPSET_LIB_SERVICES_PPI mChipsetLibServicesPpi = {
  CHIPSET_LIB_SERVICES_PPI_VERSION,
  sizeof(CHIPSET_LIB_SERVICES_PPI),
  OnStartOfTcgPei,
  InitFlashMode,
  EnableFvbWrites,
  ResetSystem,
  ProgramGpio,
  ReadNBVID,
  GetVgaTypeInfo,
  LegacyRegionAccessCtrl
};

static EFI_PEI_PPI_DESCRIPTOR mInstallPpiDescriptor = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gChipsetLibServicesPpiGuid,
  &mChipsetLibServicesPpi
};


BOOLEAN                         ImageInMemory = FALSE;

EFI_PEIM_ENTRY_POINT (ChipsetLibServicesPeiEntry)

EFI_STATUS
EFIAPI
ChipsetLibServicesPeiEntry (
  IN EFI_FFS_FILE_HEADER    *FfsHeader,
  IN EFI_PEI_SERVICES       **PeiServices
  )
/*++

Routine Description:

  Install ChipsetCtrl Ppi

Arguments:
  FfsHeader     -   FFS File Header
  PeiServices   -   PeiServices to locate various PPIs

Returns:

  Status

  EFI_SUCCESS           - Operation successful complete.

--*/
{
  EFI_STATUS                    Status;

  if (ImageInMemory) {
      
    EFI_PEI_PPI_DESCRIPTOR        *PpiDescriptor;
    VOID                          *Ppi;
  
    //
    // Locate old PPI
    //
    Status = (**PeiServices).LocatePpi (
                                  PeiServices,
                                  mInstallPpiDescriptor.Guid,
                                  0,
                                  &PpiDescriptor,
                                  &Ppi
                                  );
    
    //
    // Reinstall PPI into memory
    //
    if (!EFI_ERROR (Status)) {

      Status = (**PeiServices).ReInstallPpi (
                                  PeiServices,
                                  PpiDescriptor,
                                  &mInstallPpiDescriptor
                                  );
    }

  } else {  

    EFI_PEI_NOTIFY_DESCRIPTOR    *NotifyDescriptor;
    EFI_FFS_FILE_HEADER          **FfsHeaderPtr;

    //
    // Install PPI
    //
    Status = (**PeiServices).InstallPpi (PeiServices, &mInstallPpiDescriptor);
    ASSERT_PEI_ERROR (PeiServices, Status);

    //
    // Allocate memory in CAR to store callback context (Notify Descriptor)
    // and my FfsHeader for convenience to reinstall Ppi into RAM.
    //
    Status = (**PeiServices).AllocatePool (
                               PeiServices,
                               sizeof (EFI_PEI_NOTIFY_DESCRIPTOR) + sizeof(EFI_FFS_FILE_HEADER *),
                               &NotifyDescriptor
                               );

    //
    // Register memory discover notify function.
    //  
    if (!EFI_ERROR (Status)) {

      FfsHeaderPtr = (EFI_FFS_FILE_HEADER **)(NotifyDescriptor + 1);
      *FfsHeaderPtr = FfsHeader;

      NotifyDescriptor->Flags   = EFI_PEI_PPI_DESCRIPTOR_NOTIFY_CALLBACK |
                                  EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST;
      NotifyDescriptor->Guid    = &gPeiFvFileLoaderPpiGuid;
      NotifyDescriptor->Notify  = FvFileLoaderPpiNotifyCallback;         

      //
      //  Now publish the notification with the PEI Core
      //
      Status = (**PeiServices).NotifyPpi (PeiServices, NotifyDescriptor);
      ASSERT_PEI_ERROR (PeiServices, Status);
    }
    

  }


  return Status;
}

EFI_STATUS
EFIAPI
FvFileLoaderPpiNotifyCallback (
  IN EFI_PEI_SERVICES           **PeiServices,
  IN EFI_PEI_NOTIFY_DESCRIPTOR  *NotifyDescriptor,
  IN VOID                       *Ppi
  )
/*++

Routine Description:

  FvLoader PPI discover callback function. Use to reload PPIs that provide by 
  me into memory.

Arguments:
  PeiServices           - PeiServices to locate various PPIs
  NotifyDescriptor      - Notify descriptor
  Ppi                   - Installed Ppi

Returns:

  Status

  EFI_SUCCESS           - Operation successful complete.

--*/
{
  EFI_FFS_FILE_HEADER               *FfsHeader;
  EFI_STATUS                        Status;
  EFI_PEI_FV_FILE_LOADER_PPI        *FvFileLoader;
  EFI_PHYSICAL_ADDRESS              ImageAddress;
  EFI_PHYSICAL_ADDRESS              EntryPoint;
  UINT64                            ImageSize;
  UINTN                             ImageOffset;
  

  Status = EFI_SUCCESS;
  //
  // Load myself into memory
  //
  if (!ImageInMemory) {
  
  
    FfsHeader = *(EFI_FFS_FILE_HEADER **)(NotifyDescriptor + 1); 
    FvFileLoader = (EFI_PEI_FV_FILE_LOADER_PPI *)Ppi;                               

    Status = FvFileLoader->FvLoadFile (
                             FvFileLoader,
                             FfsHeader,
                             &ImageAddress,
                             &ImageSize,
                             &EntryPoint
                             );
      
    if (!EFI_ERROR (Status)) {
      ImageOffset = (UINTN) EntryPoint - (UINTN) ChipsetLibServicesPeiEntry;
      *(BOOLEAN *) ((UINTN) &ImageInMemory + ImageOffset) = TRUE;
      #ifdef INSYDE_DEBUG
      INSYDE_DEBUG_SEND_INFO(PeiServices, ImageAddress, 0, NULL, 0)
      #endif
      Status = ((EFI_PEIM_ENTRY_POINT) (UINTN) EntryPoint) (FfsHeader, PeiServices);
    }
  }
  
  return Status;
}

