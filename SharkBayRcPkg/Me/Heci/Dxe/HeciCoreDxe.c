/** @file

  Heci driver core. For Dxe Phase, determines the HECI device and initializes it.

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

#include "HeciDxe.h"
#include "HeciCoreDxe.h"
#include "HeciHpet.h"

///
/// Globals used in Heci driver
///
UINT16        HECICtlrBDF;
static UINT64 HeciMBAR = 0;

///
/// Extern for shared HECI data and protocols
///
extern HECI_INSTANCE  *mHeciContext;

/**
  The routing of MmIo Read Dword

  @param[in] a                    The address of Mmio

  @retval                         Return the valut of MmIo Read
**/
UINT32
MmIoReadDword (
  IN  UINT64                      a
  )
{
  volatile HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;

  HeciRegHCsrPtr = (HECI_HOST_CONTROL_REGISTER *) a;
  return HeciRegHCsrPtr->ul;
}

/**
  The routing of MmIo Write Dword

  @param[in] a                    The address of Mmio
  @param[in] b                    Value revised

  @retval None
**/
VOID
MmIoWriteDword (
  IN  UINT64                      a,
  IN  UINT32                      b
  )
{
  volatile HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;

  HeciRegHCsrPtr      = (HECI_HOST_CONTROL_REGISTER *) a;

  HeciRegHCsrPtr->ul  = b;
}

///
/// Macro definition for function used in Heci driver
///
#define MMIOREADDWORD(a)      MmIoReadDword (a)
#define MMIOWRITEDWORD(a, b)  MmIoWriteDword (a, b)

#ifdef EFI_DEBUG

/**
  For serial debugger used, it will show the buffer message line by line to serial console.

  @param[in] Message              the address point of buffer message
  @param[in] Length               message length

  @retval None
**/
VOID
ShowBuffer (
  IN  UINT8                       *Message,
  IN  UINT32                      Length
  )
{
  UINT32  Index;
  UINT32  Offset;
  CHAR16  Buffer[51]; // To construct a line needs 51 chars.

  Index  = 0;
  Offset = 0;
  ZeroMem (Buffer, sizeof (Buffer));

  while (Length-- > 0) {
    //
    // Get the corresponding offset value from the index of buffer message.
    //
    Offset = ((Index & 0x0F) > 7) ? (((Index & 0x0F) * 3) + 2) : ((Index & 0x0F) * 3);

    //
    // Print "- " at the half of a line increases the readability of debug message.
    //
    if ((Index & 0x0F) == 0x08) {
      UnicodeSPrint (&Buffer[24], 3 * sizeof (CHAR16), L"- ");
    }

    //
    // Collect the data of buffer message.
    //
    UnicodeSPrint (&Buffer[Offset], 4 * sizeof (CHAR16), L"%02x ", Message[Index]);

    //
    // A line contains 16 bytes of buffer message. If a line is complete, it will be shown through DEBUG macro.
    //
    if (Offset == 47) {
      DEBUG ((EFI_D_ERROR, "%02x: %s\n", (Index & 0xF0), Buffer));
    }

    Index++;
  }

  //
  // If a line isn't complete, show the remaining data.
  //
  if (Offset != 47) {
    DEBUG ((EFI_D_ERROR, "%02x: %s\n", (Index & 0xF0), Buffer));
  }
  return ;
}

#endif // End Of EFI_DEBUG

//
// Heci driver function definitions
//

/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in] None.

  @retval EFI_SUCCESS             HECI device is present and initialized.
  @retval EFI_DEVICE_ERROR        No HECI controller.
  @exception EFI_UNSUPPORTED      HECI MSG is unsupported because ME MODE is in ME ALT Disabled &
                                  SECOVR JMPR
  @retval EFI_TIMEOUT             ME is not ready
