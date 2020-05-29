/** @file
 H2O SMM Chipset Services protocol interface definition.
 
 This file provides the H2O SMM Chipset Services protocol definition. It is 
 consumed by drivers and libraries in the Kernel and Platform layers. It is
 produced by drivers and libraries in the Chipset layer.
 
 WARNING: This protocol should not be called directly. Instead, the library
 functions in the SmmChipsetSvcLib should be used, because they add the 
 necessary error checking.
 
***************************************************************************
* Copyright (c) 2012 - 2013, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#ifndef _H2O_SMM_CHIPSET_SERVICES_H_
#define _H2O_SMM_CHIPSET_SERVICES_H_
#include <ChipsetSvc.h>

#define H2O_SMM_CHIPSET_SERVICES_PROTOCOL_GUID \
  {0x42b70ab8, 0xb17f, 0x48b9, {0x93, 0x35, 0x10, 0x24, 0xa6, 0xe0, 0xa, 0x7b }}

//
// Function prototypes
//
/**
 Platform specific function to enable/disable flash device write access. 

 @param[in]         EnableWrites        TRUE  - Enable
                                        FALSE - Disable
                    
 @retval            EFI_SUCCESS         Function alway returns successfully
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_ENABLE_FD_WRITES) (
  IN      BOOLEAN                         EnableWrites
);

/**
 Legacy Region Access Control. 
 
 @param[in]         Start               Start of the region to lock or unlock.
 @param[in]         Length              Length of the region.
 @param[in]         Mode                If LEGACY_REGION_ACCESS_LOCK, then
                                        LegacyRegionAccessCtrl() lock the specific
                                        legacy region.
                    
 @retval            EFI_SUCCESS         Function returns successfully
 @retval            EFI_INVALID_PARAMETER 1. The parameter Start is small then 0xC0000.
                                          2. The parameter Length is to long. 
                                             The Start + Length - 1 should small then 0xF0000.
                                          3. The Mode parameter is neither 
                                             LEGACY_REGION_ACCESS_LOCK nor LEGACY_REGION_ACCESS_UNLOCK

*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_LEGACY_REGION_ACCESS_CTRL) (
  IN      UINT32                          Start,
  IN      UINT32                          Length,
  IN      UINT32                          Mode  
  );

/**
 Resets the system.

 @param[in]         ResetType           The type of reset to perform.
                                
 @retval            EFI_UNSUPPORTED     ResetType is invalid.
 @return            others              Error occurs
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_RESET_SYSTEM) (
  IN      EFI_RESET_TYPE                  ResetType
  );

/**
 This routine issues SATA COM reset on the specified SATA port 

 @param[in]         PortNumber          The SATA port number to be reset
                    
 @retval            EFI_SUCCESS         The SATA port has been reset successfully
 @retval            EFI_DEVICE_ERROR    1.SATA controller isn't in IDE, AHCI or RAID mode.
                                        2.Get error when getting PortNumberMapTable.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_CS_SVC_SATA_COMRESET) (
  IN  UINTN                    PortNumber
  );

typedef struct _H2O_SMM_CHIPSET_SERVICES_PROTOCOL {
  UINT32 Size;                          ///< size of this structure, in bytes.
  
  H2O_CS_SVC_ENABLE_FD_WRITES            EnableFdWrites;
  H2O_CS_SVC_LEGACY_REGION_ACCESS_CTRL   LegacyRegionAccessCtrl;  
  H2O_CS_SVC_RESET_SYSTEM                ResetSystem;
  H2O_CS_SVC_SATA_COMRESET               SataComReset;
} H2O_SMM_CHIPSET_SERVICES_PROTOCOL;

extern EFI_GUID gH2OSmmChipsetServicesProtocolGuid;

#endif
