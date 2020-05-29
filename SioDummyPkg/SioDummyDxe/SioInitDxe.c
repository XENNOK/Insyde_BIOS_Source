/** @file

Lock unlock sio and structure of install device protocol

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SioInitDxe.h"

//
// Provide mSioResourceFunction Protocol Interface 
//
EFI_SIO_RESOURCE_FUNCTION mSioResourceFunction[] = {
  NULL_ID,
  SIO_ID1,
  SIO_ID2,
  NULL_ID,
  FALSE,
};

extern EFI_STATUS InstallPs2KProtocol (SIO_DEVICE_LIST_TABLE* Resource);
extern EFI_STATUS InstallPs2MProtocol (SIO_DEVICE_LIST_TABLE* Resource);
extern EFI_STATUS InstallCom1Protocol (SIO_DEVICE_LIST_TABLE* Resource);
extern EFI_STATUS InstallCom2Protocol (SIO_DEVICE_LIST_TABLE* Resource);

//
// Provide DeviceProtocolFunction Table
//
EFI_INSTALL_DEVICE_FUNCTION mDeviceFunction[] = {
  {COM,     DEVICE_INSTANCE0, 0, InstallCom1Protocol},
  {COM,     DEVICE_INSTANCE1, 0, InstallCom2Protocol},  
  {KYBD,    DEVICE_INSTANCE0, 0, InstallPs2KProtocol},
  {MOUSE,   DEVICE_INSTANCE1, 0, InstallPs2MProtocol}, 
  {NULL_ID, 0,                0, NULL},
};

EFI_SIO_TABLE mSioTable[] = {
  //======Default Start======//

  //====== Default End ======//
  {0    ,0    }
};

