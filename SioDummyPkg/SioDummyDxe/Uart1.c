/** @file

Device Protocol

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SioInitDxe.h"

static EFI_ISA_DEVICE_RESOURCE *DeviceResource;
static UINT32 Uid;  // Device Instance

/**
  brief-description of function. 

  extended description of function.  
  
**/
static EFI_STATUS
SetDevicePower (
  IN BOOLEAN  OnOff
  )   
{
  return EFI_UNSUPPORTED;
}

/**
  Get current resource from resource list.  

  @param  ResourceList[out]       provide device IO/IRQ/DMA resource list table.

  @retval EFI_SUCCESS             Function complete successfully. 
  
**/
static EFI_STATUS
GetCurrentResource (
  OUT EFI_ISA_DEVICE_RESOURCE  **ResourceList
  )  
{
	*ResourceList = DeviceResource;

  return EFI_SUCCESS;
}

/**
  Get possible resource from resource list. 

  @param  ResourceList[out]       provide device IO/IRQ/DMA resource list table.

  @retval EFI_SUCCESS             Function complete successfully. 
  
**/
static EFI_STATUS
GetPossibleResource(
  OUT EFI_ISA_DEVICE_RESOURCE  **ResourceList,
  OUT UINT32                   *Hid
  )    
{
  *ResourceList = DeviceResource;
  *Hid = Uid;
  return EFI_SUCCESS;
}

/**
  Get resource from resource list table and set resource.

  @param  ResourceList[out]       provide device IO/IRQ/DMA resource list table.

  @retval EFI_SUCCESS             Function complete successfully. 
  
**/
static EFI_STATUS
SetResource (
  IN EFI_ISA_DEVICE_RESOURCE  *ResourceList
  )
{
  return EFI_SUCCESS;
}

/**
  Set CR02.Bit3 to active device.

  @param  Enable[in]              enable or disable.

  @retval EFI_SUCCESS             Function complete successfully. 
  
**/
static EFI_STATUS
EnableDevice (
  IN BOOLEAN  Enable
  )   
{
  return EFI_SUCCESS;
}

static EFI_ISA_ACPI_DEVICE_FUNCTION mIsaFunction = {
  SetDevicePower,
  GetPossibleResource,
  SetResource,
  EnableDevice,
  GetCurrentResource,
};

/**
  brief-description of function. 

  extended description of function.  
  
**/
EFI_STATUS
InstallCom1Protocol ( 
  IN SIO_DEVICE_LIST_TABLE* DeviceList
  )
{
  EFI_STATUS                Status = EFI_SUCCESS;
  EFI_HANDLE                DeviceHandle = NULL;

  //
  // Register ID for this device only
  //
  Uid = DeviceList->DeviceInstance;

  //
  // DeviceList resource transfer to IsaDevice resource
  //
  DeviceResource = AllocatePool(sizeof(EFI_ISA_DEVICE_RESOURCE));

//[-start-140102-IB12970048-add]//
  if (DeviceResource == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-140102-IB12970048-add]//
  
  DeviceListToDeviceResource(DeviceList, DeviceResource);

  Status = gBS->InstallProtocolInterface (
                  &DeviceHandle,
                  &gEfiIsaPnp501DeviceProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  &mIsaFunction
                  );

  return Status;
}
