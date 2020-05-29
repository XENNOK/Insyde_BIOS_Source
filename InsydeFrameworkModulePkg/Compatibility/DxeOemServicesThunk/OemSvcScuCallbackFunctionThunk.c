/** @file
  Provide OEM to modify the hot key callback function in SCU.

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

//
// Note: the service is unsupported in Rev.5.0
//

/**
  Provide OEM to modify the hot key callback function in SCU.

  Note: The service is called following the HotKeyCallback () function
        and all parameters of this service can cover by HotKeyCallback () function.
        Also, HotKeyCallback () function belongs to platform layer.
        Base on above points, this service in fact can be covered by HotKeyCallback () function.

  @param  *This                 Pointer to Form Callback protocol instance.
  @param  KeyValue              A unique value which is sent to the original exporting driver so that it
                                can identify the type of data to expect. The format of the data tends to
	                            vary based on the op-code that generated the callback.
  @param  *Data                 A pointer to the data being sent to the original exporting driver.
  @param  **Packet              A pointer to a packet of information which a driver passes back to the browser.
  @param  *MyIfrNVData          A pointer to SYSTEM_CONFIGURATION.
  @param  *UsePacket            A pointer to the status of the using of Packet.
  @param  *HiiHandle            Point to Hii handle.
  @param  *Broswer              A point to EFI_FORM_BROWSER_PROTOCOL.

  @retval EFI_SUCCESS           Get the Packet success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcScuCallbackFunctionThunk (
  IN OEM_SERVICES_PROTOCOL                 *OemServices,
  IN  UINTN                                NumOfArgs,
  ...
  )
{
  return EFI_UNSUPPORTED;
}
