/** @file
  SpiAccessLib implementation for SPI Flash Type devices

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include <Uefi.h>

#include <PiDxe.h>
#include <Protocol/SmmBase2.h>

#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/PcdLib.h>
#include <Library/S3BootScriptLib.h>
#include <Library/FdSupportLib.h>
#include <Library/SpiAccessLib.h>

#include <Library/DebugLib.h>
#include <Library/IrsiRegistrationLib.h>
#include <Library/SpiAccessInitLib.h>
//[-start-130416-IB10820284-remove]//
//#include <Library/UefiBootServicesTableLib.h>
//[-end-130416-IB10820284-remove]//
#include "SpiAccess.h"
//[-start-120419-IB10820037-add]//
#include <PchAccess.h>
#include <SaCommonDefinitions.h>
#include <Library/IoLib.h>
//[-end-120419-IB10820037-add]//
//[-start-120524-IB06710088-add]//
#include <OemSpi.h>
//[-end-120524-IB06710088-add]//

//[-start-130808-IB10300050-add]//
#include <Guid/CheckFlashAccess.h>
//[-start-140103-IB05160535-remove]//
//#include <Library/UefiRuntimeServicesTableLib.h>
//#include <Library/UefiBootServicesTableLib.h>
//[-end-140103-IB05160535-remove]//
//[-end-130808-IB10300050-add]//

//[-start-140103-IB05160535-modify]//
//[-start-130808-IB10300050-add]//
UINT32                  mIhisiFlash;
//[-end-130808-IB10300050-add]//
//[-end-140103-IB05160535-modify]//

SPI_CONFIG_BLOCK        mSpiConfigBlock;

BOOLEAN                 mSpiDeviceType;
EFI_PHYSICAL_ADDRESS    mValidWriteRegionStart;
EFI_PHYSICAL_ADDRESS    mValidWriteRegionEnd;
EFI_PHYSICAL_ADDRESS    mValidReadRegionStart;
EFI_PHYSICAL_ADDRESS    mValidReadRegionEnd;

VOID                    *mFlashBase   = NULL;
UINT8                   *mRcrbSpiBase = NULL;

/**
  Read SPI configuration from PCH SPI register

  @param CacheEnable            It indicates whether the cache is enabled (TRUE)
                                or not

  @retval EFI_SUCCESS           Function successfully returned

**/
//[-start-120418-IB05300312-add]//
//[-start-120907-IB05300326-add]//
BOOLEAN                        mPfatProtocolInitialed = FALSE;
//[-end-120907-IB05300326-add]//
PFAT_PROTOCOL                  *mPfatProtocol = NULL;
//[-end-120418-IB05300312-add]//
EFI_SMM_SYSTEM_TABLE2       *mSmst2;

//[-start-120524-IB06710088-add]//
#if  SPI_READ_BY_MEMORY_MAPPED
EFI_STATUS
SeperateAndCopyBiosRegionData (
  IN UINTN                    SrcOffset,
  IN UINT8                    *DstAddress,
  IN UINTN                    BufferSize,
  IN OUT SPI_READ_REGION      *NonBiosRegion
  )
/*++
Routine Description:

  Record the non BIOS region information and copy the BIOS region data from memory mapped address.

Arguments:

  SrcOffset            - Source data offset in SPI ROM
  DstAddress           - Destination address in memory
  BufferSize           - The size need to be read
  NonBiosRegion        - Record the seperated non BIOS region information

Returns:

  EFI_SUCCESS          - Successfully returns

--*/
{
  UINTN                SpiBiosRegionStart;
  UINTN                SpiBiosRegionEnd;
  UINTN                EndOfSrcOffset;

  SpiBiosRegionStart = (UINTN) (((MmSpi32(R_PCH_SPI_FREG1_BIOS) & B_PCH_SPI_FREG1_BASE_MASK) << 12));
  SpiBiosRegionEnd = SpiBiosRegionStart + PcdGet32 (PcdFlashAreaSize) - 1;
  EndOfSrcOffset = SrcOffset + BufferSize - 1;

  if ((SrcOffset >= SpiBiosRegionStart) && (EndOfSrcOffset <= SpiBiosRegionEnd)) {
    //
    // Read region is covered in BIOS region
    //
    CopyMem (DstAddress, (UINT8 *) (PcdGet32 (PcdFlashAreaBaseAddress) + (SrcOffset - SpiBiosRegionStart)), BufferSize);

  } else if ((EndOfSrcOffset < SpiBiosRegionStart) || (SrcOffset > SpiBiosRegionEnd)) {
    //
    // Read region is seperated with BIOS region
    //
    NonBiosRegion[0].Src = SrcOffset;
    NonBiosRegion[0].Dst = (UINTN)DstAddress;
    NonBiosRegion[0].Size = BufferSize;

  } else if ((SrcOffset < SpiBiosRegionStart) && (EndOfSrcOffset <= SpiBiosRegionEnd)) {
    //
    // Upper read region is overlapped with BIOS region
    //
    NonBiosRegion[0].Src = SrcOffset;
    NonBiosRegion[0].Dst = (UINTN)DstAddress;
    NonBiosRegion[0].Size = SpiBiosRegionStart - SrcOffset;

    CopyMem ((UINT8 *)((UINTN)DstAddress + NonBiosRegion[0].Size), (UINT8 *) (UINTN) (PcdGet32 (PcdFlashAreaBaseAddress)), BufferSize - NonBiosRegion[0].Size);

  } else if ((SrcOffset >= SpiBiosRegionStart) && (EndOfSrcOffset > SpiBiosRegionEnd)) {
    //
    // Lower read region is overlapped with BIOS region
    //
    NonBiosRegion[0].Src = SpiBiosRegionEnd + 1;
    NonBiosRegion[0].Size = EndOfSrcOffset - SpiBiosRegionEnd;
    NonBiosRegion[0].Dst = (UINTN)DstAddress + BufferSize - NonBiosRegion[0].Size;

    CopyMem (DstAddress, (UINT8 *) (PcdGet32 (PcdFlashAreaBaseAddress) + (SrcOffset - SpiBiosRegionStart)), BufferSize - NonBiosRegion[0].Size);

  } else {
    //
    // BIOS region is covered in read region
    //
    NonBiosRegion[0].Src = SrcOffset;
    NonBiosRegion[0].Dst = (UINTN)DstAddress;
    NonBiosRegion[0].Size = SpiBiosRegionStart - SrcOffset;
    NonBiosRegion[1].Src = SpiBiosRegionEnd + 1;
    NonBiosRegion[1].Size = EndOfSrcOffset - SpiBiosRegionEnd;
    NonBiosRegion[1].Dst = (UINTN)DstAddress + BufferSize - NonBiosRegion[1].Size;

    CopyMem ((UINT8 *) ((UINTN) DstAddress + NonBiosRegion[0].Size), (UINT8 *) (UINTN) PcdGet32 (PcdFlashAreaBaseAddress), (UINTN) PcdGet32 (PcdFlashAreaSize));
  }

  return EFI_SUCCESS;
}
#endif
//[-end-120524-IB06710088-add]//
EFI_STATUS
SpiReadConfiguration (
  IN  BOOLEAN     CacheEnable
  )
{
  STATIC UINT8        BiosCtlSave = 0;


  if (CacheEnable) {
     BiosCtlSave = (PchLpcPciCfg8(R_PCH_LPC_BIOS_CNTL) & B_PCH_LPC_BIOS_CNTL_SRC);
//[-start-120419-IB10820037-modify]//
//[-start-120905-IB10870021-modify]//
     PchLpcPciCfg8AndThenOr ((UINT8)R_PCH_LPC_BIOS_CNTL,(UINT8) ~B_PCH_LPC_BIOS_CNTL_SRC,(UINT8) (0x02 << 2));
//[-end-1200905-IB10870021-modify]//
//[-end-120419-IB10820037-modify]//
  } else {
//[-start-120419-IB10820037-modify]//
//[-start-120905-IB10870021-modify]//
    PchLpcPciCfg8AndThenOr ((UINT8)R_PCH_LPC_BIOS_CNTL,(UINT8) ~B_PCH_LPC_BIOS_CNTL_SRC, (UINT8)BiosCtlSave);
//[-end-120905-IB10870021-modify]//
//[-end-120419-IB10820037-modify]//
  }

  return EFI_SUCCESS;
}

