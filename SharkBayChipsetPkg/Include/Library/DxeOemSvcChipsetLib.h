/** @file
  Definition for Dxe OEM Services Chipset Lib.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _DXE_OEM_SVC_CHIPSET_LIB_H_
#define _DXE_OEM_SVC_CHIPSET_LIB_H_
//[-start-121106-IB10820151-add]//
#include <Uefi.h>
#include <IndustryStandard/Acpi.h>
//[-end-121106-IB10820151-add]//
//[-start-121107-IB10820154-add]//
#include <Protocol/IgdOpRegion.h>
//[-end-121107-IB10820154-add]//
//[-start-121108-IB10820155-add]//
#include <Protocol/PciRootBridgeIo.h>
#include <Protocol/UsbLegacyPlatform.h>
//[-start-121218-IB10820203-add]//
#include <Protocol/GlobalNvsArea.h>
//[-end-121218-IB10820203-add]//
//[-start-121219-IB10820205-add]//
#include <Protocol/AcpiPlatformPolicy.h>
#include <Protocol/AmtPlatformPolicy.h>
//[-end-121219-IB10820205-add]//
//[-start-121220-IB10820206-add]//
#include <Protocol/AtPlatformPolicy.h>
#include <Protocol/PlatformGopPolicy.h>
#include <Protocol/ICCPlatformPolicy.h>
#include <Protocol/MePlatformPolicy.h>
#include <Protocol/PchPlatformPolicy.h>
#include <Protocol/RapidStartPlatformPolicy.h>
#include <Protocol/SaPlatformPolicy.h>
#include <Protocol/XtuPolicy.h>
//[-end-121220-IB10820206-add]//
//[-start-130102-IB05280028-add]//
#include <Protocol/I2cHidDevicesPolicy.h>
//[-end-130102-IB05280028-add]//
#include <ChipsetSetupConfig.h>
//[-end-121108-IB10820155-add]//
//[-start-121115-IB10820164-add]//
#include <Library/BadgingSupportLib.h>
//[-end-121115-IB10820164-add]//
//[-start-130503-12360003-add]//
#include <Protocol/BiosRegionLock.h>
//[-end-130503-12360003-add]//
//[-start-121220-IB10820206-remove]//
///**
// Returns Lid status.
//
// @param[in out]     DetectLidStatus     On entry, points to default lid status. 
//                                        On exit, points to returned lid status. 0 = Lid closed. 1 = Lid open 
//
// @retval            EFI_UNSUPPORTED     Returns unsupported by default.
// @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
//*/
//EFI_STATUS
//GetLidStatus (
//  IN OUT UINT8             *DetectLidStatus
//  );
//[-end-121220-IB10820206-remove]//
//[-start-121106-IB10820151-add]//
/**
 This function provides an interface to modify the ACPI description table header.

 @param[in out]     *Table              On entry, points to ACPI description table header.
                                        On exit , points to updated ACPI description table header.
 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
*/
EFI_STATUS
OemSvcModifyAcpiDescriptionHeader (
  IN OUT  EFI_ACPI_DESCRIPTION_HEADER            *Table
  );
//[-end-121106-IB10820151-add]//

//[-start-121219-IB10820204-remove]//
//[-start-121107-IB10820153-add]//
///**
//
// This function provides an interface to update OemGlobalNvs table content.
//
// @param[in]         None
//
// @retval            EFI_UNSUPPORTED     Returns unsupported by default.
// @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
//*/
//EFI_STATUS
//UpdateOemGlobalNvs (
//  VOID
//  );
//[-end-121107-IB10820153-add]//
//[-end-121219-IB10820204-remove]//

//[-start-121107-IB10820154-add]//
/**

 This function provides an interface to update IGD OpRegion content.

 @param [in out]    *IgdOpRegion        On entry, points to memorys buffer for Internal graphics device, 
                                        this buffer set aside comminocate between ACPI code and POST.
                                        On exit , points to updated memory buffer for Internal Graphics Device.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
*/
EFI_STATUS
OemSvcSetIgdOpRegion (
  IN OUT  IGD_OPREGION_PROTOCOL     *IgdOpRegion
  );
