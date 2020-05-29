/** @file

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

#ifndef _SIO_LIBRARY_H_
#define _SIO_LIBRARY_H_

#include <Sio/SioCommon.h>

/**
  Pass in informatioin to find device resource from PCD data.  

  @param[in]       SioInstance           Which SIO pass in.
  @param[in, out]  *TableList            PCD data pointer.
  @param[in]       TypeInstance          SIO instance.
  @param[in]       Device                What kind of the device.
  @param[in]       DeviceInstance        Device instance.

  @retval          PcdPointer            If found device, it'll return pointer or else it return NULL. 
**/
SIO_DEVICE_LIST_TABLE*
FindSioDevice (
  SIO_DEVICE_LIST_TABLE    *PcdPointer,
  UINT8                    TypeInstance,
  UINT8                    Device,
  UINT8                    DeviceInstance
  ); 

/**
  Word to chage place for high byte, low byte
   
  @param[in] Value    The number need to chage.

  @retval    Value    Function complete successfully. 
**/
UINT16
UpsideDown (
  IN UINT16                           Value
  );

#endif
