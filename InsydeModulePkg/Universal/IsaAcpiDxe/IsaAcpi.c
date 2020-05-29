/** @file

Create resources from sio device for IsaBus table

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "IsaAcpiDriver.h"

typedef struct {
  EFI_SIO_RESOURCE_FUNCTION  *FunctionPtr;
} EFI_SIO_RESOURCE_FUNCTION_PTR;

typedef struct {
  EFI_ISA_ACPI_DEVICE_FUNCTION  *FunctionPtr;
} EFI_ISA_ACPI_DEVICE_FUNCTION_PTR;

UINTN NumSio = 0;
EFI_SIO_RESOURCE_FUNCTION_PTR *SioResourceFunction = NULL;

UINTN NumPnp303 = 0;
EFI_ISA_ACPI_DEVICE_FUNCTION_PTR *Ps2Protocol = NULL;

UINTN NumPnpF03 = 0;
EFI_ISA_ACPI_DEVICE_FUNCTION_PTR *Ps2Protocol2 = NULL;

UINTN NumPnp401 = 0;
EFI_ISA_ACPI_DEVICE_FUNCTION_PTR *LptProtocol = NULL;

UINTN NumPnp501 = 0;
EFI_ISA_ACPI_DEVICE_FUNCTION_PTR *ComProtocol = NULL;

UINTN NumPnp510 = 0;
EFI_ISA_ACPI_DEVICE_FUNCTION_PTR *IrProtocol = NULL;

UINTN NumPnp604 = 0;
EFI_ISA_ACPI_DEVICE_FUNCTION_PTR *FdcProtocol = NULL;

UINTN NumPnpB02f = 0;
EFI_ISA_ACPI_DEVICE_FUNCTION_PTR *GameProtocol = NULL;
 
EFI_ISA_ACPI_RESOURCE_LIST  *mDeviceList = NULL;
EFI_ISA_ACPI_RESOURCE_LIST *gIsaAcpiDeviceList = NULL;
extern UINTN NumPnp200;  // DMA

BOOLEAN  mDMAInitial = FALSE;

typedef struct{
  EFI_ISA_ACPI_RESOURCE IsaIo;
  EFI_ISA_ACPI_RESOURCE IsaIrq;
  EFI_ISA_ACPI_RESOURCE IsaNull;
} ISA_ACPI_IO_IRQ_RESOURCE;

typedef struct{
  EFI_ISA_ACPI_RESOURCE IsaIo;
  EFI_ISA_ACPI_RESOURCE IsaIrq;
  EFI_ISA_ACPI_RESOURCE IsaDma;
  EFI_ISA_ACPI_RESOURCE IsaNull;
} ISA_ACPI_IO_IRQ_DMA_RESOURCE;

/**
  Init DMA controller
**/
VOID
DmaControllerInit (
  VOID
  );

/**
  The entry point of the Lpc driver.  

  @param[in]       DeviceGuid         Deivce Guid.
  @param[out]      NumberOfHandles    Total number we get.
  @param[out]      ProtocolBuffer     Data buffer.  

  @retval          EFI_SUCCESS        Function complete successfully. 
**/
EFI_STATUS
GetDeviceList (
  IN  EFI_GUID   *DeviceGuid, 
  OUT UINTN      *NumberOfHandles, 
  OUT VOID       **ProtocolBuffer
  )
{
  UINT8                           Index;
  UINTN                           SizeOfHandlesBuffer;
  EFI_STATUS                      Status;
  EFI_HANDLE                      *HandleBuffer;
  UINTN                           *TempBuffer;

  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  DeviceGuid,
                  NULL,
                  NumberOfHandles,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  SizeOfHandlesBuffer = *NumberOfHandles * sizeof(VOID*);
  Status = gBS->AllocatePool(EfiBootServicesData, SizeOfHandlesBuffer , ProtocolBuffer);
  if (Status != EFI_SUCCESS)
  {
    return Status;
  }

  TempBuffer = *ProtocolBuffer;
  
  for (Index = 0; Index < *NumberOfHandles; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    DeviceGuid,
                    (VOID *) &TempBuffer[Index]
                    );

    if (EFI_ERROR (Status)) {
      continue;
    }
  }
  gBS->FreePool (HandleBuffer);

  return EFI_SUCCESS;
}

/**
  Set SIO flag  

  @param[in]  OnOff       Select on or off to control flag.
**/
VOID
SioSetScriptFlag (
  IN BOOLEAN                           OnOff
  )
{
  UINTN Index;
  for (Index = 0; Index<NumSio; Index++)
  {
    SioResourceFunction[Index].FunctionPtr->ScriptFlag = OnOff;
  }
  return;
}

