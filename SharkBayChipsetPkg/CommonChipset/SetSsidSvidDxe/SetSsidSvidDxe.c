/** @file
  Dxe driver will register a ready to boot event to program SsidSvid.

;******************************************************************************
;* Copyright (c) 2012-2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>
#include <IndustryStandard/Pci.h>
#include <Library/IoLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>
#include <Library/CommonPciLib.h>
#include <Library/PciExpressLib.h>
//[-start-130409-IB06720212-remove]//
//#include <Library/PlatformSsidLib.h>
//[-end-130409-IB06720212-remove]//
#include <Library/S3BootScriptLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/DxeChipsetSvcLib.h>
//[-start-130903-IB12360023-add]//
#include <Library/PcdLib.h>
//[-end-130903-IB12360023-add]//
#include <Library/DxeOemSvcKernelLib.h>
//[-start-130429-IB10130047-add]//
//[-start-131119-IB07390115-modify]//
#include <Protocol/PciEnumerationComplete.h>
//[-end-131119-IB07390115-modify]//
//[-end-130429-IB10130047-add]//

VOID
EFIAPI
SetSsidSvidCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  );

VOID
ProgramSsidSvid (
  IN     UINT8     Bus,
  IN     UINT8     Dev,
  IN     UINT8     Func,
  IN     UINT32    SsidSvid
  );

//[-start-130409-IB06720212-add]//
//[-start-140115-IB07390124-remove]//
//EFI_STATUS
//ProgramSsidSvidForSpecialDevice(
//  UINT8                                      Bus,
//  UINT8                                      Dev,
//  UINT8                                      Func,
//  UINT16                                     VendorId,
//  UINT16                                     DeviceId,
//  UINT16                                     ClassCode,
//  UINT32                                     SsidSvid
//  );
//
//EFI_STATUS
//ProgramSsidSvidFunction0x10DE (
//  IN     UINT8    Bus,
//  IN     UINT8    Dev,
//  IN     UINT8    Func,
//  IN     UINT32   SsidSvid
//  );
//
//EFI_STATUS
//ProgramSsidSvidFunction0x1002 (
//  IN     UINT8    Bus,
//  IN     UINT8    Dev,
//  IN     UINT8    Func,
//  IN     UINT32   SsidSvid
//  );
//
//OEM_SSID_SVID_TABLE mPlatformSsidTableDefault[] = {
//  0x10DE, DEVICE_ID_DONT_CARE, ProgramSsidSvidFunction0x10DE,
//  0x1002, DEVICE_ID_DONT_CARE, ProgramSsidSvidFunction0x1002,
//  DEVICE_ID_DONT_CARE, DEVICE_ID_DONT_CARE, NULL
//};
//[-end-140115-IB07390124-remove]//
//[-end-130409-IB06720212-add]//

/**
  The Entry Point of SetSsidSvidDxe. It register a ready to boot event for programming SsidSvid.

  @param ImageHandle    The firmware allocated handle for the EFI image.
  @param SystemTable    A pointer to the EFI System Table.

  @retval EFI_SUCCESS   The entry point is executed successfully.

**/
EFI_STATUS
EFIAPI
SetSsidSvidEntryPoint (
  IN     EFI_HANDLE          ImageHandle,
  IN     EFI_SYSTEM_TABLE    *SystemTable
  )
{
//[-start-130429-IB10130047-remove]//
//  EFI_STATUS                                 Status;
//  EFI_EVENT                                  ReadyToBootEvent;
//
//  Status = EfiCreateEventReadyToBootEx (
//             TPL_NOTIFY,
//             SetSsidSvidCallBack,
//             NULL,
//             &ReadyToBootEvent
//             );
//  ASSERT_EFI_ERROR (Status);
//    
//  return Status;
//[-end-130429-IB10130047-remove]//
//[-start-130429-IB10130047-add]//
//[-start-130605-12360012-modify]//
  EFI_EVENT                                  SetSsidSvidEvent;
  VOID                                       *Registration; 

//[-start-131119-IB07390115-modify]//
  SetSsidSvidEvent = EfiCreateProtocolNotifyEvent (
                       &gEfiPciEnumerationCompleteProtocolGuid,
                       TPL_NOTIFY,
                       SetSsidSvidCallBack,
                       NULL,
                       &Registration
                       );
//[-end-131119-IB07390115-modify]//

  ASSERT (SetSsidSvidEvent != NULL);
 
  return EFI_SUCCESS;
//[-end-130605-12360012-modify]//
//[-end-130429-IB10130047-add]//
}

