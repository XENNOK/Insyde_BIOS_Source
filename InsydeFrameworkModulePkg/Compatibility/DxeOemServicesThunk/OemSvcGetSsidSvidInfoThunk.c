/** @file
  Provide OEM to decide what SSID/SVID to use.

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
  The OemService is used by OEM to mainly decide what SSID/SVID to use. 
  The service provides two methods to programming SSID/SVID. 
  The method one is that common chipset code will skip device programming, when the service returns EFI_UNSUPPORTED. 
  The other is that OEM checks SKU and return the SSID/SVID when the service returns EFI_SUCCESS.

  @param  Bus 			        Bus number.
  @param  Dev			        Device number.
  @param  Func                	Function number. 
  @param  VendorID            	Vendor ID.
  @param  DeviceID            	Device ID.
  @param  ClassCode           	Class Code.
  @param  *SsidSvid            	Point to SSID\SVID.

  @retval EFI_SUCCESS           Platform/Chipset/Common Code will programming the SSID/SVID depending on the parameter "SsidSvid".
  @retval EFI_UNSUPPORTED       Common chipset code will skip device programming.
**/
EFI_STATUS
OemSvcGetSsidSvidInfoThunk (
  IN     OEM_SERVICES_PROTOCOL        *This,
  IN     UINTN                        NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINT8                                 Bus;
  UINT8                                 Dev;
  UINT8                                 Func;
  UINT16                                VendorId;
  UINT16                                DeviceId;
  UINT16                                ClassCode;
  UINT32                                *SsidSvid;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  Bus                   = VA_ARG ( Marker, UINT8);
  Dev                   = VA_ARG ( Marker, UINT8);
  Func                  = VA_ARG ( Marker, UINT8);
  VendorId              = VA_ARG ( Marker, UINT16);
  DeviceId              = VA_ARG ( Marker, UINT16);
  ClassCode             = VA_ARG ( Marker, UINT16);
  SsidSvid              = VA_ARG ( Marker, UINT32 *);
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcUpdateSsidSvidInfo (
             Bus,
             Dev,
             Func,
             VendorId,
             DeviceId,
             ClassCode,
             SsidSvid
             );

  return Status;
}