/**
  Search next resources of the IsaAcpi.  

  @param[in]       Device              Device ID.
  @param[out]      IsaAcpiDevice       Pointer IsaAcpi device.
  @param[out]      NextIsaAcpiDevice   Point next IsaAcpi device.
**/
VOID
IsaDeviceLookup (
  IN  EFI_ISA_ACPI_DEVICE_ID      *Device,
  OUT EFI_ISA_ACPI_RESOURCE_LIST  **IsaAcpiDevice,
  OUT EFI_ISA_ACPI_RESOURCE_LIST  **NextIsaAcpiDevice
  )
{
  UINTN  Index;

  *IsaAcpiDevice = NULL;
  if (NextIsaAcpiDevice != NULL) {
    *NextIsaAcpiDevice = NULL;
  }
  if (Device == NULL) {
    Index = 0;
  } else {
    for (Index = 0; gIsaAcpiDeviceList[Index].Device.HID != 0; Index++) {
      if (Device->HID == gIsaAcpiDeviceList[Index].Device.HID &&
          Device->UID == gIsaAcpiDeviceList[Index].Device.UID    ) {
        break;
      }
    }
    if (gIsaAcpiDeviceList[Index].Device.HID == 0) {
      return;
    }
    *IsaAcpiDevice = &(gIsaAcpiDeviceList[Index]);
    Index++;
  }
  if (NextIsaAcpiDevice != NULL && gIsaAcpiDeviceList[Index].Device.HID != 0) {
    *NextIsaAcpiDevice = &(gIsaAcpiDeviceList[Index]);
  }
}

/**
  Search resources of the IsaAcpi.  

  @param[in]       This            This IsaAcpi protocol.
  @param[out]      Device          Pointer IsaAcpi device.

  @retval          EFI_SUCCESS     Function complete successfully. 
**/
EFI_STATUS
EFIAPI
IsaDeviceEnumerate (
  IN     EFI_ISA_ACPI_PROTOCOL       *This,
  OUT    EFI_ISA_ACPI_DEVICE_ID      **Device
  )
{
  EFI_ISA_ACPI_RESOURCE_LIST  *IsaAcpiDevice;
  EFI_ISA_ACPI_RESOURCE_LIST  *NextIsaAcpiDevice;

  IsaDeviceLookup ( *Device, &IsaAcpiDevice, &NextIsaAcpiDevice);
  if (NextIsaAcpiDevice == NULL) {
    return EFI_NOT_FOUND;
  }
  *Device = &(NextIsaAcpiDevice->Device);

  return EFI_SUCCESS;
}

/**
  Connect to device protocol to control power on or off.  

  @param[in]   This            This IsaAcpi protocol.
  @param[in]   Device          Pointer IsaAcpi device.
  @param[in]   OnOff           Pointer IsaAcpi device.

  @retval      EFI_SUCCESS     Function complete successfully. 
**/
EFI_STATUS
EFIAPI
IsaDeviceSetPower (
  IN     EFI_ISA_ACPI_PROTOCOL       *This,
  IN     EFI_ISA_ACPI_DEVICE_ID      *Device,
  IN     BOOLEAN                     OnOff
  )
{
  SioSetScriptFlag (TRUE);
  switch (Device->HID) {
    
  case EISA_PNP_ID(0x303):
    if(Device->UID<NumPnp303)
    {
       Ps2Protocol[Device->UID].FunctionPtr->SetPower(OnOff);
    }
    break;

  case EISA_PNP_ID(0x401):
    if (Device->UID < NumPnp401) {
      LptProtocol[Device->UID].FunctionPtr->SetPower(OnOff);
    }
    break;
    
  case EISA_PNP_ID(0x501):
    if (Device->UID < NumPnp501) {
      ComProtocol[Device->UID].FunctionPtr->SetPower(OnOff);
    }
    break;

  case EISA_PNP_ID(0x510):
    if (Device->UID < NumPnp510) {
      IrProtocol[Device->UID].FunctionPtr->SetPower(OnOff);
    }
    break;
    
  case EISA_PNP_ID(0x604):
    if (Device->UID < NumPnp604) {
      FdcProtocol[Device->UID].FunctionPtr->SetPower(OnOff);
    }
    break;

  case EISA_PNP_ID(0xB02F):
    if (Device->UID < NumPnpB02f) {
      GameProtocol[Device->UID].FunctionPtr->SetPower(OnOff);
    }
    break; 

  case EISA_PNP_ID(0xF03):
    if(Device->UID<NumPnpF03)
    {
       Ps2Protocol2[Device->UID].FunctionPtr->SetPower(OnOff);
    }
    break;     
  }
  SioSetScriptFlag (FALSE);
  return EFI_UNSUPPORTED;
}

