/** @file

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

/*++

Module Name:

  AspmOverrideDxe.c

Abstract:

  A DXE driver for override the device ASPM setting.

--*/

#include "AspmOverrideDxe.h"
#include <Library/PciExpressLib.h>

PCIE_BRIDGE_MAP_TABLE                   *mPcieBridgeMap; //To record each bridge's info. of current platform
PCIE_DEVICE_MAP_TABLE                   *mPcieDeviceMap; //To record each Dev's info. under specific pcie bridge of current platform

EFI_OVERRIDE_ASPM_PROTOCOL              mOverrideAspmProtocol;
EFI_SMM_SYSTEM_TABLE2                   *mSmst;

/**
  A DXE driver for override the device ASPM setting.

  @param  ImageHandle    Handle for this drivers loaded image protocol.
  @param  SystemTable    EFI system table

  @retval EFI_SUCCESS    The driver installed without error.
**/
EFI_STATUS
EFIAPI
AspmOverrideEntryPoint (
  IN EFI_HANDLE           ImageHandle,
  IN EFI_SYSTEM_TABLE     *SystemTable
  )
{
  EFI_STATUS                       Status;
  EFI_HANDLE                       Handle;
  BOOLEAN                          InSmm;
  EFI_SMM_SW_DISPATCH2_PROTOCOL    *SwDispatch;
  EFI_SMM_SW_REGISTER_CONTEXT      SwContext;
  EFI_HANDLE                       SwHandle;
  EFI_EVENT                        ReadyToBootEvent;
  EFI_SMM_BASE2_PROTOCOL           *SmmBase;

  //
  // SMM check
  //
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    SmmBase->InSmm (SmmBase, &InSmm);
  } else {
    InSmm = FALSE;
  }

  if (!InSmm) {
    //
    // Regest event to override ASPM register before boot
    //
    Status = EfiCreateEventReadyToBootEx  (
             (TPL_CALLBACK - 1),
             AspmOverrideReadyToBootCallBack,
             NULL,
             &ReadyToBootEvent
             );
    ASSERT_EFI_ERROR (Status);
    if (EFI_ERROR(Status)) {
      return Status;
    }
  } else { //In SMM
    //
    // Get Smm Syatem Table
    //
    Status = SmmBase->GetSmstLocation(
                        SmmBase,
                        &mSmst
                        );
    if (EFI_ERROR (Status)) {
      return Status;
    }

    //
    // Install OverrideAspmProtocol to provide OverrideAspmFunction in SMM
    //
    mOverrideAspmProtocol.OverrideAspmFunc = OverrideAspmFunction;
    Handle = NULL;
    Status = mSmst->SmmInstallProtocolInterface (
                      &Handle,
                      &gEfiOverrideAspmProtocolGuid,
                      EFI_NATIVE_INTERFACE,
                      &mOverrideAspmProtocol
                      );
    if (EFI_ERROR (Status)) {
      return Status;
    } 

    // 
    // Install OverrideAspmProtocol for ECP SMM drivers. 
    // Should be removed when no more ECP.
    //
    EcpAspmOverrideSupported();

    //
    // Locate the SMM SW dispatch protocol to register a SMI callback function to execute ASPM override
    //
    Status = mSmst->SmmLocateProtocol (
                      &gEfiSmmSwDispatch2ProtocolGuid,
                      NULL,
                      (VOID **)&SwDispatch
                      );

    SwContext.SwSmiInputValue = SW_SMI_ASPM_OVERRIDE;
    Status = SwDispatch->Register (
                           SwDispatch,
                           SmmOverrideAspmFunction,
                           &SwContext,
                           &SwHandle
                           );
    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return Status;
}

