/** @file
  DebugDriver driver will print out the driver connection information.

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "DebugDriver.h"

VOID                            *gRegistration;
EFI_DRIVER_BINDING_INFO         gDriverBindingInfo[MAX_DRIVER_BINDING_INFO];
UINTN                           gDrivingBindingTotalCount = 0;
UINTN                           gDrivingBindingIndex = 0;



/**
  Locate the driver binding handle which a specified driver binding protocol installed on.
  
  @param        DriverBindingNeed
  @param        Handle

  @retval       EFI_NOT_FOUND  Could not find the handle.
  @retval       EFI_SUCCESS    Successfully find the associated driver binding handle.

**/
STATIC
EFI_STATUS
GetHandleFromDriverBinding (
  IN   EFI_DRIVER_BINDING_PROTOCOL           *DriverBindingNeed,
  OUT  EFI_HANDLE                            *Handle 
 )
 {
  EFI_STATUS                          Status ;
  EFI_DRIVER_BINDING_PROTOCOL         *DriverBinding;
  UINTN                               DriverBindingHandleCount;
  EFI_HANDLE                          *DriverBindingHandleBuffer;
  UINTN                               Index;
  
  DriverBindingHandleCount = 0;
  DriverBindingHandleBuffer = NULL;
  *Handle = NULL;
  Status = gBS->LocateHandleBuffer (
              ByProtocol,   
              &gEfiDriverBindingProtocolGuid,  
              NULL,
              &DriverBindingHandleCount, 
              &DriverBindingHandleBuffer
              );
  if (EFI_ERROR (Status) || DriverBindingHandleCount == 0) {
    return EFI_NOT_FOUND;
  }
  
  for (Index = 0 ; Index < DriverBindingHandleCount; Index++ ) {
    Status = gBS->OpenProtocol(
                      DriverBindingHandleBuffer[Index],
                      &gEfiDriverBindingProtocolGuid,
                      &DriverBinding,
                      DriverBindingNeed->ImageHandle,
                      NULL,
                      EFI_OPEN_PROTOCOL_GET_PROTOCOL
                      );
                      
   if (!EFI_ERROR (Status) && DriverBinding != NULL) {
    
    if ( DriverBinding == DriverBindingNeed ) {
      *Handle = DriverBindingHandleBuffer[Index];
      gBS->FreePool (DriverBindingHandleBuffer);         
      return EFI_SUCCESS ;
    }
   }
 }
 
 gBS->FreePool (DriverBindingHandleBuffer);
 return EFI_NOT_FOUND ;
}

EFI_STATUS
EFIAPI
DebugDriverBindingSupported (
  IN EFI_DRIVER_BINDING_PROTOCOL    *This,
  IN EFI_HANDLE                     Controller,
  IN EFI_DEVICE_PATH_PROTOCOL       *RemainingDevicePath
  )
{
  UINTN                       Index;
  EFI_STATUS                  Status;
  
  for (Index = 0; Index < gDrivingBindingIndex; Index++) {
    if (This == gDriverBindingInfo[Index].DriveBinding) break;
  }
  if (Index == gDrivingBindingIndex) {
    DEBUG ((EFI_D_ERROR, "!!!Error can not find the DriverBindingInfo(Supported)\n"));
    return EFI_UNSUPPORTED;
  }
  
  // Call original DriverBinding Start function
  Status =  gDriverBindingInfo[Index].Supported (This, Controller, RemainingDevicePath);
  
  return Status;
}

