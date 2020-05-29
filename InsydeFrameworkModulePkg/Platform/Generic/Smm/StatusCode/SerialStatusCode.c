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
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to
// the additional terms of the license agreement
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

  SerialStatusCode.c

Abstract:

  Lib to provide Serial I/O status code reporting Routines. This routine
  does not use PPI's but is monolithic.

  In general you should use PPI's, but some times a monolithic driver
  is better. The best justification for monolithic code is debug.

--*/

#include "SerialStatusCode.h"
#ifdef INSYDE_DEBUG
#ifdef EFI_DEBUG
#include EFI_PROTOCOL_CONSUMER (DebugComm)
#endif
#endif

VOID
EFIAPI
SerialInitializeStatusCode (
  VOID
  )
/*++

Routine Description:

  Initialize Serial Port

    The Baud Rate Divisor registers are programmed and the LCR
    is used to configure the communications format. Hard coded
    UART config comes from globals in DebugSerialPlatform lib.

Arguments:

  None

Returns:

  None

--*/
{
  UINTN Divisor;
  UINT8 OutputData;
  UINT8 Data;

  //
  // Some init is done by the platform status code initialization.
  //
  //
  // Map 5..8 to 0..3
  //
  Data = (UINT8) (gData - (UINT8) 5);

  //
  // Calculate divisor for baud generator
  //
  Divisor = 115200 / gBps;

  //
  // Set communications format
  //
  OutputData = (UINT8) ((DLAB << 7) | ((gBreakSet << 6) | ((gParity << 3) | ((gStop << 2) | Data))));
  IoWrite8 (gComBase + LCR_OFFSET, OutputData);

  //
  // Configure baud rate
  //
  IoWrite8 (gComBase + BAUD_HIGH_OFFSET, (UINT8) (Divisor >> 8));
  IoWrite8 (gComBase + BAUD_LOW_OFFSET, (UINT8) (Divisor & 0xff));

  //
  // Switch back to bank 0
  //
  OutputData = (UINT8) ((~DLAB << 7) | ((gBreakSet << 6) | ((gParity << 3) | ((gStop << 2) | Data))));
  IoWrite8 (gComBase + LCR_OFFSET, OutputData);
}

VOID
DebugSerialWrite (
  IN UINT8  Character
  )
/*++

Routine Description:

 DebugSerialWrite - Outputs a character to the Serial port

  Repeatedly polls the TXRDY bit of the Line Status Register
  until the Transmitter Holding Register is empty.  The character
  is then written to the Serial port.

Arguments:

  Character   - Character to write

Returns:

  None

--*/
{
  UINT8 Data;

  //
  // Wait for the serail port to be ready.
  //
  do {
    Data = IoRead8 (gComBase + LSR_OFFSET);
  } while ((Data & LSR_TXRDY) == 0);

  IoWrite8 (gComBase, Character);
}

VOID
DebugSerialPrint (
  IN UINT8    *OutputString
  )
/*++

Routine Description:

  Prints a string to the Serial port

Arguments:

  OutputString - Ascii string to print to serial port.

Returns:

  None

--*/
{
  EFI_STATUS  Status;

  Status = EFI_SUCCESS;

  for (; *OutputString != 0; OutputString++) {
    DebugSerialWrite (*OutputString);
  }
}

EFI_STATUS
EFIAPI
SerialReportStatusCode (
  IN EFI_STATUS_CODE_TYPE     CodeType,
  IN EFI_STATUS_CODE_VALUE    Value,
  IN UINT32                   Instance,
  IN EFI_GUID                 * CallerId,
  IN EFI_STATUS_CODE_DATA     * Data OPTIONAL
  )
