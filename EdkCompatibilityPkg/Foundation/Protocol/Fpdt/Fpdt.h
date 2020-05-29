/*++

Copyright (c) 2011, Intel Corporation                                                         
All rights reserved. This program and the accompanying materials                          
are licensed and made available under the terms and conditions of the BSD License         
which accompanies this distribution.  The full text of the license may be found at        
http://opensource.org/licenses/bsd-license.php                                            
                                                                                          
THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,                     
WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.             

Module Name:

    Fpdt.h

Abstract:

    Fpdt Performance Protocol
  
--*/

#ifndef _FPDT_H
#define _FPDT_H

#define FPDT_PERFORMANCE_PROTOCOL_GUID \
  { \
    0x444c3203, 0xf8b1, 0x42a7, 0xab, 0xe9, 0x2e, 0x58, 0x2, 0x5b, 0xe1, 0x2a \
  }

EFI_FORWARD_DECLARATION (FPDT_PERFORMANCE_PROTOCOL);

typedef
EFI_STATUS
(EFIAPI *FPDT_PERFORMANCE_PROTOCOL_GET_FPDT_ADDRESS) (
  IN FPDT_PERFORMANCE_PROTOCOL   *This,
  OUT UINT32                     *FpdtAddress
  );

typedef
EFI_STATUS
(EFIAPI *FPDT_PERFORMANCE_PROTOCOL_UPDATE_RECORD) (
  IN FPDT_PERFORMANCE_PROTOCOL              *This,
  IN EFI_HANDLE                             Handle OPTIONAL,
  IN UINT16                                 Recordtype,
  IN UINT64                                 Data,
  IN UINT16                                 Identifier OPTIONAL
  );

typedef struct _FPDT_PERFORMANCE_PROTOCOL {
  FPDT_PERFORMANCE_PROTOCOL_GET_FPDT_ADDRESS  GetFpdtAddress;
  FPDT_PERFORMANCE_PROTOCOL_UPDATE_RECORD     UpdateRecord;
} FPDT_PERFORMANCE_PROTOCOL;

extern EFI_GUID gFpdtPerformanceProtocolGuid;

#endif