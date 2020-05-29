/** @file

 Component description file for the RapidStart Common code PEIM/SMM/DXE library

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
#include <Uefi.h>
#include "RapidStartData.h"
#include <ChipsetCmos.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/BaseLib.h>
#include <Library/CmosLib.h>
#include <Library/PchPlatformLib/PchPlatformLibrary.h>
#include <PchRegs/PchRegsLpc.h>

#define R_PCH_RTC_INDEX_ALT       0x74
#define R_PCH_RTC_TARGET_ALT      0x75
#define R_PCH_RTC_EXT_INDEX_ALT   0x76
#define R_PCH_RTC_EXT_TARGET_ALT  0x77

#define R_PCH_RTC_REGC            0x0C
#define B_PCH_RTC_REGC_AF         0x20

#define RTC_INDEX_MASK            0x7F
#define RTC_BANK_SIZE             0x80

#define R_PCH_RTC_SECOND          0x00
#define R_PCH_RTC_ALARM_SECOND    0x01
#define R_PCH_RTC_MINUTE          0x02
#define R_PCH_RTC_ALARM_MINUTE    0x03
#define R_PCH_RTC_HOUR            0x04
#define R_PCH_RTC_ALARM_HOUR      0x05
#define R_PCH_RTC_DAY_OF_WEEK     0x06
#define R_PCH_RTC_DAY_OF_MONTH    0x07
#define R_PCH_RTC_MONTH           0x08
#define R_PCH_RTC_YEAR            0x09

///
/// Non-volatile RapidStart flag must be kept within locked CMOS range
/// to prevent triggering RapidStart resume by manipulating CMOS in OS runtime,
/// and mitigate attacks on SMRAM image content.
///
//#ifndef FFS_NV_FLAG_REG
//#define FFS_NV_FLAG_REG 0xBB
//#endif
///
/// Some runtime configuration settings (e.g. enabled wake events must be
/// available to determine RapidStart transition before memory is initialized.
/// This CMOS register is used to store these settings, thus must be
/// within unlocked range.
///
// #ifndef FFS_NV_DATA_REG
// #define FFS_NV_DATA_REG 0x0047
// #endif
///
/// Defines for KSC common code
/// Timeout if KSC command/data fails
///
#define KSC_TIME_OUT    0x20000
#define KSC_WAIT_PERIOD 15
///
/// The Keyboard and System management Controller (KSC) implements a standard 8042 keyboard
/// controller interface at ports 0x60/0x64 and a ACPI compliant system management controller
/// at ports 0x62/0x66. Port 0x66 is the command and status port, port 0x62 is the data port.
///
#define KSC_D_PORT  0x62
#define KSC_C_PORT  0x66

//
// Status Port 0x62
//
#define KSC_S_OVR_TMP                               0x80  ///< Current CPU temperature exceeds the threshold
#define KSC_S_SMI_EVT                               0x40  ///< SMI event is pending
#define KSC_S_SCI_EVT                               0x20  ///< SCI event is pending
#define KSC_S_BURST                                 0x10  ///< KSC is in burst mode or normal mode
#define KSC_S_CMD                                   0x08  ///< Byte in data register is command/data
#define KSC_S_IGN                                   0x04  ///< Ignored
#define KSC_S_IBF                                   0x02  ///< Input buffer is full/empty
#define KSC_S_OBF                                   0x01  ///< Output buffer is full/empty
#define KSC_CMD_SET_CRITICAL_BATTERY_WAKE_THRESHOLD 0x34
#define DISABLE_CRITICAL_BATTERY_WAKE               0
#define MINIMAL_CRITICAL_BATTERY_WAKE_THRESHOLD     15
#define KSC_CMD_GET_WAKE_STATUS                     0x35
#define KSC_WAKE_STATUS_CRITICAL_BATTERY            1
#define KSC_CMD_CLEAR_WAKE_STATUS                   0x36

/**
  Read specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for read

  @retval The data of specific location in RTC/CMOS RAM.
**/
UINT8
RtcRead (
  IN      UINT8        Location
  )
{
  UINT8 RtcIndexPort;
  UINT8 RtcDataPort;

  ///
  /// CMOS access registers (using alternative access not to handle NMI bit)
  ///
  if (Location < RTC_BANK_SIZE) {
    ///
    /// First bank
    ///
    RtcIndexPort  = R_PCH_RTC_INDEX_ALT;
    RtcDataPort   = R_PCH_RTC_TARGET_ALT;
  } else {
    ///
    /// Second bank
    ///
    RtcIndexPort  = R_PCH_RTC_EXT_INDEX_ALT;
    RtcDataPort   = R_PCH_RTC_EXT_TARGET_ALT;
  }

  IoWrite8 (RtcIndexPort, Location & RTC_INDEX_MASK);
  return IoRead8 (RtcDataPort);
}

