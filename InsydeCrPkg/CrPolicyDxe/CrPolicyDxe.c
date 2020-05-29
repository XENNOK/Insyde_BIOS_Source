/** @file
  This driver will install CRPolicy protocol for reference.

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

#include "CrPolicyDxe.h"
#include <CrSetupConfig.h>
#include <Library/VariableLib.h>


//
// data type definitions
//
#define SERIAL_PORT_DEFAULT_FIFO_DEPTH 16

EFI_CR_POLICY_PROTOCOL mCRPolicyTemplate = {
  FALSE, //Disable                                    //CREnable;
  0, //COMA                                             //CRSerialPort;
  7, //115200                                           //CRBaudRate;
  1, // NON Parity                                      //CRParity;
  8, // 8 bits                                             //CRDataBits;
  1, // 1 bit                                              //CRStopBits;
  0, // NON FlowControl                            //CRFlowControl;
  0, // VT100                                          //CRTerminalType;
  CR_TERMINAL_KEY_SENSELESS,     //CRTerminalKey;
  CR_TERMINAL_COMBO_KEY_ENABLE,  //CRComboKey;
  CR_TERMINAL_ROW_25,            //CRTerminalRows;
  CR_24ROWS_POLICY_DEL_FIRST,    //CR24RowsPolicy;
  5,                             //CRInfoWaitTime;
  TRUE,                          //CRAfterPost;
  CR_TERMINAL_VIDEO_COLOR,       //CRVideoType;
  TRUE,                          //CRShowHelp;
  CR_HEADLESS_USE_VBUFFER,       //CRHeadlessVBuffer;
  SERIAL_PORT_DEFAULT_FIFO_DEPTH,//CRFifoLength;
  FALSE,                         //AcpiSpcr;
  CR_ASYNC_TERMINAL_ENABLE,      //CRAsyncTerm;         
  FALSE,                         //CRForce80x25;         
  FALSE,                         //CRFailSaveBaudrate;
  CR_UNKBC_SUPPORT_ENABLE,       //CRUnKbcSupport
  CR_AUTO_REFRESH_ENABLE,        //CRAutoRefresh;
  CR_MANUAL_REFRESH_ENABLE,      //CRManualRefresh;
  CR_TERMINAL_CHARSET_GRAPHIC,   //CRTerminalCharSet;
  0,                             //CRWriteCharInterval;  
  {0},                           //CRSerialDevData
  {0}                            //CrDevDefaultAttr
};


/**
 Initialize Console Redirection Devices default setting .

 @param CrPolicy                 The give Console Redirection policy

 @retval EFI_SUCCESS            Success to set Console Redirection Devices default setting
 @return Other                      Fail to set default setting.

**/
EFI_STATUS
InitCrDevDefaultAttribute (
  IN OUT  EFI_CR_POLICY_PROTOCOL    *CrPolicy
  )
{
  CR_DEVICE_ATTRIBUTES    *CrDevAttri;
  UINTN                   Idx;

  for (Idx = 0; Idx < CR_MAX_SUPPORT_SERIAL_PORT; Idx++) {
    CrDevAttri = &CrPolicy->CrDevDefaultAttr[Idx];
    
    // Only enable serial port 0 in default setting
    if (Idx == 0)
      CrDevAttri->PortEnable = CR_ENABLE;
    else
      CrDevAttri->PortEnable = CR_DISABLE;

    CrDevAttri->UseGlobalSetting = CR_ENABLE;
    CrDevAttri->TerminalType     = CR_TERMINAL_VT100;
    CrDevAttri->BaudRate         = 7;//115200
    CrDevAttri->Parity           = CR_PARITY_NONE;
    CrDevAttri->DataBits         = CR_DATA_8BIT;
    CrDevAttri->StopBits         = CR_STOP_1BIT;
    CrDevAttri->FlowControl      = CR_FLOW_CONTROL_NONE;
  }

  return EFI_SUCCESS;
};