/**
  Check the address is fall in valid region or not

  @param DstAddress             Target address to be validated
  @param Size                   Region size

  @retval TRUE                  The specified region is valid
  @retval FALSE                 The specified region is invalid

**/
BOOLEAN
CheckValidRegion(
  IN EFI_PHYSICAL_ADDRESS       DstAddress,
  IN UINTN                      Size
  )
{
  return (BOOLEAN)(DstAddress >= mValidWriteRegionStart && DstAddress + Size - 1 <= mValidWriteRegionEnd);
}

/**
  Wait for SPI device not busy

  @param Type                   SPI_SOFTWARE : 0
                                SPI_HARDWARE : 1

  @retval EFI_SUCCESS           Function successfully returned

**/
EFI_STATUS
WaitForSpiDeviceNotBusy(
  IN BOOLEAN                    Type
  )
{
  volatile  UINT16   SpiStatus;

  if (Type == SPI_SOFTWARE)
  {
    //
    // Wait until "SPI Cycle In Progress" bit is 0
    //
    do
    {
      SpiStatus = MmSpi16(R_PCH_SPI_SSFS);
    }
    while ((SpiStatus & B_PCH_SPI_SSFS_SCIP) == B_PCH_SPI_SSFS_SCIP);
    //
    // Clear status bits
    //
    MmSpi16Or(R_PCH_SPI_SSFS, B_PCH_SPI_SSFS_AEL | B_PCH_SPI_SSFS_FCERR | B_PCH_SPI_SSFS_CDS);
  }
  else
  {
    //
    // Wait until "SPI Cycle In Progress" bit is 0
    //
    do
    {
      SpiStatus = MmSpi16(R_PCH_SPI_HSFS);
    }
    while ((SpiStatus & B_PCH_SPI_HSFS_SCIP) == B_PCH_SPI_HSFS_SCIP);
    //
    // Clear status bits
    //
    MmSpi16Or(R_PCH_SPI_HSFS, B_PCH_SPI_HSFS_AEL | B_PCH_SPI_HSFS_FCERR | B_PCH_SPI_HSFS_FDONE);
  }
  return EFI_SUCCESS;
}

/**
  Wait for SPI cycle completed

  @param Type                   SPI_SOFTWARE : 0
                                SPI_HARDWARE : 1

  @retval EFI_SUCCESS           Function successfully returned

**/
EFI_STATUS
WaitForSpiCycleComplete(
  IN BOOLEAN                    Type
  )
{
  volatile  UINT16   SpiStatus;

  if (Type == SPI_SOFTWARE)
  {
    //
    // Wait until "Cycle Done Status" bit is 1
    //
    do {
      SpiStatus = MmSpi16(R_PCH_SPI_SSFS);
    } while ((SpiStatus & B_PCH_SPI_SSFS_CDS) != B_PCH_SPI_SSFS_CDS && !(SpiStatus & B_PCH_SPI_SSFS_FCERR));
    //
    // Clear status bits
    //
    MmSpi16Or(R_PCH_SPI_SSFS, B_PCH_SPI_SSFS_AEL | B_PCH_SPI_SSFS_FCERR | B_PCH_SPI_SSFS_CDS);
    //
    // Check for errors
    //
    if ((SpiStatus & (B_PCH_SPI_SSFS_AEL | B_PCH_SPI_SSFS_FCERR)) != 0) return EFI_DEVICE_ERROR;

    } else {
    //
    // Wait until "Flash Cycle Done" bit is 1
    //
    do {
      SpiStatus = MmSpi16(R_PCH_SPI_HSFS);
    } while ((SpiStatus & B_PCH_SPI_HSFS_FDONE) != B_PCH_SPI_HSFS_FDONE && !(SpiStatus & B_PCH_SPI_HSFS_FCERR));
    //
    // Clear status bits
    //
    MmSpi16Or(R_PCH_SPI_HSFS, B_PCH_SPI_HSFS_AEL | B_PCH_SPI_HSFS_FCERR | B_PCH_SPI_HSFS_FDONE);
    //
    // Check for errors
    //
    if ((SpiStatus & (B_PCH_SPI_HSFS_AEL | B_PCH_SPI_HSFS_FCERR)) != 0) return EFI_DEVICE_ERROR;
  }
  return EFI_SUCCESS;
}

/**
  Starts an SPI transaction

  @param Type                   SPI_SOFTWARE : 0
                                SPI_HARDWARE : 1
  @param Address                SPIA
  @param Length                 SPIS bit 13:8
  @param OpcodePtr              SPIS bit 6:4
  @param PrefixPtr              SPIS bit 3
  @param Atomic                 SPIS bit 2
  @param Data                   SPIS bit 14

  @retval EFI_SUCCESS           Function successfully returned

**/
EFI_STATUS
SpiTransfer(
  IN  BOOLEAN  Type,
  IN  UINT8   *Address,
  IN  UINT8    Length,
  IN  UINT8    OpcodePtr,
  IN  BOOLEAN  PrefixPtr,
  IN  BOOLEAN  Atomic,
  IN  BOOLEAN  Data
  )
{
  WaitForSpiDeviceNotBusy(Type);

  if (Type == SPI_SOFTWARE)
  {
    MmSpi32(R_PCH_SPI_FADDR) = (UINT32)(UINTN)Address;
    MmSpi16AndThenOr(R_PCH_SPI_SSFC, ~B_PCH_SPI_SSFC_DBC_MASK, (Length << 8));
    MmSpi16AndThenOr(R_PCH_SPI_SSFC, ~B_PCH_SPI_SSFC_COP, (OpcodePtr << 4));
    MmSpi16AndThenOr(R_PCH_SPI_SSFC, ~B_PCH_SPI_SSFC_SPOP, (PrefixPtr ? B_PCH_SPI_SSFC_SPOP:0));
    MmSpi16AndThenOr(R_PCH_SPI_SSFC, ~B_PCH_SPI_SSFC_ACS, (Atomic ? B_PCH_SPI_SSFC_ACS:0));
    MmSpi16AndThenOr(R_PCH_SPI_SSFC, ~B_PCH_SPI_SSFC_DS, (Data ? B_PCH_SPI_SSFC_DS:0));
    MmSpi16Or(R_PCH_SPI_SSFC, B_PCH_SPI_SSFC_SCGO);
  }
  else
  {
    MmSpi32(R_PCH_SPI_FADDR) = (UINT32)(UINTN)Address;
    MmSpi16AndThenOr(R_PCH_SPI_HSFC, ~B_PCH_SPI_HSFC_FDBC_MASK, Length << 8);
    MmSpi16AndThenOr(R_PCH_SPI_HSFC, ~B_PCH_SPI_HSFC_FCYCLE_MASK, OpcodePtr << 1);
    MmSpi16Or(R_PCH_SPI_HSFC, B_PCH_SPI_HSFC_FCYCLE_FGO);
  }

  return WaitForSpiCycleComplete(Type);
}

