/** @file
 H2O IPMI Utility Protocol header file.

 H2O IPMI Utility Protocol provides the function to execute H2O IPMI utility.

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

#ifndef _H2O_IPMI_UTILITY_PROTOCOL_H_
#define _H2O_IPMI_UTILITY_PROTOCOL_H_


//
// Variable Guid
//
#define H2O_IPMI_UTILITY_PROTOCOL_GUID \
  {0x6d8be92c, 0xd97e, 0x431d, 0x8a, 0xdf, 0xd3, 0xf8, 0x0d, 0x53, 0x51, 0x45}

typedef struct _H2O_IPMI_UTILITY_PROTOCOL H2O_IPMI_UTILITY_PROTOCOL;


/**
 Execute H2O IPMI utility.

 @param[in]         This                Pointer to H2O_IPMI_INTERFACE_PROTOCOL instance.

 @retval EFI_SUCCESS                    Execute success.
*/
typedef
EFI_STATUS
(EFIAPI *H2O_IPMI_UTILITY_PROTOCOL_EXECUTE_IPMI_UTILITY) (
  IN  H2O_IPMI_UTILITY_PROTOCOL         *This
);


//
// H2O_IPMI_UTILITY_PROTOCOL structure
//
struct _H2O_IPMI_UTILITY_PROTOCOL {
  H2O_IPMI_UTILITY_PROTOCOL_EXECUTE_IPMI_UTILITY ExecuteIpmiUtility;
};


extern EFI_GUID gH2OIpmiUtilityProtocolGuid;


#endif

