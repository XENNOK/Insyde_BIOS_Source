/** @file
 H2O IPMI common library header file.

 This file contains functions prototype of some commonly used functions.

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

#ifndef _IPMI_COMMON_LIB_H_
#define _IPMI_COMMON_LIB_H_


/**
 This function converts EFI_TIME structure to a timestamp.

 @param[in]         Time                EFI_TIME structure to be converted.
 @param[out]        TimeStamp           TimeStamp converted from EFI_TIME structure.
*/
VOID
EfiTimeToTimeStamp (
  IN  EFI_TIME                          *Time,
  OUT UINT32                            *TimeStamp
  );


/**
 Convert timestamp to EFI_TIME structure.

 @param[in]         TimeStamp           Timestamp to be converted.
 @param[out]        Time                EFI_TIME structure use to store the result converting from TimeStamp.
*/
VOID
TimeStampToEfiTime (
  IN  UINT32                            TimeStamp,
  OUT EFI_TIME                          *Time
  );


/**
 Check if the input string is valid IPv4 format and return the IPv4 number.

 @param[in]         InStr               The input string in IPv4 format. This can be IPv4 IP Address,
                                        IPv4 Subnet Mask and IPv4 Gateway Address.
 @param[in]         IPv4                The buffer to store IPv4 value.

 @retval TRUE       The input IPv4 string is valid IPv4 format.
 @retval FALSE      The input IPv4 string is invalid IPv4 format.
*/
BOOLEAN
IsValidIpv4 (
  IN  CHAR16                            *InStr,
  OUT UINT8                             *Ipv4
  );


/**
 Check if the input string is valid IPv6 format.

 @param[in]         InStr               The input string in IPv6 format. This can be IPv4 IP Address
                                        IPv6 Gateway Address.
 @param[in]         IPv6                The buffer to store IPv6 value.

 @retval TRUE       The input IPv6 string is valid IPv6 format.
 @retval FALSE      The input IPv6 string is invalid IPv6 format.
*/
BOOLEAN
IsValidIpv6 (
  IN  CHAR16                            *InStr,
  OUT UINT8                             *Ipv6
  );


#endif