/**
  Write specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for write
  @param[in] Value           The data that will be written to RTC/CMOS RAM
**/
VOID
RtcWrite (
  IN      UINT8        Location,
  IN      UINT8        Value
  )
{
  UINT8 RtcIndexPort;
  UINT8 RtcDataPort;

  ///
  /// CMOS access registers (using alternative access not to handle NMI bit)
  ///
  if (Location < RTC_BANK_SIZE) {
    ///
    /// First bank
    ///
    RtcIndexPort  = R_PCH_RTC_INDEX_ALT;
    RtcDataPort   = R_PCH_RTC_TARGET_ALT;
  } else {
    ///
    /// Second bank
    ///
    RtcIndexPort  = R_PCH_RTC_EXT_INDEX_ALT;
    RtcDataPort   = R_PCH_RTC_EXT_TARGET_ALT;
  }

  IoWrite8 (RtcIndexPort, Location & RTC_INDEX_MASK);
  IoWrite8 (RtcDataPort, Value);
}

/**
  Read word from specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for read

  @retval The data of specific location in RTC/CMOS RAM.
**/
UINT16
RtcRead16 (
  IN      UINT8        Location
  )
{
  return RtcRead (Location) | (RtcRead (Location + 1) << 8);
}

/**
  Write word to specific RTC/CMOS RAM

  @param[in] Location        Point to RTC/CMOS RAM offset for write
  @param[in] Value           The data that will be written to RTC/CMOS RAM
**/
VOID
RtcWrite16 (
  IN      UINT8        Location,
  IN      UINT16       Value
  )
{
  RtcWrite (Location, (UINT8) Value);
  RtcWrite (Location + 1, (UINT8) (Value >> 8));
}

/**
  Initialize RTC Timer
**/
VOID
RtcInit (
  VOID
  )
{
  UINT8 RegB;
  RegB = RtcRead (R_PCH_RTC_REGB);
  RegB |= B_PCH_RTC_REGB_HOURFORM;
  RegB &= ~B_PCH_RTC_REGB_DM;
  RtcWrite (R_PCH_RTC_REGB, RegB);
}

/**
  Wait for updating RTC process finished.
**/
STATIC
VOID
RtcWaitEndOfUpdate (
  VOID
  )
{
  while (RtcRead (R_PCH_RTC_REGA) & B_PCH_RTC_REGA_UIP) {
  }
}

/**
  Get current RTC time

  @param[out] tm          RTC time structure including Second, Minute and Hour.

  @retval EFI_SUCCESS     Operation successfully and RTC_TIME structure contained current time.
**/
EFI_STATUS
RtcGetTime (
  OUT      RTC_TIME        *tm
  )
{
  ASSERT (tm != NULL);
  RtcWaitEndOfUpdate ();
  tm->Second  = BcdToDecimal8 (RtcRead (R_PCH_RTC_SECOND));
  tm->Minute  = BcdToDecimal8 (RtcRead (R_PCH_RTC_MINUTE));
  tm->Hour    = BcdToDecimal8 (RtcRead (R_PCH_RTC_HOUR));
  tm->Date    = BcdToDecimal8 (RtcRead (R_PCH_RTC_DAY_OF_MONTH));
  tm->Month   = BcdToDecimal8 (RtcRead (R_PCH_RTC_MONTH));
  tm->Year    = (UINT16) BcdToDecimal8 (RtcRead (R_PCH_RTC_YEAR)) + 2000;
  return EFI_SUCCESS;
}

/**
  Check if RTC Alarm has been enabled.

  @retval TRUE      RTC Alarm is enabled
  @retval FALSE     RTC Alarm is not enabled
**/
BOOLEAN
RtcIsAlarmEnabled (
  VOID
  )
{
  return (RtcRead (R_PCH_RTC_REGB) & B_PCH_RTC_REGB_AIE) != 0;
}

