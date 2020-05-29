//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
/*++
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.
--*/
/*++

Copyright (c)  2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.


Module Name:

  PciConfigAccess.h

Abstract:

  Macros to simplify and abstract the interface to PCI configuration.

--*/
#include "IoAccess.h"


VOID
PciWriteConfig8 (
  IN  UINT32 Register,
  IN  UINT8 Val
  )
{
  UINT16 DataInxPort;
  Register |= PCI_ACCRESS_ENABLE_BIT;
  IoOutput32(PCI_ADDRESS_INDEX, Register);
  DataInxPort = PCI_DATA_INDEX + (UINT16)(Register & 3);
  IoOutput8(DataInxPort, (UINT8)Val);

}

VOID
PciWriteConfig16 (
  IN  UINT32 Register,
  IN  UINT16 Val
  )
{
  UINT16 DataInxPort;
  Register |= PCI_ACCRESS_ENABLE_BIT;
  IoOutput32(PCI_ADDRESS_INDEX, Register);
  DataInxPort = PCI_DATA_INDEX + (UINT16)(Register & 3);
  IoOutput16(DataInxPort, (UINT16)Val);
}

VOID
PciWriteConfig32 (
  IN  UINT32 Register,
  IN  UINT32 Val
  )
{
  Register |= PCI_ACCRESS_ENABLE_BIT;
  IoOutput32(PCI_ADDRESS_INDEX, Register);
  IoOutput32(PCI_DATA_INDEX, Val);

}

UINT8
PciReadConfig8 (
  IN  UINT32 Register
  )
{

  UINT16 DataInxPort;
  Register |= PCI_ACCRESS_ENABLE_BIT;
  IoOutput32(PCI_ADDRESS_INDEX, Register);
  DataInxPort = PCI_DATA_INDEX + (UINT16)(Register & 3);
  return IoInput8(DataInxPort);

}

UINT16
PciReadConfig16 (
  IN  UINT32 Register
  )
{

  UINT16 DataInxPort;
  Register |= PCI_ACCRESS_ENABLE_BIT;
  IoOutput32(PCI_ADDRESS_INDEX, Register);
  DataInxPort = PCI_DATA_INDEX + (UINT16)(Register & 3);
  return IoInput16(DataInxPort);
}

UINT32
PciReadConfig32 (
  IN  UINT32 Register
  )
{
  Register |= PCI_ACCRESS_ENABLE_BIT;
  IoOutput32(PCI_ADDRESS_INDEX, Register);
  return IoInput32(PCI_DATA_INDEX);

}

