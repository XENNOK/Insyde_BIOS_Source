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

#include <DisplaySelection.h>

UINT8                                  *mPtrToVbtTable = NULL;
PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO   *mPlugInVgaInfo = NULL;

//[-start-130330-IB05400394-add]//
UINT8 *
HiiCreateSuppressIfOpCode (
  IN VOID  *OpCodeHandle
  )
{
  EFI_IFR_SUPPRESS_IF  OpCode;

  return InternalHiiCreateOpCodeExtended (OpCodeHandle, &OpCode, EFI_IFR_SUPPRESS_IF_OP, sizeof (OpCode), 0, 1);
}

UINT8 *
HiiCreateIdEqualOpCode (
  IN VOID                 *OpCodeHandle,
  IN EFI_QUESTION_ID      QuestionId,
  IN UINT16               Value
  )
{
  EFI_IFR_EQ_ID_VAL  OpCode;

  OpCode.QuestionId = QuestionId;
  OpCode.Value = Value;
  return InternalHiiCreateOpCodeExtended (OpCodeHandle, &OpCode, EFI_IFR_EQ_ID_VAL_OP, sizeof (OpCode), 0, 0);
}

UINT8 *
HiiCreateIdEqualListOpCode (
  IN VOID                 *OpCodeHandle,
  IN EFI_QUESTION_ID      QuestionId,
  IN UINT8                OptionNum,
  IN UINT16               *Value
  )
{
  EFI_IFR_EQ_ID_VAL_LIST  *OpCode;
  UINTN                   OpCodeSize;
  UINT8                   *Buffer;
  UINT8                   Index;


  OpCodeSize = sizeof (EFI_IFR_EQ_ID_VAL_LIST) + sizeof (UINT16) * (OptionNum - 1);
  OpCode = AllocatePool (OpCodeSize);
  if (OpCode == NULL) {
    return NULL;
  }
  
  OpCode->QuestionId = QuestionId;
  OpCode->ListLength = OptionNum;
  for (Index = 0; Index < OptionNum; Index++) {
    OpCode->ValueList[Index] = Value[Index];  
  }
  Buffer = InternalHiiCreateOpCodeExtended (OpCodeHandle, OpCode, EFI_IFR_EQ_ID_VAL_LIST_OP, OpCodeSize, 0, 0);
  FreePool (OpCode);

  return Buffer;
}

EFI_STATUS
HiiCreateOneOfOptionOpCodeEx (
  IN     VOID                 *OpCodeHandle,
  IN     UINTN                OptionCount,
  IN     OPTION_ENTRY         *OptionsList,
  IN     UINT8                Type,
  IN     EFI_QUESTION_ID      QuestionId,
  IN     UINTN                OptionCount2,
  IN     UINT8                *PrimaryTypeList,  
  IN     UINT8                *MapList
  )
{
  UINTN                       Index;
  UINT8                       Index2;
  UINT8                       OptionNum;
  UINT8                       TypeIndex;
  UINT16                      *Value;

  
  
  if ((OptionCount != 0) && (OptionsList == NULL)) {
    return EFI_INVALID_PARAMETER;
  }
  
  for (Index = 0; Index < OptionCount; Index++) {
    TypeIndex = 0;
    for (Index2 = 0; Index2 < OptionCount; Index2++) {
      if (MapList[Index2*2] == OptionsList[Index].Value) {
        TypeIndex = Index2;
        break;
      }
    }
    
    OptionNum = 0;
    for (Index2 = 0; Index2 < OptionCount2; Index2++) {
      if ((MapList[TypeIndex*2+1] & PrimaryTypeList[Index2]) != 0) {
        OptionNum++; 
      }
    }
    
    if (OptionNum > 0) {
      HiiCreateSuppressIfOpCode (OpCodeHandle);
      Value = AllocatePool (OptionNum);
//[-start-140625-IB05080432-add]//
      if (Value == NULL) {
        return EFI_OUT_OF_RESOURCES;
      }
//[-end-140625-IB05080432-add]//
      OptionNum = 0;
      for (Index2 = 0; Index2 < OptionCount2; Index2++) {
        //
        // If this option need to be suppressed
        //
        if ((MapList[TypeIndex*2+1] & PrimaryTypeList[Index2]) != 0) {
          //
          // Gudge if this suppressed option has many conditions
          //
          Value[OptionNum] = PrimaryTypeList[Index2];
          OptionNum++; 
        }
      }
      if (OptionNum > 1) {
        HiiCreateIdEqualListOpCode (OpCodeHandle, QuestionId, OptionNum, Value);
      } else {
        HiiCreateIdEqualOpCode (OpCodeHandle, QuestionId, Value[0]);
      }
      FreePool (Value);
    }

    HiiCreateOneOfOptionOpCode (
      OpCodeHandle,
      OptionsList[Index].StringToken,
      OptionsList[Index].Flags,
      Type,
      (UINT8)OptionsList[Index].Value
      );
    
    if (OptionNum > 0) {
  	  //
  	  // This end opcode is for suppressif
  	  //	
      HiiCreateEndOpCode (OpCodeHandle);
    }
  }
  return EFI_SUCCESS;
}
//[-end-130330-IB05400394-add]//


EFI_DEVICE_PATH_PROTOCOL *
PartMatchInstance (
  IN     EFI_DEVICE_PATH_PROTOCOL  *Multi,
  IN     EFI_DEVICE_PATH_PROTOCOL  *Single
  )
{
  EFI_DEVICE_PATH_PROTOCOL  *Instance;
  UINTN                     InstanceSize;
  UINTN                     SingleDpSize;
  UINTN                     Size;
  EFI_DEVICE_PATH_PROTOCOL  *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL  *OldDevicePath;

  NewDevicePath = NULL;

  if (Multi == NULL || Single == NULL) {
    return FALSE;
  }

  Instance        =  GetNextDevicePathInstance (&Multi, &InstanceSize);
  SingleDpSize    =  GetDevicePathSize (Single) - END_DEVICE_PATH_LENGTH;
  InstanceSize    -= END_DEVICE_PATH_LENGTH;

  while (Instance != NULL) {

    Size = (SingleDpSize < InstanceSize) ? SingleDpSize : InstanceSize;

    if ((CompareMem (Instance, Single, Size) == 0)) {
      OldDevicePath = NewDevicePath;
      NewDevicePath = AppendDevicePathInstance (OldDevicePath, Instance);

      if (OldDevicePath != NULL) {
        gBS->FreePool (OldDevicePath);
      }
    }

    gBS->FreePool (Instance);
    Instance = GetNextDevicePathInstance (&Multi, &InstanceSize);
    InstanceSize  -= END_DEVICE_PATH_LENGTH;
  }

  return NewDevicePath;
}

EFI_STATUS
RemainingDevicePathFromConOutVar (
  IN EFI_HANDLE                               VgaControllerHandle,
  OUT EFI_DEVICE_PATH_PROTOCOL                **RemainingDevicePath
  )
{
  EFI_STATUS                        Status;
  EFI_DEVICE_PATH_PROTOCOL          *ConOutVar;
  UINTN                             ConOutSize;
  EFI_DEVICE_PATH_PROTOCOL          *VideoOutputDevicePath;
  EFI_DEVICE_PATH_PROTOCOL          *ControllerDevicePath;

  ConOutVar = NULL;
  *RemainingDevicePath = NULL;
  VideoOutputDevicePath = NULL;
  //
  // Get last run Acpi_ADR Value from ConOut.
  //
  ConOutVar = GetVariableAndSize (
                L"ConOut",
                &gEfiGlobalVariableGuid,
                &ConOutSize
                );
  if (ConOutVar != NULL) {
    Status = gBS->HandleProtocol (
                    VgaControllerHandle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&ControllerDevicePath
                    );

    VideoOutputDevicePath = PartMatchInstance (ConOutVar, ControllerDevicePath);
    gBS->FreePool (ConOutVar);

    if (VideoOutputDevicePath != NULL) {
      while (!IsDevicePathEnd (VideoOutputDevicePath)) {
        if ((DevicePathType (VideoOutputDevicePath) == ACPI_DEVICE_PATH) &&
            (DevicePathSubType (VideoOutputDevicePath) == ACPI_ADR_DP)) {
          if (DevicePathNodeLength(VideoOutputDevicePath) != sizeof (ACPI_ADR_DEVICE_PATH)) {
            return EFI_SUCCESS;
          } else {
            *RemainingDevicePath = VideoOutputDevicePath;
          }
        }
        VideoOutputDevicePath = NextDevicePathNode (VideoOutputDevicePath);
      }
    }
  }

  return EFI_SUCCESS;
}

ACPI_ADR_DEVICE_PATH *
GetAcpiADRFromDevicePath (
  IN EFI_DEVICE_PATH_PROTOCOL  *DevicePath
  )
{
  ACPI_ADR_DEVICE_PATH         *AcpiADR;

  while (!IsDevicePathEnd (DevicePath)) {
    if ((DevicePathType (DevicePath) == ACPI_DEVICE_PATH) &&
        (DevicePathSubType (DevicePath) == ACPI_ADR_DP)) {
      AcpiADR = (ACPI_ADR_DEVICE_PATH*)DuplicateDevicePath(DevicePath);
      return AcpiADR;
    }
    DevicePath = NextDevicePathNode (DevicePath);
  }

  return NULL;
}

