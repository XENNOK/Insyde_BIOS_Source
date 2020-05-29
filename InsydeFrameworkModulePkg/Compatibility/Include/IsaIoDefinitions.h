//;******************************************************************************
//;* Copyright (c) 1983-2011, Insyde Software Corporation. All Rights Reserved.
//;*
//;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
//;* transmit, broadcast, present, recite, release, license or otherwise exploit
//;* any part of this publication in any form, by any means, without the prior
//;* written permission of Insyde Software Corporation.
//;*
//;******************************************************************************
//
// This file contains a 'Sample Driver' and is licensed as such
// under the terms of your license agreement with Intel or your
// vendor.  This file may be modified by the user, subject to  
// the additional terms of the license agreement               
//
/*++

Copyright (c) 2005 Intel Corporation. All rights reserved
This software and associated documentation (if any) is furnished
under a license and may only be used or copied in accordance
with the terms of the license. Except as permitted by such
license, no part of this software or documentation may be
reproduced, stored in a retrieval system, or transmitted in any
form or by any means without the express written consent of
Intel Corporation.

Module Name:

  IsaIoDefinitions.h
  
Abstract:
  
  The header file to choose the EFI_ISA_IO_PROTOCOL 
  or EFI_LIGHT_ISA_IO_PROTOCOL.
  
Revision History:

--*/

#ifndef _EFI_ISA_IO_DEFINITIONS_H
#define _EFI_ISA_IO_DEFINITIONS_H

#ifndef SIZE_REDUCTION_ISA_COMBINED

#include EFI_PROTOCOL_DEFINITION (IsaIo)
#define EFI_INTERFACE_DEFINITION_FOR_ISA_IO EFI_ISA_IO_PROTOCOL
#define  EFI_ISA_IO_PROTOCOL_VERSION &gEfiIsaIoProtocolGuid
#define EFI_ISA_IO_OPERATION_TOKEN   EfiIsaIoOperationBusMasterWrite

#else

#include EFI_PROTOCOL_DEFINITION (LightIsaIo)
#define EFI_INTERFACE_DEFINITION_FOR_ISA_IO EFI_LIGHT_ISA_IO_PROTOCOL
#define  EFI_ISA_IO_PROTOCOL_VERSION &gEfiLightIsaIoProtocolGuid
#define EFI_ISA_IO_OPERATION_TOKEN   EfiIsaIoOperationSlaveWrite  
#define ADD_SERIAL_NAME(x, y)

#endif


#endif