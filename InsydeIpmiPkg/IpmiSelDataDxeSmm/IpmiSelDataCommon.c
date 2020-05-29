/** @file
 H2O IPMI SEL Data module common functions implement code.

 This c file contains common functions for SEL Data module in DXE/SMM phase.
 
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

#include <IpmiSelDataCommon.h>



//
// Global for the handle that the SelInfo Protocol is installed
//
H2O_IPMI_SEL_DATA_PROTOCOL   mSelDataProtocol;


/**
 Get Sel Data of SelId.

 @param[in]         This                Pointer to H2O_IPMI_SEL_DATA_PROTOCOL instance.
 @param[in]         SelId               The SEL record ID to get which sel data.
 @param[out]        SelData             A buffer to H2O_IPMI_SEL_ENTRY.
 
 @retval EFI_SUCCESS                    Get SEL data success.
 @retval EFI_UNSUPPORTED                Get SEL data fail or speificed Id does not exist.
*/
EFI_STATUS
EFIAPI
GetSelData (
  IN  H2O_IPMI_SEL_DATA_PROTOCOL    *This,
  IN  UINT16                        SelId,
  OUT H2O_IPMI_SEL_ENTRY            *SelData
  )
{
  EFI_STATUS                      Status;
  UINT8                           SendDataSize;
  UINT8                           SendData[6];
  UINT8                           RecvDataSize;


  ZeroMem (SendData, (sizeof (SendData)));
  CopyMem (&SendData[2], &SelId, sizeof(SelId));
  SendData[4] = 0;
  SendData[5] = 0xFF;
  SendDataSize = 6;

  Status = EFI_SUCCESS;
  Status = IpmiLibExecuteIpmiCmd (
             H2O_IPMI_NETFN_STORAGE,
             H2O_IPMI_CMD_GET_SEL_ENTRY,
             SendData,
             SendDataSize,
             SelData,
             &RecvDataSize
             );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  return Status;
}


/**
 Get All Sel entry data.

 @param[in]         This                Pointer to H2O_IPMI_SEL_DATA_PROTOCOL instance.
 @param[out]        SelCount            Number of log entries in SEL.
 @param[out]        SelData             A H2O_IPMI_SEL_ENTRY array to store all H2O_IPMI_SEL_ENTRY.
 
 @retval EFI_SUCCESS                    Get SEL data success.
 @retval EFI_UNSUPPORTED                Get SEL data fail.
*/
EFI_STATUS
EFIAPI
GetAllSelData (
  IN  H2O_IPMI_SEL_DATA_PROTOCOL    *This,
  OUT UINT16                        *SelCount,
  OUT H2O_IPMI_SEL_ENTRY            **SelData
  )
{
  EFI_STATUS                Status;
  UINT8                     SendDataSize;
  UINT8                     SendData[6];
  UINT8                     RecvDataSize;
  UINT16                    EntryCount;
  UINT16                    NextId;
  H2O_IPMI_SEL_INFO         SelInfo;
  H2O_IPMI_SEL_ENTRY        *SelBuffer;
  

  Status = EFI_SUCCESS;
  SendDataSize = 0;
  ZeroMem (SendData, (sizeof (SendData)));
  EntryCount = 0;
  Status = IpmiLibExecuteIpmiCmd (
             H2O_IPMI_NETFN_STORAGE,
             H2O_IPMI_CMD_GET_SEL_ENTRY,
             SendData,
             SendDataSize,
             &SelInfo,
             &RecvDataSize
             );
  if (EFI_ERROR (Status)) {
    return EFI_UNSUPPORTED;
  }
  EntryCount = SelInfo.SelEntries;

  SelBuffer = (H2O_IPMI_SEL_ENTRY*)AllocatePool (sizeof(H2O_IPMI_SEL_ENTRY) * EntryCount);
  if (SelBuffer == NULL) {
    return EFI_UNSUPPORTED;
  }

  *SelCount = 0;
  NextId = 0x0000;
  while (EntryCount > 0) {
    GetSelData (This, NextId, &SelBuffer[*SelCount]);
    NextId = SelBuffer[*SelCount].NextRecordId;
    EntryCount--;
    (*SelCount)++;
    if (NextId == 0xFFFF) {
      break;
    }
  }
  *SelData = SelBuffer;
  return Status;
}


/**
 Fill private data content.

*/
VOID
InitialIpmiSelDataContent (
  VOID
  )
{
  mSelDataProtocol.GetAllSelData = GetAllSelData;
  mSelDataProtocol.GetSelData = GetSelData;
}

