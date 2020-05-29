/** @file
Console for formBrowser
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

#include "InternalH2OFormBrowser.h"
#include <Protocol/SetupMouse.h>

H2O_FORM_BROWSER_CONSOLE_DEV                 *mConsoleDevList      = NULL;
UINT32                                        mConsoleDevListCount = 0;
#define EISA_SERIAL_DEVICE_ID    0x0501

CONSOLE_TYPE_MAP mConsoleMap[] = {
  {&gEfiSimpleTextInProtocolGuid,      H2O_FORM_BROWSER_CONSOLE_STI},
  {&gEfiSimpleTextInputExProtocolGuid, H2O_FORM_BROWSER_CONSOLE_STI2},
  {&gEfiSimplePointerProtocolGuid,     H2O_FORM_BROWSER_CONSOLE_SP},
  {&gEfiAbsolutePointerProtocolGuid,   H2O_FORM_BROWSER_CONSOLE_AP},
  {&gEfiSimpleTextOutProtocolGuid,     H2O_FORM_BROWSER_CONSOLE_STO},
  {&gEfiGraphicsOutputProtocolGuid,    H2O_FORM_BROWSER_CONSOLE_GO},
  {NULL, 0},
};

/**
  Converts a device path to an Unicode string.

  This function is dependent on the Device Path To Text protocol. If protocol
  does not installed, then return unknown device path L"?" directly.

  @param  [in] DevPath      Given device path instance

  @return   Converted string from given device path.(Caller is responsible to free the string)
  @retval   L""  Can not locate gEfiDevicePathToTextProtocolGuid protocol for converting.
**/
STATIC
CHAR8 *
DevicePathToAsciiStr (
  IN EFI_DEVICE_PATH_PROTOCOL     *DevPath
  )
{
  EFI_STATUS                       Status;
  EFI_DEVICE_PATH_TO_TEXT_PROTOCOL *DevPathToText;
  CHAR16                           *ToText;
  CHAR8                            *AsciiText;

  AsciiText = NULL;

  if (DevPath == NULL) {
    AsciiText = AllocatePool (sizeof (CHAR8));
    if (AsciiText != NULL) {
      AsciiText[0] = '\0';
    }
  } else {

    Status = gBS->LocateProtocol (
                    &gEfiDevicePathToTextProtocolGuid,
                    NULL,
                    (VOID **) &DevPathToText
                    );
    if (!EFI_ERROR (Status)) {
      ToText = DevPathToText->ConvertDevicePathToText (
                                DevPath,
                                FALSE,
                                TRUE
                                );
      if (ToText != NULL) {
        AsciiText = AllocateZeroPool ((StrLen (ToText) + 1) * sizeof (CHAR8));
        if (AsciiText != NULL) {
          UnicodeStrToAsciiStr (ToText, AsciiText);
        }
        FreePool (ToText);
      }
    }
  }
  return AsciiText;
}

/**
  Find device path node.

  @param  [in.out] DevPath          Given device path instance
  @param  [in]     DevPathType      Device path type
  @param  [in]     DevPathSubType   Device path sub type

  @return   Device path
**/
EFI_DEVICE_PATH_PROTOCOL *
LocateDevicePathNode(
  IN OUT EFI_DEVICE_PATH_PROTOCOL   **DevicePath,
  IN UINT8                          DevPathType,
  IN UINT8                          DevPathSubType
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;

  if (*DevicePath == NULL) {
    return NULL;
  }

  DevPath = *DevicePath;

  while (!IsDevicePathEnd (DevPath)) {
    if (DevPath->Type == DevPathType && DevPath->SubType == DevPathSubType) {
      *DevicePath = NextDevicePathNode (DevPath);
      return DevPath;
    }
    DevPath = NextDevicePathNode(DevPath);
  }

  *DevicePath = NULL;

  if (DevPathType == END_DEVICE_PATH_TYPE && DevPathSubType == END_ENTIRE_DEVICE_PATH_SUBTYPE) {
    return DevPath;
  }

  return NULL;
}

