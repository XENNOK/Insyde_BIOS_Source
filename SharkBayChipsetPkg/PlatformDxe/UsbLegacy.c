/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

/*++


@file:

  UsbLegacy.c

@brief:
  The Usb Legacy Protocol is used provide a standard interface to the
  UsbLegacy code for platform modifications of operation.

--*/
#include <Platform.h>
//[-start-121108-IB10820155-remove]//
//#include <OemPlatform.h>
//[-end-121108-IB10820155-remove]//
//[-start-121109-IB05280008-add]//
#include <PostKey.h>
//[-end-121109-IB05280008-add]//

#include <Protocol/UsbLegacyPlatform.h>
#include <Guid/ConsoleLock.h>
//[-start-121108-IB10820155-add]//
#include <Library/DxeOemSvcChipsetLib.h>
//[-end-121108-IB10820155-add]//



//[-start-121109-IB05280008-remove]//
//KEY_ELEMENT mKeyList;
//[-end-121109-IB05280008-remove]//

//
// Local  prototypes
//
EFI_USB_LEGACY_PLATFORM_PROTOCOL    mUsbLegacyPlatform;

/**

  Return all platform information that can modify USB legacy operation

  @param[IN]  This                 Protocol instance pointer. 
  @param[OUT]  UsbLegacyModifiers   List of keys to monitor from. This includes both

  @retval   EFI_SUCCESS    Modifiers exist.
                EFI_NOT_FOUND  Modifiers not not exist.
                
**/
EFI_STATUS
GetUsbPlatformOptions (
  IN   EFI_USB_LEGACY_PLATFORM_PROTOCOL *This,
  OUT  USB_LEGACY_MODIFIERS             *UsbLegacyModifiers
  )
{
  EFI_SETUP_UTILITY_PROTOCOL            *SetupUtility;
  EFI_STATUS                            Status;
  CHIPSET_CONFIGURATION                  *SystemSteupPtr;

  UsbLegacyModifiers->UsbLegacyEnable           = 0x00;
  UsbLegacyModifiers->UsbZip                    = 0x00;
  UsbLegacyModifiers->UsbZipEmulation           = 0x00;
  UsbLegacyModifiers->UsbFixedDiskWpBootSector  = 0x00;
  UsbLegacyModifiers->UsbBoot                   = 0x00;
  SystemSteupPtr = NULL;
  SetupUtility = NULL;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  SystemSteupPtr = (CHIPSET_CONFIGURATION*)SetupUtility->SetupNvData;
  //
  // Check SETUP for behavior modifications
  //

  if (SystemSteupPtr->LegacyUsbSupport == 0x01) {
    UsbLegacyModifiers->UsbLegacyEnable = 0x01;
  }
//[-start-120709-IB02040279-add]//
  if (SystemSteupPtr->LegacyUsbSupport == 0x02) {
    UsbLegacyModifiers->UsbUefiOnly = 0x01;
  }
//[-end-120709-IB02040279-add]//
  UsbLegacyModifiers->UsbZip = 0x02;

  UsbLegacyModifiers->UsbZipEmulation =
       SystemSteupPtr->UsbZipEmulation;

  UsbLegacyModifiers->UsbFixedDiskWpBootSector = 0x00;


  //
  // Set UsbBoot default value Support USB boot
  //
  if (SystemSteupPtr->UsbBoot == 0) {
    UsbLegacyModifiers->UsbBoot = 0x02;//DEFINE_USB_BOOT_ENABLE;
  } else if (SystemSteupPtr->UsbBoot == 1) {
    UsbLegacyModifiers->UsbBoot = 0x01;//DEFINE_USB_BOOT_DISABLED;
  }

  //
  // Set Support USB2.0 Device
  //
  if ((SystemSteupPtr->Ehci1Enable != 0) || (SystemSteupPtr->Ehci2Enable != 0)) {
     UsbLegacyModifiers->UsbEhciSupport = 0x01;
  }
//[-start-121108-IB10820155-modify]//
//[-start-130524-IB05160451-modify]//
//[-start-121128-IB05280020-modify]//
  Status = OemSvcSetUsbLegacyPlatformOptions (UsbLegacyModifiers);
//[-end-121128-IB05280020-modify]//
  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib OemSvcSetUsbLegacyPlatformOptions, Status : %r\n", Status ) );
//[-end-130524-IB05160451-modify]//

  return EFI_SUCCESS;
//[-end-121108-IB10820155-modify]//

}