/**
  A call back function to trigger a SMI to execute ASPM override when signal a ReadyToBoot event.

  @param  Event        Event instance (unused)
  @param  Context      Event Context (It is not used)

  @retval void
**/
VOID
EFIAPI
AspmOverrideReadyToBootCallBack (
  IN EFI_EVENT     Event,
  IN VOID          *Context
  )
{
  EFI_STATUS                   Status;
  EFI_SMM_CONTROL2_PROTOCOL    *SmmControl;
  UINT8                        SmiDataValue;

  Status = gBS->LocateProtocol (
                  &gEfiSmmControl2ProtocolGuid,
                  NULL,
                  (VOID **)&SmmControl
                  );

  if (EFI_ERROR (Status)) {
    return;
  }

  SmiDataValue = SW_SMI_ASPM_OVERRIDE;
  
  SmmControl->Trigger(
                    SmmControl,
                    &SmiDataValue,
                    NULL,
                    0,
                    0
                    );

  gBS->CloseEvent (Event);
 
}

/**
  A callback function to call OverrideAspmFunction() to execute ASPM setting modification.

  @param  DispatchHandle     Unused
  @param  Context            Unused
  @param  CommBuffer         Unused
  @param  CommBufferSize     Unused

  @retval EFI_SUCCESS        This function execute successfully
**/
EFI_STATUS
EFIAPI
SmmOverrideAspmFunction (
  IN EFI_HANDLE                  DispatchHandle,
  IN CONST VOID                  *Context,
  IN OUT VOID                    *CommBuffer,
  IN OUT UINTN                   *CommBufferSize
  )
{
  OverrideAspmFunction ();
  return EFI_SUCCESS;
}

/**
  This function get the project ASPM setting and collect PCIE bridge and device information to set the ASPM register.

  @retval EFI_SUCCESS        This function override ASPM setting successfully.
  @retval EFI_UNSUPPORTED    No any PCIE bridge be found, so it is unsupported.
**/
EFI_STATUS
OverrideAspmFunction (
  VOID
  )
{
  EFI_STATUS            Status;
  PCIE_ASPM_DEV_INFO    *PcieAspmDevs;
  
  PcieAspmDevs = PcdGetPtr(PcdPcieAspmDevs);
  Status = EFI_SUCCESS;

  //
  // OemServices
  //
  Status = OemSvcGetAspmOverrideInfo (
             &PcieAspmDevs
             );

  if (!EFI_ERROR (Status)) {
    return EFI_SUCCESS;
  }

  if (PcieAspmDevs[0].VendorId == PCIE_ASPM_DEV_END_OF_TABLE) {
    return EFI_SUCCESS;
  }
  
  //
  // Collect all pcie bridges and pcie devices info. on current platform.
  //
  if (!CreatPCIeBridgeDeviceMap ()) {
    //
    // Can't find any pcie bridge, don't need to executive overridden.
    //
    return EFI_NOT_FOUND;
  }

  Status = OverrideDevice (PcieAspmDevs);
  
  return Status;
}

/**
  A function to search the ASPM register location (offset) by using the capability register
  to trace and identify if the capability ID match 0x10.
  
  @param  Bus       The PCIE bus number of the searching device
  @param  Dev       The PCIE device number of the searching device
  @param  Fun       The PCIE function number of the searching device

  @retval 0         No PCIExpress capability register
  @retval Register  The PCIExpress capability register offset of given PCIE device
**/
UINT16
FindLinkCtrlOffset (
  IN UINT8     Bus,
  IN UINT8     Dev,
  IN UINT8     Fun
  )
{
  UINT16      Reg;
  UINT16      Cap;
  UINT16      Data16;

  Data16 = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Fun, PCI_CAPBILITY_POINTER_OFFSET));

  if (((Data16 & 0x00FF) == 0x00) || ((Data16  & 0x00FF) == 0xFF)) {
    return 0x00;
  } else {
    Data16 &= 0x00FF;
    Data16 <<= 8;
  }

  do {
    Reg = (Data16 >> 8);
    Data16 = PciExpressRead16 (PCI_EXPRESS_LIB_ADDRESS (Bus, Dev, Fun, Reg));
    Cap = (Data16 & 0x00FF);
  } while ((Cap != EFI_PCI_CAPABILITY_ID_PCIEXP) && ( (Data16 >> 8) != 0x00));

  if (Cap != EFI_PCI_CAPABILITY_ID_PCIEXP) {
    return 0x00;
  } else {
    return (Reg + LINK_CONTROL_REG);
  }
}

