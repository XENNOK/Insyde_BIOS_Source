/** @file
  This driver provides IHISI interface in SMM mode

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

#ifndef _IHISI_OEM_FEATURE_H_
#define _IHISI_OEM_FEATURE_H_

#include "IhisiSmm.h"
//[-start-120913-IB05300329-add]//
#include "PfatDefinitions.h"
//[-end-120913-IB05300329-add]//

//[-start-120913-IB05300329-add]//
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Protocol/GlobalNvsArea.h>
//[-end-120913-IB05300329-add]//

#define AP_COMMUNICATION_SIGNATURE        SIGNATURE_32 ('$', 'E', 'D', 'I')
#define BIOS_COMMUNICATION_SIGNATURE      SIGNATURE_32 ('$', 'E', 'D', 'O')

#define OEM_COMMON_FEATURE_FUNCTIONS      { 0x41, OemExtraDataCommunication }, \
                                          { 0x42, OemExtraDataWrite }, \
                                          { 0x47, OemExtraDataRead }

//
// OEM Extra Data Communication Function Prototype
//

//[-start-120913-IB05300329-add]//
EFI_STATUS
OemCommonFeatureInit (
  VOID
  );
//[-end-120913-IB05300329-add]//

/**
  AH=41h, OEM Extra Data Communication

  01h = VBIOS
  02h~03h = Reserved
  04h~0Ch = User Define
  0Dh~4Fh = Reserved
  50h = OA 3.0 Read/Write
  51h = OA 3.0 Erase (Reset to default)
  52h = OA 3.0 Populate Header
  53h = OA 3.0 De-Populate Header
  54h = Logo Update (Write)
  55h = Check BIOS sign by System BIOS
  56~FFh = Reserved

  @retval EFI_SUCCESS    Process OEM extra data communication successful.
  @return Other          Process OEM extra data communication failed.
**/
EFI_STATUS
OemExtraDataCommunication (
  VOID
  );

/**
  AH=42h, OEM Extra Data Write.

  Function 41h and 42h (or 47h) are pairs. The function 41h has to be called before calling
  into function 42h.

  @retval EFI_SUCCESS            OEM Extra Data Write successful.
  @return Other                  OEM Extra Data Write failed.
**/
EFI_STATUS
OemExtraDataWrite (
  VOID
  );

/**
  Function to read OA3.0 data.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
OemExtraDataRead (
  VOID
  );


//
// OA 3.0 Function Prototype
//

/**
  AH=41h, OEM Extra Data Communication type 50h to read/write OA3.0.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS  Read or write OA3.0 successful.
**/
EFI_STATUS
Oa30ReadWriteFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  );

/**
  AH=41h, OEM Extra Data Communication type 51h to erase OA3.0 (reset to default).

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS  Erase OA3.0 successful.
**/
EFI_STATUS
Oa30EraseFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  );

/**
  AH=41h, OEM Extra Data Communication type 52h to populate header.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS   populate header successful.
**/
EFI_STATUS
Oa30PopulateHeaderFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  );

/**
  AH=41h, OEM Extra Data Communication type 53h to de-populate header.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS   populate header successful.
**/
EFI_STATUS
Oa30DePopulateHeaderFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  );

/**
  Function to write OA3.0 data and do action which request from IHISI function 42h.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
Oa30DataWrite (
  VOID
  );

/**
  Function to read OA3.0 data.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
Oa30DataRead (
  VOID
  );

/**
  AH=41h, OEM Extra Data Communication type 53h for VBIOS relative function.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
VbiosFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  );

/**
  AH=41h, OEM Extra Data Communication type 54h to update logo.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS   Update logo successful.
**/
EFI_STATUS
LogoUpdateFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  );

/**
  AH=41h, OEM Extra Data Communication type 55h to Check BIOS sign by System BIOS.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_SUCCESS   Successfully returns.
**/
EFI_STATUS
CheckBiosSignBySystemBiosFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  );

/**
  AH=41h, OEM Extra Data Communication type 02h~03h are reserved functions.

  @param[in]  ApCommDataBuffer   Pointer to AP communication data buffer.
  @param[out] BiosCommDataBuffer Pointer to BIOS communication data buffer.

  @retval EFI_UNSUPPORTED   Return unsupported to indicate this is reserved function.
**/

//[-start-120913-IB05300329-add]//
EFI_STATUS
PfatUpdateHandleFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  );

EFI_STATUS
PfatUpdateWrite (
  VOID
  );
//[-end-120913-IB05300329-add]//

//[-start-120917-IB05300333-add]//
EFI_STATUS
PfatReturnHandleFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  );
//[-end-120917-IB05300333-add]//

EFI_STATUS
ReservedFunction (
  IN  AP_COMMUNICATION_DATA_TABLE          *ApCommDataBuffer,
  OUT BIOS_COMMUNICATION_DATA_TABLE        *BiosCommDataBuffer
  );
//[-start-121002-IB05300338-add]//
extern EFI_GLOBAL_NVS_AREA                 *mGlobalNvsArea;
//[-end-121002-IB05300338-add]//

#endif
