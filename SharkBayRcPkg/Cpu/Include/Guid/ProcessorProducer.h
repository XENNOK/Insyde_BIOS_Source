/**
  This file contains 'Framework Code' and is licensed as such
  under the terms of your license agreement with Intel or your
  vendor.  This file may not be modified, except as allowed by
  additional terms of your license agreement.

@copyright
  Copyright (c) 2011 - 2012 Intel Corporation. All rights reserved
  This software and associated documentation (if any) is furnished
  under a license and may only be used or copied in accordance
  with the terms of the license. Except as permitted by such
  license, no part of this software or documentation may be
  reproduced, stored in a retrieval system, or transmitted in any
  form or by any means without the express written consent of
  Intel Corporation.

@file
  ProcessorProducer.h

@brief
  GUIDs used for SMRAM CPU DATA Header signature

**/
#ifndef _PROCESSOR_PRODUCER_HEADER_H_
#define _PROCESSOR_PRODUCER_HEADER_H_

///
/// SMRAM CPU DATA Header for search by CpuS3Peim
///
#define PROCESSOR_PRODUCER_GUID \
  { 0x1bf06aea, 0x5bec, 0x4a8d, {0x95, 0x76, 0x74, 0x9b, 0x09, 0x56, 0x2d, 0x30} }

extern EFI_GUID gProcessorProducerGuid;

#endif
