/** @file
  Definition for Smm OEM Services Chipset Lib.

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

#ifndef _SMM_OEM_SVC_CHIPSET_LIB_H_
#define _SMM_OEM_SVC_CHIPSET_LIB_H_
//[-start-121113-IB10820163-add]//
#include <Uefi.h>
#include <Protocol/LegacyBios.h>
//[-end-121113-IB10820163-add]//
//[-start-130125-IB10820224-add]//
#include <ChipsetIhisi.h>
//[-end-130125-IB10820224-add]//

#ifndef _DEVICE_REGISTER_TABLE_H
typedef struct {
  UINTN               RegNum;
  UINT8               PciBus;
  UINT8               PciDev;
  UINT8               PciFun;
  UINT8               *PciRegTable;
  UINT32              *PciRegTableSave;
} SR_DEVICE;

typedef struct {
  UINT8               Bus;
  UINT8               Dev;
  UINT8               Fun;
} P2P_BRIDGE;

typedef struct {
  SR_DEVICE           Device;
  P2P_BRIDGE          P2PB;
} SR_OEM_DEVICE;
#endif

/**
 This function provides OEM to add additional PCI registers which is not listed 
 on global register table PciDeviceSubResList [].It will replace global register
 table PciDeviceOemSubResList[].

 Boot to OS/"S3 resume", call this function to get OEM PCI register table to 
 save/restore OEM PCI register Value. 

 @param[out]        *PciDeviceOemList   Points to the list of OEM PCI device registers which must be saved on an S3.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
*/
EFI_STATUS
OemSvcGetSaveRestorePciDeviceOemList (
  OUT SR_OEM_DEVICE             **PciDeviceOemList
  );
//[-start-121113-IB10820163-add]//
/**
 This function provides an interface to get OemInt15VbiosFunctionHook array.
 Caller will use this to install additional VbiosHookCallBack function.

 @param[out]        *OemInt15VbiosFunctionlist  Point to OemInt15VbiosFunctionHook array.
 @param[out]        *Size               The number of entries in OemInt15VbiosFunctionHook array.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
*/
EFI_STATUS
GetOemInt15VbiosFunctionlist (
  IN OUT UINT16                          **OemInt15VbiosFunctionlist,
  OUT UINT16                             *Size
  );
