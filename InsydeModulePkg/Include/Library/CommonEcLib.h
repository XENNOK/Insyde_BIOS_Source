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

#ifndef _COMMON_EC_LIBRARY_H_
#define _COMMON_EC_LIBRARY_H_

#include <Uefi.h>

#define KEY_OBF                           1
#define KEY_IBF                           2
#define KEY_DATA                          0x60
#define KEY_CMD_STATE                     0x64
#define KBC_READ_CMD_BYTE                 0x20
#define KBC_WRITE_CMD_BYTE                0x60
#define IRQ_8259_MASK                     0x21

EFI_STATUS
Stall ( 
  IN UINTN              Microseconds
  );

/**
  Wait for output buffer full

  @param[in]  CommandState - the Io to read.

  @retval     EFI_SUCCESS - input buffer full.
  
**/
EFI_STATUS
WaitKbcObf (
  IN UINT16              CommandState
  );

/**
  Wait for input buffer empty

  @param[in]    CommandState - the Io to read.

  @retval       EFI_SUCCESS - input buffer full.

  
**/
EFI_STATUS
WaitKbcIbe (
  IN UINT16              CommandState
  );

/**
  Write data to Kbc data port

  @param[in]  CommandState - the Io to write.
  @param[in]  Data - The data which want write to Kbc data port

  @retval     EFI_SUCCESS - Write data to Kbc port successfully
  
**/
EFI_STATUS
WriteKbc (
  IN UINT16             CommandState,
  IN UINT8              Data
  );

/**
  Read data from kbc data port

  @param[in] CommandState - the Io to read.
  @param[in] Data - IN OUT :The data which read from Kbc data port

  @retval EFI_SUCCESS - Read data from Kbc port successfully.
  @retval EFI_INVALID_PARAMETER - The input parameter is invalid.
  
**/
EFI_STATUS
ReadKbc (
  IN UINT16                 CommandState,
  IN OUT UINT8              *Data
  );

#endif 
