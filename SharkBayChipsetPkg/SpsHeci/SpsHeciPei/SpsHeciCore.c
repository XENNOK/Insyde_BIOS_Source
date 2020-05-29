/** @file

  Heci driver core. For PEI Phase, determines the HECI device and initializes it.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SpsHeciPei.h"

#include <SpsHeciRegs.h>

#include <SpsChipset.h>

/**
  Delay for at least the request number of microseconds

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] Microseconds         Number of microseconds to delay.

  @retval EFI_SUCCESS             The function completed successfully.
**/
EFI_STATUS
EFIAPI
Stall (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       UINTN                       Microseconds
  )
{
  UINTN   Ticks;
  UINTN   Counts;
  UINT32  CurrentTick;
  UINT32  OriginalTick;
  UINT32  RemainingTick;
  UINT32  AcpiPm1TimerBar;

  if (Microseconds == 0) {
    return EFI_SUCCESS;
  }

  AcpiPm1TimerBar = (
      PciRead32 (PCI_LIB_ADDRESS (SPS_PCI_BUS_NUMBER_PCH, SPS_PCI_DEVICE_NUMBER_PCH_LPC, SPS_PCI_FUNCTION_NUMBER_PCH_LPC, R_SPS_LPC_ACPI_BASE)) & B_SPS_LPC_ACPI_BASE_BAR
      ) +
    R_SPS_ACPI_PM1_TMR;

  OriginalTick = IoRead32 ((UINTN) (UINT64) AcpiPm1TimerBar);
  OriginalTick &= (SPS_PCH_ACPI_TIMER_MAX_VALUE - 1);
  CurrentTick = OriginalTick;

  ///
  /// The timer frequency is 3.579545MHz, so 1 ms corresponds to 3.58 clocks
  ///
  Ticks = Microseconds * 358 / 100 + OriginalTick + 1;

  ///
  /// The loops needed for timer overflow
  ///
  Counts = Ticks / SPS_PCH_ACPI_TIMER_MAX_VALUE;

  ///
  /// Remaining clocks within one loop
  ///
  RemainingTick = Ticks % SPS_PCH_ACPI_TIMER_MAX_VALUE;

  ///
  /// Do not intend to use TMROF_STS bit of register PM1_STS, because this add extra
  /// one I/O operation, and may generate SMI
  ///
  while (Counts != 0) {
    CurrentTick = IoRead32 ((UINTN) (UINT64) AcpiPm1TimerBar);
    CurrentTick &= (SPS_PCH_ACPI_TIMER_MAX_VALUE - 1);
    if (CurrentTick <= OriginalTick) {
      Counts--;
    }

    OriginalTick = CurrentTick;
  }

  while ((RemainingTick > CurrentTick) && (OriginalTick <= CurrentTick)) {
    OriginalTick  = CurrentTick;
    CurrentTick   = IoRead32 ((UINTN) (UINT64) AcpiPm1TimerBar);
    CurrentTick &= (SPS_PCH_ACPI_TIMER_MAX_VALUE - 1);
  }

  return EFI_SUCCESS;
}

/**
  Enable Hpet function.

  @param[in] PeiServices          General purpose services available to every PEIM.

  @retval UINT32                  Return the High Precision Event Timer base address
**/
volatile
UINT32 *
EnableHpet (
  IN CONST EFI_PEI_SERVICES            **PeiServices
  )
{
  volatile UINT32 *HpetConfigReg;
  volatile UINT32 *HpetTimer;
  UINT32          Temp32;
  HpetConfigReg = NULL;
  ///
  /// Get the High Precision Event Timer base address and enable the memory range
  ///
  HpetConfigReg = (UINT32 *) (SPS_RCRB_BASE + R_SPS_RCRB_HPTC);
  switch (*HpetConfigReg & B_SPS_RCRB_HPTC_AS) {
  case 0:
    HpetTimer = (VOID *) (SPS_HPET_ADDRESS_0);
    break;

  case 1:
    HpetTimer = (VOID *) (SPS_HPET_ADDRESS_1);
    break;

  case 2:
    HpetTimer = (VOID *) (SPS_HPET_ADDRESS_2);
    break;

  case 3:
    HpetTimer = (VOID *) (SPS_HPET_ADDRESS_3);
    break;

  default:
    HpetTimer = NULL;
    break;
  }

  *HpetConfigReg = *HpetConfigReg | B_SPS_RCRB_HPTC_AE;
  ///
  /// Read back from RCBA area.
  ///
  Temp32 = *HpetConfigReg;

  ///
  /// Start the timer so it is up and running
  ///
  HpetTimer[SPS_HPET_GEN_CONFIG_LOW] = SPS_HPET_START;

  return HpetTimer;
}

