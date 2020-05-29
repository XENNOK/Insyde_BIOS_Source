/** @file

  Bios Event Log implementation.

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

#include "BiosEventLog.h"

#ifdef H2OIPMI_SUPPORT
EFI_STATUS
CheckBmcSelOverWriteEnable(
  VOID
  )
/*++

Routine Description:

  Check if BMC SEL support "DELETE SEL" command.
  If yes, we can implement the "Overwrite" function.
  
Arguments:

  VOID
  
Returns:

  EFI_STATUS
  
--*/
{
  EFI_STATUS                            Status;
  H2O_IPMI_INTERFACE_PROTOCOL           *Ipmi = NULL;
  UINT8                                 RecvSize = MAX_BUFFER_SIZE;
  UINT8                                 RecvBuf[MAX_BUFFER_SIZE];
  H2O_IPMI_SEL_INFO                     *IpmiSelInfo;

  Status  = gBS->LocateProtocol (&gH2OIpmiInterfaceProtocolGuid, NULL, (VOID **)&Ipmi);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  RecvSize = MAX_BUFFER_SIZE;
  Status = GetBmcSelInfo (Ipmi, RecvBuf, &RecvSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  IpmiSelInfo = (H2O_IPMI_SEL_INFO *)RecvBuf;

  if ((IpmiSelInfo->OperationSupport & DELETE_SEL_CMD_SUPPORTED) == 0) {
    //
    // Delete SEL cmd not supported.
    //
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

EFI_STATUS
GetBmcSelInfo (
  IN H2O_IPMI_INTERFACE_PROTOCOL  *IpmiTransport,
  IN UINT8                        *RecvBuf,
  IN UINT8                        *RecvSize
  )
/*++

Routine Description:

  Excute IPMI CMD to get BMC SEL information.
  
Arguments:

  IpmiTransport   H2O_IPMI_INTERFACE_PROTOCOL interface   
  RecvBuf         BmcSel information.
  RecvSize
  
Returns:

  EFI_STATUS
  
--*/
{
  EFI_STATUS                      Status;
  H2O_IPMI_CMD_HEADER             Request = { H2O_IPMI_BMC_LUN,
                                              H2O_IPMI_NETFN_STORAGE,
                                              H2O_IPMI_CMD_GET_SEL_INFO
                                              };

  Status = IpmiTransport->ExecuteIpmiCmd (
                                      IpmiTransport,
                                      Request,
                                      NULL,
                                      0,
                                      RecvBuf,
                                      RecvSize,
                                      NULL
                                      );
  return Status;
}
#endif

