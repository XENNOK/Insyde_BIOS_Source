/** @file
 
;******************************************************************************
;* Copyright (c) 2012-2013, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/


#ifndef _BIOS_REGION_LOCK_INFO_H_
#define _BIOS_REGION_LOCK_INFO_H_

#include <Base.h>
#include <Protocol/BiosRegionLock.h>

#define MAX_BIOS_REGION_LOCK   5

typedef struct {
  UINTN  Base;
  UINTN  Length;
} BIOS_REGION_LOCK_BASE;

typedef struct {
  BIOS_REGION_LOCK_BASE      BiosLock[MAX_BIOS_REGION_LOCK];
  BIOS_REGION_LOCK_PROTOCOL  BiosRegionLock;
} BIOS_REGION_LOCK_INSTANCE;

#define BIOS_REGION_LOCK_INSTANCE_FROM_THIS(this)  BASE_CR (this, BIOS_REGION_LOCK_INSTANCE, BiosRegionLock)


/**
 This function set the BIOS region by requested type to the BIOS protect region register.

 @param[in]         This                Protocol instance pointer.
 @param[in]         Type                The BIOS Region type which need to be locked.

 @retval EFI_ACCESS_DENIED     The BIOS protect region register has been locked.
 @retval EFI_INVALID_PARAMETER The input Type is invalid.
 @retval EFI_OUT_OF_RESOURCES  All BIOS protect region registers have been set and the requested region 
                               can not be merge to existing protected region.
 @retval EFI_SUCCESS           Set BIOS protect region register successfully
*/
EFI_STATUS
EFIAPI
SetRegionByType (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN BIOS_REGION_TYPE            Type
);


/**
 This function sets the BIOS region by requested base address and legnth to the BIOS protect region register.

 @param[in]         This                Protocol instance pointer.
 @param[in]         BaseAddress         The start address of the BIOS region which need to be locked.
 @param[in]         Length              The Length of the BIOS region which need to be locked.

 @retval EFI_ACCESS_DENIED     The BIOS protect region register has been locked.
 @retval EFI_OUT_OF_RESOURCES  All BIOS protect region registers have been set and the requested region 
                               can not be merge to existing protected region.
 @retval EFI_SUCCESS           Set BIOS protect region register successfully
*/
EFI_STATUS
EFIAPI
SetRegionByAddress (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN UINTN                       BaseAddress,
  IN UINTN                       Length 
);


/**
 This function removes the BIOS region by requested type from the BIOS protect region register.
 
 @param[in]         This                Protocol instance pointer.
 @param[in]         Type                The BIOS Region type which need to be locked.
 
 @retval EFI_ACCESS_DENIED     The BIOS protect region register has been locked.
 @retval EFI_INVALID_PARAMETER The input Type is invalid.
 @retval EFI_OUT_OF_RESOURCES  The BIOS protect region registers are not enough to set for all discontinuous BIOS region
 @retval EFI_SUCCESS           Set BIOS protect region register successfully
*/
EFI_STATUS
EFIAPI
ClearRegionByType (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN BIOS_REGION_TYPE            Type
);


/**
 This function removes the BIOS region by requested base address and legnth from the BIOS protect region register.
 
 @param[in]         This                Protocol instance pointer.
 @param[in]         BaseAddress         The start address of the BIOS region which need to be locked.
 @param[in]         Length              The Length of the BIOS region which need to be locked.

 @retval EFI_ACCESS_DENIED     The BIOS protect region register has been locked.
 @retval EFI_OUT_OF_RESOURCES  The BIOS protect region registers are not enough to set for all discontinuous BIOS region
 @retval EFI_SUCCESS           Set BIOS protect region register successfully
*/
EFI_STATUS
EFIAPI
ClearRegionByAddress (
  IN BIOS_REGION_LOCK_PROTOCOL  *This,
  IN UINTN                       BaseAddress,
  IN UINTN                       Length 
);


/**
 This function locks BIOS region described by PR0 to PR5 and trigger software SMI to enable BIOS Region SMM Protection.
 
 @param[in]         This                Protocol instance pointer.

 @retval EFI_SUCCESS
*/
EFI_STATUS
EFIAPI
Lock (
  IN BIOS_REGION_LOCK_PROTOCOL  *This
);

#endif //_BIOS_REGION_LOCK_INFO_H_
