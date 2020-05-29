//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains 'Framework Code' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may not be modified, except as allowed by
// additional terms of your license agreement.
//
/*++

Copyright (c)  1999 - 2002 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  SimpleSerial.h

Abstract:

  Simple Serial PPI as defined in EFI 2.0

  This code abstracts the PEI core to provide Serial Access Services

--*/

#ifndef _PEI_SIMPLE_SERIAL_PPI_H_
#define _PEI_SIMPLE_SERIAL_PPI_H_

#define PEI_SIMPLE_SERIAL_PPI_GUID \
  { \
    0xe62404da, 0xea79, 0x413a, 0x89, 0x28, 0xb0, 0xea, 0x6e, 0xd4, 0x4, 0x8 \
  }

EFI_FORWARD_DECLARATION (PEI_SIMPLE_SERIAL_PPI);

typedef
EFI_STATUS
(EFIAPI *PEI_SIMPLE_SERIAL_INIT) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_SIMPLE_SERIAL_PPI          * This,
  IN UINT16                         PortNum,
  IN UINTN                          Bps,
  IN UINT8                          Data,
  IN UINT8                          Stop,
  IN UINT8                          Parity,
  IN UINT8                          BreakSet
  );

typedef
EFI_STATUS
(EFIAPI *PEI_SIMPLE_SERIAL_WRITE) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_SIMPLE_SERIAL_PPI          * This,
  IN UINTN                          PortNum,
  IN UINT8                          Character
  );

typedef
EFI_STATUS
(EFIAPI *PEI_SIMPLE_SERIAL_READ) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_SIMPLE_SERIAL_PPI          * This,
  IN  UINTN                         PortNum,
  OUT UINT8                         *Character
  );

typedef
EFI_STATUS
(EFIAPI *PEI_SIMPLE_SERIAL_PRINT) (
  IN EFI_PEI_SERVICES               **PeiServices,
  IN PEI_SIMPLE_SERIAL_PPI          * This,
  IN UINT8                          *OutputString
  );

typedef struct _PEI_SIMPLE_SERIAL_PPI {
  PEI_SIMPLE_SERIAL_INIT  PeiSerialInit;
  PEI_SIMPLE_SERIAL_WRITE PeiSerialWrite;
  PEI_SIMPLE_SERIAL_READ  PeiSerialRead;
  PEI_SIMPLE_SERIAL_PRINT PeiSerialPrint;
} PEI_SIMPLE_SERIAL_PPI;

extern EFI_GUID gPeiSimpleSerialPpiGuid;

#endif