/*++

Routine Description:

  Provide a serial port print

Arguments:

  PeiServices - General purpose services available to every PEIM.

Returns:

  Status -  EFI_SUCCESS if the interface could be successfully
            installed

--*/
// GC_TODO:    CodeType - add argument and description to function comment
// GC_TODO:    Value - add argument and description to function comment
// GC_TODO:    Instance - add argument and description to function comment
// GC_TODO:    CallerId - add argument and description to function comment
// GC_TODO:    Data - add argument and description to function comment
{
  CHAR8   Buffer[EFI_STATUS_CODE_DATA_MAX_SIZE];
  UINT32  LineNumber;
  CHAR8   *Filename;
  CHAR8   *Description;
  CHAR8   *Format;
  VA_LIST Marker;
  UINT32  ErrorLevel;
  UINTN   CharCount;

  Buffer[0] = '\0';

  SerialInitializeStatusCode();

  if (ReportStatusCodeExtractAssertInfo (CodeType, Value, Data, &Filename, &Description, &LineNumber)) {
    //
    // Processes PEI_ASSERT ()
    //
    ASPrint (
      Buffer,
      EFI_STATUS_CODE_DATA_MAX_SIZE,
      "\nPEI_ASSERT!: %a (%d): %a\n",
      Filename,
      LineNumber,
      Description
      );

  } else if (ReportStatusCodeExtractDebugInfo (Data, &ErrorLevel, &Marker, &Format)) {
    //
    // Process PEI_DEBUG () macro to Serial
    //
    AvSPrint (Buffer, EFI_STATUS_CODE_DATA_MAX_SIZE, Format, Marker);

  } else if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
    //
    // Process Errors
    //
    CharCount = ASPrint (Buffer, EFI_STATUS_CODE_DATA_MAX_SIZE, "ERROR: C%x:V%x I%x", CodeType, Value, Instance);
    //
    // Make sure we don't try to print values that weren't intended to be printed, especially NULL GUID pointers.
    //
    if (CallerId) {
      CharCount += ASPrint (
                    &Buffer[CharCount - 1],
                    (EFI_STATUS_CODE_DATA_MAX_SIZE - (sizeof (Buffer[0]) * CharCount)),
                    " %g",
                    CallerId
                    );
    }

    if (Data) {
      CharCount += ASPrint (
                    &Buffer[CharCount - 1],
                    (EFI_STATUS_CODE_DATA_MAX_SIZE - (sizeof (Buffer[0]) * CharCount)),
                    " %x",
                    Data
                    );
    }

    CharCount += ASPrint (
                  &Buffer[CharCount - 1],
                  (EFI_STATUS_CODE_DATA_MAX_SIZE - (sizeof (Buffer[0]) * CharCount)),
                  "\n"
                  );

  }

  if (Buffer[0] != '\0') {
    //
    // Callout to platform Lib function to do print.
    //
    DebugSerialPrint (Buffer);
  }
  //
  // Big switch to print human readable status codes.
  //
  DEBUG_CODE (
  {
    if ((CodeType & EFI_STATUS_CODE_TYPE_MASK) == EFI_ERROR_CODE) {
      Buffer[0] = '\0';

      //
      // What severity error
      //
      switch (CodeType & EFI_STATUS_CODE_SEVERITY_MASK) {

      case EFI_ERROR_MINOR:
        CharCount = ASPrint (Buffer, EFI_STATUS_CODE_DATA_MAX_SIZE, "MINOR ERROR: ");
        break;

      case EFI_ERROR_MAJOR:
        CharCount = ASPrint (Buffer, EFI_STATUS_CODE_DATA_MAX_SIZE, "MAJOR ERROR: ");
        break;

      case EFI_ERROR_UNRECOVERED:
        CharCount = ASPrint (Buffer, EFI_STATUS_CODE_DATA_MAX_SIZE, "UNRECOVERED ERROR: ");
        break;

      case EFI_ERROR_UNCONTAINED:
        CharCount = ASPrint (Buffer, EFI_STATUS_CODE_DATA_MAX_SIZE, "UNCONTAINED ERROR: ");
        break;

      default:
        CharCount = ASPrint (Buffer, EFI_STATUS_CODE_DATA_MAX_SIZE, "UNKNOWN ERROR: ");
        break;
      }
      //
      // What is the class/subclass?
      //
      switch (Value & (EFI_STATUS_CODE_CLASS_MASK | EFI_STATUS_CODE_SUBCLASS_MASK)) {

      case EFI_COMPUTING_UNIT_UNSPECIFIED:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_COMPUTING_UNIT_UNSPECIFIED:"
                      );
        CharCount--;
        break;

      case EFI_COMPUTING_UNIT_HOST_PROCESSOR:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_COMPUTING_UNIT_HOST_PROCESSOR:"
                      );
        CharCount--;
        //
        // Do CU specific error codes
        //
        switch (Value & EFI_STATUS_CODE_OPERATION_MASK) {

        case EFI_CU_HP_EC_INVALID_TYPE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_HP_EC_INVALID_TYPE"
                        );
          CharCount--;
          break;

        case EFI_CU_HP_EC_INVALID_SPEED:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_HP_EC_INVALID_SPEED"
                        );
          CharCount--;
          break;

        case EFI_CU_HP_EC_MISMATCH:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_CU_HP_EC_MISMATCH");
          CharCount--;
          break;

        case EFI_CU_HP_EC_TIMER_EXPIRED:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_HP_EC_TIMER_EXPIRED"
                        );
          CharCount--;
          break;

        case EFI_CU_HP_EC_SELF_TEST:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_CU_HP_EC_SELF_TEST");
          CharCount--;
          break;

        case EFI_CU_HP_EC_INTERNAL:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_CU_HP_EC_INTERNAL");
          CharCount--;
          break;

        case EFI_CU_HP_EC_THERMAL:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_CU_HP_EC_THERMAL");
          CharCount--;
          break;

        case EFI_CU_HP_EC_LOW_VOLTAGE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_HP_EC_LOW_VOLTAGE"
                        );
          CharCount--;
          break;

        case EFI_CU_HP_EC_HIGH_VOLTAGE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_HP_EC_HIGH_VOLTAGE"
                        );
          CharCount--;
          break;

        case EFI_CU_HP_EC_CACHE:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_CU_HP_EC_CACHE");
          CharCount--;
          break;

        case EFI_CU_HP_EC_MICROCODE_UPDATE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_HP_EC_MICROCODE_UPDATE"
                        );
          CharCount--;
          break;

        case EFI_CU_HP_EC_NO_MICROCODE_UPDATE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_HP_EC_NO_MICROCODE_UPDATE"
                        );
          CharCount--;
          break;

        case EFI_CU_HP_EC_CORRECTABLE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_HP_EC_CORRECTABLE"
                        );
          CharCount--;
          break;

        case EFI_CU_HP_EC_UNCORRECTABLE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_HP_EC_UNCORRECTABLE"
                        );
          CharCount--;
          break;

        default:
          break;
        } break;

      case EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_COMPUTING_UNIT_FIRMWARE_PROCESSOR:"
                      );
        CharCount--;
        break;

      case EFI_COMPUTING_UNIT_IO_PROCESSOR:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_COMPUTING_UNIT_IO_PROCESSOR:"
                      );
        CharCount--;
        break;

      case EFI_COMPUTING_UNIT_CACHE:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_COMPUTING_UNIT_CACHE:"
                      );
        CharCount--;
        //
        // Do cache specific error codes
        //
        switch (Value & EFI_STATUS_CODE_OPERATION_MASK) {

        case EFI_CU_CACHE_EC_INVALID_TYPE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_CACHE_EC_INVALID_TYPE"
                        );
          CharCount--;
          break;

        case EFI_CU_CACHE_EC_INVALID_SPEED:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_CACHE_EC_INVALID_SPEED"
                        );
          CharCount--;
          break;

        case EFI_CU_CACHE_EC_INVALID_SIZE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_CACHE_EC_INVALID_SIZE"
                        );
          CharCount--;
          break;

        default:
          break;
        } break;

      case EFI_COMPUTING_UNIT_MEMORY:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_COMPUTING_UNIT_MEMORY:"
                      );
        CharCount--;
        //
        // Do memory specific error codes
        //
        switch (Value & EFI_STATUS_CODE_OPERATION_MASK) {

        case EFI_CU_MEMORY_EC_INVALID_TYPE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_MEMORY_EC_INVALID_TYPE"
                        );
          CharCount--;
          break;

        case EFI_CU_MEMORY_EC_INVALID_SPEED:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_MEMORY_EC_INVALID_SPEED"
                        );
          CharCount--;
          break;

        case EFI_CU_MEMORY_EC_CORRECTABLE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_MEMORY_EC_CORRECTABLE"
                        );
          CharCount--;
          break;

        case EFI_CU_MEMORY_EC_UNCORRECTABLE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_MEMORY_EC_UNCORRECTABLE"
                        );
          CharCount--;
          break;

        case EFI_CU_MEMORY_EC_SPD_FAIL:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_MEMORY_EC_SPD_FAIL"
                        );
          CharCount--;
          break;

        case EFI_CU_MEMORY_EC_INVALID_SIZE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_MEMORY_EC_INVALID_SIZE"
                        );
          CharCount--;
          break;

        case EFI_CU_MEMORY_EC_MISMATCH:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_MEMORY_EC_MISMATCH"
                        );
          CharCount--;
          break;

        case EFI_CU_MEMORY_EC_S3_RESUME_FAIL:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_MEMORY_EC_S3_RESUME_FAIL"
                        );
          CharCount--;
          break;

        case EFI_CU_MEMORY_EC_UPDATE_FAIL:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_MEMORY_EC_UPDATE_FAIL"
                        );
          CharCount--;
          break;

        default:
          break;
        } break;

      case EFI_COMPUTING_UNIT_CHIPSET:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_COMPUTING_UNIT_CHIPSET:"
                      );
        CharCount--;
        break;

      case EFI_PERIPHERAL_UNSPECIFIED:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_PERIPHERAL_UNSPECIFIED:"
                      );
        CharCount--;
        break;

      case EFI_PERIPHERAL_KEYBOARD:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_PERIPHERAL_KEYBOARD:"
                      );
        CharCount--;
        //
        // Do KB specific error codes
        //
        switch (Value & EFI_STATUS_CODE_OPERATION_MASK) {

        case EFI_P_KEYBOARD_EC_LOCKED:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_P_KEYBOARD_EC_LOCKED"
                        );
          CharCount--;
          break;

        case EFI_P_KEYBOARD_EC_STUCK_KEY:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_P_KEYBOARD_EC_STUCK_KEY"
                        );
          CharCount--;
          break;

        default:
          break;
        } break;

      case EFI_PERIPHERAL_MOUSE:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_PERIPHERAL_MOUSE:");
        CharCount--;
        //
        // Do mouse specific error codes
        //
        switch (Value & EFI_STATUS_CODE_OPERATION_MASK) {

        case EFI_P_MOUSE_EC_LOCKED:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_P_MOUSE_EC_LOCKED");
          CharCount--;
          break;

        default:
          break;
        } break;

      case EFI_PERIPHERAL_LOCAL_CONSOLE:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_PERIPHERAL_LOCAL_CONSOLE:"
                      );
        CharCount--;
        break;

      case EFI_PERIPHERAL_REMOTE_CONSOLE:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_PERIPHERAL_REMOTE_CONSOLE:"
                      );
        CharCount--;
        break;

      case EFI_PERIPHERAL_SERIAL_PORT:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_PERIPHERAL_SERIAL_PORT:"
                      );
        CharCount--;
        break;

      case EFI_PERIPHERAL_PARALLEL_PORT:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_PERIPHERAL_PARALLEL_PORT:"
                      );
        CharCount--;
        break;

      case EFI_PERIPHERAL_FIXED_MEDIA:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_PERIPHERAL_FIXED_MEDIA:"
                      );
        CharCount--;
        break;

      case EFI_PERIPHERAL_REMOVABLE_MEDIA:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_PERIPHERAL_REMOVABLE_MEDIA:"
                      );
        CharCount--;
        break;

      case EFI_PERIPHERAL_AUDIO_INPUT:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_PERIPHERAL_AUDIO_INPUT:"
                      );
        CharCount--;
        break;

      case EFI_PERIPHERAL_AUDIO_OUTPUT:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_PERIPHERAL_AUDIO_OUTPUT:"
                      );
        CharCount--;
        break;

      case EFI_PERIPHERAL_LCD_DEVICE:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_PERIPHERAL_LCD_DEVICE:"
                      );
        CharCount--;
        break;

      case EFI_IO_BUS_UNSPECIFIED:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_UNSPECIFIED:");
        CharCount--;
        break;

      case EFI_IO_BUS_PCI:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_PCI:");
        CharCount--;
        break;

      case EFI_IO_BUS_USB:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_USB:");
        CharCount--;
        break;

      case EFI_IO_BUS_IBA:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_IBA:");
        CharCount--;
        break;

      case EFI_IO_BUS_AGP:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_AGP:");
        CharCount--;
        break;

      case EFI_IO_BUS_PC_CARD:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_PC_CARD:");
        CharCount--;
        break;

      case EFI_IO_BUS_LPC:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_LPC:");
        CharCount--;
        break;

      case EFI_IO_BUS_SCSI:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_SCSI:");
        CharCount--;
        break;

      case EFI_IO_BUS_ATA_ATAPI:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_ATA_ATAPI:");
        CharCount--;
        break;

      case EFI_IO_BUS_FC:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_FC:");
        CharCount--;
        break;

      case EFI_IO_BUS_IP_NETWORK:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_IP_NETWORK:");
        CharCount--;
        break;

      case EFI_IO_BUS_SMBUS:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_SMBUS:");
        CharCount--;
        break;

      case EFI_IO_BUS_I2C:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IO_BUS_I2C:");
        CharCount--;
        break;

      case EFI_SOFTWARE_UNSPECIFIED:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_SOFTWARE_UNSPECIFIED:"
                      );
        CharCount--;
        break;

      case EFI_SOFTWARE_SEC:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_SOFTWARE_EFI_SEC:");
        CharCount--;
        break;

      case EFI_SOFTWARE_PEI_CORE:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_SOFTWARE_EFI_PEI_CORE:"
                      );
        CharCount--;
        break;

      case EFI_SOFTWARE_PEI_MODULE:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_SOFTWARE_PEI_MODULE:"
                      );
        CharCount--;
        break;

      case EFI_SOFTWARE_DXE_CORE:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_SOFTWARE_EFI_DXE_CORE:"
                      );
        CharCount--;
        break;

      case EFI_SOFTWARE_EFI_BOOT_SERVICE:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_SOFTWARE_EFI_BOOT_SERVICE:"
                      );
        CharCount--;
        break;

      case EFI_SOFTWARE_EFI_RUNTIME_SERVICE:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_SOFTWARE_EFI_RUNTIME_SERVICE:"
                      );
        CharCount--;
        break;

      case EFI_SOFTWARE_DXE_BS_DRIVER:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_SOFTWARE_DXE_BS_DRIVER:"
                      );
        CharCount--;
        break;

      case EFI_SOFTWARE_DXE_RT_DRIVER:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_SOFTWARE_DXE_RT_DRIVER:"
                      );
        CharCount--;
        break;

      case EFI_SOFTWARE_SMM_DRIVER:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_SOFTWARE_SMM_DRIVER:"
                      );
        CharCount--;
        break;

      case EFI_SOFTWARE_RT:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_SOFTWARE_RT:");
        CharCount--;
        break;

      case EFI_SOFTWARE_AL:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_SOFTWARE_AL:");
        CharCount--;
        break;

      case EFI_SOFTWARE_EFI_APPLICATION:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_SOFTWARE_EFI_APPLICATION:"
                      );
        CharCount--;
        break;

      case EFI_SOFTWARE_EFI_OS_LOADER:
        CharCount += ASPrint (
                      &Buffer[CharCount - 1],
                      EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                      "EFI_SOFTWARE_EFI_OS_LOADER:"
                      );
        CharCount--;
        break;

      default:
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "Unknown:");
        CharCount--;
        break;
      }
      //
      // Do shared class error codes
      //
      switch (Value & EFI_STATUS_CODE_CLASS_MASK) {

      case EFI_COMPUTING_UNIT:
        switch (Value & EFI_STATUS_CODE_OPERATION_MASK) {

        case EFI_CU_EC_NON_SPECIFIC:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_CU_EC_NON_SPECIFIC");
          CharCount--;
          break;

        case EFI_CU_EC_DISABLED:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_CU_EC_DISABLED");
          CharCount--;
          break;

        case EFI_CU_EC_NOT_SUPPORTED:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_CU_EC_NOT_SUPPORTED");
          CharCount--;
          break;

        case EFI_CU_EC_NOT_DETECTED:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_CU_EC_NOT_DETECTED");
          CharCount--;
          break;

        case EFI_CU_EC_NOT_CONFIGURED:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_CU_EC_NOT_CONFIGURED"
                        );
          CharCount--;
          break;

        default:
          break;
        } break;

      case EFI_PERIPHERAL:
        switch (Value & EFI_STATUS_CODE_OPERATION_MASK) {

        case EFI_P_EC_NON_SPECIFIC:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_P_EC_NON_SPECIFIC");
          CharCount--;
          break;

        case EFI_P_EC_DISABLED:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_P_EC_DISABLED");
          CharCount--;
          break;

        case EFI_P_EC_NOT_SUPPORTED:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_P_EC_NOT_SUPPORTED");
          CharCount--;
          break;

        case EFI_P_EC_NOT_DETECTED:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_P_EC_NOT_DETECTED");
          CharCount--;
          break;

        case EFI_P_EC_NOT_CONFIGURED:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_P_EC_NOT_CONFIGURED");
          CharCount--;
          break;

        case EFI_P_EC_INTERFACE_ERROR:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_P_EC_INTERFACE_ERROR"
                        );
          CharCount--;
          break;

        case EFI_P_EC_CONTROLLER_ERROR:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_P_EC_CONTROLLER_ERROR"
                        );
          CharCount--;
          break;

        case EFI_P_EC_INPUT_ERROR:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_P_EC_INPUT_ERROR");
          CharCount--;
          break;

        case EFI_P_EC_OUTPUT_ERROR:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_P_EC_OUTPUT_ERROR");
          CharCount--;
          break;

        case EFI_P_EC_RESOURCE_CONFLICT:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_P_EC_RESOURCE_CONFLICT"
                        );
          CharCount--;
          break;

        default:
          break;
         }break;

      case EFI_IO_BUS:
        switch (Value & EFI_STATUS_CODE_OPERATION_MASK) {

        case EFI_IOB_EC_NON_SPECIFIC:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IOB_EC_NON_SPECIFIC");
          CharCount--;
          break;

        case EFI_IOB_EC_DISABLED:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IOB_EC_DISABLED");
          CharCount--;
          break;

        case EFI_IOB_EC_NOT_SUPPORTED:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_IOB_EC_NOT_SUPPORTED"
                        );
          CharCount--;
          break;

        case EFI_IOB_EC_NOT_DETECTED:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IOB_EC_NOT_DETECTED");
          CharCount--;
          break;

        case EFI_IOB_EC_NOT_CONFIGURED:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_IOB_EC_NOT_CONFIGURED"
                        );
          CharCount--;
          break;

        case EFI_IOB_EC_INTERFACE_ERROR:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_IOB_EC_INTERFACE_ERROR"
                        );
          CharCount--;
          break;

        case EFI_IOB_EC_CONTROLLER_ERROR:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_IOB_EC_CONTROLLER_ERROR"
                        );
          CharCount--;
          break;

        case EFI_IOB_EC_READ_ERROR:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IOB_EC_READ_ERROR");
          CharCount--;
          break;

        case EFI_IOB_EC_WRITE_ERROR:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_IOB_EC_WRITE_ERROR");
          CharCount--;
          break;

        case EFI_IOB_EC_RESOURCE_CONFLICT:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_IOB_EC_RESOURCE_CONFLICT"
                        );
          CharCount--;
          break;

        default:
          break;
        } break;

      case EFI_SOFTWARE:
        switch (Value & EFI_STATUS_CODE_OPERATION_MASK) {

        case EFI_SW_EC_NON_SPECIFIC:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_SW_EC_NON_SPECIFIC");
          CharCount--;
          break;

        case EFI_SW_EC_LOAD_ERROR:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_SW_EC_LOAD_ERROR");
          CharCount--;
          break;

        case EFI_SW_EC_INVALID_PARAMETER:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_SW_EC_INVALID_PARAMETER"
                        );
          CharCount--;
          break;

        case EFI_SW_EC_UNSUPPORTED:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_SW_EC_UNSUPPORTED");
          CharCount--;
          break;

        case EFI_SW_EC_INVALID_BUFFER:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_SW_EC_INVALID_BUFFER"
                        );
          CharCount--;
          break;

        case EFI_SW_EC_OUT_OF_RESOURCES:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_SW_EC_OUT_OF_RESOURCES"
                        );
          CharCount--;
          break;

        case EFI_SW_EC_ABORTED:
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "EFI_SW_EC_ABORTED");
          CharCount--;
          break;

        case EFI_SW_EC_ILLEGAL_SOFTWARE_STATE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_SW_EC_ILLEGAL_SOFTWARE_STATE"
                        );
          CharCount--;
          break;

        case EFI_SW_EC_ILLEGAL_HARDWARE_STATE:
          CharCount += ASPrint (
                        &Buffer[CharCount - 1],
                        EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount,
                        "EFI_SW_EC_ILLEGAL_HARDWARE_STATE"
                        );
          CharCount--;
          break;

        default:
          break;
        } break;

      default:
        break;
      }
      if (Buffer[0] != '\0') {
      //
      // Callout to platform Lib function to do print.
      //
        if (Buffer[CharCount - 1] == ':') {
          CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "Unknown, look it up.");
          CharCount--;
        }
        CharCount += ASPrint (&Buffer[CharCount - 1], EFI_STATUS_CODE_DATA_MAX_SIZE - CharCount, "\n");
        CharCount--;

        DebugSerialPrint (Buffer);
      }
    }
  }
  )
#ifdef INSYDE_DEBUG
#ifdef EFI_DEBUG
    DDTPrint(NULL,Buffer);
#endif
#endif            
  return EFI_SUCCESS;
}
