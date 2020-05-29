/** @file
  Provide OEM to switch the CSM16 function.

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
   Provide OEM to switch the CSM16 function. Current functions of CSM16 as following: 
   
    Bit         Function	                                Disable bit (set to 0)	               Enable bit (set to 1)
   --------------------------------------------------------------------------------------------------------------
    0           RESERVED                                         
    1,          POST_TIME_DISABLE_KEYBOARD                0: Enable KeyboardReset                          1: Disable KeyboardReset 
    2,          POST_TIME_DISABLE_KEYBOARD_BEEP           0: Enable KeyboardBeep                           1: Disable KeyboardBeep
    3,          LEGACY_AUTO_BOOT_RETRY                    0: Disable AutoBootRetry                         1: Enable AutoBootRetry
    4,          ODD_IMAGE_DETECT                          0: Detect ODD image before INT19                 1: Detect ODD image after INT19
    5,          SET_VGAMODE_BEFORE_INT19                  0: Disable                                       1: Set Vga mode before INT19
    6,          ENABLE_A20_BEFORE_INT19                   0: Disable                                       1: Enable A20 before INT19
    7,          A20_GATE_SUPPORT                          0: supported on keyboard controller              1: supported with bit 1 of I/O port 92h  
    8,          MONITORKEY_SUPPORT_IN_OPROM               0: Disable MonitorKey during dispatch OPROM      1: Enable MonitorKey during dispatch OPROM
    9           PAUSE_KEY_SUPPORT                         0: Enable Pause Key                              1: Disable Pause Key 
    10,         SMBIOS_DATA_LOCATE                        0: Locate at E segment                           1: Locate at F segment
    11 ~ 13     For PMM high memory size
                  if the value is 0-> 4 MB (default)
                  if the value is 1-> 2 to the (4 + 1) = 32 MB  
                  if the value is 2-> 2 to the (4 + 2) = 64 MB
                  if the value is 3-> 2 to the (4 + 3) = 128 MB
                  if the value is 4-> 2 to the (4 + 4) = 256 MB
                  if the value is 5-> 2 to the (4 + 5) = 512 MB
                  if the value is 6-> 2 to the (4 + 6) = 1024 MB
                  if the value is 7-> 2 to the (4 + 7) = 2048 MB  
    14 ~ 15     RESERVED
   --------------------------------------------------------------------------------------------------------------   

  @param  *CSM16OemSwitchPtr    Point to the CSM16OemSwitch of H2O_TABLE in LegacyBios.

  @retval EFI_SUCCESS           Set the CSM16 OEM switch success.
  @retval Others                Base on OEM design.
**/
EFI_STATUS
OemSvcCsm16ReferSwitchThunk (
  IN OEM_SERVICES_PROTOCOL             *OemServices,
  IN UINTN                             NumOfArgs,
  ...
  )
{
  VA_LIST                               Marker;
  UINT16                                *Csm16OemSwitchPtr;
  EFI_STATUS                            Status;

  VA_START (Marker, NumOfArgs);
  Csm16OemSwitchPtr = VA_ARG (Marker, UINT16 *);   
  VA_END (Marker);

  //
  // make a call to DxeOemSvcKernelLib internally
  //
  Status = OemSvcCsm16ReferSwitch (Csm16OemSwitchPtr);
  
  return Status;
}