**/
EFI_STATUS
InitializeHeciPrivate (
  VOID
  )
{
  HECI_HOST_CONTROL_REGISTER          HeciRegHCsr;
  volatile HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;
  EFI_STATUS                          Status;
  HECI_FWS_REGISTER                   MeFirmwareStatus;

  Status = EFI_SUCCESS;

  SaveHpet ();
  do {
    ///
    /// Store HECI vendor and device information away
    ///
    mHeciContext->DeviceInfo = HeciPciRead16 (PCI_DEVICE_ID_OFFSET);

    ///
    /// Check for HECI-1 PCI device availability
    ///
    if (mHeciContext->DeviceInfo == 0xFFFF) {
      Status = EFI_DEVICE_ERROR;
      break;
    }

    MeFirmwareStatus.ul = HeciPciRead32 (R_FWSTATE);

    ///
    /// Check for ME FPT Bad
    ///
    if (MeFirmwareStatus.r.FptBad) {
      Status = EFI_DEVICE_ERROR;
      break;
    }
    ///
    /// Check for ME error status
    ///
    if (MeFirmwareStatus.r.ErrorCode) {
      Status = EFI_NOT_READY;
      if (MeFirmwareStatus.r.ErrorCode == ME_ERROR_CODE_UNKNOWN || MeFirmwareStatus.r.ErrorCode == ME_ERROR_CODE_IMAGE_FAILURE) {
        ///
        /// ME failed to start so no HECI
        ///
        Status = EFI_DEVICE_ERROR;
        break;
      }
    }
    ///
    /// HECI MSG is unsupported if ME MODE is in ME ALT Disabled & SECOVR JMPR
    ///
    if ((MeFirmwareStatus.r.MeOperationMode == ME_OPERATION_MODE_SECOVR_JMPR) ||
        (MeFirmwareStatus.r.MeOperationMode == ME_OPERATION_MODE_DEBUG)
        ) {
      Status = EFI_UNSUPPORTED;
      break;
    }
    ///
    /// Store HECI revision ID
    ///
    mHeciContext->RevisionInfo = HeciPciRead8 (PCI_REVISION_ID_OFFSET);

    ///
    /// Check if Base register is 64 bits wide.
    ///
    if (HeciPciRead32 (R_HECIMBAR) & 0x4) {
      mHeciContext->HeciMBAR = (((UINT64) HeciPciRead32 (R_HECIMBAR + 4) << 32) |
                                (UINT64) HeciPciRead32 (R_HECIMBAR)) & 0xFFFFFFF0;
    } else {
      mHeciContext->HeciMBAR = (UINT64) HeciPciRead32 (R_HECIMBAR) & 0xFFFFFFF0;
    }
    ///
    /// Get HECI_MBAR and see if it is programmed
    /// to a useable value
    ///
    HeciMBAR = mHeciContext->HeciMBAR;

    ///
    /// Load temporary address for HECI_MBAR if one is not assigned
    ///
    if (mHeciContext->HeciMBAR == 0) {
      DEBUG ((EFI_D_ERROR, "Heci MMIO Bar not programmed in DXE phase\n"));
    }
    ///
    /// Enable HECI BME and MSE
    ///
    HeciPciOr8 (
      PCI_COMMAND_OFFSET,
      EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER
      );

    ///
    /// Set HECI interrupt delivery mode.
    /// HECI-1 using legacy/MSI interrupt
    ///
    HeciPciAnd8 (R_HIDM, 0xFC);

    ///
    /// Need to do following on ME init:
    ///
    ///  1) wait for ME_CSR_HA reg ME_RDY bit set
    ///
    if (WaitForMEReady () != EFI_SUCCESS) {
      Status = EFI_TIMEOUT;
      break;
    }
    ///
    ///  2) setup H_CSR reg as follows:
    ///     a) Make sure H_RST is clear
    ///     b) Set H_RDY
    ///     c) Set H_IG
    ///
    HeciRegHCsrPtr  = (VOID *) (UINTN) (mHeciContext->HeciMBAR + H_CSR);
    HeciRegHCsr.ul  = HeciRegHCsrPtr->ul;
    if (HeciRegHCsrPtr->r.H_RDY == 0) {
      HeciRegHCsr.r.H_RST = 0;
      HeciRegHCsr.r.H_RDY = 1;
      HeciRegHCsr.r.H_IG  = 1;
      HeciRegHCsrPtr->ul  = HeciRegHCsr.ul;
    }
  } while (Status != EFI_SUCCESS && Status != EFI_NOT_READY);

  RestoreHpet ();

  return Status;
}

/**
  Waits for the ME to report that it is ready for communication over the HECI
  interface.

  @param[in] None.

  @retval EFI_SUCCESS             ME is ready
  @retval EFI_TIMEOUT             ME is not ready
**/
EFI_STATUS
WaitForMEReady (
  VOID
  )
{
  UINT32                    TimerStart;
  UINT32                    TimerEnd;
  HECI_ME_CONTROL_REGISTER  HeciRegMeCsrHa;

  ///
  ///  Wait for ME ready
  ///
  ///
  /// Check for ME ready status
  ///
  StartTimer (&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);
  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
  while (HeciRegMeCsrHa.r.ME_RDY_HRA == 0) {
    ///
    /// If 5 second timeout has expired, return fail
    ///
    if (Timeout (TimerStart, TimerEnd) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    ///
    /// Perform IO delay
    ///
    IoDelay (HECI_WAIT_DELAY);

    HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
  }
  ///
  /// ME ready!!!
  ///
  return EFI_SUCCESS;
}

/**
  Checks if HECI reset has occured.

  @param[in] None.

  @retval TRUE                    HECI reset occurred
  @retval FALSE                   No HECI reset occurred
**/
BOOLEAN
CheckForHeciReset (
  VOID
  )
{
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;
  HECI_ME_CONTROL_REGISTER        HeciRegMeCsrHa;

  ///
  /// Init Host & ME CSR
  ///
  HeciRegHCsr.ul    = MMIOREADDWORD (HeciMBAR + H_CSR);
  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);

  if ((HeciRegMeCsrHa.r.ME_RDY_HRA == 0) || (HeciRegHCsr.r.H_RDY == 0)) {
    return TRUE;
  }

  return FALSE;
}

/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in] None.

  @retval EFI_SUCCESS             HECI device is present and initialized
  @retval EFI_TIMEOUT             ME is not ready