#ifdef EFI_DEBUG
/**
  For EFI debug used, will show the message of buffer to terminal.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] Message              The address of message.
  @param[in] Length               The length of message.
**/
VOID
ShowBuffer (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN UINT8                             *Message,
  IN UINT32                            Length
  )
{
  UINT32  LineBreak;
  UINT32  Index;
  LineBreak = 0;
  Index     = 0;

  while (Length-- > 0) {
    if (LineBreak == 0) {
      DEBUG ((EFI_D_ERROR, "%02x: ", (Index & 0xF0)));
    }

    DEBUG ((EFI_D_ERROR, "%02x ", Message[Index++]));
    LineBreak++;
    if (LineBreak == 16) {
      DEBUG ((EFI_D_ERROR, "\n"));
      LineBreak = 0;
    }

    if (LineBreak == 8) {
      DEBUG ((EFI_D_ERROR, "- "));
    }
  }

  DEBUG ((EFI_D_ERROR, "\n"));
  return ;
}
#endif /// End Of EFI_DEBUG

/**
  Determines if the HECI device is present and, if present, initializes it for
  use by the BIOS.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI
  @param[in, out] HeciMemBar      HECI Memory BAR

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_DEVICE_ERROR        No HECI device
  @retval EFI_TIMEOUT             HECI does not return the buffer before timeout
  @exception EFI_UNSUPPORTED      HECI MSG is unsupported
**/
EFI_STATUS
EFIAPI
InitializeHeci (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN OUT   UINT32                      *HeciMemBar
  )
{
  SPS_HECI_HOST_CONTROL_REGISTER          HeciRegHCsr;
  volatile SPS_HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;
  volatile SPS_HECI_ME_CONTROL_REGISTER   *HeciRegMeCsrHaPtr;
  UINT32                                  HeciMBar;
  UINT32                                  MeMode;

  HeciRegHCsrPtr    = NULL;
  HeciRegMeCsrHaPtr = NULL;

  ///
  /// Check for HECI-1 PCI device availability
  ///
  if (SpsHeciPciRead16 (PCI_DEVICE_ID_OFFSET) == 0xFFFF) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Check for ME error status
  ///
  if ((SpsHeciPciRead32 (R_FWSTATE) & 0xF000) != 0) {
    ///
    /// ME failed to start so no HECI
    ///
    return EFI_DEVICE_ERROR;
  }
  ///
  /// HECI MSG is unsupported if ME MODE is in Security Override
  ///
  HeciGetMeMode (PeiServices, &MeMode);
  if (MeMode == SPS_ME_MODE_SECOVER) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Get HECI_MBAR and see if it is programmed
  /// to a useable value
  ///
  HeciMBar = SpsHeciPciRead32 (R_HECIMBAR) & 0xFFFFFFF0;

  ///
  /// Load temporary address for HECI_MBAR if one is not assigned
  ///
  if (HeciMBar == 0) {
    DEBUG ((EFI_D_ERROR, "Heci MMIO Bar not programmed in PEI phase\n"));
  }

  *HeciMemBar = HeciMBar;

  ///
  /// Enable HECI BME and MSE
  ///
  SpsHeciPciOr8 (
    PCI_COMMAND_OFFSET,
    EFI_PCI_COMMAND_MEMORY_SPACE | EFI_PCI_COMMAND_BUS_MASTER
    );

  ///
  /// Set HECI interrupt delivery mode.
  /// HECI-1 using legacy/MSI interrupt
  ///
  SpsHeciPciAnd8 (R_HIDM, 0xFC);

  ///
  ///  2) setup H_CSR reg as follows:
  ///     a) Make sure H_RST is clear
  ///     b) Set H_RDY
  ///     c) Set H_IG
  ///
  HeciRegHCsrPtr  = (VOID *) (HeciMBar + H_CSR);
  HeciRegHCsr.ul  = HeciRegHCsrPtr->ul;
  if (HeciRegHCsrPtr->r.H_RDY == 0) {
    HeciRegHCsr.r.H_RST = 0;
    HeciRegHCsr.r.H_RDY = 1;
    HeciRegHCsr.r.H_IG  = 1;
    HeciRegHCsrPtr->ul  = HeciRegHCsr.ul;
  }

  return EFI_SUCCESS;
}

