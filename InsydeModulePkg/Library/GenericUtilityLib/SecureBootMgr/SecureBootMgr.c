/** @file
  GenericUtilityLib

;******************************************************************************
;* Copyright (c) 2012 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#include "SecureBootMgr.h"
#include "EnrollHash.h"
#include "Password.h"

#include <KernelSetupConfig.h>
#include <Protocol/SmmBase2.h>
#include <SmiTable.h>

/**
  This function uses to check system whether supports SMM mode or not.

  @retval TRUE        System supports SMM mode
  @retval FALSE       System doesn't support SMM mode.
**/
STATIC
BOOLEAN
IsSmmModeSupported (
  VOID
  );


EFI_GUID            mSecureBootGuid = SECURE_BOOT_FORMSET_GUID;

SECURE_BOOT_MANAGER_CALLBACK_DATA  mSecureBootPrivate = {
  SECURE_BOOT_CALLBACK_DATA_SIGNATURE,
  NULL,
  NULL,
  {
    SecureBootExtractConfig,
    SecureBootRouteConfig,
    SecureBootCallback
  }
};

HII_VENDOR_DEVICE_PATH  mSecureBootHiiVendorDevicePath = {
  {
    {
      HARDWARE_DEVICE_PATH,
      HW_VENDOR_DP,
      {
        (UINT8) (sizeof (VENDOR_DEVICE_PATH)),
        (UINT8) ((sizeof (VENDOR_DEVICE_PATH)) >> 8)
      }
    },
    SECURE_BOOT_FORMSET_GUID
  },
  {
    END_DEVICE_PATH_TYPE,
    END_ENTIRE_DEVICE_PATH_SUBTYPE,
    {
      (UINT8) (END_DEVICE_PATH_LENGTH),
      (UINT8) ((END_DEVICE_PATH_LENGTH) >> 8)
    }
  }
};

UINT8    mDefaultAthenData[] = {
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x28, 0x02, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xCC, 0xdd, 0x26, 0x02, 0x97, 0x9d, 0x44, 0x1f, 0x64, 0x95, 0xf8, 0x88, 0x46, 0xef, 0x9f, 0xb2,
  0xa1, 0x6c, 0xbc, 0x89, 0x1c, 0x29, 0xad, 0x4e, 0x5f, 0x67, 0xe0, 0xdb, 0xeb, 0x2f, 0xa4, 0xc0, 0x3f, 0x44, 0x72, 0x5c, 0x93, 0x39, 0x6d, 0xe5, 0x1c, 0xe2, 0x2b, 0xcb, 0x76, 0x11, 0x71, 0x4f,
  0xc7, 0xe7, 0xa2, 0xde, 0x1e, 0x07, 0x56, 0x92, 0xfc, 0x63, 0x61, 0xf1, 0x06, 0xbb, 0x77, 0x26, 0xb8, 0x99, 0x7c, 0x0e, 0xb8, 0x81, 0x24, 0x2e, 0x72, 0x78, 0x8c, 0xfa, 0x6a, 0x71, 0x5d, 0x08,
  0x42, 0xda, 0xde, 0x86, 0xfe, 0x99, 0x92, 0xf7, 0xf8, 0xa0, 0x3d, 0x1f, 0x52, 0x0b, 0xb8, 0xc6, 0x1b, 0xd5, 0x39, 0xcb, 0x34, 0xdb, 0x2b, 0x25, 0x57, 0x44, 0x72, 0x02, 0x93, 0x67, 0x2a, 0x82,
  0x90, 0xa2, 0x5a, 0x76, 0xcb, 0x81, 0x7d, 0x42, 0x39, 0x1a, 0xf8, 0x54, 0xe0, 0x42, 0xbf, 0x49, 0x36, 0x1c, 0x4e, 0x88, 0xda, 0x5c, 0x7b, 0xd4, 0x67, 0xf6, 0x82, 0x30, 0xe4, 0x18, 0xf9, 0xb3,
  0xe8, 0xab, 0xa4, 0xca, 0x53, 0x82, 0xcf, 0xf4, 0xac, 0x83, 0x4b, 0xd6, 0xd4, 0xa1, 0xc0, 0x01, 0xfc, 0x16, 0x9d, 0xca, 0xc5, 0x17, 0x2e, 0x70, 0xff, 0xaf, 0x86, 0xc8, 0x43, 0x02, 0xde, 0x52,
  0xb1, 0x23, 0xf0, 0x2f, 0xd8, 0x0a, 0x46, 0xd3, 0xaf, 0x6b, 0x7e, 0x77, 0xf6, 0x72, 0x00, 0x66, 0x69, 0x66, 0x37, 0x71, 0x19, 0xc7, 0x7b, 0xf0, 0xd1, 0x41, 0xfd, 0x2b, 0x9b, 0xa1, 0x14, 0xb5,
  0xb3, 0x55, 0xb9, 0x96, 0x53, 0x9f, 0x29, 0x63, 0xb2, 0x38, 0x2e, 0x29, 0x10, 0x16, 0x8a, 0xbd, 0x2e, 0xed, 0x3b, 0xdc, 0xb5, 0x40, 0x6f, 0xab, 0x26, 0xea, 0xb5, 0x28, 0x36, 0x6b, 0x4a, 0x68,
  0x85, 0xd2, 0xb1, 0xff, 0xbf, 0xe6, 0x0a, 0xd3, 0x8c, 0xe5, 0x01, 0xda, 0xfd, 0x65, 0x75, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};

STRING_DEPOSITORY          *mFileOptionStrDepository;
STATIC UINT8 mEnrollHashSha256Data[] = {
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x26, 0x16, 0xc4, 0xc1, 0x4c, 0x50, 0x92, 0x40, 0xac, 0xa9, 0x41, 0xf9, 0x36, 0x93, 0x43, 0x28, 0x4c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};


