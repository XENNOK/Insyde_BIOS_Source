/** @file
 PEI Chipset Services Library.

 This file contains only one function that is PeiCsSvcPlatformStage1Init().
 The function PeiCsSvcPlatformStage1Init() use chipset services to be
 Platform initialization in PEI phase stage 1.

***************************************************************************
* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
*
* You may not reproduce, distribute, publish, display, perform, modify, adapt,
* transmit, broadcast, present, recite, release, license or otherwise exploit
* any part of this publication in any form, by any means, without the prior
* written permission of Insyde Software Corporation.
*
******************************************************************************
*/

#include <Pi/PiPeiCis.h>
#include <Library/DebugLib.h>
#include <Library/PciCf8Lib.h>
#include <Library/IoLib.h>
#include <Library/PeiServicesTablePointerLib.h>
#include <Library/PeiServicesLib.h>
#include <Library/PostCodeLib.h>
//[-start-130709-IB05160465-add]//
#include <Library/MemoryAllocationLib.h>
//[-end-130709-IB05160465-add]//
#include <Ppi/CpuIo.h>
#include <Ppi/Wdt.h>
#include <Ppi/WarmResetDetected.h>
#include <Ppi/ReadOnlyVariable2.h>
#include <HeciRegs.h>
#include <MeChipset.h>
#include <PchAccess.h>
//[-start-130129-IB10820225-remove]//
//#include <ChipsetIRQ.h>
//[-end-130129-IB10820225-remove]//
#include <OEM.h>
#include <ChipsetSetupConfig.h>
#include <SaRegs.h>
#include <SaAccess.h>
#include <PostCode.h>
//[-start-121130-IB05330392-add]//
#include <Library/PchPlatformLib.h>
//[-end-121130-IB05330392-add]//
//[-start-130715-IB06720227-add]//
#include <Protocol/DevicePath.h>
#include <Protocol/LegacyBiosPlatform.h>
#include <Library/PcdLib.h>
#include <Library/BaseMemoryLib.h>
//[-end-130715-IB06720227-add]//
//[-start-140612-IB06720257-add]//
#include <ChipsetIrqRoutingEntry.h>
//[-end-140612-IB06720257-add]//

//[-start-121210-IB10820189-remove]//
#if 0
//[-start-121123-IB11410024-add]//
//
// Avoid header file confilct by Adding this solution. 
// Will remove it after kernel(Scheduled in Tag 47) 
// complete the correction in InsydeModulePkg/OemSvcTodo/ChipsetIrq.h
//
#define B0D19_INTD_IR       CHIPSET_PIRQH
//[-end-121123-IB11410024-add]//

//
// Define Bus/Device Valid Interrupt Routing
//
//[-start-121016-IB06150252-add]//
//#ifdef ULT_SUPPORT
#define B0D19IR_VAL          (B0D19_INTD_IR - 1)
//#endif
//[-end-121016-IB06150252-add]//
//[-start-120316-IB05330329-add]//
#define B0D20IR_VAL         ((B0D20_INTD_IR - 1) << 12) |\
                            ((B0D20_INTC_IR - 1) << 8)  |\
                            ((B0D20_INTB_IR - 1) << 4)  |\
                            (B0D20_INTA_IR - 1)
//[-end-120316-IB05330329-add]//
//[-start-120727-IB05330364-add]//
//#ifdef ULT_SUPPORT
#define B0D21IR_VAL         ((B0D21_INTD_IR - 1) << 12) |\
                            ((B0D21_INTC_IR - 1) << 8)  |\
                            ((B0D21_INTB_IR - 1) << 4)  |\
                            (B0D21_INTA_IR - 1)
//#endif
//[-end-120727-IB05330364-add]//
#define B0D22IR_VAL         ((B0D22_INTD_IR - 1) << 12) |\
                            ((B0D22_INTC_IR - 1) << 8)  |\
                            ((B0D22_INTB_IR - 1) << 4)  |\
                            (B0D22_INTA_IR - 1)
//[-start-120727-IB05330364-add]//
//#ifdef ULT_SUPPORT
#define B0D23IR_VAL         ((B0D23_INTD_IR - 1) << 12) |\
                            ((B0D23_INTC_IR - 1) << 8)  |\
                            ((B0D23_INTB_IR - 1) << 4)  |\
                            (B0D23_INTA_IR - 1)
//#endif
//[-end-120727-IB05330364-add]//
                            
#define B0D25IR_VAL         ((B0D25_INTD_IR - 1) << 12) |\
                            ((B0D25_INTC_IR - 1) << 8)  |\
                            ((B0D25_INTB_IR - 1) << 4)  |\
                            (B0D25_INTA_IR - 1)

#define B0D26IR_VAL         ((B0D26_INTD_IR - 1) << 12) |\
                            ((B0D26_INTC_IR - 1) << 8)  |\
                            ((B0D26_INTB_IR - 1) << 4)  |\
                            (B0D26_INTA_IR - 1)

#define B0D27IR_VAL         ((B0D27_INTD_IR - 1) << 12) |\
                            ((B0D27_INTC_IR - 1) << 8)  |\
                            ((B0D27_INTB_IR - 1) << 4)  |\
                            (B0D27_INTA_IR - 1)

#define B0D28IR_VAL         ((B0D28_INTD_IR - 1) << 12) |\
                            ((B0D28_INTC_IR - 1) << 8)  |\
                            ((B0D28_INTB_IR - 1) << 4)  |\
                            (B0D28_INTA_IR - 1)

#define B0D29IR_VAL         ((B0D29_INTD_IR - 1) << 12) |\
                            ((B0D29_INTC_IR - 1) << 8)  |\
                            ((B0D29_INTB_IR - 1) << 4)  |\
                            (B0D29_INTA_IR - 1)

#define B0D31IR_VAL         ((B0D31_INTD_IR - 1) << 12) |\
                            ((B0D31_INTC_IR - 1) << 8)  |\
                            ((B0D31_INTB_IR - 1) << 4)  |\
                            (B0D31_INTA_IR - 1)