/**
  Waits for the ME to report that it is ready for communication over the HECI
  interface.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI.
  @param[in] HeciMemBar           HECI Memory BAR.

  @retval EFI_SUCCESS             The function completed successfully.
  @retval EFI_TIMEOUT             If 5 second timeout has expired, return fail.
**/
EFI_STATUS
WaitForMEReady (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN       UINT32                      HeciMemBar
  )
{
  UINT32                                    TimerStart;
  UINT32                                    TimerEnd;
  volatile SPS_HECI_ME_CONTROL_REGISTER     *HeciRegMeCsrHaPtr;
  volatile UINT32                           *HpetTimer;

  HeciRegMeCsrHaPtr = (VOID *) (HeciMemBar + ME_CSR_HA);
  ///
  /// Wait for ME ready
  ///
  ///
  /// Check for ME ready status
  ///
  HpetTimer = StartTimer (PeiServices, &TimerStart, &TimerEnd, SPS_PEI_HECI_INIT_TIMEOUT);
  while (HeciRegMeCsrHaPtr->r.ME_RDY_HRA == 0) {
    ///
    /// If 5 second timeout has expired, return fail
    ///
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    ///
    /// Perform IO delay
    ///
    Stall (PeiServices, SPS_PEI_HECI_WAIT_DELAY);
  }
  ///
  /// ME ready!!!
  ///
  return EFI_SUCCESS;
}

/**
  Read the HECI Message from Intel ME with size in Length into
  buffer Message. Set Blocking to BLOCKING and code will wait
  until one message packet is received. When set to
  NON_BLOCKING, if the circular buffer is empty at the time, the
  code not wait for the message packet read.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI.
  @param[in] HeciMemBar           HECI Memory BAR.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[in] MessageBody          Pointer to a buffer used to receive a message.
  @param[in, out] Length          Pointer to the length of the buffer on input and the length
                                  of the message on return. (in bytes)

  @exception EFI_UNSUPPORTED      Current ME mode doesn't support this function
  @retval EFI_SUCCESS             One message packet read
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @retval EFI_DEVICE_ERROR        Zero-length message packet read
  @retval EFI_BUFFER_TOO_SMALL    The caller's buffer was not large enough
**/
EFI_STATUS
EFIAPI
HeciReadMsg (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN       UINT32                      Blocking,
  IN       UINT32                      HeciMemBar,
  IN       UINT32                      *MessageBody,
  IN OUT   UINT32                      *Length
  )
{
  SPS_HECI_MESSAGE_HEADER    PacketHeader;
  UINT32                     CurrentLength;
  UINT32                     MessageComplete;
  EFI_STATUS                 ReadError;
  UINT32                     PacketBuffer;
  UINT32                     timer_start;
  UINT32                     timer_end;
  volatile UINT32            *HpetTimer;
  UINT32                     MeMode;

  CurrentLength   = 0;
  MessageComplete = 0;

  HeciGetMeMode (PeiServices, &MeMode);
  if (MeMode == SPS_ME_MODE_SECOVER) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  ///  CheckAndFixHeciForAccess (PeiServices, This);
  ///
  /// Make sure that HECI is ready for communication.
  ///
  if (WaitForMEReady (PeiServices, This, HeciMemBar) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }
  ///
  /// Set up timer for BIOS timeout.
  ///
  HpetTimer = StartTimer (PeiServices, &timer_start, &timer_end, SPS_PEI_HECI_READ_TIMEOUT);
  while ((CurrentLength < *Length) && (MessageComplete == 0)) {
    ///
    /// If 1 second timeout has expired, return fail as we have not yet received a full message
    ///
    if (Timeout (timer_start, timer_end, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }

    PacketBuffer = *Length - CurrentLength;
    ReadError = HeciPacketRead (
                  PeiServices,
                  Blocking,
                  HeciMemBar,
                  &PacketHeader,
                  (UINT32 *) &MessageBody[CurrentLength / 4],
                  &PacketBuffer
                  );

    DEBUG ((EFI_D_ERROR, "HECI Header: %08x\n", PacketHeader.Data));

    ///
    /// Check for error condition on read
    ///
    if (EFI_ERROR (ReadError)) {
      *Length = 0;
      return ReadError;
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
        *Length = 0;
        return EFI_SUCCESS;
      } else {
        ///
        /// We should not expect a zero-length message packet except as described above.
        ///
        *Length = 0;
        return EFI_DEVICE_ERROR;
      }
    }
    ///
    /// Track the length of what we have read so far
    ///
    CurrentLength += PacketBuffer;

  }
  ///
  /// If we get here the message should be complete, if it is not
  /// the caller's buffer was not large enough.
  ///
  if (MessageComplete == 0) {
    *Length = 0;
    return EFI_BUFFER_TOO_SMALL;
  }

  *Length = CurrentLength;

  DEBUG ((EFI_D_ERROR, "HECI ReadMsg:\n"));
#ifdef EFI_DEBUG
  DEBUG_CODE (
    ShowBuffer (PeiServices, (UINT8 *) MessageBody, *Length);
  );
#endif /// End Of EFI_DEBUG
  return EFI_SUCCESS;
}