EFI_STATUS
EFIAPI
DebugDriverBindingStart (
  IN EFI_DRIVER_BINDING_PROTOCOL  *This,
  IN EFI_HANDLE                   Controller,
  IN EFI_DEVICE_PATH_PROTOCOL     *RemainingDevicePath
  )
{
  UINTN                             Index;
  EFI_STATUS                        Status;
  EFI_HANDLE                        DbgDriverBindingHandle;
  EFI_STATUS                        DbgStatus;
  CHAR16                            *DbgDriverName;
  CHAR8                             *DbgLangName1;
  CHAR8                             *DbgLangName2;
  EFI_COMPONENT_NAME_PROTOCOL       *DbgComponentNameProtocol;
  EFI_COMPONENT_NAME2_PROTOCOL      *DbgComponentName2Protocol;
  EFI_DEVICE_PATH_PROTOCOL          *DbgDevPath;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL  *DbgDpathToText;
  CHAR16                            *DbgTextStr;
  EFI_LOADED_IMAGE_PROTOCOL         *DbgLoadedImage;
  EFI_DRIVER_BINDING_PROTOCOL       *DriverBinding;
  
  DbgDriverName = NULL;
  DbgLangName1 = LANGUAGE_CODE_ENGLISH1;
  DbgLangName2 = LANGUAGE_CODE_ENGLISH2;
  DbgDpathToText = NULL;
  DbgLoadedImage = NULL;
  
  for (Index = 0; Index < gDrivingBindingIndex; Index++) {
    if (This == gDriverBindingInfo[Index].DriveBinding) {
      break;
    }
  }
  if (Index == gDrivingBindingIndex) {
    DEBUG ((EFI_D_ERROR, "!!!Error can not find the DriverBindingInfo(Start)\n"));
    return EFI_UNSUPPORTED;
  }

  DriverBinding = This;

  DbgStatus = GetHandleFromDriverBinding (DriverBinding, &DbgDriverBindingHandle);
  if (!EFI_ERROR (DbgStatus)) {
    DbgStatus = gBS->HandleProtocol (
                                   DbgDriverBindingHandle,
                                   &gEfiComponentNameProtocolGuid,
                                   (VOID **)&DbgComponentNameProtocol
                                   );
    if (EFI_ERROR (DbgStatus)) {
      DbgStatus = gBS->HandleProtocol (
                                     DbgDriverBindingHandle,
                                     &gEfiComponentName2ProtocolGuid,
                                     (VOID **)&DbgComponentName2Protocol
                                     );
      if (!EFI_ERROR (DbgStatus)) {
        // gEfiComponentName2Protocol
        DbgStatus = DbgComponentName2Protocol->GetDriverName (DbgComponentName2Protocol, DbgLangName2, &DbgDriverName);
      }
      goto DbgComponentNameDone;
    }
    // gEfiComponentNameProtocol
    DbgStatus = DbgComponentNameProtocol->GetDriverName (DbgComponentNameProtocol, DbgLangName1, &DbgDriverName);

DbgComponentNameDone:
    if (!EFI_ERROR (DbgStatus) && DbgDriverName != NULL) {
      DEBUG ((EFI_D_ERROR, "++(Driver Image)%s ...... Connecting\n", DbgDriverName));
    } else {
      DbgStatus = gBS->HandleProtocol (
                                       DbgDriverBindingHandle,
                                       &gEfiLoadedImageProtocolGuid,
                                       (VOID **)&DbgLoadedImage
                                       );
      if (!EFI_ERROR (DbgStatus)) {
        DEBUG ((EFI_D_ERROR, "++(Driver Image base)0x%lx ...... Connecting\n", DbgLoadedImage->ImageBase));
      } else {
        DEBUG ((EFI_D_ERROR, "++(Driver Image Handle)0x%lx ...... Connecting\n", DbgDriverBindingHandle));
      }
    }

    // Show Controller device path
    DEBUG ((EFI_D_ERROR, "(Contorller dpath)"));
    DbgStatus = gBS->LocateProtocol (
                                     &gEfiDevicePathToTextProtocolGuid,
                                     NULL,
                                     (VOID **)&DbgDpathToText
                                     );
    DbgStatus = gBS->HandleProtocol (
                                   Controller,
                                   &gEfiDevicePathProtocolGuid,
                                   (VOID **)&DbgDevPath
                                   );
    if (!EFI_ERROR (DbgStatus)) {
      if (DbgDpathToText != NULL) {
        DbgTextStr = DbgDpathToText->ConvertDevicePathToText (DbgDevPath, TRUE, TRUE);
        DEBUG ((EFI_D_ERROR, " %s \n", DbgTextStr));
        gBS->FreePool (DbgTextStr);
      }
    } else {
      DEBUG ((EFI_D_ERROR, " Handle 0x%lx \n", Controller));
    }
    if (RemainingDevicePath != NULL && DbgDpathToText != NULL) {
      DbgTextStr = DbgDpathToText->ConvertDevicePathToText (RemainingDevicePath, TRUE, TRUE);
      DEBUG ((EFI_D_ERROR, "(RemainingDevicePath) %s \n", DbgTextStr));
      gBS->FreePool (DbgTextStr);
    } else {
      DEBUG ((EFI_D_ERROR, "(RemainingDevicePath) NULL \n"));
    }
  }  
  
  // Call original DriverBinding Start function
  Status =  gDriverBindingInfo[Index].Start (This, Controller, RemainingDevicePath);
  
  DEBUG ((EFI_D_ERROR, "--(Driver)"));
  
  if (DbgDriverName != NULL) {
    DEBUG ((EFI_D_ERROR, "%s", DbgDriverName));
  } else {
    DbgStatus = gBS->HandleProtocol (
                                     DbgDriverBindingHandle,
                                     &gEfiLoadedImageProtocolGuid,
                                     (VOID **)&DbgLoadedImage
                                     );
    if (!EFI_ERROR (DbgStatus)) {
      DEBUG ((EFI_D_ERROR, "Image base 0x%lx", DbgLoadedImage->ImageBase));
    } else {
      DEBUG ((EFI_D_ERROR, "Image Handle 0x%lx", DbgDriverBindingHandle));
    }
  }
  DEBUG ((EFI_D_ERROR, " ...... return status = %r\n", Status));
  
  return Status;
}

