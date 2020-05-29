/** @file
 HII Config Access implementation for IPMI Config in Device Manager driver.

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


EFI_GUID                                mIpmiDmConfigFormSetGuid = IPMI_DM_CONFIG_FORMSET_GUID;


/**
 This function allows a caller to extract the current configuration for one
 or more named elements from the target driver.
 Extract mIpmiDmConfigPrivateData->IpmiDmConfig to config when enter config menu.

 @param[in]         This                Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param[in]         Request             A null-terminated Unicode string in <ConfigRequest> format.
 @param[out]        Progress            On return, points to a character in the Request string.
                                        Points to the string's null terminator if request was successful.
                                        Points to the most recent '&' before the first failing name/value
                                        pair (or the beginning of the string if the failure is in the
                                        first name/value pair) if the request was not successful.
 @param[out]        Results             A null-terminated Unicode string in <ConfigAltResp> format which
                                        has all values filled in for the names in the Request string.
                                        String to be allocated by the called function.

 @retval EFI_SUCCESS                    The Results is filled with the requested values.
 @retval EFI_OUT_OF_RESOURCES           Not enough memory to store the results.
 @retval EFI_INVALID_PARAMETER          Request is NULL, illegal syntax, or unknown name.
 @retval EFI_NOT_FOUND                  Routing data doesn't match any storage in this driver.
*/
EFI_STATUS
EFIAPI
ConfigAccessExtract (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN CONST EFI_STRING                       Request,
  OUT      EFI_STRING                       *Progress,
  OUT      EFI_STRING                       *Results
  )
{
  EFI_STATUS                                Status;
  EFI_STRING                                ConfigRequestHdr;
  EFI_STRING                                ConfigRequest;
  BOOLEAN                                   AllocatedRequest;
  UINTN                                     Size;

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;
  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &mIpmiDmConfigFormSetGuid, IPMI_CONFIG_DATA_NAME)) {
    return EFI_NOT_FOUND;
  }


  //
  // Do something necessary before extract Config Data.
  // Such as handling the Config Data non-referenced by IFR.
  //
  IpmiDmConfigAccessExtract (mIpmiDmConfigPrivateData->IpmiDmConfigData);


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
                         &mIpmiDmConfigFormSetGuid,
                         IPMI_CONFIG_DATA_NAME,
                         mIpmiDmConfigPrivateData->DriverHandle
                         );
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint (
      ConfigRequest,
      Size,
      L"%s&OFFSET=0&WIDTH=%016LX",
      ConfigRequestHdr,
      (UINT64) mIpmiDmConfigPrivateData->IpmiDmConfigDataSize
      );
    FreePool (ConfigRequestHdr);
  }

  Status = gHiiConfigRouting->BlockToConfig (
                                gHiiConfigRouting,
                                ConfigRequest,
                                mIpmiDmConfigPrivateData->IpmiDmConfigData,
                                mIpmiDmConfigPrivateData->IpmiDmConfigDataSize,
                                Results,
                                Progress
                                );

  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    FreePool (ConfigRequest);
  }

  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  return Status;

}