**/
EFI_STATUS
EFIAPI
HeciInitialize (
  VOID
  )
{
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;

  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  HeciMBAR = CheckAndFixHeciForAccess ();

  ///
  /// Need to do following on ME init:
  ///
  ///  1) wait for ME_CSR_HA reg ME_RDY bit set
  ///
  if (WaitForMEReady () != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }
  ///
  ///  2) setup H_CSR reg as follows:
  ///     a) Make sure H_RST is clear
  ///     b) Set H_RDY
  ///     c) Set H_IG
  ///
  HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  if (HeciRegHCsr.r.H_RDY == 0) {
    HeciRegHCsr.r.H_RST = 0;
    HeciRegHCsr.r.H_RDY = 1;
    HeciRegHCsr.r.H_IG  = 1;
    MMIOWRITEDWORD (HeciMBAR + H_CSR, HeciRegHCsr.ul);
  }

  return EFI_SUCCESS;
}

/**
  Heci Re-initializes it for Host

  @param[in] None.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_STATUS              Status code returned by ResetHeciInterface
**/
EFI_STATUS
EFIAPI
HeciReInitialize (
  VOID
  )
{
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;
  EFI_STATUS                      Status;

  Status = EFI_SUCCESS;
  ///
  /// Need to do following on ME init:
  ///
  ///  1) wait for HOST_CSR_HA reg H_RDY bit set
  ///
  ///    if (WaitForHostReady() != EFI_SUCCESS) {
  ///
  if (MeResetWait (HECI_INIT_TIMEOUT) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }

  HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  if (HeciRegHCsr.r.H_RDY == 0) {
    Status = ResetHeciInterface ();

  }

  return Status;
}

/**
  Heci Re-initializes it for Me

  @param[in] None.

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Re-initialization done
**/
EFI_STATUS
EFIAPI
HeciReInitialize2 (
  VOID
  )
{
  HECI_ME_CONTROL_REGISTER        HeciRegMeCsrHa;
  EFI_STATUS                      Status;
  UINT32                          TimerStart;
  UINT32                          TimerEnd;

  Status = EFI_SUCCESS;
  ///
  /// Need to do following on ME init:
  ///
  ///  1) wait for HOST_CSR_HA reg H_RDY bit set
  ///
  ///    if (WaitForHostReady() != EFI_SUCCESS) {
  ///
  StartTimer (&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);
  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
  while (HeciRegMeCsrHa.r.ME_RDY_HRA == 1) {
    ///
    /// If 5 second timeout has expired, return fail
    ///
    if (Timeout (TimerStart, TimerEnd) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }

    IoDelay (HECI_WAIT_DELAY);

    HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
  }

  if (WaitForMEReady () != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }

  return Status;
}