//
// Define Bus/Device Valid Interrupt Pin
//
//[-start-120316-IB05330329-add]//
#define B0D20IP_VAL         (B0D20F3_IP << 12) |\
                            (B0D20F2_IP << 8)  |\
                            (B0D20F1_IP << 4)  |\
                            (B0D20F0_IP)
//[-end-120316-IB05330329-add]//

#define B0D22IP_VAL         (B0D22F3_IP << 12) |\
                            (B0D22F2_IP << 8)  |\
                            (B0D22F1_IP << 4)  |\
                            (B0D22F0_IP)

#define B0D25IP_VAL         (B0D25F0_IP)

#define B0D26IP_VAL         (B0D26F0_IP)

#define B0D27IP_VAL         (B0D27F0_IP)

#define B0D28IP_VAL         (B0D28F7_IP << 28) |\
                            (B0D28F6_IP << 24) |\
                            (B0D28F5_IP << 20) |\
                            (B0D28F4_IP << 16) |\
                            (B0D28F3_IP << 12) |\
                            (B0D28F2_IP << 8)  |\
                            (B0D28F1_IP << 4)  |\
                            (B0D28F0_IP)

#define B0D29IP_VAL         (B0D29F0_IP)

#define B0D31IP_VAL         (B0D31F6_IP << 24) |\
                            (B0D31F5_IP << 20) |\
                            (B0D31F3_IP << 12) |\
                            (B0D31F2_IP << 8) 
#endif
//[-end-121210-IB10820190-remove]//
typedef enum {
  OrData,
  AndData
} DATA_TYPE;

typedef struct {
  UINT32  BassAddress;
  UINT32  RangeAddress;
} IO_DECODE_TABLE;

UINT8  IoDecodeRegisterTable[] = {
  R_PCH_LPC_GEN1_DEC,
  R_PCH_LPC_GEN2_DEC,
  R_PCH_LPC_GEN3_DEC,
  R_PCH_LPC_GEN4_DEC
  };
  
//[-start-121205-IB10820184-remove]//
//IO_DECODE_TABLE  IoDecodeTable[] = {
//  {IO_DECODE_BASS_ADDRESS_1, IO_DECODE_RANGE_ADDRESS_1},
//  {IO_DECODE_BASS_ADDRESS_2, IO_DECODE_RANGE_ADDRESS_2},
//  {IO_DECODE_BASS_ADDRESS_3, IO_DECODE_RANGE_ADDRESS_3},
//  {IO_DECODE_BASS_ADDRESS_4, IO_DECODE_RANGE_ADDRESS_4}
//  };
//[-end-121205-IB10820184-remove]//
  
static EFI_PEI_PPI_DESCRIPTOR     mPpiWarmResetDetected = {
  (EFI_PEI_PPI_DESCRIPTOR_PPI | EFI_PEI_PPI_DESCRIPTOR_TERMINATE_LIST),
  &gPeiWarmResetDetectedGuid,
  NULL
};

//[-start-140709-IB06720257-modify]//
//[-start-130715-IB06720227-add]//
/**
     
 Parse INT[A#:D#] value of target entry and generate interrupt route register configuration value .

 @param[IN]       IrqRoutingEntry               A pointer point to IRQ routing entry of the target device.
 
 @return          Interrupt route register value of input device number.
*/
STATIC
UINT16
TranslateLinkValueOfEntryToRoutingRegisterValue (
  EFI_LEGACY_IRQ_ROUTING_ENTRY_AND_IP_REGISTER  *IrqRoutingEntry
)
{
//
// First, transefer link vaue of INT[A#:D#] to PIRQ index,
// Second, combind this index for internal interrupt route register configuration.
//
  UINT8  *PirqLinkValuePtr;
  UINT8  PirqLinkValueNumber;
  UINT8  InterruptPinIndex;
  UINT8  PirqIndex;
  UINT16 DeviceIrRegisterValue;

  DeviceIrRegisterValue = 0;
  InterruptPinIndex = 0;
  PirqIndex = 0;

  PirqLinkValuePtr = (UINT8 *)PcdGetExPtr (&gChipsetPkgTokenSpaceGuid, PcdPirqLinkValueArray);
  PirqLinkValueNumber = (UINT8)(LibPcdGetExSize (&gChipsetPkgTokenSpaceGuid, PcdToken(PcdPirqLinkValueArray)) / sizeof (UINT8));

  for (InterruptPinIndex = 0; InterruptPinIndex < 4; InterruptPinIndex++ ) {
    for (PirqIndex = 0; PirqIndex < PirqLinkValueNumber; PirqIndex++ ) {   
      if (!(CompareMem (&PirqLinkValuePtr[PirqIndex], &IrqRoutingEntry->LeagcyIrqRoutingEntry.PirqEntry[InterruptPinIndex].Pirq, 1))) {
        // Bit 2:0   of DeviceIrRegisterValue : Interrupt A Pin Route
        // Bit 6:4   of DeviceIrRegisterValue : Interrupt B Pin Route
        // Bit 10:8  of DeviceIrRegisterValue : Interrupt C Pin Route
        // Bit 14:12 of DeviceIrRegisterValue : Interrupt D Pin Route
        DeviceIrRegisterValue = DeviceIrRegisterValue | (PirqIndex << (InterruptPinIndex * 4));
        break;
      }
    }
  }

  return DeviceIrRegisterValue;
}
//[-end-130715-IB06720227-add]//
//[-end-140709-IB06720257-modify]//

