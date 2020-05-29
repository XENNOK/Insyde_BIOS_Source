/** @file
 Provide main functions of Bios Region Lock protocol
 
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

#include <Uefi.h>
#include <PchAccess.h>
#include <PchRegs.h>
#include <SmiTable.h>
#include <ChipsetSmiTable.h>
#include <Library/BaseLib.h>
#include <Library/IoLib.h>
#include <Library/DebugLib.h>
#include <Library/PcdLib.h>
#include "BiosRegionLockInfo.h"
#include "BiosRegionLockHelpFun.h"


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
  )
{
#if 0
  UINTN      Base;
  UINTN      Length;
  EFI_STATUS Status;

  if (PchMmRcrb16 (R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FLOCKDN) {
    DEBUG ((EFI_D_ERROR, "SPI has been locked - Access Denied!\n"));
    return EFI_ACCESS_DENIED;
  }

  Status = GetBiosRegionAddress (Type, &Base, &Length);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return SetRegionByAddress (This, Base, Length);
#else
  return EFI_UNSUPPORTED;
#endif
}

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
  )
{
  BIOS_REGION_LOCK_INSTANCE  *Private;
  EFI_STATUS                  Status;

  if (PchMmRcrb16 (R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FLOCKDN) {
    DEBUG ((EFI_D_ERROR, "SPI has been locked - Access Denied!\n"));
    return EFI_ACCESS_DENIED;
  }

  Private = BIOS_REGION_LOCK_INSTANCE_FROM_THIS (This);
  Status  = MergeToBase (Private->BiosLock, BaseAddress, Length);
  if (EFI_ERROR (Status)) {
    return Status;
  }

//[-start-120118-IB08450031-modify]//
  return EFI_SUCCESS;
//[-end-120118-IB08450031-modify]//
}

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
  )
{
#if 0
  UINTN                       Base;
  UINTN                       Length;
  EFI_STATUS                  Status;

  if (PchMmRcrb16 (R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FLOCKDN) {
    DEBUG ((EFI_D_ERROR, "SPI has been locked - Access Denied!\n"));
    return EFI_ACCESS_DENIED;
  }
  
  Status = GetBiosRegionAddress (Type, &Base, &Length);
  if (EFI_ERROR (Status)) {
    return Status;
  }

  return ClearRegionByAddress (This, Base, Length);
#else
  return EFI_UNSUPPORTED;
#endif
}

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
  )
{
  BIOS_REGION_LOCK_INSTANCE  *Private;
  EFI_STATUS                  Status;

  if (PchMmRcrb16 (R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FLOCKDN) {
    DEBUG ((EFI_D_ERROR, "SPI has been locked - Access Denied!\n"));
    return EFI_ACCESS_DENIED;
  }

  Private = BIOS_REGION_LOCK_INSTANCE_FROM_THIS (This);
  Status = RemoveFromBase (Private->BiosLock, BaseAddress, Length);
  if (EFI_ERROR (Status)) {
    return Status;
  }

//[-start-120118-IB08450031-modify]//
  return EFI_SUCCESS;
//[-end-120118-IB08450031-modify]//
  
}

/**
 This function locks BIOS region described by PR0 to PR5 and trigger software SMI to enable BIOS Region SMM Protection.
 
 @param[in]         This                Protocol instance pointer.

 @retval EFI_SUCCESS
*/
EFI_STATUS
EFIAPI
Lock (
  IN BIOS_REGION_LOCK_PROTOCOL  *This
  )
{
//[-start-120118-IB08450031-add]//
  BIOS_REGION_LOCK_INSTANCE  *Private;
  EFI_STATUS                  Status;
//[-end-120118-IB08450031-add]//

//[-start-120118-IB08450031-modify]//

  if (PchMmRcrb16 (R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FLOCKDN) {
    DEBUG ((EFI_D_ERROR, "SPI has been locked - Access Denied!\n"));
    return EFI_ACCESS_DENIED;
  }

  Private = BIOS_REGION_LOCK_INSTANCE_FROM_THIS (This);

  //
  // Program PR registers
  //
  Status = ProgramRegister (Private->BiosLock);
  ASSERT_EFI_ERROR (Status);

  //
  // Lock the Protected Range registers
  //
  if ((PchMmRcrb32 (R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FLOCKDN) == 0) {
    PchMmRcrb16Or (R_PCH_SPI_HSFS, B_PCH_SPI_HSFS_FLOCKDN);
    // 
    // Invoke PchBiosLockSwSmiCallback to enable BIOS Region SMM Protection
    //
    IoWrite8 (SW_SMI_PORT, BIOS_LOCK_SW_SMI);
    DEBUG ((EFI_D_ERROR, "SPI is locked now!\n"));
  } else {
    DEBUG ((EFI_D_ERROR, "SPI is already locked. Can not lock again!\n"));
    CpuDeadLoop ();
  }
//[-end-120118-IB08450031-modify]//

  return EFI_SUCCESS;
}