//[-end-121107-IB10820154-add]//

//[-start-121108-IB10820155-add]//
/**
 This function provides an interface to hook before and after DxePlatformEntryPoint (PlatformDxe.inf).PlatformDxe.inf
 is responsible for setting up any platform specific policy or initialization information.

 @param[in out]     *SetupVariable      On entry, points to CHIPSET_CONFIGURATION instance.
                                        On exit , points to updated CHIPSET_CONFIGURATION instance.
 @param[in out]     *PciRootBridgeIo    On entry, points to EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL instance.
                                        On exit , points to updated EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL instance.
 @param[in out]     Flag                TRUE : Before DxePlatformEntryPoint.
                                        FALSE: After DxePlatformEntryPoint.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcHookPlatformDxe (
  IN  CHIPSET_CONFIGURATION             *SetupVariable,
  IN  EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL  *PciRootBridgeIo,
  IN OUT  BOOLEAN                      Flag
  );

//[-start-121128-IB05280020-modify]//
/**
 This function provides an interface to modify USB legacy options.

 @param[in out]     *UsbLegacyModifiers On entry, points to USB_LEGACY_MODIFIERS instance.
                                        On exit , points to updated USB_LEGACY_MODIFIERS instance.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
*/
EFI_STATUS
OemSvcSetUsbLegacyPlatformOptions (
  IN OUT  USB_LEGACY_MODIFIERS                *UsbLegacyModifiers
  );
//[-end-121128-IB05280020-modify]//

