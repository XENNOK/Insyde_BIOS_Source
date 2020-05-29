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

#include <PlatformInfo.h>
#include <Guid/MeInfo.h>

//[-start-120619-IB06150226-modify]//
STEPPING_STRING_DEFINITION          MeFwSkuString[] = {
                                      {INTEL_ME_1_5MB_FW, L"1.5MB"},
                                      {INTEL_ME_5MB_FW,   L"5MB"},
                                      {0xFF, L""}
                                      };
//[-end-120619-IB06150226-modify]//
//[-start-120727-IB05300320-remove]//
//static
//EFI_STATUS
//GetMeFwVersion (
//  IN OUT GEN_GET_FW_VER_ACK        *MsgGenGetFwVersionAck
//  );
//[-end-120727-IB05300320-remove]//

EFI_STATUS
GetMeVersion (
  IN    VOID                      *OpCodeHandle,
  IN    EFI_HII_HANDLE            MainHiiHandle,
  IN    EFI_HII_HANDLE            AdvanceHiiHandle,
  IN    CHAR16                    *StringBuffer
  )
{
  EFI_STATUS                        Status;
//[-start-120727-IB05300320-remove]//
//  GEN_GET_FW_VER_ACK                MsgGenGetFwVersionAck;
//[-end-120727-IB05300320-remove]//
  STRING_REF                        MeFwVersionString;
  STRING_REF                        MeVersionString;
//[-start-120301-IB07360173-modify]//
  DXE_MBP_DATA_PROTOCOL             *MBPDataProtocol;
//  BOOLEAN                           MebpSupportFlag;
//  ME_BIOS_PAYLOAD                   *MeBiosPayload;
//[-end-120301-IB07360173-modify]//
//[-start-120727-IB05300320-remove]//
//  EFI_HECI_PROTOCOL                 *Heci;
//  UINT32                            MeMode;
//  UINT32                            MeStatus;  
//[-end-120727-IB05300320-remove]//
//[-start-120619-IB06150226-add]//
  UINTN                             Index;
//[-end-120619-IB06150226-add]//
  UINTN                             VariableSize;
  RECORD_ME_VERSION                 MeVersion;
  EFI_GUID                          RecordMeVersionGuid = RECORD_ME_VERSION_GUID;

  Status = EFI_UNSUPPORTED;

//[-start-130408-IB10310017-add]//
  ///
  /// ME Version Hook for SPS.
  ///
  if (FeaturePcdGet (PcdSpsMeSupported)) {
    Status = GetSpsMeVersion (OpCodeHandle, MainHiiHandle, AdvanceHiiHandle, StringBuffer);
    if (!EFI_ERROR (Status)) {
      return EFI_SUCCESS;
    }
  }
//[-start-130408-IB10310017-add]//

  if (!FeaturePcdGet (PcdMeSupported)) {
    return Status;
  }
//[-start-120301-IB07360173-modify]//
  Status = gBS->LocateProtocol ( &gMeBiosPayloadDataProtocolGuid, NULL, (VOID **)&MBPDataProtocol );
//[-end-120301-IB07360173-modify]//

//[-start-120727-IB05300320-remove]//
//  Status = gBS->LocateProtocol (
//                  &gEfiHeciProtocolGuid,
//                  NULL,
//                  &Heci
//                  );
//
//  EfiZeroMem (&MsgGenGetFwVersionAck, sizeof (GEN_GET_FW_VER_ACK));
//
////[-start-120301-IB07360173-remove]//
////  if (MebpSupportFlag) {
////[-end-120301-IB07360173-remove]//
////[-start-120301-IB07360173-modify]//
////[-start-120307-IB07360175-modify]//
//  if ( MBPDataProtocol->MeBiosPayload.FwVersionName.MajorVersion != 9 ) {
////[-end-120307-IB07360175-modify]//
//    Status = EFI_DEVICE_ERROR;
//  } else {
//    if (Heci != NULL) {
//      Status = Heci->GetMeMode (&MeMode);
//      if (EFI_ERROR (Status) || (MeMode != ME_MODE_NORMAL)) {
//        Status = EFI_DEVICE_ERROR;      
//      } else {  
//        Status = Heci->GetMeStatus (&MeStatus);
//        if (EFI_ERROR (Status) || ((MeStatus & 0xF) != ME_READY)) {
//          Status = EFI_DEVICE_ERROR;      
//        }  
//      }    
//    } else {
//      Status = EFI_DEVICE_ERROR;          
//    }
////[-end-120301-IB07360173-modify]//
//  }
////[-start-120301-IB07360173-remove]//
////  } else {
////    Status = GetMeFwVersion (&MsgGenGetFwVersionAck);
////  }
////[-end-120301-IB07360173-remove]//
//[-end-120727-IB05300320-remove]//
//[-start-120619-IB06150226-modify]//
  if (!EFI_ERROR (Status)) {
    for (Index = 0; MeFwSkuString[Index].ReversionValue != 0xFF; Index++) {
      if (MBPDataProtocol->MeBiosPayload.FwPlatType.RuleData.Fields.IntelMeFwImageType == MeFwSkuString[Index].ReversionValue) {
        break;
      }
    }
//[-start-120301-IB07360173-modify]//
    UnicodeSPrint (
      StringBuffer,
      0x100,
      L"%d.%d.%d.%d / %s",
      (UINTN) (UINT16) MBPDataProtocol->MeBiosPayload.FwVersionName.MajorVersion,
      (UINTN) (UINT16) MBPDataProtocol->MeBiosPayload.FwVersionName.MinorVersion,
      (UINTN) (UINT16) MBPDataProtocol->MeBiosPayload.FwVersionName.HotfixVersion,
      (UINTN) (UINT16) MBPDataProtocol->MeBiosPayload.FwVersionName.BuildVersion,
      MeFwSkuString[Index].SteppingString
      );
//[-end-120301-IB07360173-modify]//
  } else {
    VariableSize = sizeof (RECORD_ME_VERSION);
    Status = gRT->GetVariable (
                    L"MeVersion",
                    &RecordMeVersionGuid,
                    NULL,
                    &VariableSize,
                    &MeVersion
                    );
    if (!EFI_ERROR (Status)) {
      UnicodeSPrint (
        StringBuffer,
        0x100,
        L"%d.%d.%d.%d / %s",
        (UINTN) (UINT16) MeVersion.MajorVersion,
        (UINTN) (UINT16) MeVersion.MinorVersion,
        (UINTN) (UINT16) MeVersion.HotfixVersion,
        (UINTN) (UINT16) MeVersion.BuildVersion,
        (CHAR16 *) MeVersion.SteppingString
        );
    } else {  
      StringBuffer = HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_ME_VERSION), NULL);
    }
  }
