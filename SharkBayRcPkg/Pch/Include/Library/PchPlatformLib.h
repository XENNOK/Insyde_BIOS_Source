/** @file
  Header file for PchPlatform Lib.

@copyright
  Copyright (c) 2008 - 2013 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

  This file contains a 'Sample Driver' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may be modified by the user, subject to
  the additional terms of the license agreement
**/
#ifndef _PCH_PLATFORM_LIB_H_
#define _PCH_PLATFORM_LIB_H_
#include <PchRegs.h>
///
/// Timeout value used when Sending / Receiving messages.
/// NOTE: this must cover the longest possible wait time
/// between message being sent and response being available.
/// e.g. Virtual function readiness might take some time.
///

/**
  Delay for at least the request number of microseconds.
  This function would be called by runtime driver, please do not use any MMIO marco here.

  @param[in] Microseconds         Number of microseconds to delay.

  @retval NONE
**/
VOID
EFIAPI
PchPmTimerStall (
  IN  UINTN   Microseconds
  );

/**
  Check whether SPI is in descriptor mode

  @param[in] PchRootComplexBar    The PCH Root Complex Bar

  @retval TRUE                    SPI is in descriptor mode
  @retval FALSE                   SPI is not in descriptor mode
**/
BOOLEAN
EFIAPI
PchIsSpiDescriptorMode (
  IN  UINTN   PchRootComplexBar
  );

/**
  Return Pch stepping type

  @param[in] None

  @retval PCH_STEPPING            Pch stepping type
**/
PCH_STEPPING
EFIAPI
PchStepping (
  VOID
  );

/**
  Determine if PCH is supported

  @param[in] None

  @retval TRUE                    PCH is supported
  @retval FALSE                   PCH is not supported
**/
BOOLEAN
IsPchSupported (
  VOID
  );

/**
  This function can be called to enable/disable Alternate Access Mode

  @param[in] PchRootComplexBar    The PCH Root Complex Bar
  @param[in] AmeCtrl              If TRUE, enable Alternate Access Mode.
                                  If FALSE, disable Alternate Access Mode.

  @retval NONE
**/
VOID
EFIAPI
PchAlternateAccessMode (
  IN  UINTN         PchRootComplexBar,
  IN  BOOLEAN       AmeCtrl
  );

/**
  Configures PCH IOBP

  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] Address              Address of the IOBP register block
  @param[in] AndMask              Mask to AND with the register
  @param[in] OrMask               Mask to OR with the register

  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_DEVICE_ERROR        Transaction fail
**/
EFI_STATUS
EFIAPI
ProgramIobp (
  IN UINT32                              RootComplexBar,
  IN UINT32                              Address,
  IN UINT32                              AndMask,
  IN UINT32                              OrMask
  );

/**
  Read data from PCH IOBP register block

  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] Address              Address of the IOBP register block
  @param[in] Data                 Data contain in the IOBP register block

  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_DEVICE_ERROR        Transaction fail
**/
EFI_STATUS
EFIAPI
ReadIobp (
  IN UINT32                              RootComplexBar,
  IN UINT32                              Address,
  OUT UINT32                             *Data
  );


typedef enum {
  MemoryMapRead          = 0x0,
  MemoryMapWrite         = 0x1,
  IoMapRead              = 0x2,
  IoMapWrite             = 0x3,
  PciConfigRead          = 0x4,
  PciConfigWrite         = 0x5,
  PrivateControlRead     = 0x6,
  PrivateControlWrite    = 0x7
} PCH_IOBP_OPCODE;

/**
  Configures PCH IOBP

  @param[in] RootComplexBar       RootComplexBar value of this PCH device
  @param[in] Address              Address of the IOBP register block
  @param[in] Opcode               Iobp Opcode
  @param[in] RouteId              Route Id
  @param[in, out] Data32          Read/Write data
  @param[out] Response            Response

  @retval EFI_SUCCESS             Successfully completed.
  @retval EFI_DEVICE_ERROR        Transaction fail
**/
EFI_STATUS
EFIAPI
PchIobpExecution (
  IN UINT32                              RootComplexBar,
  IN UINT32                              Address,
  IN PCH_IOBP_OPCODE                     Opcode,
  IN UINT8                               RouteId,
  IN OUT UINT32                          *Data32,
  OUT UINT8                              *Response
  );