//[-start-140709-IB06720257-modify]//
//[-start-130628-IB06720227-add]//
/**
 Retrieve internal device interrupt route register value from PCD (PcdControllerDeviceIrqRoutingEntry).

 @param[IN]        DeviceNumber                 Which internal device you want to retrieve interrupt route register value.    

 @return           Interrupt route register value of input device number.
*/
STATIC
UINT16
InternalDevIrRegisterValueFromRoutingTablePcd(
  UINT16 DeviceNumber
)
{
   EFI_LEGACY_IRQ_ROUTING_ENTRY_AND_IP_REGISTER  *IrqRoutingEntry;
   UINTN  RoutingTableCount;
   UINTN  TargetEntryIndex;
   UINT16 DeviceIrRegisterValue;

   IrqRoutingEntry   = NULL;
   TargetEntryIndex   = 0;
   RoutingTableCount = 0;
   DeviceIrRegisterValue = 0;

   IrqRoutingEntry   = (EFI_LEGACY_IRQ_ROUTING_ENTRY_AND_IP_REGISTER *)PcdGetExPtr (&gChipsetPkgTokenSpaceGuid, PcdControllerDeviceIrqRoutingEntry);
   RoutingTableCount = (LibPcdGetExSize (&gChipsetPkgTokenSpaceGuid, PcdToken(PcdControllerDeviceIrqRoutingEntry)) / sizeof (EFI_LEGACY_IRQ_ROUTING_ENTRY_AND_IP_REGISTER));

   //
   // Find out the target entry from PcdControllerDeviceIrqRoutingEntry list.
   //
   for (TargetEntryIndex = 0; TargetEntryIndex < RoutingTableCount; TargetEntryIndex++, IrqRoutingEntry++) {

     if ((IrqRoutingEntry->LeagcyIrqRoutingEntry.Bus != 0) || (IrqRoutingEntry->LeagcyIrqRoutingEntry.Device != (DeviceNumber << 3))){
       continue;
     }

     //
     // Parse INT[A#:D#] of target entry and generate IR register configuration value .
     //
     DeviceIrRegisterValue = TranslateLinkValueOfEntryToRoutingRegisterValue (IrqRoutingEntry);
     break;

   }
   
  return DeviceIrRegisterValue;
}
//[-end-130628-IB06720227-add]//
//[-end-140709-IB06720257-modify]//

//[-start-140612-IB06720257-add]//
/**
 Retrieve internal device interrupt pin register value from PCD (PcdControllerDeviceIrqRoutingEntry).

 @param[IN]        DeviceNumber                 Which internal device you want to retrieve interrupt route register value.    

 @return           Interrupt pin register value of input device number.
*/
STATIC
UINT32
InternalDevIpRegisterValueFromRoutingTablePcd(
  UINT16 DeviceNumber
)
{
   EFI_LEGACY_IRQ_ROUTING_ENTRY_AND_IP_REGISTER  *IrqRoutingEntryAndIPRegister;
   UINTN  RoutingTableCount;
   UINTN  TargetEntryIndex;
   UINT32 DeviceIpRegisterValue;

   IrqRoutingEntryAndIPRegister   = NULL;
   TargetEntryIndex   = 0;
   RoutingTableCount = 0;
   DeviceIpRegisterValue = 0;

   IrqRoutingEntryAndIPRegister   = (EFI_LEGACY_IRQ_ROUTING_ENTRY_AND_IP_REGISTER  *)PcdGetExPtr (&gChipsetPkgTokenSpaceGuid, PcdControllerDeviceIrqRoutingEntry);
   RoutingTableCount = (LibPcdGetExSize (&gChipsetPkgTokenSpaceGuid, PcdToken(PcdControllerDeviceIrqRoutingEntry)) / sizeof (EFI_LEGACY_IRQ_ROUTING_ENTRY_AND_IP_REGISTER));

   //
   // Find out the IP register value from PcdControllerDeviceIrqRoutingEntry list.
   //
   for (TargetEntryIndex = 0; TargetEntryIndex < RoutingTableCount; TargetEntryIndex++, IrqRoutingEntryAndIPRegister++) {

     if ((IrqRoutingEntryAndIPRegister->LeagcyIrqRoutingEntry.Bus != 0) || (IrqRoutingEntryAndIPRegister->LeagcyIrqRoutingEntry.Device != (DeviceNumber << 3))){
       continue;
     }

     DeviceIpRegisterValue = IrqRoutingEntryAndIPRegister->DevIpRegValue;
     break;

   }
   
  return DeviceIpRegisterValue;
}
//[-end-140612-IB06720257-add]//


STATIC
VOID
IoModify (
  IN CONST EFI_PEI_SERVICES     **PeiServices,
  IN UINT16                     Register,
  IN UINT8                      Value,
  IN DATA_TYPE                  DataType
  )
{
  UINT8   Data8;
  Data8 = IoRead8(Register);
  
  if (DataType == OrData) {
    Data8 |= Value;
  } else {
    Data8 &= ~Value;
  }

  IoWrite8 (Register, Data8);

  return;
}

STATIC
VOID
ProgramChipsetIRQ (
  VOID
  )
{
//[-start-130715-IB06720227-modify]//
//[-start-130430-IB06720225-modify]//
  UINT32 DeviceIpRegisterValue;
//  UINT16 DeviceIrRegisterValue;
  DeviceIpRegisterValue = 0;
//  DeviceIrRegisterValue = 0; 

  //
  // Program D31 interrupt pin/route register
  //
//[-start-120316-IB05330329-modify]//
//[-start-120327-IB05330332-modify]//
//[-start-121210-IB10820189-modify]//
//[-start-140612-IB06720257-modify]//
//  DeviceIpRegisterValue = PcdGetEx32(&gChipsetPkgTokenSpaceGuid, PcdDevice31InterruptPinRegisterValue);
  DeviceIpRegisterValue = InternalDevIpRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_LPC);
//[-end-140612-IB06720257-modify]//
  PchMmRcrb32AndThenOr ( R_PCH_RCRB_D31IP, 0xF00F00F0, DeviceIpRegisterValue); //Bit 0:3 RO. Bit 4:7, Bit 16:19, Bit 28:31 Reserved.
  if ((InternalDevIrRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_LPC)) != 0) {
//  DeviceIrRegisterValue = PcdGetEx16(&gChipsetPkgTokenSpaceGuid, PcdDevice31InterruptRouteRegisterValue); 
  PchMmRcrb16AndThenOr ( R_PCH_RCRB_D31IR, 0x8888, InternalDevIrRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_LPC));
  }

  //
  // Program D29 interrupt pin/route register
  //
//[-start-140612-IB06720257-modify]//
//  DeviceIpRegisterValue = PcdGetEx32(&gChipsetPkgTokenSpaceGuid, PcdDevice29InterruptPinRegisterValue);
  DeviceIpRegisterValue = InternalDevIpRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_USB);