/**
  A function to override the ASPM register of searched PCIE bridge and device according OemServices setting.
  
  @param  PcieAspmDevs  A data pointer to a group data will be used to modify device ASPM setting

  @retval EFI_SUCCESS   To override ASPM setting is successful.
**/
EFI_STATUS
OverrideDevice (
  IN PCIE_ASPM_DEV_INFO     *PcieAspmDevs
  )
{
  EFI_STATUS     Status;
  UINT8          BusIndex;
  UINT8          DevIndex;
  UINT8          Index;
  UINT8          BridgeIndex;
  UINT8          RootDev;
  UINT8          RootFunc;
  UINT8          RootASPM;
  UINT8          DeviceBus;
  UINT8          DeviceDev;
  UINT8          DeviceASPM;
  UINT16         LinkCtrlOffset;
  UINT8          PcieDeviceIndex;
  UINT8          AspmMask = 0xFC;

  RootDev = 0;
  RootFunc = 0;
  Status = EFI_SUCCESS;

  for (BusIndex = 0; mPcieDeviceMap[BusIndex].Bus != 0xFF; BusIndex++) {
    for (DevIndex = 0; mPcieDeviceMap[BusIndex].PcieDev[DevIndex].Dev != 0xFF; DevIndex++) {
      Index = 0;
      while (PcieAspmDevs[Index].VendorId != PCIE_ASPM_DEV_END_OF_TABLE) {
        if ((PcieAspmDevs[Index].VendorId == mPcieDeviceMap[BusIndex].PcieDev[DevIndex].VenderId) && 
            (PcieAspmDevs[Index].DeviceId == mPcieDeviceMap[BusIndex].PcieDev[DevIndex].DeviceId) &&
            ((PcieAspmDevs[Index].RevId == 0xFF) ||
             (PcieAspmDevs[Index].RevId == mPcieDeviceMap[BusIndex].PcieDev[DevIndex].RevId))) {
             
          DeviceBus = mPcieDeviceMap[BusIndex].Bus;
          DeviceDev = mPcieDeviceMap[BusIndex].PcieDev[DevIndex].Dev;

          for (BridgeIndex = 0; mPcieBridgeMap[BridgeIndex].Dev != 0xFF; BridgeIndex++) {
            if (mPcieBridgeMap[BridgeIndex].SecondaryBusNum == DeviceBus){
              RootDev = mPcieBridgeMap[BridgeIndex].Dev;
              RootFunc = mPcieBridgeMap[BridgeIndex].Func;
              break;
            }
          }
          
          RootASPM = PcieAspmDevs[Index].RootAspm;
          DeviceASPM = PcieAspmDevs[Index].EndpointAspm;          
  
          //
          // Step01. Search Link Ctrl. if "not found" then "ignor"
          //
          LinkCtrlOffset = FindLinkCtrlOffset (0, RootDev, RootFunc);
          if ( LinkCtrlOffset == 0x00) {
            continue;
          }
  
          //
          // Step02. Override Link Ctrl of Bridge
          //
          PciExpressAndThenOr8 (
                    PCI_EXPRESS_LIB_ADDRESS (0, RootDev, RootFunc, LinkCtrlOffset),
                    AspmMask,
                    RootASPM
                    );
          
          //
          //Step03. Override Link Ctrl of devices under this bridge
          //
          for (PcieDeviceIndex = 0; mPcieDeviceMap[BusIndex].PcieDev[PcieDeviceIndex].Dev != 0xFF; PcieDeviceIndex++) {
           
            LinkCtrlOffset = FindLinkCtrlOffset (
                                          mPcieDeviceMap[BusIndex].Bus,
                                          mPcieDeviceMap[BusIndex].PcieDev[PcieDeviceIndex].Dev,
                                          mPcieDeviceMap[BusIndex].PcieDev[PcieDeviceIndex].Fnc
                                          );
            if ( LinkCtrlOffset == 0x00) {
              continue;
            }
        
            PciExpressAndThenOr8 (
                    PCI_EXPRESS_LIB_ADDRESS (
                      mPcieDeviceMap[BusIndex].Bus, 
                      mPcieDeviceMap[BusIndex].PcieDev[PcieDeviceIndex].Dev,
                      mPcieDeviceMap[BusIndex].PcieDev[PcieDeviceIndex].Fnc,            
                      LinkCtrlOffset),
                    AspmMask,
                    DeviceASPM
                    );
            
          }

          break;
        }
        Index++;
      }
    }
  }

  mSmst->SmmFreePool (mPcieBridgeMap);
  mSmst->SmmFreePool (mPcieDeviceMap);

  return Status;
}

