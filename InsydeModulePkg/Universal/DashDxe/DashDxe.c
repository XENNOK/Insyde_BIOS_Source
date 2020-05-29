/** @file
  Driver for DASH Management Protocols

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "DashDxe.h"


DASH_CONTEXT mDashContext;



/**
 Driver entry point

 @param [in]   ImageHandle      The image handle.
 @param [in]   SystemTable      The system table.

 @retval EFI_SUCCESS            Command completed successfully
 @return Other value              Unknown error

**/
EFI_STATUS
EFIAPI 
DashEntry (
  IN EFI_HANDLE       ImageHandle,
  IN EFI_SYSTEM_TABLE *SystemTable
  )
{
  EFI_STATUS              Status;
  MCTP_SMBUS_PROTOCOL     *MctpSmbus;
  MCTP_BUS_CONTEXT        *MctpBusContext;
  MCTP_CONTEXT            *MctpContext;
  PLDM_CONTEXT            *PldmContext;
  DASH_IO_CFG_PROTOCOL         *DashIoCfg;
  

  Status = gBS->LocateProtocol (&gDashIoCfgProtocolGuid, NULL, (VOID **)&DashIoCfg);
  if(EFI_ERROR(Status)) {
    return Status;
  }

  Status = gBS->LocateProtocol (&gMctpSmbusProtocolGuid, NULL, (VOID **)&MctpSmbus);
  if(EFI_ERROR(Status)) {
    return Status;
  }

  DashIoCfg->InitAsf(DashIoCfg);
  
  MctpBusContext   = &mDashContext.MctpBusContext;
  MctpContext      = &mDashContext.MctpContext;
  PldmContext      = &mDashContext.PldmContext;
  

  Status = SetupMctpBusProtocol (MctpBusContext, MctpSmbus);
  EFI_ERROR_RETURN (Status);
  
  Status = SetupMctpProtocol (MctpContext, &MctpBusContext->MctpBus);
  EFI_ERROR_RETURN (Status);

  Status = SetupPldmProtocol (PldmContext, &MctpContext->Mctp);
  EFI_ERROR_RETURN (Status);

  
  //
  // Protocol interface installation
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                                           &ImageHandle,
                                           &gMctpBusProtocolGuid,
                                           &MctpBusContext->MctpBus,
                                           &gMctpProtocolGuid,
                                           &MctpContext->Mctp,
                                           &gPldmProtocolGuid,
                                           &PldmContext->Pldm, 
                                           NULL
                                         );            
  return Status;
}