//[-end-140612-IB06720257-modify]//
  PchMmRcrb32AndThenOr ( R_PCH_RCRB_D29IP, 0xFFFFFFF0, DeviceIpRegisterValue); //Bit 4:31 Reserved
  if ((InternalDevIrRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_USB)) != 0) {
//  DeviceIrRegisterValue = PcdGetEx16(&gChipsetPkgTokenSpaceGuid, PcdDevice29InterruptRouteRegisterValue); 
  PchMmRcrb16AndThenOr ( R_PCH_RCRB_D29IR, 0x8888, InternalDevIrRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_USB));
  }

  //
  // Program D28 interrupt pin/route register
  //
//[-start-140612-IB06720257-modify]//
//  DeviceIpRegisterValue = PcdGetEx32(&gChipsetPkgTokenSpaceGuid, PcdDevice28InterruptPinRegisterValue);
  DeviceIpRegisterValue = InternalDevIpRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS);
//[-end-140612-IB06720257-modify]//
  PchMmRcrb32AndThenOr ( R_PCH_RCRB_D28IP, 0x0, DeviceIpRegisterValue);
  if ((InternalDevIrRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS)) != 0) {
//  DeviceIrRegisterValue = PcdGetEx16(&gChipsetPkgTokenSpaceGuid, PcdDevice28InterruptRouteRegisterValue); 
  PchMmRcrb16AndThenOr ( R_PCH_RCRB_D28IR, 0x8888, InternalDevIrRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_PCIE_ROOT_PORTS));
  }

  //
  // Program D27 interrupt pin/route register
  //
//[-start-140612-IB06720257-modify]//
//  DeviceIpRegisterValue = PcdGetEx32(&gChipsetPkgTokenSpaceGuid, PcdDevice27InterruptPinRegisterValue);
  DeviceIpRegisterValue = InternalDevIpRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_AZALIA);
//[-end-140612-IB06720257-modify]//
  PchMmRcrb32AndThenOr ( R_PCH_RCRB_D27IP, 0xFFFFFFF0, DeviceIpRegisterValue); //Bit 4:31 Reserved
  if ((InternalDevIrRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_AZALIA)) != 0) {
//  DeviceIrRegisterValue = PcdGetEx16(&gChipsetPkgTokenSpaceGuid, PcdDevice27InterruptRouteRegisterValue); 
  PchMmRcrb16AndThenOr ( R_PCH_RCRB_D27IR, 0x8888, InternalDevIrRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_AZALIA));
  }
  
  //
  // Program D26 interrupt pin/route register
  //
//[-start-140612-IB06720257-modify]//
//  DeviceIpRegisterValue = PcdGetEx32(&gChipsetPkgTokenSpaceGuid, PcdDevice26InterruptPinRegisterValue);
  DeviceIpRegisterValue = InternalDevIpRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_USB_EXT);
//[-end-140612-IB06720257-modify]//
  PchMmRcrb32AndThenOr ( R_PCH_RCRB_D26IP, 0xFFFFFFF0, DeviceIpRegisterValue); //Bit 4:31 Reserved
  if ((InternalDevIrRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_USB_EXT)) != 0) {
//  DeviceIrRegisterValue = PcdGetEx16(&gChipsetPkgTokenSpaceGuid, PcdDevice26InterruptRouteRegisterValue); 
  PchMmRcrb16AndThenOr ( R_PCH_RCRB_D26IR, 0x8888, InternalDevIrRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_USB_EXT));
  }

  //
  // Program D25 interrupt pin/route register
  //
//[-start-140612-IB06720257-modify]//
//  DeviceIpRegisterValue = PcdGetEx32(&gChipsetPkgTokenSpaceGuid, PcdDevice25InterruptPinRegisterValue);
  DeviceIpRegisterValue = InternalDevIpRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_LAN);
//[-end-140612-IB06720257-modify]//
  PchMmRcrb32AndThenOr ( R_PCH_RCRB_D25IP, 0xFFFFFFF0, DeviceIpRegisterValue); //Bit 4:31 Reserved
  if ((InternalDevIrRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_LAN)) != 0) {
//  DeviceIrRegisterValue = PcdGetEx16(&gChipsetPkgTokenSpaceGuid, PcdDevice25InterruptRouteRegisterValue); 
  PchMmRcrb16AndThenOr ( R_PCH_RCRB_D25IR, 0x8888, InternalDevIrRegisterValueFromRoutingTablePcd (PCI_DEVICE_NUMBER_PCH_LAN));
  }

  //
  // Program D22 interrupt pin/route register
  //
//[-start-140612-IB06720257-modify]//
//  DeviceIpRegisterValue = PcdGetEx32(&gChipsetPkgTokenSpaceGuid, PcdDevice22InterruptPinRegisterValue);
  DeviceIpRegisterValue = InternalDevIpRegisterValueFromRoutingTablePcd (ME_DEVICE_NUMBER);
//[-end-140612-IB06720257-modify]//
  PchMmRcrb32AndThenOr ( R_PCH_RCRB_D22IP, 0xFFFF0000, DeviceIpRegisterValue); //Bit 16:31 Reserved
  if ((InternalDevIrRegisterValueFromRoutingTablePcd (ME_DEVICE_NUMBER)) != 0) {
//  DeviceIrRegisterValue = PcdGetEx16(&gChipsetPkgTokenSpaceGuid, PcdDevice22InterruptRouteRegisterValue); 
  PchMmRcrb16AndThenOr ( R_PCH_RCRB_D22IR, 0x8888, InternalDevIrRegisterValueFromRoutingTablePcd (ME_DEVICE_NUMBER));
  }

  //
  // Program D20 interrupt pin/route register
  //
//[-start-120316-IB05330329-add]//
//[-start-140612-IB06720257-modify]//
//  DeviceIpRegisterValue = PcdGetEx32(&gChipsetPkgTokenSpaceGuid, PcdDevice20InterruptPinRegisterValue);
  DeviceIpRegisterValue = InternalDevIpRegisterValueFromRoutingTablePcd (20);
