/** @file

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

#ifndef _AZALIA_POLICY_H_
#define _AZALIA_POLICY_H_

#define AZALIA_POLICY_GUID \
  { 0xE197BE68, 0x9AE2, 0x448E, 0xAC, 0x56, 0xA4, 0x70, 0x82, 0x5F, 0xD8, 0xD8 }


//
// The structure definition PEI_AZALIA_POLICY_PPI will follow PCH_AZALIA_CONFIG structure definition
// in PchPlatformPolicy.h. Because of the verb table structure definition had been unified by Insyde,
// PCH_AZALIA_VERB_TABLE is useless structure and is redefined to "UINT8" here.
//
#define PCH_AZALIA_VERB_TABLE UINT8

typedef struct _AZALIA_POLICY {
  UINT8                 Pme       : 1;      // 0: Disable; 1: Enable
  UINT8                 DS        : 1;      // 0: Docking is not supported; 1:Docking is supported
  UINT8                 DA        : 1;      // 0: Docking is not attached; 1:Docking is attached
//[-start-120606-IB04770232-modify]//
//   UINT8                 HdmiCodec : 1;      // 0: Disable; 1: Enable
  UINT8                 Rsvdbits  : 5;
//[-end-120606-IB04770232-modify]//
  UINT8                 AzaliaVerbTableNum; // Number of verb tables provided by platform
  PCH_AZALIA_VERB_TABLE *AzaliaVerbTable;   // Pointer to the actual verb table(s)
  UINT16                ResetWaitTimer;     // The delay timer after Azalia reset, the value is number of microseconds
} AZALIA_POLICY;

//[-start-130218-IB07250301-remove]//
//#ifndef _PEI_HOB_H_
//#ifndef __HOB__H__
//typedef struct _EFI_HOB_GENERIC_HEADER {
//  UINT16                    HobType;
//  UINT16                    HobLength;
//  UINT32                    Reserved;
//} EFI_HOB_GENERIC_HEADER;
//
//typedef struct _EFI_HOB_GUID_TYPE {
//  EFI_HOB_GENERIC_HEADER    Header;
//  EFI_GUID                  Name;
//  //
//  // Guid specific data goes here
//  //
//} EFI_HOB_GUID_TYPE;
//#endif
//#endif
//[-end-130218-IB07250301-remove]//

typedef struct _AZALIA_INFO_HOB_DATA {
  UINT32                    AzaliaStatus;
  BOOLEAN                   AzaliaEnable;                // 0 : Disable  1 : Enable
} AZALIA_INFO_HOB_DATA;

typedef struct _AZALIA_INFO_HOB {
  EFI_HOB_GUID_TYPE         EfiHobGuidType;
  AZALIA_INFO_HOB_DATA      AzaliaInfoHobData;
} AZALIA_INFO_HOB;

extern EFI_GUID gAzaliaPolicyGuid;

#endif
