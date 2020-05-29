//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************

#ifndef _PnP_H_
#define _PnP_H_

#pragma pack (1)


typedef
INT16
(*PNP_REDIRECTED_ENTRY_POINT) (
  IN VOID                               *Frame
  );

typedef struct _PNP_FAR_PTR {
  UINT16                                Offset;
  UINT16                                Segment;
} PNP_FAR_PTR;

typedef struct {
  UINT16                                PushedBp;
  PNP_FAR_PTR                           ReturnAddress;
  INT16                                 Function;
} PNP_GENERIC_ENTRY_FRAME;

//
// PnP return codes
//

#define PNP_SUCCESS                                 0x00   // Function completed successfully

// Warning Codes 01h through 7Fh
#define PNP_NOT_SET_STATICALLY                      0x7F   // Warning that indicates a device could not be configured
                                                           // statically, but was successfully configured dynamically.
                                                           // This return code is used only when function 02h is
                                                           // requested to set a device both statically and dynamically.

// Error Codes 81h through FFh
#define PNP_UNKNOWN_FUNCTION                        0x81   // Unknown, or invalid, function number passed
#define PNP_FUNCTION_NOT_SUPPORTED                  0x82   // The function is not supported on this system.
#define PNP_INVALID_HANDLE                          0x83   // Device node number/handle passed is invalid or out of range.
#define PNP_BAD_PARAMETER                           0x84   // Function detected invalid resource descriptors or resource
                                                           // descriptors were specified out of order.
#define PNP_SET_FAILED                              0x85   // Set Device Node function failed.
#define PNP_EVENTS_NOT_PENDING                      0x86   // There are no events pending.
#define PNP_SYSTEM_NOT_DOCKED                       0x87   // The system is currently not docked.
#define PNP_NO_ISA_PNP_CARDS                        0x88   // Indicates that no ISA Plug and Play cards are installed in the system.
#define PNP_UNABLE_TO_DETERMINE_DOCK_CAPABILITIES   0x89   // Indicates that the system was not able to determine the
                                                           // capabilities of the docking station.
#define PNP_CONFIG_CHANGE_FAILED_NO_BATTERY         0x8A   // The system failed the undocking sequence because it
                                                           // detected that the system unit did not have a battery.
#define PNP_CONFIG_CHANGE_FAILED_ RESOURCE_CONFLICT 0x8B   // The system failed to successfully dock because it
                                                           // detected a resource conflict with one of the primary
                                                           // boot devices; such as Input, Output, or the IPL device.
#define PNP_BUFFER_TOO_SMALL                        0x8C   // The memory buffer passed in by the caller was not
                                                           // large enough to hold the data to be returned by the system BIOS.
#define PNP_USE_ESCD_SUPPORT                        0x8D   // This return code is used by functions 09h and 0Ah to instruct
                                                           // the caller that reporting resources explicitly assigned to
                                                           // devices in the system to the system BIOS must be handled
                                                           // through the interfaces defined by the ESCD Specification.
#define PNP_MESSAGE_NOT_SUPPORTED                   0x8E   // This return code indicates the message passed to the system
                                                           // BIOS through function 04h, Send Message, is not supported on the system.
#define PNP_HARDWARE_ERROR                          0x8F   // This return code indicates that the system BIOS detected a
                                                           // hardware failure.

#pragma pack ()

VOID *
PnpFarToLinear(
  IN PNP_FAR_PTR                        Ptr
  )
/*++

Routine Description:

  Does a quick conversion from a 16-bit C far pointer to a linear
  address.  Unfortunately, this can only support far pointers from
  16-bit real mode.

Arguments:

  far - 16-bit far pointer

Returns:

  None

--*/
;

INT16
UnsupportedPnpFunction(
  PNP_GENERIC_ENTRY_FRAME               *Frame
  )
/*++

Routine Description:

  default function returning that the call was to an unsupported function.

Arguments:

  Frame - unused

Returns:

  None

--*/
;

UINT32
LogicalToPhysicalAddress (
  IN UINT32 LinearAddress,
  IN UINT32 CR3Value,
  IN UINT32 CR4Value,
  IN UINTN  SegementSelector
  )
/*++

Routine Description:

  default function returning that the call was to an unsupported function.

Arguments:

  Frame - unused

Returns:

  None

--*/
;
#endif
