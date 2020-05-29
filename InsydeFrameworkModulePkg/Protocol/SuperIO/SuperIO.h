//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++

Copyright 2004, Intel Corporation
All rights reserved. This program and the accompanying materials
are licensed and made available under the terms and conditions of the BSD License
which accompanies this distribution.  The full text of the license may be found at
http://opensource.org/licenses/bsd-license.php

THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

Module Name:

    SuperIO.h

Abstract:

    EFI ISA Acpi Protocol

Revision History

--*/

#ifndef _SUPER_IO_H_
#define _SUPER_IO_H_
#include "pci22.h"
#include "SetupConfig.h"


#define EFI_SUPERIO_PROTOCOL_GUID \
        {0xb01bca78, 0xe966, 0x4c77, 0x8b, 0xf, 0x24, 0x5a, 0xa2, 0x59, 0x1e, 0x33}

EFI_FORWARD_DECLARATION (EFI_SUPER_IO);


//
// Resource List definition:
// at first, the resource was defined as below
// but in the future, it will be defined again that follow ACPI spec: ACPI resource type
// so that, in this driver, we can interpret the ACPI table and get the ISA device information.
//

typedef struct {
  UINT32                              ConfigPort;
  UINT32                              IndexPort;
  UINT32                              DataPort;
} SIO_DEV_CID;

//
//COM PORT
//
typedef
EFI_STATUS
(EFIAPI *EFI_COM_SUB_GETBASEADDRESS) (
  OUT UINT16   *BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_COM_SUB_GETDMA) (
  OUT UINT8    *DMAChannel
  );

typedef
EFI_STATUS
(EFIAPI *EFI_COM_SUB_GETENABLEDISABLE) (
  OUT UINT8    *EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_COM_SUB_GETINTERRUPT) (
  OUT UINT8    *InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_COM_SUB_GETMODE) (
  OUT UINT8    *Mode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_COM_SUB_SETBASEADDRESS) (
  IN UINT16   BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_COM_SUB_SETDMA) (
  IN UINT8    DMAChannel
  );

typedef
EFI_STATUS
(EFIAPI *EFI_COM_SUB_SETENABLEDISABLE) (
  IN UINT8    EnableDisable
  ); 

typedef
EFI_STATUS
(EFIAPI *EFI_COM_SUB_SETINTERRUPT) (
  IN  UINT8    InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_COM_SUB_SETMODE) (
  IN  UINT8    Mode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_COM_SUB_SETPOWER) (
  IN UINT8    Power
  );

//
//LPT PORT
//
typedef
EFI_STATUS
(EFIAPI *EFI_LPT_SUB_GETBASEADDRESS) (
  OUT UINT16   *BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_LPT_SUB_GETDMA) (
  OUT UINT8    *DMAChannel
  );

typedef
EFI_STATUS
(EFIAPI *EFI_LPT_SUB_GETENABLEDISABLE) (
  OUT UINT8    *EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_LPT_SUB_GETINTERRUPT) (
  OUT UINT8    *InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_LPT_SUB_GETMODE) (
  OUT UINT8    *Mode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_LPT_SUB_SETBASEADDRESS) (
  IN UINT16   BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_LPT_SUB_SETDMA) (
  IN UINT8    DMAChannel
  );

typedef
EFI_STATUS
(EFIAPI *EFI_LPT_SUB_SETENABLEDISABLE) (
  IN UINT8    EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_LPT_SUB_SETINTERRUPT) (
  IN  UINT8    InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_LPT_SUB_SETMODE) (
  IN  UINT8    Mode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_LPT_SUB_SETPOWER) (
  IN UINT8    Power
  );

//
//FDC PORT
//
typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SUB_GETBASEADDRESS) (
  OUT UINT16   *BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SUB_GETDMA) (
  OUT UINT8    *DMAChannel
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SUB_GETENABLEDISABLE) (
  OUT UINT8    *EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SUB_GETINTERRUPT) (
  OUT UINT8    *InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SUB_GETWRITEPROTECT) (
  OUT UINT8    *Mode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SUB_SETBASEADDRESS) (
  IN UINT16   BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SUB_SETDMA) (
  IN UINT8    DMAChannel
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SUB_SETENABLEDISABLE) (
  IN UINT8    EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SUB_SETINTERRUPT) (
  IN  UINT8    InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SUB_SETWRITEPROTECT) (
  IN  UINT8    Mode
  );

typedef
EFI_STATUS
(EFIAPI *EFI_FDC_SUB_SETPOWER) (
  IN UINT8    Power
  );

//
//KBC PORT
//
typedef
EFI_STATUS
(EFIAPI *EFI_KBC_SUB_SETKBCBASEADDRESS) (
  IN UINT16   BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_SUB_SETMSBASEADDRESS) (
  IN UINT16   BaseAddress
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_SUB_GETENABLEDISABLE) (
  IN BOOLEAN    *EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_SUB_SETENABLEDISABLE) (
  IN BOOLEAN    EnableDisable
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_SUB_SETKBCINTERRUPT) (
  IN UINT8    InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_SUB_SETMSINTERRUPT) (
  IN UINT8    InterruptNo
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_SUB_SETKBCPOWER) (
  IN UINT8    Power
  );