EFI_STATUS
UpdateDisplayPortInfo (
  IN PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO    *VgaControllerInfo
  )
{
  UINTN                            DriverBindingHandleCount;
  EFI_HANDLE                       *DriverBindingHandleBuffer;
  EFI_STATUS                       Status;
  UINTN                            DrvierBindingIndex;
  UINTN                            ChildHandleIndex;
  CHAR16                           *TempControllerName;
  CHAR16                           *TempVideoOutputName;
  CHAR16                           *ControllerName;
  CHAR16                           *VideoOutputName;
  EFI_COMPONENT_NAME2_PROTOCOL     *ComponentNameInterface;
  UINTN                            ChildhandleCount;
  EFI_HANDLE                       *ChildhandleBuffer;
  EFI_DEVICE_PATH_PROTOCOL         *ChildHandleDevicePath;
  EFI_DEVICE_PATH_PROTOCOL         *ControllerDevicePath;
  VIDEO_OUTPUT_PORT_INFO           *TempDisplayPortInfo;
  UINTN                            Index;

  TempVideoOutputName                   = NULL;
  TempControllerName                    = NULL;
  VgaControllerInfo->NumOfConnectedPort = 0;
  ComponentNameInterface                = NULL;
  DriverBindingHandleCount              = 0;
  ChildhandleCount                      = 0;

  Status = gBS->HandleProtocol (
                  VgaControllerInfo->ControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&ControllerDevicePath
                  );
  if (EFI_ERROR(Status)) {
    return EFI_NOT_FOUND;
  }

  //
  // Get Controller Name by DriverBinding Protocol.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiDriverBindingProtocolGuid,
                  NULL,
                  &DriverBindingHandleCount,
                  &DriverBindingHandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  for (DrvierBindingIndex = 0; DrvierBindingIndex < DriverBindingHandleCount; DrvierBindingIndex++) {
    Status = gBS->HandleProtocol (
                    DriverBindingHandleBuffer[DrvierBindingIndex],
                    &gEfiComponentName2ProtocolGuid,
                    (VOID **)&ComponentNameInterface
                    );
    if (!EFI_ERROR(Status)) {
      TempControllerName = NULL;
      Status = ComponentNameInterface->GetControllerName (
                                         ComponentNameInterface,
                                         VgaControllerInfo->ControllerHandle,
                                         NULL,
                                         LANGUAGE_CODE_ENGLISH,
                                         &TempControllerName
                                         );
      if (!EFI_ERROR(Status)) {
        //
        // Update Controller Name.
        //
        ControllerName = AllocateZeroPool (StrSize (TempControllerName));
//[-start-130207-IB10870073-add]//
        ASSERT (ControllerName != NULL);
        if (ControllerName == NULL) {
          return EFI_OUT_OF_RESOURCES;
        }
//[-end-130207-IB10870073-add]//
        StrCpy (ControllerName, TempControllerName);
        VgaControllerInfo->ControllerName = ControllerName;
        break;
      }
    }
  }

  if (DriverBindingHandleCount != 0) {
    gBS->FreePool (DriverBindingHandleBuffer);
  }

  if (EFI_ERROR(Status)) {
    return Status;
  }
  //
  // Get connected display port name.
  //
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiEdidDiscoveredProtocolGuid,
                  NULL,
                  &ChildhandleCount,
                  &ChildhandleBuffer
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  TempDisplayPortInfo = AllocateZeroPool (sizeof(VIDEO_OUTPUT_PORT_INFO) * ChildhandleCount);
//[-start-130207-IB10870073-add]//
  ASSERT (TempDisplayPortInfo != NULL);
  if (TempDisplayPortInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//
  
  for (ChildHandleIndex = 0; ChildHandleIndex < ChildhandleCount; ChildHandleIndex++) {
    Status = gBS->HandleProtocol (
                    ChildhandleBuffer[ChildHandleIndex],
                    &gEfiDevicePathProtocolGuid,
                    (VOID*)&ChildHandleDevicePath
                    );
//[-start-140625-IB05080432-modify]//
    if ((!EFI_ERROR(Status)) && (ComponentNameInterface != NULL)) {
//[-end-140625-IB05080432-modify]//
      if (CompareMem (
              ControllerDevicePath,
              ChildHandleDevicePath,
              GetDevicePathSize (ControllerDevicePath) - END_DEVICE_PATH_LENGTH
              ) == 0) {
        Status = ComponentNameInterface->GetControllerName (
                                           ComponentNameInterface,
                                           VgaControllerInfo->ControllerHandle,
                                           ChildhandleBuffer[ChildHandleIndex],
                                           LANGUAGE_CODE_ENGLISH,
                                           &TempVideoOutputName
                                           );
        if (!EFI_ERROR(Status)) {
          VideoOutputName = AllocateZeroPool (StrSize (TempVideoOutputName));
//[-start-130207-IB10870073-add]//
          ASSERT (VideoOutputName != NULL);
          if (VideoOutputName == NULL) {
            return EFI_OUT_OF_RESOURCES;
          }
//[-end-130207-IB10870073-add]//
          StrCpy (VideoOutputName, TempVideoOutputName);
          TempDisplayPortInfo[ChildHandleIndex].PortName = VideoOutputName;
        } else {
          TempDisplayPortInfo[ChildHandleIndex].PortName = L"\0";
        }
        TempDisplayPortInfo[ChildHandleIndex].PortAcpiADR = GetAcpiADRFromDevicePath (ChildHandleDevicePath);
        VgaControllerInfo->NumOfConnectedPort++;
      }
    }
  }

  Index = 0;
  VgaControllerInfo->ConnectedPortInfo = AllocateZeroPool (sizeof(PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO) * VgaControllerInfo->NumOfConnectedPort);
//[-start-130207-IB10870073-add]//
  ASSERT (VgaControllerInfo->ConnectedPortInfo != NULL);
  if (VgaControllerInfo->ConnectedPortInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//
  for (ChildHandleIndex = 0; ChildHandleIndex < ChildhandleCount; ChildHandleIndex++) {
    if (TempDisplayPortInfo[ChildHandleIndex].PortAcpiADR != NULL) {
      VgaControllerInfo->ConnectedPortInfo[Index].PortName = TempDisplayPortInfo[ChildHandleIndex].PortName;
      VgaControllerInfo->ConnectedPortInfo[Index].PortAcpiADR = TempDisplayPortInfo[ChildHandleIndex].PortAcpiADR;
      Index++;
    }
  }
  gBS->FreePool (TempDisplayPortInfo);
  if (ChildhandleCount != 0) {
    gBS->FreePool (ChildhandleBuffer);
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetAcpiADRFromGopHandle (
  IN    EFI_HANDLE             VgaControllerHandle,
  OUT   ACPI_ADR_DEVICE_PATH   **AcpiADR
  )
{
  EFI_STATUS                 Status;
  EFI_HANDLE                 *GopHandleBuffer;
  UINTN                      GopHandleCount;
  UINTN                      Index;
  EFI_DEVICE_PATH_PROTOCOL   *ControllerDevicePath;
  EFI_DEVICE_PATH_PROTOCOL   *GopDevicePath;

  Status = gBS->HandleProtocol (
                  VgaControllerHandle,
                  &gEfiDevicePathProtocolGuid,
                  (VOID **)&ControllerDevicePath
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  GopHandleCount = 0;
  Status = gBS->LocateHandleBuffer (
                  ByProtocol,
                  &gEfiGraphicsOutputProtocolGuid,
                  NULL,
                  &GopHandleCount,
                  &GopHandleBuffer
                  );
  if (!EFI_ERROR (Status)) {
    //
    // Add all the child handles as possible Console Device
    //
    for (Index = 0; Index < GopHandleCount; Index++) {
      Status = gBS->HandleProtocol (
                      GopHandleBuffer[Index],
                      &gEfiDevicePathProtocolGuid,
                      (VOID*)&GopDevicePath
                      );
      if (EFI_ERROR (Status)) {
        continue;
      }
      
      if (CompareMem (ControllerDevicePath, GopDevicePath, GetDevicePathSize (ControllerDevicePath) - END_DEVICE_PATH_LENGTH) == 0) {
        *AcpiADR = GetAcpiADRFromDevicePath (GopDevicePath);
        gBS->FreePool (GopHandleBuffer);
        return EFI_SUCCESS;
      }
    }

    if (GopHandleCount != 0) {
      gBS->FreePool (GopHandleBuffer);
    }
  }

  return EFI_NOT_FOUND;
}

EFI_STATUS
PlugInVgaUpdateInfo (
  VOID
  )
{
  EFI_HANDLE                               *HandleList;
  EFI_HANDLE                               *PtrToHandle;
  UINTN                                    NumOfHandles;
  UINTN                                    Index;
  PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO     *PtrToList;
  UINTN                                    VarSize;
  EFI_DEVICE_PATH_PROTOCOL                 *RemainingDevicePath;
  EFI_STATUS                               Status;
  EFI_CONSOLE_CONTROL_PROTOCOL             *ConsoleControl;

  if (mPlugInVgaInfo != NULL) {
    return EFI_SUCCESS;
  }

  HandleList = NULL;
  HandleList = GetVariableAndSize (
                 PLUG_IN_VIDEO_DISPLAY_INFO_VAR_NAME,
                 &gEfiGlobalVariableGuid,
                 &VarSize
                 );
  if (HandleList == NULL) {
    return EFI_NOT_FOUND;
  }

  PtrToHandle = HandleList;
  NumOfHandles = 0;
  while (*PtrToHandle != NULL_HANDLE) {
    NumOfHandles++;
    PtrToHandle++;
  }
  if (NumOfHandles == 0) {
    return EFI_NOT_FOUND;
  }
  //
  // Add the NULL PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO as the end of list.
  //
  mPlugInVgaInfo = AllocateZeroPool ((NumOfHandles + 1) * sizeof(PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO));
//[-start-130207-IB10870073-add]//
  ASSERT (mPlugInVgaInfo != NULL);
  if (mPlugInVgaInfo == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
//[-end-130207-IB10870073-add]//
  //
  // Set Mode to the TextMode before re-ConnectController.
  //
  Status = gBS->LocateProtocol (&gEfiConsoleControlProtocolGuid, NULL, (VOID**)&ConsoleControl);
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  Status = ConsoleControl->SetMode (ConsoleControl, EfiConsoleControlScreenText);

  PtrToList = mPlugInVgaInfo;

  for (Index = 0; Index < NumOfHandles; Index++) {

    PtrToList->ControllerHandle = HandleList[Index];
    //
    // Get Original RemainingDevicePath.
    //
    RemainingDevicePathFromConOutVar (PtrToList->ControllerHandle, &RemainingDevicePath);

    Status = gBS->DisconnectController (PtrToList->ControllerHandle, NULL, NULL);
    //
    // Parsing RemainingDeivePath == NULL to get all display prot info.
    //
    gBS->ConnectController (PtrToList->ControllerHandle, NULL, NULL, FALSE);
    UpdateDisplayPortInfo (PtrToList);
    GetAcpiADRFromGopHandle (PtrToList->ControllerHandle, &PtrToList->DefaultAcpiADR);
    gBS->DisconnectController (PtrToList->ControllerHandle, NULL, NULL);
    //
    // Restore original display selection.
    //
    gBS->ConnectController (PtrToList->ControllerHandle, NULL, RemainingDevicePath, TRUE);

    PtrToList++;
  }

  gBS->FreePool (HandleList);

  return Status;
}

VOID
FindDefaultOption (
  IN OUT PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO   *VgaControllerInfo,
  IN  EFI_DEVICE_PATH_PROTOCOL                  *ConOutDevicePath,
  IN  UINTN                                     NumOfOptions,
  OUT EFI_IFR_ONE_OF_OPTION                     *OptionList,
  IN  VOID                                      *OptionsOpCodeHandle
  )
{
  ACPI_ADR_DEVICE_PATH            *AcpiADR;
  UINTN                           Index;

  AcpiADR = NULL;

  while (!IsDevicePathEnd (ConOutDevicePath)) {
    if ((DevicePathType (ConOutDevicePath) == ACPI_DEVICE_PATH) &&
        (DevicePathSubType (ConOutDevicePath) == ACPI_ADR_DP)) {
      if (DevicePathNodeLength(ConOutDevicePath) != sizeof (ACPI_ADR_DEVICE_PATH)){
        VgaControllerInfo->SelectedAcpiADR = VgaControllerInfo->DefaultAcpiADR;
        HiiCreateOneOfOptionOpCode (
            OptionsOpCodeHandle,
            OptionList[NumOfOptions - 1].Option,
            EFI_IFR_OPTION_DEFAULT,
            EFI_IFR_NUMERIC_SIZE_4,
            (UINT32)OptionList[NumOfOptions - 1].Value.u32
        );
        return;
      }
      AcpiADR = (ACPI_ADR_DEVICE_PATH*)ConOutDevicePath;
    }
    ConOutDevicePath = NextDevicePathNode (ConOutDevicePath);
  }

  if (AcpiADR == NULL) {
    return;
  }

  for (Index = 0; Index < NumOfOptions; Index++) {
    if (AcpiADR->ADR == OptionList[Index].Value.u32) {
      VgaControllerInfo->SelectedAcpiADR = VgaControllerInfo->ConnectedPortInfo[Index].PortAcpiADR;
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle,
        OptionList[Index].Option,
        EFI_IFR_OPTION_DEFAULT,
        EFI_IFR_NUMERIC_SIZE_4,
        (UINT32)OptionList[Index].Value.u32
        );    
      return;
    }
  }

  OptionList[NumOfOptions - 1].Flags = EFI_IFR_OPTION_DEFAULT;

  return;
}


EFI_DEVICE_PATH_PROTOCOL *
UpdateDevicePath (
  IN  EFI_DEVICE_PATH_PROTOCOL  *Source,
  IN  EFI_DEVICE_PATH_PROTOCOL  *ControllerDevicePath,
  IN  EFI_DEVICE_PATH_PROTOCOL  *CustomizedConDevicePath
  )
{
  EFI_DEVICE_PATH_PROTOCOL    *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL    *OldDevicePath;
  EFI_DEVICE_PATH_PROTOCOL    *Instance;
  EFI_DEVICE_PATH_PROTOCOL    *PtrToSrc;
  UINTN                       Size;
  EFI_DEVICE_PATH_PROTOCOL    *BufferPtr;


  NewDevicePath = NULL;
  Instance      = NULL;

  PtrToSrc = Source;
  while (PtrToSrc != NULL) {
    Instance = GetNextDevicePathInstance (&PtrToSrc, &Size);
    BufferPtr = Instance;

    if (CompareMem (Instance, ControllerDevicePath, GetDevicePathSize (ControllerDevicePath) - END_DEVICE_PATH_LENGTH) == 0) {
      Instance = CustomizedConDevicePath;
    }

    OldDevicePath = NewDevicePath;
    NewDevicePath = AppendDevicePathInstance (NewDevicePath, Instance);

    if (OldDevicePath != NULL) {
      gBS->FreePool (OldDevicePath);
    }
    gBS->FreePool (BufferPtr);
  }

  return NewDevicePath;
}

EFI_STATUS
PlugInVgaDisplaySelectionSave (
  VOID
  )
{
  EFI_STATUS                               Status;
  EFI_DEVICE_PATH_PROTOCOL                 *ControllerDevicePath;
  EFI_DEVICE_PATH_PROTOCOL                 *NewDevicePath;
  EFI_DEVICE_PATH_PROTOCOL                 *NewConOutVar;
  EFI_DEVICE_PATH_PROTOCOL                 *OldConOutVar;
  UINTN                                    ConOutSize;
  PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO     *PtrToList;
  BOOLEAN                                   UpdateConOutVar;  


  if (mPlugInVgaInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  PtrToList = mPlugInVgaInfo;
  NewDevicePath = NULL;
  UpdateConOutVar = FALSE;
  Status = EFI_UNSUPPORTED;

  NewConOutVar = GetVariableAndSize (
                   L"ConOut",
                   &gEfiGlobalVariableGuid,
                   &ConOutSize
                   );

  while (PtrToList->ControllerHandle != NULL_HANDLE) {

    Status = gBS->HandleProtocol (
                    PtrToList->ControllerHandle,
                    &gEfiDevicePathProtocolGuid,
                    (VOID **)&ControllerDevicePath
                    );

    if (!EFI_ERROR(Status)&& (PtrToList->SelectedAcpiADR != NULL)) {
      NewDevicePath = AppendDevicePathNode (ControllerDevicePath, (EFI_DEVICE_PATH_PROTOCOL*)PtrToList->SelectedAcpiADR);
      OldConOutVar = NewConOutVar;
      NewConOutVar = UpdateDevicePath (OldConOutVar, ControllerDevicePath, NewDevicePath);
	    UpdateConOutVar =TRUE;
      gBS->FreePool (OldConOutVar);
    }

    PtrToList++;
  }

  if (UpdateConOutVar) {
    ConOutSize = GetDevicePathSize (NewConOutVar);
    Status = gRT->SetVariable (
                    L"ConOut",
                    &gEfiGlobalVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS | EFI_VARIABLE_NON_VOLATILE,
                    ConOutSize,
                    NewConOutVar
                    );
  }
  
  return Status;
}

EFI_STATUS
PlugInVideoDisplaySelectionOption (
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  EFI_IFR_TYPE_VALUE                     *Value
  )
{
  PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO     *PtrToList;
  UINTN                                    Index;

  if (mPlugInVgaInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  PtrToList = mPlugInVgaInfo + (QuestionId - KEY_PLUG_IN_DISPLAY_SELECTION1);

  if (Value->u32 == 0) {
    PtrToList->SelectedAcpiADR = PtrToList->DefaultAcpiADR;
  } else {
    for (Index = 0; Index < PtrToList->NumOfConnectedPort; Index++) {
      if (PtrToList->ConnectedPortInfo[Index].PortAcpiADR->ADR == Value->u32)
       PtrToList->SelectedAcpiADR = PtrToList->ConnectedPortInfo[Index].PortAcpiADR;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
PlugInVideoDisplaySelectionLabel (
  IN  EFI_HII_HANDLE                      HiiHandle
  )
{
  EFI_STATUS                                Status;
  EFI_IFR_ONE_OF_OPTION                     *OptionList;  
  UINTN                                     OptionsConut;
  PLUG_IN_ACTIVE_VIDEO_CONTROLLER_INFO      *PtrToList;
  UINTN                                     ControllerIndex;
  UINTN                                     ChildHandleIndex;
  EFI_STRING_ID                             ControllerStringId;
  EFI_STRING_ID                             ChildStringId;
  EFI_QUESTION_ID                           QuestionId;
  EFI_DEVICE_PATH_PROTOCOL                  *ConOutVar;
  UINTN                                     ConOutSize;
  EFI_DEVICE_PATH_PROTOCOL                  *MatchConOutDevice;
  EFI_DEVICE_PATH_PROTOCOL                  *ControllerDevicePath;
  EFI_STRING_ID                             HelpStringId;
  CHAR16                                    *PluginHelpString = \
                                             L"Display Selection for Plug-in Vga Controller, Only supported Uefi Boot mode";

  VOID                                      *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                        *StartLabel;
  VOID                                      *OptionsOpCodeHandle;

  if (mPlugInVgaInfo == NULL) {
    return EFI_UNSUPPORTED;
  }

  ConOutVar = NULL;  
  ConOutVar = GetVariableAndSize (
                L"ConOut",
                &gEfiGlobalVariableGuid,
                &ConOutSize
                );
  if (ConOutVar == NULL) {
    return EFI_NOT_FOUND;
  }

  PtrToList = mPlugInVgaInfo;
  ControllerIndex = 0;
  while (PtrToList->ControllerHandle != NULL_HANDLE) {
    StartOpCodeHandle = HiiAllocateOpCodeHandle ();
    ASSERT (StartOpCodeHandle != NULL);

    OptionsOpCodeHandle= HiiAllocateOpCodeHandle ();
    ASSERT ( OptionsOpCodeHandle != NULL);
  
    StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
//[-start-130330-IB05400394-modify]//
    StartLabel->Number       = PLUG_IN_DISPLAY_SELECTION_START_LABEL;  
//[-end-130330-IB05400394-modify]//

  
    Status = gBS->HandleProtocol (
                    PtrToList->ControllerHandle, 
                    &gEfiDevicePathProtocolGuid, 
                    (VOID **)&ControllerDevicePath
                    );
    
    MatchConOutDevice = PartMatchInstance (ConOutVar, ControllerDevicePath);
    if (MatchConOutDevice == NULL) {
      PtrToList++;
      ControllerIndex++;
      continue;
    }
    //
    // Add the Controller Name.
    //
    ControllerStringId=HiiSetString (HiiHandle, 0, PtrToList->ControllerName, NULL);
    HelpStringId=HiiSetString (HiiHandle, 0, PluginHelpString, NULL);
    //
    // Add the Video Output Ports.
    //
    OptionsConut = (PtrToList->NumOfConnectedPort + 1);
    OptionList = AllocateZeroPool (sizeof(EFI_IFR_ONE_OF_OPTION) * OptionsConut);

    for (ChildHandleIndex = 0; ChildHandleIndex < PtrToList->NumOfConnectedPort; ChildHandleIndex++) {
      ChildStringId=HiiSetString (HiiHandle, 0, PtrToList->ConnectedPortInfo[ChildHandleIndex].PortName, NULL);
      OptionList[ChildHandleIndex].Option    = ChildStringId;
      OptionList[ChildHandleIndex].Value.u32    = PtrToList->ConnectedPortInfo[ChildHandleIndex].PortAcpiADR->ADR;
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle,
        ChildStringId,
        EFI_IFR_TYPE_NUM_SIZE_32,
        EFI_IFR_NUMERIC_SIZE_4,
        (UINT32)PtrToList->ConnectedPortInfo[ChildHandleIndex].PortAcpiADR->ADR
      );
    }

    ChildStringId=HiiSetString (HiiHandle, 0, L"Default", NULL);
    OptionList[OptionsConut - 1].Option = ChildStringId;
    OptionList[OptionsConut - 1].Value.u32    = 0;
    HiiCreateOneOfOptionOpCode (
      OptionsOpCodeHandle,
      ChildStringId,
      EFI_IFR_TYPE_NUM_SIZE_32,
      EFI_IFR_NUMERIC_SIZE_4,
      (UINT32)0
      );

    FindDefaultOption (PtrToList, MatchConOutDevice, OptionsConut, OptionList, OptionsOpCodeHandle);
    

    QuestionId = (EFI_QUESTION_ID)(KEY_PLUG_IN_DISPLAY_SELECTION1 + ControllerIndex);

    HiiCreateOneOfOpCode (
      StartOpCodeHandle,
      QuestionId,
      INVALID_VARSTORE_ID,
      0,
      ControllerStringId,
      HelpStringId,
      0x04,
      EFI_IFR_NUMERIC_SIZE_4,
      OptionsOpCodeHandle,
      NULL
      );
    
    HiiUpdateForm (
      HiiHandle,
      NULL, 
      (EFI_FORM_ID)VIDEO_CONFIG_FORM_ID,
      StartOpCodeHandle,
      NULL
      );
    HiiFreeOpCodeHandle (StartOpCodeHandle);
    HiiFreeOpCodeHandle (OptionsOpCodeHandle);
    gBS->FreePool (MatchConOutDevice);
    
    PtrToList++;
    ControllerIndex++;
  }

    gBS->FreePool (ConOutVar);

  return EFI_SUCCESS;
}

EFI_STATUS
IgdGetVbtTable (
  OUT   UINT8    **PtrToVbt
  )
{
  EFI_STATUS                    Status;
  PLATFORM_GOP_POLICY_PROTOCOL  *PlatformGopPolicy;
  EFI_PHYSICAL_ADDRESS          VbtTable;
  UINT32                        VbtSize;

  if (mPtrToVbtTable != NULL) {
    *PtrToVbt = mPtrToVbtTable;
    return EFI_SUCCESS;
  }

  Status = gBS->LocateProtocol (
                  &gPlatformGopPolicyProtocolGuid,
                  NULL,
                  (VOID **)&PlatformGopPolicy
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = PlatformGopPolicy->GetVbtData (&VbtTable, &VbtSize);
  if (EFI_ERROR(Status)) {
    return Status;
  }

  mPtrToVbtTable = (UINT8*) (UINTN) VbtTable;
  *PtrToVbt      = mPtrToVbtTable;

  return Status;
}

UINT8
IgdVbtDisplayTypeToSCU (
  IN UINT8               VbtDisplayTypeValue
  )
{
  switch (VbtDisplayTypeValue) {

  case VBT_CHILD_DEVICE_TYPE_NA:
    return SCU_IGD_BOOT_TYPE_DISABLE;
    break;

  case VBT_CHILD_DEVICE_TYPE_LFP:
    return SCU_IGD_BOOT_TYPE_LFP;
    break;

  case VBT_CHILD_DEVICE_TYPE_CRT:
    return SCU_IGD_BOOT_TYPE_CRT;
    break;

  case VBT_CHILD_DEVICE_TYPE_EFP:
    return SCU_IGD_BOOT_TYPE_EFP;
    break;

  case VBT_CHILD_DEVICE_TYPE_EFP2:
    return SCU_IGD_BOOT_TYPE_EFP2;
    break;

  case VBT_CHILD_DEVICE_TYPE_EFP3:
    return SCU_IGD_BOOT_TYPE_EFP3;
    break;

  default:
    return SCU_IGD_BOOT_TYPE_DISABLE;
    break;
  }
}

//[-start-120216-IB09960004-add]//
UINT8
IgdVbtDisplayTypeToIndex (
  IN UINT8               VbtDisplayTypeValue
  )
{
  switch (VbtDisplayTypeValue) {
    case VBT_CHILD_DEVICE_TYPE_NA:
      return SCU_IGD_INDEX_DISABLE;
      break;
    case VBT_CHILD_DEVICE_TYPE_LFP:
      return SCU_IGD_INDEX_LFP;
      break;
    case VBT_CHILD_DEVICE_TYPE_CRT:
      return SCU_IGD_INDEX_CRT;
      break;
    case VBT_CHILD_DEVICE_TYPE_EFP:
      return SCU_IGD_INDEX_EFP;
      break;
    case VBT_CHILD_DEVICE_TYPE_EFP2:
      return SCU_IGD_INDEX_EFP2;
      break;
    case VBT_CHILD_DEVICE_TYPE_EFP3:
      return SCU_IGD_INDEX_EFP3;
      break;
    default:
      return SCU_IGD_INDEX_DISABLE;
      break;
  }
}
//[-end-120216-IB09960004-add]//

//[-start-120216-IB09960004-modify]//
BOOLEAN
IgdIsChildListSupported (
  IN   UINT8              *IgdVbtTable,
  IN   UINT8              SCUDisplayType,
  IN   BOOLEAN            PrimaryOrSecondary
//  IN   UINT8              SCUPrimaryDisplayType,
//  IN   UINT8              SCUSecondaryDisplayType
  )
{
  UINTN              Index;
  UINT8              VbtPrimaryType;
  UINT8              VbtSecondaryType;
  UINTN              VbtOffset;

  VbtOffset = VBT_CHILD_DEVICE_1_OFFSET;

  for (Index = 0; Index < VBT_MAX_NUM_OF_CHILD_DEVICE; Index++) {
    if (PrimaryOrSecondary) {
      VbtPrimaryType = IgdVbtTable[VbtOffset];
      if (SCUDisplayType == IgdVbtDisplayTypeToSCU (VbtPrimaryType)) {
        return TRUE;
      }
    } else {
      VbtSecondaryType = IgdVbtTable[VbtOffset+1];
      if (SCUDisplayType == IgdVbtDisplayTypeToSCU (VbtSecondaryType)) {
        return TRUE;
      }
    }
//    if ((SCUPrimaryDisplayType == IgdVbtDisplayTypeToSCU (VbtPrimaryType)) &&
//        (SCUSecondaryDisplayType == IgdVbtDisplayTypeToSCU (VbtSecondaryType))) {
//      return TRUE;
//    }
    VbtOffset += VBT_NEXT_CHILD_DEVICE;
  }

  return FALSE;
}
//[-end-120216-IB09960004-modify]//

//[-start-120216-IB09960004-add]//
VOID
VbtTableToOptionMap (
  IN      UINT8               *IgdVbtTable,
  IN      UINTN               PrimaryOptionsCount,
  IN      UINTN               SecondaryOptionsCount,
  IN OUT  IGD_SCU_OPTION_MAP  *DisplayOptionMap,
  IN OUT  UINT8               *PrimaryTypeList,
  IN OUT  UINT8               *MapList
  )
{
  UINTN              Index;
  UINT8              Index2;
  UINT8              VbtPrimaryType;
  UINT8              VbtSecondaryType;
  UINT8              VbtPrimaryIndex;
  UINT8              VbtSecondaryIndex;
  UINTN              VbtOffset;

  VbtOffset = VBT_CHILD_DEVICE_1_OFFSET;

  for (Index = 0; Index < VBT_MAX_NUM_OF_CHILD_DEVICE; Index++) {
    VbtPrimaryType = IgdVbtTable[VbtOffset];
    VbtSecondaryType = IgdVbtTable[VbtOffset+1];
    VbtPrimaryIndex = IgdVbtDisplayTypeToIndex (VbtPrimaryType);
    VbtSecondaryIndex = IgdVbtDisplayTypeToIndex (VbtSecondaryType);

    PrimaryTypeList[VbtPrimaryIndex] = VbtPrimaryType;
    DisplayOptionMap[VbtSecondaryIndex].SecondaryType = VbtSecondaryType;
    DisplayOptionMap[VbtSecondaryIndex].PrimaryType = DisplayOptionMap[VbtSecondaryIndex].PrimaryType | VbtPrimaryType;

    VbtOffset += VBT_NEXT_CHILD_DEVICE;
  }
  for (Index = 0; Index < SecondaryOptionsCount; Index++) {
    MapList[Index*2] = DisplayOptionMap[Index].SecondaryType;

    for (Index2 = 0; Index2 < PrimaryOptionsCount; Index2++) {
      if ((DisplayOptionMap[Index].PrimaryType & PrimaryTypeList[Index2]) == 0) {
        MapList[Index*2+1] = MapList[Index*2+1] | PrimaryTypeList[Index2];
      }
    }
  }
}
//[-end-120216-IB09960004-add]//

VOID
IgdCreateEfiModeDisplayOption (
  CHIPSET_CONFIGURATION  *MyIfrNVData,
  UINT8                 *IgdVbtTable,
  UINT8                 PrimaryDisplayType,
  UINT8                 SecondaryDisplayType,
  VOID                  *StartOpCodeHandle,
  VOID                  *StartOpCodeHandle2,
  VOID                  *OptionsOpCodeHandle,
  VOID                  *OptionsOpCodeHandle2
  )
{
  UINTN                                     Index;
  UINTN                                     InfoIndex;
  UINTN                                     PrimaryOptionsCount;
  UINTN                                     SecondaryOptionsCount;
  CHIPSET_CONFIGURATION                      SetupNvData;
  UINT16                                    VarOffset;
  IGD_SCU_OPTION_INFO                       DisplayOptionInfo[] = {{STRING_TOKEN(STR_CRT_TEXT), SCU_IGD_BOOT_TYPE_CRT},
                                                                   {STRING_TOKEN(STR_LFP_TEXT), SCU_IGD_BOOT_TYPE_LFP},
                                                                   {STRING_TOKEN(STR_EFP_TEXT), SCU_IGD_BOOT_TYPE_EFP},
                                                                   {STRING_TOKEN(STR_EFP2_TEXT), SCU_IGD_BOOT_TYPE_EFP2},
                                                                   {STRING_TOKEN(STR_EFP3_TEXT), SCU_IGD_BOOT_TYPE_EFP3},
                                                                   {0xFF,0xFF}
                                                                  };
//[-start-120216-IB09960004-add]//
  IGD_SCU_OPTION_MAP                        *DisplayOptionMap;
  UINT8                                     *PrimaryTypeList;
  UINT8                                     *MapList;
//[-end-120216-IB09960004-add]//
//[-start-130401-IB05400394-add]//
  OPTION_ENTRY                              *SecondaryOptionList;
//[-end-130401-IB05400394-add]//

  PrimaryOptionsCount = 0;
  InfoIndex = 0;
  while (DisplayOptionInfo[InfoIndex].SCUDisplayType != 0xFF) {
    //
    // Vbt Table is NULL, skip the checking of child list.
    // List all of the display options on SCU.
    //
    if (IgdVbtTable == NULL) {
      PrimaryOptionsCount++;
    } else {
      //
      // Check the Vbt Table, To find the supporting of child list.
      //
      if (IgdIsChildListSupported (
            IgdVbtTable,
            DisplayOptionInfo[InfoIndex].SCUDisplayType,
//[-start-120216-IB09960004-modify]//
//            SCU_IGD_BOOT_TYPE_DISABLE
            TRUE
//[-end-120216-IB09960004-modify]//
          )){
        PrimaryOptionsCount++;
      }
    }
    InfoIndex++;
  }

  PrimaryOptionsCount = PrimaryOptionsCount + 1; // Add the default setting.
//[-start-120216-IB09960004-modify]//
//  if (PrimaryDisplayType == SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT) {
//  }
//[-end-120216-IB09960004-modify]//
  HiiCreateOneOfOptionOpCode (
    OptionsOpCodeHandle,
    STRING_TOKEN(STR_VBIOS_DEFAULT_TEXT),
    EFI_IFR_OPTION_DEFAULT,
    EFI_IFR_NUMERIC_SIZE_1,
    (UINT8)SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT
    );

  InfoIndex = 0;
  Index = 1;
  while (DisplayOptionInfo[InfoIndex].SCUDisplayType != 0xFF) {
    //
    // Vbt Table is NULL, skip the checking of child list.
    // List all of the display options on SCU.
    //
    if (IgdVbtTable == NULL) {
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle,
        DisplayOptionInfo[InfoIndex].StringId,
        EFI_IFR_TYPE_NUM_SIZE_8,
        EFI_IFR_NUMERIC_SIZE_1,
        DisplayOptionInfo[InfoIndex].SCUDisplayType
        );
//[-start-120216-IB09960004-modify]//
//      if (DisplayOptionInfo[InfoIndex].SCUDisplayType == PrimaryDisplayType) {
//        PrimaryOptionList[Index].Flags = EFI_IFR_OPTION_DEFAULT;
//      }
//[-end-120216-IB09960004-modify]//
      Index++;
    } else {
      //
      // Check the Vbt Table, To find the supporting of child list.
      //
      if (IgdIsChildListSupported(
            IgdVbtTable,
            DisplayOptionInfo[InfoIndex].SCUDisplayType,
//[-start-120216-IB09960004-modify]//
//            SCU_IGD_BOOT_TYPE_DISABLE
            TRUE
          )) {
        HiiCreateOneOfOptionOpCode (
          OptionsOpCodeHandle,
          DisplayOptionInfo[InfoIndex].StringId,
          EFI_IFR_TYPE_NUM_SIZE_8,
          EFI_IFR_NUMERIC_SIZE_1,
          DisplayOptionInfo[InfoIndex].SCUDisplayType
          );
//         if (DisplayOptionInfo[InfoIndex].SCUDisplayType == PrimaryDisplayType) {
//           PrimaryOptionList[Index].Flags = EFI_IFR_OPTION_DEFAULT;
//         }
//[-end-120216-IB09960004-modify]//
         Index++;
      }
    }
    InfoIndex++;
  }

  VarOffset = (UINT16)((UINTN)(&SetupNvData.IGDBootType)-(UINTN)(&SetupNvData));

  HiiCreateOneOfOpCode (
    StartOpCodeHandle,
    KEY_IGD_PRIMARY_DISPLAY_SELECTION,
    CONFIGURATION_VARSTORE_ID,
    VarOffset,
    (STRING_REF)STRING_TOKEN(STR_IGD_BOOT_TYPE_STRING),
    (STRING_REF)STRING_TOKEN(STR_IGD_BOOT_TYPE_HELP),
    0x04,
    EFI_IFR_NUMERIC_SIZE_1,
    OptionsOpCodeHandle,
    NULL
    );
       
//[-start-120216-IB09960004-modify]//
//  gBS->FreePool (PrimaryOptionList);
  //
  // Run Default setting, only show the Primary options.
  //
//  if (PrimaryDisplayType == SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT) {
//    if (MyIfrNVData != NULL) {
//      MyIfrNVData->IGDBootTypeSecondary = SCU_IGD_BOOT_TYPE_DISABLE;
//    }        
//    return Status;
//  }
//[-end-120216-IB09960004-modify]//

  SecondaryOptionsCount = 0;
  InfoIndex = 0;  
  while (DisplayOptionInfo[InfoIndex].SCUDisplayType != 0xFF) {
    if (IgdVbtTable == NULL) {
      SecondaryOptionsCount++;
    } else {    
      if (IgdIsChildListSupported(
            IgdVbtTable,
//[-start-120216-IB09960004-modify]//
//            PrimaryDisplayType,
            DisplayOptionInfo[InfoIndex].SCUDisplayType,
            FALSE
//[-end-120216-IB09960004-modify]//
          )){
        SecondaryOptionsCount++;
      }
    }
    InfoIndex++;
  }
  
//[-start-120216-IB09960004-modify]//
  //
  // No match Secondary display.
  //
//  if (SecondaryOptionsCount == 0) {
//    if (MyIfrNVData != NULL) {
//      MyIfrNVData->IGDBootTypeSecondary = SCU_IGD_BOOT_TYPE_DISABLE;
//    }
//    return Status;
//  }
//[-end-120216-IB09960004-modify]//

  SecondaryOptionsCount = SecondaryOptionsCount + 1; // Add the default setting.
//[-start-120216-IB09960004-modify]//
//  if (SecondaryDisplayType == SCU_IGD_BOOT_TYPE_DISABLE) {
//  }  
//[-start-130401-IB05400394-modify]//
  SecondaryOptionList = NULL;
  if (IgdVbtTable == NULL) {
    HiiCreateOneOfOptionOpCode (
      OptionsOpCodeHandle2,
      STRING_TOKEN(STR_DISABLED_TEXT),
      EFI_IFR_OPTION_DEFAULT,
      EFI_IFR_NUMERIC_SIZE_1,
      (UINT8)SCU_IGD_BOOT_TYPE_DISABLE
      );
  } else {  
    SecondaryOptionList = AllocateZeroPool (sizeof(OPTION_ENTRY) * SecondaryOptionsCount);
//[-start-140625-IB05080432-add]//
    if (SecondaryOptionList == NULL) {
      return;
    }
//[-end-140625-IB05080432-add]//
    SecondaryOptionList[0].StringToken = STRING_TOKEN(STR_DISABLED_TEXT);
    SecondaryOptionList[0].Value = SCU_IGD_BOOT_TYPE_DISABLE;
    SecondaryOptionList[0].Flags = EFI_IFR_OPTION_DEFAULT;
  }
//[-end-130401-IB05400394-modify]//


  //
  // Check current secondary display value match Primary Display.
  //
//  if ((IgdVbtTable != NULL) &&
//      !(IgdIsChildListSupported (IgdVbtTable, PrimaryDisplayType, SecondaryDisplayType))
//      ) {   
//    if (MyIfrNVData != NULL) {
//      MyIfrNVData->IGDBootTypeSecondary = SCU_IGD_BOOT_TYPE_DISABLE;
//    }
//    SecondaryOptionList[0].Flags = EFI_IFR_OPTION_DEFAULT;
//  }
//[-end-120216-IB09960004-modify]//
  
  InfoIndex = 0;
  Index = 1;
  while (DisplayOptionInfo[InfoIndex].SCUDisplayType != 0xFF) {
    if (IgdVbtTable == NULL) {
      HiiCreateOneOfOptionOpCode (
        OptionsOpCodeHandle2,
        DisplayOptionInfo[InfoIndex].StringId,
        EFI_IFR_TYPE_NUM_SIZE_8,
        EFI_IFR_NUMERIC_SIZE_1,
        (UINT8)DisplayOptionInfo[InfoIndex].SCUDisplayType
        );
//[-start-120216-IB09960004-modify]//
//      if (DisplayOptionInfo[InfoIndex].SCUDisplayType == SecondaryDisplayType) {
//        SecondaryOptionList[Index].Flags = EFI_IFR_OPTION_DEFAULT;
//      }
//[-end-120216-IB09960004-modify]//
      Index++;
    } else {
//[-start-130401-IB05400394-modify]//
      if (IgdIsChildListSupported(
            IgdVbtTable,
//[-start-120216-IB09960004-modify]//
//            PrimaryDisplayType,
            DisplayOptionInfo[InfoIndex].SCUDisplayType,
            FALSE
            )){
        SecondaryOptionList[Index].StringToken  = DisplayOptionInfo[InfoIndex].StringId;
        SecondaryOptionList[Index].Value = DisplayOptionInfo[InfoIndex].SCUDisplayType; 
//        HiiCreateOneOfOptionOpCode (
//          OptionsOpCodeHandle2,
//          DisplayOptionInfo[InfoIndex].StringId,
//          EFI_IFR_TYPE_NUM_SIZE_8,
//          EFI_IFR_NUMERIC_SIZE_1,
//          (UINT8)DisplayOptionInfo[InfoIndex].SCUDisplayType
//          );
//        if (DisplayOptionInfo[InfoIndex].SCUDisplayType == SecondaryDisplayType) {
//          SecondaryOptionList[Index].Flags = EFI_IFR_OPTION_DEFAULT;
//        }
//[-end-120216-IB09960004-modify]//
//[-end-130401-IB05400394-modify]//
        Index++;
      }
    }
    InfoIndex++;
  }
    
//[-start-120216-IB09960004-add]//
  //
  // Sequentially create OpCodes of suppressif, idequal, oneof and endif.
  //
//jdebug  CreateSuppressIfCode (UpdateData);
  
//jdebug  CreateIdEqualCode (KEY_IGD_PRIMARY_DISPLAY_SELECTION, SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT, UpdateData);

//[-start-130330-IB05400394-add]//
  HiiCreateSuppressIfOpCode (StartOpCodeHandle);
  HiiCreateIdEqualOpCode (StartOpCodeHandle, KEY_IGD_PRIMARY_DISPLAY_SELECTION, SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT);
//[-end-130330-IB05400394-add]//
  
  VarOffset = (UINT16)((UINTN)(&SetupNvData.IGDBootTypeSecondary)-(UINTN)(&SetupNvData));

  if (IgdVbtTable != NULL) {
    //  DisplayOptionMap = AllocateZeroPool (sizeof (IGD_SCU_OPTION_MAP) * PrimaryOptionsCount);
    DisplayOptionMap = AllocateZeroPool (sizeof (IGD_SCU_OPTION_MAP) * SecondaryOptionsCount );
//[-start-130207-IB10870073-add]//
    ASSERT (DisplayOptionMap != NULL);
    if (DisplayOptionMap == NULL) {
      return;
    }
//[-end-130207-IB10870073-add]//
    PrimaryTypeList = AllocateZeroPool (sizeof (UINT8) * PrimaryOptionsCount);
//[-start-130207-IB10870073-add]//
    ASSERT (PrimaryTypeList != NULL);
    if (PrimaryTypeList == NULL) {
      return;
    }
//[-end-130207-IB10870073-add]//
    MapList = AllocateZeroPool (sizeof (UINT8) * SecondaryOptionsCount * 2);
//[-start-130207-IB10870073-add]//
    ASSERT (MapList != NULL);
    if (MapList == NULL) {
      return;
    }
//[-end-130207-IB10870073-add]//
    VbtTableToOptionMap (
      IgdVbtTable, 
      PrimaryOptionsCount,
      SecondaryOptionsCount, 
      DisplayOptionMap, 
      PrimaryTypeList,
      MapList
      );
    gBS->FreePool (DisplayOptionMap);
//jdebug    
//    Status = CreateOneOfOpCodeEx (
//               KEY_IGD_SECONDARY_DISPLAY_SELECTION,
//               CONFIGURATION_VARSTORE_ID,
//               VarOffset,
//               (STRING_REF)STRING_TOKEN(STR_IGD_BOOT_TYPE2_STRING),
//               (STRING_REF)STRING_TOKEN(STR_IGD_BOOT_TYPE2_HELP),
//               0,
//               EFI_IFR_NUMERIC_SIZE_1,
//               SecondaryOptionsCount, 
//               SecondaryOptionList,
//               KEY_IGD_PRIMARY_DISPLAY_SELECTION,
//               PrimaryOptionsCount, 
//               PrimaryTypeList,
//               MapList,
//               UpdateData
//               );

//[-start-130330-IB05400394-add]//
    HiiCreateOneOfOptionOpCodeEx (
      OptionsOpCodeHandle2,
      SecondaryOptionsCount,
      SecondaryOptionList,
      EFI_IFR_NUMERIC_SIZE_1,
      KEY_IGD_PRIMARY_DISPLAY_SELECTION,
      PrimaryOptionsCount,
      PrimaryTypeList,
      MapList
      );

    HiiCreateOneOfOpCode (
      StartOpCodeHandle,
      KEY_IGD_SECONDARY_DISPLAY_SELECTION,
      CONFIGURATION_VARSTORE_ID,
      VarOffset,
      (STRING_REF)STRING_TOKEN(STR_IGD_BOOT_TYPE2_STRING),
      (STRING_REF)STRING_TOKEN(STR_IGD_BOOT_TYPE2_HELP),
      0,
      EFI_IFR_NUMERIC_SIZE_1,
      OptionsOpCodeHandle2,
      NULL
      );
    FreePool (SecondaryOptionList);
//[-end-130330-IB05400394-add]//

    gBS->FreePool (PrimaryTypeList);
    gBS->FreePool (MapList);
  } else {
//[-start-130330-IB05400394-modify]//
    HiiCreateOneOfOpCode (
      StartOpCodeHandle,
      KEY_IGD_SECONDARY_DISPLAY_SELECTION,
      CONFIGURATION_VARSTORE_ID,
      VarOffset,
      (STRING_REF)STRING_TOKEN(STR_IGD_BOOT_TYPE2_STRING),
      (STRING_REF)STRING_TOKEN(STR_IGD_BOOT_TYPE2_HELP),
      0,
      EFI_IFR_NUMERIC_SIZE_1,
      OptionsOpCodeHandle2,
      NULL
      );
//[-end-130330-IB05400394-modify]//
  }
//[-start-130330-IB05400394-add]//
  HiiCreateEndOpCode (StartOpCodeHandle);
//[-end-130330-IB05400394-add]//
//jdebug  Status = CreateEndOpCode (UpdateData);
//[-end-120216-IB09960004-add]//

}

//[-start-130401-IB05400394-modify]//
VOID
IgdCreateCSMSupportedDisplayOption (
  IN  UINT8                 PrimaryDisplayType,
  IN  UINT8                 SecondaryDisplayType,
  IN  VOID                  *StartOpCodeHandle,
  IN  VOID                  *StartOpCodeHandle2,
  IN  VOID                  *OptionsOpCodeHandle,
  IN  VOID                  *OptionsOpCodeHandle2
  )
{

  UINTN                                     InfoIndex;
  UINT16                                    VarOffset;
  UINT8                                     Flags;
  IGD_SCU_OPTION_INFO                       DisplayOptionInfo[] =
                                               {{STRING_TOKEN(STR_CRT_TEXT), SCU_IGD_BOOT_TYPE_CRT},
                                                {STRING_TOKEN(STR_LFP_TEXT), SCU_IGD_BOOT_TYPE_LFP},
                                                {STRING_TOKEN(STR_EFP_TEXT), SCU_IGD_BOOT_TYPE_EFP},
                                                {STRING_TOKEN(STR_EFP2_TEXT), SCU_IGD_BOOT_TYPE_EFP2},
                                                {STRING_TOKEN(STR_EFP3_TEXT), SCU_IGD_BOOT_TYPE_EFP3},
                                                {0xFF,0xFF}};


  //
  // Create "VBIOS Default" option for primay display type
  //
  HiiCreateOneOfOptionOpCode (
    OptionsOpCodeHandle,
    STRING_TOKEN(STR_VBIOS_DEFAULT_TEXT),
    EFI_IFR_OPTION_DEFAULT,
    EFI_IFR_NUMERIC_SIZE_1,
    (UINT8)SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT
    );

  //
  // Create "Disable" option for secondary display type
  //
  HiiCreateOneOfOptionOpCode (
    OptionsOpCodeHandle2,
    STRING_TOKEN(STR_DISABLED_TEXT),
    EFI_IFR_OPTION_DEFAULT,
    EFI_IFR_NUMERIC_SIZE_1,
    (UINT8)SCU_IGD_BOOT_TYPE_DISABLE
    );

  //
  // Create other display device type for both
  //
  InfoIndex = 0;
  Flags = EFI_IFR_TYPE_NUM_SIZE_8;
  while (DisplayOptionInfo[InfoIndex].SCUDisplayType != 0xFF) {
    HiiCreateOneOfOptionOpCode (
      OptionsOpCodeHandle,
      DisplayOptionInfo[InfoIndex].StringId,
      Flags,
      EFI_IFR_NUMERIC_SIZE_1,
      (UINT8)DisplayOptionInfo[InfoIndex].SCUDisplayType
      );
    HiiCreateOneOfOptionOpCode (
      OptionsOpCodeHandle2,
      DisplayOptionInfo[InfoIndex].StringId,
      Flags,
      EFI_IFR_NUMERIC_SIZE_1,
      (UINT8)DisplayOptionInfo[InfoIndex].SCUDisplayType
      );
    InfoIndex++;
  }

  VarOffset = (UINT16)(UINTN)&(((CHIPSET_CONFIGURATION*)0)->IGDBootType);
  HiiCreateOneOfOpCode (
    StartOpCodeHandle,
    KEY_IGD_PRIMARY_DISPLAY_SELECTION,
    CONFIGURATION_VARSTORE_ID,
    VarOffset,
    (STRING_REF)STRING_TOKEN(STR_IGD_BOOT_TYPE_STRING),
    (STRING_REF)STRING_TOKEN(STR_IGD_BOOT_TYPE_HELP),
    EFI_IFR_FLAG_CALLBACK,
    EFI_IFR_NUMERIC_SIZE_1,
    OptionsOpCodeHandle,
    NULL
    );

  //
  // Suppressif item when primary display type is default
  //
  HiiCreateSuppressIfOpCode (StartOpCodeHandle);
  HiiCreateIdEqualOpCode (StartOpCodeHandle, KEY_IGD_PRIMARY_DISPLAY_SELECTION, SCU_IGD_BOOT_TYPE_VBIOS_DEFAULT);

  VarOffset = (UINT16)(UINTN)&(((CHIPSET_CONFIGURATION*)0)->IGDBootTypeSecondary);
  HiiCreateOneOfOpCode (
    StartOpCodeHandle,
    KEY_IGD_SECONDARY_DISPLAY_SELECTION,
    CONFIGURATION_VARSTORE_ID,
    VarOffset,
    (STRING_REF)STRING_TOKEN(STR_IGD_BOOT_TYPE2_STRING),
    (STRING_REF)STRING_TOKEN(STR_IGD_BOOT_TYPE2_HELP),
    0,
    EFI_IFR_NUMERIC_SIZE_1,
    OptionsOpCodeHandle2,
    NULL
    );
  
  HiiCreateEndOpCode (StartOpCodeHandle);
  

}
//[-end-130401-IB05400394-modify]//

EFI_STATUS
IgdInitialVideoDisplaySelectionLabel (
  IN  EFI_HII_HANDLE                      HiiHandle
  )
{
  EFI_STATUS                                Status;
  EFI_SETUP_UTILITY_PROTOCOL                *SetupUtility;
  CHIPSET_CONFIGURATION                      *SetupVariable;
  EFI_LEGACY_BIOS_PROTOCOL                  *LegacyBios;
  UINT8                                     *PtrToVbtTable;
  VOID                                      *StartOpCodeHandle;
  VOID                                      *StartOpCodeHandle2;
  EFI_IFR_GUID_LABEL                        *StartLabel;
  EFI_IFR_GUID_LABEL                        *StartLabel2;
  VOID                                      *OptionsOpCodeHandle;
  VOID                                      *OptionsOpCodeHandle2;

  Status = gBS->LocateProtocol (&gEfiSetupUtilityProtocolGuid, NULL, (VOID **)&SetupUtility);
  ASSERT_EFI_ERROR(Status);
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);


  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);
  
  StartOpCodeHandle2 = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  OptionsOpCodeHandle= HiiAllocateOpCodeHandle ();
  ASSERT ( OptionsOpCodeHandle != NULL);

  OptionsOpCodeHandle2= HiiAllocateOpCodeHandle ();
  ASSERT ( OptionsOpCodeHandle != NULL);
  
//[-start-130330-IB05400394-modify]//
  StartLabel               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = IGD_DISPLAY_SELECTION_START_LABEL;  
  
  StartLabel2               = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle2, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel2->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel2->Number       = IGD_DISPLAY_SELECTION_END_LABEL;   
//[-end-130330-IB05400394-modify]//
  if (EFI_ERROR(Status)) {
    //
    // No CSM, GOP is working.
    //
    PtrToVbtTable = NULL;
    IgdGetVbtTable (&PtrToVbtTable);
    IgdCreateEfiModeDisplayOption (
               NULL,
               PtrToVbtTable, 
               SetupVariable->IGDBootType, 
               SetupVariable->IGDBootTypeSecondary,
               StartOpCodeHandle,
               StartOpCodeHandle2,
               OptionsOpCodeHandle,
               OptionsOpCodeHandle2
               );

  } else {
    //
    // CSM is working
    //
//[-start-130401-IB05400394-modify]//
    IgdCreateCSMSupportedDisplayOption (
      SetupVariable->IGDBootType, 
      SetupVariable->IGDBootTypeSecondary,
      StartOpCodeHandle,
      StartOpCodeHandle2,
      OptionsOpCodeHandle,
      OptionsOpCodeHandle2
      );
//[-end-130401-IB05400394-modify]//
  }
  
  
//[-start-130330-IB05400394-modify]//
  HiiUpdateForm (
    HiiHandle,
    NULL, 
    (EFI_FORM_ID) IGD_FORM_ID,
    StartOpCodeHandle,
    StartOpCodeHandle2
    );
//[-end-130330-IB05400394-modify]//
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (OptionsOpCodeHandle);
//[-start-130330-IB05400394-add]//
  HiiFreeOpCodeHandle (StartOpCodeHandle2);
  HiiFreeOpCodeHandle (OptionsOpCodeHandle2);
//[-end-130330-IB05400394-add]//
  return Status;
}

//[-start-120216-IB09960004-remove]//
//EFI_STATUS
//IgdVideoDisplaySelectionOption (
//  IN     EFI_HII_HANDLE                      HiiHandle,  
//  IN OUT CHIPSET_CONFIGURATION                *MyIfrNVData,
//  IN     EFI_QUESTION_ID                     QuestionId,
//  IN     UINT8                               PrimaryDisplaySelected
//  )
//{
//  EFI_STATUS                   Status;
//  EFI_HII_UPDATE_DATA          UpdateData;
//  EFI_LEGACY_BIOS_PROTOCOL     *LegacyBios;
//  UINT8                        *PtrToVbtTable;
//
//
//  Status = gBS->LocateProtocol (
//                  &gEfiLegacyBiosProtocolGuid,
//                  NULL,
//                  &LegacyBios
//                  );
//
//  if (EFI_ERROR(Status)) {
//    //
//    // No CSM, GOP is working.
//    //
//    IfrLibInitUpdateData (&UpdateData, 0x1000);
//
//    PtrToVbtTable = NULL;
//    IgdGetVbtTable (&PtrToVbtTable);
//
//    Status = IgdCreateEfiModeDisplayOption (
//               MyIfrNVData,
//               PtrToVbtTable, 
//               PrimaryDisplaySelected, 
//               MyIfrNVData->IGDBootTypeSecondary,
//               &UpdateData
//               );
//    if (EFI_ERROR(Status)) {
//      IfrLibFreeUpdateData (&UpdateData);
//      return Status;
//    }
//
//    IfrLibUpdateForm (HiiHandle, NULL, VIDEO_CONFIG_FORM_ID, IGD_DISPLAY_SELECTION_START_LABEL, FALSE, &UpdateData);
//
//    IfrLibFreeUpdateData (&UpdateData);
//    return Status;
//
//  } else {
//    //
//    // CSM is working
//    //
//    return EFI_SUCCESS;
//  }
//
//}
//[-end-120216-IB09960004-remove]//

//[-start-130401-IB05400394-modify]//
EFI_STATUS
ClearFormDataFromLabel (
  IN     EFI_HII_HANDLE                      HiiHandle,
  IN     EFI_FORM_ID                         FormID,
  IN     UINT16                              Label,
  IN     UINT16                              Label2
  )
{
  VOID                                *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL                  *StartLabel;
  VOID                                *OptionsOpCodeHandle;
  VOID                                *StartOpCodeHandle2;
  EFI_IFR_GUID_LABEL                  *StartLabel2;
  VOID                                *OptionsOpCodeHandle2;

  
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);
  
  OptionsOpCodeHandle= HiiAllocateOpCodeHandle ();
  ASSERT ( OptionsOpCodeHandle != NULL);
  
  StartLabel = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number = Label;

  StartOpCodeHandle2 = NULL;
  OptionsOpCodeHandle2 = NULL;
  if (Label2 != 0) {
    StartOpCodeHandle2 = HiiAllocateOpCodeHandle ();
    ASSERT (StartOpCodeHandle2 != NULL);
    
    OptionsOpCodeHandle2 = HiiAllocateOpCodeHandle ();
    ASSERT ( OptionsOpCodeHandle != NULL);
    
    StartLabel2 = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle2, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
    StartLabel2->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
    StartLabel2->Number = Label2;
  }

  HiiUpdateForm (
    HiiHandle,
    NULL, 
    (EFI_FORM_ID)FormID,
    StartOpCodeHandle,
    StartOpCodeHandle2
    );
  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (OptionsOpCodeHandle);

  if (StartOpCodeHandle2 != NULL) {
    HiiFreeOpCodeHandle (StartOpCodeHandle2);
  }

  if (OptionsOpCodeHandle2 != NULL) {
    HiiFreeOpCodeHandle (OptionsOpCodeHandle2);
  }
  
  return EFI_SUCCESS;
}
//[-end-130401-IB05400394-modify]//

EFI_STATUS
InitialDualVgaControllersLabel (
  IN  EFI_HII_HANDLE                      HiiHandle
  )
{
  EFI_STATUS                    Status;
  EFI_LEGACY_BIOS_PROTOCOL      *LegacyBios;
  UINTN                         OptionsConut;
  EFI_SETUP_UTILITY_PROTOCOL    *SetupUtility;
  CHIPSET_CONFIGURATION          *SetupVariable;
  CHIPSET_CONFIGURATION          SetupNvData;
  UINT16                        VarOffset;
  VOID                          *StartOpCodeHandle;
  EFI_IFR_GUID_LABEL            *StartLabel;
  VOID                          *OptionsOpCodeHandle;
  UINT8                         Flags;

  Status = gBS->LocateProtocol (
                  &gEfiSetupUtilityProtocolGuid,
                  NULL,
                  (VOID **)&SetupUtility
                  );
  ASSERT_EFI_ERROR(Status);
  SetupVariable = (CHIPSET_CONFIGURATION *)(SetupUtility->SetupNvData);

  Status = gBS->LocateProtocol (
                  &gEfiLegacyBiosProtocolGuid,
                  NULL,
                  (VOID **)&LegacyBios
                  );

  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  ASSERT (StartOpCodeHandle != NULL);

  OptionsOpCodeHandle= HiiAllocateOpCodeHandle ();
  ASSERT ( OptionsOpCodeHandle != NULL);

  StartLabel         = (EFI_IFR_GUID_LABEL *) HiiCreateGuidOpCode (StartOpCodeHandle, &gEfiIfrTianoGuid, NULL, sizeof (EFI_IFR_GUID_LABEL));
  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
//[-start-130330-IB05400394-modify]//
  StartLabel->Number     = DUAL_VGA_SUPPORT_START_LABEL;
//[-end-130330-IB05400394-modify]//

  //
  // Only supported in the non-CSM mode.
  //
  if (EFI_ERROR(Status)) {
    OptionsConut = 2; // Enable and disable.
    Flags = EFI_IFR_TYPE_NUM_SIZE_8; 
    if (SetupVariable->UefiDualVgaControllers  == DUAL_VGA_CONTROLLER_ENABLE) {
      Flags = EFI_IFR_OPTION_DEFAULT;
    }
    HiiCreateOneOfOptionOpCode (
      OptionsOpCodeHandle,
      STRING_TOKEN(STR_ENABLED_TEXT),
      Flags,
      EFI_IFR_NUMERIC_SIZE_1,
      DUAL_VGA_CONTROLLER_ENABLE
      );
//[-start-110929-IB07370068-modify]//
//[-end-110929-IB07370068-modify]//
    Flags = EFI_IFR_TYPE_NUM_SIZE_8; 
    if (DUAL_VGA_CONTROLLER_DISABLE == SetupVariable->UefiDualVgaControllers) {
      Flags = EFI_IFR_OPTION_DEFAULT;
    }
    HiiCreateOneOfOptionOpCode (
      OptionsOpCodeHandle,
      STRING_TOKEN(STR_DISABLED_TEXT),
      Flags,
      EFI_IFR_NUMERIC_SIZE_1,
      DUAL_VGA_CONTROLLER_DISABLE
      ); 


    VarOffset = (UINT16)((UINTN)(&SetupNvData.UefiDualVgaControllers)-(UINTN)(&SetupNvData));

    HiiCreateOneOfOpCode (
      StartOpCodeHandle,
      KEY_DUAL_VGA_SUPPORT,
      CONFIGURATION_VARSTORE_ID,
      VarOffset,
      (STRING_REF)STRING_TOKEN(STR_UEFI_DUAL_VGA_SUPPORT_PROMPT),
      (STRING_REF)STRING_TOKEN(STR_UEFI_DUAL_VGA_SUPPORT_HELP),
      0x04,
      EFI_IFR_NUMERIC_SIZE_1,
      OptionsOpCodeHandle,
      NULL
      );

    Status= HiiUpdateForm (
              HiiHandle,
              NULL, 
              (EFI_FORM_ID)VIDEO_CONFIG_FORM_ID,
              StartOpCodeHandle,
              NULL
              );

    HiiFreeOpCodeHandle (StartOpCodeHandle);
    HiiFreeOpCodeHandle (OptionsOpCodeHandle);
    return Status;
  } else {
    return EFI_UNSUPPORTED;
  }
}
