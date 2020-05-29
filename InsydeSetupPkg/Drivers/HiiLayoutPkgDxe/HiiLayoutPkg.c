/** @file

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "HiiLayoutPkg.h"

extern UINT8         HiiLayoutPkgDxeImages[];
extern UINT8         ProjectBin[];
EFI_GUID             mLayoutPackageListGuid = {0xa7958683, 0x3242, 0x4963, {0xb6, 0x42, 0xa7, 0xc9, 0x77, 0x34, 0x3f, 0xd4}};

/**
  HII Database Protocol notification event handler.

  Register layout package when HII Database Protocol has been installed.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.

**/
EFI_STATUS
GetLayoutPackageFromFv (
  IN OUT UINT8                         **LayoutPkg
  )
{
  EFI_STATUS                           Status;
  UINTN                                FvProtocolCount;
  EFI_HANDLE                           *FvHandles;
  EFI_FIRMWARE_VOLUME2_PROTOCOL        *Fv;
  UINTN                                Index;
  UINT32                               AuthenticationStatus;
  UINTN                                ImageSize;
  UINT8                                *ImageData;
  UINTN                                PackageLength;
  UINT8                                *Package;


  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiFirmwareVolume2ProtocolGuid,
                  NULL,
                  &FvProtocolCount,
                  &FvHandles
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }

  ImageData = NULL;
  ImageSize = 0;

  for (Index = 0; Index < FvProtocolCount; Index++) {
    Status = gBS->HandleProtocol (
                    FvHandles[Index],
                    &gEfiFirmwareVolume2ProtocolGuid,
                    (VOID **) &Fv
                    );
    if (EFI_ERROR (Status)) {
      continue;
    }

    Status = EFI_NOT_FOUND;
    //
    // gH2OHiiLayoutFileGuid = H2O_HII_LAYOUT_FILE_GUID
    //
    Status = Fv->ReadSection (
                  Fv,
                  &gH2OHiiLayoutFileGuid,
                  EFI_SECTION_RAW,
                  0,
                  (VOID **)&ImageData,
                  &ImageSize,
                  &AuthenticationStatus
                  );
    if (!EFI_ERROR (Status)) {
      break;
    }
  }
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (ImageSize <= sizeof (H2O_LAYOUT_PACKAGE_HDR)) {
    return EFI_NOT_FOUND;
  }

  PackageLength = 4 + ImageSize;
  Package = (UINT8 *)AllocateZeroPool (PackageLength);
  if (Package == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  CopyMem (Package, &PackageLength, 4);
  CopyMem (Package + 4, ImageData, ImageSize);

  FreePool (ImageData);

  *LayoutPkg = Package;

  return EFI_SUCCESS;
}

/**
  HII Database Protocol notification event handler.

  Register layout package when HII Database Protocol has been installed.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.

**/
VOID
RegisterLayoutPackage (
  IN     EFI_EVENT                     Event,
  IN     VOID                          *Context
  )
{
  EFI_STATUS                           Status;
  UINT8                                *LayoutPkg;

  EFI_HII_HANDLE                       HiiHandle;
  EFI_HII_DATABASE_PROTOCOL            *HiiDatabase;
  LAYOUT_DATABASE_PROTOCOL             *LayoutDatabase;

  //
  // Layout pkg from FV.
  //
  LayoutPkg = NULL;
  Status = GetLayoutPackageFromFv (&LayoutPkg);
  if (EFI_ERROR (Status)) {
    LayoutPkg = &ProjectBin[0];
  }

  //
  // Locate HII Database Protocol
  //
  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &HiiDatabase);
  ASSERT (Status == EFI_SUCCESS);

  //
  // Add this layout package to a package list then install it.
  // BUGBUG: HiiDatabase not support layout pkg yet
  //
  HiiHandle = NULL;
  //HiiHandle = HiiAddPackages (&mLayoutPackageListGuid, NULL, LayoutPkg, NULL);
  //ASSERT (HiiHandle != NULL);

  //
  // Restore HIIHandle
  //
  Status = gBS->LocateProtocol (&gLayoutDatabaseProtocolGuid, NULL, (VOID **) &LayoutDatabase);
  ASSERT (Status == EFI_SUCCESS);

  LayoutDatabase->LayoutPkgHiiHandle = HiiHandle;
  LayoutDatabase->LayoutPkgAddr = (UINTN)(UINTN *)LayoutPkg;
  LayoutDatabase->LayoutListHead = NULL;

}