/**
  Function to pull one messsage packet off the HECI circular buffer.
  Corresponds to HECI HPS (part of) section 4.2.4

  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[out] MessageHeader       Pointer to a buffer for the message header.
  @param[in] MessageData          Pointer to a buffer to recieve the message in.
  @param[in][out] Length          On input is the size of the callers buffer in bytes. On
                                  output this is the size of the packet in bytes.

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        The circular buffer is overflowed.
  @retval EFI_NO_RESPONSE         The circular buffer is empty
  @retval EFI_TIMEOUT             Failed to receive a full message
  @retval EFI_BUFFER_TOO_SMALL    Message packet is larger than caller's buffer
**/
EFI_STATUS
HECIPacketRead (
  IN      UINT32                  Blocking,
  OUT     HECI_MESSAGE_HEADER     *MessageHeader,
  OUT     UINT32                  *MessageData,
  IN OUT  UINT32                  *Length
  )
{
  BOOLEAN                         GotMessage;
  UINT32                          TimerStart;
  UINT32                          TimerEnd;
  UINT32                          TimerStart1;
  UINT32                          TimerEnd1;
  UINT32                          i;
  UINT32                          LengthInDwords;
  HECI_ME_CONTROL_REGISTER        HeciRegMeCsrHa;
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;

  GotMessage = FALSE;
  ///
  /// Initialize memory mapped register pointers
  ///
  ///  VOLATILE HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr     = (VOID*)(mHeciContext->HeciMBAR + H_CSR);
  ///  VOLATILE HECI_ME_CONTROL_REGISTER   *HeciRegMeCsrHaPtr  = (VOID*)(mHeciContext->HeciMBAR + ME_CSR_HA);
  ///  VOLATILE UINT32                     *HeciRegMeCbrwPtr   = (VOID*)(mHeciContext->HeciMBAR + ME_CB_RW);
  ///
  /// clear Interrupt Status bit
  ///
  HeciRegHCsr.ul      = MMIOREADDWORD (HeciMBAR + H_CSR);
  HeciRegHCsr.r.H_IS  = 1;

  ///
  /// test for circular buffer overflow
  ///
  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
  if (OverflowCB (
        HeciRegMeCsrHa.r.ME_CBRP_HRA,
        HeciRegMeCsrHa.r.ME_CBWP_HRA,
        HeciRegMeCsrHa.r.ME_CBD_HRA
        ) != EFI_SUCCESS) {
    ///
    /// if we get here, the circular buffer is overflowed
    ///
    *Length = 0;
    return EFI_DEVICE_ERROR;
  }
  ///
  /// If NON_BLOCKING, exit if the circular buffer is empty
  ///
  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);;
  if ((FilledSlots (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA) == 0) && (Blocking == NON_BLOCKING)) {
    *Length = 0;
    return EFI_NO_RESPONSE;
  }
  ///
  /// Start timeout counter
  ///
  StartTimer (&TimerStart, &TimerEnd, HECI_READ_TIMEOUT);

  ///
  /// loop until we get a message packet
  ///
  while (!GotMessage) {
    ///
    /// If 1 second timeout has expired, return fail as we have not yet received a full message.
    ///
    if (Timeout (TimerStart, TimerEnd) != EFI_SUCCESS) {
      *Length = 0;
      return EFI_TIMEOUT;
    }
    ///
    /// Read one message from HECI buffer and advance read pointer.  Make sure
    /// that we do not pass the write pointer.
    ///
    HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);;
    if (FilledSlots (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA) > 0) {
      ///
      /// Eat the HECI Message header
      ///
      MessageHeader->Data = MMIOREADDWORD (HeciMBAR + ME_CB_RW);

      ///
      /// Compute required message length in DWORDS
      ///
      LengthInDwords = ((MessageHeader->Fields.Length + 3) / 4);

      ///
      /// Just return success if Length is 0
      ///
      if (MessageHeader->Fields.Length == 0) {
        ///
        /// Set Interrupt Generate bit and return
        ///
        MMIOREADDWORD (HeciMBAR + H_CSR);
        HeciRegHCsr.r.H_IG = 1;
        MMIOWRITEDWORD (HeciMBAR + H_CSR, HeciRegHCsr.ul);
        *Length = 0;
        return EFI_SUCCESS;
      }
      ///
      /// Make sure that the message does not overflow the circular buffer.
      ///
      HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
      if ((MessageHeader->Fields.Length + sizeof (HECI_MESSAGE_HEADER)) > (HeciRegMeCsrHa.r.ME_CBD_HRA * 4)) {
        *Length = 0;
        return EFI_DEVICE_ERROR;
      }
      ///
      /// Make sure that the callers buffer can hold the correct number of DWORDS
      ///
      if ((MessageHeader->Fields.Length) <= *Length) {
        ///
        /// Start timeout counter for inner loop
        ///
        StartTimer (&TimerStart1, &TimerEnd1, HECI_READ_TIMEOUT);

        ///
        /// Wait here until entire message is present in circular buffer
        ///
        HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
        while (LengthInDwords > FilledSlots (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA)) {
          ///
          /// If 1 second timeout has expired, return fail as we have not yet received a full message
          ///
          if (Timeout (TimerStart1, TimerEnd1) != EFI_SUCCESS) {
            *Length = 0;
            return EFI_TIMEOUT;
          }
          ///
          /// Wait before we read the register again
          ///
          IoDelay (HECI_WAIT_DELAY);

          ///
          /// Read the register again
          ///
          HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
        }
        ///
        /// copy rest of message
        ///
        for (i = 0; i < LengthInDwords; i++) {
          MessageData[i] = MMIOREADDWORD (HeciMBAR + ME_CB_RW);
        }
        ///
        /// Update status and length
        ///
        GotMessage  = TRUE;
        *Length     = MessageHeader->Fields.Length;

      } else {
        ///
        /// Message packet is larger than caller's buffer
        ///
        *Length = 0;
        return EFI_BUFFER_TOO_SMALL;
      }
    }
    ///
    /// Wait before we try to get a message again
    ///
    IoDelay (HECI_WAIT_DELAY);
  }
  ///
  /// Read ME_CSR_HA.  If the ME_RDY bit is 0, then an ME reset occurred during the
  /// transaction and the message should be discarded as bad data may have been retrieved
  /// from the host's circular buffer
  ///
  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
  if (HeciRegMeCsrHa.r.ME_RDY_HRA == 0) {
    *Length = 0;
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Set Interrupt Generate bit
  ///
  HeciRegHCsr.ul      = MMIOREADDWORD (HeciMBAR + H_CSR);
  HeciRegHCsr.r.H_IG  = 1;
  MMIOWRITEDWORD (HeciMBAR + H_CSR, HeciRegHCsr.ul);

  return EFI_SUCCESS;
}