EFI_STATUS
EFIAPI
DebugDriverBindingStop (
  IN  EFI_DRIVER_BINDING_PROTOCOL   *This,
  IN  EFI_HANDLE                    Controller,
  IN  UINTN                         NumberOfChildren,
  IN  EFI_HANDLE                    *ChildHandleBuffer
  )
{
  UINTN                             Index;
  EFI_STATUS                        Status;
  EFI_HANDLE                        DbgDriverBindingHandle;
  EFI_STATUS                        DbgStatus;
  CHAR16                            *DbgDriverName = NULL;
  CHAR8                             *DbgLangName1 = LANGUAGE_CODE_ENGLISH1;
  CHAR8                             *DbgLangName2 = LANGUAGE_CODE_ENGLISH2;
  EFI_COMPONENT_NAME_PROTOCOL       *DbgComponentNameProtocol;
  EFI_COMPONENT_NAME2_PROTOCOL      *DbgComponentName2Protocol;
  EFI_DEVICE_PATH_PROTOCOL          *DbgDevPath;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL  *DbgDpathToText = NULL;
  CHAR16                            *DbgTextStr;
  EFI_LOADED_IMAGE_PROTOCOL         *DbgLoadedImage = NULL;
  UINTN                             DbgIndex;
  EFI_DRIVER_BINDING_PROTOCOL       *DriverBinding;
  
  for (Index = 0; Index < gDrivingBindingIndex; Index++) {
    if (This == gDriverBindingInfo[Index].DriveBinding) {
      break;
    }
  }
  if (Index == gDrivingBindingIndex) {
    DEBUG ((EFI_D_ERROR, "!!!Error can not find the DriverBindingInfo(Stop)\n"));
    return EFI_UNSUPPORTED;
  }
  
  DriverBinding = This;
  DbgStatus = GetHandleFromDriverBinding (DriverBinding, &DbgDriverBindingHandle);
  if (!EFI_ERROR (DbgStatus)) {
    DbgStatus = gBS->HandleProtocol (
                                   DbgDriverBindingHandle,
                                   &gEfiComponentNameProtocolGuid,
                                   (VOID **)&DbgComponentNameProtocol
                                   );
    if (EFI_ERROR (DbgStatus)) {
      DbgStatus = gBS->HandleProtocol (
                                     DbgDriverBindingHandle,
                                     &gEfiComponentName2ProtocolGuid,
                                     (VOID **)&DbgComponentName2Protocol
                                     );
      if (!EFI_ERROR (DbgStatus)) {
        // gEfiComponentName2Protocol
        DbgStatus = DbgComponentName2Protocol->GetDriverName (DbgComponentName2Protocol, DbgLangName2, &DbgDriverName);
      }
      goto DbgComponentNameDone;
    }
    // gEfiComponentNameProtocol
    DbgStatus = DbgComponentNameProtocol->GetDriverName (DbgComponentNameProtocol, DbgLangName1, &DbgDriverName);

DbgComponentNameDone:
    if (!EFI_ERROR (DbgStatus) && DbgDriverName != NULL) {
      DEBUG ((EFI_D_ERROR, ">>(Driver Image)%s ...... Disconnecting\n", DbgDriverName));
    } else {
      DbgStatus = gBS->HandleProtocol (
                                       DbgDriverBindingHandle,
                                       &gEfiLoadedImageProtocolGuid,
                                       (VOID **)&DbgLoadedImage
                                       );
      if (!EFI_ERROR (DbgStatus)) {
        DEBUG ((EFI_D_ERROR, ">>(Driver Image base)0x%lx ...... Disconnecting\n", DbgLoadedImage->ImageBase));
      } else {
        DEBUG ((EFI_D_ERROR, ">>(Driver Image Handle)0x%lx ...... Disconnecting\n", DbgDriverBindingHandle));
      }
    }

    // Show Controller device path
    DEBUG ((EFI_D_ERROR, "(Contorller dpath)"));
    DbgStatus = gBS->LocateProtocol (
                                     &gEfiDevicePathToTextProtocolGuid,
                                     NULL,
                                     (VOID **)&DbgDpathToText
                                     );
    DbgStatus = gBS->HandleProtocol (
                                   Controller,
                                   &gEfiDevicePathProtocolGuid,
                                   (VOID **)&DbgDevPath
                                   );
    if (!EFI_ERROR (DbgStatus)) {
      if (DbgDpathToText != NULL) {
        DbgTextStr = DbgDpathToText->ConvertDevicePathToText (DbgDevPath, TRUE, TRUE);
        DEBUG ((EFI_D_ERROR, " %s \n", DbgTextStr));
        gBS->FreePool (DbgTextStr);
      }
    } else {
      DEBUG ((EFI_D_ERROR, " Handle 0x%lx \n", Controller));
    }

    if (NumberOfChildren > 0) {
      if (ChildHandleBuffer != NULL) {
        for (DbgIndex = 0; DbgIndex < NumberOfChildren; DbgIndex++) {
          DEBUG ((EFI_D_ERROR, "(ChildHandle dpath)"));
          DbgStatus = gBS->HandleProtocol (
                                     ChildHandleBuffer[DbgIndex],
                                     &gEfiDevicePathProtocolGuid,
                                     (VOID **)&DbgDevPath
                                     );
          if (!EFI_ERROR (DbgStatus)) {
            if (DbgDpathToText != NULL) {
              DbgTextStr = DbgDpathToText->ConvertDevicePathToText (DbgDevPath, TRUE, TRUE);
              DEBUG ((EFI_D_ERROR, " %s \n", DbgTextStr));
              gBS->FreePool (DbgTextStr);
            }
          } else {
            DEBUG ((EFI_D_ERROR, " Handle 0x%lx \n", ChildHandleBuffer[DbgIndex]));
          }
        }
      } else {
        DEBUG ((EFI_D_ERROR, " No ChildHandle \n"));
      }
    }
  }
  
  // Call original DriverBinding Stop function
  Status =  gDriverBindingInfo[Index].Stop (This, Controller, NumberOfChildren, ChildHandleBuffer);
  
  DEBUG ((EFI_D_ERROR, "<<(Driver)"));
  
  if (DbgDriverName != NULL) {
    DEBUG ((EFI_D_ERROR, "%s", DbgDriverName));
  } else {
    DbgStatus = gBS->HandleProtocol (
                                     DbgDriverBindingHandle,
                                     &gEfiLoadedImageProtocolGuid,
                                     (VOID **)&DbgLoadedImage
                                     );
    if (!EFI_ERROR (DbgStatus)) {
      DEBUG ((EFI_D_ERROR, "Image base 0x%lx", DbgLoadedImage->ImageBase));
    } else {
      DEBUG ((EFI_D_ERROR, "Image Handle 0x%lx", DbgDriverBindingHandle));
    }
  }
  DEBUG ((EFI_D_ERROR, " ...... return status = %r\n", Status));
  
  return Status;
}