/**
 This function provides an interface to do additional VbiosHookCallBack function that are list on 
 OemInt15VbiosFunctionHook array.

 @param[in]         Int15FunNum         Int15 function number.
 @param[in out]     CpuRegs             The structure containing CPU Registers (AX, BX, CX, DX etc.).
 @param[in]         Context             Context.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcVbiosHookCallBack (
  IN UINT32                             Int15FunNum,
  IN OUT EFI_IA32_REGISTER_SET          *CpuRegs,
  IN VOID                               *Context
  );
//[-end-121113-IB10820163-add]//

//[-start-130125-IB10820224-add]//
/**
 This function offers an interface to Hook IHISI Sub function AH=10h,function "GetACStatus"

 @param[in, out]    AcStatus            On entry, pointer to AcStatus Address.
                                        On exit, points to updated AcStatus Address.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS10HookGetAcStatus (
  IN OUT UINT8      *AcStatus
  );

/**
 This function offers an interface to Hook IHISI Sub function AH=10h,function "GetBattLife"

 @param[in, out]    BattLife            On entry, pointer to BattLife Address.
                                        On exit, points to updated BattLife Address.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS10HookGetBatterylife (
  IN OUT UINT8      *BattLife
  );

/**
 This function offers an interface to Hook IHISI Sub function AH=10h,function "GetPermission"

 @param[in, out]    Permission          On entry, pointer to flash Permission Address.
                                        On exit, points to updated flash Permission Address.


 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS10HookGetPermission (
  IN OUT UINT16     *Permission
  );

/**
 This function offers an interface to Hook IHISI Sub function AH=11h,function "FbtsAPCheck"

 @param[in, out]    ApStatus            On entry, pointer to ApStatus Address.
                                        On exit, points to updated ApStatus Address.


 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS11HookFbtsApCheck (
  IN OUT UINT8      *ApStatus
  );

/**
 This function offers an interface to Hook IHISI Sub function AH=12h,function "GetOemFlashMap"

 @param[in out]     *Media_mOemRomMapSize         On entry, the size of FBTS_PLATFORM_ROM_MAP matrix.
                                                  On exit , the size of updated FBTS_PLATFORM_ROM_MAP matrix.
 @param[in out]     **Media_mOemRomMap            On entry, points to FBTS_PLATFORM_ROM_MAP matrix.
                                                  On exit , points to updated FBTS_PLATFORM_ROM_MAP matrix.
 @param[in out]     *Media_mOemPrivateRomMapSize  On entry, the size of FBTS_PLATFORM_PRIVATE_ROM matrix.
                                                  On exit , the size of updated FBTS_PLATFORM_PRIVATE_ROM matrix.
 @param[in out]     **Media_mOemPrivateRomMap     On entry, points to FBTS_PLATFORM_PRIVATE_ROM matrix.
                                                  On exit , points to updated FBTS_PLATFORM_PRIVATE_ROM matrix.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS12HookGetOemFlashMap (
  IN OUT UINTN                          *Media_mOemRomMapSize,
  IN OUT FBTS_PLATFORM_ROM_MAP          **Media_mOemRomMap,
  IN OUT UINTN                          *Media_mOemPrivateRomMapSize,
  IN OUT FBTS_PLATFORM_PRIVATE_ROM      **Media_mOemPrivateRomMap
  );

/**
 This function offers an interface to Hook IHISI Sub function AH=20h,function "FetsDoBeforeFlashing"
 Hook function before flashing EC part.
 
 @param[in, out]    FlashingDataBuffer  Double pointer to data buffer.
 @param[in, out]    SizeToFlash         Data size by bytes want to flash.
 @param[in, out]    DestBlockNo         Dsstination block number.
 
 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS20HookFetsDoBeforeFlashing (
  IN OUT UINT8      **FlashingDataBuffer,
  IN OUT UINTN      *SizeToFlash,
  IN OUT UINT8      *DestBlockNo
  );

/**
 This function offers an interface to Hook IHISI Sub function AH=20h,function "FetsDoAfterFlashing"
 Hook function after flashing EC part.
 
 @param[in]         ActionAfterFlashing Input action flag.
 
 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS20HookFetsDoAfterFlashing (
  IN UINT8          ActionAfterFlashing
  );

/**
 This function offers an interface to Hook IHISI Sub function AH=20h,function "FetsEcIdle"
 Fets EC idle function
 
 @param[in]         Idle                TRUE, inform EC to enter idle state.
                                        FALSE, wake up EC from idle
 
 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS20HookFetsEcIdle (
  IN BOOLEAN        Idle
  );

/**
 This function offers an interface to Hook IHISI Sub function AH=20h,function "FetsEcFlash"
 Function uses to flash EC part.
 
 @param[in]         FlashingDataBuffer  Double pointer to data buffer.
 @param[in]         SizeToFlash         Data size by bytes want to flash.
 @param[in]         DestBlockNo         Dsstination block number.
 
 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS20HookFetsEcFlash (
  IN UINT8          *FlashingDataBuffer,
  IN UINTN          SizeToFlash,
  IN UINT8          DestBlockNo
  );

/**
 This function offers an interface to Hook IHISI Sub function AH=21h,function "FetsGetPartInfo"

 @param[in, out]    IhisiStatus         On entry, pointer to IhisiStatus Address.
                                        On exit, points to updated IhisiStatus Address.
 @param[in, out]    EcPartSize          On entry, pointer to EcPartSize Address.
                                        On exit, points to updated EcPartSize Address.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS21HookFetsGetPartInfo (
  IN OUT UINT32     *IhisiStatus,
  IN OUT UINT32     *EcPartSize
  );

/**
 This function offers an interface to do IHISI Sub function AH=41h,
 OEM Extra Data Communication type 1h for VBIOS relative function.

 @param[in]         ApCommDataBuffer    Pointer to AP communication data buffer.
 @param[in, out]    BiosCommDataBuffer  On entry, pointer to BIOS communication data buffer.
                                        On exit, points to updated BIOS communication data buffer.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS41T1VbiosFunction (
  IN AP_COMMUNICATION_DATA_TABLE        *ApCommDataBuffer,
  IN OUT BIOS_COMMUNICATION_DATA_TABLE  *BiosCommDataBuffer
  );

/**
 This function offers an interface to do IHISI Sub function AH=41h,
 OEM Extra Data Communication type 54h to update logo.

 @param[in]         ApCommDataBuffer    Pointer to AP communication data buffer.
 @param[in, out]    BiosCommDataBuffer  On entry, pointer to BIOS communication data buffer.
                                        On exit, points to updated BIOS communication data buffer.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS41T54LogoUpdateFunction (
  IN AP_COMMUNICATION_DATA_TABLE        *ApCommDataBuffer,
  IN OUT BIOS_COMMUNICATION_DATA_TABLE  *BiosCommDataBuffer
  );

/**
 This function offers an interface to do IHISI Sub function AH=41h,
 OEM Extra Data Communication type 55h to Check BIOS sign by System BIOS.

 @param[in]         ApCommDataBuffer    Pointer to AP communication data buffer.
 @param[in, out]    BiosCommDataBuffer  On entry, pointer to BIOS communication data buffer.
                                        On exit, points to updated BIOS communication data buffer.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS41T55CheckBiosSignBySystemBiosFunction (
  IN AP_COMMUNICATION_DATA_TABLE        *ApCommDataBuffer,
  IN OUT BIOS_COMMUNICATION_DATA_TABLE  *BiosCommDataBuffer
  );

/**
 This function offers an interface to do IHISI Sub function AH=41h,
 OEM Extra Data Communication type xx are reserved functions.

 @param[in]         ApCommDataBuffer    Pointer to AP communication data buffer.
 @param[in, out]    BiosCommDataBuffer  On entry, pointer to BIOS communication data buffer.
                                        On exit, points to updated BIOS communication data buffer.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS41ReservedFunction (
  IN AP_COMMUNICATION_DATA_TABLE        *ApCommDataBuffer,
  IN OUT BIOS_COMMUNICATION_DATA_TABLE  *BiosCommDataBuffer
  );

/**
 This function offers an interface to do IHISI Sub function AH=41h,
 OEM Extra Data Communication type 50h to read/write OA3.0.

 @param[in]         ApCommDataBuffer    Pointer to AP communication data buffer.
 @param[in, out]    BiosCommDataBuffer  On entry, pointer to BIOS communication data buffer.
                                        On exit, points to updated BIOS communication data buffer.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcIhisiS41T50HookOa30ReadWriteFunction (
  IN AP_COMMUNICATION_DATA_TABLE        *ApCommDataBuffer,
  IN OUT BIOS_COMMUNICATION_DATA_TABLE  *BiosCommDataBuffer
  );
//[-end-130125-IB10820224-add]//
#endif