/**
  Get current RTC Alarm time.

  @param[out] tm                  A structure which will be updated with current RTC Alarm time

  @retval EFI_NOT_STARTED      RTC Alarm has not been enabled yet.
  @retval EFI_SUCCESS          RTC Alarm enabled and RTC_TIME structure contain current Alarm time setting.
**/
EFI_STATUS
RtcGetAlarm (
  OUT      RTC_TIME        *tm
  )
{
  ASSERT (tm != NULL);
  if (!RtcIsAlarmEnabled ()) {
    return EFI_NOT_STARTED;
  }

  RtcWaitEndOfUpdate ();
  tm->Second  = BcdToDecimal8 (RtcRead (R_PCH_RTC_ALARM_SECOND));
  tm->Minute  = BcdToDecimal8 (RtcRead (R_PCH_RTC_ALARM_MINUTE));
  tm->Hour    = BcdToDecimal8 (RtcRead (R_PCH_RTC_ALARM_HOUR));
  tm->Date    = BcdToDecimal8 (RtcRead (R_PCH_RTC_REGD) & 0x3F);
  tm->Month   = 0;
  tm->Year    = 0;
  return EFI_SUCCESS;
}

/**
  Set RTC Alarm with specific time

  @param[in] tm             A time interval structure which will be used to setup an RTC Alarm

  @retval EFI_SUCCESS     RTC Alarm has been enabled with specific time interval
**/
EFI_STATUS
RtcSetAlarm (
  IN      RTC_TIME        *tm
  )
{
  UINT8 RegB;

  ASSERT (tm != NULL);

  RegB = RtcRead (R_PCH_RTC_REGB);

  RtcWaitEndOfUpdate ();

  ///
  /// Inhibit update cycle
  ///
  RtcWrite (R_PCH_RTC_REGB, RegB | B_PCH_RTC_REGB_SET);

  RtcWrite (R_PCH_RTC_ALARM_SECOND, DecimalToBcd8 (tm->Second));
  RtcWrite (R_PCH_RTC_ALARM_MINUTE, DecimalToBcd8 (tm->Minute));
  RtcWrite (R_PCH_RTC_ALARM_HOUR, DecimalToBcd8 (tm->Hour));
  RtcWrite (R_PCH_RTC_REGD, DecimalToBcd8 (tm->Date));

  ///
  /// Allow update cycle and enable wake alarm
  ///
  RegB &= ~B_PCH_RTC_REGB_SET;
  RtcWrite (R_PCH_RTC_REGB, RegB | B_PCH_RTC_REGB_AIE);

  return EFI_SUCCESS;
}

/**
  Check if CRB KSC controller present or not.

  @retval EFI_SUCCESS       - CRB KSC present.
  @retval EFI_DEVICE_ERROR  - CRB KSC not present.
**/
STATIC
EFI_STATUS
CheckKscPresence (
  VOID
  )
{
  if (IoRead8 (KSC_C_PORT) == 0xff) {
    return EFI_DEVICE_ERROR;
  } else {
    return EFI_SUCCESS;
  }
}

/**
  Receives status from Keyboard System Controller.

  @param[in] KscStatus  - Status byte to receive

  @retval EFI_SUCCESS       - Always success
  @retval EFI_DEVICE_ERROR  - KSC not present
**/
STATIC
EFI_STATUS
ReceiveKscStatus (
  UINT8   *KscStatus
  )
{
  ///
  /// Verify if KscLib has been initialized, NOT if EC dose not exist.
  ///
  if (CheckKscPresence () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Read and return the status
  ///
  *KscStatus = IoRead8 (KSC_C_PORT);

  return EFI_SUCCESS;
}

/**
  Sends command to Keyboard System Controller.

  @param[in] Command  - Command byte to send

  @retval EFI_SUCCESS       - Command success
  @retval EFI_DEVICE_ERROR  - Command error
**/
STATIC
EFI_STATUS
SendKscCommand (
  UINT8   Command
  )
{
  UINTN Index;
  UINT8 KscStatus;

  KscStatus = 0;
  ///
  /// Verify if KscLib has been initialized, NOT if EC dose not exist.
  ///
  if (CheckKscPresence () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  Index = 0;

  ///
  /// Wait for KSC to be ready (with a timeout)
  ///
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_IBF) != 0) && (Index < KSC_TIME_OUT)) {
    PchPmTimerStall (KSC_WAIT_PERIOD);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }

  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Send the KSC command
  ///
  IoWrite8 (KSC_C_PORT, Command);

  return EFI_SUCCESS;
}