//[-start-130416-IB10820284-remove]//
//EFI_STATUS
//LocatePfatProtocol (
//  IN CONST EFI_GUID                     *Protocol,
//  IN VOID                               *Interface,
//  IN EFI_HANDLE                         Handle
//  )
//{
//  EFI_STATUS  Status;
//
//  if (mPfatProtocol != NULL) {
//    return EFI_SUCCESS;
//  }
//
//  Status = mSmst2->SmmLocateProtocol (
//                     &gSmmPfatProtocolGuid,
//                     NULL,
//                     (VOID **)&mPfatProtocol
//                     );
//  if (EFI_ERROR (Status)) {
//    mPfatProtocol = NULL;
//  } else {
//    mPfatProtocolInitialed = TRUE;
//  }
//
//  return EFI_SUCCESS;
//}
//[-end-130416-IB10820284-remove]//

/**
  Udates the block lock register for the FW block indicated by the input parameters

  @param BaseAddress            Physical address within the flash's address space
  @param Size                   Size in bytes
  @param LockState              FULL_ACCESS                         Full access
                                WRITE_LOCK                          Write Locked
                                LOCK_DOWN                           Locked open (full access locked down).
                                LOCK_DOWN + WRITE_LOCK              Write-locked down.
                                READ_LOCK                           Read locked.
                                READ_LOCK + WRITE_LOCK              Read and write locked.
                                READ_LOCK + LOCK_DOWN               Read-locked down.
                                READ_LOCK + LOCK_DOWN + WRITE_LOCK  Read- and write-locked down.

  @retval EFI_SUCCESS           Function successfully returned

**/
EFI_STATUS
SpiFlashLock (
  IN  UINT8   *BaseAddress,
  IN  UINTN   Size,
  IN  UINT8   LockState
  )
{
//[-start-120417-IB05300312-add]//
  EFI_STATUS          Status;
//[-end-120417-IB05300312-add]//
  UINTN  Index;
  UINTN  BaseAddr;

//[-start-120417-IB05300312-add]//
  if (FeaturePcdGet (PcdPfatSupport)) {
    if (mPfatProtocol != NULL) {
      Status = EFI_SUCCESS;
      if (LockState == SPI_WRITE_LOCK) {
        Status = mPfatProtocol->Execute (
                                  mPfatProtocol,
                                  FALSE
                                  );
      }
      return Status;
    }
  }
//[-end-120417-IB05300312-add]//

  switch (LockState)
  {
    case SPI_WRITE_LOCK:
      if (mSpiConfigBlock.GlobalProtect && mSpiDeviceType == SPI_SOFTWARE)
      {
        // Write to SPI Device Status Register
        MmSpi8(R_PCH_SPI_FDATA00) = mSpiConfigBlock.GlobalProtectCode;   // set block protection bits
        SpiTransfer(
          SPI_SOFTWARE,
          0,
          0,
          SPI_OPCODE_INDEX_WRITE_S,
          SPI_PREFIX_INDEX_WRITE_S_EN,
          1,
          1);
      }
      if (mSpiConfigBlock.BlockProtect)
      {
        BaseAddr = (UINTN)BaseAddress & ~(mSpiConfigBlock.BlockProtectSize - 1);
        MmSpi8(R_PCH_SPI_FDATA00) = mSpiConfigBlock.BlockProtectCode;
      	for (Index = 0; Index < Size; Index += mSpiConfigBlock.BlockProtectSize)
      	{
          SpiTransfer(
            SPI_SOFTWARE,
            (UINT8*)BaseAddr + Index,
            0,
            SPI_OPCODE_INDEX_LOCK,
            SPI_PREFIX_INDEX_WRITE_EN,
            1,
            (BOOLEAN)mSpiConfigBlock.BlockProtectDataRequired);
        }
      }
      break;
    case SPI_FULL_ACCESS:
      if (mSpiConfigBlock.GlobalProtect && mSpiDeviceType == SPI_SOFTWARE)
      {


        // Write to SPI Device Status Register
        MmSpi8(R_PCH_SPI_FDATA00) = mSpiConfigBlock.GlobalUnprotectCode;  // clear block protection bits
        SpiTransfer(
          SPI_SOFTWARE,
          0,
          0,
          SPI_OPCODE_INDEX_WRITE_S,
          SPI_PREFIX_INDEX_WRITE_S_EN,
          1,
          1);
      }

      if (mSpiConfigBlock.BlockProtect)
      {
        BaseAddr = (UINTN)BaseAddress & ~(mSpiConfigBlock.BlockProtectSize - 1);
      	MmSpi8(R_PCH_SPI_FDATA00) = mSpiConfigBlock.BlockUnprotectCode;
      	for (Index = 0; Index < Size; Index += mSpiConfigBlock.BlockProtectSize)
      	{

          SpiTransfer(
            SPI_SOFTWARE,
            (UINT8*)BaseAddr + Index,
            0,
            SPI_OPCODE_INDEX_UNLOCK,
            SPI_PREFIX_INDEX_WRITE_EN,
            1,
            (BOOLEAN)mSpiConfigBlock.BlockProtectDataRequired);
        }
      }
      break;
    }


  return EFI_SUCCESS;
}

