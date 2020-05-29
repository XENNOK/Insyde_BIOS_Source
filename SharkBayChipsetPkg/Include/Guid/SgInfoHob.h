/** @file
  This file contains definitions required for creation of SG Information data HOB.

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

#ifndef _SG_INFORMATION_DATA_HOB_H_
#define _SG_INFORMATION_DATA_HOB_H_

//
// {648CE07B-AE5D-4973-BD3C-8C9153C05DC5}
//
#define SG_INFOFORMATION_DATA_HOB_GUID \
  { \
    0x648ce07b, 0xae5d, 0x4973, 0xbd, 0x3c, 0x8c, 0x91, 0x53, 0xc0, 0x5d, 0xc5 \
  }

extern EFI_GUID                               gH2OSgInformationDataHobGuid;

#pragma pack (push, 1)
typedef struct _SG_INFORMATION_DATA_HOB {
  EFI_HOB_GUID_TYPE                           EfiHobGuidType;
  EFI_GUID                                    SgModeMxmBinaryGuid;
  EFI_GUID                                    PegModeMasterMxmBinaryGuid;
  EFI_GUID                                    PegModeSlaveMxmBinaryGuid;
  EFI_GUID                                    AmdDiscreteSsdtGuid;
  EFI_GUID                                    AmdPowerXpressSsdtGuid;
  EFI_GUID                                    AmdUltPowerXpressSsdtGuid;
  EFI_GUID                                    NvidiaDiscreteSsdtGuid;
  EFI_GUID                                    NvidiaOptimusSsdtGuid;
  EFI_GUID                                    NvidiaUltOptimusSsdtGuid;
  UINT16                                      ActiveDgpuPwrEnableDelay;
  UINT16                                      ActiveDgpuHoldRstDelay;
  UINT16                                      InactiveDgpuHoldRstDelay;
  UINT8                                       SgDgpuPwrOk;
  UINT8                                       SgDgpuHoldRst;
  UINT8                                       SgDgpuEdidSel;
  UINT8                                       SgDgpuSel;
  UINT8                                       SgDgpuPwmSel;
  UINT8                                       SgDgpuPwrEnable;
  UINT8                                       SgDgpuPrsnt;
  UINT8                                       SgSlaveDgpuPwrOk;
  UINT8                                       SgSlaveDgpuHoldRst;
  UINT8                                       SgSlaveDgpuPwrEnable;
  UINT8                                       SgSlaveDgpuPrsnt;
  UINT8                                       SgMode;
  UINT8                                       BootType;
  UINT8                                       PowerXpressSetting;
  UINT8                                       PannelScaling;
  UINT8                                       DgpuBridgeBus;
  UINT8                                       DgpuBridgeDevice;
  UINT8                                       DgpuBridgeFunction;
  UINT8                                       SlaveDgpuBridgeBus;
  UINT8                                       SlaveDgpuBridgeDevice;
  UINT8                                       SlaveDgpuBridgeFunction;
  UINT8                                       AmdSecondaryGrcphicsCommandRegister;
  UINT8                                       NvidiaSecondaryGrcphicsCommandRegister;
  BOOLEAN                                     SgGpioSupport;
  BOOLEAN                                     SlaveMxmGpuSupport;
  BOOLEAN                                     SgDgpuEdidSelExist;
  BOOLEAN                                     SgDgpuSelExist;
  BOOLEAN                                     SgDgpuPwmSelExist;
  BOOLEAN                                     SgDgpuPrsntExist;
  BOOLEAN                                     SgSlaveDgpuPrsntExist;
  BOOLEAN                                     IsUltBoard;
  BOOLEAN                                     OptimusDgpuHotPlugSupport;
  BOOLEAN                                     OptimusDgpuPowerControlSupport;
  BOOLEAN                                     GpsFeatureSupport;
  BOOLEAN                                     VenturaFeatureSupport;
  BOOLEAN                                     OptimusGc6FeatureSupport;
} SG_INFORMATION_DATA_HOB;
#pragma pack (pop)

#endif