/**
  Connect to device protocol to get current resource.  

  @param[in]   This            This IsaAcpi protocol.
  @param[in]   Device          Device ID.
  @param[out]  ResourceList    Pointer IsaAcpi resources.

  @retval      EFI_SUCCESS     Function complete successfully. 
**/
EFI_STATUS
EFIAPI
IsaGetCurrentResource (
  IN     EFI_ISA_ACPI_PROTOCOL        *This,
  IN     EFI_ISA_ACPI_DEVICE_ID       *Device,
  OUT    EFI_ISA_ACPI_RESOURCE_LIST   **ResourceList
  )
{
  IsaDeviceLookup ( Device, ResourceList, NULL);

  if (*ResourceList == NULL) {
    return EFI_NOT_FOUND;
  }

  return EFI_SUCCESS;
}

/**
  Connect to device protocol to get porssible resource.  

  @param[in]   This             This IsaAcpi protocol.
  @param[in]   Device           Device ID.
  @param[out]  ResourceList     Pointer IsaAcpi resources.

  @retval      EFI_SUCCESS      Function complete successfully. 
  @retval      EFI_UNSUPPORTED  Function not supported yet. 
**/
EFI_STATUS
EFIAPI
IsaGetPossibleResource (
  IN     EFI_ISA_ACPI_PROTOCOL       *This,
  IN     EFI_ISA_ACPI_DEVICE_ID      *Device,
  OUT    EFI_ISA_ACPI_RESOURCE_LIST  **ResourceList
  )
{
  //
  // Not supported yet
  //
  return EFI_UNSUPPORTED;
}

/**
  Connect to device protocol to set resources.  

  @param[in]   This             This IsaAcpi protocol.
  @param[in]   Device           Device ID.
  @param[in]   ResourceList     Pointer IsaAcpi resources.

  @retval      EFI_SUCCESS      Function complete successfully. 
**/
EFI_STATUS
EFIAPI
IsaSetResource (
  IN     EFI_ISA_ACPI_PROTOCOL       *This,
  IN     EFI_ISA_ACPI_DEVICE_ID      *Device,
  IN     EFI_ISA_ACPI_RESOURCE_LIST  *ResourceList
  )
{
  EFI_STATUS Status = EFI_UNSUPPORTED;
  EFI_ISA_DEVICE_RESOURCE  IsaDeviceResource;
  IsaDeviceResource.IoPort = (UINT16)ResourceList->ResourceItem[0].StartRange;
  IsaDeviceResource.IrqNumber =(UINT8)ResourceList->ResourceItem[1].StartRange;
  IsaDeviceResource.DmaChannel =(UINT8)ResourceList->ResourceItem[2].StartRange;

  SioSetScriptFlag (TRUE);
  switch (Device->HID) {
  
  case EISA_PNP_ID (0x303):
    if (Device->UID < NumPnp303) {
      Status = Ps2Protocol[Device->UID].FunctionPtr->SetResource (&IsaDeviceResource);
    }
    break;

  case EISA_PNP_ID (0x401):
    if (Device->UID < NumPnp401) {
      Status = LptProtocol[Device->UID].FunctionPtr->SetResource (&IsaDeviceResource);
    }
    break;
    
  case EISA_PNP_ID (0x501):
    if (Device->UID < NumPnp501) {
      Status = ComProtocol[Device->UID].FunctionPtr->SetResource (&IsaDeviceResource);
    }
    break;

  case EISA_PNP_ID (0x510):
    if (Device->UID < NumPnp510) {
      Status = IrProtocol[Device->UID].FunctionPtr->SetResource (&IsaDeviceResource);
    }
    break;
    
  case EISA_PNP_ID (0x604):
    if (Device->UID < NumPnp604) {
      Status = FdcProtocol[Device->UID].FunctionPtr->SetResource(&IsaDeviceResource);
    }
    break; 

  case EISA_PNP_ID (0xB02F):
    if (Device->UID < NumPnpB02f) {
      Status = GameProtocol[Device->UID].FunctionPtr->SetResource(&IsaDeviceResource);
    }
    break;   

  case EISA_PNP_ID (0xF03):
    if (Device->UID < NumPnpF03) {
      Status = Ps2Protocol2[Device->UID].FunctionPtr->SetResource (&IsaDeviceResource);
    }
    break;  
  }
  SioSetScriptFlag (FALSE);
  return EFI_SUCCESS;
}