/**
  Sends data to Keyboard System Controller.

  @param[in] Data  - Data byte to send

  @retval EFI_SUCCESS       - Success
  @retval EFI_DEVICE_ERROR  - Error
**/
STATIC
EFI_STATUS
SendKscData (
  UINT8   Data
  )
{
  UINTN Index;
  UINT8 KscStatus;

  ///
  /// Verify if KscLib has been initialized, NOT if EC dose not exist.
  ///
  if (CheckKscPresence () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  Index = 0;

  ///
  /// Wait for KSC to be ready (with a timeout)
  ///
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_IBF) != 0) && (Index < KSC_TIME_OUT)) {
    PchPmTimerStall (KSC_WAIT_PERIOD);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }

  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Send the data and return
  ///
  IoWrite8 (KSC_D_PORT, Data);
  return EFI_SUCCESS;
}

/**
  Receives data from Keyboard System Controller.

  @param[in] Data  - Data byte received

  @retval EFI_SUCCESS       - Read success
  @retval EFI_DEVICE_ERROR  - Read error
**/
STATIC
EFI_STATUS
ReceiveKscData (
  UINT8   *Data
  )
{
  UINTN Index;
  UINT8 KscStatus;

  ///
  /// Verify if KscLib has been initialized, NOT if EC dose not exist.
  ///
  if (CheckKscPresence () != EFI_SUCCESS) {
    return EFI_DEVICE_ERROR;
  }

  Index = 0;

  ///
  /// Wait for KSC to be ready (with a timeout)
  ///
  ReceiveKscStatus (&KscStatus);
  while (((KscStatus & KSC_S_OBF) == 0) && (Index < KSC_TIME_OUT)) {
    PchPmTimerStall (KSC_WAIT_PERIOD);
    ReceiveKscStatus (&KscStatus);
    Index++;
  }

  if (Index >= KSC_TIME_OUT) {
    return EFI_DEVICE_ERROR;
  }
  ///
  /// Read KSC data and return
  ///
  *Data = IoRead8 (KSC_D_PORT);

  return EFI_SUCCESS;
}

/**
  Enable or disable critical battery wakeup event.

  @param[in] Enabled - Enable or disable Critical Battery wakeup event.
**/
VOID
RapidStartInitializeCriticalBatteryWakeupEvent (
  IN      BOOLEAN                     Enabled
  )
{
  UINT8 Threshold;
  ///
  /// If platform doesn't have KSC controller, skip all KSC commands.
  ///
  if (CheckKscPresence () != EFI_SUCCESS) {
    return ;
  }
  ///
  /// This function can be modified to always enable critical battery event if platform design required.
  /// If platform code already handled critical battery wakeup event, refer to spec and ensure required
  /// Minimal battery capacity threshold implemented for RapidStart Entry transition.
  ///
  if (Enabled == FALSE) {
    ///
    /// Disable critical battery wake event.
    ///
    Threshold = DISABLE_CRITICAL_BATTERY_WAKE;
  } else {
    //
    // Enable critical battery wake event.
    //
//[-start-120928-IB10930000-modify]//
    Threshold = ReadExtCmos8 ( R_XCMOS_INDEX, R_XCMOS_DATA, CritticalBatWakeThres);
//[-end-120928-IB10930000-modify]//
//    Threshold = MINIMAL_CRITICAL_BATTERY_WAKE_THRESHOLD;
  }

  SendKscCommand (KSC_CMD_SET_CRITICAL_BATTERY_WAKE_THRESHOLD);
  SendKscData (Threshold);
}

/**
  Check if current wakeup is because of critical low battery.

  @retval TRUE  - Current wakeup source is critical low battery event.
  @retval FALSE - The wakeup source is not critical low battery event.
**/
BOOLEAN
RapidStartCheckCriticalBatteryWakeupEvent (
  VOID
  )
{
  UINT8 WakeStatus;

  ///
  /// If platform doesn't have KSC controller, return FALSE
  ///
  if (CheckKscPresence () != EFI_SUCCESS) {
    return FALSE;
  }

  SendKscCommand (KSC_CMD_GET_WAKE_STATUS);
  ///
  /// if get wake status failed, return false
  ///
  if (ReceiveKscData (&WakeStatus) != EFI_SUCCESS) {
    return FALSE;
  }

  if (WakeStatus & KSC_WAKE_STATUS_CRITICAL_BATTERY) {
    return TRUE;
  } else {
    return FALSE;
  }
}

/**
  Clear all wakeup status.
**/
VOID
RapidStartClearAllKscWakeStatus (
  VOID
  )
{
  ///
  /// If platform doesn't have KSC controller, skip all KSC commands.
  ///
  if (CheckKscPresence () != EFI_SUCCESS) {
    return ;
  }

  SendKscCommand (KSC_CMD_CLEAR_WAKE_STATUS);
}