/**
  HII Database Protocol notification event handler.

  Register layout package when HII Database Protocol has been installed.

  @param[in] Event    Event whose notification function is being invoked.
  @param[in] Context  Pointer to the notification function's context.

**/
VOID
RegisterImagePackage (
  IN     EFI_EVENT                     Event,
  IN     VOID                          *Context
  )
{
  EFI_STATUS                           Status;
  UINT8                                *ImagePkg;

  EFI_HII_HANDLE                       HiiHandle;
  EFI_HII_DATABASE_PROTOCOL            *HiiDatabase;
  LAYOUT_DATABASE_PROTOCOL             *LayoutDatabase;

  //
  // Locate HII Database Protocol
  //
  Status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &HiiDatabase);
  ASSERT (Status == EFI_SUCCESS);

  //
  // Add this imgae package to a package list then install it.
  //
  HiiHandle = NULL;
  ImagePkg = (UINT8 *)&HiiLayoutPkgDxeImages[0];
  HiiHandle = HiiAddPackages (&mLayoutPackageListGuid, NULL, ImagePkg, NULL);
  ASSERT (HiiHandle != NULL);

  //
  // Restore HIIHandle
  //
  Status = gBS->LocateProtocol (&gLayoutDatabaseProtocolGuid, NULL, (VOID **) &LayoutDatabase);
  ASSERT (Status == EFI_SUCCESS);

  LayoutDatabase->ImagePkgHiiHandle = HiiHandle;

}

/**
  Install HII Layout Package.

  @retval  EFI_SUCCESS       Install HII Layout Package successfully.
  @return  other             Failed to install HII Layout Package.

**/
EFI_STATUS
InstallHiiLayoutPkg (
  VOID
  )
{
  EFI_EVENT                   HiiRegistration;

  //
  // Register notify function on HII Database Protocol to add layout package.
  //
  EfiCreateProtocolNotifyEvent (
    &gEfiHiiDatabaseProtocolGuid,
    TPL_CALLBACK,
    RegisterLayoutPackage,
    NULL,
    &HiiRegistration
    );

  return EFI_SUCCESS;
}

/**
  Install HII Image Package.

  @retval  EFI_SUCCESS       Install HII Image Package successfully.
  @return  other             Failed to install HII Image Package.

**/
EFI_STATUS
InstallHiiImagePkg (
  VOID
  )
{
  EFI_EVENT                   HiiRegistration;

  //
  // Register notify function on HII Database Protocol to add image package.
  //
  EfiCreateProtocolNotifyEvent (
    &gEfiHiiDatabaseProtocolGuid,
    TPL_CALLBACK,
    RegisterImagePackage,
    NULL,
    &HiiRegistration
    );

  return EFI_SUCCESS;
}

/**
  Install HII Image Package.

  @retval  EFI_SUCCESS       Install HII Image Package successfully.
  @return  other             Failed to install HII Image Package.

**/
EFI_STATUS
InstalLayoutDatabaseProtocol (
  VOID
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           DriverHandle;
  LAYOUT_DATABASE_PROTOCOL             *LayoutDatabase;

  //
  // Get DriverHandle and install LAYOUT_DATABASE_PROTOCOL on it
  //
  LayoutDatabase = AllocateZeroPool (sizeof (LAYOUT_DATABASE_PROTOCOL));
  if (LayoutDatabase == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  LayoutDatabase->Size = sizeof (LAYOUT_DATABASE_PROTOCOL);

  //
  // Init and Install
  //
  DriverHandle = NULL;
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &DriverHandle,
                  &gLayoutDatabaseProtocolGuid,
                  LayoutDatabase,
                  NULL
                  );
  LayoutDatabase->DriverHandle = DriverHandle;
  LayoutDatabase->LayoutPkgHiiHandle = 0;
  LayoutDatabase->ImagePkgHiiHandle = 0;
  LayoutDatabase->LayoutPkgAddr = 0;

  ASSERT (Status == EFI_SUCCESS);

  return EFI_SUCCESS;
}

/**
  The user Entry Point for module HiiLayoutPkgDxe. The user code starts with this function.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval  EFI_SUCCESS       The entry point is executed successfully.
  @return  other             Some error occurs when executing this entry point.

**/
EFI_STATUS
EFIAPI
HiiLayoutPkgDxeEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS              Status;

  //
  // Install HII Layout Pkg.
  //
  Status = InstalLayoutDatabaseProtocol ();
  Status = InstallHiiLayoutPkg ();
  Status = InstallHiiImagePkg ();
  ASSERT_EFI_ERROR (Status);

  return Status;
}

