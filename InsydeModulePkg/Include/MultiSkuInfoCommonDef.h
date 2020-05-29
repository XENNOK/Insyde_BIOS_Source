/** @file
  Provide Multi-Sku common defination.

;******************************************************************************
;* Copyright (c) 2012, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef _MULTI_SKU_INFO_COMMON_DEF_H_
#define _MULTI_SKU_INFO_COMMON_DEF_H_

//
// Common Include File Definition
//
#include <Uefi.h>
//
// Marco Definition for Verb Table
//

#define _DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_4(a) \
COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  mVerbTable##a##Header1 = {              \
  OEM_VERB_TABLE_##a##_HEADER1                                                   \
};                                                                               \
UINT32                                   mVerbTable##a##Data1[] = {              \
  OEM_VERB_TABLE_##a##_DATA1                                                     \
};                                                                               \
COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  mVerbTable##a##Header2 = {              \
  OEM_VERB_TABLE_##a##_HEADER2                                                   \
};                                                                               \
UINT32                                   mVerbTable##a##Data2[] = {              \
  OEM_VERB_TABLE_##a##_DATA2                                                     \
};                                                                               \
COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  mVerbTable##a##Header3 = {              \
  OEM_VERB_TABLE_##a##_HEADER3                                                   \
};                                                                               \
UINT32                                   mVerbTable##a##Data3[] = {              \
  OEM_VERB_TABLE_##a##_DATA3                                                     \
};                                                                               \
COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  mVerbTable##a##Header4 = {              \
  OEM_VERB_TABLE_##a##_HEADER4                                                   \
};                                                                               \
UINT32                                   mVerbTable##a##Data4[] = {              \
  OEM_VERB_TABLE_##a##_DATA4                                                     \
};                                                                               \

#define DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_4(a) _DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_4(a)

#define _DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_3(a) \
COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  mVerbTable##a##Header1 = {              \
  OEM_VERB_TABLE_##a##_HEADER1                                                   \
};                                                                               \
UINT32                                   mVerbTable##a##Data1[] = {              \
  OEM_VERB_TABLE_##a##_DATA1                                                     \
};                                                                               \
COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  mVerbTable##a##Header2 = {              \
  OEM_VERB_TABLE_##a##_HEADER2                                                   \
};                                                                               \
UINT32                                   mVerbTable##a##Data2[] = {              \
  OEM_VERB_TABLE_##a##_DATA2                                                     \
};                                                                               \
COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  mVerbTable##a##Header3 = {              \
  OEM_VERB_TABLE_##a##_HEADER3                                                   \
};                                                                               \
UINT32                                   mVerbTable##a##Data3[] = {              \
  OEM_VERB_TABLE_##a##_DATA3                                                     \
};                                                                               \

#define DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_3(a) _DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_3(a)

#define _DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_2(a) \
COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  mVerbTable##a##Header1 = {              \
  OEM_VERB_TABLE_##a##_HEADER1                                                   \
};                                                                               \
UINT32                                   mVerbTable##a##Data1[] = {              \
  OEM_VERB_TABLE_##a##_DATA1                                                     \
};                                                                               \
COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  mVerbTable##a##Header2 = {              \
  OEM_VERB_TABLE_##a##_HEADER2                                                   \
};                                                                               \
UINT32                                   mVerbTable##a##Data2[] = {              \
  OEM_VERB_TABLE_##a##_DATA2                                                     \
};                                                                               \

#define DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_2(a) _DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_2(a)

#define _DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_1(a) \
COMMON_CHIPSET_AZALIA_VERB_TABLE_HEADER  mVerbTable##a##Header1 = {              \
  OEM_VERB_TABLE_##a##_HEADER1                                                   \
};                                                                               \
UINT32                                   mVerbTable##a##Data1[] = {              \
  OEM_VERB_TABLE_##a##_DATA1                                                     \
};                                                                               \

#define DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_1(a) _DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_1(a)

#define _COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_4(a) \
COMMON_CHIPSET_AZALIA_VERB_TABLE mAzaliaVerbTable##a[] = {  \
  { &( mVerbTable##a##Header1 ), mVerbTable##a##Data1 },    \
  { &( mVerbTable##a##Header2 ), mVerbTable##a##Data2 },    \
  { &( mVerbTable##a##Header3 ), mVerbTable##a##Data3 },    \
  { &( mVerbTable##a##Header4 ), mVerbTable##a##Data4 },    \
  { NULL, NULL }                                            \
};                                                          \

#define COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_4(a) _COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_4(a)

#define _COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_3(a) \
COMMON_CHIPSET_AZALIA_VERB_TABLE mAzaliaVerbTable##a[] = {  \
  { &( mVerbTable##a##Header1 ), mVerbTable##a##Data1 },    \
  { &( mVerbTable##a##Header2 ), mVerbTable##a##Data2 },    \
  { &( mVerbTable##a##Header3 ), mVerbTable##a##Data3 },    \
  { NULL, NULL }                                            \
};                                                          \

#define COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_3(a) _COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_3(a)

#define _COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_2(a) \
COMMON_CHIPSET_AZALIA_VERB_TABLE mAzaliaVerbTable##a[] = {  \
  { &( mVerbTable##a##Header1 ), mVerbTable##a##Data1 },    \
  { &( mVerbTable##a##Header2 ), mVerbTable##a##Data2 },    \
  { NULL, NULL }                                            \
};                                                          \

#define COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_2(a) _COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_2(a)

#define _COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_1(a) \
COMMON_CHIPSET_AZALIA_VERB_TABLE mAzaliaVerbTable##a[] = {  \
  { &( mVerbTable##a##Header1 ), mVerbTable##a##Data1 },    \
  { NULL, NULL }                                            \
};                                                          \

#define COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_1(a) _COLLECT_DEFINE_VERB_TABLE_LOCAL_HEADER_DATA_1(a)

#define _VERB_TABLE_HEADER_DATA_BUFFER_ADDRESS(a) mAzaliaVerbTable##a;

#define VERB_TABLE_HEADER_DATA_BUFFER_ADDRESS(a) _VERB_TABLE_HEADER_DATA_BUFFER_ADDRESS(a)

#endif