/**
  Reads a message from the ME across HECI.

  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in][out] MessageBody     Pointer to a buffer used to receive a message.
  @param[in][out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI or zero-length message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
**/
EFI_STATUS
EFIAPI
HeciReceive (
  IN      UINT32                  Blocking,
  IN OUT  UINT32                  *MessageBody,
  IN OUT  UINT32                  *Length
  )
{
  HECI_MESSAGE_HEADER             PacketHeader;
  UINT32                          CurrentLength;
  UINT32                          MessageComplete;
  EFI_STATUS                      ReadError;
  EFI_STATUS                      Status;
  UINT32                          PacketBuffer;
  UINT32                          timer_start;
  UINT32                          timer_end;
  BOOLEAN                         QuitFlag;

  CurrentLength   = 0;
  MessageComplete = 0;
  Status          = EFI_SUCCESS;
  QuitFlag        = FALSE;

  SaveHpet ();

  do {
    ///
    /// Make sure that HECI device BAR is correct and device is enabled.
    ///
    HeciMBAR = CheckAndFixHeciForAccess ();

    ///
    /// Make sure we do not have a HECI reset
    ///
    if (CheckForHeciReset ()) {
      ///
      /// if HECI reset than try to re-init HECI
      ///
      Status = HeciInitialize ();

      if (EFI_ERROR (Status)) {
        Status = EFI_DEVICE_ERROR;
        break;
      }
    }
    ///
    /// Make sure that HECI is ready for communication.
    ///
    if (WaitForMEReady () != EFI_SUCCESS) {
      Status = EFI_TIMEOUT;
      break;
    }
    ///
    /// Set up timer for BIOS timeout.
    ///
    StartTimer (&timer_start, &timer_end, HECI_READ_TIMEOUT);
    while ((CurrentLength < *Length) && (MessageComplete == 0)) {
      ///
      /// If 1 second timeout has expired, return fail as we have not yet received a full message
      ///
      if (Timeout (timer_start, timer_end) != EFI_SUCCESS) {
        Status    = EFI_TIMEOUT;
        QuitFlag  = TRUE;
        break;
      }

      PacketBuffer = *Length - CurrentLength;
      ReadError = HECIPacketRead (
                    Blocking,
                    &PacketHeader,
                    (UINT32 *) &MessageBody[CurrentLength / 4],
                    &PacketBuffer
                    );

      ///
      /// Check for error condition on read
      ///
      if (EFI_ERROR (ReadError)) {
        *Length   = 0;
        Status    = ReadError;
        QuitFlag  = TRUE;
        break;
      }
      ///
      /// Get completion status from the packet header
      ///
      MessageComplete = PacketHeader.Fields.MessageComplete;

      ///
      /// Check for zero length messages
      ///
      if (PacketBuffer == 0) {
        ///
        /// If we are not in the middle of a message, and we see Message Complete,
        /// this is a valid zero-length message.
        ///
        if ((CurrentLength == 0) && (MessageComplete == 1)) {
          *Length   = 0;
          Status    = EFI_SUCCESS;
          QuitFlag  = TRUE;
          break;
        } else {
          ///
          /// We should not expect a zero-length message packet except as described above.
          ///
          *Length   = 0;
          Status    = EFI_DEVICE_ERROR;
          QuitFlag  = TRUE;
          break;
        }
      }
      ///
      /// Track the length of what we have read so far
      ///
      CurrentLength += PacketBuffer;

    }

    if (QuitFlag == TRUE) {
      break;
    }
    ///
    /// If we get here the message should be complete, if it is not
    /// the caller's buffer was not large enough.
    ///
    if (MessageComplete == 0) {
      *Length = 0;
      Status  = EFI_BUFFER_TOO_SMALL;
      break;
    }

    *Length = CurrentLength;

    DEBUG ((EFI_D_ERROR, "HECI ReadMsg:\n"));
#ifdef EFI_DEBUG
    DEBUG_CODE (
      ShowBuffer ((UINT8 *) MessageBody, *Length);
    );
#endif
  } while (EFI_ERROR (Status));

  RestoreHpet ();

  return Status;
}

