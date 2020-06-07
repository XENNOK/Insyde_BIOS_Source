/** @file

  PCI Utility Header

;******************************************************************************
;* Copyright (c) 2018, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corp.
;*
;******************************************************************************
*/


#ifndef _PCI_UTILTIY_H_
#define _PCI_UTILTIY_H_

#include <Uefi.h>
#include <Library/HydraLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRunTimeServicesTableLib.h>
#include <Protocol/PciRootBridgeIo.h>
#include <stdio.h>
#include <Protocol/PciRootBridgeIo.h>
#include <IndustryStandard/Pci22.h>

#define NO_TYPE 255
#define DEF_ROWS 25
#define DEF_COLS 80

#define DATA_NUM(x) ((x)*2)-1

#endif