/**
  Check whether Gbe Region is valid in SPI Flash

  @param[in] PchRootComplexBar    The PCH Root Complex Bar

  @retval TRUE                    Gbe Region is valid
  @retval FALSE                   Gbe Region is invalid
**/
BOOLEAN
EFIAPI
PchIsGbeRegionValid (
  IN  UINTN   PchRootComplexBar
  );

/**
   Check if integrated Gbe controller present

  @param[in] None

  @retval TRUE                    Integrated Gbe present
  @retval FALSE                   Integrated Gbe not present
**/
BOOLEAN
EFIAPI
PchIsIntegratedGbePresent (
  IN  VOID
  );
  
typedef enum {
  PchH          = 1,
  PchLp,
  PchUnknownSeries
} PCH_SERIES;

/**
  Return Pch Series

  @param[in] None

  @retval PCH_SERIES            Pch Series
**/
PCH_SERIES
EFIAPI
GetPchSeries (
  VOID
  );

/**
  Get Pch Maximum Pcie Root Port Number

  @param[in] None

  @retval PcieMaxRootPort         Pch Maximum Pcie Root Port Number
**/
UINT8
EFIAPI
GetPchMaxPciePortNum (
  VOID
  );

/**
  Get Pch Maximum Sata Port Number

  @param[in] None

  @retval Pch Maximum Sata Port Number
**/
UINT8
EFIAPI
GetPchMaxSataPortNum (
  VOID
  );

/**
  Get Pch Maximum Sata Controller Number

  @param[in] None

  @retval Pch Maximum Sata Controller Number
**/
UINT8
EFIAPI
GetPchMaxSataControllerNum (
  VOID
  );

/**
  Get Pch Maximum Usb Port Number of EHCI Controller

  @param[in] None

  @retval Pch Maximum Usb Port Number of EHCI Controller
**/
UINT8
EFIAPI
GetPchEhciMaxUsbPortNum (
  VOID
  );

/**
  Get Pch Maximum EHCI Controller Number

  @param[in] None

  @retval Pch Maximum EHCI Controller Number
**/
UINT8
EFIAPI
GetPchEhciMaxControllerNum (
  VOID
  );

/**
  Get Pch Usb Maximum Physical Port Number

  @param[in] None

  @retval Pch Usb Maximum Physical Port Number
**/
UINT8
EFIAPI
GetPchUsbMaxPhysicalPortNum (
  VOID
  );

/**
  Get Pch Maximum Usb2 Port Number of XHCI Controller

  @param[in] None

  @retval Pch Maximum Usb2 Port Number of XHCI Controller
**/
UINT8
EFIAPI
GetPchXhciMaxUsb2PortNum (
  VOID
  );

/**
  Get Pch Maximum Usb3 Port Number of XHCI Controller

  @param[in] None

  @retval Pch Maximum Usb3 Port Number of XHCI Controller
**/
UINT8
EFIAPI
GetPchXhciMaxUsb3PortNum (
  VOID
  );

typedef enum {
  WarmBoot          = 1,
  ColdBoot,
  PwrFlr,
  PwrFlrSys,
  PwrFlrPch,
  PchPmStatusMax
} PCH_PM_STATUS;

/**
  Query PCH to determine the Pm Status

  @param[in] PmStatus - The Pch Pm Status to be probed

  @retval Return TRUE if Status querried is Valid or FALSE if otherwise
**/
BOOLEAN
GetPchPmStatus (
  PCH_PM_STATUS PmStatus
  )
;

/**
  Get Pch Pcie Root Port Function Number by Root Port Number

  @param[in] UINT8 Root Port Number (start from 0)

  @retval Pch Pcie Root Port Function Number
**/
UINT8
EFIAPI
GetPchPcieRpfn (
  IN  UINTN   PchRootComplexBar,
  IN  UINT8   RpNumber
  );

/**
  Get Pch Pcie Root Port Number by Root Port Function Number

  @param[in] UINT8 Root Port Function Number

  @retval Pch Pcie Root Port Number
  @retval 0xFF  No Root Port Number found
**/
UINT8
EFIAPI
GetPchPcieRpNumber (
  IN  UINTN   PchRootComplexBar,
  IN  UINT8   Rpfn
  );

/**
  Returns GbE over PCIe port number.

  @return  Root port number (0-based)
  @retval  GbE over PCIe disabled
**/
UINTN
PchGetGbePortNumber (
  VOID 
  );
#endif