/**
  Function sends one messsage (of any length) through the HECI circular buffer.

  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HECI
**/
EFI_STATUS
EFIAPI
HeciSend (
  IN UINT32                       *Message,
  IN UINT32                       Length,
  IN UINT8                        HostAddress,
  IN UINT8                        MeAddress
  )
{
  UINT32                          CBLength;
  UINT32                          SendLength;
  UINT32                          CurrentLength;
  HECI_MESSAGE_HEADER             MessageHeader;
  EFI_STATUS                      WriteStatus;
  EFI_STATUS                      Status;
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;
  UINT32                          MeMode;

  CurrentLength = 0;
  Status        = EFI_SUCCESS;

  SaveHpet ();

  HeciGetMeMode (&MeMode);
  do {
    if (MeMode == ME_MODE_SECOVER) {
      Status = EFI_UNSUPPORTED;
      break;
    }
    ///
    /// Make sure that HECI device BAR is correct and device is enabled.
    ///
    HeciMBAR = CheckAndFixHeciForAccess ();

    ///
    /// Make sure we do not have a HECI reset
    ///
    if (CheckForHeciReset ()) {
      ///
      /// if HECI reset than try to re-init HECI
      ///
      Status = HeciInitialize ();

      if (EFI_ERROR (Status)) {
        Status = EFI_DEVICE_ERROR;
        break;
      }
    }

    DEBUG ((EFI_D_ERROR, "HECI SendMsg:\n"));
#ifdef EFI_DEBUG
    DEBUG_CODE (
      ShowBuffer ((UINT8 *) Message, Length);
    );
#endif
    ///
    /// Make sure that HECI is ready for communication.
    ///
    if (WaitForMEReady () != EFI_SUCCESS) {
      Status = EFI_TIMEOUT;
      break;
    }
    ///
    /// Set up memory mapped registers
    ///
    HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);

    ///
    /// Grab Circular Buffer length
    ///
    CBLength = HeciRegHCsr.r.H_CBD;

    ///
    /// Prepare message header
    ///
    MessageHeader.Data                = 0;
    MessageHeader.Fields.MeAddress    = MeAddress;
    MessageHeader.Fields.HostAddress  = HostAddress;

    ///
    /// Break message up into CB-sized packets and loop until completely sent
    ///
    while (Length > CurrentLength) {
      ///
      /// Set the Message Complete bit if this is our last packet in the message.
      /// Needs to be 'less than' to account for the header OR Needs to be exactly equal to CB depth.
      ///
      if (((((Length - CurrentLength) + 3) / 4) < CBLength) ||
          ((((((Length - CurrentLength) + 3) / 4) == CBLength) && ((((Length - CurrentLength) + 3) % 4) == 0)))
          ) {
        MessageHeader.Fields.MessageComplete = 1;
      }
      ///
      /// Calculate length for Message Header
      ///    header length == smaller of circular buffer or remaining message (both account for the size of the header)
      ///
      SendLength = ((CBLength <= (((Length - CurrentLength) + 3) / 4)) ? ((CBLength - 1) * 4) : (Length - CurrentLength));
      MessageHeader.Fields.Length = SendLength;

      DEBUG ((EFI_D_ERROR, "HECI Header: %08x\n", MessageHeader.Data));

      ///
      /// send the current packet (CurrentLength can be treated as the index into the message buffer)
      ///
      WriteStatus = HeciPacketWrite (&MessageHeader, (UINT32 *) ((UINTN) Message + CurrentLength));
      if (EFI_ERROR (WriteStatus)) {
        Status = WriteStatus;
        break;
      }
      ///
      /// Update the length information
      ///
      CurrentLength += SendLength;
    }

    if (EFI_ERROR (Status)) {
      break;
    }
  } while (EFI_ERROR (Status));

  RestoreHpet ();

  return Status;

}

