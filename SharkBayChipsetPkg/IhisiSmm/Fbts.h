/** @file
  This driver provides IHISI interface in SMM mode

;******************************************************************************
;* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _IHISI_FBTS_H_
#define _IHISI_FBTS_H_

#include "IhisiSmm.h"
#include "FbtsOemHook.h"
#include <EfiFlashMap.h>
#include <PchRegs.h>
#include <PchAccess.h>
#include <ChipsetSetupConfig.h>
#include <ChipsetCmos.h>

#include <Library/CmosLib.h>
//[-start-130808-IB06720232-remove]//
//#include <Library/EcLib.h>
//[-end-130808-IB06720232-remove]//
#include <Library/HobLib.h>
#include <Library/FdSupportLib.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//

#include <Protocol/SmmBase2.h>
#include <Protocol/SmmRuntime.h>
#include <Protocol/SmmFwBlockService.h>
#include <Protocol/SmmTdt.h>
#include <Library/IoLib.h>

#include <Guid/FlashMapHob.h>


//[-start-130327-IB08340100-add]//
#define FLASH_COMPLETE_STATUS_SIGNATURE        SIGNATURE_64 ('$', 'F', 'I', 'N', 'I', 'S', 'H', 'I')

typedef enum {
  ApTerminated = 0,
  NormalFlash,
  PartialFlash,
} FLASH_COMPLETE_STATUS;
//[-end-130327-IB08340100-add]//


//
// FBTS Support Functions
//
//[-start-140505-IB10310055-modify]//
#define FBTS_FUNCTIONS  \
        { 0x10, FbtsGetSupportVersion       }, \
        { 0x11, FbtsGetPlatformInfo         }, \
        { 0x12, FbtsGetPlatformRomMap       }, \
        { 0x13, FbtsGetFlashPartInfo        }, \
        { 0x14, FbtsRead                    }, \
        { 0x15, FbtsWrite                   }, \
        { 0x16, Fbtscomplete                }, \
        { 0x17, GetRomFileAndPlatformTable  }, \
        { 0x1B, SkipMcCheckAndBinaryTrans   }, \
        { 0x1C, FbtsGetATpInformation       }, \
        { 0x1E, FbtsGetWholeBiosRomMap      }, \
        { 0x50, FbtsPassImage               }, \
        { 0x51, FbtsWriteImageToAddress     }
//[-end-140505-IB10310055-modify]//

//[-start-130327-IB08340100-add]//
#pragma pack(1)

typedef struct {
  UINT64      Signature;
  UINT32      SturctureSize;
  UINT8       CompleteStatus;
} FBTS_FLASH_COMPLETE_STATUS;

#pragma pack()
//[-end-130327-IB08340100-add]//


//
// Function Prototype
//

/**
  AH=10h, Get FBTS supported version and FBTS permission.

  @retval EFI_SUCCESS        Get whole BIOS ROM map successful.
**/
EFI_STATUS
FbtsGetSupportVersion (
  VOID
  );

/**
  AH=11h, Get platform information.

  @retval EFI_SUCCESS        Get platform information successful.
  @return Other              Get platform information failed.
**/
EFI_STATUS
FbtsGetPlatformInfo (
  VOID
  );

/**
  AH=12h, Get Platform ROM map protection.

  @retval EFI_SUCCESS        Get Platform ROM map protection successful.
**/
EFI_STATUS
FbtsGetPlatformRomMap (
  VOID
  );

/**
  AH=13h, Get Flash part information.

  @retval EFI_SUCCESS        Get Flash part information successful.
  @return Other              Get Flash part information failed.
**/
EFI_STATUS
FbtsGetFlashPartInfo (
  VOID
  );

/**
  Passing information to flash program on whether
  if current BIOS (AT-p capable) can be flashed with
  other BIOS ROM file
  AT-p: (Anti-Theft PC Protection).

  @retval EFI_SUCCESS   Success returns.
**/
EFI_STATUS
FbtsGetATpInformation (
  VOID
  );

/**
   AH=14h, FBTS Read.

  @retval EFI_SUCCESS        Read data from flash part successful.
  @return Other              Read data from flash part failed.
**/
EFI_STATUS
FbtsRead (
  VOID
  );