//[-end-140612-IB06720257-modify]//
  PchMmRcrb32AndThenOr ( R_PCH_RCRB_D20IP, 0xFFFFFFF0, DeviceIpRegisterValue); //Bit 16:31 Reserved
  if ((InternalDevIrRegisterValueFromRoutingTablePcd (20)) != 0) {
//  DeviceIrRegisterValue = PcdGetEx16(&gChipsetPkgTokenSpaceGuid, PcdDevice20InterruptRouteRegisterValue); 
  PchMmRcrb16AndThenOr ( R_PCH_RCRB_D20IR, 0x8888, InternalDevIrRegisterValueFromRoutingTablePcd (20));  
  }

//[-end-120316-IB05330329-add]//
//[-start-120727-IB05330364-add]//
//[-start-120820-IB10820110-modify]//
  if (FeaturePcdGet (PcdUltFlag)) {
    if ((InternalDevIrRegisterValueFromRoutingTablePcd (21)) != 0) {
//    DeviceIrRegisterValue = PcdGetEx16(&gChipsetPkgTokenSpaceGuid, PcdDevice21InterruptRouteRegisterValue); 
    PchMmRcrb16AndThenOr (R_PCH_RCRB_D21IR, 0x8888, InternalDevIrRegisterValueFromRoutingTablePcd (21));
    } 
    
    if ((InternalDevIrRegisterValueFromRoutingTablePcd (23)) != 0) {
//    DeviceIrRegisterValue = PcdGetEx16(&gChipsetPkgTokenSpaceGuid, PcdDevice23InterruptRouteRegisterValue); 
    PchMmRcrb16AndThenOr (R_PCH_RCRB_D23IR, 0x8888, InternalDevIrRegisterValueFromRoutingTablePcd (23));
    }

//[-start-121016-IB06150252-add]//
    if ((InternalDevIrRegisterValueFromRoutingTablePcd (19)) != 0) {
//    DeviceIrRegisterValue = PcdGetEx16(&gChipsetPkgTokenSpaceGuid, PcdDevice19InterruptRouteRegisterValue); 
    PchMmRcrb16AndThenOr ( R_PCH_RCRB_D19IR, 0xFFF8, InternalDevIrRegisterValueFromRoutingTablePcd (19));
    }
    
//[-end-121016-IB06150252-add]//
  }
//[-end-121210-IB10820189-modify]//
//[-end-120820-IB10820110-modify]//
//[-end-120727-IB05330364-add]//
//[-end-130430-IB06720225-modify]//
//[-end-130715-IB06720227-modify]//
}

STATIC
EFI_STATUS
PchInit (
  IN CONST EFI_PEI_SERVICES      **PeiServices
  )
{
  UINT8       Index;
  UINT16      Data16;
  UINTN       TableCount;
  EFI_STATUS  Status;
//[-start-121130-IB05330392-add]//
  PCH_SERIES                         PchSeries;
//[-end-121130-IB05330392-add]//
//[-start-121205-IB10820184-add]//
  IO_DECODE_TABLE     *IoDecodeTable;
//[-end-121205-IB10820184-add]//

//[-start-121130-IB05330392-add]//
  Index = 0;
  Data16 = 0;
  TableCount = 0;
  Status = EFI_SUCCESS;
//[-end-121130-IB05330392-add]//
//[-start-121205-IB10820184-add]//
  IoDecodeTable=(IO_DECODE_TABLE *)PcdGetPtr (PcdPchLpcIoDecodeTable);
  ASSERT (IoDecodeTable != NULL);
//[-end-121205-IB10820184-add]//
  // if PM Base not set yet, => set to 0x400
  Index = PchLpcPciCfg8 (R_PCH_LPC_ACPI_CNT);
  if (!(Index & B_PCH_LPC_ACPI_CNT_ACPI_EN)) {
    //
    // Set PM Base
    //
    PchLpcPciCfg32AndThenOr (
      R_PCH_LPC_ACPI_BASE,
      (UINT32) (~B_PCH_LPC_ACPI_BASE_BAR),
      PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress)
      );

    //
    // Enable PM Base
    //
    PchLpcPciCfg8Or (
      R_PCH_LPC_ACPI_CNT,
      (UINT8) B_PCH_LPC_ACPI_CNT_ACPI_EN
      );    
  }

  //
  // Clear temporary Thermal Device resource
  //
  MmioWrite8 (MmPciAddress (0, DEFAULT_PCI_BUS_NUMBER_PCH, PCI_DEVICE_NUMBER_PCH_THERMAL, PCI_FUNCTION_NUMBER_PCH_THERMAL, R_PCH_THERMAL_COMMAND), (UINT8)0x0);

  TableCount = 0;

  //
  // LPC I/F Enable
  //
  // COMA&B control by IsaAcpi
//[-start-121205-IB10820184-modify]//
  PchLpcPciCfg16AndThenOr (R_PCH_LPC_IO_DEC, 0, PcdGet16 (PcdPchLpcDecodeRange));
  PchLpcPciCfg16AndThenOr (R_PCH_LPC_ENABLES, 0, PcdGet16 (PcdPchLpcEnableList));
//[-end-121205-IB10820184-modify]//
  //
  // For enable write variable
  //
  PchLpcPciCfg8And(R_PCH_LPC_BIOS_CNTL, (UINT8)(~BIT5));
//[-start-130124-IB04770265-add]//
  if (FeaturePcdGet (PcdThunderBoltSupported)) {
//[-start-140623-IB05080432-modify]//
//    for (TableCount = 0 ; TableCount < sizeof (IoDecodeTable) / sizeof (IO_DECODE_TABLE) ; TableCount++) {
    for (TableCount = 0 ; TableCount < LibPcdGetSize(PcdToken (PcdPchLpcIoDecodeTable)) / sizeof (IO_DECODE_TABLE) ; TableCount++) {
//[-end-140623-IB05080432-modify]//
        if( (PchLpcPciCfg16 (IoDecodeRegisterTable[TableCount]) & (UINT16)(B_PCH_LPC_GEN1_DEC_IOBAR | B_PCH_LPC_GEN1_DEC_EN)) == 0x681 ||
          (PchLpcPciCfg16 (IoDecodeRegisterTable[TableCount]) & (UINT16)(B_PCH_LPC_GEN1_DEC_IOBAR | B_PCH_LPC_GEN1_DEC_EN)) == 0x1641) {
          PchLpcPciCfg32AndThenOr (IoDecodeRegisterTable[TableCount], (UINT32)~(B_PCH_LPC_GEN1_DEC_IOBAR | B_PCH_LPC_GEN1_DEC_IODRA | B_PCH_LPC_GEN1_DEC_EN), 0);
        }  
    }
  }
