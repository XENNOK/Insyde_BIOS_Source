/** @file
 H2O IPMI SEL Info module implement code.

 This c file contains common functions for H2O IPMI SEL Info module internal use.

;******************************************************************************
;* Copyright (c) 2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************

*/


#include <IpmiSelInfoCommon.h>


//
// Intelligent Platform Management Interface Specification  Second Generation v2.0
// Document Revision 1.0  February 12, 2004, June 12, 2009 Markup
// Table 42-2, Generic Event/Reading Type Codes
//
SENSOR_EVENT_TYPE_ENTRY mGenericEventTypes[] = {
  //
  // Threshold Based States
  //
  {0x01, 0x00, 0xFF, SENSOR_EVENT_CLASS_THRESHOLD, L"Threshold", L"Lower Non-critical going low"},
  {0x01, 0x01, 0xFF, SENSOR_EVENT_CLASS_THRESHOLD, L"Threshold", L"Lower Non-critical going high"},
  {0x01, 0x02, 0xFF, SENSOR_EVENT_CLASS_THRESHOLD, L"Threshold", L"Lower Critical going low"},
  {0x01, 0x03, 0xFF, SENSOR_EVENT_CLASS_THRESHOLD, L"Threshold", L"Lower Critical going high"},
  {0x01, 0x04, 0xFF, SENSOR_EVENT_CLASS_THRESHOLD, L"Threshold", L"Lower Non-recoverable going low"},
  {0x01, 0x05, 0xFF, SENSOR_EVENT_CLASS_THRESHOLD, L"Threshold", L"Lower Non-recoverable going high"},
  {0x01, 0x06, 0xFF, SENSOR_EVENT_CLASS_THRESHOLD, L"Threshold", L"Upper Non-critical going low"},
  {0x01, 0x07, 0xFF, SENSOR_EVENT_CLASS_THRESHOLD, L"Threshold", L"Upper Non-critical going high"},
  {0x01, 0x08, 0xFF, SENSOR_EVENT_CLASS_THRESHOLD, L"Threshold", L"Upper Critical going low"},
  {0x01, 0x09, 0xFF, SENSOR_EVENT_CLASS_THRESHOLD, L"Threshold", L"Upper Critical going high"},
  {0x01, 0x0A, 0xFF, SENSOR_EVENT_CLASS_THRESHOLD, L"Threshold", L"Upper Non-recoverable going low"},
  {0x01, 0x0B, 0xFF, SENSOR_EVENT_CLASS_THRESHOLD, L"Threshold", L"Upper Non-recoverable going high"},
  //
  // DMI-based L"usage state" States
  //
  {0x02, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Usage State", L"Transition to Idle"},
  {0x02, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Usage State", L"Transition to Active"},
  {0x02, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Usage State", L"Transition to Busy"},
  //
  // Digital-Discrete Event States
  //
  {0x03, 0x00, 0xFF, SENSOR_EVENT_CLASS_DIGITAL, L"Digital State", L"State Deasserted"},
  {0x03, 0x01, 0xFF, SENSOR_EVENT_CLASS_DIGITAL, L"Digital State", L"State Asserted"},
  {0x04, 0x00, 0xFF, SENSOR_EVENT_CLASS_DIGITAL, L"Digital State", L"Predictive Failure Deasserted"},
  {0x04, 0x01, 0xFF, SENSOR_EVENT_CLASS_DIGITAL, L"Digital State", L"Predictive Failure Asserted"},
  {0x05, 0x00, 0xFF, SENSOR_EVENT_CLASS_DIGITAL, L"Digital State", L"Limit Not Exceeded"},
  {0x05, 0x01, 0xFF, SENSOR_EVENT_CLASS_DIGITAL, L"Digital State", L"Limit Exceeded"},
  {0x06, 0x00, 0xFF, SENSOR_EVENT_CLASS_DIGITAL, L"Digital State", L"Performance Met"},
  {0x06, 0x01, 0xFF, SENSOR_EVENT_CLASS_DIGITAL, L"Digital State", L"Performance Lags"},
  //
  // Severity Event States
  //
  {0x07, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Severity State", L"Transition to OK"},
  {0x07, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Severity State", L"Transition to Non-critical from OK"},
  {0x07, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Severity State", L"Transition to Critical from less severe"},
  {0x07, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Severity State", L"Transition to Non-recoverable from less severe"},
  {0x07, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Severity State", L"Transition to Non-critical from more severe"},
  {0x07, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Severity State", L"Transition to Critical from Non-recoverable"},
  {0x07, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Severity State", L"Transition to Non-recoverable"},
  {0x07, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Severity State", L"Monitor"},
  {0x07, 0x08, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Severity State", L"Informational"},
  //
  // Availability Status States
  //
  {0x08, 0x00, 0xFF, SENSOR_EVENT_CLASS_DIGITAL,  L"Availability State", L"Device Absent"},
  {0x08, 0x01, 0xFF, SENSOR_EVENT_CLASS_DIGITAL,  L"Availability State", L"Device Present"},
  {0x09, 0x00, 0xFF, SENSOR_EVENT_CLASS_DIGITAL,  L"Availability State", L"Device Disabled"},
  {0x09, 0x01, 0xFF, SENSOR_EVENT_CLASS_DIGITAL,  L"Availability State", L"Device Enabled"},
  {0x0A, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Availability State", L"Transition to Running"},
  {0x0A, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Availability State", L"Transition to In Test"},
  {0x0A, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Availability State", L"Transition to Power Off"},
  {0x0A, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Availability State", L"Transition to On Line"},
  {0x0A, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Availability State", L"Transition to Off Line"},
  {0x0A, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Availability State", L"Transition to Off Duty"},
  {0x0A, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Availability State", L"Transition to Degraded"},
  {0x0A, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Availability State", L"Transition to Power Save"},
  {0x0A, 0x08, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Availability State", L"Install Error"},
  //
  // Redundancy States
  //
  {0x0B, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Redundancy State", L"Fully Redundant"},
  {0x0B, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Redundancy State", L"Redundancy Lost"},
  {0x0B, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Redundancy State", L"Redundancy Degraded"},
  {0x0B, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Redundancy State", L"Non-Redundant: Sufficient from Redundant"},
  {0x0B, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Redundancy State", L"Non-Redundant: Sufficient from Insufficient"},
  {0x0B, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Redundancy State", L"Non-Redundant: Insufficient Resources"},
  {0x0B, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Redundancy State", L"Redundancy Degraded from Fully Redundant"},
  {0x0B, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Redundancy State", L"Redundancy Degraded from Non-Redundant"},
  //
  // ACPI Device Power States
  //
  {0x0C, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"ACPI Device Power State", L"D0 Power State"},
  {0x0C, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"ACPI Device Power State", L"D1 Power State"},
  {0x0C, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"ACPI Device Power State", L"D2 Power State"},
  {0x0C, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"ACPI Device Power State", L"D3 Power State"},
  //
  // End
  //
  {0x00, 0x00, 0xFF, 0x00, NULL, NULL},
};


//
// Intelligent Platform Management Interface Specification  Second Generation v2.0
// Document Revision 1.0  February 12, 2004, June 12, 2009 Markup 
// Table 42-3, Sensor Type Codes
//
SENSOR_EVENT_TYPE_ENTRY mSensorSpecificTypes[] = {
  {0x00, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Reserved",    NULL},
  {0x01, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Temperature", NULL},
  {0x02, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Voltage",     NULL},
  {0x03, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Current",     NULL},
  {0x04, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Fan",         NULL},

  {0x05, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Physical Security", L"General Chassis intrusion"},
  {0x05, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Physical Security", L"Drive Bay intrusion"},
  {0x05, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Physical Security", L"I/O Card area intrusion"},
  {0x05, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Physical Security", L"Processor area intrusion"},
  {0x05, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Physical Security", L"System unplugged from LAN"},
  {0x05, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Physical Security", L"Unauthorized dock"},
  {0x05, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Physical Security", L"FAN area intrusion"},

  {0x06, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Platform Security", L"Front Panel Lockout violation attempted"},
  {0x06, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Platform Security", L"Pre-boot password violation - user password"},
  {0x06, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Platform Security", L"Pre-boot password violation - setup password"},
  {0x06, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Platform Security", L"Pre-boot password violation - network boot password"},
  {0x06, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Platform Security", L"Other pre-boot password violation"},
  {0x06, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Platform Security", L"Out-of-band access password violation"},

  {0x07, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"IERR"},
  {0x07, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"Thermal Trip"},
  {0x07, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"FRB1/BIST failure"},
  {0x07, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"FRB2/Hang in POST failure"},
  {0x07, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"FRB3/Processor startup/init failure"},
  {0x07, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"Configuration Error"},
  {0x07, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"SM BIOS Uncorrectable CPU-complex Error"},
  {0x07, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"Presence detected"},
  {0x07, 0x08, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"Disabled"},
  {0x07, 0x09, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"Terminator presence detected"},
  {0x07, 0x0A, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"Throttled"},
  {0x07, 0x0B, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"Machine Check Exception"},
  {0x07, 0x0C, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Processor", L"Correctable Machine Check Error"},

  {0x08, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Supply", L"Presence detected"},
  {0x08, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Supply", L"Failure detected"},
  {0x08, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Supply", L"Predictive failure"},
  {0x08, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Supply", L"Power Supply AC lost"},
  {0x08, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Supply", L"AC lost or out-of-range"},
  {0x08, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Supply", L"AC out-of-range, but present"},
  {0x08, 0x06, 0x00, SENSOR_EVENT_CLASS_DISCRETE, L"Power Supply", L"Config Error: Vendor Mismatch"},
  {0x08, 0x06, 0x01, SENSOR_EVENT_CLASS_DISCRETE, L"Power Supply", L"Config Error: Revision Mismatch"},
  {0x08, 0x06, 0x02, SENSOR_EVENT_CLASS_DISCRETE, L"Power Supply", L"Config Error: Processor Missing"},
  {0x08, 0x06, 0x03, SENSOR_EVENT_CLASS_DISCRETE, L"Power Supply", L"Config Error: Power Supply Rating Mismatch"},
  {0x08, 0x06, 0x04, SENSOR_EVENT_CLASS_DISCRETE, L"Power Supply", L"Config Error: Voltage Rating Mismatch"},
  {0x08, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Supply", L"Config Error"},

  {0x09, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Unit", L"Power off/down"},
  {0x09, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Unit", L"Power cycle"},
  {0x09, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Unit", L"240VA power down"},
  {0x09, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Unit", L"Interlock power down"},
  {0x09, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Unit", L"AC lost/Power Input Lost"},
  {0x09, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Unit", L"Soft-power control failure"},
  {0x09, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Unit", L"Failure detected"},
  {0x09, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Power Unit", L"Predictive failure"},

  {0x0A, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Cooling Device",           NULL},
  {0x0B, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Other Units-based Sensor", NULL},

  {0x0C, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Memory", L"Correctable ECC"},
  {0x0C, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Memory", L"Uncorrectable ECC"},
  {0x0C, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Memory", L"Parity"},
  {0x0C, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Memory", L"Memory Scrub Failed"},
  {0x0C, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Memory", L"Memory Device Disabled"},
  {0x0C, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Memory", L"Correctable ECC logging limit reached"},
  {0x0C, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Memory", L"Presence Detected"},
  {0x0C, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Memory", L"Configuration Error"},
  {0x0C, 0x08, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Memory", L"Spare"},
  {0x0C, 0x09, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Memory", L"Throttled"},
  {0x0C, 0x0A, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Memory", L"Critical Overtemperature"},

  {0x0d, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Drive Slot", L"Drive Present"},
  {0x0d, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Drive Slot", L"Drive Fault"},
  {0x0d, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Drive Slot", L"Predictive Failure"},
  {0x0d, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Drive Slot", L"Hot Spare"},
  {0x0d, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Drive Slot", L"Parity Check In Progress"},
  {0x0d, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Drive Slot", L"In Critical Array"},
  {0x0d, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Drive Slot", L"In Failed Array"},
  {0x0d, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Drive Slot", L"Rebuild In Progress"},
  {0x0d, 0x08, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Drive Slot", L"Rebuild Aborted"},

  {0x0e, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"POST Memory Resize", NULL},

  {0x0f, 0x00, 0x00, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"Unspecified"},
  {0x0f, 0x00, 0x01, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"No system memory installed"},
  {0x0f, 0x00, 0x02, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"No usable system memory"},
  {0x0f, 0x00, 0x03, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"Unrecoverable IDE device failure"},
  {0x0f, 0x00, 0x04, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"Unrecoverable system-board failure"},
  {0x0f, 0x00, 0x05, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"Unrecoverable diskette failure"},
  {0x0f, 0x00, 0x06, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"Unrecoverable hard-disk controller failure"},
  {0x0f, 0x00, 0x07, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"Unrecoverable PS/2 or USB keyboard failure"},
  {0x0f, 0x00, 0x08, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"Removable boot media not found"},
  {0x0f, 0x00, 0x09, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"Unrecoverable video controller failure"},
  {0x0f, 0x00, 0x0A, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"No video device selected"},
  {0x0f, 0x00, 0x0B, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"BIOS corruption detected"},
  {0x0f, 0x00, 0x0C, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"CPU voltage mismatch"},
  {0x0f, 0x00, 0x0d, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"CPU speed mismatch failure"},
  {0x0f, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Error", L"Unknown Error"},

  {0x0f, 0x01, 0x00, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Unspecified"},
  {0x0f, 0x01, 0x01, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Memory initialization"},
  {0x0f, 0x01, 0x02, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Hard-disk initialization"},
  {0x0f, 0x01, 0x03, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Secondary CPU Initialization"},
  {0x0f, 0x01, 0x04, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"User authentication"},
  {0x0f, 0x01, 0x05, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"User-initiated system setup"},
  {0x0f, 0x01, 0x06, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"USB resource configuration"},
  {0x0f, 0x01, 0x07, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"PCI resource configuration"},
  {0x0f, 0x01, 0x08, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Option ROM initialization"},
  {0x0f, 0x01, 0x09, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Video initialization"},
  {0x0f, 0x01, 0x0A, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Cache initialization"},
  {0x0f, 0x01, 0x0B, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"SMBus initialization"},
  {0x0f, 0x01, 0x0C, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Keyboard controller initialization"},
  {0x0f, 0x01, 0x0d, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Management controller initialization"},
  {0x0f, 0x01, 0x0e, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Docking station attachment"},
  {0x0f, 0x01, 0x0f, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Enabling docking station"},
  {0x0f, 0x01, 0x10, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Docking station ejection"},
  {0x0f, 0x01, 0x11, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Disabling docking station"},
  {0x0f, 0x01, 0x12, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Calling operating system wake-up vector"},
  {0x0f, 0x01, 0x13, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"System boot initiated"},
  {0x0f, 0x01, 0x14, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Motherboard initialization"},
  {0x0f, 0x01, 0x15, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"reserved"},
  {0x0f, 0x01, 0x16, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Floppy initialization"},
  {0x0f, 0x01, 0x17, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Keyboard test"},
  {0x0f, 0x01, 0x18, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Pointing device test"},
  {0x0f, 0x01, 0x19, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Primary CPU initialization"},
  {0x0f, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Hang", L"Unknown Hang"},

  {0x0f, 0x02, 0x00, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Unspecified"},
  {0x0f, 0x02, 0x01, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Memory initialization"},
  {0x0f, 0x02, 0x02, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Hard-disk initialization"},
  {0x0f, 0x02, 0x03, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Secondary CPU Initialization"},
  {0x0f, 0x02, 0x04, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"User authentication"},
  {0x0f, 0x02, 0x05, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"User-initiated system setup"},
  {0x0f, 0x02, 0x06, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"USB resource configuration"},
  {0x0f, 0x02, 0x07, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"PCI resource configuration"},
  {0x0f, 0x02, 0x08, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Option ROM initialization"},
  {0x0f, 0x02, 0x09, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Video initialization"},
  {0x0f, 0x02, 0x0A, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Cache initialization"},
  {0x0f, 0x02, 0x0B, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"SMBus initialization"},
  {0x0f, 0x02, 0x0C, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Keyboard controller initialization"},
  {0x0f, 0x02, 0x0d, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Management controller initialization"},
  {0x0f, 0x02, 0x0e, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Docking station attachment"},
  {0x0f, 0x02, 0x0f, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Enabling docking station"},
  {0x0f, 0x02, 0x10, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Docking station ejection"},
  {0x0f, 0x02, 0x11, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Disabling docking station"},
  {0x0f, 0x02, 0x12, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Calling operating system wake-up vector"},
  {0x0f, 0x02, 0x13, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"System boot initiated"},
  {0x0f, 0x02, 0x14, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Motherboard initialization"},
  {0x0f, 0x02, 0x15, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"reserved"},
  {0x0f, 0x02, 0x16, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Floppy initialization"},
  {0x0f, 0x02, 0x17, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Keyboard test"},
  {0x0f, 0x02, 0x18, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Pointing device test"},
  {0x0f, 0x02, 0x19, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Primary CPU initialization"},
  {0x0f, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Firmware Progress", L"Unknown Progress"},

  {0x10, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Event Logging Disabled", L"Correctable memory error logging disabled"},
  {0x10, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Event Logging Disabled", L"Event logging disabled"},
  {0x10, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Event Logging Disabled", L"Log area reset/cleared"},
  {0x10, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Event Logging Disabled", L"All event logging disabled"},
  {0x10, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Event Logging Disabled", L"Log full"},
  {0x10, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Event Logging Disabled", L"Log almost full"},
  {0x10, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Event Logging Disabled", L"Correctable machine check error logging disabled"},

  {0x11, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 1", L"BIOS Reset"},
  {0x11, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 1", L"OS Reset"},
  {0x11, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 1", L"OS Shut Down"},
  {0x11, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 1", L"OS Power Down"},
  {0x11, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 1", L"OS Power Cycle"},
  {0x11, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 1", L"OS NMI/Diag Interrupt"},
  {0x11, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 1", L"OS Expired"},
  {0x11, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 1", L"OS pre-timeout Interrupt"},

  {0x12, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Event", L"System Reconfigured"},
  {0x12, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Event", L"OEM System boot event"},
  {0x12, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Event", L"Undetermined system hardware failure"},
  {0x12, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Event", L"Entry added to auxiliary log"},
  {0x12, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Event", L"PEF Action"},
  {0x12, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Event", L"Timestamp Clock Sync"},

  {0x13, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Critical Interrupt", L"NMI/Diag Interrupt"},
  {0x13, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Critical Interrupt", L"Bus Timeout"},
  {0x13, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Critical Interrupt", L"I/O Channel check NMI"},
  {0x13, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Critical Interrupt", L"Software NMI"},
  {0x13, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Critical Interrupt", L"PCI PERR"},
  {0x13, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Critical Interrupt", L"PCI SERR"},
  {0x13, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Critical Interrupt", L"EISA failsafe timeout"},
  {0x13, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Critical Interrupt", L"Bus Correctable error"},
  {0x13, 0x08, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Critical Interrupt", L"Bus Uncorrectable error"},
  {0x13, 0x09, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Critical Interrupt", L"Fatal NMI"},
  {0x13, 0x0A, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Critical Interrupt", L"Bus Fatal Error"},
  {0x13, 0x0A, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Critical Interrupt", L"Bus Degraded"},

  {0x14, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Button", L"Power Button pressed"},
  {0x14, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Button", L"Sleep Button pressed"},
  {0x14, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Button", L"Reset Button pressed"},
  {0x14, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Button", L"FRU Latch"},
  {0x14, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Button", L"FRU Service"},

  {0x15, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Module/Board",                NULL},
  {0x16, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Microcontroller/Coprocessor", NULL},
  {0x17, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Add-in Card",                 NULL},
  {0x18, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Chassis",                     NULL},
  {0x19, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Chip Set",                    NULL},
  {0x1A, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Other FRU",                   NULL},

  {0x1B, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Cable/Interconnect", L"Connected"},
  {0x1B, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Cable/Interconnect", L"Config Error"},

  {0x1C, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Terminator", NULL},

  {0x1D, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Boot Initiated", L"Initiated by power up"},
  {0x1D, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Boot Initiated", L"Initiated by hard reset"},
  {0x1D, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Boot Initiated", L"Initiated by warm reset"},
  {0x1D, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Boot Initiated", L"User requested PXE boot"},
  {0x1D, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Boot Initiated", L"Automatic boot to diagnostic"},
  {0x1D, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Boot Initiated", L"OS initiated hard reset"},
  {0x1D, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Boot Initiated", L"OS initiated warm reset"},
  {0x1D, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System Boot Initiated", L"System Restart"},

  {0x1E, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Boot Error", L"No bootable media"},
  {0x1E, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Boot Error", L"Non-bootable disk in drive"},
  {0x1E, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Boot Error", L"PXE server not found"},
  {0x1E, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Boot Error", L"Invalid boot sector"},
  {0x1E, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Boot Error", L"Timeout waiting for selection"},

  {0x1F, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Boot", L"A: boot completed"},
  {0x1F, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Boot", L"C: boot completed"},
  {0x1F, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Boot", L"PXE boot completed"},
  {0x1F, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Boot", L"Diagnostic boot completed"},
  {0x1F, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Boot", L"CD-ROM boot completed"},
  {0x1F, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Boot", L"ROM boot completed"},
  {0x1F, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Boot", L"boot completed - device not specified"},

  {0x20, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Stop/Shutdown", L"Error during system startup"},
  {0x20, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Stop/Shutdown", L"Run-time critical stop"},
  {0x20, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Stop/Shutdown", L"OS graceful stop"},
  {0x20, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Stop/Shutdown", L"OS graceful shutdown"},
  {0x20, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Stop/Shutdown", L"PEF initiated soft shutdown"},
  {0x20, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"OS Stop/Shutdown", L"Agent not responding"},

  {0x21, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Slot/Connector", L"Fault Status"},
  {0x21, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Slot/Connector", L"Identify Status"},
  {0x21, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Slot/Connector", L"Device Installed"},
  {0x21, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Slot/Connector", L"Ready for Device Installation"},
  {0x21, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Slot/Connector", L"Ready for Device Removal"},
  {0x21, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Slot/Connector", L"Slot Power is Off"},
  {0x21, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Slot/Connector", L"Device Removal Request"},
  {0x21, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Slot/Connector", L"Interlock"},
  {0x21, 0x08, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Slot/Connector", L"Slot is Disabled"},
  {0x21, 0x09, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Slot/Connector", L"Spare Device"},

  {0x22, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"S0/G0: working"},
  {0x22, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"S1: sleeping with system hw & processor context maintained"},
  {0x22, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"S2: sleeping, processor context lost"},
  {0x22, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"S3: sleeping, processor & hw context lost, memory retained"},
  {0x22, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"S4: non-volatile sleep/suspend-to-disk"},
  {0x22, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"S5/G2: soft-off"},
  {0x22, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"S4/S5: soft-off"},
  {0x22, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"G3: mechanical off"},
  {0x22, 0x08, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"Sleeping in S1/S2/S3 state"},
  {0x22, 0x09, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"G1: sleeping"},
  {0x22, 0x0A, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"S5: entered by override"},
  {0x22, 0x0B, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"Legacy ON state"},
  {0x22, 0x0C, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"Legacy OFF state"},
  {0x22, 0x0e, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"System ACPI Power State", L"Unknown"},

  {0x23, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 2", L"Timer expired"},
  {0x23, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 2", L"Hard reset"},
  {0x23, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 2", L"Power down"},
  {0x23, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 2", L"Power cycle"},
  {0x23, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 2", L"reserved"},
  {0x23, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 2", L"reserved"},
  {0x23, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 2", L"reserved"},
  {0x23, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 2", L"reserved"},
  {0x23, 0x08, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Watchdog 2", L"Timer interrupt"},

  {0x24, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Platform Alert", L"Platform generated page"},
  {0x24, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Platform Alert", L"Platform generated LAN alert"},
  {0x24, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Platform Alert", L"Platform Event Trap generated"},
  {0x24, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Platform Alert", L"Platform generated SNMP trap, OEM format"},

  {0x25, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Entity Presence", L"Present"},
  {0x25, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Entity Presence", L"Absent"},
  {0x25, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Entity Presence", L"Disabled"},

  {0x26, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Monitor ASIC/IC", NULL},

  {0x27, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"LAN", L"Heartbeat Lost"},
  {0x27, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"LAN", L"Heartbeat"},

  {0x28, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Management Subsystem Health", L"Sensor access degraded or unavailable"},
  {0x28, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Management Subsystem Health", L"Controller access degraded or unavailable"},
  {0x28, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Management Subsystem Health", L"Management controller off-line"},
  {0x28, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Management Subsystem Health", L"Management controller unavailable"},
  {0x28, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Management Subsystem Health", L"Sensor failure"},
  {0x28, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Management Subsystem Health", L"FRU failure"},

  {0x29, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Battery", L"Low"},
  {0x29, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Battery", L"Failed"},
  {0x29, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Battery", L"Presence Detected"},

  {0x2B, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Hardware change detected"},
  {0x2B, 0x01, 0x00, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected"},
  {0x2B, 0x01, 0x01, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Mngmt Ctrl Dev Id"},
  {0x2B, 0x01, 0x02, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Mngmt Ctrl Firm Rev"},
  {0x2B, 0x01, 0x03, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Mngmt Ctrl Dev Rev"},
  {0x2B, 0x01, 0x04, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Mngmt Ctrl Manuf Id"},
  {0x2B, 0x01, 0x05, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Mngmt Ctrl IPMI Vers"},
  {0x2B, 0x01, 0x06, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Mngmt Ctrl Aux Firm Id"},
  {0x2B, 0x01, 0x07, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Mngmt Ctrl Firm Boot Block"},
  {0x2B, 0x01, 0x08, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Mngmt Ctrl Other"},
  {0x2B, 0x01, 0x09, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, BIOS/EFI change"},
  {0x2B, 0x01, 0x0A, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, SMBIOS change"},
  {0x2B, 0x01, 0x0B, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, O/S change"},
  {0x2B, 0x01, 0x0C, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, O/S loader change"},
  {0x2B, 0x01, 0x0D, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Service Diag change"},
  {0x2B, 0x01, 0x0E, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Mngmt SW agent change"},
  {0x2B, 0x01, 0x0F, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Mngmt SW App change"},
  {0x2B, 0x01, 0x10, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Mngmt SW Middle"},
  {0x2B, 0x01, 0x11, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, Prog HW Change (FPGA)"},
  {0x2B, 0x01, 0x12, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, board/FRU module change"},
  {0x2B, 0x01, 0x13, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, board/FRU component change"},
  {0x2B, 0x01, 0x14, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, board/FRU replace equ ver"},
  {0x2B, 0x01, 0x15, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, board/FRU replace new ver"},
  {0x2B, 0x01, 0x16, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, board/FRU replace old ver"},
  {0x2B, 0x01, 0x17, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change detected, board/FRU HW conf change"},
  {0x2B, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Hardware incompatibility detected"},
  {0x2B, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software incompatibility detected"},
  {0x2B, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Invalid or unsupported hardware version"},
  {0x2B, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Invalid or unsupported firmware or software version"},
  {0x2B, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Hardware change success"},
  {0x2B, 0x07, 0x00, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success"},
  {0x2B, 0x07, 0x01, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Mngmt Ctrl Dev Id"},
  {0x2B, 0x07, 0x02, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Mngmt Ctrl Firm Rev"},
  {0x2B, 0x07, 0x03, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Mngmt Ctrl Dev Rev"},
  {0x2B, 0x07, 0x04, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Mngmt Ctrl Manuf Id"},
  {0x2B, 0x07, 0x05, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Mngmt Ctrl IPMI Vers"},
  {0x2B, 0x07, 0x06, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Mngmt Ctrl Aux Firm Id"},
  {0x2B, 0x07, 0x07, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Mngmt Ctrl Firm Boot Block"},
  {0x2B, 0x07, 0x08, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Mngmt Ctrl Other"},
  {0x2B, 0x07, 0x09, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, BIOS/EFI change"},
  {0x2B, 0x07, 0x0A, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, SMBIOS change"},
  {0x2B, 0x07, 0x0B, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, O/S change"},
  {0x2B, 0x07, 0x0C, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, O/S loader change"},
  {0x2B, 0x07, 0x0D, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Service Diag change"},
  {0x2B, 0x07, 0x0E, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Mngmt SW agent change"},
  {0x2B, 0x07, 0x0F, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Mngmt SW App change"},
  {0x2B, 0x07, 0x10, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Mngmt SW Middle"},
  {0x2B, 0x07, 0x11, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, Prog HW Change (FPGA)"},
  {0x2B, 0x07, 0x12, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, board/FRU module change"},
  {0x2B, 0x07, 0x13, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, board/FRU component change"},
  {0x2B, 0x07, 0x14, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, board/FRU replace equ ver"},
  {0x2B, 0x07, 0x15, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, board/FRU replace new ver"},
  {0x2B, 0x07, 0x16, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, board/FRU replace old ver"},
  {0x2B, 0x07, 0x17, SENSOR_EVENT_CLASS_DISCRETE, L"Version Change", L"Firmware or software change success, board/FRU HW conf change"},

  {0x2C, 0x00, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"FRU State", L"Not Installed"},
  {0x2C, 0x01, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"FRU State", L"Inactive"},
  {0x2C, 0x02, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"FRU State", L"Activation Requested"},
  {0x2C, 0x03, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"FRU State", L"Activation in Progress"},
  {0x2C, 0x04, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"FRU State", L"Active"},
  {0x2C, 0x05, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"FRU State", L"Deactivation Requested"},
  {0x2C, 0x06, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"FRU State", L"Deactivation in Progress"},
  {0x2C, 0x07, 0xFF, SENSOR_EVENT_CLASS_DISCRETE, L"FRU State", L"Communication lost"},

  {0x00, 0x00, 0x00, 0x00, NULL, NULL},
};

SEL_GENERATORID_OFFSET mGeneratorID[] = {
  {0x00, 0x0F, L"BIOS"},
  {0x10, 0x1F, L"SMI Handler"},
  {0x20, 0x2F, L"System Management Software"},
  {0x30, 0x3F, L"OEM"},
  {0x40, 0x40, L"Remote Console software 1"},
  {0x41, 0x41, L"Remote Console software 2"},
  {0x42, 0x42, L"Remote Console software 3"},
  {0x43, 0x43, L"Remote Console software 4"},
  {0x44, 0x44, L"Remote Console software 5"},
  {0x45, 0x45, L"Remote Console software 6"},
  {0x46, 0x46, L"Remote Console software 7"},
  {0x47, 0x47, L"Terminal Mode Remote Console software"},
  {MAX_SENSOR_GENDEATOR_ID_DEFINED, MAX_SENSOR_GENDEATOR_ID_DEFINED, NULL},
};

CHAR16  *mGeneratorString[] ={
  L"IPMB Slave Address",
  L"System Software ID",
  L"IPMB Device LUN",
};


//
// Global for the handle that the SelInfo Protocol is installed
//
H2O_IPMI_SEL_INFO_PROTOCOL    mSelInfoProtocol;
SENSOR_NAME_AND_UNIT_TABLE    *mSensorNameAndUnitTable = NULL;


/**
 Create the table contains sensor name & sensor unit.

*/
VOID
CreateSensorNameAndUnitTable (
  VOID
  )
{
  SDR_DATA_STURCT                       *SdrData;
  UINT16                                SdrCount;
  EFI_STATUS                            Status;
  UINT16                                Index;


  //
  // Get SDR data
  //
  Status = IpmiLibGetSdrData (SDR_ID_ALL, 0x0300, &SdrCount, &SdrData);
  if (EFI_ERROR (Status)) {
    return;
  }

  //
  // Currently we need sensor name & unit
  //
  mSensorNameAndUnitTable = AllocateZeroPool ((UINTN)(sizeof (SENSOR_NAME_AND_UNIT_TABLE) * MAX_SENSOR_NUMBER));
  if (mSensorNameAndUnitTable == NULL) {
    FreePool (SdrData);
    return;
  }

  //
  // Start fill in
  //
  for (Index = 0; Index < SdrCount; ++Index) {
    mSensorNameAndUnitTable[SdrData[Index].SensorNumber].Name = SdrData[Index].Name;
    if (SdrData[Index].DataStatus & SDR_SENSOR_TYPE_ANALOG) {
      mSensorNameAndUnitTable[SdrData[Index].SensorNumber].Unit = SdrData[Index].Unit;
    }
  }

}


/**
 Convert SEL entry to string based SEL entry read from "Get SEL Entry" IPMI Command.

 @param[in]         This                H2O_IPMI_SEL_INFO_PROTOCOL pointer.
 @param[in]         SelEntry            SEL entry to be coverted.
 @param[out]        SelInfo             Structure contains SEL entry information description strings.
                                        Callers should pass in structure buffer.

 @retval EFI_SUCCESS                    Convert success.
 @retval EFI_UNSUPPORTED                Event type is unspecified or sensor type is out of Range.
 @retval EFI_INVALID_PARAMETER          If one of arguments is NULL pointer.
*/
EFI_STATUS
EFIAPI
GetSelInfo (
  IN  H2O_IPMI_SEL_INFO_PROTOCOL        *This,
  IN  VOID                              *SelEntry,
  OUT SEL_INFO_STRUCT                   *SelInfo
  )
{
  H2O_IPMI_SEL_DATA                     *SelData;
  SENSOR_EVENT_TYPE_ENTRY               *Table;
  UINT8                                 Code;
  UINT8                                 Offset;
  EFI_STATUS                            Status;
  UINT8                                 Index;
  EFI_STATUS                            ConvertStatus;
  CHAR16                                ValueStr[MAX_VALUE_STRING_LEN];

  //
  // Check parameters first
  //
  if (This == NULL || SelEntry == NULL || SelInfo == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  //
  // Check RecordType to see if this is not standard format record
  //
  SelData = (H2O_IPMI_SEL_DATA*)SelEntry;
  if (SelData->RecordType != 0x02) {
    return EFI_UNSUPPORTED;
  }

  //
  // The record is standard record format, check event type to see if it's data is unspecified
  // and check sensor type code whether out of Range (reserved or OEM reserved).
  //
  if ((SelData->EventType < 0x01) || (SelData->SensorType > 0x2C)) {
    return EFI_UNSUPPORTED;
  }

  Status = EFI_UNSUPPORTED;

  //
  // Check event type to see if it's threshold(01h) or generic(02h-0Ch) or sensor-specific(6Fh).
  //
  if (SelData->EventType <= 0x0C || SelData->EventType == 0x6F) {


    //
    // Create sensor name & unit table first
    //
    if (mSensorNameAndUnitTable == NULL) {
      CreateSensorNameAndUnitTable ();
    }


    //
    // This SEL entry should be supported.
    //
    if (SelData->EventType == 0x6F) {
      Table = mSensorSpecificTypes;
      Code = SelData->SensorType;
    } else {
      Table = mGenericEventTypes;
      Code = SelData->EventType;
    }


    Offset = SelData->EventData1 & 0x0F;
    ZeroMem (SelInfo->SensorName, (UINTN) sizeof (SelInfo->SensorName));
    ZeroMem (SelInfo->Type, (UINTN) sizeof (SelInfo->Type));
    ZeroMem (SelInfo->Desc, (UINTN) sizeof (SelInfo->Desc));
    ZeroMem (SelInfo->Generator, (UINTN) sizeof (SelInfo->Generator));


    while (Table->Type != NULL) {

      if ((Table->Code == Code && Table->Offset == Offset && Table->Desc != NULL) &&
          ((Table->Data == ALL_OFFSETS_SPECIFIED) ||
           ((SelData->EventData1 & DATA_BYTE2_SPECIFIED_MASK) &&
            (Table->Data == SelData->EventData2))) ) {
        StrCpy (SelInfo->Type, Table->Type);
        StrCpy (SelInfo->Desc, Table->Desc);
        Status = EFI_SUCCESS;

        if (mSensorNameAndUnitTable == NULL || mSensorNameAndUnitTable[SelData->SensorNum].Name== NULL) {
          ZeroMem (SelInfo->SensorName, (UINTN) sizeof (SelInfo->SensorName));
        } else {
          StrCpy (SelInfo->SensorName, mSensorNameAndUnitTable[SelData->SensorNum].Name);
        }

        //
        // Concatenate sensor trigger reading & threshold to SelInfo->Desc.
        //
        if ((mSensorNameAndUnitTable != NULL && mSensorNameAndUnitTable[SelData->SensorNum].Unit != NULL) &&
            (Table->Class == SENSOR_EVENT_CLASS_THRESHOLD)) {
          if (SelData->EventData1 & HAVE_TRIGGER_READING) {
            ConvertStatus = IpmiLibSensorReadingToStr (SelData->SensorNum, SelData->EventData2, DEFAULT_PRECISION, ValueStr);
            if (ConvertStatus == EFI_SUCCESS) {
              StrCat (SelInfo->Desc, L" (Reading: ");
              StrCat (SelInfo->Desc, ValueStr);
              StrCat (SelInfo->Desc, mSensorNameAndUnitTable[SelData->SensorNum].Unit);
              if (SelData->EventData1 & HAVE_TRIGGER_THRESHOLD) {
                ConvertStatus = IpmiLibSensorReadingToStr (SelData->SensorNum, SelData->EventData3, DEFAULT_PRECISION, ValueStr);
                if (ConvertStatus == EFI_SUCCESS) {
                  StrCat (SelInfo->Desc, L", Threshold: ");
                  StrCat(SelInfo->Desc, ValueStr);
                  StrCat (SelInfo->Desc, mSensorNameAndUnitTable[SelData->SensorNum].Unit);
                  StrCat (SelInfo->Desc, L")");
                }
              }
            }
          }
        }


        break;

      }

      Table++;
    }

  }

  StrCpy (SelInfo->IdType, mGeneratorString[(SelData->GeneratorId & 0x01)]);
  ZeroMem (SelInfo->Lun, (UINTN) sizeof (SelInfo->Lun));
  if (!(SelData->GeneratorId & 0x01)) {
    StrCpy (SelInfo->Lun, mGeneratorString[2]);
  } else {
    Index = 0;
    while (mGeneratorID[Index].Offset < MAX_SENSOR_GENDEATOR_ID_DEFINED) {
      if (((SelData->GeneratorId >> 1) <= mGeneratorID[Index].EndOffset) && ((SelData->GeneratorId >> 1) >= mGeneratorID[Index].Offset)) {
        StrCpy (SelInfo->Generator, mGeneratorID[Index].Name);
        break;
      }
      Index++;
    }
  }

  //
  // Make "Type" string is always sensor type
  //
  if (SelData->EventType != 0x6F) {
    Table = mSensorSpecificTypes;
    Code = SelData->SensorType;

    while (Table->Type != NULL) {
      if (Table->Code == Code) {
        StrCpy (SelInfo->Type, Table->Type);
        break;
      }
      Table++;
    }
  }

  return Status;

}


/**
 Fill private data content.

*/
VOID
InitialIpmiSelInfoContent (
  VOID
  )
{
  mSelInfoProtocol.GetSelInfo = GetSelInfo;
}

