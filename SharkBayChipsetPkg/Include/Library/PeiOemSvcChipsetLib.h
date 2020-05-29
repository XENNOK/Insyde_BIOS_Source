/** @file
  Definition for PEI OEM Services Chipset Lib.

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

#ifndef _PEI_OEM_SVC_CHIPSET_LIB_H_
#define _PEI_OEM_SVC_CHIPSET_LIB_H_

#include <Ppi/MePlatformPolicyPei.h>
#include <Ppi/PlatformMeHook.h>
#include <Ppi/PchUsbPolicy.h>
#include <Ppi/AmtPlatformPolicyPei.h>
#include <Ppi/AzaliaPolicy.h>
#include <Ppi/PchPlatformPolicy.h>
#include <Ppi/CpuPlatformPolicy.h>
#include <Ppi/SaPlatformPolicy.h>
//[-start-130326-IB06720210-add]//
#include <MrcOemPlatform.h>
//[-end-130326-IB06720210-add]//
//[-start-140604-IB06720256-add]//
#include <ChipsetGpio.h>
//[-end-140604-IB06720256-add]//

//[-start-140604-IB06720256-remove]//
////[-start-130325-IB05280056-modify]//
//// GPIO_SETTING_TABLE field name is reference to PCH EDS mnemonic
//// Name       | 2-Chip Spec  | ULT Spec
//// GpioUseSel | GPIO_USE_SEL | GPIO_USE_SEL
//// GpioIoSel  | GP_IO_SEL    | GPIO_IO_SEL
//// GpoLvl     | GP_LVL       | GPO_LVL
//// GpioBlink  | GPO_BLINK    | GPO_BLINK
//// GpiInv     | GPI_INV      | GPI_INV
//// GpioOwn    | None         | GPIO_OWN
//// GpiRout    | None         | GPI_ROUT
//// GpiIe      | None         | GPI_IE
//// GpiLxEb    | None         | GPI_LxEB
//// GpiWp      | None         | GPIWP
//// GpinDis    | None         | GPINDIS
//// None value set 0
////
//typedef struct {
//  UINT16 GpioUseSel       : 1;   ///< 0:Native    1:GPIO
//  UINT16 GpioIoSel        : 1;   ///< 0:Output    1:Input
//  UINT16 GpoLvl           : 1;   ///< 0:Low       1:Hign
//  UINT16 GpioBlink        : 1;   ///< 0:Disable   1:Enable
//  UINT16 GpiInv           : 1;   ///< 0:Disable   1:Enable
//  UINT16 GpioOwn          : 1;   ///< 0:GPE01     1:GPI_IS[n]
//  UINT16 GpiRout          : 1;   ///< 0:SCI       1:NMI/SMI
//  UINT16 GpiIe            : 1;   ///< 0:Disable   1:Enable
//  UINT16 GpiLxEb          : 1;   ///< 0:Edge      1:Level
//  UINT16 GpiWp            : 2;   ///< 0:Null      1:pulldown   10:pullup     11:Invaild
//  UINT16 GpinDis          : 1;   ///< 0:Disable   1:Enable
//  UINT16 Reserved         : 4;
//} GPIO_SETTINGS_TABLE;
////[-end-130325-IB05280056-modify]//
//[-end-140604-IB06720256-remove]//

//[-start-121128-IB05280020-add]//
typedef struct {
  UINT8 Register;     ///< SIO reigister for index port
  UINT8 Value;        ///< SIO value for data port
} EFI_SIO_TABLE;

typedef struct {
  UINT8 Register;    ///< GPIO register
  UINT8 Value;       ///< GPIO value
} EFI_SIO_GPIO_TABLE;
//[-end-121128-IB05280020-add]//

//[-start-121219-IB10870058-remove]//
///**
// This Function offers an interface for chipset code to call before it sends Global Reset HECI Message to ME.
//
// @param[in]         None
//
// @retval            EFI_UNSUPPORTED     Returns unsupported by default.
// @retval            EFI_MEDIA_CHANGE    Alter the Configuration Parameter or hook code.
//*/
//EFI_STATUS
//PlatformMeHookPpiPreGlobalReset (
//  VOID
//  );
//[-end-121219-IB10870058-remove]//

