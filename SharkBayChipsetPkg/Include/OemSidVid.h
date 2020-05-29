/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _OEM_SID_SVID_DEFINE_H_
#define _OEM_SID_SVID_DEFINE_H_

#define NULL_SID_VID      0xffff

typedef struct {
  UINT16  SSID;
  UINT16  SVID;
} SID_SVID_ENTRY;

typedef struct  {
  SID_SVID_ENTRY      DefaultId;
  SID_SVID_ENTRY      Peg;
  SID_SVID_ENTRY      McBus;
  SID_SVID_ENTRY      Igd;
  SID_SVID_ENTRY      IchPciEBridge;
  SID_SVID_ENTRY      IchPciBridge;
  SID_SVID_ENTRY      Lan;
  SID_SVID_ENTRY      Lpc;
  SID_SVID_ENTRY      IDE;
  SID_SVID_ENTRY      Sata;
  SID_SVID_ENTRY      Uhci;
  SID_SVID_ENTRY      Ehci;
  SID_SVID_ENTRY      SmBus;
  SID_SVID_ENTRY      Ac97Code;
  SID_SVID_ENTRY      Ac97Modem;
  SID_SVID_ENTRY      AzaliaCode;
  SID_SVID_ENTRY      AzaliaModem;
  SID_SVID_ENTRY      CardBus;
  SID_SVID_ENTRY      Thermal;  
  SID_SVID_ENTRY      HECI0;
  SID_SVID_ENTRY      HECI1;
  SID_SVID_ENTRY      IDER;
  SID_SVID_ENTRY      SOL;  
  VOID                *OemDefineEntryPtr;
} OEM_SID_SVID_TABLE;

#endif