/**
  Updated Binding protocol function pointer
  
  @param        Event
  @param        Context

  @retval

**/
VOID
RegisterDriverBindingCallback (
  IN EFI_EVENT       Event,
  IN VOID           *Context
)
{
  UINTN                               HandleBufSize;
  EFI_HANDLE                          Handle;
  EFI_STATUS                          Status;
  EFI_DRIVER_BINDING_PROTOCOL         *DriverBinding;
  
  if (gDrivingBindingTotalCount >= MAX_DRIVER_BINDING_INFO) {
    DEBUG ((EFI_D_ERROR, "!!!Error gDrivingBindingTotalCount over MAX_DRIVER_BINDING_INFO\n"));
    return;
  }
  
  do {
    HandleBufSize = sizeof (EFI_HANDLE);  
    Status = gBS->LocateHandle (
                      ByRegisterNotify,
                      NULL,
                      gRegistration,
                      &HandleBufSize,
                      &Handle
                      );
  
    if (Status == EFI_NOT_FOUND) {
      break;
    }
  
    Status = gBS->HandleProtocol (Handle, &gEfiDriverBindingProtocolGuid, (VOID **)&DriverBinding);
  
    if (EFI_ERROR(Status)) {
      break;
    }
  
    // Save original DriverBinding function pointer
    gDriverBindingInfo[gDrivingBindingIndex].DriveBinding = DriverBinding;
    gDriverBindingInfo[gDrivingBindingIndex].Supported    = DriverBinding->Supported;
    gDriverBindingInfo[gDrivingBindingIndex].Start        = DriverBinding->Start;
    gDriverBindingInfo[gDrivingBindingIndex].Stop         = DriverBinding->Stop;
    gDrivingBindingIndex++;
    gDrivingBindingTotalCount++;
    
    // Changed the DriverBinding function pointer to new one
    DriverBinding->Supported = DebugDriverBindingSupported;
    DriverBinding->Start     = DebugDriverBindingStart;
    DriverBinding->Stop      = DebugDriverBindingStop;
  } while (TRUE);
  
  return;
}

/**
  Entry point for the driver.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS       The entry point is executed successfully.

**/
EFI_STATUS
DxeDebugDriverEntryPoint (
  IN EFI_HANDLE         ImageHandle,
  IN EFI_SYSTEM_TABLE   *SystemTable
  )
{
#if defined(MDEPKG_NDEBUG)
  return  RETURN_UNSUPPORTED;
#else
  EFI_STATUS                        Status;
  EFI_EVENT                         RegistProtocolEvent;
  
  Status = EFI_SUCCESS;
  
  Status = gBS->CreateEvent (
                  EVT_NOTIFY_SIGNAL,
                  TPL_NOTIFY,
                  RegisterDriverBindingCallback,
                  NULL,
                  &RegistProtocolEvent
                  );
  ASSERT (!EFI_ERROR (Status));

  Status = gBS->RegisterProtocolNotify (
                  &gEfiDriverBindingProtocolGuid,
                  RegistProtocolEvent,
                  &gRegistration
                  );
    
  return Status;
#endif
}