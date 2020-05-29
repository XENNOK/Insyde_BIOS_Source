/** @file
  Provide OEM to get the HotPlugBridgeInfo table.

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

#include "DxeOemServicesThunk.h"
#include <Library/DxeOemSvcKernelLib.h>


/**
  Provide OEM to get the HotPlugBridgeInfo table. 
  This table contains the hot plug bridge address info and defines reserved BUS, I/O and memory range for bridge device.

  @param  *HotPlugBridgeInfoTable  Point to HOT_PLUG_BRIDGE_INFO Table

  @retval EFI_SUCCESS              Get hot plug bridge info success.
  @retval Others                   Base on OEM design.
**/
EFI_STATUS
OemSvcGetHotplugBridgeInfoThunk (
  IN OEM_SERVICES_PROTOCOL              *OemServices,
  IN UINTN                              NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  HOT_PLUG_BRIDGE_INFO                  **HotPlugBridgeInfoTable;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  HotPlugBridgeInfoTable = VA_ARG (Marker, HOT_PLUG_BRIDGE_INFO **);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcGetHotplugBridgeInfo (HotPlugBridgeInfoTable);

  return Status;
}