/**
  Function to pull one messsage packet off the HECI circular buffer.
  Corresponds to HECI HPS (part of) section 4.2.4

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] HeciMemBar           HECI Memory BAR.
  @param[in] Blocking             Used to determine if the read is BLOCKING or NON_BLOCKING.
  @param[out] MessageHeader       Pointer to a buffer for the message header.
  @param[out] MessageData         Pointer to a buffer to recieve the message in.
  @param[in, out] Length          On input is the size of the callers buffer in bytes. On
                                  output this is the size of the packet in bytes.

  @retval EFI_SUCCESS             One message packet read.
  @retval EFI_DEVICE_ERROR        The circular buffer is overflowed.
  @retval EFI_NO_RESPONSE         The circular buffer is empty
  @retval EFI_TIMEOUT             Failed to receive a full message
  @retval EFI_BUFFER_TOO_SMALL    Message packet is larger than caller's buffer
**/
EFI_STATUS
HeciPacketRead (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       UINT32                      Blocking,
  IN       UINT32                      HeciMemBar,
  OUT      SPS_HECI_MESSAGE_HEADER     *MessageHeader,
  OUT      UINT32                      *MessageData,
  IN OUT   UINT32                      *Length
  )
{
  BOOLEAN                             GotMessage;
  UINT32                              TimerStart;
  UINT32                              TimerEnd;
  UINT32                              TimerStart1;
  UINT32                              TimerEnd1;
  UINT32                              i;
  UINT32                              LengthInDwords;
  SPS_HECI_ME_CONTROL_REGISTER        HeciRegMeCsrHa;
  volatile UINT32                     *HpetTimer;
  ///
  /// Initialize memory mapped register pointers
  ///
  volatile SPS_HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;
  volatile SPS_HECI_ME_CONTROL_REGISTER   *HeciRegMeCsrHaPtr;
  volatile UINT32                         *HeciRegMeCbrwPtr;

  HeciRegHCsrPtr    = (VOID *) (HeciMemBar + H_CSR);
  HeciRegMeCsrHaPtr = (VOID *) (HeciMemBar + ME_CSR_HA);
  HeciRegMeCbrwPtr  = (VOID *) (HeciMemBar + ME_CB_RW);

  GotMessage        = FALSE;

  ///
  /// clear Interrupt Status bit
  ///
  HeciRegHCsrPtr->r.H_IS = 1;

  ///
  /// test for circular buffer overflow
  ///
  HeciRegMeCsrHa.ul = HeciRegMeCsrHaPtr->ul;
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
  HeciRegMeCsrHa.ul = HeciRegMeCsrHaPtr->ul;
  if ((FilledSlots (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA) == 0) && (Blocking == NON_BLOCKING)) {
    *Length = 0;
    return EFI_NO_RESPONSE;
  }
  ///
  /// Start timeout counter
  ///
  HpetTimer = StartTimer (PeiServices, &TimerStart, &TimerEnd, SPS_PEI_HECI_READ_TIMEOUT);

  ///
  /// loop until we get a message packet
  ///
  while (!GotMessage) {
    ///
    /// If 1 second timeout has expired, return fail as we have not yet received a full message.
    ///
    if (Timeout (TimerStart, TimerEnd, HpetTimer) != EFI_SUCCESS) {
      *Length = 0;
      return EFI_TIMEOUT;
    }
    ///
    /// Read one message from HECI buffer and advance read pointer.  Make sure
    /// that we do not pass the write pointer.
    ///
    HeciRegMeCsrHa.ul = HeciRegMeCsrHaPtr->ul;
    if (FilledSlots (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA) > 0) {
      ///
      /// Eat the HECI Message header
      ///
      MessageHeader->Data = *HeciRegMeCbrwPtr;

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
        HeciRegHCsrPtr->r.H_IG  = 1;
        *Length                 = 0;
        return EFI_SUCCESS;
      }
      ///
      /// Make sure that the message does not overflow the circular buffer.
      ///
      if ((MessageHeader->Fields.Length + sizeof (SPS_HECI_MESSAGE_HEADER)) > (HeciRegMeCsrHaPtr->r.ME_CBD_HRA * 4)) {
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
        HpetTimer = StartTimer (PeiServices, &TimerStart1, &TimerEnd1, SPS_PEI_HECI_READ_TIMEOUT);

        ///
        /// Wait here until entire message is present in circular buffer
        ///
        HeciRegMeCsrHa.ul = HeciRegMeCsrHaPtr->ul;
        while (LengthInDwords > FilledSlots (HeciRegMeCsrHa.r.ME_CBRP_HRA, HeciRegMeCsrHa.r.ME_CBWP_HRA)) {
          ///
          /// If 1 second timeout has expired, return fail as we have not yet received a full message
          ///
          if (Timeout (TimerStart1, TimerEnd1, HpetTimer) != EFI_SUCCESS) {
            *Length = 0;
            return EFI_TIMEOUT;
          }
          ///
          /// Wait before we read the register again
          ///
          Stall (PeiServices, SPS_PEI_HECI_WAIT_DELAY);

          ///
          /// Read the register again
          ///
          HeciRegMeCsrHa.ul = HeciRegMeCsrHaPtr->ul;
        }
        ///
        /// copy rest of message
        ///
        for (i = 0; i < LengthInDwords; i++) {
          MessageData[i] = *HeciRegMeCbrwPtr;
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
    Stall (PeiServices, SPS_PEI_HECI_WAIT_DELAY);
  }
  ///
  /// Read ME_CSR_HA.  If the ME_RDY bit is 0, then an ME reset occurred during the
  /// transaction and the message should be discarded as bad data may have been retrieved
  /// from the host's circular buffer
  ///
  if (HeciRegMeCsrHaPtr->r.ME_RDY_HRA == 0) {
    *Length = 0;
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Set Interrupt Generate bit
  ///
  HeciRegHCsrPtr->r.H_IG = 1;

  return EFI_SUCCESS;
}

/**
  Function sends one messsage (of any length) through the HECI circular buffer.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI.
  @param[in] HeciMemBar           HECI Memory BAR.
  @param[in] Message              Pointer to the message data to be sent.
  @param[in] Length               Length of the message in bytes.
  @param[in] HostAddress          The address of the host processor.
  @param[in] MeAddress            Address of the ME subsystem the message is being sent to.

  @retval EFI_SUCCESS             One message packet sent.
  @retval EFI_DEVICE_ERROR        Failed to initialize HECI
  @retval EFI_TIMEOUT             HECI is not ready for communication
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HEC
**/
EFI_STATUS
HeciSendMsg (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN       UINT32                      *Message,
  IN       UINT32                      HeciMemBar,
  IN       UINT32                      Length,
  IN       UINT8                       HostAddress,
  IN       UINT8                       MeAddress
  )
{
  UINT32                                    CBLength;
  UINT32                                    SendLength;
  UINT32                                    CurrentLength;
  SPS_HECI_MESSAGE_HEADER                   MessageHeader;
  EFI_STATUS                                WriteStatus;
  volatile SPS_HECI_HOST_CONTROL_REGISTER   *HeciRegHCsrPtr;
  UINT32                                    MeMode;

  CurrentLength   = 0;
  HeciRegHCsrPtr  = NULL;

  HeciGetMeMode (PeiServices, &MeMode);
  if (MeMode == SPS_ME_MODE_SECOVER) {
    return EFI_UNSUPPORTED;
  }

  DEBUG ((EFI_D_ERROR, "HECI SendMsg:\n"));
#ifdef EFI_DEBUG
  DEBUG_CODE (
    ShowBuffer (PeiServices, (UINT8 *) Message, Length);
  );
#endif /// End Of EFI_DEBUG
  ///
  /// Make sure that HECI device BAR is correct and device is enabled.
  ///
  ///  CheckAndFixHeciForAccess (PeiServices, This);
  ///
  /// Make sure that HECI is ready for communication.
  ///
  if (WaitForMEReady (PeiServices, This, HeciMemBar) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }
  ///
  /// Set up memory mapped registers
  ///
  HeciRegHCsrPtr = (VOID *) (HeciMemBar + H_CSR);

  ///
  /// Grab Circular Buffer length
  ///
  CBLength = HeciRegHCsrPtr->r.H_CBD;

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
    /// Needs to be 'less than' to account for the header.
    ///
    if ((((Length - CurrentLength) + 3) / 4) < CBLength) {
      MessageHeader.Fields.MessageComplete = 1;
    }
    ///
    /// Calculate length for Message Header
    ///    header length == smaller of circular buffer or remaining message (both account for the size of the header)
    ///
    SendLength                  = ((CBLength < (((Length - CurrentLength) + 3) / 4)) ? ((CBLength - 1) * 4) : (Length - CurrentLength));
    MessageHeader.Fields.Length = SendLength;

    DEBUG ((EFI_D_ERROR, "HECI Header: %08x\n", MessageHeader.Data));
    ///
    /// send the current packet (CurrentLength can be treated as the index into the message buffer)
    ///
    WriteStatus = HeciPacketWrite (
                    PeiServices,
                    This,
                    HeciMemBar,
                    &MessageHeader,
                    (UINT32 *) ((UINT32) Message + CurrentLength)
                    );
    if (EFI_ERROR (WriteStatus)) {
      return WriteStatus;
    }
    ///
    /// Update the length information
    ///
    CurrentLength += SendLength;
  }

  return EFI_SUCCESS;
}

/**
  Function sends one messsage packet through the HECI circular buffer
  Corresponds to HECI HPS (part of) section 4.2.3

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI.
  @param[in] HeciMemBar           HECI Memory BAR.
  @param[in] MessageHeader        Pointer to the message header.
  @param[in] MessageData          Pointer to the actual message data.

  @retval EFI_SUCCESS             One message packet sent
  @retval EFI_DEVICE_ERROR        ME is not ready
  @retval EFI_TIMEOUT             HECI is not ready for communication
**/
EFI_STATUS
HeciPacketWrite (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN       UINT32                      HeciMemBar,
  IN       SPS_HECI_MESSAGE_HEADER     *MessageHeader,
  IN       UINT32                      *MessageData
  )
{
  UINT32                                  timer_start;
  UINT32                                  timer_end;
  UINT32                                  i;
  UINT32                                  LengthInDwords;
  SPS_HECI_HOST_CONTROL_REGISTER          HeciRegHCsr;
  volatile UINT32                         *HpetTimer;
  volatile SPS_HECI_HOST_CONTROL_REGISTER *HeciRegHCsrPtr;
  volatile SPS_HECI_ME_CONTROL_REGISTER   *HeciRegMeCsrHaPtr;
  volatile UINT32                         *HeciRegHCbwwPtr;

  HeciRegHCsrPtr    = (VOID *) (HeciMemBar + H_CSR);
  HeciRegMeCsrHaPtr = (VOID *) (HeciMemBar + ME_CSR_HA);
  HeciRegHCbwwPtr   = (VOID *) (HeciMemBar + H_CB_WW);
  ///
  /// Make sure that HECI is ready for communication.
  ///
  if (WaitForMEReady (PeiServices, This, HeciMemBar) != EFI_SUCCESS) {
    return EFI_TIMEOUT;
  }
  ///
  /// Start timeout counter
  ///
  HpetTimer = StartTimer (PeiServices, &timer_start, &timer_end, SPS_PEI_HECI_SEND_TIMEOUT);

  ///
  /// Compute message length in DWORDS
  ///
  LengthInDwords = ((MessageHeader->Fields.Length + 3) / 4);

  ///
  /// Wait until there is sufficient room in the circular buffer
  /// Must have room for message and message header
  ///
  HeciRegHCsr.ul = HeciRegHCsrPtr->ul;
  while ((LengthInDwords + 1) > (HeciRegHCsr.r.H_CBD - FilledSlots (HeciRegHCsr.r.H_CBRP, HeciRegHCsr.r.H_CBWP))) {
    ///
    /// If 1 second timeout has expired, return fail as the circular buffer never emptied
    ///
    if (Timeout (timer_start, timer_end, HpetTimer) != EFI_SUCCESS) {
      return EFI_TIMEOUT;
    }
    ///
    /// Wait before we read the register again
    ///
    Stall (PeiServices, SPS_PEI_HECI_WAIT_DELAY);

    ///
    /// Read Host CSR for next iteration
    ///
    HeciRegHCsr.ul = HeciRegHCsrPtr->ul;
  }
  ///
  /// Write Message Header
  ///
  *HeciRegHCbwwPtr = MessageHeader->Data;

  ///
  /// Write Message Body
  ///
  for (i = 0; i < LengthInDwords; i++) {
    *HeciRegHCbwwPtr = MessageData[i];
  }
  ///
  /// Set Interrupt Generate bit
  ///
  HeciRegHCsrPtr->r.H_IG = 1;

  ///
  /// Test if ME Ready bit is set to 1, if set to 0 a fatal error occured during
  /// the transmission of this message.
  ///
  if (HeciRegMeCsrHaPtr->r.ME_RDY_HRA == 0) {
    return EFI_DEVICE_ERROR;
  }

  return EFI_SUCCESS;
}

/**
  Function sends one messsage through the HECI circular buffer and waits
  for the corresponding ACK message.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 The address of HECI PPI.
  @param[in, out] Message         Pointer to the message buffer.
  @param[in] HeciMemBar           HECI Memory BAR.
  @param[in, out] Length          Length of the message in bytes.
  @param[in] HostAddress          Address of the sending entity.
  @param[in] MeAddress            Address of the ME entity that should receive the message.

  @retval EFI_SUCCESS             Command succeeded
  @retval EFI_DEVICE_ERROR        HECI Device error, command aborts abnormally
  @retval EFI_TIMEOUT             HECI does not return the bufferbefore timeout
  @retval EFI_BUFFER_TOO_SMALL    Message Buffer is too small for the Acknowledge
  @exception EFI_UNSUPPORTED      Current ME mode doesn't support send message through HECI
**/
EFI_STATUS
HeciSendwAck (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       SPS_PEI_HECI_PPI            *This,
  IN OUT   UINT32                      *Message,
  IN       UINT32                      HeciMemBar,
  IN OUT   UINT32                      *Length,
  IN       UINT8                       HostAddress,
  IN       UINT8                       MeAddress
  )
{
  EFI_STATUS                      Status;
  UINT32                          MeMode;

  HeciGetMeMode (PeiServices, &MeMode);
  if (MeMode == SPS_ME_MODE_SECOVER) {
    return EFI_UNSUPPORTED;
  }
  ///
  /// Send the message
  ///
  Status = HeciSendMsg (PeiServices, This, Message, HeciMemBar, *Length, HostAddress, MeAddress);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  ///
  /// Wait for ACK message
  ///
  return HeciReadMsg (PeiServices, This, BLOCKING, HeciMemBar, Message, Length);
}

/**
  Calculate if the circular buffer has overflowed.
  Corresponds to HECI HPS (part of) section 4.2.1

  @param[in] ReadPointer          Location of the read pointer.
  @param[in] WritePointer         Location of the write pointer.

  @retval UINT8                   Number of filled slots.
**/
UINT8
FilledSlots (
  IN UINT32                            ReadPointer,
  IN UINT32                            WritePointer
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
  IN UINT32                            ReadPointer,
  IN UINT32                            WritePointer,
  IN UINT32                            BufferDepth
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
  Used for calculating timeouts

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] Start                Snapshot of the HPET timer
  @param[in] End                  Calculated time when timeout period will be done
  @param[in] Time                 Timeout period in microseconds

  @retval None
**/
volatile
UINT32 *
StartTimer (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  OUT      UINT32                      *Start,
  OUT      UINT32                      *End,
  IN       UINT32                      Time
  )
{
  UINT32          Ticks;
  volatile UINT32 *HpetTimer;
  ///
  /// Make sure that HPET is enabled and running
  ///
  HpetTimer = EnableHpet (PeiServices);

  ///
  /// Read current timer value into start time from HPET
  ///
  *Start = HpetTimer[SPS_HPET_MAIN_COUNTER_LOW];

  ///
  /// Convert microseconds into 70ns timer ticks
  ///
  Ticks = Time * SPS_HPET_TICKS_PER_MICRO;

  ///
  /// Compute end time
  ///
  *End = *Start + Ticks;

  return HpetTimer;
}

/**
  Used to determine if a timeout has occured.

  @param[in] Start                Snapshot of the HPET timer when the timeout period started.
  @param[in] End                  Calculated time when timeout period will be done.
  @param[in] HpetTimer            The value of High Precision Event Timer

  @retval EFI_TIMEOUT             Timeout occured.
  @retval EFI_SUCCESS             Not yet timed out
**/
EFI_STATUS
Timeout (
  IN UINT32                            Start,
  IN UINT32                            End,
  IN volatile UINT32                   *HpetTimer
  )
{
  UINT32  Current;

  ///
  /// Read HPET and assign the value as the current time.
  ///
  Current = HpetTimer[SPS_HPET_MAIN_COUNTER_LOW];

  ///
  /// If no timeout, current timer always is +1 to Start timer
  ///
  ///  if(This->HeciTimeout == 0){
  ///    Current = Start + 1;
  ///  }
  ///
  /// Test basic case (no overflow)
  ///
  if ((Start < End) && (End <= Current)) {
    return EFI_TIMEOUT;
  }
  ///
  /// Test basic start/end conditions with overflowed timer
  ///
  if ((Start < End) && (Current < Start)) {
    return EFI_TIMEOUT;
  }
  ///
  /// Test for overflowed start/end condition
  ///
  if ((Start > End) && ((Current < Start) && (Current > End))) {
    return EFI_TIMEOUT;
  }
  ///
  /// Catch corner case of broken arguments
  ///
  if (Start == End) {
    return EFI_TIMEOUT;
  }
  ///
  /// Else, we have not yet timed out
  ///
  return EFI_SUCCESS;
}

/**
  Get an abstract Intel ME Status from Firmware Status Register.
  This is used to control BIOS flow for different Intel ME
  functions.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] MeStatus             Pointer for status report,

  @retval EFI_SUCCESS             MeStatus copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeStatus is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeStatus (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       UINT32                      *MeStatus
  )
{
  SPS_HECI_FWS_REGISTER      MeFirmwareStatus;

  if (MeStatus == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MeFirmwareStatus.ul = SpsHeciPciRead32 (R_FWSTATE);
  if (MeFirmwareStatus.r.CurrentState == SPS_ME_STATE_NORMAL && MeFirmwareStatus.r.ErrorCode == SPS_ME_ERROR_CODE_NO_ERROR) {
    *MeStatus = SPS_ME_READY;
  } else if (MeFirmwareStatus.r.CurrentState == SPS_ME_STATE_RECOVERY) {
    *MeStatus = SPS_ME_IN_RECOVERY_MODE;
  } else {
    *MeStatus = SPS_ME_NOT_READY;
  }

  if (MeFirmwareStatus.r.FwInitComplete == SPS_ME_FIRMWARE_COMPLETED) {
    *MeStatus |= SPS_ME_FW_INIT_COMPLETE;
  }

  DEBUG ((EFI_D_ERROR, "HECI MeStatus %X\n", *MeStatus));

  return EFI_SUCCESS;
}

/**
  Get an abstract ME operation mode from firmware status
  register. This is used to control BIOS flow for different
  Intel ME functions.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] MeMode               Pointer for ME Mode report,

  @retval EFI_SUCCESS             MeMode copied
  @retval EFI_INVALID_PARAMETER   Pointer of MeMode is invalid
**/
EFI_STATUS
EFIAPI
HeciGetMeMode (
  IN CONST EFI_PEI_SERVICES            **PeiServices,
  IN       UINT32                      *MeMode
  )
{
  SPS_HECI_FWS_REGISTER      MeFirmwareStatus;

  if (MeMode == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  MeFirmwareStatus.ul = SpsHeciPciRead32 (R_FWSTATE);

  switch (MeFirmwareStatus.r.MeOperationMode) {
    case SPS_ME_OPERATION_MODE_NORMAL:
    case SPS_ME_OPERATION_MODE_SPS:
      *MeMode = SPS_ME_MODE_NORMAL;
      break;

    case SPS_ME_OPERATION_MODE_DEBUG:
      *MeMode = SPS_ME_MODE_DEBUG;
      break;

    case SPS_ME_OPERATION_MODE_SOFT_TEMP_DISABLE:
      *MeMode = SPS_ME_MODE_TEMP_DISABLED;
      break;

    case SPS_ME_OPERATION_MODE_SECOVR_JMPR:
    case SPS_ME_OPERATION_MODE_SECOVR_HECI_MSG:
      *MeMode = SPS_ME_MODE_SECOVER;
      break;

    default:
      *MeMode = SPS_ME_MODE_FAILED;
  }

  DEBUG ((EFI_D_ERROR, "HECI MeMode %X\n", *MeMode));
  return EFI_SUCCESS;
}