//[-end-120619-IB06150226-modify]//

  MeVersionString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL); 
//[-start-121002-IB06460449-modify]//
//  StringBuffer=HiiGetString (AdvanceHiiHandle, STRING_TOKEN (STR_ME_FW_VERSION_STRING), NULL);
//
//  MeFwVersionString=HiiSetString (MainHiiHandle, 0, StringBuffer, NULL); 
  NewStringToHandle (
    AdvanceHiiHandle,
    STRING_TOKEN (STR_ME_FW_VERSION_STRING),
    MainHiiHandle,
    &MeFwVersionString
    );
//[-end-121002-IB06460449-modify]//

  HiiCreateTextOpCode (OpCodeHandle,MeFwVersionString, 0, MeVersionString );

  return Status;
}

//[-start-120727-IB05300320-remove]//
//EFI_STATUS
//GetMeFwVersion (
//  IN OUT GEN_GET_FW_VER_ACK        *MsgGenGetFwVersionAck
//  )
//{
//  EFI_STATUS            Status;
//  UINT32                Length;
//  GEN_GET_FW_VER        *MsgGenGetFwVersion;
//  GEN_GET_FW_VER        GenGetFwVersion;
//  EFI_HECI_PROTOCOL     *Heci;
//
//  Status = gBS->LocateProtocol (
//                  &gEfiHeciProtocolGuid,
//                  NULL,
//                  &Heci
//                  );
//  if (EFI_ERROR (Status)) {
//    return Status;
//  }
//
//  //
//  // Allocate MsgGenGetFwVersion data structure
//  //
//  MsgGenGetFwVersion = &GenGetFwVersion;
//  MsgGenGetFwVersion->MKHIHeader.Fields.GroupId    = MKHI_GEN_GROUP_ID;
//  MsgGenGetFwVersion->MKHIHeader.Fields.Command    = GEN_GET_FW_VERSION_CMD;
//  MsgGenGetFwVersion->MKHIHeader.Fields.IsResponse = 0;
//  Length = sizeof (GEN_GET_FW_VER);
//  
//  //
//  // Send Get Firmware Version Request to ME
//  //
//  Status = Heci->SendMsg (
//                   (UINT32 *) MsgGenGetFwVersion,
//                   Length,
//                   BIOS_FIXED_HOST_ADDR,
//                   HECI_CORE_MESSAGE_ADDR
//                   );
//  if (EFI_ERROR (Status)) {
//    return Status;
//  }
//
//  Length = sizeof (GEN_GET_FW_VER_ACK);
//  Status = Heci->ReadMsg (
//                   BLOCKING,
//                   (UINT32 *) MsgGenGetFwVersionAck,
//                   &Length
//                   );
//  if (EFI_ERROR (Status)) {
//    return Status;
//  }
//
//  return Status;
//}
//[-end-120727-IB05300320-remove]//
