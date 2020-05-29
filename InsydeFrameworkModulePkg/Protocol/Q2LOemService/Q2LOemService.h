//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _Q2L_OEM_SERVICE_H_
#define _Q2L_OEM_SERVICE_H_

// {649E9A1A-DA5F-484e-AA60-6838F6BE004D}
#define Q2L_OEM_SERVICE_PROTOCOL_GUID  \
  { \
    0x649e9a1a, 0xda5f, 0x484e, 0xaa, 0x60, 0x68, 0x38, 0xf6, 0xbe, 0x0, 0x4d \
  }

EFI_FORWARD_DECLARATION (Q2L_OEM_SERVICE_PROTOCOL);

//
// QueryBootMenu()
//
// Return codes:
//
// EFI_SUCCESS          is returned if this succeeds
// EFI_UNSUPPORTED      unsupport Boot Menu returned
//
typedef
EFI_STATUS
(EFIAPI *Q2L_OEM_SERVICE_QUERYBOOTMENU) (
  IN Q2L_OEM_SERVICE_PROTOCOL     *This,
  IN UINT8                       *ExpireTime
  );

//
// QueryQ2LSPFunction()
//
// Return codes:
//
// EFI_SUCCESS          is returned if this succeeds
// EFI_UNSUPPORTED      unsupport SP Function returned
//
typedef
EFI_STATUS
(EFIAPI *Q2L_OEM_SERVICE_QUERYSPFUNCTION) (
  IN Q2L_OEM_SERVICE_PROTOCOL     *This,
  IN UINT32                       *Enable
  );

//
// QueryKeyStatus
//
// Return codes:
// 
// 0: no key is pressed
// 1: up key is pressed
// 2: down key is pressed
// 3: select key is pressed
//    
typedef
INTN
(EFIAPI *Q2L_OEM_SERVICE_QUERYKEYSTATUS)(
    IN Q2L_OEM_SERVICE_PROTOCOL     *This
    );

//
// QueryPollingTime
//
// Return codes:
// Value: the period of polling Mouse and OEM Key status;
//
typedef
UINT64
(EFIAPI *Q2L_OEM_SERVICE_QUERYPOLLINGTIME)(
    IN Q2L_OEM_SERVICE_PROTOCOL     *This
    );

//
// QueryDefaultButtonID
//
// Return codes:
// Value: the default Button ID;
//
typedef
INT32
(EFIAPI *Q2L_OEM_SERVICE_QUERYDEFAULTBUTTONID)(
    IN Q2L_OEM_SERVICE_PROTOCOL     *This
    );
    
//
// Q2L_OEM_SERVICE PROTOCOL
//
typedef struct _Q2L_OEM_SERVICE_PROTOCOL {
  Q2L_OEM_SERVICE_QUERYBOOTMENU	       QueryBootMenu;
  Q2L_OEM_SERVICE_QUERYSPFUNCTION      QuerySPFunction;
  Q2L_OEM_SERVICE_QUERYKEYSTATUS       QueryKeyStatus;
  Q2L_OEM_SERVICE_QUERYPOLLINGTIME     QueryPollingTime;
  Q2L_OEM_SERVICE_QUERYDEFAULTBUTTONID QueryDefaultButtonID;
} Q2L_OEM_SERVICE_PROTOCOL;

extern EFI_GUID               gQ2LOemServiceProtocolGuid;

#endif