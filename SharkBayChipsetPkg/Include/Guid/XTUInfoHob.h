//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
This file contains an 'Intel Peripheral Driver' and uniquely
identified as "Intel Mobile Silicon Support Module" and is
licensed for Intel Mobile CPUs and chipsets under the terms of your
license agreement with Intel or your vendor.  This file may
be modified by the user, subject to additional terms of the
license agreement
--*/
/*++

Copyright (c) 2006-2009 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  XTUInfoHob.h

Abstract:

  This file contains definitions required for creation of 
  XTU Info HOB.

--*/
#ifndef _XTU_HOB_H_
#define _XTU_HOB_H_
//[-start-120808-IB10820098-modify]//
#define XTU_INFO_HOB_GUID \
  { \
    0x8174b45b, 0x805e, 0x4682, {0x9d, 0x62, 0xae, 0x95, 0x9e, 0x82, 0xa2, 0x13 }\
  }
//[-end-120808-IB10820098-modify]//
extern EFI_GUID gXTUInfoHobGuid;

#pragma pack(push, 1)

#ifndef _PEI_HOB_H_
#ifndef __HOB__H__
typedef struct _EFI_HOB_GENERIC_HEADER {
  UINT16  HobType;
  UINT16  HobLength;
  UINT32  Reserved;
} EFI_HOB_GENERIC_HEADER;

typedef struct _EFI_HOB_GUID_TYPE {
  EFI_HOB_GENERIC_HEADER  Header;
  EFI_GUID                Name;
  //
  // Guid specific data goes here
  //
} EFI_HOB_GUID_TYPE;
#endif
#endif


typedef struct {
  UINT8                       LimitCore1;
  UINT8                       LimitCore2;
  UINT8                       LimitCore3;
  UINT8                       LimitCore4;
//[-start-120215-IB03780423-modify]//
} TURBO_RATIO_LIMIT;
//[-end-120215-IB03780423-modify]//

typedef struct { 
  UINT8                       MaxRatio;
  UINT8                       MinRatio;
} CPU_NON_TURBO_RATIO;

typedef struct {
  UINT16                      PowerLimit1Default;
  UINT16                      PowerLimit2Default;
} TURBO_TDP_LIMIT_DEFAULT;

typedef struct {
  UINT16                      MaxSupportFreq;
  UINT16                      MinSupportFreq;
  UINT16                      CurrentFreq;
} HOST_CLOCK_DATA;

//[-start-121129-IB03780468-modify]//
typedef struct {
  UINT16                      DdrMultiplier;
  UINT16                      DdrRefClk;
  UINT16                      tCL;
  UINT16                      tRCD;
  UINT16                      tRP;
  UINT16                      tRAS;
  UINT16                      tWR;
  UINT16                      tRFC;  
  UINT16                      tRRD;
  UINT16                      tWTR;
  UINT16                      tRTP;
  UINT16                      tRC;  
  UINT16                      tFAW;
  UINT16                      tCWL;
  UINT16                      tREFI;
  UINT16                      tRPAB;
  UINT16                      NMode; 
} PROFILE_TIMING_DATA;
//[-end-121129-IB03780468-modify]//

typedef struct {   
  PROFILE_TIMING_DATA         Default;
  PROFILE_TIMING_DATA         Profile1;
  PROFILE_TIMING_DATA         Profile2;
} XMP_PROFILE;

typedef struct {   
  TURBO_RATIO_LIMIT           TurboRatioLimitDefault;
//[-start-120215-IB03780423-add]//
  TURBO_RATIO_LIMIT           TurboRatioLimitMaximum;
//[-end-120215-IB03780423-add]//
  CPU_NON_TURBO_RATIO         CpuNonTurboRatio;
  TURBO_TDP_LIMIT_DEFAULT     TruboTdpLimitDefault;
  HOST_CLOCK_DATA             HostClockData;
  XMP_PROFILE                 XmpProfile;  
} XTU_INFO_DATA;


typedef struct {
  EFI_HOB_GUID_TYPE           EfiHobGuidType;
  XTU_INFO_DATA               Data;
} XTU_INFO_HOB;

#pragma pack(pop)


#endif