//[-start-121114-IB05280012-remove]//
///**
//  This function provides an interface to modify Platform Monitor Key options.
//
//  @param[in out]  KeyList             List of keys to monitor from. This includes both USB & PS2 keyboard inputs.
//  @param[in out]  KeyListSize         Size of KeyList in bytes 
//
//  @retval EFI_UNSUPPORTED             Returns unsupported by default.
//  @retval EFI_MEDIA_CHANGED           Alter the Configuration Parameter or hook code.
//**/
//EFI_STATUS
//SetPlatformMonitorKeyOptions (
//  IN OUT  KEY_ELEMENT                      **KeyList,
//  IN OUT  UINTN                            *KeyListSize
//  );
//[-end-121114-IB05280012-remove]//
//[-end-121108-IB10820155-add]//
//[-start-121109-IB10820159-add]//
/**
 This function provides an interface to hook PlatformResetSystem 

 @param[in]         ResetType           Warm or cold.
 @param[in]         ResetStatus         Possible cause of reset.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcHookPlatformReset (
  IN EFI_RESET_TYPE           ResetType,
  IN EFI_STATUS               ResetStatus
  );
//[-end-121109-IB10820159-add]//
//[-start-121204-IB05280021-remove]//
////[-start-121113-IB10820160-add]//
///**
// This function provides an interface to get/set PrePost Hotkey.
//
// @param[in]         Flag                TRUE : Get PrePost Hotkey
//                                        FALSE: Set PrePost Hotkey
// @param[in out]     *HotKey             Postkey bit data
//                                        F1_KEY_BIT   0x01
//                                        F2_KEY_BIT   0x02
//                                        DEL_KEY_BIT  0x04
//                                        F10_KEY_BIT  0x08
//                                        F12_KEY_BIT  0x10
//                                        ESC_KEY_BIT  0x20
//                                        UP_ARROW_KEY_BIT 0x40
//                                        F3_KEY_BIT  0x80
//                                        F9_KEY_BIT  0x100
//
// @retval            EFI_UNSUPPORTED     Returns unsupported by default.
// @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
//*/
//EFI_STATUS
//GetSetOemPrePostHotKey (
//  IN BOOLEAN              Flag,
//  IN OUT UINT8            *Hotkey
//  );
////[-end-121113-IB10820160-add]//
//[-end-121204-IB05280021-remove]//
//[-start-121115-IB10820164-add]//
/**
 This function provides an interface to modify OEM Logo and POST String.

 @param[in out]     *EFI_OEM_BADGING_LOGO_DATA                   On entry, points to a structure that specifies image data.
                                                                 On exit , points to updated structure.
 @param[in out]     *BadgingDataSize    On entry, the size of EFI_OEM_BADGING_LOGO_DATA matrix.
                                        On exit , the size of updated EFI_OEM_BADGING_LOGO_DATA matrix.
 @param[in out]     *OemBadgingString   On entry, points to OEM_BADGING_STRING matrix.
                                        On exit , points to updated OEM_BADGING_STRING matrix.
 @param[in out]     *OemBadgingStringInTextMode                  On entry, points to OEM_BADGING_STRING matrix in text mode.
                                                                 On exit , points to updated OEM_BADGING_STRING matrix in text mode.
 @param[in out]     *StringCount        The number is POST string count.
                                        On entry, base on SetupVariable->QuietBoot 
                                                  1 : The number of entries in OemBadgingString,
                                                  0 : The number of entries in OemBadgingStringInTextMode.
                                        On exit , base on SetupVariable->QuietBoot 
                                                  1 : The number of entries in updated OemBadgingString,
                                                  0 : The number of entries in updated OemBadgingStringInTextMode.
 @param[in out]     *OemBadgingStringAfterSelectWithMe           On entry, points to OEM_BADGING_STRING matrix after selected.
                                                                 On exit , points to updated OEM_BADGING_STRING matrix after selected.
 @param[in out]     *OemBadgingStringAfterSelectWithMeInTextMode On entry, points to OEM_BADGING_STRINGmatrix after selected  in text mode.
                                                                 On exit , points to updated OEM_BADGING_STRING matrix after selected  in text mode.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
//[-start-121128-IB05280020-modify]//
EFI_STATUS
OemSvcUpdateOemBadgingLogoData (
  IN OUT EFI_OEM_BADGING_LOGO_DATA            **EfiOemBadgingLogoData,
  IN OUT UINTN                                *BadgingDataSize,
  IN OUT OEM_BADGING_STRING                   **OemBadgingString,
  IN OUT OEM_BADGING_STRING                   **OemBadgingStringInTextMode,
  IN OUT UINTN                                *StringCount,
  IN OUT OEM_BADGING_STRING                   **OemBadgingStringAfterSelectWithMe,
  IN OUT OEM_BADGING_STRING                   **OemBadgingStringAfterSelectWithMeInTextMode
  );
//[-end-121128-IB05280020-modify]//
//[-end-121115-IB10820164-add]//
//[-start-121218-IB10820203-add]//
//[-start-121219-IB10820204-modify]//
/**
 This function provides an interface to update GlobalNvs table content.

 @param[in, out]    mGlobalNvsArea      On entry, points to EFI_GLOBAL_NVS_AREA  instance.
                                        On exit , points to updated EFI_GLOBAL_NVS_AREA  instance.
 @param[in, out]    mOemGlobalNvsArea   On entry, points to EFI_OEM_GLOBAL_NVS_AREA  instance.
                                        On exit , points to updated EFI_OEM_GLOBAL_NVS_AREA  instance.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateGlobalNvs (
  EFI_GLOBAL_NVS_AREA                   *mGlobalNvsArea,
  EFI_OEM_GLOBAL_NVS_AREA               *mOemGlobalNvsArea
  );
//[-end-121219-IB10820204-modify]//
//[-end-121218-IB10820203-add]//
//[-start-121219-IB10820205-add]//
/**
 This function offers an interface to modify ACPI_PLATFORM_POLICY_PROTOCOL Data before the system 
 installs ACPI_PLATFORM_POLICY_PROTOCOL.

 @param[in, out]    *mAcpiPlatformPolicyProtocol  On entry, points to ACPI_PLATFORM_POLICY_PROTOCOL structure.
                                                  On exit, points to updated ACPI_PLATFORM_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configurat on Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateAcpiPlatformPolicy (
  IN OUT ACPI_PLATFORM_POLICY_PROTOCOL  *AcpiPlatformPolicyProtocol
  );
/**
 This function offers an interface to modify DXE_AMT_POLICY_PROTOCOL Data before the system
 installs DXE_AMT_POLICY_PROTOCOL.

 @param[in, out]    *AMTPlatformPolicy  On entry, points to DXE_AMT_POLICY_PROTOCOL structure.
                                        On exit, points to updated DXE_AMT_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateAmtPlatformPolicy (
  IN OUT DXE_AMT_POLICY_PROTOCOL        *AmtPlatformPolicy
  );
//[-end-121219-IB10820205-add]//
//[-start-121220-IB10820206-add]//
/**
 This function offers an interface to modify DXE_AT_POLICY_PROTOCOL Data before the system
 installs DXE_AT_POLICY_PROTOCOL.

 @param[in, out]    *AtPlatformPolicy   On entry, points to DXE_AT_POLICY_PROTOCOL structure.
                                        On exit, points to updated DXE_AT_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateDxeAtPolicy (
    IN OUT DXE_AT_POLICY_PROTOCOL       *AtPlatformPolicy
  );

/**
 This function offers an interface to modify DXE_CPU_PLATFORM_POLICY_PROTOCOL Data before the system
 installs DXE_CPU_PLATFORM_POLICY_PROTOCOL.

 @param[in, out]    *CPUPlatformPolicy  On entry, points to DXE_CPU_PLATFORM_POLICY_PROTOCOL structure.
                                        On exit, points to updated DXE_CPU_PLATFORM_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateDxeCpuPlatformPolicy (
  IN OUT VOID                           *CPUPlatformPolicy
  );

/**
 This function offers an interface to modify PLATFORM_GOP_POLICY_PROTOCOL Data before the system
 installs PLATFORM_GOP_POLICY_PROTOCOL.

 @param[in, out]    *PlatformGopPolicy  On entry, points to PLATFORM_GOP_POLICY_PROTOCOL structure.
                                        On exit, points to updated PLATFORM_GOP_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdatePlatformGopPolicy (
  IN OUT PLATFORM_GOP_POLICY_PROTOCOL   *PlatformGopPolicy
  );

/**
 This function offers an interface to modify DXE_PLATFORM_ICC_POLICY_PROTOCOL Data before the system
 installs DXE_PLATFORM_ICC_POLICY_PROTOCOL.

 @param[in, out]    *IccPlatformPolicy  On entry, points to DXE_PLATFORM_ICC_POLICY_PROTOCOL structure.
                                        On exit, points to updated DXE_PLATFORM_ICC_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateDxePlatformIccPolicy (
  IN OUT DXE_PLATFORM_ICC_POLICY_PROTOCOL         *IccPlatformPolicy
  );

/**
 This function offers an interface to modify DXE_ME_POLICY_PROTOCOL Data before the system
 installs DXE_ME_POLICY_PROTOCOL.

 @param[in, out]    *MePlatformPolicy   On entry, points to DXE_ME_POLICY_PROTOCOL structure.
                                        On exit, points to updated DXE_ME_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateDxeMePolicy (
  IN OUT DXE_ME_POLICY_PROTOCOL         *MePlatformPolicy
  );

/**
 This function offers an interface to modify DXE_PCH_PLATFORM_POLICY_PROTOCOL Data before the system
 installs DXE_PCH_PLATFORM_POLICY_PROTOCOL.

 @param[in, out]    *PchPolicy          On entry, points to DXE_PCH_PLATFORM_POLICY_PROTOCOL structure.
                                        On exit, points to updated DXE_PCH_PLATFORM_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdatDxePchPlatformPolicy (
  IN OUT DXE_PCH_PLATFORM_POLICY_PROTOCOL         *PchPolicy
  );

/**
 This function offers an interface to modify RAPID_START_PLATFORM_POLICY_PROTOCOL Data before the system
 installs RAPID_START_PLATFORM_POLICY_PROTOCOL.

 @param[in, out]    *DxePlatformRapidStartPolicy  On entry, points to RAPID_START_PLATFORM_POLICY_PROTOCOL structure.
                                                  On exit, points to updated RAPID_START_PLATFORM_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateRapidStartPlatformPolicy (
  IN OUT RAPID_START_PLATFORM_POLICY_PROTOCOL     *DxePlatformRapidStartPolicy
  );

/**
 This function offers an interface to modify DXE_PLATFORM_SA_POLICY_PROTOCOL Data before the system
 installs DXE_PLATFORM_SA_POLICY_PROTOCOL.

 @param[in, out]    *SaPlatformPolicy   On entry, points to DXE_PLATFORM_SA_POLICY_PROTOCOL structure.
                                        On exit, points to updated DXE_PLATFORM_SA_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateDxePlatformSaPolicy (
  IN OUT DXE_PLATFORM_SA_POLICY_PROTOCOL          *SaPlatformPolicy
  );

/**
 This function offers an interface to modify DXE_XTU_POLICY_PROTOCOL Data before the system
 installs DXE_XTU_POLICY_PROTOCOL.

 @param[in, out]    *XtuPlatformPolicy  On entry, points to DXE_XTU_POLICY_PROTOCOL structure.
                                        On exit, points to updated DXE_XTU_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateDxeXtuPolicy (
  IN OUT DXE_XTU_POLICY_PROTOCOL        *XtuPlatformPolicy
  );
//[-end-121220-IB10820206-add]//
//[-start-130102-IB05280028-add]//
/**
 This function offers an interface to modify EFI_I2C_HID_DEVICES_POLICY_PROTOCOL Data before the system
 installs EFI_I2C_HID_DEVICES_POLICY_PROTOCOL.

 @param[in, out]   *I2cPlatformPolicy   On entry, points to EFI_I2C_HID_DEVICES_POLICY_PROTOCOL structure.
                                        On exit, points to updated EFI_I2C_HID_DEVICES_POLICY_PROTOCOL structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateI2cPlatformPolicy (
  IN OUT EFI_I2C_HID_DEVICES_POLICY_PROTOCOL        *I2cPlatformPolicy
  );
//[-end-130102-IB05280028-add]//

//[-start-130613-IB05400415-modify]//
//[-start-130422-IB05400398-add]//
/**
 This function offers an interface to update board id string.

 @param[in]         BoardId                       Current board id.
 @param[out]        *StringBuffer                 String buffer will store board id string.
 @param[in]         StringBufferSize              String buffer size.
 @param [in]        SrcHiiHandle                  New Hii Handle.
 @param [in]        SrcStringId                   New String Token.
 @param [in]        DstHiiHandle                  Hii Handle of the package to be updated.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateBoardIdString (
  IN   UINT8                    BoardId,
  OUT  CHAR16                   *StringBuffer,
  IN   UINTN                    StringBufferSize,
  IN   EFI_HII_HANDLE           SrcHiiHandle,
  IN   EFI_STRING_ID            SrcStringId,
  IN   EFI_HII_HANDLE           DstHiiHandle
  );

/**
 This function provides an interface to update SSDT install status.

 @param[in]         OemTableId          OEM table id of this table.
 @param[in]         SetupVariable       Pointer to chipset configuration structure.
 @param[out]        NewStatus           Pointer to updated status.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
*/
EFI_STATUS
OemSvcUpdateSsdtStatus (
  IN  UINT64                            OemTableId,
  IN  CHIPSET_CONFIGURATION             *SetupVariable,
  OUT EFI_STATUS                        *NewStatus
  );
//[-end-130422-IB05400398-add]//
//[-end-130613-IB05400415-modify]//
//[-start-130424-12360003-add]//
/**
  Customize BIOS protect region before boot to OS. 

  @param[out]        *BiosRegionTable    Pointer to BiosRegion Table.
  @param[out]        ProtectRegionNum    The number of Bios protect region instances.
  
  @retval            EFI_UNSUPPORTED     Returns unsupported by default, OEM did not implement this function.
  @retval            EFI_MEDIA_CHANGED   Provide table for kernel to set protect region and lock flash program registers. 
**/
EFI_STATUS
OemSvcUpdateBiosProtectTable (
//[-start-130927-IB12360020-modify]//
  OUT BIOS_PROTECT_REGION           **BiosRegionTable,
  OUT UINT8                         *ProtectRegionNum
//[-end-130927-IB12360020-modify]//
  );
//[-end-130424-12360003-add]//
#endif