//[-end-130124-IB04770265-add]//  
  //
  // Program PCH LPC I/F Generic decode address
  //
  Index = 0;
//[-start-121205-IB10820184-modify]//
  for (TableCount = 0 ; TableCount < LibPcdGetSize(PcdToken (PcdPchLpcIoDecodeTable)) / sizeof (IO_DECODE_TABLE) ; TableCount++) {
//[-end-121205-IB10820184-modify]//
    if (IoDecodeTable[TableCount].BassAddress != 0) {
      while (Index < (sizeof (IoDecodeRegisterTable) / sizeof (UINT8))) {
//[-start-120815-10370016-modify]//
        //if (PchLpcPciCfg16 (IoDecodeRegisterTable[Index]) != 0) {
          if ((PchLpcPciCfg16 (IoDecodeRegisterTable[Index]) & (UINT16)(B_PCH_LPC_GEN1_DEC_IOBAR | B_PCH_LPC_GEN1_DEC_EN)) != 0) {
          // 
          // Check if the BaseAddress in the IoDecodeTable is already programmed,
          // if yes, skip this BaseAddress to the next.
          //
          //if ((PchLpcPciCfg16 (IoDecodeRegisterTable[Index]) & (UINT16)(~BIT0)) == (UINT16)(IoDecodeTable[TableCount].BassAddress)) {
          if ((PchLpcPciCfg16 (IoDecodeRegisterTable[Index]) & (UINT16)(B_PCH_LPC_GEN1_DEC_IOBAR)) == (UINT16)(IoDecodeTable[TableCount].BassAddress)) {
            Index++;
            break;
          }
          Index++;
        } else {
          //PchLpcPciCfg32AndThenOr (IoDecodeRegisterTable[Index++], 0, (UINT32)((UINT32)(IoDecodeTable[TableCount].RangeAddress) + (UINT32)(IoDecodeTable[TableCount].BassAddress + 1)));
          PchLpcPciCfg32AndThenOr (IoDecodeRegisterTable[Index++], (UINT32)~(B_PCH_LPC_GEN1_DEC_IOBAR | B_PCH_LPC_GEN1_DEC_IODRA | B_PCH_LPC_GEN1_DEC_EN), (UINT32)((UINT32)(IoDecodeTable[TableCount].RangeAddress) + (UINT32)(IoDecodeTable[TableCount].BassAddress + 1)));
          break;
        }
//[-end-120815-10370016-modify]//
      }
    }
  }

//[-start-121130-IB05330392-modify]//
  //
  // Set Coprocessor Error Enable(CEN)
  //
  PchSeries = GetPchSeries();
  if (PchSeries == PchH) {
//[-start-130308-IB10130033-modify]//
    PchMmRcrb16Or (R_PCH_RCRB_OIC, B_PCH_RCRB_OIC_CEN);
//[-end-130308-IB10130033-modify]//
  }  
//[-end-121130-IB05330392-modify]//

  //
  // Disable the Watchdog timer expiration from causing a system reset
  //
  PchMmRcrb32Or (R_PCH_RCRB_GCS, B_PCH_RCRB_GCS_NR);

  //
  // Program PCH Device Interrupt Pin/Routing
  //
  ProgramChipsetIRQ ();
   
  //
  // Halt the TCO timer
  //
//[-start-121008-IB11410008-modify]//
  Data16 = IoRead16 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + PCH_TCO_BASE + R_PCH_TCO1_CNT);
  
  Data16 |= B_PCH_TCO_CNT_TMR_HLT;
  IoWrite16(PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + PCH_TCO_BASE + R_PCH_TCO1_CNT, Data16);

  //
  // Clear the Second TO status bit
  //
  IoModify (PeiServices, PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + PCH_TCO_BASE + R_PCH_TCO2_STS, B_PCH_TCO2_STS_SECOND_TO, OrData);
//[-end-121008-IB11410008-modify]//
  //
  // Disable SERR NMI and IOCHK# NMI in port 61
  //
  IoModify (PeiServices, R_PCH_NMI_SC,B_PCH_NMI_SC_PCI_SERR_EN | B_PCH_NMI_SC_IOCHK_NMI_EN,OrData);
  //
  // Set IO Decode Registers
  //
  PchLpcPciCfg16AndThenOr (R_PCH_LPC_IO_DEC, 0, 0x0010);

  //
  // Set Serial IRQ Enable and set to contunuous mode
  //
  PchLpcPciCfg8Or (R_PCH_LPC_SERIRQ_CNT, (B_PCH_LPC_SERIRQ_CNT_SIRQEN | B_PCH_LPC_SERIRQ_CNT_SIRQMD));

//[-start-121008-IB11410008-modify]//
  IoWrite8 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_STS, B_PCH_ACPI_PM1_STS_BM);
//[-end-121008-IB11410008-modify]//
  //
  // Install Warm Reset Detected PPI
  //
  if ((PchLpcPciCfg16(R_PCH_LPC_GEN_PMCON_2) & B_PCH_LPC_GEN_PMCON_MEM_SR) == B_PCH_LPC_GEN_PMCON_MEM_SR) {
    Status = PeiServicesInstallPpi (&mPpiWarmResetDetected);
  }

  return EFI_SUCCESS;
}

