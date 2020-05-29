/** @file

  Header file for AMT Status Code support at PEI phase


;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corporation. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _PEI_AMT_STATUS_CODE_PPI_H_
#define _PEI_AMT_STATUS_CODE_PPI_H_

#define PEI_AMT_STATUS_CODE_PPI_GUID \
  { 0x881807D2, 0x98D1, 0x4EC9, { 0xAF, 0xA0, 0x77, 0x46, 0xC4, 0x2F, 0x24, 0x49 }}

extern EFI_GUID gPeiAmtStatusCodePpiGuid;

typedef struct _PEI_AMT_STATUS_CODE_PPI  PEI_AMT_STATUS_CODE_PPI;

/**
  Provides an interface that a software module can call to report an ASF PEI status code.
  The extension to report status code to Intel AMT, so that Intel AMT PET message will
  be sent out in PEI.

  @param[in] PeiServices          General purpose services available to every PEIM.
  @param[in] This                 This interface.
  @param[in] Type                 Indicates the type of status code being reported.
  @param[in] Value                Describes the current status of a hardware or software entity.
                                  This included information about the class and subclass that is
                                  used to classify the entity as well as an operation.
  @param[in] Instance             The enumeration of a hardware or software entity within
                                  the system. Valid instance numbers start with 1.
  @param[in] CallerId             This optional parameter may be used to identify the caller.
                                  This parameter allows the status code driver to apply different
                                  rules to different callers.
  @param[in] Data                 This optional parameter may be used to pass additional data.

  @retval EFI_SUCCESS             The function completed successfully
  @retval EFI_DEVICE_ERROR        The function should not be completed due to a device error.
**/
typedef
EFI_STATUS
(EFIAPI *PEI_AMT_STATUS_CODE_PPI_REPORT_STATUS_CODE) (
  IN  EFI_PEI_SERVICES            **PeiServices,
  IN  PEI_AMT_STATUS_CODE_PPI     *This,
  IN  EFI_STATUS_CODE_TYPE        Type,
  IN  EFI_STATUS_CODE_VALUE       Value,
  IN  UINT32                      Instance,
  IN  EFI_GUID                    *CallerId OPTIONAL,
  IN  EFI_STATUS_CODE_DATA        *Data OPTIONAL
  )
;

///
/// Intel AMT PEI Status Code PPI
/// The PEI platform status code driver should be responsible to use this interface to report
/// status code to Intel AMT. For example, memory init, memory init done, chassis intrusion.
/// So that Intel AMT driver will send corresponding PET message out.
///
struct _PEI_AMT_STATUS_CODE_PPI {
  ///
  /// The extension to report status code to Intel AMT, so that Intel AMT PET message will
  /// be sent out in PEI.
  ///
  PEI_AMT_STATUS_CODE_PPI_REPORT_STATUS_CODE  ReportStatusCode;
};

///
/// Queue Hob.
/// This hob is used by PEI Status Code generic driver before Intel AMT PEI status code driver
/// starts.
/// 1) If AMT driver is not ready yet, StatusCode driver can save information
///    to QueueHob.
/// 2) If after AMT driver start, but ME is not ready, the AMT
///    driver can also save information to QueueHob.
/// Later, when ME is ready, AMT driver will send all the
/// message in the QueueHob.
///
typedef struct {
  EFI_HOB_GUID_TYPE     EfiHobGuidType; ///< The GUID type hob header
  EFI_STATUS_CODE_VALUE Value;          ///< Status code value
  EFI_STATUS_CODE_TYPE  Type;           ///< Status code type
} AMT_PET_QUEUE_HOB;

#define AMT_PET_QUEUE_HOB_GUID \
  { 0xCA0801D3, 0xAFB1, 0x4DEC, { 0x9B, 0x65, 0x93, 0x65, 0xEC, 0xC7, 0x93, 0x6B }}

extern EFI_GUID gAmtPetQueueHobGuid;

#define AMT_FORCE_PUSH_PET_HOB_GUID \
  { 0x4EFA0DB6, 0x26DC, 0x4BB1, { 0xA7, 0x6F, 0x14, 0xBC, 0x63, 0x0C, 0x7B, 0x3C }}

typedef struct {
  EFI_HOB_GUID_TYPE EfiHobGuidType;
  INT32             MessageType;
} AMT_FORCE_PUSH_PET_HOB;

extern EFI_GUID gAmtForcePushPetHobGuid;

#endif