//[-start-121224-IB10870061-remove]//
///**
// This function returns current Azalia Docking Attach status.
//
// @param[in, out]    CurrentAzaliaDockingAttachStatus    Azalia Docking Attach status
//
// @retval            EFI_UNSUPPORTED     Returns unsupported by default.
// @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
//*/
//EFI_STATUS
//PchAzaliaPolicyPpiDa (
//  IN OUT   UINT8            *CurrentAzaliaDockingAttachStatus
//  );
//[-end-121224-IB10870061-remove]//

//[-start-121128-IB05280020-modify]//
/**
 This function offers an interface to Dynamically modify
 gChipsetPkgTokenSpaceGuid.PcdPeiGpioTable or change GPIO setting procedure.

 @param[in, out]    GpioTable           On entry, points to a structure that specifies the GPIO setting. 
                                        On exit, points to the updated structure. 
                                        The default value is decided by gChipsetPkgTokenSpaceGuid.PcdPeiGpioTable1.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcModifyGpioSettingTable (
  IN OUT GPIO_SETTINGS_TABLE  *GpioTable
  );
//[-end-121128-IB05280020-modify]//


//[-start-121128-IB05280020-modify]//
/**
 This function offers an interface to Dynamically modify gChipsetPkgTokenSpaceGuid.PcdPeiSioGpioTable,
 gChipsetPkgTokenSpaceGuid.PcdPeiSioTable or change procedure about SIO initial.

 @param[in, out]    SioTable            On entry, points to a structure that specifies the SIO register and value. 
                                        On exit, points to the updated structure. 
                                        The default value is decided by gChipsetPkgTokenSpaceGuid.PcdPeiSioTable.
 @param[in, out]    SioGpioTable        On entry, points to a structure that specifies the SIO GPIO register and value.
                                        On exit, points to the updated structure.
                                        The default value is decided by gChipsetPkgTokenSpaceGuid.PcdPeiSioGpioTable1.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcHookInitSio (
  IN OUT EFI_SIO_TABLE       *SioTable,
  IN OUT EFI_SIO_GPIO_TABLE  *SioGpioTable
  );
//[-end-121128-IB05280020-modify]//

//[-start-121113-IB07250300-add]//
/**
 This function offers an interface for OEM code to change the MXM GPU power enable sequence
 and modify the Switchable Graphics Information data HOB.

 @param[in]         None

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcMxmDgpuPowerSequence (
  VOID
  );
//[-end-121113-IB07250300-add]//

//[-start-121224-IB10870062-modify]//
/**
 This function offers an interface to modify PEI_ME_PLATFORM_POLICY_PPI data before the system 
 installs PEI_ME_PLATFORM_POLICY_PPI.

 @param[in, out]   *MePlatformPolicyPpi           On entry, points to PEI_ME_PLATFORM_POLICY_PPI structure.
                                                  On exit, points to updated PEI_ME_PLATFORM_POLICY_PPI structure.
 @param[in, out]   *MePlatformPolicyPpiDesc       On entry, points to EFI_PEI_PPI_DESCRIPTOR structure.
                                                  On exit, points to updated EFI_PEI_PPI_DESCRIPTOR structure.
 
 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
 */
 EFI_STATUS
 OemSvcUpdatePeiMePlatformPolicy (
  IN OUT PEI_ME_PLATFORM_POLICY_PPI               *MePlatformPolicyPpi,
  IN OUT EFI_PEI_PPI_DESCRIPTOR                   *MePlatformPolicyPpiDesc
  );