/**
  AH=15h, FBTS Write.

  @retval EFI_SUCCESS        Write data to flash part successful.
  @return Other              Write data to flash part failed.
**/
EFI_STATUS
FbtsWrite (
  VOID
  );

/**
  AH=16h, This function uses to execute some specific action after the flash process is completed.

  @retval EFI_SUCCESS        Function succeeded.
  @return Other              Error occurred in this function.
**/
EFI_STATUS
Fbtscomplete (
  VOID
  );

/**
  Get platform and Rom file flash descriptor region.

  @retval EFI_SUCCESS   Success returns.
**/
EFI_STATUS
GetRomFileAndPlatformTable (
  VOID
  );

/**
  Skip module check allows and binary file transmissions.

  @retval EFI_SUCCESS   Success returns.
**/
EFI_STATUS
SkipMcCheckAndBinaryTrans (
  VOID
  );

/**
  Get Flash table from Rom file.
  if DescriptorMode is true, the FlashTable will be filled.
  if the descriptor is false,the FlashTable will be filled RegionTypeEos(0xff) directly.

  @param[in]  InputDataBuffer    the pointer to Rom file.
  @param[in]  DataBuffer         IN: the input buffer address.
                                 OUT:the flash region table from rom file.
  @param[in]  DescriptorMode     TRUE is indicate this Rom file is descriptor mode
                                 FALSE is indicate this Rom file is non-descriptor mode

  @retval EFI_SUCCESS            Successfully returns
**/
EFI_STATUS
GetRomFileFlashTable (
  IN       UINT8           *InputDataBuffer,
  IN OUT   FLASH_REGION    *DataBuffer,
  IN       BOOLEAN         DescriptorMode
  );

/**
  Initialize Fbts relative services

  @retval EFI_SUCCESS        Initialize Fbts services successful.
  @return Others             Any error occurred.
**/
EFI_STATUS
FbtsInit (
  VOID
  );

/**
  Get flash map from HOB.

  @retval EFI_SUCCESS        Get flash map from HOB successful.
  @return Others             Any error occurred.
**/
EFI_STATUS
GetFlashMapByHob (
  VOID
  );

/**
  Fill platform protect ROM map information to module ROM map buffer.

  @retval EFI_SUCCESS        Set platform protect ROM map information to module ROM map buffer successful.
  @retval EFI_UNSUPPORTED    Module ROM map buffer is full.
**/
EFI_STATUS
FillPlatformRomMapBuffer (
  IN UINT8    Type,
  IN UINT32   Address,
  IN UINT32   Length,
  IN UINT8    Entry
);

/**
  Fill platform private ROM map information to module ROM map buffer.

  @retval EFI_SUCCESS        Set platform ROM map information to module ROM map buffer successful.
  @retval EFI_UNSUPPORTED    Module ROM map buffer is full.
**/
EFI_STATUS
FillPlatformPrivateRomMapBuffer (
  IN UINT32   Address,
  IN UINT32   Length,
  IN UINT8    Entry
);

/**
  AH=1Eh, Get whole BIOS ROM map (Internal BIOS map)

  @retval EFI_SUCCESS        Get whole BIOS ROM map successful.
**/
EFI_STATUS
FbtsGetWholeBiosRomMap (
  VOID
  );

//[-start-1400505-IB10310055-add]//
/**
  IHISI Function 50h.
  Passing Image to BIOS for specific check
  (Tool is master to pass image to BIOS via the interface.)

  @param None.

  @retval EFI_SUCCESS           Image is valid so far.
  @retval EFI_UNSUPPORTED       Image is valid and the last check is bypassed.
  @retval EFI_BAD_BUFFER_SIZE   Invalid data size.
  @retval Others                Image is invalid
*/
EFI_STATUS
FbtsPassImage (
  VOID
  );

/**
  IHISI Function 51h.
  Inform BIOS to write image data to SPI ROM.

  @param None.

  @retval EFI_SUCCESS        Write data to flash part successful.
  @return Other              Write data to flash part failed.
**/
EFI_STATUS
FbtsWriteImageToAddress (
  VOID
  );
//[-end-140505-IB10310055-add]//

#endif