/**
 Get the ISA serial device path node for given device path

 @param [in] DevicePath         The device path to be processed

 @return  The retrieved ISA serial device path
 @retval NULL                   The ISA serial device path is not found

**/
ACPI_HID_DEVICE_PATH *
LocateIsaSerialDevicePathNode (
  IN EFI_DEVICE_PATH_PROTOCOL   *DevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  ACPI_HID_DEVICE_PATH      *AcpiDevPath;

  if (DevicePath == NULL)
    return NULL;

  DevPath = DevicePath;

  AcpiDevPath = (ACPI_HID_DEVICE_PATH *) LocateDevicePathNode (&DevPath, ACPI_DEVICE_PATH, ACPI_DP);

  while (AcpiDevPath != NULL) {

    if (AcpiDevPath->HID == EISA_PNP_ID(EISA_SERIAL_DEVICE_ID))
      return AcpiDevPath;

    AcpiDevPath = (ACPI_HID_DEVICE_PATH *) LocateDevicePathNode (&DevPath, ACPI_DEVICE_PATH, ACPI_DP);
  }

  return NULL;
}

/**
 Check whether a give device path is an ISA serial device

 @param [in] DevicePath         The device path to be processed

 @return TRUE                   The device path is an ISA serial device
 @retval FALSE                  The device path is not an ISA serial device

**/
BOOLEAN
IsIsaSerialDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  ACPI_HID_DEVICE_PATH      *SerialDevPath;

  if (DevicePath == NULL) {
    return FALSE;
  }

  SerialDevPath = LocateIsaSerialDevicePathNode (DevicePath);

  if (SerialDevPath != NULL) {
    return TRUE;
  }

  return FALSE;
}



/**
 Check whether a give device path is a PCI serial device

 @param [in] DevicePath         The device path to be processed

 @return TRUE                   The device path is a PCI serial device
 @retval FALSE                  The device path is not a PCI serial device

**/
BOOLEAN
IsPciSerialDevicePathInstance (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
)
{
  EFI_DEVICE_PATH_PROTOCOL  *DevPath;
  PCI_DEVICE_PATH           *PciDevPath;

  if (DevicePath == NULL)
    return FALSE;

  DevPath = DevicePath;

  PciDevPath = (PCI_DEVICE_PATH *)LocateDevicePathNode (&DevPath, HARDWARE_DEVICE_PATH, HW_PCI_DP);

  while (PciDevPath != NULL) {

    if (DevPath->Type == MESSAGING_DEVICE_PATH &&
        DevPath->SubType == MSG_UART_DP)
      return TRUE;

    PciDevPath = (PCI_DEVICE_PATH *) LocateDevicePathNode (&DevPath, HARDWARE_DEVICE_PATH, HW_PCI_DP);
  }

  return FALSE;
}

/**
 Check whether a given device handle belongs to console device type or not

 @param[in] Handle              The device handle to be processed

 @return TRUE                   The device handle belongs to console device type
 @retval FALSE                  The device handle does not belong to console device type

**/
BOOLEAN
IsConsoleDeviceType (
  IN EFI_HANDLE                            Handle
  )
{
  UINTN                                    Index;
  VOID                                     *Instance;
  EFI_STATUS                               Status;

  Index = 0;
  while (mConsoleMap[Index].Guid != NULL) {
    Status = gBS->HandleProtocol (Handle, mConsoleMap[Index].Guid, (VOID **) &Instance);
    if (!EFI_ERROR (Status)) {
      return TRUE;
    }

    Index++;
  }

  return FALSE;
}

