/** @file

  The definition of Post Message PEI PPI.
  
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

#ifndef _EFI_PEI_POST_MESSAGE_PPI_H_
#define _EFI_PEI_POST_MESSAGE_PPI_H_

/**
  Report Status Code to PEI Post Message.

  @param  PeiServices      An indirect pointer to the EFI_PEI_SERVICES table published by the PEI Foundation.
  @param  CodeType         Indicates the type of status code being reported.
  @param  CodeValue        Describes the current status of a hardware or
                           software entity. This includes information about the class and
                           subclass that is used to classify the entity as well as an operation.
                           For progress codes, the operation is the current activity.
                           For error codes, it is the exception.For debug codes,it is not defined at this time.
  @param  Instance         The enumeration of a hardware or software entity within
                           the system. A system may contain multiple entities that match a class/subclass
                           pairing. The instance differentiates between them. An instance of 0 indicates
                           that instance information is unavailable, not meaningful, or not relevant.
                           Valid instance numbers start with 1.
  @param  CallerId         This optional parameter may be used to identify the caller.
                           This parameter allows the status code driver to apply different rules to
                           different callers.
  @param  Data             This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS     
**/
typedef
EFI_STATUS
(EFIAPI *PEI_POST_MESSAGE_STATUS_CODE) (
  IN CONST  EFI_PEI_SERVICES         **PeiServices,
  IN EFI_STATUS_CODE_TYPE            CodeType,
  IN EFI_STATUS_CODE_VALUE           Value, 
  IN UINT32                          Instance  OPTIONAL,
  IN CONST  EFI_GUID                 *CallerId OPTIONAL,
  IN CONST  EFI_STATUS_CODE_DATA     *Data     OPTIONAL
  );

typedef
struct _H2O_PEI_EVENT_MESSAGE_PPI {
  PEI_POST_MESSAGE_STATUS_CODE              PeiPostMessageStatusCode;
} H2O_PEI_POST_MESSAGE_PPI;

extern EFI_GUID                         gH2OPeiPostMessagePpiGuid;

#endif