STATIC
EFI_STATUS
HeciInit (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN EFI_PEI_CPU_IO_PPI        *CpuIo
  )
{
  UINT64              Address;
  UINT32              Data32;

  POST_CODE (PEI_HECI_INIT); //PostCode = 0x7C, HECI Initial
  //
  // Assign HECI_MBAR.
  //
  Address = HECI_PCI_ADDR + R_HECIMBAR;
  Data32  = PcdGet32 (PcdHeciMmioBaseAddress);
  (**PeiServices).PciCfg->Write (
                              PeiServices,
                              (**PeiServices).PciCfg,
                              EfiPeiPciCfgWidthUint32,
                              Address,
                              &Data32
                              );

  //
  // Enable HECI BME and MSE.
  //
  Address = HECI_PCI_ADDR + R_COMMAND;
  (**PeiServices ).PciCfg->Read (
                              PeiServices,
                              (**PeiServices).PciCfg,
                              EfiPeiPciCfgWidthUint32,
                              Address,
                              &Data32
                              );
  
  Data32 = Data32 | ( B_BME | B_MSE );
  (**PeiServices ).PciCfg->Write (
                              PeiServices,
                              (**PeiServices).PciCfg,
                              EfiPeiPciCfgWidthUint32,
                              Address,
                              &Data32
                              );

  return EFI_SUCCESS;
}

STATIC
VOID
PowerFailureProcess (
  IN CONST EFI_PEI_SERVICES    **PeiServices,
  IN       UINT16              Pm1Sts,
  IN       UINT16              Pm1Cnt
  )
{

  EFI_STATUS          Status;
  //
  // System BIOS should follow cold boot path if PWR_FLR, GEN_RST_STS or PWRBTNOR_STS is set to 1 regardless of the value in the SLP_TYP field.
  //
  // 
  // PM1sts is R/WC.
  //
  if ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) == V_PCH_ACPI_PM1_CNT_S3) {
   Pm1Sts &= (UINT16)(B_PCH_ACPI_PM1_STS_WAK | B_PCH_ACPI_PM1_STS_PRBTNOR);
  } else {
   Pm1Sts &= (UINT16)~(B_PCH_ACPI_PM1_STS_WAK | B_PCH_ACPI_PM1_STS_PRBTNOR);
  }

  IoWrite16 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_STS , Pm1Sts);
            
  PchLpcPciCfg8AndThenOr (
    R_PCH_LPC_GEN_PMCON_2,
    (UINT8)~(B_PCH_LPC_GEN_PMCON_SRS | B_PCH_LPC_GEN_PMCON_CTS | B_PCH_LPC_GEN_PMCON_MIN_SLP_S4 | B_PCH_LPC_GEN_PMCON_SYSPWR_FLR | B_PCH_LPC_GEN_PMCON_PWROK_FLR),
    (UINT8)B_PCH_LPC_GEN_PMCON_PWROK_FLR
    );
//[-start-120912-IB04770244-modify]//
  //
  // Clear PWR_FLR, GEN_RST_STS and SUS_PWR_FLR when power failure
  //
  PchLpcPciCfg16AndThenOr (
    R_PCH_LPC_GEN_PMCON_3,
    (UINT16)~(B_PCH_LPC_GEN_PMCON_SUS_PWR_FLR | B_PCH_LPC_GEN_PMCON_GEN_RST_STS | B_PCH_LPC_GEN_PMCON_PWR_FLR),
    (UINT16) (B_PCH_LPC_GEN_PMCON_SUS_PWR_FLR | B_PCH_LPC_GEN_PMCON_GEN_RST_STS | B_PCH_LPC_GEN_PMCON_PWR_FLR)
    );
//[-end-120912-IB04770244-modify]//

  if ((Pm1Cnt & B_PCH_ACPI_PM1_CNT_SLP_TYP) != V_PCH_ACPI_PM1_CNT_S4) {
    Status = PeiServicesSetBootMode (BOOT_WITH_FULL_CONFIGURATION);
    ASSERT_EFI_ERROR (Status);
  }
}

STATIC
EFI_STATUS
PeiMchInit (
  IN CONST EFI_PEI_SERVICES        **PeiServices,
  IN       EFI_PEI_CPU_IO_PPI       *CpuIo
  )
{
  McD0PciCfg8 (0xF3) = (UINT8)( McD0PciCfg8 (0xF3 ) | BIT0);

//[-start-120404-IB05300309-modify]//
  McD0PciCfg64 (R_SA_MCHBAR) = (UINT64)(MCH_BASE_ADDRESS | BIT0);
  McD0PciCfg64 (R_SA_DMIBAR)   = ( UINT64 )(PcdGet32 (PcdDmiBaseAddress) | BIT0);
  McD0PciCfg64 (R_SA_PXPEPBAR) = ( UINT64 )(PcdGet32 (PcdEpBaseAddress) | BIT0);
//[-end-120404-IB05300309-modify]//

  McD1PciCfg32Or (0xF0, BIT11);
  McD1PciCfg32And (0xC24, (~BIT16));

  //
  // MCHBAR + 6120h, VLWCTRL
  // Clear MCHBAR + 0x6120[0]
  //
  Mmio8And (MCH_BASE_ADDRESS, 0x6120, ~BIT0);
  Mmio8Or (MCH_BASE_ADDRESS, 0x5418, BIT4 | BIT5);
  
  return EFI_SUCCESS;
}

/**
 Platform initialization in PEI phase stage 1.

 @param[in]         None

 @retval            EFI_SUCCESS         This function alway return successfully
*/
EFI_STATUS
PlatformStage1Init (
  VOID
  )
{
  EFI_STATUS                           Status;
  UINT16                               Pm1Sts;
  UINT16                               Pm1Cnt;
  EFI_PEI_CPU_IO_PPI                   *CpuIo;
  EFI_PEI_PCI_CFG2_PPI                 *PciCfg;
  WDT_PPI                              *Wdt;
  EFI_PEI_READ_ONLY_VARIABLE2_PPI      *VariableServices;
  UINTN                                VariableSize;
  VOID                                 *SystemConfiguration;
  EFI_GUID                             SystemConfigurationGuid = SYSTEM_CONFIGURATION_GUID;
  CONST EFI_PEI_SERVICES               **PeiServices;
  
  PeiServices = GetPeiServicesTablePointer ();

  //
  // There is a second path on entry to handle publication of
  // memory-based PPI
  //
  POST_CODE (PEI_PCIE_MMIO_INIT); //PostCode = 0x74, PCIE MMIO BAR Initial

  CpuIo   = (**PeiServices).CpuIo;
  PciCfg  = (**PeiServices).PciCfg;

  Status = PeiMchInit (PeiServices, CpuIo);
  ASSERT_EFI_ERROR (Status);

  //
  // Do basic PCH init
  //
  POST_CODE (PEI_SB_REG_INIT); //PostCode = 0x76, South Bridge Early Initial
  Status = PchInit (PeiServices);
  ASSERT_EFI_ERROR (Status);

//[-start-130112-IB10310017-modify]//
  if (FeaturePcdGet (PcdMeSupported) || FeaturePcdGet (PcdSpsMeSupported)) {
//[-end-130112-IB10310017-modify]//
    Status = HeciInit ( PeiServices, CpuIo );
    ASSERT_EFI_ERROR (Status);
  }
  ASSERT_EFI_ERROR (Status);

  Pm1Sts = IoRead16 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_STS);
  Pm1Cnt = IoRead16 (PcdGet16 (PcdPerfPkgAcpiIoPortBaseAddress) + R_PCH_ACPI_PM1_CNT);