#ifdef RAPID_START_WHOLE_MEMORY_CHECK
/**
  Calculate CRC32 value for inputed data

  @param[in] Data             - data to calc on CRC
  @param[in] DataSize         - data size
  @param[in] Crc32            - crc value.
  @param[in] RapidStartData   - A data buffer stored RapidStart internal non-volatile information.
  @param[in] CrcTable         - The base CRC table

  @retval EFI_INVALID_PARAMETER  inputed parameters are invalid
  @retval EFI_SUCCESS            Process successfully and CRC value has been passed by Crc32 parameter.
**/
EFI_STATUS
RapidStartCalculateCrc32 (
  IN  UINT8                        *Data,
  IN  UINT32                       DataSize,
  IN  UINT32                       *Crc32,
  IN  RAPID_START_PERSISTENT_DATA  *RapidStartData,
  IN  UINT32                       *CrcTable
  )
{
  UINT32  i;
  UINT32  crc;

  crc = (UINT32) -1;

  if (!DataSize || !Crc32) {
    return EFI_INVALID_PARAMETER;
  }
  ///
  /// Calculate the CRC
  ///
  for (i = 0; i < DataSize; i++) {
    if (((Data + i) >= (UINT8 *) (UINTN) RapidStartData->AcpiReservedMemoryBase) &&
        ((Data + i) < (UINT8 *) (UINTN) (RapidStartData->AcpiReservedMemoryBase + RapidStartData->AcpiReservedMemorySize))
        ) {
      continue;
    }

    if (((Data + i) < (UINT8 *) (UINTN) RapidStartData->RapidStartMem) ||
        ((Data + i) >= (UINT8 *) (UINTN) (RapidStartData->RapidStartMem + RapidStartData->RapidStartMemSize))
        ) {
      crc = (crc >> 8) ^ CrcTable[(UINT8) crc ^ ((UINT8 *) Data)[i]];
    }
  }

  *Crc32 = ~crc;

  return EFI_SUCCESS;
}

/**
  Log CRC32 mismatched address and length

  @param[in] MismatchedBaseAddress      - Memory address caused mismatched CRC32
  @param[in] MismatchedDataSize         - memory length for calculating CRC32

  @retval EFI_SUCCESS            Process successfully
**/
EFI_STATUS
LogCrc32MismatchedAddress (
  UINTN  MismatchedBaseAddress,
  UINT32 MismatchedDataSize
  )
{
  return EFI_SUCCESS;
}