/**
 A function to search and record all PCIE bridge, and scan all PCIE devices under these bridge.
  
  @param  VOID

  @retval TRUE   PCIE bridge exist.
  @retval FALSE  No PCIE bridge be found.
**/
BOOLEAN
CreatPCIeBridgeDeviceMap (
  VOID
  )
{
  BOOLEAN     BridgeExist;

  BridgeExist = FALSE;                  //Assume that all bridges are disabled
  if (SearchPCIeBridge ()) {             //Scan Pcie bridge
    SearchPCIeDevice ();                 //If "found pcie bridge" then "scan pcie devices under these bridge"
    BridgeExist = TRUE;                 //At least one bridge is enabled
  }

  return BridgeExist;
}

/**
 A function to search all PCIE bridge exist or not and record existent bridge in mPcieBridgeMap.
  
  @param  VOID

  @retval TRUE   PCIE bridge exist.
  @retval FALSE  No PCIE bridge be found.
**/
BOOLEAN
SearchPCIeBridge (
  VOID
  )
{
  EFI_STATUS     Status;
  UINT32         Data32;
  UINT32         Func32;
  UINT32         Temp32;
  UINT8          Data8;
  UINT8          BridgeIndex;
  UINT8          FncIndex;
  UINT8          ArrayIndex;
  BOOLEAN        BridgeExist;
  UINT8          Index;
  UINT32         Size;
  UINT8          TotalBridges;

  //
  // Step01. Scan PCIE Bridge. The result is put into Data32
  //
  //
  // Data32 BitMap
  // bit[02]:Dev2Bridge, bit[03]:Dev3Bridge, bit[04]:Dev4Bridge, bit[05]:Dev5Bridge....and so on.
  // 1: Bridge enabled, 0: Bridge disabled   
  //
  Data32 = 0x00000000;
  Func32 = 0x00000000;
  TotalBridges = 0;
  for (Index = 0x00; Index <= PCI_MAX_DEVICE; Index++) {          //0x1f: Max Pcie bridge count on BUS0
    Temp32 = PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS (0, Index, 0, PCI_VENDOR_ID_OFFSET));
    if (Temp32 == 0xFFFFFFFF) {
      continue;
    }
    
    //
    // Check Multi-Function
    //
    Data8 = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (0, Index, 0, PCI_HEADER_TYPE_OFFSET));
    if ((Data8 & HEADER_TYPE_MULTI_FUNCTION) != 0x00) {
      //
      // Check for each PCI function
      //
      for (FncIndex = 0; FncIndex <= 0x7; FncIndex++) {
        Temp32 = PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS (0, Index, FncIndex, PCI_REVISION_ID_OFFSET));
    
        if ((Temp32 & 0xFFFFFF00) == 0x06040000 || (Temp32 & 0xFFFFFF00) == 0x06040100) {
          //
          // Class code: 060400 & 060401  => PCI to PCI bridge
          //
          if (FindLinkCtrlOffset (0, Index, FncIndex) != 0x00) {     //if FindLinkCtrlOffset != 0x00 => Belong to PCIE
            Data32 |= (1 << Index);
            Func32 |= (1 << Index);
            TotalBridges++;
          }
        }        
      }
    } else {
      Temp32 = PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS (0, Index, 0, PCI_REVISION_ID_OFFSET));
  
      if ((Temp32 & 0xFFFFFF00) == 0x06040000 || (Temp32 & 0xFFFFFF00) == 0x06040100) {
        //
        // Class code: 060400 & 060401  => PCI to PCI bridge
        //
        if (FindLinkCtrlOffset (0, Index, 0x00) != 0x00) {     //if FindLinkCtrlOffset != 0x00 => Belong to PCIE
          Data32 |= (1 << Index);
          TotalBridges++;
        }
      }
    }

  }

  BridgeExist = FALSE;
  if (TotalBridges == 0) {
    return BridgeExist;
  } else {  
    BridgeExist = TRUE;
  }  

  //
  // Step02. Initialize mPcieBridgeMap and mPcieDeviceMap structs
  //
  // since bridge exist, allocate space for the mPcieBridgeMap to record each bridge's info.
  //
  Size = (TotalBridges + 1) * sizeof (PCIE_BRIDGE_MAP_TABLE);
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    Size,
                    (VOID **)&mPcieBridgeMap
                    );                       

  if (EFI_ERROR (Status)) {
    return FALSE;
  }

  //
  // since bridge exist, allocate space for the mPcieDeviceMap to record each Dev's info under each pcie bridge.
  //
  Size = (TotalBridges + 1) * sizeof (PCIE_DEVICE_MAP_TABLE);
  Status = mSmst->SmmAllocatePool (
                    EfiRuntimeServicesData,
                    Size,
                    (VOID **)&mPcieDeviceMap
                    );   
                      
  if (EFI_ERROR (Status)) {
    mSmst->SmmFreePool (mPcieBridgeMap);
    return FALSE;
  }

  //
  //Step03. Put the bus info. of each pcie bridge into the mPcieBridgeMap
  //
  ArrayIndex = 0x00;
  for (BridgeIndex = 0; BridgeIndex <= PCI_MAX_DEVICE; BridgeIndex++) {
    if (((Data32 >> BridgeIndex) & 0x01) == 0x01) {  //Bridge is enabled
      if (((Func32 >> BridgeIndex) & 0x01) == 0x01) {  //Multi-Function
        for (FncIndex = 0; FncIndex <= PCI_MAX_FUNC; FncIndex++) {
          Temp32 = PciExpressRead32 (PCI_EXPRESS_LIB_ADDRESS (0, BridgeIndex, FncIndex, PCI_VENDOR_ID_OFFSET));

          if (Temp32 == 0xFFFFFFFF)
            continue;

          Data8 = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (0, BridgeIndex, FncIndex, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));
          mPcieBridgeMap[ArrayIndex].Dev             = BridgeIndex;
          mPcieBridgeMap[ArrayIndex].Func            = FncIndex;
          mPcieBridgeMap[ArrayIndex].SecondaryBusNum = Data8;
          ArrayIndex++;
        }
      } else {
        Data8 = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (0, BridgeIndex, 0, PCI_BRIDGE_SECONDARY_BUS_REGISTER_OFFSET));
        mPcieBridgeMap[ArrayIndex].Dev             = BridgeIndex;
        mPcieBridgeMap[ArrayIndex].Func            = 0x00;
        mPcieBridgeMap[ArrayIndex].SecondaryBusNum = Data8;
        ArrayIndex++;
      }
    }
  }

  mPcieBridgeMap[ArrayIndex].Dev             = 0xFF;
  mPcieBridgeMap[ArrayIndex].Func            = 0xFF;
  mPcieBridgeMap[ArrayIndex].SecondaryBusNum = 0xFF;

  return BridgeExist;
}

