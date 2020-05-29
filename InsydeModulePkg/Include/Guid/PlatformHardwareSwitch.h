/** @file

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PLATFORM_HARDWARE_SWITCH_GUID_H_
#define _PLATFORM_HARDWARE_SWITCH_GUID_H_

#define PLATFORM_HARDWARE_SWITCH_GUID  \
  { \
    0x8D4D2344, 0x4185, 0x42D5, 0xA5, 0x7B, 0x69, 0x59, 0x4E, 0x5, 0xAA, 0x8D \
  }

#define PORT_DISABLE          0         //Disable port
#define PORT_ENABLE           1         //Enable port
#define PCIE_PORT_AUTO        2         //For PCIE root port configuration only, AUTO means follow reference code implementation.
#define AUTO_ENABLE           1         //Auto Enable

typedef struct _SWITCH_BITCONFIG {
  UINT32                Port00En  :1;
  UINT32                Port01En  :1;
  UINT32                Port02En  :1;
  UINT32                Port03En  :1;
  UINT32                Port04En  :1;
  UINT32                Port05En  :1;
  UINT32                Port06En  :1;
  UINT32                Port07En  :1;
  UINT32                Port08En  :1;
  UINT32                Port09En  :1;
  UINT32                Port10En  :1;
  UINT32                Port11En  :1;
  UINT32                Port12En  :1;
  UINT32                Port13En  :1;
  UINT32                Port14En  :1;
  UINT32                Port15En  :1;
  UINT32                Port16En  :1;
  UINT32                Port17En  :1;
  UINT32                Port18En  :1;
  UINT32                Port19En  :1;
  UINT32                Port20En  :1;
  UINT32                Port21En  :1;
  UINT32                Port22En  :1;
  UINT32                Port23En  :1;
  UINT32                Port24En  :1;
  UINT32                Port25En  :1;
  UINT32                Port26En  :1;
  UINT32                Port27En  :1;
  UINT32                Port28En  :1;
  UINT32                Port29En  :1;
  UINT32                Port30En  :1;
  UINT32                Port31En  :1;
  UINT32                Port32En  :1;
  UINT32                Port33En  :1;
  UINT32                Port34En  :1;
  UINT32                Port35En  :1;
  UINT32                Port36En  :1;
  UINT32                Port37En  :1;
  UINT32                Port38En  :1;
  UINT32                Port39En  :1;
  UINT32                Port40En  :1;
  UINT32                Port41En  :1;
  UINT32                Port42En  :1;
  UINT32                Port43En  :1;
  UINT32                Port44En  :1;
  UINT32                Port45En  :1;
  UINT32                Port46En  :1;
  UINT32                Port47En  :1;
  UINT32                Port48En  :1;
  UINT32                Port49En  :1;
  UINT32                Port50En  :1;
  UINT32                Port51En  :1;
  UINT32                Port52En  :1;
  UINT32                Port53En  :1;
  UINT32                Port54En  :1;
  UINT32                Port55En  :1;
  UINT32                Port56En  :1;
  UINT32                Port57En  :1;
  UINT32                Port58En  :1;
  UINT32                Port59En  :1;
  UINT32                Port60En  :1;
  UINT32                Port61En  :1;
  UINT32                Port62En  :1;
  UINT32                Port63En  :1;
} SWITCH_BITCONFIG;

typedef struct _SWITCH_BITCONFIG_2 {
  UINT32                Port00En  :2;
  UINT32                Port01En  :2;
  UINT32                Port02En  :2;
  UINT32                Port03En  :2;
  UINT32                Port04En  :2;
  UINT32                Port05En  :2;
  UINT32                Port06En  :2;
  UINT32                Port07En  :2;
  UINT32                Port08En  :2;
  UINT32                Port09En  :2;
  UINT32                Port10En  :2;
  UINT32                Port11En  :2;
  UINT32                Port12En  :2;
  UINT32                Port13En  :2;
  UINT32                Port14En  :2;
  UINT32                Port15En  :2;
  UINT32                Port16En  :2;
  UINT32                Port17En  :2;
  UINT32                Port18En  :2;
  UINT32                Port19En  :2;
  UINT32                Port20En  :2;
  UINT32                Port21En  :2;
  UINT32                Port22En  :2;
  UINT32                Port23En  :2;
  UINT32                Port24En  :2;
  UINT32                Port25En  :2;
  UINT32                Port26En  :2;
  UINT32                Port27En  :2;
  UINT32                Port28En  :2;
  UINT32                Port29En  :2;
  UINT32                Port30En  :2;
  UINT32                Port31En  :2;
} SWITCH_BITCONFIG_2;

typedef union _HARDWARE_SWITCH {
  SWITCH_BITCONFIG      Config;
  UINT64                CONFIG;
} HARDWARE_SWITCH;

typedef union _HARDWARE_SWITCH_2 {
  SWITCH_BITCONFIG_2    Config;
  UINT64                CONFIG;
} HARDWARE_SWITCH_2;

typedef struct _PLATFORM_HARDWARE_SWITCH {
  HARDWARE_SWITCH       SataPortSwitch;
  HARDWARE_SWITCH       UsbPortSwitch;
  HARDWARE_SWITCH_2     PciePortSwitch;
} PLATFORM_HARDWARE_SWITCH;

extern EFI_GUID gPlatformHardwareSwitchGuid;

#endif