VOID
EFIAPI
SetSsidSvidCallBack (
  IN EFI_EVENT        Event,
  IN VOID             *Context
  )
{
  EFI_STATUS          Status;
  UINT8               Bus;
  UINT8               Dev;
  UINT8               Func;
  UINT32              SsidSvid;
  UINT8               Value8;
  UINT16              VendorId;
  UINT16              DeviceId;
  UINT16              ClassCode;
  UINT8               HeaderType;
  UINT8               BusLimit;
  UINT8               FuncLimit;
//[-start-131008-IB06720235-modify]//
//[-start-130411-IB06720212-add]//
//[-start-140115-IB07390124-remove]//
//  UINT32               DefaultSsidSvid;
//[-end-140115-IB07390124-remove]//
//[-end-130411-IB06720212-add]//
//[-end-131008-IB06720235-modify]//
//[-start-130605-12360012-add]//
  VOID                *ProtocolPointer;

  //
  // Check whether this is real ExitPmAuth notification, or just a SignalEvent
  //
//[-start-131119-IB07390115-modify]//
  Status = gBS->LocateProtocol (&gEfiPciEnumerationCompleteProtocolGuid, NULL, (VOID **)&ProtocolPointer);
//[-end-131119-IB07390115-modify]//
  if (EFI_ERROR (Status)) {
    return ;
  }
//[-end-130605-12360012-add]//
  Status   = EFI_SUCCESS;

  BusLimit = 1;
  SsidSvid = 0xFFFFFFFF;

  for (Bus = 0; Bus <= BusLimit; Bus++) {
    for (Dev = 0; Dev <= 0x1F ; Dev++) {
//[-start-140115-IB07390124-modify]//
      ClassCode = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, 0, PCI_CLASSCODE_OFFSET + 1));
      if (ClassCode == 0xFFFF) {
        continue;
      }
      
      HeaderType = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, 0, PCI_HEADER_TYPE_OFFSET));
      if (HeaderType & HEADER_TYPE_MULTI_FUNCTION) {
        FuncLimit = 7;
      } else {
        FuncLimit = 0;
      }
      
      for (Func = 0; Func <= FuncLimit; Func++) {
        ClassCode = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_CLASSCODE_OFFSET + 1));
        
        if (ClassCode == 0xFFFF) {
          continue;
        } else if (ClassCode == ((PCI_CLASS_BRIDGE << 8) | PCI_CLASS_BRIDGE_P2P)) {
          Value8 = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_BRIDGE_SUBORDINATE_BUS_REGISTER_OFFSET));
          if (Value8 > BusLimit) {
            BusLimit = Value8;
          }
        }
//[-end-140115-IB07390124-modify]//

        VendorId = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_VENDOR_ID_OFFSET));
        DeviceId = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_DEVICE_ID_OFFSET));

//[-start-131007-IB06720235-modify]//
        SsidSvid = (UINT32)PcdGet32(PcdDefaultSsidSvid);

        if (SsidSvid == 0) {
//[-start-140115-IB07390124-modify]//
//[-start-130411-IB06720212-add]//
//[-start-130717-12360016-modify]//
          SsidSvid = ((UINT32)DeviceId << 16) + VendorId;
//[-end-130717-12360016-modify]//
//[-end-130411-IB06720212-add]//
        }
//[-end-131007-IB06720235-modify]//

	      //
	      // OemServices
	      //
        Status = OemSvcUpdateSsidSvidInfo (Bus, Dev, Func, VendorId, DeviceId, ClassCode, &SsidSvid);
        if (!EFI_ERROR(Status)) {
          continue;
        }

//[-start-140115-IB07390124-remove]//
//        //
//        // 2. Program SSID/SVID, this function should only service project specific device which should not be at Bus 0
//        //
//        if (Bus > 0) {
////[-start-130409-IB06720212-modify]//
//          Status = ProgramSsidSvidForSpecialDevice (Bus, Dev, Func, VendorId, DeviceId, ClassCode, SsidSvid);
////[-end-130409-IB06720212-modify]//
//          if (!EFI_ERROR(Status)) {
//             continue;
//          }
//        }
//[-start-140115-IB07390124-remove]//
        
        Status = DxeCsSvcProgramChipsetSsid (Bus, Dev, Func, VendorId, DeviceId, SsidSvid);
        if (!EFI_ERROR(Status)) {
           continue;
        } 
        ProgramSsidSvid (Bus, Dev, Func, SsidSvid);
      }
    } 
  }
  return ;
}

VOID
ProgramSsidSvid (
  IN     UINT8     Bus,
  IN     UINT8     Dev,
  IN     UINT8     Func,
  IN     UINT32    SsidSvid
  )
{
  UINT8           PciHeaderType;
  UINT8           SubsystemCapOffset;
  UINT8           PciSsidOffset;
  EFI_STATUS      Status;
  UINT64          BootScriptPciAddress;
//[-start-130903-IB12360023-remove]//
//#ifdef NO_BRIDGE_DEVICE_SSID
//  UINT16          ClassCode;
//#endif
//[-end-130903-IB12360023-remove]//


  PciHeaderType = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_HEADER_TYPE_OFFSET));