/**

  Return all platform information that can modify USB legacy operation

  @param[IN]   This         Protocol instance pointer.
  @param[OUT]  KeyList      List of keys to monitor from. This includes both USB & PS2 keyboard inputs.
  @param[OUT]  KeyListSize  Size of KeyList in bytes
  
  @retval:
   EFI_SUCCESS    Keys are to be monitored.
   EFI_NOT_FOUND  No keys are to be monitored.
                
**/
EFI_STATUS
GetPlatformMonitorKeyOptions (
  IN   EFI_USB_LEGACY_PLATFORM_PROTOCOL *This,
  OUT  KEY_ELEMENT                    **KeyList,
  OUT  UINTN                          *KeyListSize
  )
{
  EFI_STATUS               Status;
  UINT8                    ConsoleLock = 0;
  UINTN                    ConsoleLockDataSize;
//[-start-121109-IB05280008-add]//
  UINT8                    Count;
  KEY_ELEMENT              EndEntry;

  Count =0;
  
  ZeroMem(&EndEntry, sizeof(KEY_ELEMENT));
//[-end-121109-IB05280008-add]//

  ConsoleLockDataSize = sizeof (ConsoleLock);

  if (FeaturePcdGet (PcdMeSupported)) {
    Status = gRT->GetVariable (
                    EFI_CONSOLE_LOCK_VARIABLE_NAME,
                    &gEfiConsoleLockGuid,
                    NULL,
                    &ConsoleLockDataSize,
                    &ConsoleLock
                    );

    if (EFI_ERROR (Status)) {
      DEBUG ((EFI_D_INIT | EFI_D_INFO, "Find console lock variable failed !!\n"));
      ConsoleLock = 0;
    }
  }

  if (ConsoleLock == 1) {
    //
    // ConsoleLock, monitor key always not found, prevent from entering SCU and boot manager
    //
    return EFI_NOT_FOUND;
  }
  //
  // Return KeyList if it exists else return
  // *KeyList = NULL;
  // *KeyListSize = 0;
  //
//[-start-140210-IB05400513-modify]//
//[-start-121109-IB05280008-modify]//
  //*KeyList = &mKeyList[0];
  * KeyList = (KEY_ELEMENT *)PcdGetPtr (PcdPlatformKeyList);
  while (CompareMem (&EndEntry, &((*KeyList)[Count]),sizeof(KEY_ELEMENT)) !=0 ) {
    Count++;
  }
  *KeyListSize = Count * sizeof(KEY_ELEMENT);
//[-end-121109-IB05280008-modify]//
//[-end-140210-IB05400513-modify]//

//[-start-121108-IB10820155-modify]//
//[-start-121114-IB05280012-remove]//
//  Status = SetPlatformMonitorKeyOptions (KeyList, KeyListSize); 
//  DEBUG ( ( EFI_D_ERROR | EFI_D_INFO, "DxeOemSvcChipsetLib SetPlatformMonitorKeyOptions, Status : %r\n", Status ) );
//[-end-121114-IB05280012-remove]//

  return EFI_SUCCESS;
//[-end-121108-IB10820155-modify]// 

}

/**

  Install Driver to produce USB Legacy platform protocol.

  @param  VOID      

  @retval:

    EFI_SUCCESS  USB Legacy Platform protocol installed
    Other        No protocol installed, unload driver.
                
**/
EFI_STATUS
UsbLegacyPlatformInstall (
  )
{
  EFI_STATUS                           Status;
  EFI_HANDLE                           Handle;
  //
  // Grab a copy of all the protocols we depend on. Any error would
  // be a dispatcher bug!.
  //


  mUsbLegacyPlatform.GetUsbPlatformOptions  = GetUsbPlatformOptions;
  mUsbLegacyPlatform.GetPlatformMonitorKeyOptions  = GetPlatformMonitorKeyOptions;


  //
  // Make a new handle and install the protocol
  //
  Handle = NULL;
  Status = gBS->InstallProtocolInterface (
                  &Handle,
                  &gEfiUsbLegacyPlatformProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mUsbLegacyPlatform
                  );
  return Status;
}
