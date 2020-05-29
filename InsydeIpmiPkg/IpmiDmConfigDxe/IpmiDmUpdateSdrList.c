/** @file
 IPMI Device Manager Config Update SDR List.

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


#include "IpmiDmConfigAccess.h"


#define SDR_ID_FIELD_WIDTH              3
#define SDR_NAME_FIELD_WIDTH            20
#define SDR_VALUE_FIELD_WIDTH           10


/**
 Get SDRs information from BMC and create OpCode in IPMI Device Manager Config.

 @param[in]         HiiHandle           EFI_HII_HANDLE.

*/
VOID
IpmiDmUpdateSdrList (
  IN  EFI_HII_HANDLE                    HiiHandle
  )
{
  EFI_STATUS                            Status;
  H2O_IPMI_SDR_PROTOCOL                 *Sdr;
  SDR_DATA_STURCT                       *SdrBuffer;
  UINT16                                SdrCount;
  UINT16                                Index;
  CHAR16                                HelpStr[0x100];
  CHAR16                                TempStr[IPMI_CONFIG_TEMP_STR_LEN];
  CHAR16                                SpaceStr[0x20];
  EFI_STRING_ID                         TokenToUpdate;
  EFI_STRING_ID                         HelpTokenToUpdate;
  UINTN                                 Length;
  VOID						                      *StartOpCodeHandle;
  VOID                                  *EndOpCodeHandle;
  EFI_IFR_GUID_LABEL                    *StartLabel;
  EFI_IFR_GUID_LABEL                    *EndLabel;

  Status = gBS->LocateProtocol (&gH2OIpmiSdrProtocolGuid, NULL, (VOID **)&Sdr);
  if (EFI_ERROR (Status)) {
    return;
  }

  Status = Sdr->GetSdrData (Sdr, SDR_ID_ALL, 0x0300, &SdrCount, &SdrBuffer);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Start to update SDR list
  //
  StartOpCodeHandle = HiiAllocateOpCodeHandle ();
  EndOpCodeHandle = HiiAllocateOpCodeHandle ();

  StartLabel = (EFI_IFR_GUID_LABEL*) HiiCreateGuidOpCode (
                                       StartOpCodeHandle,
                                       &gEfiIfrTianoGuid,
                                       NULL,
                                       sizeof (EFI_IFR_GUID_LABEL)
                                       );

  StartLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  StartLabel->Number       = IPMI_BMC_SDR_LIST_LABEL;

  EndLabel = (EFI_IFR_GUID_LABEL*) HiiCreateGuidOpCode (
                                     EndOpCodeHandle,
                                     &gEfiIfrTianoGuid,
                                     NULL,
                                     sizeof (EFI_IFR_GUID_LABEL)
                                     );

  EndLabel->ExtendOpCode = EFI_IFR_EXTEND_OP_LABEL;
  EndLabel->Number       = IPMI_BMC_SDR_LIST_END_LABEL;

  ZeroMem (HelpStr, sizeof (HelpStr));
  ZeroMem (TempStr, sizeof (TempStr));
  StrCpy (SpaceStr, L"                              ");
  for (Index = 0; Index < SdrCount; ++Index) {

    TempStr[0] = 0x20;
    UnicodeValueToString (
      &TempStr[1],
      PREFIX_ZERO,
      (UINT64)SdrBuffer[Index].RecordId,
      SDR_ID_FIELD_WIDTH
      );
    StrnCat (TempStr, SpaceStr, SDR_ID_FIELD_WIDTH);
    Length = StrLen (SdrBuffer[Index].Name);
    StrCat (TempStr, SdrBuffer[Index].Name);
    if (Length < SDR_NAME_FIELD_WIDTH) {
      StrnCat (TempStr, SpaceStr, (UINTN)(SDR_NAME_FIELD_WIDTH - Length));
    }
    Length = StrLen (SdrBuffer[Index].ValueStr);
    StrCat (TempStr, SdrBuffer[Index].ValueStr);
    if (Length < SDR_VALUE_FIELD_WIDTH) {
      StrnCat (TempStr, SpaceStr, (UINTN)(SDR_VALUE_FIELD_WIDTH - Length));
    }
    StrCat (TempStr, SdrBuffer[Index].Unit);

    //
    // Print other information in help field
    //
    UnicodeSPrint (HelpStr, sizeof (HelpStr), L"Sensor Number: 0x%02x\n", SdrBuffer[Index].SensorNumber);

    if (SdrBuffer[Index].DataStatus & SDR_SENSOR_TYPE_ANALOG) {
      StrCat (HelpStr, L"Lower Non-Critical Threshold: ");
      StrCat (HelpStr, SdrBuffer[Index].LowNonCriticalThresStr);
      StrCat (HelpStr, L"\n");
      StrCat (HelpStr, L"Lower Critical Threshold: ");
      StrCat (HelpStr, SdrBuffer[Index].LowCriticalThresStr);
      StrCat (HelpStr, L"\n");
      StrCat (HelpStr, L"Lower Non-Recoverable Threshold: ");
      StrCat (HelpStr, SdrBuffer[Index].LowNonRecoverThresStr);
      StrCat (HelpStr, L"\n");
      StrCat (HelpStr, L"Upper Non-Critical Threshold: ");
      StrCat (HelpStr, SdrBuffer[Index].UpNonCriticalThresStr);
      StrCat (HelpStr, L"\n");
      StrCat (HelpStr, L"Upper Critical Threshold: ");
      StrCat (HelpStr, SdrBuffer[Index].UpCriticalThresStr);
      StrCat (HelpStr, L"\n");
      StrCat (HelpStr, L"Upper Non-Recoverable Threshold: ");
      StrCat (HelpStr, SdrBuffer[Index].UpNonRecoverThresStr);
    } else {
      StrCat (HelpStr, L"This is not an analog sensor, no threshold data.");
    }

    TokenToUpdate = HiiSetString (HiiHandle, 0, TempStr, NULL);
    HelpTokenToUpdate = HiiSetString (HiiHandle, 0, HelpStr, NULL);

    HiiCreateGotoOpCode (
      StartOpCodeHandle,
      0,
      TokenToUpdate,
      HelpTokenToUpdate,
      0,
      0
      );

    ZeroMem (TempStr, IPMI_CONFIG_TEMP_STR_LEN * sizeof (CHAR16));
    ZeroMem (HelpStr, 0x100 * sizeof (CHAR16));

  }

  HiiUpdateForm (
    HiiHandle,
    NULL,
    IPMI_BMC_SDR_LIST_FORM_ID,
    StartOpCodeHandle,
    EndOpCodeHandle
    );

  HiiFreeOpCodeHandle (StartOpCodeHandle);
  HiiFreeOpCodeHandle (EndOpCodeHandle);

  FreePool (SdrBuffer);

}