//[-start-130903-IB12360023-modify]//
  if (FixedPcdGetBool (PcdNoBridgeDeviceSsid)) {
    UINT16          ClassCode;
    //
    // According to PCI spec Section "Subsystem Vendor ID and Subsystem ID"
    //   Base class 6 with sub class 0,1,2,3,4, or base class 8 with sub class 0,1,2,3
    //   are excluded from the requirement of programming these registers
    //
    ClassCode = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_CLASSCODE_OFFSET + 1));
    if ((ClassCode >= (PCI_CLASS_BRIDGE << 8) + PCI_CLASS_BRIDGE_HOST) 
      && (ClassCode <= (PCI_CLASS_BRIDGE << 8) + PCI_CLASS_BRIDGE_P2P)) {
      return;
    }
    if ((ClassCode >= (PCI_CLASS_SYSTEM_PERIPHERAL << 8) + PCI_SUBCLASS_PIC) && 
      (ClassCode <= (PCI_CLASS_SYSTEM_PERIPHERAL << 8) + PCI_SUBCLASS_RTC)) {
      return;
    }
  }
//[-end-130903-IB12360023-modify]//

  if (( PciHeaderType & HEADER_LAYOUT_CODE) != HEADER_TYPE_DEVICE ) {
    Status = PciFindCapId ( Bus, Dev, Func, EFI_PCI_CAPABILITY_ID_SSID, &SubsystemCapOffset );
    if ( !EFI_ERROR ( Status ) ) {
      PciSsidOffset = SubsystemCapOffset + 0x04;
      PciExpressWrite32 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PciSsidOffset), SsidSvid);

      BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Dev, Func, PciSsidOffset);
      S3BootScriptSavePciCfgWrite (
          S3BootScriptWidthUint32, 
          BootScriptPciAddress,
          1, 
          &SsidSvid);

    }
  } else {
    PciExpressWrite32 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Func, PCI_SUBSYSTEM_VENDOR_ID_OFFSET), SsidSvid);

    BootScriptPciAddress = S3_BOOT_SCRIPT_LIB_PCI_ADDRESS (Bus, Dev, Func, PCI_SUBSYSTEM_VENDOR_ID_OFFSET);
    S3BootScriptSavePciCfgWrite (
        S3BootScriptWidthUint32, 
        BootScriptPciAddress,
        1, 
        &SsidSvid);
    
  }
  return;
}

//[-start-130409-IB06720212-add]//
//[-start-131007-IB06720235-modify]//
//[-start-140115-IB07390124-remove]//
///**
//  Program SSID/SVID on special add-on device with special mechanism.
//
//  @param[in] Bus        PCI Bus number.
//  @param[in] Dev        PCI Device number.
//  @param[in] Func       PCI Function number.
//  @param[in] VendorId   Vendor ID.
//  @param[in] DeviceId   Device ID.
//  @param[in] ClassCode  Class Code.
//  @param[in] SsidSvid   SSID/SVID.
//
//  @retval EFI_SUCCESS     Find out the special add-on device from list, 
//                          and program SSID/SVID on it successfully.
//  @retval EFI_UNSUPPORTED Can't find the special add-on device, 
//                          and program SSID/SVID as default.
//
//**/
//EFI_STATUS
//ProgramSsidSvidForSpecialDevice(
//  IN UINT8             Bus,
//  IN UINT8             Dev,
//  IN UINT8             Func,
//  IN UINT16            VendorId,
//  IN UINT16            DeviceId,
//  IN UINT16            ClassCode,
//  IN UINT32            SsidSvid
//  )
//{
//  UINT8                               Index;
////  OEM_SSID_SVID_TABLE                 *OemPlatformSsidTable;
//
////  OemPlatformSsidTable = NULL;
//
////
//// Get special programming add-on device list from default PlatformSsidTable.
////
//  for (Index = 0; mPlatformSsidTableDefault[Index].SpecialSsidSvidFunction != NULL; Index++ ) {
//    if (mPlatformSsidTableDefault[Index].VendorId == VendorId) {
//      if ((mPlatformSsidTableDefault[Index].DeviceId == DEVICE_ID_DONT_CARE)
//        || (mPlatformSsidTableDefault[Index].DeviceId == DeviceId)){
//        mPlatformSsidTableDefault[Index].SpecialSsidSvidFunction (Bus, Dev, Func, SsidSvid);
//        return EFI_SUCCESS;
//      }
//    }
//  }
//
//////
////// Get special programming add-on device from OEM PlatformSsidTable.
//////
////  OemSvcProgramSsidSvidForSpecialDevice (&OemPlatformSsidTable);
////  if (OemPlatformSsidTable == NULL) {
////    return EFI_UNSUPPORTED;
////  }
////  
////  for (Index = 0; OemPlatformSsidTable[Index].SpecialSsidSvidFunction != NULL; Index++ ) {
////    if (OemPlatformSsidTable[Index].VendorId == VendorId) {
////      if ((OemPlatformSsidTable[Index].DeviceId == DEVICE_ID_DONT_CARE)
////        || (OemPlatformSsidTable[Index].DeviceId == DeviceId)){
////        OemPlatformSsidTable[Index].SpecialSsidSvidFunction (Bus, Dev, Func, SsidSvid);
////        return EFI_SUCCESS;
////      }
////    }
////  }
//  
//  return EFI_UNSUPPORTED;
//}
//[-end-140115-IB07390124-remove]//
//[-end-131007-IB06720235-modify]//
//[-end-130409-IB06720212-add]//