/**
  Function sends one messsage packet through the HECI circular buffer
  Corresponds to HECI HPS (part of) section 4.2.3

  @param[in] MessageHeader        Pointer to the message header.
  @param[in] MessageData          Pointer to the actual message data.

  @retval EFI_SUCCESS             One message packet sent
  @retval EFI_DEVICE_ERROR        ME is not ready
  @retval EFI_TIMEOUT             HECI is not ready for communication
**/
EFI_STATUS
HeciPacketWrite (
  IN  HECI_MESSAGE_HEADER         *MessageHeader,
  IN  UINT32                      *MessageData
  )
{
  UINT32                          timer_start;
  UINT32                          timer_end;
  UINT32                          i;
  UINT32                          LengthInDwords;
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;
  HECI_ME_CONTROL_REGISTER        HeciRegMeCsrHa;

  ///
  /// Make sure that HECI is ready for communication.
  ///
  if (WaitForMEReady () != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }
  ///
  /// Start timeout counter
  ///
  StartTimer (&timer_start, &timer_end, HECI_SEND_TIMEOUT);

  ///
  /// Compute message length in DWORDS
  ///
  LengthInDwords = ((MessageHeader->Fields.Length + 3) / 4);

  ///
  /// Wait until there is sufficient room in the circular buffer
  /// Must have room for message and message header
  ///
  HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  while ((LengthInDwords + 1) > (HeciRegHCsr.r.H_CBD - FilledSlots (HeciRegHCsr.r.H_CBRP, HeciRegHCsr.r.H_CBWP))) {
    ///
    /// If 1 second timeout has expired, return fail as the circular buffer never emptied
    ///
    if (Timeout (timer_start, timer_end) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    ///
    /// Wait before we read the register again
    ///
    IoDelay (HECI_WAIT_DELAY);

    ///
    /// Read Host CSR for next iteration
    ///
    HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  }
  ///
  /// Write Message Header
  ///
  MMIOWRITEDWORD (HeciMBAR + H_CB_WW, MessageHeader->Data);

  ///
  /// Write Message Body
  ///
  for (i = 0; i < LengthInDwords; i++) {
    MMIOWRITEDWORD (HeciMBAR + H_CB_WW, MessageData[i]);
  }
  ///
  /// Set Interrupt Generate bit
  ///
  HeciRegHCsr.ul      = MMIOREADDWORD (HeciMBAR + H_CSR);
  HeciRegHCsr.r.H_IG  = 1;
  MMIOWRITEDWORD (HeciMBAR + H_CSR, HeciRegHCsr.ul);

  ///
  /// Test if ME Ready bit is set to 1, if set to 0 a fatal error occured during
  /// the transmission of this message.
  ///
  HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
  if (HeciRegMeCsrHa.r.ME_RDY_HRA == 0) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Function sends one messsage through the HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in][out] Message         Pointer to the message buffer.
  @param[in] Length               Length of the message in bytes.
  @param[in][out] RecLength       Length of the message response in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the bufferbefore timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HECI
**/
EFI_STATUS
EFIAPI
HeciSendwACK (
  IN OUT  UINT32                  *Message,
  IN      UINT32                  Length,
  IN OUT  UINT32                  *RecLength,
  IN      UINT8                   HostAddress,
  IN      UINT8                   MeAddress
  )
{
  EFI_STATUS  Status;
  UINT16      RetryCount;
  UINT32      TempRecLength;
  UINT32      MeMode;

  HeciGetMeMode (&MeMode);
  if (MeMode == ME_MODE_SECOVER) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Send the message
  ///
  Status = HeciSend (Message, Length, HostAddress, MeAddress);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Wait for ACK message
  ///
  TempRecLength = *RecLength;
  for (RetryCount = 0; RetryCount < HECI_MAX_RETRY; RetryCount++) {
    ///
    /// Read Message
    ///
    Status = HeciReceive (BLOCKING, Message, &TempRecLength);
    if (!EFI_ERROR (Status)) {
      break;
    }
    ///
    /// Reload receive length as it has been modified by the read function
    ///
    TempRecLength = *RecLength;
  }
  ///
  /// Return read length and status
  ///
  *RecLength = TempRecLength;
  return Status;
}

/**
  Me reset and waiting for ready

  @param[in] Delay                The biggest waiting time

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Me is ready
**/
EFI_STATUS
EFIAPI
MeResetWait (
  IN  UINT32                      Delay
  )
{
  HECI_HOST_CONTROL_REGISTER  HeciRegHCsr;
  UINT32                      TimerStart;
  UINT32                      TimerEnd;

  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  HeciMBAR = CheckAndFixHeciForAccess ();

  ///
  /// Wait for the HOST Ready bit to be cleared to signal a reset
  ///
  StartTimer (&TimerStart, &TimerEnd, Delay);
  HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  while (HeciRegHCsr.r.H_RDY == 1) {
    ///
    /// If timeout has expired, return fail
    ///
    if (Timeout (TimerStart, TimerEnd) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }

    HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  }

  return EFI_SUCCESS;
}

/**
  Function forces a reinit of the heci interface by following the reset heci interface via host algorithm
  in HPS 0.90 doc 4-17-06 njy

  @param[in] none

  @retval EFI_TIMEOUT             ME is not ready
  @retval EFI_SUCCESS             Interface reset
**/
EFI_STATUS
EFIAPI
ResetHeciInterface (
  VOID
  )
{
  HECI_HOST_CONTROL_REGISTER      HeciRegHCsr;
  HECI_ME_CONTROL_REGISTER        HeciRegMeCsrHa;
  UINT32                          TimerStart;
  UINT32                          TimerEnd;

  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  HeciMBAR = CheckAndFixHeciForAccess ();

  ///
  /// Enable Reset
  ///
  HeciRegHCsr.ul      = MMIOREADDWORD (HeciMBAR + H_CSR);
  HeciRegHCsr.r.H_RST = 1;
  HeciRegHCsr.r.H_IG  = 1;
  MMIOWRITEDWORD (HeciMBAR + H_CSR, HeciRegHCsr.ul);

  ///
  /// Make sure that the reset started
  ///
  /// HeciRegHCsr.ul = MMIOREADDWORD(HeciMBAR + H_CSR);
  ///
  StartTimer (&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);
  do {
    ///
    /// If 5 second timeout has expired, return fail
    ///
    if (Timeout (TimerStart, TimerEnd) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    ///
    /// Read the ME CSR
    ///
    HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  } while (HeciRegHCsr.r.H_RDY == 1);

  ///
  /// Wait for ME to perform reset
  ///
  /// HeciRegMeCsrHa.ul = MMIOREADDWORD(HeciMBAR + ME_CSR_HA);
  ///
  StartTimer (&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);
  do {
    ///
    /// If 5 second timeout has expired, return fail
    ///
    if (Timeout (TimerStart, TimerEnd) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    ///
    /// Read the ME CSR
    ///
    HeciRegMeCsrHa.ul = MMIOREADDWORD (HeciMBAR + ME_CSR_HA);
  } while (HeciRegMeCsrHa.r.ME_RDY_HRA == 0);

  ///
  /// Make sure IS has been signaled on the HOST side
  ///
  /// HeciRegHCsr.ul = MMIOREADDWORD(HeciMBAR + H_CSR);
  ///
  StartTimer (&TimerStart, &TimerEnd, HECI_INIT_TIMEOUT);
  do {
    ///
    /// If 5 second timeout has expired, return fail
    ///
    if (Timeout (TimerStart, TimerEnd) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    ///
    /// Read the ME CSR
    ///
    HeciRegHCsr.ul = MMIOREADDWORD (HeciMBAR + H_CSR);
  } while (HeciRegHCsr.r.H_IS == 0);

  ///
  /// Enable host side interface
  ///
  HeciRegHCsr.ul      = MMIOREADDWORD (HeciMBAR + H_CSR);;
  HeciRegHCsr.r.H_RST = 0;
  HeciRegHCsr.r.H_IG  = 1;
  HeciRegHCsr.r.H_RDY = 1;
  MMIOWRITEDWORD (HeciMBAR + H_CSR, HeciRegHCsr.ul);

  return EFI_SUCCESS;
}

/**
  Calculate if the circular buffer has overflowed.
  Corresponds to HECI HPS (part of) section 4.2.1

  @param[in] ReadPointer          Location of the read pointer.
  @param[in] WritePointer         Location of the write pointer.

  @retval                         Number of filled slots.
**/
UINT8
FilledSlots (
  IN  UINT32                      ReadPointer,
  IN  UINT32                      WritePointer
  )
{
  UINT8 FilledSlots;

  ///
  /// Calculation documented in HECI HPS 0.68 section 4.2.1
  ///
  FilledSlots = (((INT8) WritePointer) - ((INT8) ReadPointer));

  return FilledSlots;
}

/**
  Calculate if the circular buffer has overflowed
  Corresponds to HECI HPS (part of) section 4.2.1

  @param[in] ReadPointer          Value read from host/me read pointer
  @param[in] WritePointer         Value read from host/me write pointer
  @param[in] BufferDepth          Value read from buffer depth register

  @retval EFI_DEVICE_ERROR        The circular buffer has overflowed
  @retval EFI_SUCCESS             The circular buffer does not overflowed
**/
EFI_STATUS
OverflowCB (
  IN  UINT32                      ReadPointer,
  IN  UINT32                      WritePointer,
  IN  UINT32                      BufferDepth
  )
{
  UINT8 FilledSlots;

  ///
  /// Calculation documented in HECI HPS 0.68 section 4.2.1
  ///
  FilledSlots = (((INT8) WritePointer) - ((INT8) ReadPointer));

  ///
  /// test for overflow
  ///
  if (FilledSlots > ((UINT8) BufferDepth)) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Get an abstract Intel ME State from Firmware Status Register.
  This is used to control BIOS flow for different Intel ME
  functions

  @param[out] MeStatus            Pointer for status report
                                  see MeState.h - Abstract ME status definitions.

  @retval EFI_SUCCESS             MeStatus copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeStatus is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeStatus (
  OUT UINT32                      *MeStatus
  )
{
  HECI_FWS_REGISTER               MeFirmwareStatus;

  if (MeStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MeFirmwareStatus.ul = HeciPciRead32 (R_FWSTATE);

  if (MeFirmwareStatus.r.CurrentState == ME_STATE_NORMAL && MeFirmwareStatus.r.ErrorCode == ME_ERROR_CODE_NO_ERROR) {
    *MeStatus = ME_READY;
  } else if (MeFirmwareStatus.r.CurrentState == ME_STATE_RECOVERY) {
    *MeStatus = ME_IN_RECOVERY_MODE;
  } else if (MeFirmwareStatus.r.CurrentState == ME_STATE_INIT) {
    *MeStatus = ME_INITIALIZING;
  } else if (MeFirmwareStatus.r.CurrentState == ME_STATE_DISABLE_WAIT) {
    *MeStatus = ME_DISABLE_WAIT;
  } else if (MeFirmwareStatus.r.CurrentState == ME_STATE_TRANSITION) {
    *MeStatus = ME_TRANSITION;
  } else {
    *MeStatus = ME_NOT_READY;
  }

  if (MeFirmwareStatus.r.FwUpdateInprogress) {
    *MeStatus |= ME_FW_UPDATES_IN_PROGRESS;
  }

  if (MeFirmwareStatus.r.FwInitComplete == ME_FIRMWARE_COMPLETED) {
    *MeStatus |= ME_FW_INIT_COMPLETE;
  }

  if (MeFirmwareStatus.r.MeBootOptionsPresent == ME_BOOT_OPTIONS_PRESENT) {
    *MeStatus |= ME_FW_BOOT_OPTIONS_PRESENT;
  }

  DEBUG ((EFI_D_ERROR, "HECI MeStatus %X\n", MeFirmwareStatus.ul));

  return EFI_SUCCESS;
}

/**
  Return ME Mode

  @param[out] MeMode              Pointer for ME Mode report

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeMode (
  OUT UINT32                       *MeMode
  )
{
  HECI_FWS_REGISTER MeFirmwareStatus;

  if (MeMode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MeFirmwareStatus.ul = HeciPciRead32 (R_FWSTATE);

  switch (MeFirmwareStatus.r.MeOperationMode) {
    case ME_OPERATION_MODE_NORMAL:
      *MeMode = ME_MODE_NORMAL;
      break;

    case ME_OPERATION_MODE_DEBUG:
      *MeMode = ME_MODE_DEBUG;
      break;

    case ME_OPERATION_MODE_SOFT_TEMP_DISABLE:
      *MeMode = ME_MODE_TEMP_DISABLED;
      break;

    case ME_OPERATION_MODE_SECOVR_JMPR:
    case ME_OPERATION_MODE_SECOVR_HECI_MSG:
      *MeMode = ME_MODE_SECOVER;
      break;

    default:
      *MeMode = ME_MODE_FAILED;
  }
  DEBUG ((EFI_D_ERROR, "HECI MeMode %X\n", MeFirmwareStatus.r.MeOperationMode));

  return EFI_SUCCESS;
}