/**
 Check whether a given device type belongs to console input device type or not

 @param[in] DeviceType          The device type to be processed

 @return TRUE                   The device type belongs to console input device type
 @retval FALSE                  The device type does not belongs to console input device type

**/
BOOLEAN
IsConInDeviceType (
  IN H2O_CONSOLE_DEVICE_TYPE               DeviceType
  )
{
  if ((DeviceType & H2O_FORM_BROWSER_CONSOLE_STI)  ||
      (DeviceType & H2O_FORM_BROWSER_CONSOLE_STI2) ||
      (DeviceType & H2O_FORM_BROWSER_CONSOLE_SP)   ||
      (DeviceType & H2O_FORM_BROWSER_CONSOLE_AP)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Check whether a given device type belongs to console output device type or not

 @param[in] DeviceType          The device type to be processed

 @return TRUE                   The device type belongs to console output device type
 @retval FALSE                  The device type does not belongs to console output device type

**/
BOOLEAN
IsConOutDeviceType (
  IN H2O_CONSOLE_DEVICE_TYPE               DeviceType
  )
{
  if ((DeviceType & H2O_FORM_BROWSER_CONSOLE_STO) ||
      (DeviceType & H2O_FORM_BROWSER_CONSOLE_GO)) {
    return TRUE;
  }

  return FALSE;
}

/**
 Adjust console pair if console redirection exist;
 pair SimpleTextOut with UART to text mode display engine

 @param [in,out] DisplayEngineList       Pointer of the display engine list
 @param [in]     EngineListCount         The count of the engine list
 @param [in]     ConsoleDevList          Pointer of the console device list
 @param [in]     ConsoleDevListCount     The count of console device list

 @return EFI_SUCCESS            Adjust console pair success

**/
STATIC
EFI_STATUS
FormsBrowserArrangeConsoles (
  IN OUT H2O_DISPLAY_ENGINE_PROTOCOL              **DisplayEngineList,
  IN UINTN                                        EngineListCount,
  IN H2O_FORM_BROWSER_CONSOLE_DEV                 *ConsoleDevList,
  IN UINT32                                        ConsoleDevListCount
  )
{
  return EFI_SUCCESS;
}

/**
 Add display engine

 @param [in]     Private              Formbrowser private data
 @param [in]     H2ODisplayEngine     H2O diaplay engine instance

 @return EFI_SUCCESS            Adjust console pair success

**/
STATIC
EFI_STATUS
FBAddDisplayEngine (
  IN H2O_FORM_BROWSER_PRIVATE_DATA               *Private,
  IN H2O_DISPLAY_ENGINE_PROTOCOL                 *H2ODisplayEngine
  )
{
  UINTN                       OldBufferSize;
  UINTN                       NewBufferSize;

  OldBufferSize = Private->EngineListCount * sizeof (H2O_DISPLAY_ENGINE_PROTOCOL *);
  NewBufferSize = (Private->EngineListCount + 1) * sizeof (H2O_DISPLAY_ENGINE_PROTOCOL *);
  Private->EngineList = ReallocatePool (OldBufferSize, NewBufferSize,Private->EngineList);
  if (Private->EngineList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Private->EngineList[Private->EngineListCount] = H2ODisplayEngine;
  Private->EngineListCount++;

  return EFI_SUCCESS;
}

/**
 Add those wait for event to input check event list

 @param [in] Private             Form browser pricvate data
 @param [in] Handle              Handle
 @param [in] DevicePath          Device path
 @param [in] DeviceType          Device type

 @return EFI_SUCCESS             Add input check event success

**/
STATIC
EFI_STATUS
FBAddInputCheckEvent (
  IN H2O_FORM_BROWSER_PRIVATE_DATA               *Private,
  IN EFI_HANDLE                                  Handle,
  IN EFI_DEVICE_PATH                             *DevicePath,
  IN H2O_CONSOLE_DEVICE_TYPE                     DeviceType
  )
{
  EFI_EVENT                                     *OldInputEventList;
  H2O_INPUT_EVENT_DESCRIPTION                   *OldInputEventDescList;
  EFI_SIMPLE_TEXT_INPUT_PROTOCOL                *SimpleTextIn;
  EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL             *SimpleTextInEx;
  EFI_SIMPLE_POINTER_PROTOCOL                   *SimplePointer;
  EFI_ABSOLUTE_POINTER_PROTOCOL                 *AbsolutePointer;
  VOID                                          *Protocol;
  EFI_EVENT                                     Event;
  EFI_STATUS                                    Status;

  Event = NULL;
  Protocol = NULL;

  switch (DeviceType){

  case H2O_FORM_BROWSER_CONSOLE_STI:
    Status = gBS->HandleProtocol (
                    Handle,
                    &gEfiSimpleTextInProtocolGuid,
                    (VOID **) &SimpleTextIn
                    );
    if (!EFI_ERROR (Status)) {
      Event = SimpleTextIn->WaitForKey;
      Protocol = (VOID *)SimpleTextIn;
    }
    break;

  case H2O_FORM_BROWSER_CONSOLE_STI2:
    Status = gBS->HandleProtocol (
                    Handle,
                    &gEfiSimpleTextInputExProtocolGuid,
                    (VOID **) &SimpleTextInEx
                    );
    if (!EFI_ERROR (Status)) {
      Event = SimpleTextInEx->WaitForKeyEx;
      Protocol = (VOID *)SimpleTextInEx;
    }
    break;

  case H2O_FORM_BROWSER_CONSOLE_SP:
    Status = gBS->HandleProtocol (
                    Handle,
                    &gEfiSimplePointerProtocolGuid,
                    (VOID **) &SimplePointer
                    );
    if (!EFI_ERROR (Status)) {
      Event = SimplePointer->WaitForInput;
      Protocol = (VOID *)SimplePointer;

    }
    break;

  case H2O_FORM_BROWSER_CONSOLE_AP:
    Status = gBS->HandleProtocol (
                    Handle,
                    &gEfiAbsolutePointerProtocolGuid,
                    (VOID **) &AbsolutePointer
                    );
    if (!EFI_ERROR (Status)) {
      Event = AbsolutePointer->WaitForInput;
      Protocol = (VOID *)AbsolutePointer;
    }
    break;

  default:
    ASSERT (FALSE);
    return EFI_INVALID_PARAMETER;
  }

  if (Event == NULL) {
    return EFI_INVALID_PARAMETER;
  }


  OldInputEventList     = mInputEventList;
  OldInputEventDescList = mInputEventDescList;
  mInputEventList     = (EFI_EVENT*)                   AllocateZeroPool ((mInputEventListCount + 1) * sizeof (EFI_EVENT));
  mInputEventDescList = (H2O_INPUT_EVENT_DESCRIPTION*) AllocateZeroPool ((mInputEventListCount + 1) * sizeof (H2O_INPUT_EVENT_DESCRIPTION));
  if (mInputEventList == NULL || mInputEventDescList == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  CopyMem (mInputEventList    , OldInputEventList    , sizeof (EFI_EVENT) * (UINTN)mInputEventListCount);
  CopyMem (mInputEventDescList, OldInputEventDescList, sizeof (H2O_INPUT_EVENT_DESCRIPTION) * (UINTN)mInputEventListCount);

  mInputEventList[mInputEventListCount] = Event;
  mInputEventDescList[mInputEventListCount].DevicePath = DevicePath;
  mInputEventDescList[mInputEventListCount].Protocol   = Protocol;
  mInputEventDescList[mInputEventListCount].DeviceType = DeviceType;
  mInputEventDescList[mInputEventListCount].ConsoleId  = 0;
  mInputEventDescList[mInputEventListCount].Handle     = Handle;

  mInputEventListCount++;


  FBFreePool ((VOID **)&OldInputEventList);
  FBFreePool ((VOID **)&OldInputEventDescList);

  return EFI_SUCCESS;
}

/**
 Remove device handle from input check event list

 @param [in] Private             Pointer to form browser pricvate data
 @param [in] Handle              The device handle to be processed

 @return EFI_SUCCESS             Remove input check event successfully
 @return EFI_INVALID_PARAMETER   Private is NULL
 @return EFI_NOT_FOUND           There is no event in the list

**/
STATIC
EFI_STATUS
FBRemoveInputCheckEvent (
  IN H2O_FORM_BROWSER_PRIVATE_DATA         *Private,
  IN EFI_HANDLE                            Handle
  )
{
  UINT32                                   Index;
  EFI_TPL                                  OriginalTpl;
  EFI_EVENT                                *OldInputEventList;
  H2O_INPUT_EVENT_DESCRIPTION              *OldInputEventDescList;


  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mInputEventListCount == 0 || mInputEventDescList == NULL) {
    return EFI_NOT_FOUND;
  }

  //
  // When modify mInputEventListCount and mInputEventList, reise TPL to prevent HotPlug Stop function to use these
  //
  OriginalTpl = gBS->RaiseTPL (TPL_CALLBACK);

  Index = 0;
  while (Index < mInputEventListCount) {
    if (mInputEventDescList[Index].Handle == Handle) {
      OldInputEventList     = mInputEventList;
      OldInputEventDescList = mInputEventDescList;

      if (mInputEventListCount >= 2) {
        mInputEventList     = (EFI_EVENT *)                   AllocateZeroPool ((mInputEventListCount - 1) * sizeof (EFI_EVENT));
        mInputEventDescList = (H2O_INPUT_EVENT_DESCRIPTION *) AllocateZeroPool ((mInputEventListCount - 1) * sizeof (H2O_INPUT_EVENT_DESCRIPTION));
        if (mInputEventList == NULL || mInputEventDescList == NULL) {
          mInputEventList = NULL;
          mInputEventDescList = NULL;
          return EFI_OUT_OF_RESOURCES;
        }
        if (Index == 0) {
          CopyMem (mInputEventList    , &OldInputEventList[1]    , (mInputEventListCount - 1) * sizeof (EFI_EVENT));
          CopyMem (mInputEventDescList, &OldInputEventDescList[1], (mInputEventListCount - 1) * sizeof (H2O_INPUT_EVENT_DESCRIPTION));
        } else {
          CopyMem (mInputEventList    , OldInputEventList    , Index * sizeof (EFI_EVENT));
          CopyMem (mInputEventDescList, OldInputEventDescList, Index * sizeof (H2O_INPUT_EVENT_DESCRIPTION));
          CopyMem (&mInputEventList[Index]    , &OldInputEventList[Index + 1]    , (mInputEventListCount - Index - 1) * sizeof (EFI_EVENT));
          CopyMem (&mInputEventDescList[Index], &OldInputEventDescList[Index + 1], (mInputEventListCount - Index - 1) * sizeof (H2O_INPUT_EVENT_DESCRIPTION));
        }
      } else {
        mInputEventList     = NULL;
        mInputEventDescList = NULL;
      }

      FBFreePool ((VOID **) &OldInputEventList);
      FBFreePool ((VOID **) &OldInputEventDescList);

      mInputEventListCount--;
      continue;
    }

    Index++;
  }

  gBS->RestoreTPL (OriginalTpl);

  return EFI_SUCCESS;
}

/**
 Add console device in console device list

 @param [in] Private              Pointer to form browser pricvate data
 @param [in] Handle               The device handle to be processed

 @return A pointer to the added console device data or NULL if it is not a physical device or allocate pool fail

**/
H2O_FORM_BROWSER_CONSOLE_DEV *
FBAddConsoleDevice (
  IN H2O_FORM_BROWSER_PRIVATE_DATA               *Private,
  IN EFI_HANDLE                                  Handle
  )
{
  H2O_FORM_BROWSER_CONSOLE_DEV *ConDev;
  EFI_DEVICE_PATH_PROTOCOL     *DevicePath;
  EFI_STATUS                   Status;
  UINTN                        OldBufferSize;
  UINTN                        NewBufferSize;

  Status = gBS->HandleProtocol (
                  Handle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **) &DevicePath
                  );
  if (EFI_ERROR (Status)) {
    return NULL;
  }
  OldBufferSize = mConsoleDevListCount * sizeof (H2O_FORM_BROWSER_CONSOLE_DEV);
  NewBufferSize = (mConsoleDevListCount + 1) * sizeof (H2O_FORM_BROWSER_CONSOLE_DEV);
  mConsoleDevList = ReallocatePool (OldBufferSize, NewBufferSize, mConsoleDevList);
  if (mConsoleDevList == NULL) {
    return NULL;
  }

  ConDev = &mConsoleDevList[mConsoleDevListCount];
  mConsoleDevListCount++;

  ConDev->Size       = sizeof (H2O_FORM_BROWSER_CONSOLE_DEV);
  ConDev->Name       = DevicePathToAsciiStr (DevicePath);
  ConDev->DevicePath = DevicePath;
  ConDev->Handle     = Handle;
  ConDev->DeviceType = FBGetDeviceType (Handle);
  ConDev->ConsoleId  = H2O_FORM_BROWSER_CONSOLE_NOT_ASSIGNED;
  CopyGuid (&ConDev->DisplayEngine, &gZeroGuid);

  Status = FBAddInputCheckEvent (Private, Handle, DevicePath, H2O_FORM_BROWSER_CONSOLE_STI2);
  if (EFI_ERROR (Status)) {
    FBAddInputCheckEvent (Private, Handle, DevicePath, H2O_FORM_BROWSER_CONSOLE_STI);
  }
  //
  // SetupMouse will produece SimplePointer and AbsolutePointer
  //
  if (Private->SetupMouse == NULL) {
    FBAddInputCheckEvent (Private, Handle, DevicePath, H2O_FORM_BROWSER_CONSOLE_SP);
    FBAddInputCheckEvent (Private, Handle, DevicePath, H2O_FORM_BROWSER_CONSOLE_AP);
  }
  return ConDev;
}

/**
 Remove console device from console device list

 @param [in] Private             Pointer to form browser pricvate data
 @param [in] Handle              The device handle to be processed

 @return EFI_SUCCESS             Add input check event success
 @return EFI_INVALID_PARAMETER   Private is NULL
 @return EFI_NOT_FOUND           There is no device in the list

**/
EFI_STATUS
FBRemoveConsoleDevice (
  IN H2O_FORM_BROWSER_PRIVATE_DATA         *Private,
  IN EFI_HANDLE                            Handle
  )
{
  UINT32                                   Index;
  H2O_FORM_BROWSER_CONSOLE_DEV             *OldConsoleDevList;

  if (Private == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (mConsoleDevListCount == 0 || mConsoleDevList == NULL) {
    return EFI_NOT_FOUND;
  }

  for (Index = 0; Index < mConsoleDevListCount; Index++) {
    if (mConsoleDevList[Index].Handle == Handle) {
      break;
    }
  }
  if (Index == mConsoleDevListCount) {
    return EFI_NOT_FOUND;
  }

  FBRemoveInputCheckEvent (Private, Handle);
  OldConsoleDevList = mConsoleDevList;

  if (mConsoleDevListCount >= 2) {
    mConsoleDevList = (H2O_FORM_BROWSER_CONSOLE_DEV *) AllocateZeroPool ((mConsoleDevListCount - 1) * sizeof (H2O_FORM_BROWSER_CONSOLE_DEV));
    if (Index == 0) {
      CopyMem (mConsoleDevList, &OldConsoleDevList[1], (mConsoleDevListCount - 1) * sizeof (H2O_FORM_BROWSER_CONSOLE_DEV));
    } else {
      CopyMem (mConsoleDevList        , OldConsoleDevList                , Index * sizeof (H2O_FORM_BROWSER_CONSOLE_DEV));
      CopyMem (&mConsoleDevList[Index], &OldConsoleDevList[Index + 1]    , (mConsoleDevListCount - Index - 1) * sizeof (H2O_FORM_BROWSER_CONSOLE_DEV));
    }
  } else {
    mConsoleDevList = NULL;
  }

  FBFreePool ((VOID **) &OldConsoleDevList);
  mConsoleDevListCount--;

  return EFI_SUCCESS;
}

/**
 Free console list

 @param [in] Private             Pointer to form browser private data

 @retval EFI_SUCCESS             Console list in not find
                                 Free console list successfully

**/
EFI_STATUS
FBFreeConsoleDevList (
  IN H2O_FORM_BROWSER_PRIVATE_DATA               *Private
  )
{
  UINT32 Index;

  if (mConsoleDevList == NULL) {
    return EFI_SUCCESS;
  }

  for (Index = 0; Index < mConsoleDevListCount; Index++) {
    if (mConsoleDevList[Index].Name != NULL) {
      FreePool ((VOID *)mConsoleDevList[Index].Name);
    }
  }

  FreePool (mConsoleDevList);
  mConsoleDevList = NULL;
  mConsoleDevListCount = 0;

  return EFI_SUCCESS;
}

/**
 Get device type

 @param [in] Handle              Handle whcih belong to device

 @retval DeviceType              Device type
                                 Free console list successfully

**/
H2O_CONSOLE_DEVICE_TYPE
FBGetDeviceType (
  IN EFI_HANDLE              Handle
  )
{
  UINTN                                        TypeIndex;
  UINT8                                        *DummyProtocol;
  H2O_CONSOLE_DEVICE_TYPE                      DeviceType;
  EFI_STATUS                                   Status;

  DeviceType = H2O_FORM_BROWSER_CONSOLE_SYSTEM;
  for (TypeIndex  = 0; mConsoleMap[TypeIndex].Guid != NULL; TypeIndex++) {
    Status = gBS->HandleProtocol (Handle, mConsoleMap[TypeIndex].Guid, (VOID **) &DummyProtocol);
    if (!EFI_ERROR (Status)) {
      DeviceType |= mConsoleMap[TypeIndex].Type;
    }
    TypeIndex++;
  }

  return DeviceType;
}

/**
 Set activated engine

 @param [in] Private             Formvrwser private data
 @param [in] ConsoleId           Console id

 @retval EFI_SUCCESS             Set activated engine successfully
                                 Free console list successfully

**/
EFI_STATUS
FBSetActivedEngine (
  IN H2O_FORM_BROWSER_PRIVATE_DATA          *Private,
  IN H2O_CONSOLE_ID                         ConsoleId
  )
{
  ASSERT (ConsoleId > 0 && ((UINTN)ConsoleId <= Private->EngineListCount));
  if (ConsoleId <= 0 || ((UINTN)ConsoleId > Private->EngineListCount)) {
    return EFI_INVALID_PARAMETER;
  }

  Private->ActivatedEngine = Private->EngineList[ConsoleId - 1];

  return EFI_SUCCESS;
}

/**
 Install USB enumeration protocol to make sure USB keyboard is initialized.
**/
VOID
NotifyUsbKbInitial (
  )
{
  EFI_STATUS                   Status;
  EFI_HANDLE                   Handle;
  UINT8                        *Instance;

  Status = gBS->LocateProtocol (&gEfiUsbEnumerationGuid, NULL, (VOID **) &Instance);
  if (EFI_ERROR (Status)) {
    Handle = 0;
    gBS->InstallProtocolInterface (
           &Handle,
           &gEfiUsbEnumerationGuid,
           EFI_NATIVE_INTERFACE,
           NULL
           );
  }
}

/**
 Init consoles

 @param [in] Private             Formbrowser private data

 @retval EFI_SUCCESS             Init consoles successfully

**/
EFI_STATUS
FBInitConsoles (
  IN H2O_FORM_BROWSER_PRIVATE_DATA             *Private
  )
{
  EFI_STATUS                                   Status;
  UINTN                                        NumberOfHandle;
  EFI_HANDLE                                   *HandleBuffer;
  H2O_DISPLAY_ENGINE_PROTOCOL                  *H2ODisplayEngine;
  H2O_FORM_BROWSER_CONSOLE_DEV                 *ConsoleDev;
  UINT32                                        Index;
  UINT32                                        Index2;

  HandleBuffer = NULL;

  NotifyUsbKbInitial ();
  FBHotPlugInit ();
  //
  // Setup Mouse
  //
  Status = gBS->LocateProtocol (
                  &gSetupMouseProtocolGuid,
                  NULL,
                  (VOID **) &Private->SetupMouse
                  );

  //
  // Get all Display Engines
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gH2ODisplayEngineProtocolGuid,
                  NULL,
                  &NumberOfHandle,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  for (Index = 0; Index < NumberOfHandle; Index++) {
    Status = gBS->HandleProtocol (
                    HandleBuffer[Index],
                    &gH2ODisplayEngineProtocolGuid,
                    (VOID **) &H2ODisplayEngine
                    );
    if (EFI_ERROR (Status)) {
      goto Error;
    }

    Status = FBAddDisplayEngine (Private, H2ODisplayEngine);
    if (EFI_ERROR (Status)) {
      goto Error;
    }
  }
  FreePool (HandleBuffer);
  HandleBuffer = NULL;

  //
  // Get all console devices
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDevicePathProtocolGuid,
                  NULL,
                  &NumberOfHandle,
                  &HandleBuffer
                  );
  if (EFI_ERROR (Status)) {
    goto Error;
  }
  for (Index = 0; Index < NumberOfHandle; Index++) {
    if (IsConsoleDeviceType (HandleBuffer[Index])) {
      ConsoleDev = FBAddConsoleDevice (Private, HandleBuffer[Index]);
      if (ConsoleDev == NULL) {
        goto Error;
      }
    }
  }
  FreePool (HandleBuffer);
  HandleBuffer = NULL;

  //
  // Only process output console
  //
  for (Index = 0; Index < Private->EngineListCount; Index++) {
    H2ODisplayEngine = Private->EngineList[Index];
    for (Index2 = 0; Index2 < mConsoleDevListCount; Index2++) {
      ConsoleDev = &mConsoleDevList[Index2];
      if (!IsConOutDeviceType (ConsoleDev->DeviceType)) {
        continue;
      }
      if (ConsoleDev->ConsoleId == H2O_FORM_BROWSER_CONSOLE_NOT_ASSIGNED) {
        Status = H2ODisplayEngine->SupportConsole (H2ODisplayEngine, ConsoleDev);
        if (!EFI_ERROR (Status)) {
          ConsoleDev->ConsoleId = (H2O_CONSOLE_ID)(Index + 1);
          CopyGuid (&ConsoleDev->DisplayEngine, &H2ODisplayEngine->Id);
          //
          // assgin one console to one DE, and then assgin remaining to last DE.
          //
          if (Index != (Private->EngineListCount - 1)) {
            break;
          }
        }
      }
    }
  }

  //
  // OEM Kernel Service FormsBrowserArrangeConsoles
  //
  FormsBrowserArrangeConsoles (
    Private->EngineList,
    Private->EngineListCount,
    mConsoleDevList,
    mConsoleDevListCount
    );

  //
  // Only attach output console
  //
  for (Index = 0; Index < mConsoleDevListCount; Index++) {
    ConsoleDev = &mConsoleDevList[Index];
    if (!IsConOutDeviceType (ConsoleDev->DeviceType)) {
      continue;
    }
    if (ConsoleDev->ConsoleId != H2O_FORM_BROWSER_CONSOLE_NOT_ASSIGNED && ConsoleDev->ConsoleId != 0) {
      ASSERT ((UINTN)ConsoleDev->ConsoleId <= Private->EngineListCount);
      H2ODisplayEngine = Private->EngineList[ConsoleDev->ConsoleId - 1];
      Status = H2ODisplayEngine->AttachConsole (H2ODisplayEngine, ConsoleDev);
      if (EFI_ERROR (Status)) {
        goto Error;
      }
    }
  }

  FBSetActivedEngine (Private, (H2O_CONSOLE_ID)1);
  FBConsplitterUpdateModeData ();
  Private->ConsoleInitComplete = TRUE;

  if (Private->SetupMouse != NULL) {
    Private->SetupMouse->Start(Private->SetupMouse);
    Private->SetupMouse->SetMode (Private->SetupMouse, EfiSetupMouseScreenGraphics);
  }

  return EFI_SUCCESS;

Error:

  if (HandleBuffer != NULL) {
    FreePool (HandleBuffer);
  }

  if (EFI_ERROR (Status)) {
    if (Private->EngineList != NULL) {
      FreePool (Private->EngineList);
      Private->EngineList = NULL;
      Private->EngineListCount = 0;
    }
  }

  FBFreeConsoleDevList (Private);

  return Status;
}

/**
 Detach consoles

 @param [in] Private             Formbrowser private data

 @retval EFI_SUCCESS             Detach consoles successfully

**/
EFI_STATUS
FBDetachConsoles (
  IN H2O_FORM_BROWSER_PRIVATE_DATA               *Private
  )
{
  //
  // detach console
  //
  UINT32                                Index;
  UINTN                                Index2;
  EFI_STATUS                           Status;

  if (Private->SetupMouse != NULL) {
    Private->SetupMouse->Close (Private->SetupMouse);
  }

  FBHotPlugShutdown ();

  for (Index = 0; Index < mConsoleDevListCount; Index++) {
    H2O_FORM_BROWSER_CONSOLE_DEV *ConsoleDev;
    H2O_DISPLAY_ENGINE_PROTOCOL  *H2ODisplayEngine;

    ConsoleDev = &mConsoleDevList[Index];
    if (!(ConsoleDev->DeviceType & H2O_FORM_BROWSER_CONSOLE_STO) &&
        !(ConsoleDev->DeviceType & H2O_FORM_BROWSER_CONSOLE_GO)) {
      continue;
    }

    if (ConsoleDev->ConsoleId != 0) {
      for (Index2 = 0; Index2 < Private->EngineListCount; Index2++) {
        H2ODisplayEngine = Private->EngineList[Index2];
        Status = H2ODisplayEngine->DetachConsole (H2ODisplayEngine, ConsoleDev);
      }
    }
  }

  //
  // free all resource and deattach console
  //
  FBFreePool ((VOID **)&Private->EngineList);
  Private->EngineListCount = 0;

  FBFreePool ((VOID **)&mInputEventList);
  FBFreePool ((VOID **)&mInputEventDescList);

  mInputEventListCount = 0;

  FBFreeConsoleDevList (Private);

  Private->ConsoleInitComplete = FALSE;


  return EFI_SUCCESS;
}
/**
 Check whether console of formbrowser is initialized

 @param [in]  ConsoleInitComplete       Check console is init Complete flag
 @param [in]  Private                   Formbrowser private data

**/
VOID
FBIsConsoleInit (
  IN BOOLEAN                             *ConsoleInitComplete,
  IN H2O_FORM_BROWSER_PRIVATE_DATA       **Private
)
{
  *Private = &mFBPrivate;
  *ConsoleInitComplete = (*Private)->ConsoleInitComplete;
}

/**
 Get console information

 @param [in]  ConsoleDevList            Console device list
 @param [in]  ConsoleDevListCount       Console device list count

**/
VOID
GetConsoleInfo (
    H2O_FORM_BROWSER_CONSOLE_DEV **ConsoleDevList,
    UINT32                        *ConsoleDevListCount
)
{
  *ConsoleDevList = mConsoleDevList;
  *ConsoleDevListCount = mConsoleDevListCount;

}
