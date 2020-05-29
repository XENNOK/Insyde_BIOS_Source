/** @file
  This Library will install CRVfrConfigLib for reference.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#include <IndustryStandard/Pci.h>
#include <FrameworkDxe.h>
#include <CrSetupConfig.h>

#include <Guid/MdeModuleHii.h>

#include <Library/PrintLib.h>
#include <Library/CrVfrConfigLib.h>
#include <Library/HiiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/DevicePathLib.h>
#include <Library/VariableLib.h>
#include <Library/CrBdsLib.h>
#include <Library/UefiHiiServicesLib.h>

#include <Protocol/PciIo.h>
#include <Protocol/IsaAcpi.h>
#include <Protocol/CRPolicy.h>
#include <Protocol/DevicePath.h>
#include <Protocol/UsbSerialControllerIo.h>


#define EISA_SERIAL_DEVICE_ID   0x0501

static UINT16   CrFormKeyList[CR_MAX_SUPPORT_SERIAL_PORT] = {
                                  CONSOLE_REDIRECTION_FORM_ID_0,
                                  CONSOLE_REDIRECTION_FORM_ID_1,
                                  CONSOLE_REDIRECTION_FORM_ID_2,
                                  CONSOLE_REDIRECTION_FORM_ID_3,
                                  CONSOLE_REDIRECTION_FORM_ID_4,
                                  CONSOLE_REDIRECTION_FORM_ID_5,
                                  CONSOLE_REDIRECTION_FORM_ID_6,
                                  CONSOLE_REDIRECTION_FORM_ID_7
                                };

CHAR16 *mTermType[4] = { L"VT-100",
                         L"VT-100+",
                         L"VT-UTF8",
                         L"PC-ANSI"};

CHAR16 *mParity [3] = { L"N",
                        L"E",
                        L"O"
                         };

CHAR16 *mIsaSerialPort [8] = { L"COMA",
                               L"COMB",
                               L"COMC",
                               L"COMD",
                               L"COME",
                               L"COMF",
                               L"COMG",
                               L"COMH",
                             };

CHAR16 *mPortStatus [2] = { L"Disable",
                            L"Enable"
                          };

CHAR16 *mCrSerialDevType [4] = { L"None",
                                 L"ISA",
                                 L"PCI",
                                 L"USB"
                               };

UINT32 mCRBaudRateTable [] = {
  1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200
};

UINT8                  mCurrentCallbackFormIdx;
CR_SERIAL_DEV_INFO     mCrIfrNvdata;
CR_CONFIGURATION       mCrConfigData;
EFI_HII_HANDLE         mHiiHandle;
EFI_HANDLE             mImageHandle;
EFI_STRING_ID          mCrSerialDevFormSubtittleStrTokenID;


EFI_STATUS
GetIsaSerialInfo (
  IN OUT CR_SERIAL_DEV_INFO  *CRSerialDevInfo,
  IN     BOOLEAN             DoBusConnect
  )
{
  EFI_STATUS                         Status;
  EFI_DEVICE_PATH_PROTOCOL           *IsaBridgeDevPath;
#ifdef  MULTI_SUPER_IO_SUPPORT
  EFI_MSIO_ISA_ACPI_PROTOCOL         *IsaAcpi;
  EFI_MSIO_ISA_ACPI_DEVICE_ID        *IsaDevice;
  EFI_MSIO_ISA_ACPI_RESOURCE_LIST    *ResourceList;
#else
  EFI_ISA_ACPI_PROTOCOL              *IsaAcpi;
  EFI_ISA_ACPI_DEVICE_ID             *IsaDevice;
  EFI_ISA_ACPI_RESOURCE_LIST         *ResourceList;
#endif
  UINT16                             PortAddr;
  UINT8                              IRQ;
  UINTN                              SerialDevCount;

  SerialDevCount = 0;

  if (DoBusConnect) {
    //
    // Get platform ISA Bridge DevPath
    //
    IsaBridgeDevPath = CRGetPlatformISABridgeDevPath();
    if (IsaBridgeDevPath == NULL)       
      return EFI_UNSUPPORTED;

    //
    // Force LPC device connect with IsaAcpi Driver for get serial port resource
    //
    Status = CRBdsLibConnectDevicePath (IsaBridgeDevPath);
  }

#ifdef MULTI_SUPER_IO_SUPPORT
  Status = gBS->LocateProtocol (&gEfiMsioIsaAcpiProtocolGuid,
                                  NULL,
                                  (VOID **)&IsaAcpi
                                  );
#else
  Status = gBS->LocateProtocol (&gEfiIsaAcpiProtocolGuid,
                                  NULL,
                                  (VOID **)&IsaAcpi
                                  );
#endif
  if (!EFI_ERROR(Status)) {
    
    IsaDevice = NULL;
    do {
      Status = IsaAcpi->DeviceEnumerate (IsaAcpi, &IsaDevice);
      if (EFI_ERROR (Status)) {
        break;
      }

      ResourceList = NULL;
      Status = IsaAcpi->GetCurResource (IsaAcpi, IsaDevice, &ResourceList);
      if (EFI_ERROR (Status)) {
        continue;
      }
    
      if (ResourceList->Device.HID == EISA_PNP_ID(EISA_SERIAL_DEVICE_ID)) {
        IRQ      = 0;
        PortAddr = 0;
        CRGetIsaSerialResource(ResourceList->Device.UID, &PortAddr, &IRQ);
        if (PortAddr != 0 && IRQ != 0) {
          if (SerialDevCount >= CR_MAX_SUPPORT_SERIAL_PORT) return EFI_SUCCESS;
          CRSerialDevInfo->CRDevice[SerialDevCount].Type = ISA_SERIAL_DEVICE;
          CRSerialDevInfo->CRDevice[SerialDevCount].Device.IsaSerial.ComPortAddress = PortAddr;
          CRSerialDevInfo->CRDevice[SerialDevCount].Device.IsaSerial.ComPortIrq = IRQ;
          CRSerialDevInfo->IsaDevCount++;  
          SerialDevCount++;
        }
      }
    } while (TRUE);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetPciSerialInfo (
  IN OUT CR_SERIAL_DEV_INFO  *CRSerialDevInfo,
  IN     BOOLEAN             DoBusConnect
  )
{
  EFI_STATUS                  Status;
  EFI_PCI_IO_PROTOCOL         *PciIo;
  EFI_DEVICE_PATH_PROTOCOL    *DevicePath;
  EFI_HANDLE                  *HandleBuffer;
  UINTN                       HandleCount;
  UINTN                       Idx;
  UINTN                       Segment;
  UINTN                       Bus;
  UINTN                       Dev;
  UINTN                       Func;
  UINTN                       SerialDevCount;


  SerialDevCount = CRSerialDevInfo->IsaDevCount;

  Status = CRLocateDeviceHandleBuffer (PCI_CLASS_SCC, PCI_SUBCLASS_SERIAL, PCI_IF_16550, &HandleCount, &HandleBuffer);
  if (!EFI_ERROR(Status)) {
    
    for (Idx=0; Idx<HandleCount; Idx++) {
      if (SerialDevCount >= CR_MAX_SUPPORT_SERIAL_PORT) {
        return EFI_SUCCESS;
      }
      
      Status = gBS->HandleProtocol (HandleBuffer[Idx], &gEfiPciIoProtocolGuid, (VOID **)&PciIo);
      if (EFI_ERROR(Status)) {
        continue;
      }
      
      Status = gBS->HandleProtocol(HandleBuffer[Idx], &gEfiDevicePathProtocolGuid, (VOID **)&DevicePath);
      if (EFI_ERROR(Status)) {
        continue;
      }
      
      PciIo->GetLocation (PciIo, &Segment, &Bus, &Dev, &Func);
      CRSerialDevInfo->CRDevice[SerialDevCount].Type = PCI_SERIAL_DEVICE;
      CRSerialDevInfo->CRDevice[SerialDevCount].Device.PciSerial.Bus      = (UINT8)Bus;
      CRSerialDevInfo->CRDevice[SerialDevCount].Device.PciSerial.Device   = (UINT8)Dev; 
      CRSerialDevInfo->CRDevice[SerialDevCount].Device.PciSerial.Function = (UINT8)Func;
      CRSerialDevInfo->PciDevCount++;
      SerialDevCount++;
    }

    if (HandleBuffer != NULL) {
      FreePool (HandleBuffer);
    }
    
  }
  
  return EFI_SUCCESS;
}


EFI_STATUS
GetUsbSerialInfo (
  IN OUT CR_SERIAL_DEV_INFO  *CRSerialDevInfo,
  IN     BOOLEAN             DoBusConnect
  )
{
  EFI_STATUS                           Status;
  USB_SERIAL_CONTROLLER_IO_PROTOCOL    *UsbSCIo;
  EFI_HANDLE                           *HandleBuffer;
  UINTN                                HandleCount;
  UINTN                                Idx;
  UINTN                                SerialDevCount;

  HandleBuffer = NULL;
  if (DoBusConnect) {
    //
    // Get all Usb2 controller device handle
    //
    Status = CRLocateDeviceHandleBuffer (PCI_CLASS_SERIAL, PCI_CLASS_SERIAL_USB, PCI_IF_EHCI, &HandleCount, &HandleBuffer);
    if (EFI_ERROR(Status)) {
      return Status;
    }

    for (Idx = 0; Idx < HandleCount; Idx++) {
      //
      // Connect Usb host controller 
      //    
      gBS->ConnectController (HandleBuffer[Idx], NULL, NULL, FALSE);
    }
  }
  
  SerialDevCount = CRSerialDevInfo->IsaDevCount + CRSerialDevInfo->PciDevCount;
  Status = gBS->LocateProtocol (&gUsbSerialControllerIoProtocolGuid, NULL, (VOID **)&UsbSCIo);
  if (!EFI_ERROR(Status)) {
    if (SerialDevCount >= CR_MAX_SUPPORT_SERIAL_PORT) { 
      return EFI_SUCCESS;
    }
    CRSerialDevInfo->CRDevice[SerialDevCount].Type = USB_SERIAL_DEVICE;
    CRSerialDevInfo->UsbDevCount = 1;
  }

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  return EFI_SUCCESS;
}


EFI_STATUS
GetPlatformSerialDevInfo (
  IN OUT CR_SERIAL_DEV_INFO  *CRSerialDevInfo,
  IN     BOOLEAN             DoBusConnect
  )
{

  // Get isa device info 
  GetIsaSerialInfo (CRSerialDevInfo, DoBusConnect);

  // Get pci device info
  GetPciSerialInfo (CRSerialDevInfo, DoBusConnect);

  //
  // Get Usb device info
  //
  if (FeaturePcdGet(PcdCROverUsbSupported)){
    GetUsbSerialInfo (CRSerialDevInfo, DoBusConnect);
  }
  
  return EFI_SUCCESS;
}


EFI_STATUS
UpdateCrSerialDevInfo (
  IN CR_SERIAL_DEV_INFO          *ReferenceDevInfo,
  IN OUT CR_SERIAL_DEV_INFO      *UpdateDevInfo
  )
{
  EFI_CONSOLE_REDIRECTION_DEVICE  *PlatformDev;
  EFI_CONSOLE_REDIRECTION_DEVICE  *CrDev;
  UINTN                           DevNum;
  UINTN                           DevStart;
  UINTN                           TotalDevNum;
  UINTN                           Idx;
  UINTN                           CrDevIdx;
  UINT16                          PortAddr;
  UINT16                          IRQ;
  UINTN                           Bus;
  UINTN                           Dev;
  UINTN                           Func;


  //
  // Base on Update ReferenceDevInfo UpdateDevInfo setting  
  //
  TotalDevNum = ReferenceDevInfo->IsaDevCount + ReferenceDevInfo->PciDevCount + ReferenceDevInfo->UsbDevCount;
  for (Idx = 0; Idx < TotalDevNum; Idx++) {
    PlatformDev = &UpdateDevInfo->CRDevice[Idx];

    
    if (PlatformDev->Type == ISA_SERIAL_DEVICE) {
      PortAddr = PlatformDev->Device.IsaSerial.ComPortAddress;
      IRQ      = PlatformDev->Device.IsaSerial.ComPortIrq;
      DevStart = 0;
      DevNum   = ReferenceDevInfo->IsaDevCount;
      for (CrDevIdx = DevStart; CrDevIdx < DevNum; CrDevIdx++) {
        CrDev = &ReferenceDevInfo->CRDevice[CrDevIdx];
        if (CrDev->Type == ISA_SERIAL_DEVICE &&
            PortAddr == CrDev->Device.IsaSerial.ComPortAddress &&
            IRQ == CrDev->Device.IsaSerial.ComPortIrq) {
          CopyMem (&PlatformDev->CrDevAttr, &CrDev->CrDevAttr, sizeof (CR_DEVICE_ATTRIBUTES));  
          break;
        }
      }
    }
    else if (PlatformDev->Type == PCI_SERIAL_DEVICE) {
      Bus  = PlatformDev->Device.PciSerial.Bus;
      Dev  = PlatformDev->Device.PciSerial.Device;
      Func = PlatformDev->Device.PciSerial.Function;
      DevStart = ReferenceDevInfo->IsaDevCount;
      DevNum   = ReferenceDevInfo->IsaDevCount + ReferenceDevInfo->PciDevCount;
      for (CrDevIdx = DevStart; CrDevIdx < DevNum; CrDevIdx++) {
        CrDev = &ReferenceDevInfo->CRDevice[CrDevIdx];
        if (CrDev->Type == PCI_SERIAL_DEVICE &&
            Bus == CrDev->Device.PciSerial.Bus &&
            Dev == CrDev->Device.PciSerial.Device &&
            Func == CrDev->Device.PciSerial.Function) {
          CopyMem (&PlatformDev->CrDevAttr, &CrDev->CrDevAttr, sizeof (CR_DEVICE_ATTRIBUTES));
          break;
        }
      }
    }
    else if (PlatformDev->Type == USB_SERIAL_DEVICE) {
      DevStart = ReferenceDevInfo->IsaDevCount + ReferenceDevInfo->PciDevCount;
      DevNum   = ReferenceDevInfo->IsaDevCount + ReferenceDevInfo->PciDevCount + ReferenceDevInfo->UsbDevCount;
      for (CrDevIdx = DevStart; CrDevIdx < DevNum; CrDevIdx++) {
        CrDev = &ReferenceDevInfo->CRDevice[CrDevIdx];
        if (CrDev->Type == USB_SERIAL_DEVICE) {
          CopyMem (&PlatformDev->CrDevAttr, &CrDev->CrDevAttr, sizeof (CR_DEVICE_ATTRIBUTES));
          break;
        }
      }
    }
  }

  return EFI_SUCCESS;
}


EFI_STATUS
UpdateCrDevStatusString (
  IN CR_CONFIGURATION           *CrConfig,
  IN CR_SERIAL_DEV_INFO         *CRSerialDevInfo,
  IN UINTN                      CRDevNum,
  IN UINTN                      PromptStrSize,
  IN OUT CHAR16                 *PromptStr
  )
{
  UINT8                   TermType;
  UINT32                  BaudRate;
  UINT8                   Parity;
  UINT8                   DataBit;
  UINT8                   StopBit;
  UINT8                   PortStatus;

  PortStatus = 0;
  PortStatus = CRSerialDevInfo->CRDevice[CRDevNum].CrDevAttr.PortEnable;
  
  if (CRSerialDevInfo->CRDevice[CRDevNum].CrDevAttr.UseGlobalSetting) {
    TermType   = CrConfig->GlobalTerminalType;
    BaudRate   = mCRBaudRateTable[CrConfig->GlobalBaudRate]; 
    DataBit    = CrConfig->GlobalDataBits; 
    StopBit    = (CrConfig->GlobalStopBits == 1) ? 1 : 2;
    Parity     = CrConfig->GlobalParity;
  }
  else {
    TermType   = CRSerialDevInfo->CRDevice[CRDevNum].CrDevAttr.TerminalType;
    BaudRate   = mCRBaudRateTable[CRSerialDevInfo->CRDevice[CRDevNum].CrDevAttr.BaudRate]; 
    DataBit    = CRSerialDevInfo->CRDevice[CRDevNum].CrDevAttr.DataBits; 
    StopBit    = (CRSerialDevInfo->CRDevice[CRDevNum].CrDevAttr.StopBits == 1) ? 1 : 2;
    Parity     = CRSerialDevInfo->CRDevice[CRDevNum].CrDevAttr.Parity;
  }
  
  UnicodeSPrint (
    PromptStr,
    PromptStrSize, 
    L"%s %s,%d,%s%d%d", 
    mPortStatus[PortStatus], 
    mTermType[TermType], 
    BaudRate, 
    mParity[Parity - 1], 
    DataBit, 
    StopBit 
    );

  return EFI_SUCCESS;

};


EFI_STATUS
RefreshCrDevInfor (
  void
  )
{
  CHAR16                  *PromptStr;
  UINTN                   PromptStrSize;
  UINTN                   Idx;
  UINTN                   CrDevCount;


  PromptStrSize = 0x100;
  PromptStr = AllocatePool (PromptStrSize);
  CrDevCount = mCrIfrNvdata.IsaDevCount + mCrIfrNvdata.PciDevCount + mCrIfrNvdata.UsbDevCount;

  //
  // Refresh each serial device status
  //
  for (Idx = 0; Idx < CrDevCount; Idx++) {
    ZeroMem (PromptStr, PromptStrSize);
    UpdateCrDevStatusString (&mCrConfigData, &mCrIfrNvdata, Idx, PromptStrSize, PromptStr);
    HiiSetString (mHiiHandle, mCrIfrNvdata.CRDevice[Idx].CrDevStatusStrID, PromptStr, NULL);
  }

  if (PromptStr != NULL) {
    FreePool (PromptStr);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
CrDevLoadDefaultSetting (
  IN OUT  CR_SERIAL_DEV_INFO  *CRSerialDevInfo
  )
{
  EFI_STATUS              Status;
  CR_DEVICE_ATTRIBUTES    *CrDevAttri;
  EFI_CR_POLICY_PROTOCOL  *CRPolicy;
  UINTN                   Idx;

  //
  // Get Console Redirection Policy
  //
  Status = gBS->LocateProtocol (&gEfiCRPolicyProtocolGuid, NULL, (VOID **)&CRPolicy);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  for (Idx = 0; Idx < CR_MAX_SUPPORT_SERIAL_PORT; Idx++) {
    CrDevAttri = &CRSerialDevInfo->CRDevice[Idx].CrDevAttr;
    CopyMem (CrDevAttri, &CRPolicy->CrDevDefaultAttr[Idx], sizeof (CR_DEVICE_ATTRIBUTES));     
  }

  return EFI_SUCCESS;
};


EFI_STATUS
InitCrSerialPortLabel (
  IN OUT CR_SERIAL_DEV_INFO                 *CRSerialDevInfo
  )
{
  EFI_STRING_ID              PromptStrID;
  EFI_STRING_ID              StrTokenID;
  CHAR16                     *BlankStr;
  CHAR16                     *PromptStr;
  EFI_IFR_GUID_LABEL         *CrSerialPortLabel;
  VOID						           *StartOpCodeHandle;
  UINTN                      PromptStrSize;
  UINTN                      SerialPortIdx = 0;
  UINTN                      Bus, Dev, Func;

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();

  CrSerialPortLabel = (EFI_IFR_GUID_LABEL*) HiiCreateGuidOpCode (
                                              StartOpCodeHandle,
                                              &gEfiIfrTianoGuid,
                                              NULL,
                                              sizeof (EFI_IFR_GUID_LABEL)
                                              );

  CrSerialPortLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  CrSerialPortLabel->Number       = CR_SERIAL_PORT_LABEL;

  // Add one blank line
  BlankStr = L" ";
  StrTokenID = HiiSetString (mHiiHandle, 0, BlankStr, NULL);
  
  HiiCreateTextOpCode (
    StartOpCodeHandle,
    (STRING_REF) STRING_TOKEN (StrTokenID),
    (STRING_REF) STRING_TOKEN (StrTokenID),
    (STRING_REF) STRING_TOKEN (StrTokenID)
    );

  PromptStrSize = 0x100;
  PromptStr = AllocatePool (PromptStrSize);
  for (SerialPortIdx = 0; SerialPortIdx < CRSerialDevInfo->IsaDevCount; SerialPortIdx++) {
    
    ZeroMem (PromptStr, PromptStrSize);
    UnicodeSPrint (
      PromptStr,
      PromptStrSize, 
      mIsaSerialPort[SerialPortIdx] 
      );

    PromptStrID = HiiSetString (mHiiHandle, 0, PromptStr, NULL);
   
    HiiCreateGotoOpCode (
      StartOpCodeHandle,
      CONSOLE_REDIRECTION_FORM_ID,
      STRING_TOKEN(PromptStrID),
      0,
      EFI_IFR_FLAG_CALLBACK,
      (UINT16)(CONSOLE_REDIRECTION_FORM_ID_0 + SerialPortIdx)
      );

    // Add Serial device status line
    ZeroMem (PromptStr, PromptStrSize); 
    UpdateCrDevStatusString(&mCrConfigData, CRSerialDevInfo, SerialPortIdx, PromptStrSize, PromptStr);
    PromptStrID = HiiSetString (mHiiHandle, 0, PromptStr, NULL);
    CRSerialDevInfo->CRDevice[SerialPortIdx].CrDevStatusStrID = PromptStrID;
    HiiCreateTextOpCode (
      StartOpCodeHandle,
      (STRING_REF) STRING_TOKEN (PromptStrID),
      (STRING_REF) STRING_TOKEN (StrTokenID),
      (STRING_REF) STRING_TOKEN (StrTokenID)
      );
  }

  // Add one blank line
  StrTokenID = HiiSetString (mHiiHandle, 0, BlankStr, NULL);
  
  HiiCreateTextOpCode (
    StartOpCodeHandle,
    (STRING_REF) STRING_TOKEN (StrTokenID),
    (STRING_REF) STRING_TOKEN (StrTokenID),
    (STRING_REF) STRING_TOKEN (StrTokenID)
    );


  //PCI prograss >>>
  for (SerialPortIdx=0; SerialPortIdx < CRSerialDevInfo->PciDevCount; SerialPortIdx++) {
    Bus  = CRSerialDevInfo->CRDevice[CRSerialDevInfo->IsaDevCount + SerialPortIdx].Device.PciSerial.Bus;
    Dev  = CRSerialDevInfo->CRDevice[CRSerialDevInfo->IsaDevCount + SerialPortIdx].Device.PciSerial.Device;
    Func = CRSerialDevInfo->CRDevice[CRSerialDevInfo->IsaDevCount + SerialPortIdx].Device.PciSerial.Function;
    
    ZeroMem (PromptStr, PromptStrSize);
    UnicodeSPrint (
      PromptStr,
      PromptStrSize, 
      L"Pci Serial Port %d:%d:%d",
      Bus,
      Dev,
      Func
      );
    
    PromptStrID = HiiSetString (mHiiHandle, 0, PromptStr, NULL);
    HiiCreateGotoOpCode (
      StartOpCodeHandle,
      CONSOLE_REDIRECTION_FORM_ID,
      STRING_TOKEN(PromptStrID),
      0,
      EFI_IFR_FLAG_CALLBACK,
      (UINT16)(CONSOLE_REDIRECTION_FORM_ID_0 + (CRSerialDevInfo->IsaDevCount) + SerialPortIdx)
      );

    // Add Serial device status line
    ZeroMem (PromptStr, PromptStrSize);
    UpdateCrDevStatusString(&mCrConfigData, CRSerialDevInfo, CRSerialDevInfo->IsaDevCount + SerialPortIdx, PromptStrSize, PromptStr);
    PromptStrID = HiiSetString (mHiiHandle, 0, PromptStr, NULL);
    CRSerialDevInfo->CRDevice[CRSerialDevInfo->IsaDevCount + SerialPortIdx].CrDevStatusStrID = PromptStrID;
    
    HiiCreateTextOpCode (
      StartOpCodeHandle,
      (STRING_REF) STRING_TOKEN (PromptStrID),
      (STRING_REF) STRING_TOKEN (StrTokenID),
      (STRING_REF) STRING_TOKEN (StrTokenID)
      );
  }
  
  // Add one blank line
  StrTokenID = HiiSetString (mHiiHandle, 0, BlankStr, NULL);
  
  HiiCreateTextOpCode (
    StartOpCodeHandle,
    (STRING_REF) STRING_TOKEN (StrTokenID),
    (STRING_REF) STRING_TOKEN (StrTokenID),
    (STRING_REF) STRING_TOKEN (StrTokenID)
    );


  // Usb prograss >>>
  if (CRSerialDevInfo->UsbDevCount > 0) {
    
    ZeroMem (PromptStr, PromptStrSize);
    UnicodeSPrint (
      PromptStr,
      PromptStrSize, 
      L"Usb Serial Port"
      );
    
    PromptStrID = HiiSetString (mHiiHandle, 0, PromptStr, NULL);
    HiiCreateGotoOpCode (
      StartOpCodeHandle,
      CONSOLE_REDIRECTION_FORM_ID,
      STRING_TOKEN(PromptStrID),
      0,
      EFI_IFR_FLAG_CALLBACK,
      (UINT16)(CONSOLE_REDIRECTION_FORM_ID_0 + (CRSerialDevInfo->IsaDevCount) + (CRSerialDevInfo->PciDevCount))
      );

    // Add Serial device status line
    ZeroMem (PromptStr, PromptStrSize);
    UpdateCrDevStatusString(
      &mCrConfigData, 
      CRSerialDevInfo, 
      CRSerialDevInfo->IsaDevCount + CRSerialDevInfo->PciDevCount, 
      PromptStrSize, 
      PromptStr
      );
    PromptStrID = HiiSetString (mHiiHandle, 0, PromptStr, NULL);
    CRSerialDevInfo->CRDevice[CRSerialDevInfo->IsaDevCount + CRSerialDevInfo->PciDevCount].CrDevStatusStrID = PromptStrID;
    
    HiiCreateTextOpCode (
      StartOpCodeHandle,
      (STRING_REF) STRING_TOKEN (PromptStrID),
      (STRING_REF) STRING_TOKEN (StrTokenID),
      (STRING_REF) STRING_TOKEN (StrTokenID)
      );
  }

  HiiUpdateForm (
    mHiiHandle,
    NULL,
    0x36,
    StartOpCodeHandle,
    NULL
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  if (PromptStr != NULL) {
    FreePool (PromptStr);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
InitCrSerialDevFormSubtittleLabel (void)
{
  EFI_STRING_ID                   StrTokenID;
  EFI_IFR_GUID_LABEL              *CrSerialPortLabel;
  VOID						                *StartOpCodeHandle;
  CHAR16                          *BlankStr;
  
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  
  CrSerialPortLabel = (EFI_IFR_GUID_LABEL*) HiiCreateGuidOpCode (
                                              StartOpCodeHandle,
                                              &gEfiIfrTianoGuid,
                                              NULL,
                                              sizeof (EFI_IFR_GUID_LABEL)
                                              );

  CrSerialPortLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  CrSerialPortLabel->Number       = CR_SERIAL_DEV_FORM_SUBTITTLE_LABEL;

  BlankStr = L" ";
  mCrSerialDevFormSubtittleStrTokenID = HiiSetString (mHiiHandle, 0, BlankStr, NULL);
  StrTokenID = HiiSetString (mHiiHandle, 0, BlankStr, NULL);
  HiiCreateTextOpCode (
    StartOpCodeHandle,
    (STRING_REF) STRING_TOKEN (mCrSerialDevFormSubtittleStrTokenID),
    (STRING_REF) STRING_TOKEN (StrTokenID),
    (STRING_REF) STRING_TOKEN (StrTokenID)
    ); 

  HiiUpdateForm (
    mHiiHandle,
    NULL,
    CONSOLE_REDIRECTION_FORM_ID,
    StartOpCodeHandle,
    NULL
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);

  return EFI_SUCCESS;
}


EFI_STATUS
CrSerialDevOptionInit (
  IN EFI_HII_HANDLE                         HiiHandle
  )
{
  EFI_STATUS                                  Status;
  CR_SERIAL_DEV_INFO                          PlatformCrDevInfo;
  CR_SERIAL_DEV_INFO                          *CrIfrNvdata;
  CR_CONFIGURATION                            *CrConfigData;
  UINTN                                       CrIfrNvdataSize;
  
  mHiiHandle = HiiHandle;
    
  //
  // After BDS connect procedure, get platform all serial device(IsaSerial, PciSerial, UsbSerial)
  //
  ZeroMem (&PlatformCrDevInfo, sizeof (CR_SERIAL_DEV_INFO));
  Status = GetPlatformSerialDevInfo (&PlatformCrDevInfo, FALSE);
  CrDevLoadDefaultSetting (&PlatformCrDevInfo);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  //
  // Update CRSerialDevInfo with CrIfrNvdata
  //
  CrIfrNvdata = NULL;
  CrIfrNvdataSize = sizeof (CR_SERIAL_DEV_INFO);  
  CrIfrNvdata = CommonGetVariableData (L"CRNvData", &gEfiGenericVariableGuid);
  if (CrIfrNvdata != NULL) {
    UpdateCrSerialDevInfo (CrIfrNvdata, &PlatformCrDevInfo);
  }
  
  //
  // Init CRConfig
  //
  CrConfigData = CommonGetVariableData (L"CrConfig", &gCrConfigurationGuid);
  if (CrConfigData != NULL) {
    
    CopyMem ((UINT8*)&mCrConfigData, (UINT8*)CrConfigData, sizeof (CR_CONFIGURATION));
    if (CrConfigData != NULL) {
      FreePool (CrConfigData);
    }
  }

  //
  // Init CR scu label
  //
  InitCrSerialPortLabel (&PlatformCrDevInfo);
  InitCrSerialDevFormSubtittleLabel ();


  //
  // Sync CRForm setting with CRSerialDevInfo
  //
  CopyMem (&mCrIfrNvdata, &PlatformCrDevInfo, sizeof (CR_SERIAL_DEV_INFO));

  Status = CommonSetVariable (
             L"CRNvData",
             &gEfiGenericVariableGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
             sizeof (CR_SERIAL_DEV_INFO),
             &mCrIfrNvdata
             );

  if (CrIfrNvdata != NULL) {
    FreePool (CrIfrNvdata);
  }
  
  return Status;
}



EFI_STATUS
CRScuAdvanceCallback (
  IN  EFI_QUESTION_ID             QuestionId
  )
{
  BOOLEAN                 BrowserAccessSuccess;
  CHAR16                  *PromptStr;
  UINTN                   PromptStrSize;
  UINTN                   Idx;
  UINTN                   CrDevCount;
 
  
  PromptStrSize = 0x100;
  PromptStr = AllocateZeroPool (PromptStrSize);
  
  CrDevCount = mCrIfrNvdata.IsaDevCount + mCrIfrNvdata.PciDevCount + mCrIfrNvdata.UsbDevCount;

  switch (QuestionId) {
    case CONSOLE_REDIRECTION_FORM_ID_0 :
    case CONSOLE_REDIRECTION_FORM_ID_1 :
    case CONSOLE_REDIRECTION_FORM_ID_2 :
    case CONSOLE_REDIRECTION_FORM_ID_3 :
    case CONSOLE_REDIRECTION_FORM_ID_4 :
    case CONSOLE_REDIRECTION_FORM_ID_5 :
    case CONSOLE_REDIRECTION_FORM_ID_6 :
    case CONSOLE_REDIRECTION_FORM_ID_7 : 
      //
      // Confirm which form callback
      //
      for (Idx = 0; Idx < CR_MAX_SUPPORT_SERIAL_PORT; Idx++) {
        if (QuestionId == CrFormKeyList[Idx]) {
          mCurrentCallbackFormIdx = (UINT8)Idx;
          break;
        }
      }

      //
      //  Initial CR Device Form subtitle
      //     
      UnicodeSPrint (
        PromptStr,
        PromptStrSize, 
        L"%s Serial Dev", 
        mCrSerialDevType[mCrIfrNvdata.CRDevice[mCurrentCallbackFormIdx].Type] 
        );
      
      HiiSetString (
        mHiiHandle, 
        STRING_TOKEN (mCrSerialDevFormSubtittleStrTokenID),
        PromptStr,
        NULL
        );

      //
      // Restore currentcallback serial device setting
      //
      CopyMem (
        &mCrConfigData.CrPortConfig, 
        &mCrIfrNvdata.CRDevice[mCurrentCallbackFormIdx].CrDevAttr,
        sizeof (CR_DEVICE_ATTRIBUTES)
        );

      HiiSetBrowserData (
        &gCrConfigurationGuid, 
        L"CrConfig", 
        sizeof(CR_CONFIGURATION),
        (UINT8 *)&mCrConfigData,
        NULL
        );
      
      break;

    case KEY_CONSOLE_REDIRECTION_1 : 
    case KEY_CONSOLE_REDIRECTION_2 :
    case KEY_CONSOLE_REDIRECTION_3 :    
    case KEY_CONSOLE_REDIRECTION_4 :
    case KEY_CONSOLE_REDIRECTION_5 :
    case KEY_CONSOLE_REDIRECTION_6 :
    case KEY_CONSOLE_REDIRECTION_7 :
    case KEY_CONSOLE_REDIRECTION_8 :

      //
      // Apply CrConfig setting 
      //
      BrowserAccessSuccess = HiiGetBrowserData (
                               &gCrConfigurationGuid,
                               L"CrConfig",
                               sizeof(CR_CONFIGURATION),
                               (UINT8 *)&mCrConfigData
                               );

      if (BrowserAccessSuccess) {                
        RefreshCrDevInfor ();
      }

      
      break;
      
    case KEY_CONSOLE_REDIRECTION_9 :
    case KEY_CONSOLE_REDIRECTION_10 :  
    case KEY_CONSOLE_REDIRECTION_11 :
    case KEY_CONSOLE_REDIRECTION_12 : 
    case KEY_CONSOLE_REDIRECTION_13 :
    case KEY_CONSOLE_REDIRECTION_14 :
    case KEY_CONSOLE_REDIRECTION_15 :  
    case KEY_CONSOLE_REDIRECTION_16 :
      
      //
      // Apply CRDevice private setting 
      //
      BrowserAccessSuccess = HiiGetBrowserData (
                               &gCrConfigurationGuid,
                               L"CrConfig",
                               sizeof(CR_CONFIGURATION),
                               (UINT8 *)&mCrConfigData
                               );
      
      if (BrowserAccessSuccess) {        
        CopyMem (
          &mCrIfrNvdata.CRDevice[mCurrentCallbackFormIdx].CrDevAttr, 
          &mCrConfigData.CrPortConfig,
          sizeof (CR_DEVICE_ATTRIBUTES)
          );
      }
      
      RefreshCrDevInfor ();
      
      break;

    default: 
      break;
  }


  if (PromptStr != NULL) {
    gBS->FreePool (PromptStr);
  }
  
  return EFI_SUCCESS;
  
}

EFI_STATUS
CRScuHotKeyCallback (
  IN  EFI_QUESTION_ID             QuestionId,
  IN  EFI_INPUT_KEY               Key
  ) 
{
  CR_CONFIGURATION        *CrConfig;
  CR_CONFIGURATION        *CrConfigDefault;
  CR_SERIAL_DEV_INFO      *CrIfrNvdata;
  EFI_STATUS              Status;
  UINTN                   CrDevCount;
  BOOLEAN                 SaveCurrentSetting;

  CrDevCount = mCrIfrNvdata.IsaDevCount + mCrIfrNvdata.PciDevCount + mCrIfrNvdata.UsbDevCount;
  SaveCurrentSetting = FALSE;

  switch (QuestionId) {
    case KEY_SCAN_ESC :
      //
      // Discard setup and exit
      //
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      }
      break;
      
    case KEY_SCAN_F9 :
      //
      // Load Optimal
      //
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {

        //
        // Load mCrConfigData to default setting
        //
        CrConfigDefault = CommonGetVariableData (L"CrConfigDefault", &gCrConfigurationGuid);
        if (CrConfigDefault != NULL) {
          CopyMem (&mCrConfigData, CrConfigDefault, sizeof (CR_CONFIGURATION));
          if (CrConfigDefault != NULL) {
            FreePool (CrConfigDefault);
          }
        }

        //
        // Load mCrIfrNvdata to default setting
        //        
        CrDevLoadDefaultSetting (&mCrIfrNvdata);        
        CopyMem (&mCrConfigData.CrPortConfig, &mCrIfrNvdata.CRDevice[mCurrentCallbackFormIdx].CrDevAttr, sizeof (CR_DEVICE_ATTRIBUTES));
        
        HiiSetBrowserData (
          &gCrConfigurationGuid, 
          L"CrConfig", 
          sizeof(CR_CONFIGURATION),
          (UINT8 *)&mCrConfigData,
          NULL
          );
        
        RefreshCrDevInfor ();
        
      }
      break;

    case KEY_SCAN_F10 :
      //
      // Save setup and exit.
      //
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        SaveCurrentSetting = TRUE;
      }
      break;

    case KEY_SAVE_WITHOUT_EXIT:
      //
      // Save setup and without exit
      //
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        RefreshCrDevInfor ();
        SaveCurrentSetting = TRUE;
      }
      
      break;

    case KEY_DISCARD_CHANGE:
      //
      // discard setup change
      //
      if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
        //
        // Restore CrNvData and CrConfig variable setting to mCrIfrNvdata and mCrConfigData
        //
        CrIfrNvdata = CommonGetVariableData (L"CRNvData", &gEfiGenericVariableGuid);
        if (CrIfrNvdata != NULL) {
          CopyMem (&mCrIfrNvdata, CrIfrNvdata, sizeof (CR_SERIAL_DEV_INFO));
          if (CrIfrNvdata != NULL) {
            FreePool (CrIfrNvdata);
          }
        }
        
        CrConfig = CommonGetVariableData (L"CrConfig", &gCrConfigurationGuid);
        if (CrConfig != NULL) {
          CopyMem (&mCrConfigData, CrConfig, sizeof (CR_CONFIGURATION));
          if (CrConfig != NULL) {
            FreePool (CrConfig);
          }
        }

        HiiSetBrowserData (
          &gCrConfigurationGuid, 
          L"CrConfig", 
          sizeof(CR_CONFIGURATION),
          (UINT8 *)&mCrConfigData,
          NULL
          );

        RefreshCrDevInfor ();
      }
      
      break;
      
    default: 
      break;  
  }

  if (SaveCurrentSetting) {
    Status = CommonSetVariable (
               L"CRNvData",
               &gEfiGenericVariableGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               sizeof (CR_SERIAL_DEV_INFO),
               &mCrIfrNvdata
               );


    Status = CommonSetVariable (
               L"CrConfig",
               &gCrConfigurationGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               sizeof (CR_CONFIGURATION),
               &mCrConfigData
               );

  }

  return EFI_SUCCESS;   

}


EFI_STATUS
EFIAPI
CRConfigExtractCallBack (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                    Status;
  UINTN                         BufferSize;
  EFI_STRING                    ConfigRequestHdr;
  EFI_STRING                    ConfigRequest;
  BOOLEAN                       AllocatedRequest;
  UINTN                         Size;


  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }
   *Progress = Request;
  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &gCrConfigurationGuid, L"CrConfig")) {
    return EFI_NOT_FOUND;
  }
  
  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;
  Size             = 0;

  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (
                         &gCrConfigurationGuid,
                         L"CrConfig",
                         mHiiHandle
                         );
    if (ConfigRequestHdr == NULL) {
      return EFI_NOT_FOUND;
    }

    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)StrSize(ConfigRequestHdr));
    gBS->FreePool (ConfigRequestHdr);
  }

  BufferSize = sizeof (CR_CONFIGURATION);
  Status = gHiiConfigRouting->BlockToConfig (
                                gHiiConfigRouting,
                                ConfigRequest,
                                (UINT8 *) &mCrConfigData,
                                BufferSize,
                                Results,
                                Progress
                                );
  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    gBS->FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  //
  // If execute RefreshCrDevInfor function in non-advance formset (ex: main, exit...etc),
  // function will fail. so we execute RefreshCrDevInfor here.
  //  
  RefreshCrDevInfor (); 

  return EFI_SUCCESS;
  
}

EFI_STATUS
EFIAPI
CRConfigRouteCallBack (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN CONST EFI_STRING                       Configuration,
  OUT      EFI_STRING                       *Progress
  )
{
  return EFI_SUCCESS;
}



EFI_STATUS
CRBdsScuInit (
  EFI_CR_POLICY_PROTOCOL    *CRPolicy
  )
{
  EFI_STATUS                Status;
  CR_SERIAL_DEV_INFO        *CrIfrNvdata;
  CR_SERIAL_DEV_INFO        PlatformCrDevInfo;
  UINTN                     CrIfrNvdataSize;
  
  
  //
  // Init CrNvData  
  //
  CrIfrNvdata = NULL;
  CrIfrNvdata = CommonGetVariableData (L"CRNvData", &gEfiGenericVariableGuid);
  if (CrIfrNvdata == NULL) {
    //
    // Get platform serial device.
    //
    ZeroMem (&PlatformCrDevInfo, sizeof (CR_SERIAL_DEV_INFO)); 
    GetPlatformSerialDevInfo (&PlatformCrDevInfo, TRUE);
    CrDevLoadDefaultSetting (&PlatformCrDevInfo);

    //
    // Build CrNvData
    //
    CrIfrNvdataSize = sizeof(CR_SERIAL_DEV_INFO);
    CrIfrNvdata = AllocateZeroPool (CrIfrNvdataSize);
    CopyMem (CrIfrNvdata, &PlatformCrDevInfo, CrIfrNvdataSize);
    
    Status = CommonSetVariable (
               L"CRNvData",
               &gEfiGenericVariableGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
               sizeof (CR_SERIAL_DEV_INFO),
               CrIfrNvdata
               );
  } 

  
  //
  // Update CRPolicy with CrIfrNvdata setting
  //
  if (CRPolicy != NULL) {
    CopyMem (&CRPolicy->CRSerialDevData, CrIfrNvdata, sizeof (CR_SERIAL_DEV_INFO));
  }

  if (CrIfrNvdata != NULL) {
    FreePool (CrIfrNvdata);
  }

  return EFI_SUCCESS;
}