/**
A function to search all PCIE device exist or not under mPcieBridgeMap and record existent device in mPcieDeviceMap..
  
  @param  VOID

  @retval VOID
**/
VOID
SearchPCIeDevice (
  VOID
  )
{
  UINT8      ArrayIndex;
  UINT32     PcieDeviceIndex;
  UINT8      DevIndex;
  UINT8      FncIndex;
  UINT32     Data32;
  UINT32     Data8;
  UINT16     VenderId, DeviceId;
  UINT8      RevId;

  //
  //Step01. Scan Pcie devices by mPcieBridgeMap specified.
  //        And then put the result into mPcieDeviceMap.
  ArrayIndex = 0x00;
  while (mPcieBridgeMap[ArrayIndex].Dev != 0xFF) {
    mPcieDeviceMap[ArrayIndex].Bus = mPcieBridgeMap[ArrayIndex].SecondaryBusNum;
    PcieDeviceIndex = 0x0000;
    for (DevIndex= 0; DevIndex <= PCI_MAX_DEVICE; DevIndex++) {
      VenderId = 0x0000;
      DeviceId = 0x0000;
      Data32 = PciExpressRead32 (
                 PCI_EXPRESS_LIB_ADDRESS (
                     mPcieBridgeMap[ArrayIndex].SecondaryBusNum, 
                     DevIndex, 
                     0, 
                     PCI_VENDOR_ID_OFFSET
                     )
                 );

      if (Data32 == 0xFFFFFFFF) {
        continue;
      }
      Data8 = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (mPcieBridgeMap[ArrayIndex].SecondaryBusNum, DevIndex, 0, PCI_HEADER_TYPE_OFFSET));
      
      if ( (Data8 & HEADER_TYPE_MULTI_FUNCTION) != 0x00) { //Multi function
        for (FncIndex = 0; FncIndex <= PCI_MAX_FUNC; FncIndex++) {
          
          Data32 = PciExpressRead32 (                 
                     PCI_EXPRESS_LIB_ADDRESS (
                       mPcieBridgeMap[ArrayIndex].SecondaryBusNum, 
                       DevIndex, 
                       FncIndex, 
                       PCI_VENDOR_ID_OFFSET
                       )
                   );

          if (Data32 == 0xFFFFFFFF) {
            continue;
          }
          
          VenderId = (UINT16)Data32;
          DeviceId = (UINT16)(Data32 >> 16);

          RevId = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (mPcieBridgeMap[ArrayIndex].SecondaryBusNum, DevIndex, FncIndex, PCI_REVISION_ID_OFFSET));

          mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].Dev      = DevIndex;
          mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].Fnc      = FncIndex;
          mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].VenderId = VenderId;
          mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].DeviceId = DeviceId;
          mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].RevId    = RevId;
          PcieDeviceIndex++;
        }
      } else {
        
        FncIndex = 0x00;
        VenderId = (UINT16)Data32;
        DeviceId = (UINT16)(Data32 >> 16);

        RevId = PciExpressRead8 (PCI_EXPRESS_LIB_ADDRESS (mPcieBridgeMap[ArrayIndex].SecondaryBusNum, DevIndex, 0x00, PCI_REVISION_ID_OFFSET));

        mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].Dev      = DevIndex;
        mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].Fnc      = FncIndex;
        mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].VenderId = VenderId;
        mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].DeviceId = DeviceId;
        mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].RevId    = RevId;
        PcieDeviceIndex++;
      }
    }
    mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].Dev      = 0xFF;
    mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].Fnc      = 0xFF;
    mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].VenderId = 0xFFFF;
    mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].DeviceId = 0xFFFF;
    mPcieDeviceMap[ArrayIndex].PcieDev[PcieDeviceIndex].RevId    = 0xFF;
    ArrayIndex++;
  }

  mPcieDeviceMap[ArrayIndex].Bus = 0xFF;

  return;
}

/**
 Support the Ecp driver. 

  @param  PrivateData    The pointer to SMM_OEM_SERVICES_INSTANCE.

  @retval EFI_SUCCESS       Install SmmOemServices Protocol on gBS and SmmRuntimeProtocol successfully.
  @retval other             Some error occurred when installing SmmOemServices Protocol.

**/
EFI_STATUS
EcpAspmOverrideSupported (
  VOID
  )
{
  EFI_STATUS                            Status;
  EFI_HANDLE                            Handle;
  EFI_SMM_RUNTIME_PROTOCOL              *SmmRT;

  //
  // Install SmmRuntimeProtocol.
  //
  Status = gBS->LocateProtocol (&gEfiSmmRuntimeProtocolGuid, NULL, (VOID **)&SmmRT);
  if (EFI_ERROR(Status)) {
    return Status;
  }
  
  Handle = NULL;
  SmmRT->InstallProtocolInterface (
               &Handle,
               &gEfiOverrideAspmProtocolGuid,
               EFI_NATIVE_INTERFACE,
               &mOverrideAspmProtocol
               );

  return Status;
}