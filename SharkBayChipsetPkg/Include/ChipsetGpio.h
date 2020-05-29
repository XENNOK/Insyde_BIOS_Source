/** @file
  This header file is provided to Chipset to define GPIO function.

;******************************************************************************
;* Copyright (c) 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _CHIPSET_GPIO_DEFINE_H_
#define _CHIPSET_GPIO_DEFINE_H_

//[-start-140604-IB06720256-remove]//
////
//// GPIO_XX_SELECT
////
//#define SELECT_NATIVE  0
//#define SELECT_GPIO    1
//
////
//// GPIO_XX_TYPE
////
//#define TYPE_OUT      0
//#define TYPE_IN       1
//
////
//// GPIO_XX_LEVEL
////
//#define LEVEL_LO  0
//#define LEVEL_HI  1
//
////
//// GPIO_XX_BLINK
////
//#define BLINK_DIS   0
//#define BLINK_EN    1
//
////
//// GPIO_XX_INVERT
////
//#define INVERT_DIS   0
//#define INVERT_EN    1
//
//#include <PchRegs.h>
//
//#define GPIO_00_31_SELECT_OFFSET    R_PCH_GPIO_USE_SEL
//#define GPIO_00_31_TYPE_OFFSET      R_PCH_GPIO_IO_SEL 
//#define GPIO_00_31_LEVEL_OFFSET     R_PCH_GPIO_LVL
//
//#define GPIO_00_31_BLINK_OFFSET     R_PCH_GPIO_BLINK
//#define GPIO_00_31_INVERT_OFFSET    R_PCH_GPIO_GPI_INV
//
//#define GPIO_32_63_SELECT_OFFSET    R_PCH_GPIO_USE_SEL2
//#define GPIO_32_63_TYPE_OFFSET      R_PCH_GPIO_IO_SEL2
//#define GPIO_32_63_LEVEL_OFFSET     R_PCH_GPIO_LVL2
//
//#define GPIO_64_95_SELECT_OFFSET    R_PCH_GPIO_USE_SEL3
//#define GPIO_64_95_TYPE_OFFSET      R_PCH_GPIO_IO_SEL3
//#define GPIO_64_95_LEVEL_OFFSET     R_PCH_GPIO_LVL3
//[-end-140604-IB06720256-remove]//

//[-start-140604-IB06720256-add]//
//[-start-130325-IB05280056-modify]//
// GPIO_SETTING_TABLE field name is reference to PCH EDS mnemonic
// Name       | 2-Chip Spec  | ULT Spec
// GpioUseSel | GPIO_USE_SEL | GPIO_USE_SEL
// GpioIoSel  | GP_IO_SEL    | GPIO_IO_SEL
// GpoLvl     | GP_LVL       | GPO_LVL
// GpioBlink  | GPO_BLINK    | GPO_BLINK
// GpiInv     | GPI_INV      | GPI_INV
// GpioOwn    | None         | GPIO_OWN
// GpiRout    | None         | GPI_ROUT
// GpiIe      | None         | GPI_IE
// GpiLxEb    | None         | GPI_LxEB
// GpiWp      | None         | GPIWP
// GpinDis    | None         | GPINDIS
// None value set 0
//
typedef struct {
  UINT16 GpioUseSel       : 1;   ///< 0:Native    1:GPIO
  UINT16 GpioIoSel        : 1;   ///< 0:Output    1:Input
  UINT16 GpoLvl           : 1;   ///< 0:Low       1:Hign
  UINT16 GpioBlink        : 1;   ///< 0:Disable   1:Enable
  UINT16 GpiInv           : 1;   ///< 0:Disable   1:Enable
  UINT16 GpioOwn          : 1;   ///< 0:GPE01     1:GPI_IS[n]
  UINT16 GpiRout          : 1;   ///< 0:SCI       1:NMI/SMI
  UINT16 GpiIe            : 1;   ///< 0:Disable   1:Enable
  UINT16 GpiLxEb          : 1;   ///< 0:Edge      1:Level
  UINT16 GpiWp            : 2;   ///< 0:Null      1:pulldown   10:pullup     11:Invaild
  UINT16 GpinDis          : 1;   ///< 0:Disable   1:Enable
  UINT16 Reserved         : 4;
} GPIO_SETTINGS_TABLE;
//[-end-130325-IB05280056-modify]//
//[-end-140604-IB06720256-add]//

#endif