/**
  Log or compare CRC32 value for specific memory range.

  @param[in] IsComparingCrc32 - FALSE to save CRC32 value into buffer. TRUE to compare CRC32 value with pre-saved value in buffer.
  @param[in] BaseAddress      - Base memory address for logging or checking CRC32
  @param[in] EndAddress       - End memory address for logging or checking CRC32
  @param[in] RapidStartData   - A data buffer stored RapidStart internal non-volatile information.
**/
VOID
SaveOrCompareCrc32 (
  IN  BOOLEAN                      IsComparingCrc32,
  IN  UINTN                        BaseAddress,
  IN  UINTN                        EndAddress,
  IN  RAPID_START_PERSISTENT_DATA  *RapidStartData
  )
{
  UINT32          *Crc32Record;
  UINT32          *Crc32RecordPointer;
  UINT32          Crc32;
  UINT32          DataSize;
  volatile UINTN  Crc32MismatchedAddress;
  volatile UINT32 Crc32MismatchedSize;
  UINT32          Crc32RecordSizeInDword;
  UINT32          CrcTable[256];
  UINT32          i;
  UINT32          j;

  ///
  /// init the CRC base table
  ///
  for (i = 0; i < 256; i++) {
    CrcTable[i] = i;
    for (j = 8; j > 0; j--) {
      CrcTable[i] = (CrcTable[i] & 1) ? (CrcTable[i] >> 1) ^ 0xedb88320 : CrcTable[i] >> 1;
    }
  }

  Crc32Record             = RAPID_START_CRC32_RECORD_PTR (RapidStartData);
  Crc32RecordPointer      = Crc32Record + (UINT32) DivU64x32 (BaseAddress, MEMORY_CRC32_GRANULARITY);
  Crc32RecordSizeInDword  = (UINT32) RShiftU64 (RapidStartData->Crc32RecordSize, 2);
  while (BaseAddress < EndAddress) {
    ASSERT (Crc32RecordPointer < (Crc32Record + Crc32RecordSizeInDword));
    if ((EndAddress - BaseAddress) >= MEMORY_CRC32_GRANULARITY) {
      DataSize = MEMORY_CRC32_GRANULARITY;
    } else {
      DataSize = (UINT32) (EndAddress - BaseAddress);
    }
    ///
    /// indicate which address is handling
    ///
    IoWrite16 (0x80, (UINT16) DivU64x32 (BaseAddress, MEMORY_CRC32_GRANULARITY));
    RapidStartCalculateCrc32 ((UINT8 *) BaseAddress, DataSize, &Crc32, RapidStartData, CrcTable);
    if (IsComparingCrc32) {
      ///
      /// Send debug message and log error when mismatch data is found
      ///
      if (Crc32 != *(Crc32RecordPointer)) {
        if (sizeof (BaseAddress) == 4) {
          DEBUG ((EFI_D_ERROR, "CRC_CHECK: Address=%08X, ", BaseAddress));
        } else {
          DEBUG ((EFI_D_ERROR, "CRC_CHECK: Address=%010lX, ", BaseAddress));
        }

        DEBUG ((EFI_D_ERROR, "Size=%08X, ", DataSize));
        DEBUG ((EFI_D_ERROR, "current CRC32=%08X, Expected CRC32=%08X, ", Crc32, *(Crc32RecordPointer)));
        DEBUG ((EFI_D_ERROR, "Crc32Pointer=%08X\n", Crc32RecordPointer));
        DEBUG ((EFI_D_ERROR, "\nFound mismatched CRC32!!\n\n"));
        Crc32MismatchedAddress  = BaseAddress;
        Crc32MismatchedSize     = DataSize;
        LogCrc32MismatchedAddress (Crc32MismatchedAddress, Crc32MismatchedSize);
      }
    } else {
      ///
      /// Save CRC result for comparison during RapidStart exit
      ///
      *(Crc32RecordPointer) = Crc32;
    }

    BaseAddress += MEMORY_CRC32_GRANULARITY;
    Crc32RecordPointer++;
  }
}

#endif

/**
  Gets RapidStart non-volatile flag.

  @param[out] Value         - RapidStart non-volatile flag

  @retval EFI_SUCCESS   - Return the RapidStart flag in Value argument.
**/
EFI_STATUS
RapidStartGetFlag (
  OUT     UINT8                   *Value
  )
{
  *Value = RtcRead (FFS_NV_FLAG_REG);
  return EFI_SUCCESS;
}

/**
  Sets RapidStart non-volatile flag.

  @param[in] Value         - RapidStart flag state

  @retval EFI_SUCCESS   - Flag is set.
**/
EFI_STATUS
RapidStartSetFlag (
  IN      UINT8                     Value
  )
{
  RtcWrite (FFS_NV_FLAG_REG, Value);
  return EFI_SUCCESS;
}

/**
  Retrieves non-volatile RapidStart settings.

  @param[out] Value         - RapidStart non-volatile settings

  @retval EFI_SUCCESS
**/
EFI_STATUS
RapidStartGetConfig (
  OUT     UINT8                   *Value
  )
{
  *Value = RtcRead (FFS_NV_CONFIG_REG);
  return EFI_SUCCESS;
}

/**
  Sets non-volatile RapidStart settings.

  @param[in] Value         - RapidStart non-volatile settings

  @retval EFI_SUCCESS
**/
EFI_STATUS
RapidStartSetConfig (
  IN      UINT8                     Value
  )
{
  RtcWrite (FFS_NV_CONFIG_REG, Value);
  return EFI_SUCCESS;
}

/**
  This callback is executed at the end of RapidStart Entry events initialization. (Inside sleep SMI handler)

  @param[in] TransitionStatus - Status of initialization for RapidStart Entry events.
**/
VOID
AfterInitializingEntryEvent (
  IN      EFI_STATUS                TransitionStatus
  )
{
}

/**
  Check whether RapidStart Resume.

  @retval TRUE   - Rapid Start Entry flow has completed successfully
**/
BOOLEAN
RapidStartResumeCheck (
  VOID
)
{
  EFI_STATUS                        Status;
  BOOLEAN                           RapidStartFlag;

  Status = RapidStartGetFlag (&RapidStartFlag);
  if ( !EFI_ERROR (Status) && ((RapidStartFlag & RAPID_START_FLAG_ENTRY_DONE) != 0)) {
    return TRUE;
  }

  return FALSE;
}
