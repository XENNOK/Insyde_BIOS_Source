//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name:
//;
//;   EcLib.h
//;
#ifndef _EC_LIBRARY_H_
#define _EC_LIBRARY_H_

#include "OemEcLib.h"

VOID
EcWrite(
  IN UINT8  Index,
  IN UINT8  Value
  );

UINT8
EcRead(
  IN UINT8  Index
  );

VOID
ECRamWrite(
  IN UINT8  Index,
  IN UINT8  Value
  );

UINT8
ECRamRead(
  IN UINT8  Index
  );

EFI_STATUS
WaitKbcObf (
  IN UINT16                         CommandState
  );

EFI_STATUS
WaitKbcIbe (
  IN UINT16                         CommandState
  );

EFI_STATUS
WriteKbc (
  IN UINT16             CommandState,
  IN UINT8              Data
  );

EFI_STATUS
ReadKbc (
  IN UINT16                 CommandState,
  IN OUT UINT8              *Data
  );

EFI_STATUS
EcAcpiMode (
  IN  BOOLEAN                           EnableEcMode
  );

EFI_STATUS
EcIdle (
  IN  BOOLEAN         EnableWrites
  );

EFI_STATUS
EcWait (
  IN  BOOLEAN         EnableWrites
  );

EFI_STATUS
SmmEcIdle (
  IN  BOOLEAN         EnableWrites
  );

EFI_STATUS
SmmEcWait (
  IN  BOOLEAN         EnableWrites
  );

VOID
EcInit(
  );

EFI_STATUS
RequestECToShutdown (
);

UINT8
ECGetThermalDiod (
);

BOOLEAN
PowerStateIsAc (
  VOID
  );

BOOLEAN
IsRecoverMode (
  );

EFI_STATUS
SaveRestoreKbc (
  IN  BOOLEAN                       SaveRestoreFlag
  );

//
// Define EC_CONVERT_POINTER_SUPPORT in EcLib.h if need support EC convert pointer
//
#ifdef EC_CONVERT_POINTER_SUPPORT
EFI_STATUS
EcConvertPointerSupport (
  VOID
  );
#endif

#define KEY_OBF                           1
#define KEY_IBF                           2
#define KEY_DATA                          0x60
#define KEY_CMD_STATE                     0x64
#define KBC_READ_CMD_BYTE                 0x20
#define KBC_WRITE_CMD_BYTE                0x60
#define IRQ_8259_MASK                     0x21
#endif
