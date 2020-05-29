//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name :
//;
//;   NonCSMSupport.h
//;

#ifndef _NON_CSM_SUPPORT_H_
#define _NON_CSM_SUPPORT_H_

//
// { A8512B1D-C4AD-4376-A144-DC772BB9B646}
//
#define NON_CSM_SUPPORT_PROTOCOL_GUID \
  { \
    0xa8512b1d, 0xc4ad, 0x4376, 0xa1, 0x44, 0xdc, 0x77, 0x2b, 0xb9, 0xb6, 0x46 \
  }

//
// Extern the GUID for protocol users.
//
extern EFI_GUID      gNonCSMSupportProtocolGuid;

#endif