typedef
EFI_STATUS
(EFIAPI *EFI_KBC_SUB_SETMSPOWER) (
  IN UINT8    Power
  );

typedef struct  {
   UINT32                              ComProtSupport;
   UINT32                              IrUniqueSupport;
   UINT32                              IrSupport;
   UINT32                              LptSupport;
   UINT32                              FddSupport;
   UINT32                              KbcSupport;
} SIO_DEVICE_SUPPORT_NUM;

//
// Interface structure for the ISA ACPI Protocol
//

typedef struct {
   EFI_COM_SUB_GETBASEADDRESS          GetBaseAddress;  
   EFI_COM_SUB_GETDMA                  GetDMA;  
   EFI_COM_SUB_GETENABLEDISABLE        GetEnableDisable;  
   EFI_COM_SUB_GETINTERRUPT            GetInterrupt;  
   EFI_COM_SUB_GETMODE                 GetMode;  
   EFI_COM_SUB_SETBASEADDRESS          SetBaseAddress;  
   EFI_COM_SUB_SETDMA                  SetDMA;  
   EFI_COM_SUB_SETENABLEDISABLE        SetEnableDisable;  
   EFI_COM_SUB_SETINTERRUPT            SetInterrupt;  
   EFI_COM_SUB_SETMODE                 SetMode;  
   EFI_COM_SUB_SETPOWER                SetPower;  
} EFI_COM_SUB_FUNCTION;

typedef struct {
   EFI_LPT_SUB_GETBASEADDRESS          GetBaseAddress;  
   EFI_LPT_SUB_GETDMA                  GetDMA;  
   EFI_LPT_SUB_GETENABLEDISABLE        GetEnableDisable;  
   EFI_LPT_SUB_GETINTERRUPT            GetInterrupt;  
   EFI_LPT_SUB_GETMODE                 GetMode;  
   EFI_LPT_SUB_SETBASEADDRESS          SetBaseAddress;  
   EFI_LPT_SUB_SETDMA                  SetDMA;  
   EFI_LPT_SUB_SETENABLEDISABLE        SetEnableDisable;  
   EFI_LPT_SUB_SETINTERRUPT            SetInterrupt;  
   EFI_LPT_SUB_SETMODE                 SetMode;  
   EFI_LPT_SUB_SETPOWER                SetPower;  
} EFI_LPT_SUB_FUNCTION;

typedef struct {
   EFI_FDC_SUB_GETBASEADDRESS          GetBaseAddress;  
   EFI_FDC_SUB_GETDMA                  GetDMA;  
   EFI_FDC_SUB_GETENABLEDISABLE        GetEnableDisable;  
   EFI_FDC_SUB_GETINTERRUPT            GetInterrupt;  
   EFI_FDC_SUB_GETWRITEPROTECT         GetWriteProtect;  
   EFI_FDC_SUB_SETBASEADDRESS          SetBaseAddress;  
   EFI_FDC_SUB_SETDMA                  SetDMA;  
   EFI_FDC_SUB_SETENABLEDISABLE        SetEnableDisable;  
   EFI_FDC_SUB_SETINTERRUPT            SetInterrupt;  
   EFI_FDC_SUB_SETWRITEPROTECT         SetWriteProtect;  
   EFI_FDC_SUB_SETPOWER                SetPower;  
} EFI_FDC_SUB_FUNCTION;

typedef struct {
   EFI_KBC_SUB_SETKBCBASEADDRESS       SetKbcBaseAddress;  
   EFI_KBC_SUB_SETMSBASEADDRESS        SetMsBaseAddress;  
   EFI_KBC_SUB_GETENABLEDISABLE        GetEnableDisable;
   EFI_KBC_SUB_SETENABLEDISABLE        SetEnableDisable;       
   EFI_KBC_SUB_SETKBCINTERRUPT         SetKbcInterrupt; 
   EFI_KBC_SUB_SETMSINTERRUPT          SetMsInterrupt;     
   EFI_KBC_SUB_SETKBCPOWER             SetKbcPower;  
   EFI_KBC_SUB_SETMSPOWER              SetMsPower;     
} EFI_KBC_SUB_FUNCTION;

typedef struct _EFI_SUPER_IO {
   SIO_DEV_CID	                       ThisSioDev;
   UINT32                              IrUniquePnp;
   SIO_DEVICE_SUPPORT_NUM              SioDeviceSupportNum;
   EFI_COM_SUB_FUNCTION                *Com1portSubFunction;
   EFI_COM_SUB_FUNCTION                *Com2portSubFunction;
   EFI_COM_SUB_FUNCTION                *IrUniqueSubFunction;
   EFI_COM_SUB_FUNCTION                *IrSubFunction;
   EFI_LPT_SUB_FUNCTION                *LptSubFunction;
   EFI_FDC_SUB_FUNCTION                *FdcSubFunction;
   EFI_KBC_SUB_FUNCTION                *KbcSubFunction;
   
} EFI_SUPER_IO_PROTOCOL;

extern EFI_GUID gEfiSuperIOProtocolGuid;

#endif