/**
  Connect to device protocol to set enable or disable.  

  @param[in]   This             This IsaAcpi protocol.
  @param[in]   Device           Device ID.
  @param[in]   Enable           Enable or disable.

  @retval      EFI_SUCCESS      Function complete successfully. 
**/
EFI_STATUS
EFIAPI
IsaEnableDevice (
  IN    EFI_ISA_ACPI_PROTOCOL        *This,
  IN    EFI_ISA_ACPI_DEVICE_ID       *Device,
  IN    BOOLEAN                      Enable
  )
{
  SioSetScriptFlag (TRUE);
  switch (Device->HID) {
  
  case EISA_PNP_ID (0x303):
    if (Device->UID < NumPnp303) {
      Ps2Protocol[Device->UID].FunctionPtr->EnableDevice (Enable);
    }
    break;

  case EISA_PNP_ID (0x401):
    if (Device->UID < NumPnp401) {
      LptProtocol[Device->UID].FunctionPtr->EnableDevice (Enable);
    }
    break;
    
  case EISA_PNP_ID (0x501):
    if (Device->UID < NumPnp501) {
      ComProtocol[Device->UID].FunctionPtr->EnableDevice (Enable);
    }
    break;

  case EISA_PNP_ID (0x510):
    if (Device->UID < NumPnp510) {
      IrProtocol[Device->UID].FunctionPtr->EnableDevice (Enable);
    }
    break;
  
  case EISA_PNP_ID (0x604):
    if (Device->UID < NumPnp604) {
      FdcProtocol[Device->UID].FunctionPtr->EnableDevice (Enable);
    }
    break; 

  case EISA_PNP_ID (0xB02F):
    if (Device->UID < NumPnpB02f) {
      GameProtocol[Device->UID].FunctionPtr->EnableDevice (Enable);
    }
    break;     

  case EISA_PNP_ID (0xF03):
    if (Device->UID < NumPnpF03) {
      Ps2Protocol2[Device->UID].FunctionPtr->EnableDevice (Enable);
    }
    break;    
  }
  SioSetScriptFlag (FALSE);
  return EFI_SUCCESS;
}

/**
  Init DMA controller.  

  @param[in]   This             This IsaAcpi protocol.
  @param[in]   Device           Device ID.

  @retval      EFI_SUCCESS      Function complete successfully. 
**/
EFI_STATUS
EFIAPI
IsaInitDevice (
  IN    EFI_ISA_ACPI_PROTOCOL        *This,
  IN    EFI_ISA_ACPI_DEVICE_ID       *Device
  )
{
  if ((Device->HID == EISA_PNP_ID (0x604) || Device->HID == EISA_PNP_ID (0x401)) && !mDMAInitial) {
    //
    // Initial DMA for FDC or LPT if the device exists
    //
    DmaControllerInit();
    mDMAInitial = TRUE;
  }

  return EFI_UNSUPPORTED;
}

