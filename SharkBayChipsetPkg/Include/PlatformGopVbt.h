//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _PLATFORM_GOP_VBT_H_
#define _PLATFORM_GOP_VBT_H_

//
// The following definition are project-specific, 
// The different GOP Vbt may has different definition.
//
//======================Start==============================//

//
// the Selected Display Port offset of Vbt table. 
//
#define VBT_EFP1_OFFSET  0x1A9
#define VBT_EFP2_OFFSET  0x1CA
#define VBT_EFP3_OFFSET  0x1EB
//
// The Display Port Type of Vbt Table.  
//
typedef enum {
  VbtNoneDevice = 0,
  VbtHDMIB, 
  VbtHDMIC,
  VbtHDMID,
  VbtDPB = 7,
  VbtDPC,
  VbtDPD
} VBT_EFP_DISPLAY_TYPE;
//
// Child list Display Type define.
//
#define VBT_MAX_NUM_OF_CHILD_DEVICE 0x10

#define VBT_CHILD_DEVICE_1_OFFSET  0x13F
#define VBT_NEXT_CHILD_DEVICE      0x03

//[-start-130401-IB05400394-add]//
#define VBT_CHILD_DEVICE_PRIMARY(x)      (VBT_CHILD_DEVICE_1_OFFSET + VBT_NEXT_CHILD_DEVICE * (x))
#define VBT_CHILD_DEVICE_SECONDARY(x)    (VBT_CHILD_DEVICE_1_OFFSET + 1 + VBT_NEXT_CHILD_DEVICE * (x))
//[-end-130401-IB05400394-add]//

#define VBT_CHILD_DEVICE_TYPE_NA   0x00
#define VBT_CHILD_DEVICE_TYPE_LFP  0x08
#define VBT_CHILD_DEVICE_TYPE_CRT  0x01
#define VBT_CHILD_DEVICE_TYPE_EFP  0x04
#define VBT_CHILD_DEVICE_TYPE_EFP3 0x20
#define VBT_CHILD_DEVICE_TYPE_EFP2 0x40

//======================End==============================//

#endif