/**
 Install Driver to produce CrPolicy protocol.

 @param    ImageHandle
 @param    SystemTable

 @retval EFI_SUCCESS            CrPolicy protocol installed
 @return Other                       No protocol installed.

**/
EFI_STATUS
EFIAPI
CRPolicyEntryPoint (
  IN EFI_HANDLE                         ImageHandle,
  IN EFI_SYSTEM_TABLE                   *SystemTable
  )
{
  EFI_STATUS                            Status;
  EFI_CR_POLICY_PROTOCOL                *CrPolicy;
  KERNEL_CONFIGURATION                  KernelConfig;
  CR_CONFIGURATION                      *CrConfig;
  
  //
  // Create CrPolicy with default setting
  //
  CrPolicy = (EFI_CR_POLICY_PROTOCOL*)AllocateCopyPool (sizeof (EFI_CR_POLICY_PROTOCOL), &mCRPolicyTemplate);

  Status = GetKernelConfiguration (&KernelConfig);
  if (Status == EFI_SUCCESS) {
    //
    // base on SCU setting to update CrPolicy
    //
    CrPolicy->CREnable       = KernelConfig.CREnable;
    CrPolicy->CRAfterPost    = KernelConfig.CRAfterPost;
    CrPolicy->CRSerialPort   = KernelConfig.CRSerialPort;
    CrPolicy->CRInfoWaitTime = KernelConfig.CRInfoWaitTime;
    CrPolicy->AcpiSpcr       = KernelConfig.ACPISpcr;
    switch (KernelConfig.CRTextModeResolution) {

      case CR_TEXT_MODE_AUTO:
        CrPolicy->CRForce80x25 = FALSE; 
        break;  

      case CR_TEXT_MODE_80X25:
        CrPolicy->CRForce80x25 = TRUE;
        CrPolicy->CRTerminalRows = CR_TERMINAL_ROW_25;
        break;

      case CR_TEXT_MODE_80X24DF:
        CrPolicy->CRForce80x25 = TRUE;      
        CrPolicy->CRTerminalRows = CR_TERMINAL_ROW_24;
        CrPolicy->CR24RowsPolicy = CR_24ROWS_POLICY_DEL_FIRST;      
        break;

      case CR_TEXT_MODE_80X24DL:
        CrPolicy->CRForce80x25 = TRUE;
        CrPolicy->CRTerminalRows = CR_TERMINAL_ROW_24;
        CrPolicy->CR24RowsPolicy = CR_24ROWS_POLICY_DEL_LAST;
        break;
      
      default:
        break;
    }
  }

  
  CrConfig = CommonGetVariableData (L"CrConfig", &gCrConfigurationGuid);
  if (CrConfig != NULL) {
    CrPolicy->CRTerminalType     = CrConfig->GlobalTerminalType;
    CrPolicy->CRBaudRate         = CrConfig->GlobalBaudRate;
    CrPolicy->CRDataBits         = CrConfig->GlobalDataBits;
    CrPolicy->CRStopBits         = CrConfig->GlobalStopBits;
    CrPolicy->CRParity           = CrConfig->GlobalParity;
    CrPolicy->CRFlowControl      = CrConfig->GlobalFlowControl;
    CrPolicy->CRAutoRefresh      = CrConfig->AutoRefresh == 1 ? CR_AUTO_REFRESH_ENABLE : CR_AUTO_REFRESH_DISABLE;
    CrPolicy->CRFailSaveBaudrate = CrConfig->FailSaveBaudRate == 1 ? TRUE : FALSE;     
  }
  //
  // init CrDevDefaultAttr 
  //
  InitCrDevDefaultAttribute (CrPolicy); 

  //
  // base on Oem setting to update CrPolicy 
  //
  Status = OemSvcUpdateCRPolicy (CrPolicy);
  if (Status == EFI_SUCCESS) {
    return Status;
  }
    
  //
  // Due to HyperTerm don't support graphic CharSet, 
  // if CRForce80x25 as TRUE, always use ASCII CharSet
  //
  if (CrPolicy->CRForce80x25) {
    CrPolicy->CRTerminalCharSet = CR_TERMINAL_CHARSET_ASCII;
  } else {  
    CrPolicy->CRTerminalCharSet = CR_TERMINAL_CHARSET_GRAPHIC;
  }

  if (((CrPolicy->CRAsyncTerm & CR_ASYNC_TERMINAL_FLAG) == CR_ASYNC_TERMINAL_DISABLE) ||
       (FeaturePcdGet(PcdCRSrvManagerSupported))) {
    CrPolicy->CRAutoRefresh = CR_AUTO_REFRESH_DISABLE; 
  }


  //
  //CrPolicy : if AutoRefresh support, the ManualRefresh need always support
  //
  if ((CrPolicy->CRAutoRefresh & CR_AUTO_REFRESH_FLAG) == CR_AUTO_REFRESH_ENABLE) {
    CrPolicy->CRManualRefresh = CR_MANUAL_REFRESH_ENABLE; 
  }
  
  //
  // Install CRPolicy Protocol
  //
  Status = gBS->InstallProtocolInterface (
                  &ImageHandle,
                  &gEfiCRPolicyProtocolGuid,
                  EFI_NATIVE_INTERFACE,
                  CrPolicy
                  );
  
  return Status;
}
