/** @file



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

#ifndef _EXIT_AT_AUTH_PROTOCOL_H_
#define _EXIT_AT_AUTH_PROTOCOL_H_

#include <Uefi.h>

#define EXIT_AT_AUTH_PROTOCOL_GUID \
  { 0x4BC2BF10, 0x31D9, 0x49CA, { 0xBF, 0x6C, 0x24, 0x75, 0x51, 0xBD, 0x01, 0x88 }}

extern EFI_GUID gExitAtAuthProtocolGuid;

typedef struct _EFI_EXIT_AT_PROTOCOL {
  UINT8                     Revision;  
  UINT16                    VendorString[10];  
  UINT8                     AtState;              /// State of AT FW 
  UINT8                     AtLastTheftTrigger;   /// Reason for the last trigger
  UINT16                    AtLockState;          /// If AT Fw locked?
  UINT16                    AtAmPref;             /// TDTAM or PBA
  UINT16                    AtTdtWWAN3GPresent;
  UINT16                    AtTdtWWAN3GOOB;   
  CHAR16                    UniCodeNonceStr[34];  
  UINT32                    TimeLeft;  
  UINTN                     TimeLocationY;    
  UINT16                    *IsvString;
  UINT16                    IsvPlatformId[16];
  UINT32                    IsvId;
  CHAR16                    UniCodeStr;           /// Password
  UINTN                     LastStrIndex;         /// Password Last StrIndex
  UINTN                     StrIndex;             /// Password StrIndex
  UINT8                     Language;
} EFI_EXIT_AT_PROTOCOL;


#endif