//
// Power Failure Consideration PPT BIOS SPEC 19.3
//
  //
  // Locate Variable Ppi
  //
  Status = PeiServicesLocatePpi (
             &gEfiPeiReadOnlyVariable2PpiGuid,
             0,
             NULL,
             (VOID **) &VariableServices
             );
  ASSERT_EFI_ERROR (Status);

  //
  // Get Setup Variable
  //
//[-start-130709-IB05160465-modify]//
  VariableSize = PcdGet32 (PcdSetupConfigSize);
  SystemConfiguration = AllocateZeroPool (VariableSize);
//[-start-140623-IB05080432-modify]//
  if (SystemConfiguration == NULL) {
    DEBUG ((EFI_D_INFO, "Fail to allocate memory for SystemConfiguration!!!"));
    ASSERT (SystemConfiguration != NULL);
  } else {
    Status = VariableServices->GetVariable (
                                 VariableServices,
                                 L"Setup",
                                 &SystemConfigurationGuid,
                                 NULL,
                                 &VariableSize,
                                 SystemConfiguration
                                 );
  }
//[-end-140623-IB05080432-modify]//
//[-end-130709-IB05160465-modify]//

  if ((PchLpcPciCfg16 (R_PCH_LPC_GEN_PMCON_2) & B_PCH_LPC_GEN_PMCON_SYSPWR_FLR) == B_PCH_LPC_GEN_PMCON_SYSPWR_FLR) {
    //
    // When the CPUPWR_FLR bit, GEN_PMCON_2(D31:F0:A2h[1])
    // Software must clear this bit if set.
    //
    PchLpcPciCfg8Or (
      R_PCH_LPC_GEN_PMCON_2,
      B_PCH_LPC_GEN_PMCON_SYSPWR_FLR
      );
  }
//[-start-130710-IB05160465-modify]//
//[-start-140623-IB05080432-modify]//
  if ((SystemConfiguration != NULL) && 
      (((CHIPSET_CONFIGURATION *)SystemConfiguration)->BoardCapability == 1 && ((CHIPSET_CONFIGURATION *)SystemConfiguration)->DeepSlpSx == 5)) {
//[-end-140623-IB05080432-modify]//
//[-end-130710-IB05160465-modify]//
    //
    // Deep S3 is enabled
    // 
    if ((Pm1Sts & B_PCH_ACPI_PM1_STS_PRBTNOR) == B_PCH_ACPI_PM1_STS_PRBTNOR 
              || (PchLpcPciCfg8  (R_PCH_LPC_GEN_PMCON_2) & B_PCH_LPC_GEN_PMCON_PWROK_FLR) == B_PCH_LPC_GEN_PMCON_PWROK_FLR
              || (PchLpcPciCfg16 (R_PCH_LPC_GEN_PMCON_3) & B_PCH_LPC_GEN_PMCON_PWR_FLR)   == B_PCH_LPC_GEN_PMCON_PWR_FLR 
         ){
           PowerFailureProcess (PeiServices, Pm1Sts, Pm1Cnt);
         }
  } else {
    //
    // Deep S3 is disabled.
    //
    if ((Pm1Sts & B_PCH_ACPI_PM1_STS_PRBTNOR) == B_PCH_ACPI_PM1_STS_PRBTNOR 
              || (PchLpcPciCfg8  (R_PCH_LPC_GEN_PMCON_2) & B_PCH_LPC_GEN_PMCON_PWROK_FLR)   == B_PCH_LPC_GEN_PMCON_PWROK_FLR
              || (PchLpcPciCfg16 (R_PCH_LPC_GEN_PMCON_3) & B_PCH_LPC_GEN_PMCON_PWR_FLR)     == B_PCH_LPC_GEN_PMCON_PWR_FLR
              || (PchLpcPciCfg16 (R_PCH_LPC_GEN_PMCON_3) & B_PCH_LPC_GEN_PMCON_GEN_RST_STS) == B_PCH_LPC_GEN_PMCON_GEN_RST_STS
              || (PchLpcPciCfg16 (R_PCH_LPC_GEN_PMCON_3) & B_PCH_LPC_GEN_PMCON_SUS_PWR_FLR) == B_PCH_LPC_GEN_PMCON_SUS_PWR_FLR
         ) {
      PowerFailureProcess (PeiServices, Pm1Sts, Pm1Cnt);
    }
  }
  //
  // WDT Handle
  //
  Status = PeiServicesLocatePpi (
             &gWdtPpiGuid,
             0,
             NULL,
             (VOID **) &Wdt
             );
  ASSERT_EFI_ERROR (Status);

  if (Wdt->IsWdtEnabled ()) {
    //
    // Here handle BiosAction case 2 in PchMeUma.c
    //   When ICC SDK apply "Permanently(reboot)" in SCU, 
    //   the PCH will request a "Power Cycle Reset" and the Vpro RC (PchMeUma.c) will start WDT to handle new settings.
    //
    //   When system run to here, the new settings for ICC are available, and WDT should be finish.
    //
    Wdt->Disable();
  }
  return EFI_SUCCESS;

}