/**
  Get flash number from SPI Descriptor

  @param FlashNumber            Number of SPI flash devices returned

  @retval EFI_SUCCESS           Function successfully returned
  @retval EFI_UNSUPPORTED       The SPI flash is not in Descriptor mode

**/
EFI_STATUS
GetSpiFlashNumber (
  OUT   UINT8    *FlashNumber
  )
{
  UINT32    NcValue;

  if ((MmSpi32(R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FDV) == B_PCH_SPI_HSFS_FDV) { // is descriptor mode
    //
    // Get SPI flash number from descriptor
    //
    MmSpi32(R_PCH_SPI_FDOC) = V_PCH_SPI_FDOC_FDSS_FSDM | R_PCH_SPI_FDBAR_FLASH_MAP0;
    NcValue = (MmSpi32(R_PCH_SPI_FDOD) & B_PCH_SPI_FDBAR_NC) >> 8;
    *FlashNumber = (UINT8)(NcValue + 1);
  } else {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}

/**
  Offset the BIOS address from top of 4G memory address to correct BIOS region
  described in SPI Descriptor

  @param MemoryAddress          BIOS memory mapped address
  @param BiosRegionAddress      Flash ROM start address + BIOS address in flash ROM

  @retval EFI_SUCCESS           Function successfully returned
  @retval EFI_UNSUPPORTED       The SPI flash is not in Descriptor mode

**/
EFI_STATUS
MemoryToBiosRegionAddress (
  IN UINTN       MemoryAddress,
  OUT UINTN      *BiosRegionAddress
  )
{
  UINTN   Offset;
  UINTN   BiosRegionStart;
  UINTN   RomStart;
  
//[-start-140103-IB05160535-modify]//
//[-start-130808-IB10300050-add]//
  if (mIhisiFlash == CHECK_FLASH_ACCESS_DO_NOT_CONVERT) {
    return EFI_UNSUPPORTED;
  }
//[-end-130808-IB10300050-add]//
//[-end-140103-IB05160535-modify]//

  if (((MmSpi32(R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FDV) == B_PCH_SPI_HSFS_FDV) &&
      ((UINTN)MemoryAddress >= (UINTN)mFlashBase)) {
    //
    // the data offset in BIOS region
    //
    Offset = MemoryAddress - (UINTN)mFlashBase;
    BiosRegionStart = (UINTN) (((MmSpi32(R_PCH_SPI_FREG1_BIOS) & 0x1FFF) << 12));

    RomStart = (UINTN)(0x100000000 - (UINT64)mSpiConfigBlock.DeviceSize);
    *BiosRegionAddress = RomStart + BiosRegionStart + Offset;
  } else {
    return EFI_UNSUPPORTED;
  }

  return EFI_SUCCESS;
}


/**
  Update data structures related to Spi device size base on Descriptor

  @param SpiFlashNumber         Number of SPI devices in system.

  @param FdInfo                 pointer to FD_INFO of FLASH_SPI_DEVICE,
                                if COMMON SPI DEVICE is used, the contents
                                related to flash size is updated here

  @param SpiCfgBlk              pointer to SPI_CONFIG_BLOCK of FLASH_SPI_DEVICE,
                                if COMMON SPI DEVICE is used, the contents
                                related to flash size is updated here

  @retval EFI_SUCCESS           Function successfully returned
  @retval EFI_DEVICE_ERROR      Failed to update SPI device size

**/
EFI_STATUS
UpdateSpiDeviceSize(
  IN     UINT8            SpiFlashNumber,
  IN OUT FD_INFO          *FdInfo,
  IN OUT SPI_CONFIG_BLOCK *SpiCfgBlk
  )
{
  EFI_STATUS Status;
  UINT8      FlashCompDensity12;
  UINT32     FlashSize1;
  UINT32     FlashSize2;
  UINT8      CommonSpiDeviceSize;

  Status              = EFI_SUCCESS;
  FlashSize1          = 0;
  FlashSize2          = 0;
  CommonSpiDeviceSize = 0xff;

  MmSpi32 (R_PCH_SPI_FDOC) = V_PCH_SPI_FDOC_FDSS_COMP | R_PCH_SPI_FCBA_FLCOMP;
  FlashCompDensity12 = (MmSpi32 (R_PCH_SPI_FDOD) & B_PCH_SPI_FDBAR_FCBA);

  switch (FlashCompDensity12 & B_PCH_SPI_FLCOMP_COMP1_MASK) {
  case V_PCH_SPI_FLCOMP_COMP1_512KB:
    FlashSize1 = 0x80000;
    break;
  case V_PCH_SPI_FLCOMP_COMP1_1MB:
    FlashSize1 = 0x100000;
    break;
  case V_PCH_SPI_FLCOMP_COMP1_2MB:
    FlashSize1 = 0x200000;
    break;
  case V_PCH_SPI_FLCOMP_COMP1_4MB:
    FlashSize1 = 0x400000;
    break;
  case V_PCH_SPI_FLCOMP_COMP1_8MB:
    FlashSize1 = 0x800000;
    break;
  case V_PCH_SPI_FLCOMP_COMP1_16MB:
    FlashSize1 = 0x1000000;
    break;
  default:
    Status = EFI_DEVICE_ERROR;
    break;
  }

  if (SpiFlashNumber == 2) {
    switch (FlashCompDensity12 & B_PCH_SPI_FLCOMP_COMP2_MASK) {
    case V_PCH_SPI_FLCOMP_COMP2_512KB:
      FlashSize2 = 0x80000;
      break;
    case V_PCH_SPI_FLCOMP_COMP2_1MB:
      FlashSize2 = 0x100000;
      break;
    case V_PCH_SPI_FLCOMP_COMP2_2MB:
      FlashSize2 = 0x200000;
      break;
    case V_PCH_SPI_FLCOMP_COMP2_4MB:
      FlashSize2 = 0x400000;
      break;
    case V_PCH_SPI_FLCOMP_COMP2_8MB:
      FlashSize2 = 0x800000;
      break;
    case V_PCH_SPI_FLCOMP_COMP2_16MB:
      FlashSize2 = 0x1000000;
      break;
    default:
      Status = EFI_DEVICE_ERROR;
      break;
    }
  }
  mSpiConfigBlock.DeviceSize = FlashSize1 + FlashSize2;
  if (FdInfo->Id == ID_COMMON_SPI_DEVICE) {
    //
    // BUGBUG: Now, assume Common SPI device size to FlashSize1.
    // BUGBUG: To be modified if ROM part of different size is supported
    //
    SpiCfgBlk->DeviceSize = FlashSize1;

    switch (FlashSize1) {
    case 0x20000:
      CommonSpiDeviceSize = FLASH_SIZE_128K;
      break;
    case 0x40000:
      CommonSpiDeviceSize = FLASH_SIZE_256K;
      break;
    case 0x80000:
      CommonSpiDeviceSize = FLASH_SIZE_512K;
      break;
    case 0x100000:
      CommonSpiDeviceSize = FLASH_SIZE_1024K;
      break;
    case 0x200000:
      CommonSpiDeviceSize = FLASH_SIZE_2048K;
      break;
    case 0x400000:
      CommonSpiDeviceSize = FLASH_SIZE_4096K;
      break;
    case 0x800000:
      CommonSpiDeviceSize = FLASH_SIZE_8192K;
      break;
    case 0x1000000:
      CommonSpiDeviceSize = FLASH_SIZE_16384K;
      break;
    }
    //
    // BlockSize is 0x100, so multiply 0x100 here
    //
    FdInfo->BlockMap.Mutiple = (UINT16)(FlashSize1 / (FdInfo->BlockMap.BlockSize * 0x100));
    FdInfo->Size = CommonSpiDeviceSize;
  }
  return Status;
}

/**
  Detect and Initialize SPI flash part OpCode and other parameter through PCH

  @param FlashDevice            pointer to FLASH_DEVICE structure

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to Recoginize the SPI device

**/
EFI_STATUS
EFIAPI
SpiRecognize (
  IN FLASH_DEVICE                       *FlashDevice
  )
{
  UINT32                  BiosAccessState;
  UINT32                  ValidWriteRegionStart;
  UINT32                  ValidWriteRegionEnd;
  UINT32                  ValidReadRegionStart;
  UINT32                  ValidReadRegionEnd;
  UINT8                   EraseSizeCode;
  SPI_CONFIG_BLOCK        *SpiConfigBlock;
  UINT32                  Vscc;
  UINT8                   SpiFlashNumber;
  EFI_STATUS              Status;
  UINTN                   Index;
  UINT32                  SpiRegister[] = {
                            R_PCH_SPI_SSFS,
                            R_PCH_SPI_PREOP,
                            R_PCH_SPI_OPMENU,
                            R_PCH_SPI_OPMENU + 4,
//[-start-120419-IB10820037-modify]//
                            R_PCH_SPI_VSCC0,
                            R_PCH_SPI_VSCC1
//[-end-120419-IB10820037-modify]//
                            };
  FD_INFO                 *FdInfo;

  Index         = 0;
  EraseSizeCode = 0;

  SpiConfigBlock = (SPI_CONFIG_BLOCK *)FlashDevice->TypeSpecificInfo;
  FdInfo = &FlashDevice->DeviceInfo;
  MmSpi32(R_PCH_SPI_OPMENU + 0) = *(UINT32*)&SpiConfigBlock->OpCodeMenu[0];
  MmSpi32(R_PCH_SPI_OPMENU + 4) = *(UINT32*)&SpiConfigBlock->OpCodeMenu[4];
  MmSpi16(R_PCH_SPI_OPTYPE) = SpiConfigBlock->OpType;
  MmSpi16(R_PCH_SPI_PREOP) = *(UINT16*)SpiConfigBlock->PrefixMenu;
  switch(SpiConfigBlock->BlockEraseSize)
  {
    case 0x100:
      EraseSizeCode = 0;
      break;
    case 0x1000:
      EraseSizeCode = 1;
      break;
    case 0x10000:
      EraseSizeCode = 3;
      break;
  }
  Vscc = (SpiConfigBlock->OpCodeMenu[SPI_OPCODE_INDEX_ERASE] << 8) |
         (SpiConfigBlock->GlobalProtect << 3) |
         (SpiConfigBlock->ProgramGranularity << 2) |
         (EraseSizeCode);

   if (!SpiConfigBlock->NVStatusBit) {
      if ((SpiConfigBlock->PrefixMenu[SPI_PREFIX_INDEX_WRITE_EN] == 0x06) ||
      	  (SpiConfigBlock->PrefixMenu[SPI_PREFIX_INDEX_WRITE_S_EN] == 0x06)) {
//[-start-120419-IB10820037-modify]//
        Vscc = Vscc | (UINT32) (B_PCH_SPI_VSCC0_WEWS);
//[-end-120419-IB10820037-modify]//
      }
      else if ((SpiConfigBlock->PrefixMenu[SPI_PREFIX_INDEX_WRITE_EN]== 0x50) ||
      	      (SpiConfigBlock->PrefixMenu[SPI_PREFIX_INDEX_WRITE_S_EN]== 0x50)) {
//[-start-120419-IB10820037-modify]//
        Vscc = Vscc | (UINT32) ( B_PCH_SPI_VSCC0_WSR);
//[-end-120419-IB10820037-modify]//
      }
   }
//[-start-120419-IB10820037-modify]//
  MmSpi32(R_PCH_SPI_VSCC0) = Vscc;
  MmSpi32(R_PCH_SPI_VSCC1) = Vscc;
//[-end-120419-IB10820037-modify]//
  //
  // Read device ID
  //
  if (FlashDevice->DeviceInfo.Id != ID_COMMON_SPI_DEVICE) {
    MmSpi32(R_PCH_SPI_FDATA00) = 0;
    SpiTransfer(
      SPI_SOFTWARE,
      0,
      (UINT8)SpiConfigBlock->FlashIDSize - 1,
      SPI_OPCODE_INDEX_READ_ID,
      0,
      1,
      1);


    if (MmSpi32(R_PCH_SPI_FDATA00) != FlashDevice->DeviceInfo.Id)
    {
      return EFI_UNSUPPORTED;
    }
  }

  if (!AtRuntime()) {
    SpiReadConfiguration (FALSE);
  }
  //
  //check is whether mutiple flash parts
  //
  CopyMem (&mSpiConfigBlock, FlashDevice->TypeSpecificInfo, sizeof (SPI_CONFIG_BLOCK));


  Status = GetSpiFlashNumber (&SpiFlashNumber);
  if (!EFI_ERROR (Status)) {
    //
    // Get SPI device number from SPI descriptor success
    //
    Status = UpdateSpiDeviceSize (SpiFlashNumber, FdInfo, SpiConfigBlock);
    ASSERT_EFI_ERROR (Status);
  }


  mValidWriteRegionStart = 0;
  mValidWriteRegionEnd = mSpiConfigBlock.DeviceSize - 1;
  mValidReadRegionStart = 0;
  mValidReadRegionEnd = mSpiConfigBlock.DeviceSize - 1;

  if ((MmSpi32(R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FDV) == B_PCH_SPI_HSFS_FDV)
  {
    mSpiDeviceType = SPI_HARDWARE;
    //
    // Read FLMSTR1 to get BIOS access right
    //
    MmSpi32(R_PCH_SPI_FDOC) = 0x00003000;
    BiosAccessState = MmSpi32(R_PCH_SPI_FDOD);
    //
    // Check FLMSTR1 write access
    //
     if (!(BiosAccessState & 0x0f000000))
     {
      ValidWriteRegionStart = (MmSpi32(R_PCH_SPI_BFPR) & 0x0fff) << 12;
      ValidWriteRegionEnd = ((MmSpi32(R_PCH_SPI_BFPR) >> 16) & 0x0fff) << 12;
      if (ValidWriteRegionStart < ValidWriteRegionEnd)
      {
      	mValidWriteRegionStart = ValidWriteRegionStart;
        mValidWriteRegionEnd = ValidWriteRegionEnd | 0x0fff;
      }
    }
    //
    // Setup read regions for SPI read command
    //
    ValidReadRegionStart = (MmSpi32(R_PCH_SPI_BFPR) & 0x0fff) << 12;
    ValidReadRegionEnd = ((MmSpi32(R_PCH_SPI_BFPR) >> 16) & 0x0fff) << 12;
    if (ValidReadRegionStart < ValidReadRegionEnd)
    {
      mValidReadRegionStart = ValidReadRegionStart;
      mValidReadRegionEnd = ValidReadRegionEnd | 0x0fff;
    }

  }
  else
  {
    mSpiDeviceType = SPI_SOFTWARE;


  }

  //
  // Save SPI Registers for S3 resume usage
  //
  for ( Index = 0 ; Index < ( sizeof ( SpiRegister ) / sizeof ( UINT32 ) ) ; Index = Index + 1 ) {

    S3BootScriptSaveMemWrite (
      S3BootScriptWidthUint32,
      ( UINTN )( PCH_RCRB_BASE + SpiRegister[Index] ),
      1,
      ( VOID * )( UINTN )( PCH_RCRB_BASE + SpiRegister[Index] )
      );
  }
  return EFI_SUCCESS;
}

/**
  Erase the SPI flash device from LbaWriteAddress through PCH

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param FlashAddress           Target address to be erased
  @param Size                   The size in bytes to be erased

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to erase the target address

**/
EFI_STATUS
EFIAPI
SpiErase(
  IN  FLASH_DEVICE              *FlashDevice,
  IN  UINTN                     FlashAddress,
  IN  UINTN                     Size
  )
{
  EFI_STATUS Status;
  UINTN NumBytes;
  UINTN DstAddress;
  UINTN SpiAddress;

  NumBytes = Size;

  if (GetFlashMode () == FW_DEFAULT_MODE) {
    Status = MemoryToBiosRegionAddress (FlashAddress, &SpiAddress);
    if (Status == EFI_SUCCESS) {
      FlashAddress = SpiAddress;
    }
  }

  //
  // Make sure the flash address alignment on sector/block
  //
  FlashAddress &= ~(mSpiConfigBlock.BlockEraseSize - 1);

  //
  // Calculate device offset
  //
  DstAddress = FlashAddress - (UINTN)(0x100000000 - (UINT64)mSpiConfigBlock.DeviceSize);

  SpiFlashLock ((UINT8*)DstAddress, Size, SPI_FULL_ACCESS);

//[-start-120418-IB05300312-add]//
  if (FeaturePcdGet (PcdPfatSupport)) {
    ASSERT_EFI_ERROR (mPfatProtocol != NULL);
    if (mPfatProtocol != NULL) {
      while (NumBytes > 0) {
        mPfatProtocol->Erase (
                         mPfatProtocol,
                         (UINT32)(UINTN) FlashAddress
                         );
        NumBytes     -= mSpiConfigBlock.BlockEraseSize;
        FlashAddress += mSpiConfigBlock.BlockEraseSize;
      }
      goto Done;
    }
  }
//[-end-120418-IB05300312-add]//

  while ((INTN)NumBytes > 0) {
    if (CheckValidRegion(DstAddress, mSpiConfigBlock.BlockEraseSize)) {
      Status = SpiTransfer (
                 mSpiDeviceType,
                 (UINT8*)DstAddress,
                 0,
                 (mSpiDeviceType == SPI_SOFTWARE) ? SPI_OPCODE_INDEX_ERASE : V_PCH_SPI_HSFC_FCYCLE_ERASE,
                 SPI_PREFIX_INDEX_WRITE_EN,
                 1,
                 0);
      if (EFI_ERROR(Status)) {


        return Status;
      }
    }
    DstAddress += mSpiConfigBlock.BlockEraseSize;
    NumBytes   -= mSpiConfigBlock.BlockEraseSize;
  }

//[-start-120418-IB05300312-add]//
Done:
//[-end-120418-IB05300312-add]//

  SpiFlashLock ((UINT8*)DstAddress, Size, SPI_WRITE_LOCK);

  return EFI_SUCCESS;
}

/**
  Write the SPI flash device with given address and size through PCH

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param FlashAddress           Destination Offset
  @param SrcAddress             Source Offset
  @param SPIBufferSize          The size for programming
  @param LbaWriteAddress        Write Address

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to erase the target address

**/
EFI_STATUS
EFIAPI
SpiProgram (
  IN  FLASH_DEVICE              *FlashDevice,
  IN  UINT8                     *FlashAddress,
  IN  UINT8                     *SrcAddress,
  IN  UINTN                     *SPIBufferSize,
  IN  UINTN                     LbaWriteAddress
  )
{
  EFI_STATUS           Status;
  UINTN                Index;
  UINTN                DstAddress;
  UINTN                BufferSize;
  UINTN                SpiAddress;

  if (GetFlashMode () == FW_DEFAULT_MODE) {
    Status = MemoryToBiosRegionAddress ((UINTN)FlashAddress, &SpiAddress);
    if (Status == EFI_SUCCESS) {
      FlashAddress = (UINT8 *)SpiAddress;
    }
  }

  BufferSize = *SPIBufferSize;


  //
  // Calculate device offset
  //
  DstAddress = (UINTN)FlashAddress - (UINTN)(0x100000000 - (UINT64)mSpiConfigBlock.DeviceSize);

//[-start-120418-IB05300312-modify]//
  if (!CheckValidRegion(DstAddress, BufferSize)) {
    return EFI_INVALID_PARAMETER;
  }
//[-end-120418-IB05300312-modify]//

  SpiFlashLock ((UINT8*)DstAddress, BufferSize, SPI_FULL_ACCESS);

//[-start-120418-IB05300312-add]//
  if (FeaturePcdGet (PcdPfatSupport)) {
    ASSERT_EFI_ERROR (mPfatProtocol != NULL);
    if (mPfatProtocol != NULL) {
      mPfatProtocol->Write (
                     mPfatProtocol,
                     (UINT32)(UINTN) FlashAddress,
                     (UINT32) BufferSize,
                     SrcAddress
                     );
      goto Done;
    }
  }
//[-end-120418-IB05300312-add]//

  if (mSpiDeviceType == SPI_SOFTWARE) {
    for (Index = 0; Index < BufferSize; Index++) {
      MmSpi8(R_PCH_SPI_FDATA00) = SrcAddress[Index];  // Copy 1 byte of data from Buffer to SPI Data block
      Status = SpiTransfer(
                 SPI_SOFTWARE,
                 (UINT8*)(DstAddress + Index),
                 0,
                 SPI_OPCODE_INDEX_WRITE,
                 SPI_PREFIX_INDEX_WRITE_EN,
                 1,
                 1);
      if (EFI_ERROR(Status)) {
        return Status;
      }
    }
  } else {
    UINT8  *NextWriteAddr;
    UINT8  *FinalWriteAddr;
    UINT8  *EndOf64bytePage;
    UINT8  NumBytesThisTransfer;
    UINTN  Index2;
    UINTN  NumBytes;

    Index2 = 0;
    NumBytes = BufferSize;

    NextWriteAddr = (UINT8*)DstAddress;
    FinalWriteAddr = NextWriteAddr + NumBytes - 1;
    while (NextWriteAddr <= FinalWriteAddr) {
      EndOf64bytePage = (UINT8*)((UINTN)(UINTN*)NextWriteAddr & ~0x3F) + 0x40 - 1;
      while ((NextWriteAddr <= FinalWriteAddr) && (NextWriteAddr <= EndOf64bytePage)) {
        // Determine how many bytes to write in the next transfer
        if (NumBytes < ((UINTN)EndOf64bytePage - (UINTN)NextWriteAddr + 1)) {
          NumBytesThisTransfer = (UINT8)NumBytes;
        } else {
          NumBytesThisTransfer = (UINT8)((UINTN)(UINTN*)EndOf64bytePage - (UINTN)(UINTN*)NextWriteAddr + 1);
        }
        for (Index = 0; Index < NumBytesThisTransfer; Index++) MmSpi8(R_PCH_SPI_FDATA00 + Index) = SrcAddress[Index + Index2];
        Status = SpiTransfer(
          SPI_HARDWARE,
          NextWriteAddr,
          NumBytesThisTransfer - 1, // make byte count zero-based
          V_PCH_SPI_HSFC_FCYCLE_WRITE,
          0,
          0,
          0);
        if (EFI_ERROR(Status)) {

          return Status;
        }
        NextWriteAddr += NumBytesThisTransfer;
        Index2 += NumBytesThisTransfer;
        NumBytes -= NumBytesThisTransfer;
      }
    }
  }

//[-start-120418-IB05300312-add]//
Done:
//[-end-120418-IB05300312-add]//

  SpiFlashLock ((UINT8*)DstAddress, BufferSize, SPI_WRITE_LOCK);


  return EFI_SUCCESS;
}

//[-start-120524-IB06710088-modify]//
/**
  Read the SPI flash device with given address and size through PCH

  @param FlashDevice            pointer to FLASH_DEVICE structure
  @param DstAddress             Destination buffer address
  @param FlashAddress           The flash device address to be read
  @param BufferSize             The size to be read

  @retval EFI_SUCCESS           The SPI device was successfully recognized
  @retval EFI_UNSUPPORTED       The flash device is not supported by this function
  @retval EFI_DEVICE_ERROR      Failed to erase the target address

**/
EFI_STATUS
EFIAPI
SpiRead (
  IN  FLASH_DEVICE              *FlashDevice,
  IN  UINT8                     *DstAddress,
  IN  UINT8                     *FlashAddress,
  IN  UINTN                     BufferSize
  )
{
  EFI_STATUS           Status;
  UINTN                Index;
  UINTN                SrcOffset;
  UINTN                PageIndex;
  UINTN                PageNumber = 0;
  UINT8                RemainSize = 0;
  UINTN                SpiAddress;
#if SPI_READ_BY_MEMORY_MAPPED
  SPI_READ_REGION      NonBiosRegion[2];
  UINTN                RegionIndex;

  ZeroMem (&NonBiosRegion, sizeof (NonBiosRegion));
#endif
  SrcOffset = 0;
  if (mSpiDeviceType == SPI_HARDWARE)
  {
    if (GetFlashMode () == FW_DEFAULT_MODE) {
      Status = MemoryToBiosRegionAddress ((UINTN)FlashAddress, &SpiAddress);
      if (Status == EFI_SUCCESS) {
        FlashAddress = (UINT8 *)SpiAddress;
      }
    }
    SrcOffset = (UINTN)(FlashAddress) - (UINTN)(0x100000000 - (UINT64)mSpiConfigBlock.DeviceSize);

#if SPI_READ_BY_MEMORY_MAPPED
    //
    // Read SPI data from memory mapped address
    //
    SeperateAndCopyBiosRegionData (SrcOffset, DstAddress, BufferSize, &NonBiosRegion[0]);

    //
    // Non BIOS region can not be read by memory mapped address, need to be read by sending SPI command to SPI controller.
    //
    for (RegionIndex = 0; RegionIndex < 2 && NonBiosRegion[RegionIndex].Size != 0; RegionIndex++) {
      SrcOffset = NonBiosRegion[RegionIndex].Src;
      DstAddress = (UINT8 *)(NonBiosRegion[RegionIndex].Dst);
      PageNumber = (UINTN) (NonBiosRegion[RegionIndex].Size / SPI_READ_BUFFER_SIZE);
      RemainSize = (UINT8) (NonBiosRegion[RegionIndex].Size % SPI_READ_BUFFER_SIZE);
#else
      //
      // Read SPI data by sending SPI command to SPI controller
      //
      PageNumber = (UINTN) (BufferSize / SPI_READ_BUFFER_SIZE);
      RemainSize = (UINT8) (BufferSize % SPI_READ_BUFFER_SIZE);
#endif

      for (PageIndex = 0; PageIndex < PageNumber; PageIndex ++) {
        Status = SpiTransfer(
              SPI_HARDWARE,
              (UINT8 *)(SrcOffset + (PageIndex * SPI_READ_BUFFER_SIZE)),
              SPI_READ_BUFFER_SIZE - 1, // make byte count zero-based
              V_PCH_SPI_HSFC_FCYCLE_READ,
              0,
              0,
              0);

        if (EFI_ERROR(Status)) {
          return Status;
        }
        for (Index = 0; Index < SPI_READ_BUFFER_SIZE; Index++) {
          DstAddress[(PageIndex * SPI_READ_BUFFER_SIZE) + Index] = PchMmRcrb8(R_PCH_SPI_FDATA00 + Index);
        }
      }

      if (RemainSize != 0) {
        Status = SpiTransfer(
                    SPI_HARDWARE,
                    (UINT8 *)(SrcOffset + (PageNumber * SPI_READ_BUFFER_SIZE)),
                    RemainSize - 1, // make byte count zero-based
                    V_PCH_SPI_HSFC_FCYCLE_READ,
                    0,
                    0,
                    0);

        if (EFI_ERROR(Status)) {

          return Status;
        }
        for (Index = 0; Index < RemainSize; Index++) {
          DstAddress[(PageNumber * SPI_READ_BUFFER_SIZE) + Index] = PchMmRcrb8(R_PCH_SPI_FDATA00 + Index);
        }
      }
#if SPI_READ_BY_MEMORY_MAPPED
    }
#endif
  } else {
    //
    // SPI_SOFTWARE
    //
    CopyMem (DstAddress, FlashAddress, BufferSize);
  }

  return EFI_SUCCESS;
}
//[-end-120524-IB06710088-modify]//
/**
  Check whether the flash region is used or not

  @param FlashRegion            Flash Region x Register (x = 0 - 3)

  @retval TRUE                  The region is used
  @retval FALSE                 The region is not used

**/
BOOLEAN
CheckFlashRegionIsValid (
  IN       UINT32    FlashRegion
  )
{
  BOOLEAN         Flag = TRUE;

  //
  // the Region Base must be programmed to 0x1FFFh and the Region Limit
  // to 0x0000h to disable the region.
  //
  // Now, the tool programmed 0x0fff to base and 0x000 to limit to indicate
  // this is region is not used.
  //

  //
  //The limit block address must be greater than or equal to the base block address
  //
  if ((FlashRegion & 0x1fff) > (FlashRegion >> 16 & 0x1fff)) {
    Flag = FALSE;
  }
  return Flag;
}

/**
  Get flash table from platform

  @param DataBuffer             IN: the input buffer address
                                OUT:the flash region table from rom file

  @retval EFI_SUCCESS           Function successfully returned

**/
EFI_STATUS
GetSpiPlatformFlashTable (
  IN OUT   UINT8    *DataBuffer
  )
{

  UINT8           Index;
  UINT32          FlashRegionReg;
  UINT32          FlashSize;
  FLASH_REGION    *FlashTable;
  UINT32          BootBiosDest;
  UINT32          ReadAccess;
  UINT32          WriteAccess;
  UINT32          Frap;
  BOOLEAN         DescriptorValid;
//[-start-120820-IB10820110-modify]//
  UINT32          CompareBootBiosDest;
//[-end-120820-IB10820110-modify]//

  DescriptorValid = FALSE;

  FlashTable = (FLASH_REGION *)DataBuffer;
//[-start-120728-IB05330366-modify]//
//[-start-120820-IB10820110-modify]//
  if (FeaturePcdGet (PcdUltFlag)) {
    BootBiosDest = PchMmRcrb32 (R_PCH_RCRB_GCS) &B_PCH_LP_RCRB_GCS_BBS;
  } else {
    BootBiosDest = MmSpi32 (R_PCH_RCRB_GCS) &B_PCH_H_RCRB_GCS_BBS;
  }
//[-end-120820-IB10820110-modify]//
//[-end-120728-IB05330366-modify]//

  if ((MmSpi32(R_PCH_SPI_HSFS) & B_PCH_SPI_HSFS_FDV) == B_PCH_SPI_HSFS_FDV) {
    DescriptorValid = TRUE;
  }
   //
   // Check is whether SPI interface and descriptor is valid
   //
//[-start-120728-IB05330366-modify]//
//[-start-120820-IB10820110-modify]//
  if (FeaturePcdGet (PcdUltFlag)) {
    CompareBootBiosDest = V_PCH_LP_RCRB_GCS_BBS_SPI;
  } else {
    CompareBootBiosDest = V_PCH_H_RCRB_GCS_BBS_SPI;
  }
  if ((BootBiosDest == CompareBootBiosDest) && DescriptorValid) {
//[-end-120820-IB10820110-modify]//
//[-end-120728-IB05330366-modify]//
    Frap = MmSpi32(R_PCH_SPI_FRAP);

    for (Index = DescRegionType; Index < MaxFlashRegionType; Index++) {

      FlashRegionReg = MmSpi32(R_PCH_SPI_FREG0_FLASHD + Index * 4);

      if (CheckFlashRegionIsValid (FlashRegionReg)){
        FlashTable->Type = Index;
        FlashTable->Offset = (FlashRegionReg & 0x1fff) << 12;
        FlashSize = ((FlashRegionReg >> 16 & 0x1fff) - (FlashRegionReg & 0x1fff) + 1) << 12;
        FlashTable->Size = FlashSize;
        //
        // We can override  BIOS region, ME region, and GBE regiron the permissions
        // in the Flash Descriptor through BMWAG and BMRAG.
        //
        if (Index >= BiosRegionType && Index <= GbeRegionType) {
          ReadAccess = (Frap >> (BIOS_REGION_READ_ACCESS + Index)) & ACCESS_AVAILABLE;
          ReadAccess |= ((Frap >> (BIOS_MASTER_READ_ACCESS_GRANT + Index)) & ACCESS_AVAILABLE);
          WriteAccess = (Frap >> (BIOS_REGION_WRITE_ACCESS + Index)) & ACCESS_AVAILABLE;
          WriteAccess |= ((Frap >> (BIOS_MASTER_WRITE_ACCESS_GRANT + Index)) & ACCESS_AVAILABLE);
        } else {
          ReadAccess = (Frap >> (BIOS_REGION_READ_ACCESS + Index)) & ACCESS_AVAILABLE;
          WriteAccess = (Frap >> (BIOS_REGION_WRITE_ACCESS + Index)) & ACCESS_AVAILABLE;
        }
        if (ReadAccess == ACCESS_AVAILABLE && WriteAccess == ACCESS_AVAILABLE) {
          FlashTable->Access = ACCESS_AVAILABLE;
        } else {
          FlashTable->Access = ACCESS_NOT_AVAILABLE;
        }

        FlashTable++;
      }
    }
  }


  FlashTable->Type = EndOfRegionType;
  return EFI_SUCCESS;
}



/**
  SpiAccessLib Library Class Constructor

  @retval EFI_SUCCESS           Module initialized successfully
  @retval Others                Module initialization failed

**/
EFI_STATUS
EFIAPI
SpiAccessLibInit (
  VOID
  )
{
//[-start-130416-IB10820284-remove]//
//  EFI_STATUS                  Status;
//  EFI_SMM_BASE2_PROTOCOL      *SmmBase;
//  BOOLEAN                     InSmm;
//  VOID                        *Registration;
//[-end-130416-IB10820284-remove]//
//[-start-140103-IB05160535-remove]//
////[-start-130808-IB10300050-add]//
//  EFI_STATUS                  Status;
//  UINTN                       VarBufferData;
//  UINTN                       VarBufferSize;
////[-end-130808-IB10300050-add]//
//[-end-140103-IB05160535-remove]//

  //
  // Setup the PCH_SPI base address and Flash base address for both of PostTime and RunTime phase
  //
  mRcrbSpiBase = (UINT8*)(UINTN)(PCH_RCRB_BASE + R_PCH_RCRB_SPI_BASE);
  mFlashBase   = (VOID *)(UINTN)PcdGet32(PcdFlashAreaBaseAddress);
  SpiAccessInit();
  
//[-start-140103-IB05160535-modify]//
//[-start-130808-IB10300050-add]//
//  //
//  // Get mail box variable
//  //
//  VarBufferSize = sizeof (UINTN);
//  Status = gRT->GetVariable(
//                  CHECK_FLASH_ACCESS_VARIABLE, 
//                  &gCheckFlashAccessGuid, 
//                  NULL, 
//                  &VarBufferSize, 
//                  &VarBufferData
//                  );
//                  
//  if (!EFI_ERROR(Status)) {
//    //
//    // Mail box is allocated, use it
//    //
//    mIhisiFlash = (UINT32 *)VarBufferData;
//  
//  } else {
//    //
//    // Mail box is not allocated, create it
//    //
//    Status = gBS->AllocatePool(
//                    EfiReservedMemoryType,
//                    sizeof (UINT32),
//                    (VOID **)&mIhisiFlash
//                    );
//    
//    //
//    // Initialize it to convert address and set mail box address to variable
//    //
  mIhisiFlash = CHECK_FLASH_ACCESS_CONVERT;
//    VarBufferData = (UINTN)mIhisiFlash;
//    VarBufferSize = sizeof (UINTN);
//    Status = gRT->SetVariable(
//                    CHECK_FLASH_ACCESS_VARIABLE, 
//                    &gCheckFlashAccessGuid, 
//                    EFI_VARIABLE_BOOTSERVICE_ACCESS,
//                    VarBufferSize,
//                    &VarBufferData
//                    );
//  }
//[-end-130808-IB10300050-add]//
//[-end-140103-IB05160535-modify]//

//[-start-130416-IB10820284-remove]//
//  if (FeaturePcdGet (PcdPfatSupport)) {
//    Status = gBS->LocateProtocol (
//                    &gEfiSmmBase2ProtocolGuid,
//                    NULL,
//                    (VOID **)&SmmBase
//                    );
//    InSmm = FALSE;
//    if (!EFI_ERROR (Status)) {
//      SmmBase->InSmm (SmmBase, &InSmm);
//    }
//
//    if (InSmm) {
//      //
//      // Get Smm Syatem Table
//      //
//      Status = SmmBase->GetSmstLocation(
//                          SmmBase,
//                          &mSmst2
//                          );
//      if (!EFI_ERROR (Status)) {
//        //
//        // Try to locate Smm Pfat protocol, if locate fail -> register notify for it
//        //
//        Status = mSmst2->SmmLocateProtocol (
//                        &gSmmPfatProtocolGuid,
//                        NULL,
//                        (VOID **)&mPfatProtocol
//                        );
//        if (EFI_ERROR (Status)) {
//            mPfatProtocol = NULL;
//            //
//            // Register Notify Event for LocatePfatProtocol
//            //
//            Status = mSmst2->SmmRegisterProtocolNotify (
//                               &gSmmPfatProtocolGuid,
//                               LocatePfatProtocol,
//                               &Registration
//                               );
//        }
//      }
//    }
//  }
//[-end-130416-IB10820284-remove]//
  
  IrsiAddVirtualPointer((VOID **)&mFlashBase);
  IrsiAddVirtualPointer((VOID **)&mRcrbSpiBase);

  return EFI_SUCCESS;
}


/**
  If a  driver exits with an error, it must call this routine
  to free the allocated resource before the exiting.

  @param[in] ImageHandle    The firmware allocated handle for the EFI image.
  @param[in] SystemTable    A pointer to the EFI System Table.

  @retval  EFI_SUCCESS      The Driver Lib shutdown successfully.
**/
EFI_STATUS
EFIAPI
SpiAccessLibDestruct (
  VOID
  )
{
  SpiAccessDestroy ();
  IrsiRemoveVirtualPointer ((VOID **)&mFlashBase);
  IrsiRemoveVirtualPointer ((VOID **)&mRcrbSpiBase);
  return EFI_SUCCESS;
}