STATIC UINT8 mEnrollHashSha1Data[] = {
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0xa5, 0x6c, 0x82, 0x10, 0xcf, 0xc9, 0x4a, 0xb1, 0x87, 0xbe, 0x01, 0x49, 0x66, 0x31, 0xbd, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

LIST_ENTRY        mHashLinkList;

//
// Following are the sameple code for how to update PK, KEK, db and dbx variable using Administer Secure Boot interface.
//
#if 0
STATIC UINT8 mDefaultHeaderData[] = {                                                                                                                 // gEfiCertPkcs7Guid
  0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x19, 0x00, 0x00, 0x00, 0x00, 0x02, 0xf1, 0x0e, 0x9d, 0xd2, 0xaf, 0x4a, 0xdf, 0x68, 0xee, 0x49,
                                                  //one byte for update type
  0x8a, 0xa9, 0x34, 0x7d, 0x37, 0x56, 0x65, 0xa7, 0x00
};

STATIC UINT8 mTestCert [] = {
  0x30, 0x82, 0x02, 0xF0, 0x30, 0x82, 0x01, 0xDC, 0xA0, 0x03, 0x02, 0x01, 0x02, 0x02, 0x10, 0x1D,
  0x30, 0x50, 0xAC, 0xFD, 0xFF, 0xB5, 0xB7, 0x4E, 0xFF, 0xAC, 0x38, 0x0E, 0x0F, 0x19, 0xB5, 0x30,
  0x09, 0x06, 0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1D, 0x05, 0x00, 0x30, 0x13, 0x31, 0x11, 0x30, 0x0F,
  0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x08, 0x74, 0x65, 0x73, 0x74, 0x6B, 0x65, 0x79, 0x32, 0x30,
  0x1E, 0x17, 0x0D, 0x31, 0x30, 0x30, 0x33, 0x33, 0x31, 0x31, 0x36, 0x30, 0x30, 0x30, 0x30, 0x5A,
  0x17, 0x0D, 0x32, 0x30, 0x30, 0x35, 0x32, 0x39, 0x31, 0x36, 0x30, 0x30, 0x30, 0x30, 0x5A, 0x30,
  0x13, 0x31, 0x11, 0x30, 0x0F, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x08, 0x74, 0x65, 0x73, 0x74,
  0x6B, 0x65, 0x79, 0x32, 0x30, 0x82, 0x01, 0x22, 0x30, 0x0D, 0x06, 0x09, 0x2A, 0x86, 0x48, 0x86,
  0xF7, 0x0D, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x82, 0x01, 0x0F, 0x00, 0x30, 0x82, 0x01, 0x0A,
  0x02, 0x82, 0x01, 0x01, 0x00, 0xAA, 0x2D, 0x41, 0x1A, 0xE7, 0x19, 0x3A, 0xE0, 0xD3, 0x43, 0x72,
  0x7D, 0x50, 0x02, 0x0D, 0xDF, 0x98, 0x05, 0x18, 0x45, 0xF2, 0x93, 0x51, 0x6F, 0xB6, 0x81, 0x93,
  0x2E, 0x64, 0x89, 0x31, 0x20, 0x58, 0x14, 0x30, 0x2D, 0x95, 0x1A, 0xA4, 0x40, 0x75, 0xD9, 0xEB,
  0x67, 0xAD, 0x13, 0xF4, 0x11, 0x2D, 0xDF, 0x40, 0x91, 0xED, 0xCA, 0xE7, 0xEC, 0x99, 0x7D, 0x7A,
  0x84, 0x71, 0x61, 0x9F, 0x77, 0x9A, 0xC5, 0x93, 0x8D, 0x14, 0x15, 0x5E, 0x51, 0x2B, 0xF4, 0x64,
  0x60, 0x17, 0xFB, 0x03, 0x7F, 0x3C, 0x5D, 0xE7, 0x66, 0x77, 0x67, 0xA5, 0xFC, 0xD1, 0x10, 0x3C,
  0x23, 0x91, 0x27, 0x46, 0x9A, 0x8E, 0xF3, 0x51, 0xB6, 0x90, 0x30, 0xCC, 0x6E, 0xD6, 0x15, 0x6F,
  0x77, 0x58, 0x68, 0xE7, 0x4D, 0xD2, 0x59, 0xE7, 0x55, 0x20, 0x06, 0xC7, 0xBC, 0x89, 0x66, 0x67,
  0x93, 0xDD, 0xD2, 0x26, 0x32, 0x43, 0x47, 0xEC, 0x67, 0x36, 0x66, 0x40, 0xF9, 0x46, 0x16, 0x6B,
  0xE8, 0x27, 0x1A, 0xC3, 0x73, 0xD4, 0x39, 0x08, 0x91, 0xCB, 0xA7, 0xEA, 0x03, 0x15, 0x1E, 0xF2,
  0xCF, 0x49, 0x1A, 0x1D, 0xB3, 0x99, 0x07, 0x83, 0x48, 0x5A, 0x6E, 0x56, 0xE7, 0x4D, 0x6F, 0xAD,
  0x6E, 0x84, 0x61, 0xC6, 0x04, 0xFA, 0x32, 0x01, 0x27, 0x00, 0xF0, 0x4E, 0xEF, 0x04, 0x78, 0x38,
  0xA4, 0xC7, 0x85, 0x15, 0x08, 0x40, 0x32, 0x6F, 0x64, 0x10, 0x01, 0x53, 0x75, 0x18, 0x7B, 0x60,
  0xDE, 0x66, 0x9F, 0x94, 0xF0, 0x35, 0x03, 0x07, 0x64, 0x67, 0xC3, 0x37, 0x28, 0x9D, 0x70, 0x27,
  0x98, 0x7A, 0xC0, 0x7B, 0xCD, 0x74, 0x97, 0x88, 0xB3, 0x1C, 0x86, 0xB2, 0xCF, 0x29, 0x74, 0x8B,
  0x0A, 0xEA, 0xBF, 0x7E, 0xF9, 0x1B, 0x81, 0x15, 0xE9, 0x78, 0x1C, 0xC7, 0x55, 0x25, 0xD6, 0x41,
  0x72, 0xED, 0xC4, 0x88, 0x69, 0x02, 0x03, 0x01, 0x00, 0x01, 0xA3, 0x48, 0x30, 0x46, 0x30, 0x44,
  0x06, 0x03, 0x55, 0x1D, 0x01, 0x04, 0x3D, 0x30, 0x3B, 0x80, 0x10, 0x06, 0x65, 0x0F, 0xFD, 0x3D,
  0xB0, 0xA6, 0x4C, 0xD6, 0x81, 0xE8, 0x66, 0x39, 0xA4, 0x67, 0x44, 0xA1, 0x15, 0x30, 0x13, 0x31,
  0x11, 0x30, 0x0F, 0x06, 0x03, 0x55, 0x04, 0x03, 0x13, 0x08, 0x74, 0x65, 0x73, 0x74, 0x6B, 0x65,
  0x79, 0x32, 0x82, 0x10, 0x1D, 0x30, 0x50, 0xAC, 0xFD, 0xFF, 0xB5, 0xB7, 0x4E, 0xFF, 0xAC, 0x38,
  0x0E, 0x0F, 0x19, 0xB5, 0x30, 0x09, 0x06, 0x05, 0x2B, 0x0E, 0x03, 0x02, 0x1D, 0x05, 0x00, 0x03,
  0x82, 0x01, 0x01, 0x00, 0x1C, 0x74, 0x76, 0x2D, 0x9E, 0x02, 0xB3, 0xFB, 0xFE, 0x00, 0x57, 0x07,
  0xB4, 0x55, 0x8D, 0x94, 0x24, 0x54, 0xD4, 0x03, 0x2B, 0xBB, 0x7E, 0x22, 0x6F, 0x1F, 0x38, 0x26,
  0x0A, 0x6B, 0x09, 0x02, 0xB9, 0x37, 0x98, 0x81, 0x5C, 0xF4, 0x71, 0x68, 0xF6, 0xA3, 0x7D, 0x17,
  0x18, 0xED, 0x00, 0x1D, 0x58, 0xA1, 0xD2, 0x6D, 0x81, 0xF0, 0xC0, 0x27, 0x6C, 0x8F, 0x08, 0xEC,
  0x00, 0x64, 0xB0, 0xD4, 0x8A, 0x47, 0x70, 0xFD, 0xE2, 0xFC, 0xAF, 0x87, 0x08, 0xC9, 0x23, 0x2F,
  0xA1, 0x65, 0xC1, 0xA1, 0x70, 0xF1, 0xB0, 0x76, 0x68, 0x7C, 0x6C, 0xEC, 0x24, 0x71, 0x1F, 0x39,
  0x97, 0x2E, 0xC1, 0xCD, 0x50, 0x22, 0x5B, 0x26, 0xDD, 0xDD, 0xAB, 0x97, 0x83, 0x65, 0x14, 0xBA,
  0x30, 0x1F, 0x5E, 0xDF, 0x27, 0x17, 0xDA, 0x2F, 0xE6, 0x81, 0x9A, 0x44, 0xAC, 0xD2, 0x72, 0x63,
  0x4C, 0x8D, 0x63, 0xF5, 0x24, 0x61, 0xA4, 0x52, 0x22, 0x57, 0x42, 0x1C, 0x35, 0x00, 0xCE, 0xED,
  0xAB, 0xC2, 0x53, 0xAB, 0xE0, 0x1B, 0x04, 0x43, 0x2D, 0xC9, 0x94, 0xB3, 0x02, 0xB6, 0x26, 0x53,
  0xC2, 0xF7, 0xB9, 0xA6, 0xB9, 0x10, 0xFB, 0xAF, 0x88, 0x0A, 0xE2, 0xBE, 0x67, 0x5E, 0xAE, 0x9D,
  0xB9, 0x20, 0x9E, 0xFD, 0x52, 0x15, 0x59, 0x34, 0x40, 0x65, 0xDB, 0xB3, 0xEB, 0x9E, 0x69, 0x5F,
  0xB6, 0x8C, 0x86, 0xF9, 0xA1, 0x5F, 0xD7, 0x8C, 0xAA, 0x5A, 0x98, 0x48, 0x11, 0x95, 0x05, 0xCA,
  0x7B, 0xDB, 0xD0, 0x3A, 0xC0, 0x8A, 0x1D, 0xF1, 0x4C, 0x92, 0x80, 0x90, 0xAA, 0x58, 0x81, 0x60,
  0x7F, 0xEC, 0x86, 0x69, 0x58, 0xE8, 0xAF, 0xDA, 0xAB, 0x06, 0xED, 0xAA, 0x2A, 0xEB, 0x63, 0xD5,
  0x46, 0xF9, 0x51, 0x3B, 0x76, 0x55, 0x0B, 0xE3, 0xFE, 0xD5, 0x53, 0x36, 0x14, 0x25, 0xEA, 0x09,
  0xEC, 0x7C, 0x17, 0x54
};


/**
  Internal function uses input certificate, certificate size and update type to create
  output EFI_VARIABLE_AUTHENTICATION_2 descriptor.

  @param[in]  InputBuffer  The content of input certificate.
  @param[in]  InputSize    The size in bytes of the input certificate.
  @param[in]  OwnerGuid    Input owner guid for the input signature owner. If OwnerGuid is NULL, the owner GUID
                           will be set to all ZERO.
  @param[in]  UpdateType   The Input update type. This update type should be UPDATE_AUTHENTICATED_VARIABLE
                           or APPEND_AUTHENTICATED_VARIABLE.
  @param[out] OutputBuffer A pointer to the output buffer to save the EFI_VARIABLE_AUTHENTICATION_2 descriptor.
                           This buffer is allocated with a call to  the Boot Service AllocatePool().  It is the
                           caller's responsibility to call the Boot Service FreePool() when the caller no longer
                           requires the contents of Buffer.
  @param[out] OutPutSize   The size in bytes of the EFI_VARIABLE_AUTHENTICATION_2 descriptor.

  @retval EFI_SUCCESS             Create EFI_VARIABLE_AUTHENTICATION_2 descriptor successfully.
  @retval EFI_INVALID_PARAMETER   Any input parameter in incorrect.
  @retval EFI_OUT_OF_RESOURCES    There is not enough pool memory to store the EFI_VARIABLE_AUTHENTICATION_2 descriptor.
**/
STATIC
EFI_STATUS
CreateAuthenData (
  IN  UINT8      *InputBuffer,
  IN  UINTN      InputSize,
  IN  EFI_GUID   *OwnerGuid, OPTIONAL
  IN  UINTN      UpdateType,
  OUT UINT8      **OutputBuffer,
  OUT UINTN      *OutPutSize
  )
{
  UINTN                   BufferSize;
  UINT8                   *Buffer;
  EFI_SIGNATURE_LIST      *SigList;
  EFI_SIGNATURE_DATA      *SigData;

  if (OutputBuffer == NULL || OutPutSize == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  if (UpdateType != UPDATE_AUTHENTICATED_VARIABLE && UpdateType != APPEND_AUTHENTICATED_VARIABLE) {
    return EFI_INVALID_PARAMETER;
  }
  if (InputBuffer == NULL) {
    InputSize = 0;
  }

  if (InputSize != 0) {
    BufferSize = sizeof (mDefaultHeaderData) + sizeof (EFI_SIGNATURE_LIST) + sizeof (EFI_SIGNATURE_DATA) - 1 + InputSize;
  } else {
    BufferSize = sizeof (mDefaultHeaderData);
  }
  Buffer = AllocateZeroPool (BufferSize);
  if (Buffer == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }
  //
  // Update header data and set udpate type (update or append)
  //
  CopyMem (Buffer, mDefaultHeaderData, sizeof (mDefaultHeaderData));
  *(Buffer + sizeof (mDefaultHeaderData) - sizeof (UINT8)) = (UINT8) UpdateType;

  if (InputSize != 0) {
    SigList = (EFI_SIGNATURE_LIST *) (Buffer + sizeof (mDefaultHeaderData));
    CopyMem (&SigList->SignatureType, &gEfiCertX509Guid, sizeof (EFI_GUID));
    SigList->SignatureListSize = (UINT32) (InputSize + sizeof (EFI_SIGNATURE_LIST) + sizeof (EFI_SIGNATURE_DATA) - 1);
    SigList->SignatureSize = (UINT32) (InputSize + sizeof (EFI_GUID));

    SigData = (EFI_SIGNATURE_DATA*) (SigList + 1);
    //
    // Set signature owner according to input owner GUID. Otherwise, keep this signature owner GUID
    // as all zero.
    //
    if (OwnerGuid != NULL) {
      CopyMem (&SigData->SignatureOwner, OwnerGuid, sizeof (EFI_GUID));
    }
    CopyMem (SigData->SignatureData, InputBuffer, InputSize);
  }
  *OutputBuffer = Buffer;
  *OutPutSize = BufferSize;
  return EFI_SUCCESS;
}

/**
  Function uses input certificate and certificate size to update PK variable through SMI.
  This update will not do any certification check no matter system is in setup mode or
  user mode.

  @param[in]  InputBuffer  The content of input certificate.
  @param[in]  InputSize    The size in bytes of the input certificate.
  @param[in]  OwnerGuid    Input owner guid for the input signature owner. If OwnerGuid is NULL, the owner GUID
                           will be set to all ZERO.

  @retval EFI_SUCCESS             Update input certificate to PK variable successfully.
  @retval EFI_INVALID_PARAMETER   Any input parameter in incorrect.
  @retval EFI_OUT_OF_RESOURCES    There is not enough pool memory to store the EFI_VARIABLE_AUTHENTICATION_2 descriptor.
**/
EFI_STATUS
UpdatePkVariable (
  IN  UINT8      *InputBuffer,
  IN  UINTN      InputSize,
  IN  EFI_GUID   *OwnerGuid OPTIONAL
  )
{
  UINT8         *Buffer;
  UINTN         BufferSize;
  EFI_STATUS    Status;

  Status = CreateAuthenData (InputBuffer, InputSize, OwnerGuid, UPDATE_AUTHENTICATED_VARIABLE, &Buffer, &BufferSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (IsSmmModeSupported ()) {
    SmmSecureBootCall (
      Buffer,
      BufferSize,
      UPDATE_PK_FUN_NUM,
      SW_SMI_PORT
      );
  } else {
    Status = gRT->SetVariable (
                    EFI_PLATFORM_KEY_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS,
                    BufferSize,
                    Buffer
                    );
  }
  gBS->FreePool (Buffer);
  return Status;
}


/**
  Function uses input certificate and certificate size to update KEK variable through SMI.
  This update will not do any certification check no matter system is in setup mode or
  user mode.

  @param[in]  InputBuffer  The content of input certificate.
  @param[in]  InputSize    The size in bytes of the input certificate.
  @param[in]  OwnerGuid    Input owner guid for the input signature owner. If OwnerGuid is NULL, the owner GUID
                           will be set to all ZERO.
  @param[in]  UpdateType   The Input update type. This update type should be UPDATE_AUTHENTICATED_VARIABLE
                           or APPEND_AUTHENTICATED_VARIABLE.

  @retval EFI_SUCCESS             Update input certificate to KEK variable successfully.
  @retval EFI_INVALID_PARAMETER   Any input parameter in incorrect.
  @retval EFI_OUT_OF_RESOURCES    There is not enough pool memory to store the EFI_VARIABLE_AUTHENTICATION_2 descriptor.
**/
EFI_STATUS
UpdateKekVariable (
  IN  UINT8      *InputBuffer,
  IN  UINTN      InputSize,
  IN  EFI_GUID   *OwnerGuid, OPTIONAL
  IN  UINTN      UpdateType
  )
{
  UINT8         *Buffer;
  UINTN         BufferSize;
  EFI_STATUS    Status;
  UINT32        Attribute;

  if (UpdateType != UPDATE_AUTHENTICATED_VARIABLE && UpdateType != APPEND_AUTHENTICATED_VARIABLE) {
    return EFI_INVALID_PARAMETER;
  }

  Status = CreateAuthenData (InputBuffer, InputSize, OwnerGuid, UpdateType, &Buffer, &BufferSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (IsSmmModeSupported ()) {
    SmmSecureBootCall (
      Buffer,
      BufferSize,
      UPDATE_KEK_FUN_NUM,
      SW_SMI_PORT
    );
  } else {
    Attribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
    if (UpdateType == APPEND_AUTHENTICATED_VARIABLE) {
      Attribute |= EFI_VARIABLE_APPEND_WRITE;
    }
    Status = gRT->SetVariable (
                    EFI_KEY_EXCHANGE_KEY_NAME,
                    &gEfiGenericVariableGuid,
                    Attribute,
                    BufferSize,
                    Buffer
                    );
  }
  gBS->FreePool (Buffer);
  return Status;
}

/**
  Function uses input certificate and certificate size to update db variable through SMI.
  This update will not do any certification check no matter system is in setup mode or
  user mode.

  @param[in]  InputBuffer  The content of input certificate.
  @param[in]  InputSize    The size in bytes of the input certificate.
  @param[in]  OwnerGuid    Input owner guid for the input signature owner. If OwnerGuid is NULL, the owner GUID
                           will be set to all ZERO.
  @param[in]  UpdateType   The Input update type. This update type should be UPDATE_AUTHENTICATED_VARIABLE
                           or APPEND_AUTHENTICATED_VARIABLE.

  @retval EFI_SUCCESS             Update input certificate to db variable successfully.
  @retval EFI_INVALID_PARAMETER   Any input parameter in incorrect.
  @retval EFI_OUT_OF_RESOURCES    There is not enough pool memory to store the EFI_VARIABLE_AUTHENTICATION_2 descriptor.
**/
EFI_STATUS
UpdateDbVariable (
  IN  UINT8      *InputBuffer,
  IN  UINTN      InputSize,
  IN  EFI_GUID   *OwnerGuid, OPTIONAL
  IN  UINTN      UpdateType
  )
{
  UINT8         *Buffer;
  UINTN         BufferSize;
  EFI_STATUS    Status;
  UINT32        Attribute;

  if (UpdateType != UPDATE_AUTHENTICATED_VARIABLE && UpdateType != APPEND_AUTHENTICATED_VARIABLE) {
    return EFI_INVALID_PARAMETER;
  }

  Status = CreateAuthenData (InputBuffer, InputSize, OwnerGuid, UpdateType, &Buffer, &BufferSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (IsSmmModeSupported ()) {
    SmmSecureBootCall (
      Buffer,
      BufferSize,
      UPDATE_DB_FUN_NUM,
      SW_SMI_PORT
    );
  } else {
    Attribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
    if (UpdateType == APPEND_AUTHENTICATED_VARIABLE) {
      Attribute |= EFI_VARIABLE_APPEND_WRITE;
    }
    Status = gRT->SetVariable (
                    EFI_IMAGE_SECURITY_DATABASE,
                    &gEfiGenericVariableGuid,
                    Attribute,
                    BufferSize,
                    Buffer
                    );
  }
  gBS->FreePool (Buffer);
  return EFI_SUCCESS;
}

/**
  Function uses input certificate and certificate size to update dbx variable through SMI.
  This update will not do any certification check no matter system is in setup mode or
  user mode.

  @param[in]  InputBuffer  The content of input certificate.
  @param[in]  InputSize    The size in bytes of the input certificate.
  @param[in]  OwnerGuid    Input owner guid for the input signature owner. If OwnerGuid is NULL, the owner GUID
                           will be set to all ZERO.
  @param[in]  UpdateType   The Input update type. This update type should be UPDATE_AUTHENTICATED_VARIABLE
                           or APPEND_AUTHENTICATED_VARIABLE.

  @retval EFI_SUCCESS             Update input certificate to dbx variable successfully.
  @retval EFI_INVALID_PARAMETER   Any input parameter in incorrect.
  @retval EFI_OUT_OF_RESOURCES    There is not enough pool memory to store the EFI_VARIABLE_AUTHENTICATION_2 descriptor.
**/
EFI_STATUS
UpdateDbxVariable (
  IN  UINT8      *InputBuffer,
  IN  UINTN      InputSize,
  IN  EFI_GUID   *OwnerGuid, OPTIONAL
  IN  UINTN      UpdateType
  )
{
  UINT8         *Buffer;
  UINTN         BufferSize;
  EFI_STATUS    Status;
  UINT32        Attribute;

  if (UpdateType != UPDATE_AUTHENTICATED_VARIABLE && UpdateType != APPEND_AUTHENTICATED_VARIABLE) {
    return EFI_INVALID_PARAMETER;
  }

  Status = CreateAuthenData (InputBuffer, InputSize, OwnerGuid, UpdateType, &Buffer, &BufferSize);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  if (IsSmmModeSupported ()) {
    SmmSecureBootCall (
      Buffer,
      BufferSize,
      UPDATE_DBX_FUN_NUM,
      SW_SMI_PORT
    );
  } else {
    Attribute = EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
                EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS;
    if (UpdateType == APPEND_AUTHENTICATED_VARIABLE) {
      Attribute |= EFI_VARIABLE_APPEND_WRITE;
    }
    Status = gRT->SetVariable (
                    EFI_IMAGE_SECURITY_DATABASE1,
                    &gEfiGenericVariableGuid,
                    Attribute,
                    BufferSize,
                    Buffer
                    );
  }
  gBS->FreePool (Buffer);
  return EFI_SUCCESS;
}
#endif
/**
  This function uses to check system whether supports SMM mode or not.

  @retval TRUE        System supports SMM mode
  @retval FALSE       System doesn't support SMM mode.
**/
STATIC
BOOLEAN
IsSmmModeSupported (
  VOID
  )
{
  EFI_SMM_BASE2_PROTOCOL              *SmmBase;
  BOOLEAN                             SmmSupport;
  EFI_STATUS                          Status;

  SmmSupport =FALSE;
  Status = gBS->LocateProtocol (
                  &gEfiSmmBase2ProtocolGuid,
                  NULL,
                  (VOID **) &SmmBase
                  );
  if (!EFI_ERROR (Status)) {
    SmmSupport = TRUE;
  }

  return SmmSupport;
}

/**
  This function uses to save all of administer secure boot settings

  @param  This         Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.

  @retval EFI_SUCCESS              The Results is processed successfully.
  @retval EFI_INVALID_PARAMETER    Configuration is NULL.

**/
STATIC
EFI_STATUS
SecureBootSaveChange (
  IN  EFI_HII_CONFIG_ACCESS_PROTOCOL         *This
  )
{
  SECURE_BOOT_MANAGER_CALLBACK_DATA   *PrivateData;
  UINTN                               BufferSize;
  UINT8                               EnforceSecureBoot;
  UINT8                               ResotreFactory;
  LIST_ENTRY                          *Link;
  HASH_LINK_LIST                      *HashLink;

  if (This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  PrivateData = SECURE_BOOT_CALLBACK_DATA_FROM_THIS (This);
  HiiGetBrowserData (NULL, NULL, sizeof (SECURE_BOOT_NV_DATA), (UINT8 *) &PrivateData->SecureBootData);
  if (PrivateData->SecureBootData.EnforceSecureBoot != PrivateData->BackupSecureBootData.EnforceSecureBoot) {
    EnforceSecureBoot = PrivateData->SecureBootData.EnforceSecureBoot;
    BufferSize = sizeof (mDefaultAthenData);
    mDefaultAthenData[BufferSize - 1] = (EnforceSecureBoot == 1) ? 1 : 0;
    if (IsSmmModeSupported ()) {
      SmmSecureBootCall (
        mDefaultAthenData,
        BufferSize,
        SECURE_BOOT_ENFORCE_FUN_NUM,
        SW_SMI_PORT
      );
    } else {
      gRT->SetVariable (
             EFI_SECURE_BOOT_ENFORCE_NAME,
             &gEfiGenericVariableGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS,
             BufferSize,
             mDefaultAthenData
             );
    }
    PrivateData->BackupSecureBootData.EnforceSecureBoot = PrivateData->SecureBootData.EnforceSecureBoot;
  }

  if (PrivateData->SecureBootData.ClearSecureSettings == 1) {
    if (IsSmmModeSupported ()) {
      SmmSecureBootCall (
        NULL,
        0,
        CLEAR_ALL_SECURE_SETTINGS_FUN_NUM,
        SW_SMI_PORT
      );
    } else {
      BufferSize = sizeof (mDefaultAthenData);
      mDefaultAthenData[BufferSize - 1] = 1;
      gRT->SetVariable (
             EFI_CLEAR_ALL_SECURE_SETTINGS_NAME,
             &gEfiGenericVariableGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS,
             BufferSize,
             mDefaultAthenData
             );
    }
  }


  while (!IsListEmpty (&mHashLinkList)) {
    Link = mHashLinkList.ForwardLink;
    HashLink = CR (Link, HASH_LINK_LIST, Link, HASH_LINK_LIST_SIGNATURE);
    if (HashLink->ShaType == HASHALG_SHA256) {
      BufferSize = sizeof (mEnrollHashSha256Data);
      CopyMem (
        (VOID *) &mEnrollHashSha256Data[BufferSize - sizeof (EFI_SHA256_HASH)],
        (VOID*) HashLink->Hash,
        sizeof (EFI_SHA256_HASH)
        );
      if (IsSmmModeSupported ()) {
        SmmSecureBootCall (
          mEnrollHashSha256Data,
          BufferSize,
          ADD_HASH_IMAGE_FUN_NUM,
          SW_SMI_PORT
          );
      } else {
        gRT->SetVariable (
               EFI_ADD_HASH_IMMAGE_NAME,
               &gEfiGenericVariableGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS
               | EFI_VARIABLE_APPEND_WRITE,
               BufferSize,
               mEnrollHashSha256Data
               );
      }
    } else {
      BufferSize = sizeof (mEnrollHashSha1Data);
      CopyMem (
        (VOID *) &mEnrollHashSha1Data[BufferSize - sizeof (EFI_SHA1_HASH)],
        (VOID*) HashLink->Hash,
        sizeof (EFI_SHA1_HASH)
        );
      if (IsSmmModeSupported ()) {
        SmmSecureBootCall (
          mEnrollHashSha1Data,
          BufferSize,
          ADD_HASH_IMAGE_FUN_NUM,
          SW_SMI_PORT
          );
      } else {
        gRT->SetVariable (
               EFI_ADD_HASH_IMMAGE_NAME,
               &gEfiGenericVariableGuid,
               EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
               EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_TIME_BASED_AUTHENTICATED_WRITE_ACCESS
               | EFI_VARIABLE_APPEND_WRITE,
               BufferSize,
               mEnrollHashSha1Data
               );

      }
    }
    RemoveEntryList (Link);
    gBS->FreePool (HashLink);

  }
  //
  // Move saved restore factory default to the last. This action can make sure system can set CustomSecurity variable to 0 correctly,
  // even if user also changes ClearSecureSettings or add hash to db.
  //
  if (PrivateData->SecureBootData.ResotreFactorySettings != PrivateData->BackupSecureBootData.ResotreFactorySettings) {
    ResotreFactory = PrivateData->SecureBootData.ResotreFactorySettings;
    BufferSize = sizeof (mDefaultAthenData);
    mDefaultAthenData[BufferSize - 1] = (ResotreFactory == 1) ? 1 : 0;
    if (IsSmmModeSupported ()) {
      SmmSecureBootCall (
        mDefaultAthenData,
        BufferSize,
        RESTORE_FACOTRY_DEFAULT_FUN_NUM,
        SW_SMI_PORT
      );
    } else {
      gRT->SetVariable (
             EFI_RESTORE_FACOTRY_DEFAULT_NAME,
             &gEfiGenericVariableGuid,
             EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS |
             EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_AUTHENTICATED_WRITE_ACCESS,
             BufferSize,
             mDefaultAthenData
             );
    }
    PrivateData->BackupSecureBootData.ResotreFactorySettings = PrivateData->SecureBootData.ResotreFactorySettings;
  }
  return EFI_SUCCESS;
}


/**
  Initialize all of system status in Administer Secure Boot menu.

  @retval EFI_SUCCESS      Initialize secure boot system status successful.
  @retval EFI_NOT_FOUND    Cannot find any variable for secure boot system status.

**/
EFI_STATUS
InitializeSystemStatus (
  VOID
  )
{
  UINTN               BufferSize;
  UINT8               Data;
  EFI_STATUS          Status;

  BufferSize = sizeof (UINT8);
  Status = gRT->GetVariable (
                  EFI_SETUP_MODE_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &BufferSize,
                  &Data
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mSecureBootPrivate.SecureBootData.SetupMode = Data;
  mSecureBootPrivate.BackupSecureBootData.SetupMode = Data;

  BufferSize = sizeof (UINT8);
  Status = gRT->GetVariable (
                  EFI_SECURE_BOOT_MODE_NAME,
                  &gEfiGlobalVariableGuid,
                  NULL,
                  &BufferSize,
                  &Data
                  );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mSecureBootPrivate.SecureBootData.SecureBoot = Data;
  mSecureBootPrivate.BackupSecureBootData.SecureBoot = Data;

  BufferSize = sizeof (UINT8);
  Status = gRT->GetVariable (
                  EFI_CUSTOM_SECURITY_NAME,
                  &gEfiGenericVariableGuid,
                  NULL,
                  &BufferSize,
                  &Data
                   );
  if (EFI_ERROR (Status)) {
    return Status;
  }
  mSecureBootPrivate.SecureBootData.CustomSecurity = Data;
  mSecureBootPrivate.BackupSecureBootData.CustomSecurity = Data;

  return EFI_SUCCESS;
}


/**
  Initialize HII information for the FrontPage

  @retval EFI_SUCCESS   Initialize HII information successful

**/
EFI_STATUS
InitializeSecureBoot (
  VOID
  )
{
  EFI_STATUS                  Status;
  UINTN                       BufferSize;
  UINT8                       Data;

  //
  // Install Device Path Protocol and Config Access protocol to driver handle
  //
  Status = gBS->InstallMultipleProtocolInterfaces (
                  &mSecureBootPrivate.DriverHandle,
                  &gEfiDevicePathProtocolGuid,
                  &mSecureBootHiiVendorDevicePath,
                  &gEfiHiiConfigAccessProtocolGuid,
                  &mSecureBootPrivate.ConfigAccess,
                  NULL
                  );
  ASSERT_EFI_ERROR (Status);

  //
  // Publish our HII data
  //
  mSecureBootPrivate.HiiHandle = HiiAddPackages (
                                   &mSecureBootGuid,
                                   mSecureBootPrivate.DriverHandle,
                                   SecureBootMgrVfrBin,
                                   GenericUtilityLibStrings,
                                   GenericUtilityLibImages,
                                   NULL
                                   );
  if (mSecureBootPrivate.HiiHandle == NULL) {
    return EFI_OUT_OF_RESOURCES;
  }

  Status = gBS->LocateProtocol (
                  &gH2ODialogProtocolGuid,
                  NULL,
                  (VOID **) &mSecureBootPrivate.H2ODialog
                  );
  if (EFI_ERROR(Status)) {
    return Status;
  }

  Status = InitializeSystemStatus ();
  if (EFI_ERROR(Status)) {
    return Status;
  }

  //
  // Initialize secure boot data
  //
  mSecureBootPrivate.SecureBootData.EnforceSecureBoot = 1;
  mSecureBootPrivate.BackupSecureBootData.EnforceSecureBoot = 1;
  BufferSize = sizeof (UINT8);
  Status = gRT->GetVariable (
                  EFI_SECURE_BOOT_ENFORCE_NAME,
                  &gEfiGenericVariableGuid,
                  NULL,
                  &BufferSize,
                  &Data
                  );
  if (!EFI_ERROR (Status) && Data == 0) {
    mSecureBootPrivate.SecureBootData.EnforceSecureBoot = 0;
    mSecureBootPrivate.BackupSecureBootData.EnforceSecureBoot = 0;
  }

  InitializeListHead (&mFsOptionMenu.Head);
  InitializeListHead (&mDirectoryMenu.Head);
  mFileOptionStrDepository = AllocateZeroPool (sizeof (STRING_DEPOSITORY));
  InitializeListHead (&mHashLinkList);

  return EFI_SUCCESS;
}


/**
  This function allows a caller to extract the current configuration for one
  or more named elements from the target driver.

  @param  This         Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param  Request      A null-terminated Unicode string in <ConfigRequest> format.
  @param  Progress     On return, points to a character in the Request string.
                       Points to the string's null terminator if request was successful.
                       Points to the most recent '&' before the first failing name/value
                       pair (or the beginning of the string if the failure is in the
                       first name/value pair) if the request was not successful.
  @param  Results      A null-terminated Unicode string in <ConfigAltResp> format which
                       has all values filled in for the names in the Request string.
                       String to be allocated by the called function.

  @retval EFI_SUCCESS             The Results is filled with the requested values.
  @retval EFI_OUT_OF_RESOURCES    Not enough memory to store the results.
  @retval EFI_INVALID_PARAMETER   Request is NULL, illegal syntax, or unknown name.
  @retval EFI_NOT_FOUNDv          Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
SecureBootExtractConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Request,
  OUT EFI_STRING                             *Progress,
  OUT EFI_STRING                             *Results
  )
{
  EFI_STATUS                          Status;
  UINTN                               BufferSize;
  EFI_STRING                          ConfigRequestHdr;
  EFI_STRING                          ConfigRequest;
  BOOLEAN                             AllocatedRequest;
  UINTN                               Size;
  EFI_HII_CONFIG_ROUTING_PROTOCOL     *HiiConfigRouting;
  SECURE_BOOT_MANAGER_CALLBACK_DATA   *PrivateData;

  PrivateData = SECURE_BOOT_CALLBACK_DATA_FROM_THIS (This);

  if (Progress == NULL || Results == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Request;
  if ((Request != NULL) && !HiiIsConfigHdrMatch (Request, &mSecureBootGuid, SECURE_BOOT_DATA_NAME)) {
    return EFI_NOT_FOUND;
  }
  ConfigRequestHdr = NULL;
  ConfigRequest    = NULL;
  AllocatedRequest = FALSE;
  Size             = 0;

  //
  // Convert buffer data to <ConfigResp> by helper function BlockToConfig()
  //
  BufferSize = sizeof (SECURE_BOOT_NV_DATA);
  ConfigRequest = Request;
  if ((Request == NULL) || (StrStr (Request, L"OFFSET") == NULL)) {
    //
    // Request has no request element, construct full request string.
    // Allocate and fill a buffer large enough to hold the <ConfigHdr> template
    // followed by "&OFFSET=0&WIDTH=WWWWWWWWWWWWWWWW" followed by a Null-terminator
    //
    ConfigRequestHdr = HiiConstructConfigHdr (
                         &mSecureBootGuid,
                         SECURE_BOOT_DATA_NAME,
                         mSecureBootPrivate.DriverHandle
                         );
    if (ConfigRequestHdr == NULL) {
      return EFI_OUT_OF_RESOURCES;
    }
    Size = (StrLen (ConfigRequestHdr) + 32 + 1) * sizeof (CHAR16);
    ConfigRequest = AllocateZeroPool (Size);
    ASSERT (ConfigRequest != NULL);
    AllocatedRequest = TRUE;
    UnicodeSPrint (ConfigRequest, Size, L"%s&OFFSET=0&WIDTH=%016LX", ConfigRequestHdr, (UINT64)BufferSize);
    gBS->FreePool (ConfigRequestHdr);
  }
  Status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &HiiConfigRouting);
  if (EFI_ERROR (Status)) {
    return Status;
  }
  Status = HiiConfigRouting->BlockToConfig (
                               HiiConfigRouting,
                               ConfigRequest,
                               (UINT8 *) &PrivateData->SecureBootData,
                               BufferSize,
                               Results,
                               Progress
                               );
  //
  // Free the allocated config request string.
  //
  if (AllocatedRequest) {
    gBS->FreePool (ConfigRequest);
    ConfigRequest = NULL;
  }
  //
  // Set Progress string to the original request string.
  //
  if (Request == NULL) {
    *Progress = NULL;
  } else if (StrStr (Request, L"OFFSET") == NULL) {
    *Progress = Request + StrLen (Request);
  }

  return Status;
}


/**
  This function processes the results of changes in configuration.

  @param  This             Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param  Configuration    A null-terminated Unicode string in <ConfigResp> format.
  @param  Progress         A pointer to a string filled in with the offset of the most
                           recent '&' before the first failing name/value pair (or the
                           beginning of the string if the failure is in the first
                           name/value pair) or the terminating NULL if all was successful.

  @retval EFI_SUCCESS             The Results is processed successfully.
  @retval EFI_INVALID_PARAMETER   This or Configuration or Progress is NULL.
  @retval EFI_NOT_FOUND           Routing data doesn't match any storage in this driver.

**/
EFI_STATUS
EFIAPI
SecureBootRouteConfig (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  CONST EFI_STRING                       Configuration,
  OUT EFI_STRING                             *Progress
  )
{

  if (Configuration == NULL || Progress == NULL || This == NULL) {
    return EFI_INVALID_PARAMETER;
  }

  *Progress = Configuration;
  if (!HiiIsConfigHdrMatch (Configuration, &mSecureBootGuid, SECURE_BOOT_DATA_NAME)) {
    return EFI_NOT_FOUND;
  }

  *Progress = Configuration + StrLen (Configuration);
  return SecureBootSaveChange ((EFI_HII_CONFIG_ACCESS_PROTOCOL *) This);
}


/**
  This function processes the results of changes in configuration.

  @param  This            Points to the EFI_HII_CONFIG_ACCESS_PROTOCOL.
  @param  Action          Specifies the type of action taken by the browser.
  @param  QuestionId      A unique value which is sent to the original exporting driver
                          so that it can identify the type of data to expect.
  @param  Type            The type of value for the question.
  @param  Value           A pointer to the data being sent to the original exporting driver.
  @param  ActionRequest   On return, points to the action requested by the callback function.

  @retval EFI_SUCCESS            The callback successfully handled the action.
  @retval EFI_OUT_OF_RESOURCES   Not enough storage is available to hold the variable and its data.
  @retval EFI_DEVICE_ERROR       The variable could not be saved.
  @retval EFI_UNSUPPORTED        The specified Action is not supported by the callback.

**/
EFI_STATUS
EFIAPI
SecureBootCallback (
  IN  CONST EFI_HII_CONFIG_ACCESS_PROTOCOL   *This,
  IN  EFI_BROWSER_ACTION                     Action,
  IN  EFI_QUESTION_ID                        QuestionId,
  IN  UINT8                                  Type,
  IN  EFI_IFR_TYPE_VALUE                     *Value,
  OUT EFI_BROWSER_ACTION_REQUEST             *ActionRequest
  )
{
  SECURE_BOOT_MANAGER_CALLBACK_DATA     *PrivateData;
  UINTN                                 BufferSize;
  EFI_STATUS                            Status;
  CHAR16                                *StringPtr;
  EFI_INPUT_KEY                         Key;

  if (!FeaturePcdGet (PcdH2OFormBrowserSupported) && Action == EFI_BROWSER_ACTION_CHANGING) {
    //
    // For compatible to old form browser which only use EFI_BROWSER_ACTION_CHANGING action,
    // change action to EFI_BROWSER_ACTION_CHANGED to make it workable.
    //
    Action = EFI_BROWSER_ACTION_CHANGED;
  }

  if (QuestionId == KEY_ENROLL_HASH && Action == EFI_BROWSER_ACTION_CHANGING) {
    //
    // Because form browser only send changing action for EFI_IFR_REF_OP opcode, change action to changed to make
    // the process of EFI_IFR_REF_OP opcode can be executed.
    //
    Action = EFI_BROWSER_ACTION_CHANGED;
  }

  if (Action != EFI_BROWSER_ACTION_CHANGED) {
    //
    // All other action return unsupported.
    //
    return EFI_UNSUPPORTED;
  }

  PrivateData = SECURE_BOOT_CALLBACK_DATA_FROM_THIS (This);
  BufferSize = sizeof (SECURE_BOOT_NV_DATA);

  Status = HiiGetBrowserData (
             NULL,
             NULL,
             BufferSize,
             (UINT8 *) &PrivateData->SecureBootData
             );

  switch (QuestionId) {

  case KEY_ENROLL_HASH:
    InitializeEnrollMenu (PrivateData);
    break;

  case KEY_SCAN_ESC:
    StringPtr = HiiGetString (
                  PrivateData->HiiHandle,
                  STRING_TOKEN (STR_EXIT_MENU_STRING),
                  NULL
                  );

    PrivateData->H2ODialog->ConfirmDialog (
                                   DlgYesNo,
                                   FALSE,
                                   0,
                                   NULL,
                                   &Key,
                                   StringPtr
                                   );
    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;
    }

    gBS->FreePool (StringPtr);
    break;

  case KEY_SCAN_F10:
    StringPtr = HiiGetString (
                  PrivateData->HiiHandle,
                  STRING_TOKEN (STR_SAVE_CHANGE_STRING),
                  NULL
                  );

    PrivateData->H2ODialog->ConfirmDialog (
                                   DlgYesNo,
                                   FALSE,
                                   0,
                                   NULL,
                                   &Key,
                                   StringPtr
                                   );

    if (Key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
      SecureBootSaveChange ((EFI_HII_CONFIG_ACCESS_PROTOCOL *) This);
      *ActionRequest = EFI_BROWSER_ACTION_REQUEST_EXIT;
    }
    gBS->FreePool (StringPtr);
    break;

  default:

    if (QuestionId >= ENROLL_FILE_OPTION_OFFSET && UpdateEmFileExplorer (PrivateData, QuestionId)) {
      PrivateData->EmDisplayContext  = EM_UNKNOWN_CONTEXT;
    }
    break;
  }

  return EFI_SUCCESS;
}


/**
  Provide user interface to control secure boot relative features

**/
VOID
CallSecureBootMgr (
  VOID
  )
{
  EFI_BROWSER_ACTION_REQUEST     ActionRequest;
  EFI_HII_HANDLE                 HiiHandle;
  EFI_STATUS                     Status;
  UINT8                          SecureBootEnable;
  CHAR16                         *StringPtr;
  EFI_INPUT_KEY                  Key;
  EFI_FORM_BROWSER2_PROTOCOL     *FormBrowser2;
  CHAR16                              *NewString;
  EFI_INPUT_KEY                       HotKey;
  EFI_FORM_BROWSER_EXTENSION_PROTOCOL *FormBrowserEx;

  if (IsAdministerSecureBootSupport ()) {
    //
    // Clear EFI_ADMINISTER_SECURE_BOOT_NAME variable
    //
    Status = gRT->SetVariable (
                    EFI_ADMINISTER_SECURE_BOOT_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    0,
                    NULL
                    );

  if (IsPasswordExist ()) {
    Status = CheckPassword ();
  } else {
    UnlockPasswordState (NULL, 0);
  }
    gBS->RestoreTPL (TPL_APPLICATION);

    HiiHandle = mSecureBootPrivate.HiiHandle;
    ActionRequest = EFI_BROWSER_ACTION_REQUEST_NONE;

    Status = gBS->LocateProtocol (&gEfiFormBrowser2ProtocolGuid, NULL, (VOID **) &FormBrowser2);
    ASSERT_EFI_ERROR (Status);

    Status = gBS->LocateProtocol (&gEfiFormBrowserExProtocolGuid, NULL, (VOID **) &FormBrowserEx);
    if (!EFI_ERROR (Status)) {
      //
      // First unregister the default hot key F9 and F10.
      //
      HotKey.UnicodeChar = CHAR_NULL;
      HotKey.ScanCode    = SCAN_F9;
      FormBrowserEx->RegisterHotKey (&HotKey, 0, 0, NULL);
      HotKey.ScanCode    = SCAN_F10;
      FormBrowserEx->RegisterHotKey (&HotKey, 0, 0, NULL);

      //
      // Register the default HotKey F9 and F10 again.
      //
      HotKey.ScanCode   = SCAN_F10;
      NewString         = HiiGetString (HiiHandle, STRING_TOKEN (STR_SAVE_CHANGE_STRING), NULL);
      ASSERT (NewString != NULL);
      FormBrowserEx->RegisterHotKey (&HotKey, BROWSER_ACTION_SUBMIT | BROWSER_ACTION_EXIT, 0, NewString);
    }

    Status = FormBrowser2->SendForm (
                             FormBrowser2,
                             &HiiHandle,
                             1,
                             &mSecureBootGuid,
                             0,
                             NULL,
                             &ActionRequest
                             );
    LockPasswordState ();
    if (ActionRequest == EFI_BROWSER_ACTION_REQUEST_RESET) {
      EnableResetRequired ();
    }
    StringPtr = HiiGetString (
                  mSecureBootPrivate.HiiHandle,
                  STRING_TOKEN (STR_OPERATION_FINISH_MESSAGE),
                  NULL
                  );
    mSecureBootPrivate.H2ODialog->ConfirmDialog (
                                         DlgOk,
                                         FALSE,
                                         0,
                                         NULL,
                                         &Key,
                                         StringPtr
                                         );
    gBS->FreePool (StringPtr);
  } else {
    //
    // Set EFI_ADMINISTER_SECURE_BOOT_NAME variable
    //
    SecureBootEnable = TRUE;
    Status = gRT->SetVariable (
                    EFI_ADMINISTER_SECURE_BOOT_NAME,
                    &gEfiGenericVariableGuid,
                    EFI_VARIABLE_NON_VOLATILE | EFI_VARIABLE_BOOTSERVICE_ACCESS | EFI_VARIABLE_RUNTIME_ACCESS,
                    1,
                    &SecureBootEnable
                    );

  }

  //
  // For security reqirement, reset system before exiting secure boot manager
  //
  gRT->ResetSystem (EfiResetCold, EFI_SUCCESS, 0, NULL);

  return;
}
