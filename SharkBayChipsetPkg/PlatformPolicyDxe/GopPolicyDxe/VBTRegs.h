//;******************************************************************************
//;* Copyright (c) 1983-2012, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//;
//; Module Name :
//;
//;   VBTRegs.h
//;

#ifndef _VBT_REGS_H_
#define _VBT_REGS_H_

#include "Numbers.h"

#define R_VBT_CHECKSUM                                            0x001A
#define B_VBT_CHECKSUM                                            0xFF
#define N_VBT_CHECKSUM                                            0

#define R_VBT_LFP_CONFIG                                          0x0630
#define B_VBT_LFP_CONFIG                                          (BIT4 | BIT3)
#define N_VBT_LFP_CONFIG                                          3
#define V_VBT_LFP_CONFIG_NONE                                     0
#define V_VBT_LFP_CONFIG_LVDS                                     1
#define V_VBT_LFP_CONFIG_eDP                                      3

#define R_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE                     0x103C
#define B_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE                     (BIT1 | BIT0)
#define N_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE                     0
#define V_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE_NONE                0
#define V_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE_EXT                 0
#define V_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE_I2C                 1
#define V_VBT_BACKLIGHT_CONTROL_INVERTER_TYPE_PWM                 2

#define R_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY                 0x103C
#define B_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY                 (BIT2)
#define N_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY                 2
#define V_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY_NORMAL          0
#define V_VBT_BACKLIGHT_CONTROL_INVERTER_POLARITY_INVERTED        1

#endif