/**
 This function processes the results of changes in configuration.
 Get current config setting and save to mIpmiDmConfigPrivateData->IpmiDmConfig & "IpmiConfigData" Variable.

 @param[in]         This                Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param[in]         Configuration       A null-terminated Unicode string in <ConfigResp> format.
 @param[out]        Progress            A pointer to a string filled in with the offset of the most
                                        recent '&' before the first failing name/value pair (or the
                                        beginning of the string if the failure is in the first
                                        name/value pair) or the terminating NULL if all was successful.

 @retval EFI_SUCCESS                    The Results is processed successfully.
 @retval EFI_INVALID_PARAMETER          This or Configuration or Progress is NULL.
 @retval EFI_NOT_FOUND                  Routing data doesn't match any storage in this driver.
*/
EFI_STATUS
EFIAPI
ConfigAccessRoute (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN CONST EFI_STRING                       Configuration,
  OUT      EFI_STRING                       *Progress
  )
{
  EFI_STATUS                                Status;
  UINTN                                     BufferSize;

  if (Configuration == NULL || Progress == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;
  if (!HiiIsConfigHdrMatch (Configuration, &mIpmiDmConfigFormSetGuid, IPMI_CONFIG_DATA_NAME)) {
    return EFI_NOT_FOUND;
  }

  //
  // Convert <ConfigResp> to buffer data by helper function ConfigToBlock()
  //
  BufferSize = mIpmiDmConfigPrivateData->IpmiDmConfigDataSize;
  Status = gHiiConfigRouting->ConfigToBlock (
                                gHiiConfigRouting,
                                Configuration,
                                mIpmiDmConfigPrivateData->IpmiDmConfigData,
                                &BufferSize,
                                Progress
                                );
  if (EFI_ERROR (Status)) {
    return Status;
  }


  //
  // Do something necessary in Config Access Route.
  // Such as handling the Config Data non-referenced by IFR and set configuration to BMC.
  //
  IpmiDmConfigAccessRoute (mIpmiDmConfigPrivateData->IpmiDmConfigData);


  //
  // Set IPMI Device Manager Config Data to Variable.
  //
  gRT->SetVariable (
         mIpmiDmConfigVariableName,
         &mIpmiDmConfigVariableGuid,
         EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
         mIpmiDmConfigPrivateData->IpmiDmConfigDataSize,
         mIpmiDmConfigPrivateData->IpmiDmConfigData
         );


  return EFI_SUCCESS;

}


/**
 This function processes the results of changes in configuration.
 And when select the object that have INTERACTIVE flags in VFR file.

 @param[in]         This                Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
 @param[in]         Action              Specifies the type of action taken by the browser.
 @param[in]         QuestionId          A unique value which is sent to the original exporting driver
                                        so that it can identify the type of data to expect.
 @param[in]         Type                The type of value for the question.
 @param[in]         Value               A pointer to the data being sent to the original exporting driver.
 @param[out]        ActionRequest       On return, points to the action requested by the callback function.

 @retval EFI_SUCCESS                    The callback successfully handled the action.
 @retval EFI_OUT_OF_RESOURCES           Not enough storage is available to hold the variable and its data.
 @retval EFI_DEVICE_ERROR               The variable could not be saved.
 @retval EFI_UNSUPPORTED                The specified Action is not supported by the callback.
*/
EFI_STATUS
EFIAPI
ConfigAccessCallback (
  IN CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN       EFI_BROWSER_ACTION               Action,
  IN       EFI_QUESTION_ID                  QuestionId,
  IN       UINT8                            Type,
  IN       EFI_IFR_TYPE_VALUE               *Value,
  OUT      EFI_BROWSER_ACTION_REQUEST       *ActionRequest
  )
{
  UINT8                                     *TempBrowserData;
  UINT8                                     *VfrBin;

  if ((This == NULL) || (Value == NULL) || (ActionRequest == NULL)) {
    return EFI_INVALID_PARAMETER;
  }


  TempBrowserData = AllocateZeroPool (mIpmiDmConfigPrivateData->IpmiDmConfigDataSize);

  //
  // Get current browser config data for general callback or extract VFR default for load default.
  //
  if (QuestionId == KEY_DM_IPMI_LOAD_OPTIMAL_DEFAULT) {
    VfrBin = IpmiDmConfigVfrBin;
    IpmiDmExtractVfrDefault (TempBrowserData, VfrBin, mIpmiDmConfigPrivateData->IpmiDmConfigDataSize);
  } else {
    HiiGetBrowserData (
      &mIpmiDmConfigFormSetGuid,
      IPMI_CONFIG_DATA_NAME,
      mIpmiDmConfigPrivateData->IpmiDmConfigDataSize,
      TempBrowserData
      );
  }


  //
  // Handling the object that have INTERACTIVE flags in VFR.
  //
  IpmiDmConfigAccessCallback (
    QuestionId,
    mIpmiDmConfigPrivateData->HiiHandle,
    TempBrowserData
    );


  //
  // Set config data to current browser config after callback function.
  //
  HiiSetBrowserData (
    &mIpmiDmConfigFormSetGuid,
    IPMI_CONFIG_DATA_NAME,
    mIpmiDmConfigPrivateData->IpmiDmConfigDataSize,
    TempBrowserData,
    NULL
    );


  FreePool (TempBrowserData);

  return EFI_SUCCESS;

}


/**
 Extract Default Config Data from VFR Formset Package Bin.

 @param[in, out]    ConfigData          A pointer to the config data buffer.
 @param[in]         VfrBin              The VFR Formset Package Bin of a single VFR Formset.
 @param[in]         ConfigDataSize      Data size in bytes of the config data.
*/
VOID
IpmiDmExtractVfrDefault (
  IN OUT UINT8                          *ConfigData,
  IN     UINT8                          *VfrBin,
  IN     UINTN                          ConfigDataSize
  )
{
  UINTN                                 VfrBufSize;
  UINTN                                 VfrIndex;
  EFI_IFR_OP_HEADER                     *IfrOpHeader;
  EFI_IFR_NUMERIC                       *IfrNumeric;
  EFI_IFR_CHECKBOX                      *IfrCheckBox;
  EFI_IFR_DEFAULT                       *IfrDefault;
  EFI_IFR_ONE_OF_OPTION                 *IfrOneOfOption;
  UINTN                                 NvDataIndex;
  UINTN                                 DataSize;
  

  //
  // First 4 bytes is UINT32 ARRAY LENGTH.
  //
  VfrBufSize = (UINTN)(*(UINT32*)VfrBin);
  //
  // Next is EFI_HII_FORM_PACKAGE_HDR.
  // Set VFR index after UINT32 ARRAY LENGTH and EFI_HII_FORM_PACKAGE_HDR.
  //
  VfrIndex = sizeof (UINT32) + sizeof (EFI_HII_FORM_PACKAGE_HDR);


  do {

    //
    // After UINT32 ARRAY LENGTH and EFI_HII_FORM_PACKAGE_HDR,
    // Next is IFR Opcodes one by one.
    //
    IfrOpHeader = (EFI_IFR_OP_HEADER*)(VfrBin + VfrIndex);
    DataSize = 0;

    switch (IfrOpHeader->OpCode) {

    case EFI_IFR_ONE_OF_OP:
    case EFI_IFR_NUMERIC_OP:
      IfrNumeric = (EFI_IFR_NUMERIC*)IfrOpHeader;
      NvDataIndex = IfrNumeric->Question.VarStoreInfo.VarOffset;

      if (NvDataIndex > ConfigDataSize) {
        ASSERT (NvDataIndex < ConfigDataSize);
        VfrIndex += IfrOpHeader->Length;
        break;
      }

      switch (IfrNumeric->Flags & EFI_IFR_NUMERIC_SIZE) {
      case EFI_IFR_NUMERIC_SIZE_1:
        DataSize = 1;
        break;
      case EFI_IFR_NUMERIC_SIZE_2:
        DataSize = 2;
        break;
      case EFI_IFR_NUMERIC_SIZE_4:
        DataSize = 4;
        break;
      case EFI_IFR_NUMERIC_SIZE_8:
        DataSize = 8;
        break;
      }

      //
      // There are two ways to specify defaults for this question: one or more nested 
      // EFI_IFR_ONE_OF_OPTION (lowest priority) or nested EFI_IFR_DEFAULT (highest priority).
      //
      VfrIndex += IfrOpHeader->Length;
      IfrOpHeader = (EFI_IFR_OP_HEADER*)(VfrBin + VfrIndex);

      switch (IfrOpHeader->OpCode) {

      //
      // EFI_IFR_DEFAULT
      //
      case EFI_IFR_DEFAULT_OP:
        IfrDefault = (EFI_IFR_DEFAULT*)IfrOpHeader;

        switch (IfrDefault->Type) {
        //
        // Check Default Value size equal to Data size, and set default value to Config Data.
        //
        case EFI_IFR_TYPE_NUM_SIZE_8:
          if (DataSize == 1) {
            CopyMem (ConfigData + NvDataIndex, &IfrDefault->Value, DataSize);
          }
          break;
        case EFI_IFR_TYPE_NUM_SIZE_16:
          if (DataSize == 2) {
            CopyMem (ConfigData + NvDataIndex, &IfrDefault->Value, DataSize);
          }
          break;
        case EFI_IFR_TYPE_NUM_SIZE_32:
          if (DataSize == 4) {
            CopyMem (ConfigData + NvDataIndex, &IfrDefault->Value, DataSize);
          }
          break;
        case EFI_IFR_TYPE_NUM_SIZE_64:
          if (DataSize == 4) {
            CopyMem (ConfigData + NvDataIndex, &IfrDefault->Value, DataSize);
          }
          break;
        }
        break;

      //
      // EFI_IFR_ONE_OF_OPTION
      //
      case EFI_IFR_ONE_OF_OPTION_OP:
        //
        // one or more nested
        //
        do {

          IfrOneOfOption = (EFI_IFR_ONE_OF_OPTION*)IfrOpHeader;

          if (IfrOneOfOption->Flags == EFI_IFR_OPTION_DEFAULT) {
            //
            // If this nested Opcode have EFI_IFR_OPTION_DEFAULT Flags, set default value to Config Data.
            //

            switch (IfrOneOfOption->Type) {
            //
            // Check Default Value size equal to Data size
            //
            case EFI_IFR_TYPE_NUM_SIZE_8:
              if (DataSize == 1) {
                CopyMem (ConfigData + NvDataIndex, &IfrOneOfOption->Value, DataSize);
              }
              break;
            case EFI_IFR_TYPE_NUM_SIZE_16:
              if (DataSize == 2) {
                CopyMem (ConfigData + NvDataIndex, &IfrOneOfOption->Value, DataSize);
              }
              break;
            case EFI_IFR_TYPE_NUM_SIZE_32:
              if (DataSize == 4) {
                CopyMem (ConfigData + NvDataIndex, &IfrOneOfOption->Value, DataSize);
              }
              break;
            case EFI_IFR_TYPE_NUM_SIZE_64:
              if (DataSize == 4) {
                CopyMem (ConfigData + NvDataIndex, &IfrOneOfOption->Value, DataSize);
              }
              break;
            }

            break;

          }

          //
          // If this nested Opcode doesn't have EFI_IFR_OPTION_DEFAULT Flags, find next.
          //
          VfrIndex += IfrOpHeader->Length;
          IfrOpHeader = (EFI_IFR_OP_HEADER*)(VfrBin + VfrIndex);

        } while (IfrOpHeader->OpCode == EFI_IFR_ONE_OF_OPTION_OP);
        break;

      default:
        break;

      }
      break;

    case EFI_IFR_CHECKBOX_OP:
      IfrCheckBox = (EFI_IFR_CHECKBOX*)IfrOpHeader;
      NvDataIndex = IfrCheckBox->Question.VarStoreInfo.VarOffset;

      if (NvDataIndex > ConfigDataSize) {
        ASSERT (NvDataIndex < ConfigDataSize);
        VfrIndex += IfrOpHeader->Length;
        break;
      }

      VfrIndex += IfrOpHeader->Length;
      IfrOpHeader = (EFI_IFR_OP_HEADER*)(VfrBin + VfrIndex);

      switch (IfrOpHeader->OpCode) {

      //
      // EFI_IFR_DEFAULT
      //
      case EFI_IFR_DEFAULT_OP:
        IfrDefault = (EFI_IFR_DEFAULT*)IfrOpHeader;

        switch (IfrDefault->Type) {
        case EFI_IFR_TYPE_NUM_SIZE_8:
          CopyMem (ConfigData + NvDataIndex, &IfrDefault->Value, 1);
          break;
        case EFI_IFR_TYPE_NUM_SIZE_16:
          CopyMem (ConfigData + NvDataIndex, &IfrDefault->Value, 2);
          break;
        case EFI_IFR_TYPE_NUM_SIZE_32:
          CopyMem (ConfigData + NvDataIndex, &IfrDefault->Value, 4);
          break;
        case EFI_IFR_TYPE_NUM_SIZE_64:
          CopyMem (ConfigData + NvDataIndex, &IfrDefault->Value, 8);
          break;
        }
        break;

      //
      // EFI_IFR_ONE_OF_OPTION
      //
      case EFI_IFR_ONE_OF_OPTION_OP:
        //
        // one or more nested
        //
        do {

          IfrOneOfOption = (EFI_IFR_ONE_OF_OPTION*)IfrOpHeader;

          if (IfrOneOfOption->Flags == EFI_IFR_OPTION_DEFAULT) {
            //
            // If this nested Opcode have EFI_IFR_OPTION_DEFAULT Flags, set default value to Config Data.
            //

            switch (IfrOneOfOption->Type) {
            //
            // Check Default Value size equal to Data size
            //
            case EFI_IFR_TYPE_NUM_SIZE_8:
              CopyMem (ConfigData + NvDataIndex, &IfrOneOfOption->Value, 1);
              break;
            case EFI_IFR_TYPE_NUM_SIZE_16:
              CopyMem (ConfigData + NvDataIndex, &IfrOneOfOption->Value, 2);
              break;
            case EFI_IFR_TYPE_NUM_SIZE_32:
              CopyMem (ConfigData + NvDataIndex, &IfrOneOfOption->Value, 3);
              break;
            case EFI_IFR_TYPE_NUM_SIZE_64:
              CopyMem (ConfigData + NvDataIndex, &IfrOneOfOption->Value, 4);
              break;
            }

            break;

          }

          //
          // If this nested Opcode doesn't have EFI_IFR_OPTION_DEFAULT Flags, find next.
          //
          VfrIndex += IfrOpHeader->Length;
          IfrOpHeader = (EFI_IFR_OP_HEADER*)(VfrBin + VfrIndex);

        } while (IfrOpHeader->OpCode == EFI_IFR_ONE_OF_OPTION_OP);
        break;

      default:
        break;

      }
      break;

    default:
      VfrIndex += IfrOpHeader->Length;
      break;

    }

  } while (VfrIndex < VfrBufSize);

}