/**
 This function offers an interface to modify PLATFORM_ME_HOOK_PPI data before the system 
 installs PLATFORM_ME_HOOK_PPI.

 @param[in, out]   *PlatformMeHookPpi             On entry, points to PLATFORM_ME_HOOK_PPI structure.
                                                  On exit, points to updated PLATFORM_ME_HOOK_PPI structure.
 @param[in, out]   *PlatformMeHookPpiDesc         On entry, points to EFI_PEI_PPI_DESCRIPTOR structure.
                                                  On exit, points to updated EFI_PEI_PPI_DESCRIPTOR structure.
                                                  
 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdatePlatformMeHook (
  IN OUT PLATFORM_ME_HOOK_PPI                     *PlatformMeHook,
  IN OUT EFI_PEI_PPI_DESCRIPTOR                   *PlatformMeHookPpiDesc
  );

/**
 This function offers an interface to modify PCH_USB_POLICY_PPI data before the system 
 installs PCH_USB_POLICY_PPI.

 @param[in, out]   *PchUsbPolicyPpi     On entry, points to PCH_USB_POLICY_PPI structure.
                                        On exit, points to updated PCH_USB_POLICY_PPI structure.
 @param[in, out]   *PchUsbPolicyPpiDesc On entry, points to EFI_PEI_PPI_DESCRIPTOR structure.
                                        On exit, points to updated EFI_PEI_PPI_DESCRIPTOR structure.

 @retval            EFI_UNSUPPORTED     Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED   Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS         The function performs the same operation as caller.
                                        The caller will skip the specified behavior and assuming
                                        that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdatePchUsbPolicy (
  IN OUT PCH_USB_POLICY_PPI             *PchUsbPolicyPpi,
  IN OUT EFI_PEI_PPI_DESCRIPTOR         *PchUsbPolicyPpiDesc
  );
//[-end-121224-IB10870062-modify]//

//[-start-121224-IB10870061-add]//
/**
 This function offers an interface to modify PEI_AMT_PLATFORM_POLICY_PPI data before the system 
 installs PEI_AMT_PLATFORM_POLICY_PPI.

 @param[in, out]   *AmtPlatformPolicyPpi          On entry, points to PEI_AMT_PLATFORM_POLICY_PPI structure.
                                                  On exit, points to updated PEI_AMT_PLATFORM_POLICY_PPI structure.
 @param[in, out]   *AmtPlatformPolicyPpiDesc      On entry, points to EFI_PEI_PPI_DESCRIPTOR structure.
                                                  On exit, points to updated EFI_PEI_PPI_DESCRIPTOR structure.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdatePeiAmtPlatformPolicy (
  IN OUT PEI_AMT_PLATFORM_POLICY_PPI              *AmtPlatformPolicyPpi,
  IN OUT EFI_PEI_PPI_DESCRIPTOR                   *AmtPlatformPolicyPpiDesc
  );

/**
 This function offers an interface to modify AZALIA_POLICY data before the system 
 installs AZALIA_POLICY.

 @param[in, out]   *PchAzaliaPolicyPpi            On entry, points to AZALIA_POLICY structure.
                                                  On exit, points to updated AZALIA_POLICY structure.
 @param[in, out]   *PchAzaliaPolicyPpiDesc        On entry, points to EFI_PEI_PPI_DESCRIPTOR structure.
                                                  On exit, points to updated EFI_PEI_PPI_DESCRIPTOR structure.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateAzaliaPolicy (
  IN OUT AZALIA_POLICY                           *PchAzaliaPolicyPpi,
  IN OUT EFI_PEI_PPI_DESCRIPTOR                  *PchAzaliaPolicyPpiDesc
  );

/**
 This function offers an interface to modify PCH_PLATFORM_POLICY_PPI data before the system 
 installs PCH_PLATFORM_POLICY_PPI.

 @param[in, out]   *PchPlatformPolicyPpi          On entry, points to PCH_PLATFORM_POLICY_PPI structure.
                                                  On exit, points to updated PCH_PLATFORM_POLICY_PPI structure.
 @param[in, out]   *PchPlatformPolicyPpiDesc      On entry, points to EFI_PEI_PPI_DESCRIPTOR structure.
                                                  On exit, points to updated EFI_PEI_PPI_DESCRIPTOR structure.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdatePchPlatformPolicy (
  IN OUT PCH_PLATFORM_POLICY_PPI                  *PchPlatformPolicyPpi,
  IN OUT EFI_PEI_PPI_DESCRIPTOR                   *PchPlatformPolicyPpiDesc
  );
//[-end-121224-IB10870061-add]//

//[-start-121226-IB10870063-add]//
/**
 This function offers an interface to modify PEI_CPU_PLATFORM_POLICY_PPI data before the system 
 installs PEI_CPU_PLATFORM_POLICY_PPI.

 @param[in, out]   *CpuPlatformPolicyPpi          On entry, points to PEI_CPU_PLATFORM_POLICY_PPI structure.
                                                  On exit, points to updated PEI_CPU_PLATFORM_POLICY_PPI structure.
 @param[in, out]   *CpuPlatformPolicyPpiDesc      On entry, points to EFI_PEI_PPI_DESCRIPTOR structure.
                                                  On exit, points to updated EFI_PEI_PPI_DESCRIPTOR structure.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdatePeiCpuPlatformPolicy (
  IN OUT PEI_CPU_PLATFORM_POLICY_PPI              *CpuPlatformPolicyPpi,
  IN OUT EFI_PEI_PPI_DESCRIPTOR                   *CpuPlatformPolicyPpiDesc
  );

/**
 This function offers an interface to modify SA_PLATFORM_POLICY_PPI data before the system 
 installs SA_PLATFORM_POLICY_PPI.

 @param[in, out]   *SaPlatformPolicyPpi           On entry, points to SA_PLATFORM_POLICY_PPI structure.
                                                  On exit, points to updated SA_PLATFORM_POLICY_PPI structure.
 @param[in, out]   *SaPlatformPolicyPpiDesc       On entry, points to EFI_PEI_PPI_DESCRIPTOR structure.
                                                  On exit, points to updated EFI_PEI_PPI_DESCRIPTOR structure.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcUpdateSaPlatformPolicy (
  IN OUT SA_PLATFORM_POLICY_PPI                   *SaPlatformPolicyPpi,
  IN OUT EFI_PEI_PPI_DESCRIPTOR                   *SaPlatformPolicyPpiDesc
  );
//[-end-121226-IB10870063-add]//

//[-start-130326-IB06720210-add]//
EFI_STATUS
OemSpdDataAndSlotStatus (
  IN OUT U8                      **OemSpdData,
  IN OUT MemorySlotStatus        **SlotStatus,
  IN OUT U16                     *SpdSize
  );
//[-end-130326-IB06720210-add]//

//[-start-130613-IB05400415-modify]//
//[-start-130419-IB05400398-add]//
/**
 This function offers an interface to get board id and fab id using hardware specified way.

 @param[out]        *BoardId                      Board Id get from current board.
 @param[out]        *FabId                        Fab Id get from current board.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
EFI_STATUS
OemSvcGetBoardInfo (
  OUT  UINT8   *BoardId,
  OUT  UINT8   *FabId
  );

/**
 This function offers an interface to get GPIO Table.

 @param[out]        **GpioTable                   Pointer to correct GPIO table for current board.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.

*/
EFI_STATUS
OemSvcGetGpioTable (
  OUT  GPIO_SETTINGS_TABLE   **GpioTable
  );
//[-end-130419-IB05400398-add]//
//[-end-130613-IB05400415-modify]//

//[-start-130709-IB05400426-add]//
/**
 This function offers an interface to handle thunderbolt chipset init stuff.
 It maybe init to enable or disable depend on input parameter.

 @param[in]        TbtEnabled                     TRUE for Call this function to do enable thunderbolt init procedure.
                                                  FALSE for Call this function to do disable thunderbolt init procedure.
 @param[in]        TbtChip                        Thunderbolt chip type.
 @param[out]       *DelayFlag                     TRUE for delay a while after init procedure.

 @retval            EFI_UNSUPPORTED               Returns unsupported by default.
 @retval            EFI_MEDIA_CHANGED             Alter the Configuration Parameter or hook code.
 @retval            EFI_SUCCESS                   The function performs the same operation as caller.
                                                  The caller will skip the specified behavior and assuming
                                                  that it has been handled completely by this function.
*/
VOID
OemSvcPeiTbtInit (
  IN  BOOLEAN           TbtEnabled,
  IN  UINT8             TbtChip,
  OUT BOOLEAN           *DelayFlag
  );
//[-end-130709-IB05400426-add]//

#endif