/**
  Get resource from SIO devices, then create table that IsaBus need  

  @param[in]   This             This IsaAcpi protocol.

  @retval      EFI_SUCCESS      Function complete successfully. 
**/
EFI_STATUS
EFIAPI
LpcInterfaceInit (
  IN    EFI_ISA_ACPI_PROTOCOL        *This
)
{
  EFI_STATUS                    Status = EFI_SUCCESS;
  UINTN                         TotalDeviceNumber = 0;
  ISA_ACPI_IO_IRQ_RESOURCE      *Pnp303 = NULL;
  ISA_ACPI_IO_IRQ_RESOURCE      *PnpF03 = NULL;
  ISA_ACPI_IO_IRQ_RESOURCE      *Pnp401 = NULL;
  ISA_ACPI_IO_IRQ_RESOURCE      *Pnp501 = NULL; 
  ISA_ACPI_IO_IRQ_RESOURCE      *Pnp510 = NULL;
  ISA_ACPI_IO_IRQ_RESOURCE      *PnpB02f = NULL;  
  ISA_ACPI_IO_IRQ_DMA_RESOURCE  *Pnp604 = NULL;
  EFI_ISA_DEVICE_RESOURCE       *IsaDeviceResource = NULL;
  UINT8                         DeviceIndex = 0;
  UINT8                         Index = 0;
  UINT32                        Uid;
 
  //
  //Only create Table in IsaAcpiDriver.c Stage
  // 
  if (This == NULL) {
    //
    // Get all PNP device resource
    //
    GetDeviceList ( &gEfiIsaPnp303DeviceProtocolGuid, &NumPnp303, (VOID **)&Ps2Protocol);
    Status = gBS->AllocatePool(EfiBootServicesData, NumPnp303* sizeof (ISA_ACPI_IO_IRQ_RESOURCE) , (VOID **)&Pnp303);

    GetDeviceList ( &gEfiIsaPnp401DeviceProtocolGuid, &NumPnp401, (VOID **)&LptProtocol );
    Status = gBS->AllocatePool(EfiBootServicesData, NumPnp401*sizeof (ISA_ACPI_IO_IRQ_RESOURCE) , (VOID **)&Pnp401); 

    GetDeviceList ( &gEfiIsaPnp501DeviceProtocolGuid, &NumPnp501, (VOID **)&ComProtocol);  
    Status = gBS->AllocatePool(EfiBootServicesData, NumPnp501 * sizeof (ISA_ACPI_IO_IRQ_RESOURCE) , (VOID **)&Pnp501);

    GetDeviceList ( &gEfiIsaPnp510DeviceProtocolGuid, &NumPnp510, (VOID **)&IrProtocol);  
    Status = gBS->AllocatePool(EfiBootServicesData, NumPnp510 * sizeof (ISA_ACPI_IO_IRQ_RESOURCE) , (VOID **)&Pnp510);

    GetDeviceList ( &gEfiIsaPnp604DeviceProtocolGuid, &NumPnp604, (VOID **)&FdcProtocol);
    Status = gBS->AllocatePool(EfiBootServicesData, NumPnp604*sizeof (ISA_ACPI_IO_IRQ_DMA_RESOURCE) ,(VOID **) &Pnp604); 

    GetDeviceList ( &gEfiIsaPnpB02fDeviceProtocolGuid, &NumPnpB02f, (VOID **)&GameProtocol);
    Status = gBS->AllocatePool(EfiBootServicesData, NumPnpB02f*sizeof (ISA_ACPI_IO_IRQ_RESOURCE) , (VOID **)&PnpB02f);     

    GetDeviceList ( &gEfiIsaPnpF03DeviceProtocolGuid, &NumPnpF03, (VOID **)&Ps2Protocol2);
    Status = gBS->AllocatePool(EfiBootServicesData, NumPnpF03* sizeof (ISA_ACPI_IO_IRQ_RESOURCE) , (VOID **)&PnpF03);  

    TotalDeviceNumber = NumPnp303 + NumPnp401 + NumPnp501 + NumPnp510 + NumPnp604 + NumPnpB02f + NumPnpF03 + NumPnp200 + 1; // the last 1 is for NULL list 
    Status = gBS->AllocatePool (EfiBootServicesData, TotalDeviceNumber*sizeof (EFI_ISA_ACPI_RESOURCE_LIST) , (VOID **)&gIsaAcpiDeviceList);
  
    //
    // Update ps2 controler resource
    //
    if (NumPnp303) {
      for (DeviceIndex = 0; DeviceIndex < NumPnp303; DeviceIndex++) {
        Uid = 0;
        Ps2Protocol[DeviceIndex].FunctionPtr->GetPossibleResource(&IsaDeviceResource, &Uid);
        if (IsaDeviceResource->DmaChannel != NULL_ID) {        
          //
          // Auto create the Pnp303 Resource table.
          //
          Pnp303[DeviceIndex].IsaIo.Type = EfiIsaAcpiResourceIo;
          Pnp303[DeviceIndex].IsaIo.Attribute = 0;
          Pnp303[DeviceIndex].IsaIo.StartRange = IsaDeviceResource->IoPort;
          Pnp303[DeviceIndex].IsaIo.EndRange = IsaDeviceResource->IoPort+4;
  
          Pnp303[DeviceIndex].IsaIrq.Type = EfiIsaAcpiResourceInterrupt;
          Pnp303[DeviceIndex].IsaIrq.Attribute = 0;
          Pnp303[DeviceIndex].IsaIrq.StartRange = IsaDeviceResource->IrqNumber;
          Pnp303[DeviceIndex].IsaIrq.EndRange = 0;
         
          Pnp303[DeviceIndex].IsaNull.Type = EfiIsaAcpiResourceEndOfList;
          Pnp303[DeviceIndex].IsaNull.Attribute = 0;
          Pnp303[DeviceIndex].IsaNull.StartRange = 0;
          Pnp303[DeviceIndex].IsaNull.EndRange = 0; 
  
          //
          // Send HID,UID,ISA_ACPI_IO_IRQ_RESOURCE to EFI_ISA_ACPI_RESOURCE_LIST
          //
          gIsaAcpiDeviceList[Index].Device.HID = EISA_PNP_ID(0x303);
          gIsaAcpiDeviceList[Index].Device.UID = DeviceIndex;
          gIsaAcpiDeviceList[Index].ResourceItem = &Pnp303[DeviceIndex].IsaIo;
          Index++;
        }          
      }
    }

    //
    // Update Lpt controler resource
    //
    if (NumPnp401) {
      for (DeviceIndex = 0; DeviceIndex < NumPnp401; DeviceIndex++) {
        Uid = 0;
        LptProtocol[DeviceIndex].FunctionPtr->GetPossibleResource(&IsaDeviceResource, &Uid);
        if (IsaDeviceResource->DmaChannel != NULL_ID) {        
          //
          // Auto create the Pnp401 Resource table.
          //
          Pnp401[DeviceIndex].IsaIo.Type = EfiIsaAcpiResourceIo;
          Pnp401[DeviceIndex].IsaIo.Attribute = 0;
          Pnp401[DeviceIndex].IsaIo.StartRange = IsaDeviceResource->IoPort;
          Pnp401[DeviceIndex].IsaIo.EndRange = IsaDeviceResource->IoPort|7;
  
          Pnp401[DeviceIndex].IsaIrq.Type = EfiIsaAcpiResourceInterrupt;
          Pnp401[DeviceIndex].IsaIrq.Attribute = 0;
          Pnp401[DeviceIndex].IsaIrq.StartRange = IsaDeviceResource->IrqNumber;
          Pnp401[DeviceIndex].IsaIrq.EndRange  =  0;
        
          Pnp401[DeviceIndex].IsaNull.Type  =  EfiIsaAcpiResourceEndOfList;
          Pnp401[DeviceIndex].IsaNull.Attribute  =  0;
          Pnp401[DeviceIndex].IsaNull.StartRange  =  0;
          Pnp401[DeviceIndex].IsaNull.EndRange  =  0;  
          
          //
          // Send HID,UID,ISA_ACPI_IO_IRQ_RESOURCE to EFI_ISA_ACPI_RESOURCE_LIST
          //
          gIsaAcpiDeviceList[Index].Device.HID = EISA_PNP_ID(0x401);
          gIsaAcpiDeviceList[Index].Device.UID = DeviceIndex;
          gIsaAcpiDeviceList[Index].ResourceItem =& Pnp401[DeviceIndex].IsaIo;
          Index++;
        }
      }        
    }
    //
    // Update serial controler resource
    //
    if (NumPnp501) {
      for (DeviceIndex = 0; DeviceIndex < NumPnp501; DeviceIndex++) {
        Uid = 0;
        ComProtocol[DeviceIndex].FunctionPtr->GetPossibleResource(&IsaDeviceResource, &Uid);
        if (IsaDeviceResource->DmaChannel != NULL_ID) {        
          //
          // Auto create the Pnp501 Resource table.
          //
          Pnp501[DeviceIndex].IsaIo.Type = EfiIsaAcpiResourceIo;
          Pnp501[DeviceIndex].IsaIo.Attribute = 0;
          Pnp501[DeviceIndex].IsaIo.StartRange = IsaDeviceResource->IoPort;
          Pnp501[DeviceIndex].IsaIo.EndRange = IsaDeviceResource->IoPort | 7;
    
          Pnp501[DeviceIndex].IsaIrq.Type=EfiIsaAcpiResourceInterrupt;
          Pnp501[DeviceIndex].IsaIrq.Attribute = 0;
          Pnp501[DeviceIndex].IsaIrq.StartRange = IsaDeviceResource->IrqNumber;
          Pnp501[DeviceIndex].IsaIrq.EndRange = 0;
         
          Pnp501[DeviceIndex].IsaNull.Type = EfiIsaAcpiResourceEndOfList;
          Pnp501[DeviceIndex].IsaNull.Attribute = 0;
          Pnp501[DeviceIndex].IsaNull.StartRange = 0;
          Pnp501[DeviceIndex].IsaNull.EndRange = 0;  
      
          //
          // Send HID,UID,ISA_ACPI_IO_IRQ_RESOURCE to EFI_ISA_ACPI_RESOURCE_LIST
          //
          gIsaAcpiDeviceList[Index].Device.HID = EISA_PNP_ID(0x501);
          gIsaAcpiDeviceList[Index].Device.UID = DeviceIndex;
          gIsaAcpiDeviceList[Index].ResourceItem =& Pnp501[DeviceIndex].IsaIo;
          Index++;
        }          
      }
    }
    //
    // Update IR controler resource
    //
    if (NumPnp510) {
      for (DeviceIndex = 0; DeviceIndex < NumPnp510; DeviceIndex++) {
        Uid = 0;
        IrProtocol[DeviceIndex].FunctionPtr->GetPossibleResource (&IsaDeviceResource, &Uid);
        if (IsaDeviceResource->DmaChannel != NULL_ID) {        
          //
          // Auto create the Pnp501 Resource table.
          //
          Pnp510[DeviceIndex].IsaIo.Type = EfiIsaAcpiResourceIo;
          Pnp510[DeviceIndex].IsaIo.Attribute = 0;
          Pnp510[DeviceIndex].IsaIo.StartRange = IsaDeviceResource->IoPort;
          Pnp510[DeviceIndex].IsaIo.EndRange = IsaDeviceResource->IoPort | 7;
    
          Pnp510[DeviceIndex].IsaIrq.Type = EfiIsaAcpiResourceInterrupt;
          Pnp510[DeviceIndex].IsaIrq.Attribute = 0;
          Pnp510[DeviceIndex].IsaIrq.StartRange = IsaDeviceResource->IrqNumber;
          Pnp510[DeviceIndex].IsaIrq.EndRange = 0;
         
          Pnp510[DeviceIndex].IsaNull.Type = EfiIsaAcpiResourceEndOfList;
          Pnp510[DeviceIndex].IsaNull.Attribute = 0;
          Pnp510[DeviceIndex].IsaNull.StartRange = 0;
          Pnp510[DeviceIndex].IsaNull.EndRange = 0;  
      
          //
          // Send HID,UID,ISA_ACPI_IO_IRQ_RESOURCE to EFI_ISA_ACPI_RESOURCE_LIST
          //
          gIsaAcpiDeviceList[Index].Device.HID = EISA_PNP_ID (0x510);
          gIsaAcpiDeviceList[Index].Device.UID = DeviceIndex;
          gIsaAcpiDeviceList[Index].ResourceItem =& Pnp510[DeviceIndex].IsaIo;
          Index++;
        }          
      }
    }
    //
    // Update Fdc controler resource
    //
    if (NumPnp604) {
      for (DeviceIndex = 0; DeviceIndex < NumPnp604; DeviceIndex++) {
        Uid = 0;
        FdcProtocol[DeviceIndex].FunctionPtr->GetPossibleResource(&IsaDeviceResource, &Uid);   
        //
        // Auto create the Pnp604 Resource table.
        //
        Pnp604[DeviceIndex].IsaIo.Type = EfiIsaAcpiResourceIo;
        Pnp604[DeviceIndex].IsaIo.Attribute = 0;
        Pnp604[DeviceIndex].IsaIo.StartRange = IsaDeviceResource->IoPort;
        Pnp604[DeviceIndex].IsaIo.EndRange = IsaDeviceResource->IoPort|7;
   
        Pnp604[DeviceIndex].IsaIrq.Type = EfiIsaAcpiResourceInterrupt;
        Pnp604[DeviceIndex].IsaIrq.Attribute = 0;
        Pnp604[DeviceIndex].IsaIrq.StartRange = IsaDeviceResource->IrqNumber;
        Pnp604[DeviceIndex].IsaIrq.EndRange =0 ;
   
        Pnp604[DeviceIndex].IsaDma.Type = EfiIsaAcpiResourceDma;
        Pnp604[DeviceIndex].IsaDma.Attribute = (UINT32)(EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SPEED_COMPATIBLE | EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_WIDTH_8 | EFI_ISA_IO_SLAVE_DMA_ATTRIBUTE_SINGLE_MODE);
        Pnp604[DeviceIndex].IsaDma.StartRange = IsaDeviceResource->DmaChannel;
        Pnp604[DeviceIndex].IsaDma.EndRange = 0;
         
        Pnp604[DeviceIndex].IsaNull.Type = EfiIsaAcpiResourceEndOfList;
        Pnp604[DeviceIndex].IsaNull.Attribute = 0;
        Pnp604[DeviceIndex].IsaNull.StartRange = 0;
        Pnp604[DeviceIndex].IsaNull.EndRange = 0;  
   
        //
        // Send HID,UID,ISA_ACPI_IO_IRQ_RESOURCE to EFI_ISA_ACPI_RESOURCE_LIST
        //
        gIsaAcpiDeviceList[Index].Device.HID = EISA_PNP_ID(0x604);
        gIsaAcpiDeviceList[Index].Device.UID = DeviceIndex;
        gIsaAcpiDeviceList[Index].ResourceItem =& Pnp604[DeviceIndex].IsaIo;
        Index++;       
      }
    }

    //
    // Update PS/2-style Mouse resource
    //
    if (NumPnpF03) {
      for (DeviceIndex = 0; DeviceIndex < NumPnpF03; DeviceIndex++) {
        Uid = 0;
        Ps2Protocol2[DeviceIndex].FunctionPtr->GetPossibleResource(&IsaDeviceResource, &Uid);
        if (IsaDeviceResource->DmaChannel != NULL_ID) {        
          //
          // Auto create the PnpF03 Resource table.
          //
          PnpF03[DeviceIndex].IsaIo.Type = EfiIsaAcpiResourceIo;
          PnpF03[DeviceIndex].IsaIo.Attribute = 0;
          PnpF03[DeviceIndex].IsaIo.StartRange = IsaDeviceResource->IoPort;
          PnpF03[DeviceIndex].IsaIo.EndRange = IsaDeviceResource->IoPort+4;
  
          PnpF03[DeviceIndex].IsaIrq.Type = EfiIsaAcpiResourceInterrupt;
          PnpF03[DeviceIndex].IsaIrq.Attribute = 0;
          PnpF03[DeviceIndex].IsaIrq.StartRange = IsaDeviceResource->IrqNumber;
          PnpF03[DeviceIndex].IsaIrq.EndRange = 0;
         
          PnpF03[DeviceIndex].IsaNull.Type = EfiIsaAcpiResourceEndOfList;
          PnpF03[DeviceIndex].IsaNull.Attribute = 0;
          PnpF03[DeviceIndex].IsaNull.StartRange = 0;
          PnpF03[DeviceIndex].IsaNull.EndRange = 0; 
  
          //
          // Send HID,UID,ISA_ACPI_IO_IRQ_RESOURCE to EFI_ISA_ACPI_RESOURCE_LIST
          //
          gIsaAcpiDeviceList[Index].Device.HID = EISA_PNP_ID(0xF03);
          gIsaAcpiDeviceList[Index].Device.UID = DeviceIndex;
          gIsaAcpiDeviceList[Index].ResourceItem = &PnpF03[DeviceIndex].IsaIo;
          Index++;
        }          
      }
    }

    //
    // Update Game Port controler resource
    //
    if (NumPnpB02f) {
      for (DeviceIndex = 0; DeviceIndex < NumPnpB02f; DeviceIndex++) {
        Uid = 0;
        FdcProtocol[DeviceIndex].FunctionPtr->GetPossibleResource(&IsaDeviceResource, &Uid);
        if (IsaDeviceResource->DmaChannel != NULL_ID) {        
          //
          // Auto create the Pnp604 Resource table.
          //
          PnpB02f[DeviceIndex].IsaIo.Type = EfiIsaAcpiResourceIo;
          PnpB02f[DeviceIndex].IsaIo.Attribute = 0;
          PnpB02f[DeviceIndex].IsaIo.StartRange = IsaDeviceResource->IoPort;
          PnpB02f[DeviceIndex].IsaIo.EndRange = IsaDeviceResource->IoPort|7;
   
          PnpB02f[DeviceIndex].IsaIrq.Type = EfiIsaAcpiResourceInterrupt;
          PnpB02f[DeviceIndex].IsaIrq.Attribute = 0;
          PnpB02f[DeviceIndex].IsaIrq.StartRange = IsaDeviceResource->IrqNumber;
          PnpB02f[DeviceIndex].IsaIrq.EndRange =0 ;
   
          PnpB02f[DeviceIndex].IsaNull.Type = EfiIsaAcpiResourceEndOfList;
          PnpB02f[DeviceIndex].IsaNull.Attribute = 0;
          PnpB02f[DeviceIndex].IsaNull.StartRange = 0;
          PnpB02f[DeviceIndex].IsaNull.EndRange = 0;  
   
          //
          // Send HID,UID,ISA_ACPI_IO_IRQ_RESOURCE to EFI_ISA_ACPI_RESOURCE_LIST
          //
          gIsaAcpiDeviceList[Index].Device.HID = EISA_PNP_ID(0xB02F);
          gIsaAcpiDeviceList[Index].Device.UID = DeviceIndex;
          gIsaAcpiDeviceList[Index].ResourceItem =& Pnp604[DeviceIndex].IsaIo;
          Index++;
        }          
      }
    }    

    if (NumPnp200) {
      for (DeviceIndex=0; DeviceIndex < NumPnp200; DeviceIndex++) {
        gIsaAcpiDeviceList[Index].Device.HID = EISA_PNP_ID(0x200);
        gIsaAcpiDeviceList[Index].Device.UID = DeviceIndex;
        gIsaAcpiDeviceList[Index].ResourceItem = NULL;
        Index++;
       }
    }
    
    // End of list
    gIsaAcpiDeviceList[Index].Device.HID = 0;
    gIsaAcpiDeviceList[Index].Device.UID = 0;
    gIsaAcpiDeviceList[Index].ResourceItem = NULL;
   
  } 
  return Status;
}

