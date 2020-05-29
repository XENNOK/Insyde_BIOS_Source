/** @file

  Implements a large portion of the Win32 API as a protocol.

;******************************************************************************
;* Copyright (c) 2013 - 2014, Insyde Software Corp. All Rights Reserved.
;*
;* You may not reproduce, distribute, publish, display, perform, modify, adapt,
;* transmit, broadcast, present, recite, release, license or otherwise exploit
;* any part of this publication in any form, by any means, without the prior
;* written permission of Insyde Software Corporation.
;*
;******************************************************************************
*/

#ifndef __H2O_WINDOW_PROTOCOL_H__
#define __H2O_WINDOW_PROTOCOL_H__

#include "H2OWindow/mwtypes.h"
#include "H2OWindow/windows.h"
#include "H2OWindow/windef.h"
#include "H2OWindow/wingdi.h"
#include "H2OWindow/winuser.h"
#include "H2OWindow/wintern.h"


#define H2O_WINDOW_PROTOCOL_GUID \
  { 0x10e432f7,0x3cbd,0x49bd, {0x83, 0xf6, 0x96, 0x23, 0x10, 0x98, 0x26, 0xa5}}

typedef  struct _H2O_WINDOW_PROTOCOL H2O_WINDOW_PROTOCOL;


typedef
LRESULT
(EFIAPI *H2O_WM_DEF_WINDOW_PROC) (
  HWND hwnd,
  UINT32 msg,
  WPARAM wParam,
  LPARAM lParam
  );

typedef
void
(EFIAPI *H2O_WM_MW_SET_KEYBOARD_TRANSLATOR) (
  LPFN_KEYBTRANSLATE pFn
  );

typedef
HDC
(EFIAPI *H2O_WM_GET_DC_EX) (
  HWND hwnd,
  HRGN hrgnClip,
  UINT32 flags
  );

typedef
HDC
(EFIAPI *H2O_WM_GET_DC) (
  HWND hwnd
  );

typedef
HDC
(EFIAPI *H2O_WM_GET_WINDOW_DC) (
  HWND hwnd
  );

typedef
INT32
(EFIAPI *H2O_WM_RELEASE_DC) (
  HWND hwnd,
  HDC hdc
  );

typedef
BOOL
(EFIAPI *H2O_WM_DELETE_DC) (
  HDC hdc
  );

typedef
HDC
(EFIAPI *H2O_WM_BEGIN_PAINT) (
  HWND hwnd,
  LPPAINTSTRUCT lpPaint
  );

typedef
BOOL
(EFIAPI *H2O_WM_END_PAINT) (
  HWND hwnd,
  CONST PAINTSTRUCT *lpPaint
  );

typedef
COLORREF
(EFIAPI *H2O_WM_SET_TEXT_COLOR) (
  HDC hdc,
  COLORREF crColor
  );

typedef
COLORREF
(EFIAPI *H2O_WM_GET_TEXT_COLOR) (
  HDC hdc
  );

typedef
COLORREF
(EFIAPI *H2O_WM_SET_BK_COLOR) (
  HDC hdc,
  COLORREF crColor
  );

typedef
INT32
(EFIAPI *H2O_WM_SET_BK_MODE) (
  HDC hdc,
  INT32 iBkMode
  );

typedef
UINT32
(EFIAPI *H2O_WM_SET_TEXT_ALIGN) (
  HDC hdc,
  UINT32 fMode
  );

typedef
UINT32
(EFIAPI *H2O_WM_GET_TEXT_ALIGN) (
  HDC hdc
  );

typedef
INT32
(EFIAPI *H2O_WM_SET_ROP2) (
  HDC hdc,
  INT32 fnDrawMode
  );

typedef
COLORREF
(EFIAPI *H2O_WM_GET_PIXEL) (
  HDC hdc,
  INT32 x,
  INT32 y
  );

typedef
COLORREF
(EFIAPI *H2O_WM_SET_PIXEL) (
  HDC hdc,
  INT32 x,
  INT32 y,
  COLORREF crColor
  );

typedef
BOOL
(EFIAPI *H2O_WM_MOVE_TO_EX) (
  HDC hdc,
  INT32 x,
  INT32 y,
  LPPOINT lpPoint
  );

typedef
BOOL
(EFIAPI *H2O_WM_LINE_TO) (
  HDC hdc,
  INT32 x,
  INT32 y
  );

typedef
BOOL
(EFIAPI *H2O_WM_POLYLINE) (
  HDC hdc,
  CONST POINT *lppt,
  INT32 cPoints
  );

typedef
BOOL
(EFIAPI *H2O_WM_RECTANGLE) (
  HDC hdc,
  INT32 nLeft,
  INT32 nTop,
  INT32 nRight,
  INT32 nBottom
  );

typedef
BOOL
(EFIAPI *H2O_WM_ELLIPSE) (
  HDC hdc,
  INT32 nLeftRect,
  INT32 nTopRect,
  INT32 nRightRect,
  INT32 nBottomRect
  );

typedef
BOOL
(EFIAPI *H2O_WM_ARC) (
  HDC hdc,
  INT32 nLeftRect,
  INT32 nTopRect,
  INT32 nRightRect,
  INT32 nBottomRect
  );

typedef
BOOL
(EFIAPI *H2O_WM_PIE) (
  HDC hdc,
  INT32 nLeftRect,
  INT32 nTopRect,
  INT32 nRightRect,
  INT32 nBottomRect,
  INT32 nXRadial1,
  INT32 nYRadial1,
  INT32 nXRadial2,
  INT32 nYRadial2
  );

typedef
BOOL
(EFIAPI *H2O_WM_POLYGON) (
  HDC hdc,
  CONST POINT *lpPoints,
  INT32 nCount
  );

typedef
BOOL
(EFIAPI *H2O_WM_POLY_POLYGON) (
  HDC hdc,
  CONST POINT *lpPoints,
  LPINT lpPolyCounts,
  INT32 nCount
  );

typedef
INT32
(EFIAPI *H2O_WM_FILL_RECT) (
  HDC hdc,
  CONST RECT *lprc,
  HBRUSH hbr
  );

typedef
void
(EFIAPI *H2O_WM_MW_SET_TEXT_CODING) (
  long mode
  );

typedef
BOOL
(EFIAPI *H2O_WM_TEXT_OUT ) (
  HDC Hdc,
  INT32 x,
  INT32 y,
  LPCWSTR String,
  INT32 cbString
  );

typedef
BOOL
(EFIAPI *H2O_WM_TEXT_OUT_W) (
  HDC hdc,
  INT32 x,
  INT32 y,
  LPCWSTR lpszString,
  INT32 cbString
  );

typedef
BOOL
(EFIAPI *H2O_WM_EXT_TEXT_OUT) (
  HDC hdc,
  INT32 x,
  INT32 y,
  UINT32 fuOptions,
  CONST RECT *lprc,
  LPCSTR lpszString,
  UINT32 cbCount,
  CONST INT32 *lpDx
  );

typedef
BOOL
(EFIAPI *H2O_WM_EXT_TEXT_OUT_W) (
  HDC hdc,
  INT32 x,
  INT32 y,
  UINT32 fuOptions,
  CONST RECT *lprc,
  LPCWSTR lpszString,
  UINT cbCount,
  CONST INT *lpDx
  );

typedef
INT32
(EFIAPI *H2O_WM_DRAW_TEXT_A) (
  HDC hdc,
  LPCSTR lpString,
  INT32 nCount,
  LPRECT lpRect,
  UINT32 uFormat
  );

typedef
INT32
(EFIAPI *H2O_WM_DRAW_TEXT) (
  HDC hdc,
  LPCWSTR lpString,
  INT32 nCount,
  LPRECT lpRect,
  UINT32 uFormat
  );

typedef
BOOL
(EFIAPI *H2O_WM_DRAW_DIB) (
  HDC hdc,
  INT32 x,
  INT32 y,
  PMWIMAGEHDR pimage
  );

typedef
UINT32
(EFIAPI *H2O_WM_GET_SYS_COLOR) (
  INT32 nIndex
  );

typedef
COLORREF
(EFIAPI *H2O_WM_SET_SYS_COLOR) (
  INT32 nIndex,
  COLORREF crColor	/* Microwindows only*/
  );

typedef
HBRUSH
(EFIAPI *H2O_WM_GET_SYS_COLOR_BRUSH) (
  INT32 nIndex
  );

typedef
HGDIOBJ
(EFIAPI *H2O_WM_GET_STOCK_OBJECT) (
  INT32 nObject
  );

typedef
HGDIOBJ
(EFIAPI *H2O_WM_SELECT_OBJECT) (
  HDC hdc,
  HGDIOBJ hObject
  );

typedef
BOOL
(EFIAPI *H2O_WM_DELETE_OBJECT) (
  HGDIOBJ hObject
  );

typedef
INT32
(EFIAPI *H2O_WM_SELECT_CLIP_RGN) (
  HDC hdc,
  HRGN hrgn
  );

typedef
INT32
(EFIAPI *H2O_WM_EXT_SELECT_CLIP_RGN) (
  HDC hdc,
  HRGN hrgn,
  INT32 fnMode
  );

typedef
INT32
(EFIAPI *H2O_WM_GET_UPDATE_RGN) (
  HWND hwnd,
  HRGN hrgn,
  BOOL bErase
  );

typedef
BOOL
(EFIAPI *H2O_WM_GET_UPDATE_RECT) (
  HWND hwnd,
  LPRECT lpRect,
  BOOL bErase
  );

typedef
HBRUSH
(EFIAPI *H2O_WM_CREATE_SOLID_BRUSH) (
  COLORREF crColor
  );

typedef
HPEN
(EFIAPI *H2O_WM_CREATE_PEN) (
  INT32 nPenStyle,
  INT32 nWidth,
  COLORREF crColor
  );

typedef
HBITMAP
(EFIAPI *H2O_WM_CREATE_COMPATIBLE_BITMAP) (
  HDC hdc,
  INT32 nWidth,
  INT32 nHeight
  );

typedef
HDC
(EFIAPI *H2O_WM_CREATE_COMPATIBLE_DC) (
  HDC hdc
  );

typedef
BOOL
(EFIAPI *H2O_WM_BIT_BLT) (
  HDC hdcDest,
  INT32 nXDest,
  INT32 nYDest,
  INT32 nWidth,
  INT32 nHeight,
  HDC hdcSrc,
  INT32 nXSrc,
  INT32 nYSrc,
  UINT32 dwRop
  );

typedef
BOOL
(EFIAPI *H2O_WM_STRETCH_BLT) (
  HDC hdcDest,
  INT32 nXOriginDest,
  INT32 nYOriginDest,
  INT32 nWidthDest,
  INT32 nHeightDest,
  HDC   hdcSrc,
  INT32 nXOriginSrc,
  INT32 nYOriginSrc,
  INT32 nWidthSrc,
  INT32 nHeightSrc,
  DWORD dwRop
  );

typedef
UINT32
(EFIAPI *H2O_WM_GET_SYSTEM_PALETTE_ENTRIES) (
  HDC hdc,
  UINT32 iStartIndex,
  UINT32 nEntries,
  LPPALETTEENTRY lppe
  );

typedef
INT32
(EFIAPI *H2O_WM_GET_DEVICE_CAPS) (
  HDC hdc,
  INT32 nIndex
  );

typedef
BOOL
(EFIAPI *H2O_WM_DRAW_FOCUS_RECT) (
  HDC hdc,
  LPRECT prect
  );

typedef
INT32
(EFIAPI *H2O_WM_TABBED_TEXT_OUT) (
  HDC hdc,
  INT32 x,
  INT32 y,
  LPCTSTR lpszString,
  INT32 cbString,
  INT32 ntabs,
  LPINT lpTabStops,
  INT32 nTabOrigin
  );

typedef
UINT32
(EFIAPI *H2O_WM_GET_TABBED_TEXT_EXTENT) (
  HDC hdc,
  INT32 x,
  INT32 y,
  LPCTSTR lpszString,
  INT32 cbString
  );

typedef
INT32
(EFIAPI *H2O_WM_MUL_DIV) (
  INT32 nMultiplicand,
  INT32 nMultiplier,
  INT32 nDivisor
  );

typedef
BOOL
(EFIAPI *H2O_WM_GET_TEXT_EXTENT_POINT32) (
  HDC hdc,    /* handle to DC*/
  LPCWSTR lpszStr,  /* character string*/
  int cchString,    /* number of characters*/
  LPSIZE lpSize    /* string dimensions*/
  );

typedef
UINT64
(EFIAPI *H2O_WM_GET_TICK_COUNT) (
  VOID
  );

typedef
VOID
(EFIAPI *H2O_WM_SLEEP) (
  UINT32 dwMilliseconds
  );

typedef
INT32
(EFIAPI *H2O_WM_OFFSET_RGN) (
  HRGN hrgn,
  INT32 x,
  INT32 y
  );

typedef
INT32
(EFIAPI *H2O_WM_GET_RGN_BOX) (
  HRGN hrgn,
  LPRECT rect
  );

typedef
HRGN
(EFIAPI *H2O_WM_CREATE_RECT_RGN) (
  INT32 left,
  INT32 top,
  INT32 right,
  INT32 bottom
  );

typedef
HRGN
(EFIAPI *H2O_WM_CREATE_RECT_RGN_INDIRECT) (
  const RECT* rect
  );

typedef
VOID
(EFIAPI *H2O_WM_SET_RECT_RGN) (
  HRGN hrgn,
  INT32 left,
  INT32 top,
  INT32 right,
  INT32 bottom
  );

typedef
HRGN
(EFIAPI *H2O_WM_CREATE_ROUND_RECT_RGN) (
  INT32 left,
  INT32 top,
  INT32 right,
  INT32 bottom,
  INT32 ellipse_width,
  INT32 ellipse_height
  );

typedef
HRGN
(EFIAPI *H2O_WM_CREATE_ELLIPTIC_RGN) (
  INT32 left,
  INT32 top,
  INT32 right,
  INT32 bottom
  );

typedef
HRGN
(EFIAPI *H2O_WM_CREATE_ELLIPTIC_RGN_INDIRECT) (
  const RECT *rect
  );

typedef
HRGN
(EFIAPI *H2O_WM_CREATE_POLYGON_RGN) (
  const POINT *points,
  INT32 count,
  INT32 mode
  );

typedef
UINT32
(EFIAPI *H2O_WM_GET_REGION_DATA) (
  HRGN hrgn,
  UINT32 count,
  LPRGNDATA rgndata
  );

typedef
BOOL
(EFIAPI *H2O_WM_PT_IN_REGION) (
  HRGN hrgn,
  INT32 x,
  INT32 y
  );

typedef
BOOL
(EFIAPI *H2O_WM_RECT_IN_REGION) (
  HRGN hrgn,
  const RECT *rect
  );

typedef
BOOL
(EFIAPI *H2O_WM_EQUAL_RGN) (
  HRGN hrgn1,
  HRGN hrgn2
  );

typedef
INT32
(EFIAPI *H2O_WM_COMBINE_RGN) (
  HRGN hDest,
  HRGN hSrc1,
  HRGN hSrc2,
  INT32 mode
  );

typedef
BOOL
(EFIAPI *H2O_WM_INTERSECT_RECT) (
  LPRECT dest,
  const RECT *src1,
  const RECT *src2
  );

typedef
BOOL
(EFIAPI *H2O_WM_UNION_RECT) (
  LPRECT dest,
  const RECT *src1,
  const RECT *src2
  );

typedef
BOOL
(EFIAPI *H2O_WM_EQUAL_RECT) (
  const RECT* rect1,
  const RECT* rect2
  );

typedef
BOOL
(EFIAPI *H2O_WM_SUBTRACT_RECT) (
  LPRECT dest,
  const RECT *src1,
  const RECT *src2
  );

typedef
LRESULT
(EFIAPI *H2O_WM_CALL_WINDOW_PROC) (
  WNDPROC lpPrevWndFunc,
  HWND hwnd,
  UINT32 Msg,
  WPARAM wParam,
  LPARAM lParam
  );

typedef
LRESULT
(EFIAPI *H2O_WM_SEND_MESSAGE) (
  HWND hwnd,
  UINT32 Msg,
  WPARAM wParam,
  LPARAM lParam
  );

typedef
BOOL
(EFIAPI *H2O_WM_POST_MESSAGE) (
  HWND hwnd,
  UINT32 Msg,
  WPARAM wParam,
  LPARAM lParam
  );

typedef
BOOL
(EFIAPI *H2O_WM_POST_THREAD_MESSAGE) (
  UINT32 dwThreadId,
  UINT32 Msg,
  WPARAM wParam,
  LPARAM lParam
  );

typedef
VOID
(EFIAPI *H2O_WM_POST_QUIT_MESSAGE) (
  INT32 nExitCode
  );

typedef
BOOL
(EFIAPI *H2O_WM_PEEK_MESSAGE) (
  LPMSG lpMsg,
  HWND hwnd,
  UINT32 uMsgFilterMin,
  UINT32 uMsgFilterMax,
  UINT32 wRemoveMsg
  );

typedef
BOOL
(EFIAPI *H2O_WM_GET_MESSAGE) (
  LPMSG lpMsg,
  HWND hwnd,
  UINT32 wMsgFilterMin,
  UINT32 wMsgFilterMax
  );

typedef
BOOL
(EFIAPI *H2O_WM_TRANSLATE_MESSAGE) (
  CONST MSG *lpMsg
  );

typedef
LRESULT
(EFIAPI *H2O_WM_DISPATCH_MESSAGE) (
  CONST MSG *lpMsg
  );

typedef
ATOM
(EFIAPI *H2O_WM_REGISTER_CLASS) (
  CONST WNDCLASS *lpWndClass
  );

typedef
BOOL
(EFIAPI *H2O_WM_UNREGISTER_CLASS) (
  LPCWSTR lpClassName,
  HINSTANCE hInstance
  );

typedef
HWND
(EFIAPI *H2O_WM_CREATE_WINDOW_EX) (
  UINT32 dwExStyle,
  LPCWSTR lpClassName,
  LPCWSTR lpWindowName,
  UINT32 dwStyle,
  INT32 x,
  INT32 y,
  INT32 nWidth,
  INT32 nHeight,
  HWND hwndParent,
  HMENU hMenu,
  HINSTANCE hInstance,
  LPVOID lpParam
  );

typedef
BOOL
(EFIAPI *H2O_WM_DESTROY_WINDOW) (
  HWND hwnd
  );

typedef
BOOL
(EFIAPI *H2O_WM_IS_WINDOW) (
  HWND hwnd
  );

typedef
BOOL
(EFIAPI *H2O_WM_SHOW_WINDOW) (
  HWND hwnd,
  INT32 nCmdShow
  );

typedef
BOOL
(EFIAPI *H2O_WM_INVALIDATE_RECT) (
  HWND hwnd,
  CONST RECT *lpRect,
  BOOL bErase
  );

typedef
BOOL
(EFIAPI *H2O_WM_INVALIDATE_RGN) (
  HWND hwnd,
  HRGN hrgn,
  BOOL bErase
  );

typedef
BOOL
(EFIAPI *H2O_WM_VALIDATE_RECT) (
  HWND hwnd,
  CONST RECT *lprc
  );

typedef
BOOL
(EFIAPI *H2O_WM_VALIDATE_RGN) (
  HWND hwnd,
  HRGN hrgn
  );

typedef
BOOL
(EFIAPI *H2O_WM_UPDATE_WINDOW) (
  HWND hwnd
  );

typedef
HWND
(EFIAPI *H2O_WM_GET_FOCUS) (
  VOID
  );

typedef
HWND
(EFIAPI *H2O_WM_SET_FOCUS) (
  HWND hwnd
  );

typedef
BOOL
(EFIAPI *H2O_WM_SET_FOREGROUND_WINDOW) (
  HWND hwnd
  );

typedef
HWND
(EFIAPI *H2O_WM_SET_ACTIVE_WINDOW) (
  HWND hwnd
  );

typedef
HWND
(EFIAPI *H2O_WM_GET_ACTIVE_WINDOW) (
  VOID
  );

typedef
BOOL
(EFIAPI *H2O_WM_BRING_WINDOW_TO_TOP) (
  HWND hwnd
  );

typedef
HWND
(EFIAPI *H2O_WM_GET_DESKTOP_WINDOW) (
  VOID
  );

typedef
HWND
(EFIAPI *H2O_WM_GET_PARENT) (
  HWND hwnd
  );

typedef
BOOL
(EFIAPI *H2O_WM_ENABLE_WINDOW) (
  HWND hwnd,
  BOOL bEnable
  );

typedef
BOOL
(EFIAPI *H2O_WM_ADJUST_WINDOW_RECT_EX) (
  LPRECT lpRect,
  UINT32 dwStyle,
  BOOL bMenu,
  UINT32 dwExStyle
  );

typedef
BOOL
(EFIAPI *H2O_WM_GET_CLIENT_RECT) (
  HWND hwnd,
  LPRECT lpRect
  );

typedef
BOOL
(EFIAPI *H2O_WM_GET_WINDOW_RECT) (
  HWND hwnd,
  LPRECT lpRect
  );

typedef
BOOL
(EFIAPI *H2O_WM_CLIENT_TO_SCREEN) (
  HWND hwnd,
  LPPOINT lpPoint
  );

typedef
BOOL
(EFIAPI *H2O_WM_SCREEN_TO_CLIENT) (
  HWND hwnd,
  LPPOINT lpPoint
  );

typedef
INT32
(EFIAPI *H2O_WM_MAP_WINDOW_POINTS) (
  HWND hwndFrom,
  HWND hwndTo,
  LPPOINT lpPoints,
  UINT32 cPoints
  );

typedef
BOOL
(EFIAPI *H2O_WM_SET_RECT) (
  LPRECT lprc,
  INT32 xLeft,
  INT32 yTop,
  INT32 xRight,
  INT32 yBottom
  );

typedef
BOOL
(EFIAPI *H2O_WM_SET_RECT_EMPTY) (
  LPRECT lprc
  );

typedef
BOOL
(EFIAPI *H2O_WM_COPY_RECT) (
  LPRECT lprcDst,
  CONST RECT *lprcSrc
  );

typedef
BOOL
(EFIAPI *H2O_WM_IS_RECT_EMPTY) (
  CONST RECT *lprc
  );

typedef
BOOL
(EFIAPI *H2O_WM_INFLATE_RECT) (
  LPRECT lprc,
  INT32 dx,
  INT32 dy
  );

typedef
BOOL
(EFIAPI *H2O_WM_OFFSET_RECT) (
  LPRECT lprc,
  INT32 dx,
  INT32 dy
  );

typedef
BOOL
(EFIAPI *H2O_WM_PT_IN_RECT) (
  CONST RECT *lprc,
  POINT pt
  );

typedef
INT32
(EFIAPI *H2O_WM_GET_WINDOW_LONG) (
  HWND hwnd,
  INT32 nIndex
  );

typedef
INTN
(EFIAPI *H2O_WM_GET_WINDOW_LONG_PTR ) (
  HWND hwnd,
  INT32 nIndex
  );

typedef
INT32
(EFIAPI *H2O_WM_SET_WINDOW_LONG) (
  HWND hwnd,
  INT32 nIndex,
  INT32 lNewLong
  );

typedef
INTN
(EFIAPI *H2O_WM_SET_WINDOW_LONG_PTR ) (
  HWND hwnd,
  INT32 nIndex,
  INTN lNewLong
  );

typedef
UINT16
(EFIAPI *H2O_WM_GET_WINDOW_WORD) (
  HWND hwnd,
  INT32 nIndex
  );

typedef
UINT16
(EFIAPI *H2O_WM_SET_WINDOW_WORD) (
  HWND hwnd,
  INT32 nIndex,
  UINT16 wNewWord
  );

typedef
ATOM
(EFIAPI *H2O_WM_GLOBAL_FIND_ATOM) (
  LPCWSTR lpString
  );

typedef
ATOM
(EFIAPI *H2O_WM_GLOBAL_ADD_ATOM) (
  LPCWSTR lpString
  );

typedef
BOOL
(EFIAPI *H2O_WM_SET_PROP) (
  HWND hWnd,
  LPCWSTR lpString,
  HANDLE hData
  );

typedef
HANDLE
(EFIAPI *H2O_WM_GET_PROP) (
  HWND hWnd,
  LPCWSTR lpString
  );

typedef
HANDLE
(EFIAPI *H2O_WM_REMOVE_PROP) (
  HWND hWnd,
  LPCWSTR lpString
  );

typedef
UINT32
(EFIAPI *H2O_WM_GET_CLASS_LONG) (
  HWND hwnd,
  INT32 nIndex
  );

typedef
UINTN
(EFIAPI *H2O_WM_GET_CLASS_LONG_PTR) (
  HWND hwnd,
  INT32 nIndex
  );

typedef
INT32
(EFIAPI *H2O_WM_GET_WINDOW_TEXT_LENGTH) (
  HWND hwnd
  );

typedef
INT32
(EFIAPI *H2O_WM_GET_WINDOW_TEXT) (
  HWND hwnd,
  LPWSTR lpString,
  INT32 nMaxCount
  );

typedef
BOOL
(EFIAPI *H2O_WM_SET_WINDOW_TEXT) (
  HWND hwnd,
  LPCWSTR lpString
  );

typedef
BOOL
(EFIAPI *H2O_WM_MOVE_WINDOW) (
  HWND hwnd,
  INT32 x,
  INT32 y,
  INT32 nWidth,
  INT32 nHeight,
  BOOL bRepaint
  );

typedef
BOOL
(EFIAPI *H2O_WM_GET_CURSOR_POS) (
  LPPOINT lpPoint
  );

typedef
HWND
(EFIAPI *H2O_WM_GET_CAPTURE) (
  VOID
  );

typedef
HWND
(EFIAPI *H2O_WM_SET_CAPTURE) (
  HWND hwnd
  );

typedef
BOOL
(EFIAPI *H2O_WM_RELEASE_CAPTURE) (
  VOID
  );

typedef
UINT32
(EFIAPI *H2O_WM_SET_TIMER) (
  HWND hwnd,
  UINT32 idTimer,
  UINT32 uTimeout,
  TIMERPROC lpTimerFunc
  );

typedef
BOOL
(EFIAPI *H2O_WM_KILL_TIMER) (
  HWND hwnd,
  UINT32 idTimer
  );

typedef
INT32
(EFIAPI *H2O_WM_GET_SYSTEM_METRICS) (
  INT32 nIndex
  );

typedef
BOOL
(EFIAPI *H2O_WM_SYSTEM_PARAMETERS_INFO ) (
  UINT32 uiAction,
  UINT32 uiParam,
  PVOID pvParam,
  UINT32 fWinIni
  );

typedef
HWND
(EFIAPI *H2O_WM_GET_DLG_ITEM) (
  HWND hDlg,
  INT32 nIDDlgItem
  );

typedef
BOOL
(EFIAPI *H2O_WM_ENUM_CHILD_WINDOWS) (
  HWND hWndParent,
  WNDENUMPROC lpEnumFunc,
  LPARAM lParam
  );

typedef
BOOL
(EFIAPI *H2O_WM_SET_WINDOW_POS) (
  HWND hwnd,
  HWND hwndInsertAfter,
  INT32 x,
  INT32 y,
  INT32 cx,
  INT32 cy,
  UINT32 fuFlags
  );

typedef
INT32
(EFIAPI *H2O_WM_GET_CLIP_BOX) (
  HDC   Hdc,
  RECT  *Rc
  );

typedef
COLORREF
(EFIAPI *H2O_WM_SET_DC_PEN_COLOR) (
  HDC Hdc,
  COLORREF Color
  );

typedef
COLORREF
(EFIAPI *H2O_WM_SET_DC_BRUSH_COLOR) (
  HDC Hdc,
  COLORREF Color
  );

typedef
INT32
(EFIAPI *H2O_WM_MW_OPEN) (
  VOID
  );

typedef
VOID
(EFIAPI *H2O_WM_GD_CLOSE_MOUSE) (
  VOID
  );

typedef
INT32
(EFIAPI *H2O_WM_GD_OPEN_MOUSE) (
  VOID
  );

typedef
int
(EFIAPI *H2O_WM_MW_FAST_FILL_RECT) (
  IN HDC        hDC,
  IN CONST RECT *lprc,
  IN COLORREF   crColor
  );

typedef
VOID
(EFIAPI *H2O_WM_GD_DRAW_IMAGE_PART_TO_FIT) (
  PSD psd,
  MWCOORD x,
  MWCOORD y,
  MWCOORD width,
  MWCOORD height,
  MWCOORD sx,
  MWCOORD sy,
  MWCOORD swidth,
  MWCOORD sheight,
  PSD pmd
  );

typedef
PSD
(EFIAPI *H2O_WM_GD_LOAD_IMAGE_FROM_BUFFER) (
  IN  EFI_IMAGE_INPUT      *Image
  );

typedef
PSD
(EFIAPI *H2O_WM_GD_LOAD_IMAGE_FROM_BUFFER2) (
  IN VOID                       *Buffer,
  IN INT32                      Size,
  IN INT32                      Flags
  );

typedef
EFI_STATUS
(EFIAPI *H2O_WM_GD_ADD_GOP_DEVICE) (
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop
  );

typedef
EFI_STATUS
(EFIAPI *H2O_WM_GD_ADD_EFI_KEY_DATA) (
  EFI_KEY_DATA  *KeyData
  );

typedef
EFI_STATUS
(EFIAPI *H2O_WM_GD_ADD_POINTER_DATA) (
  EFI_SIMPLE_POINTER_STATE      *SimplePointerState    OPTIONAL,
  EFI_ABSOLUTE_POINTER_STATE    *AbsolutePointerState  OPTIONAL
  );


typedef
BOOL
(EFIAPI *H2O_WM_REGISTER_HOT_KEY) (
  HWND   hWnd,
  INT32  id,
  UINT32 fsModifiers,
  UINT32 vk
  );

typedef
BOOL
(EFIAPI *H2O_WM_UNREGISTER_HOT_KEY)(
  HWND   hWnd,
  INT32  id
  );

struct _H2O_WINDOW_PROTOCOL {
  UINT32 Size;

  H2O_WM_SEND_MESSAGE                 SendMessage;
  H2O_WM_POST_MESSAGE                 PostMessage;
  H2O_WM_POST_QUIT_MESSAGE            PostQuitMessage;
  H2O_WM_GET_MESSAGE                  GetMessage;
  H2O_WM_PEEK_MESSAGE                 PeekMessage;
  H2O_WM_TRANSLATE_MESSAGE            TranslateMessage;
  H2O_WM_DISPATCH_MESSAGE             DispatchMessage;

  H2O_WM_REGISTER_CLASS               RegisterClass;
//H2O_WM_UNREGISTER_CLASS             UnRegisterClass;
  H2O_WM_CREATE_WINDOW_EX             CreateWindowEx;
  H2O_WM_DESTROY_WINDOW               DestroyWindow;
//H2O_WM_GET_WINDOW_LONG              GetWindowLong;
//H2O_WM_SET_WINDOW_LONG              SetWindowLong;
  H2O_WM_GET_WINDOW_WORD              GetWindowWord;
  H2O_WM_SET_WINDOW_WORD              SetWindowWord;
//H2O_WM_GET_CLASS_LONG               GetClassLong;
  H2O_WM_GET_WINDOW_TEXT              GetWindowText;
  H2O_WM_SET_WINDOW_TEXT              SetWindowText;

  H2O_WM_SET_TEXT_COLOR               SetTextColor;
  H2O_WM_SET_BK_COLOR                 SetBkColor;
  H2O_WM_GET_SYS_COLOR                GetSysColor;
  H2O_WM_SET_SYS_COLOR                SetSysColor;
  H2O_WM_SET_BK_MODE                  SetBkMode;
  H2O_WM_SET_ROP2                     SetROP2;
  H2O_WM_SET_PIXEL                    SetPixel;
  H2O_WM_MOVE_TO_EX                   MoveToEx;
  H2O_WM_LINE_TO                      LineTo;
  H2O_WM_RECTANGLE                    Rectangle;
  H2O_WM_FILL_RECT                    FillRect;
  H2O_WM_TEXT_OUT                     TextOut;
//H2O_WM_EXT_TEXT_OUT                 ExtTextOut;
  H2O_WM_DRAW_TEXT                    DrawText;
//H2O_WM_DRAW_DIB                     DrawDIB;
  H2O_WM_SELECT_OBJECT                SelectObject;
  H2O_WM_GET_STOCK_OBJECT             GetStockObject;
  H2O_WM_CREATE_PEN                   CreatePen;
  H2O_WM_CREATE_SOLID_BRUSH           CreateSolidBrush;
  H2O_WM_CREATE_COMPATIBLE_BITMAP     CreateCompatibleBitmap;
  H2O_WM_DELETE_OBJECT                DeleteObject;
  H2O_WM_CREATE_COMPATIBLE_DC         CreateCompatibleDC;
  H2O_WM_DELETE_DC                    DeleteDC;
  H2O_WM_BIT_BLT                      BitBlt;
//H2O_WM_GET_SYSTEM_PALETTE_ENTRIES   GetSystemPaletteEntries;

//H2O_WM_WND_SET_DESKTOP_WALLPAPER    WndSetSetDesktopWallpaper;
//H2O_WM_WND_SET_CURSOR               WndSetCursor;
//H2O_WM_WND_RAISE_WINDOW             WndRaiseWindow;
//H2O_WM_WND_LOWER_WINDOW             WndLowerWindow;
//H2O_WM_WND_GET_TOP_WINDOW           WndGetTopWindow;

  H2O_WM_GET_TICK_COUNT               GetTickCount;
//H2O_WM_SLEEP                        Sleep;
  H2O_WM_SET_TIMER                    SetTimer;
  H2O_WM_KILL_TIMER                   KillTimer;
  H2O_WM_GET_CURSOR_POS               GetCursorPos;

//  H2O_WM_SET_RECT                     SetRect;
//  H2O_WM_SET_RECT_EMPTY               SetRectEmpty;
//  H2O_WM_COPY_RECT                    CopyRect;
//  H2O_WM_IS_RECT_EMPTY                IsRectEmpty;
//  H2O_WM_INFLATE_RECT                 InflateRect;
//  H2O_WM_OFFSET_RECT                  OffsetRect;
//  H2O_WM_PT_IN_RECT                   PtInRect;
//  H2O_WM_INTERSECT_RECT               IntersectRect;
//  H2O_WM_UNION_RECT                   UnionRect;
//  H2O_WM_SUBTRACT_RECT                SubtractRect;
//  H2O_WM_EQUAL_RECT                   EqualRect;

  H2O_WM_CREATE_RECT_RGN              CreateRectRgn;
  H2O_WM_CREATE_RECT_RGN_INDIRECT     CreateRectRgnIndirect;
  H2O_WM_SET_RECT_RGN                 SetRectRgn;
  H2O_WM_CREATE_ROUND_RECT_RGN        CreateRoundRectRgn;
  H2O_WM_CREATE_ELLIPTIC_RGN          CreateEllipticRgn;
  H2O_WM_CREATE_ELLIPTIC_RGN_INDIRECT CreateEllipticRgnIndirect;
  H2O_WM_OFFSET_RGN                   OffsetRgn;
  H2O_WM_GET_RGN_BOX                  GetRgnBox;
//H2O_WM_GET_RGN_DATA                 GetRgnData;
//H2O_WM_PT_IN_RGN                    PtInRgn;
//H2O_WM_RECT_IN_RGN                  RectInRgn;
  H2O_WM_EQUAL_RGN                    EqualRgn;
  H2O_WM_COMBINE_RGN                  CombineRgn;

//H2O_WM_SELECT_CLIP_REGION           SelectClipRegion;
//H2O_WM_EXT_SELECT_CLIP_REGION       ExtSelectClipRegion;

  H2O_WM_DEF_WINDOW_PROC              DefWindowProc;
  H2O_WM_CALL_WINDOW_PROC             CallWindowProc;

  //
  // addin
  //
  H2O_WM_MW_SET_KEYBOARD_TRANSLATOR   MwSetKeyboardTranslator;
  H2O_WM_GET_DC_EX                    GetDCEx;
  H2O_WM_GET_DC                       GetDC;
  H2O_WM_GET_WINDOW_DC                GetWindowDC;
  H2O_WM_RELEASE_DC                   ReleaseDC;
  H2O_WM_BEGIN_PAINT                  BeginPaint;
  H2O_WM_END_PAINT                    EndPaint;
  H2O_WM_GET_TEXT_COLOR               GetTextColor;
  H2O_WM_SET_TEXT_ALIGN               SetTextAlign;
  H2O_WM_GET_TEXT_ALIGN               GetTextAlign;
  H2O_WM_GET_PIXEL                    GetPixel;
  H2O_WM_POLYLINE                     Polyline;
//H2O_WM_ELLIPSE                      Ellipse;
//H2O_WM_PIE                          Pie;
  H2O_WM_POLYGON                      Polygon;
  H2O_WM_POLY_POLYGON                 PolyPolygon;
  H2O_WM_MW_SET_TEXT_CODING           MwSetTextCoding;
//H2O_WM_TEXT_OUT_W                   TextOutW;
//H2O_WM_EXT_TEXT_OUT_W               ExtTextOutW;
//H2O_WM_DRAW_TEXT_A                  DrawTextA;
  H2O_WM_GET_SYS_COLOR_BRUSH          GetSysColorBrush;
  H2O_WM_SELECT_CLIP_RGN              SelectClipRgn;
  H2O_WM_EXT_SELECT_CLIP_RGN          ExtSelectClipRgn;
  H2O_WM_GET_UPDATE_RGN               GetUpdateRgn;
  H2O_WM_GET_UPDATE_RECT              GetUpdateRect;
  H2O_WM_STRETCH_BLT                  StretchBlt;
  H2O_WM_GET_DEVICE_CAPS              GetDeviceCaps;
//H2O_WM_DRAW_FOCUS_RECT              DrawFocusRect;
//H2O_WM_TABBED_TEXT_OUT              TabbedTextOut;
//H2O_WM_GET_TABBED_TEXT_EXTENT       GetTabbedTextExtent;
//H2O_WM_MUL_DIV                      MulDiv;
  H2O_WM_GET_TEXT_EXTENT_POINT32      GetTextExtentPoint32;
//H2O_WM_SLEEP                        Sleep;
  H2O_WM_CREATE_POLYGON_RGN           CreatePolygonRgn;
  H2O_WM_GET_REGION_DATA              GetRegionData;
  H2O_WM_PT_IN_REGION                 PtInRegion;
  H2O_WM_RECT_IN_REGION               RectInRegion;
  H2O_WM_POST_THREAD_MESSAGE          PostThreadMessage;
  H2O_WM_UNREGISTER_CLASS             UnregisterClass;
  H2O_WM_IS_WINDOW                    IsWindow;
  H2O_WM_SHOW_WINDOW                  ShowWindow;
  H2O_WM_INVALIDATE_RECT              InvalidateRect;
  H2O_WM_INVALIDATE_RGN               InvalidateRgn;
  H2O_WM_VALIDATE_RECT                ValidateRect;
  H2O_WM_VALIDATE_RGN                 ValidateRgn;
  H2O_WM_UPDATE_WINDOW                UpdateWindow;
  H2O_WM_GET_FOCUS                    GetFocus;
  H2O_WM_SET_FOCUS                    SetFocus;
  H2O_WM_SET_FOREGROUND_WINDOW        SetForegroundWindow;
  H2O_WM_SET_ACTIVE_WINDOW            SetActiveWindow;
  H2O_WM_GET_ACTIVE_WINDOW            GetActiveWindow;
  H2O_WM_BRING_WINDOW_TO_TOP          BringWindowToTop;
  H2O_WM_GET_DESKTOP_WINDOW           GetDesktopWindow;
  H2O_WM_GET_PARENT                   GetParent;
  H2O_WM_ENABLE_WINDOW                EnableWindow;
  H2O_WM_ADJUST_WINDOW_RECT_EX        AdjustWindowRectEx;
  H2O_WM_GET_CLIENT_RECT              GetClientRect;
  H2O_WM_GET_WINDOW_RECT              GetWindowRect;
  H2O_WM_CLIENT_TO_SCREEN             ClientToScreen;
  H2O_WM_SCREEN_TO_CLIENT             ScreenToClient;
  H2O_WM_MAP_WINDOW_POINTS            MapWindowPoints;
//H2O_WM_GET_WINDOW_LONG              GetWindowLong;
  H2O_WM_GET_WINDOW_LONG_PTR          GetWindowLongPtr;
//H2O_WM_SET_WINDOW_LONG              SetWindowLong;
  H2O_WM_SET_WINDOW_LONG_PTR          SetWindowLongPtr;
//H2O_WM_GLOBAL_FIND_ATOM             GlobalFindAtom;
//H2O_WM_GLOBAL_ADD_ATOM              GlobalAddAtom;
  H2O_WM_SET_PROP                     SetProp;
  H2O_WM_GET_PROP                     GetProp;
  H2O_WM_REMOVE_PROP                  RemoveProp;
//H2O_WM_GET_CLASS_LONG               GetClassLong;
  H2O_WM_GET_CLASS_LONG_PTR           GetClassLongPtr;
  H2O_WM_GET_WINDOW_TEXT_LENGTH       GetWindowTextLength;
  H2O_WM_MOVE_WINDOW                  MoveWindow;
  H2O_WM_GET_CAPTURE                  GetCapture;
  H2O_WM_SET_CAPTURE                  SetCapture;
  H2O_WM_RELEASE_CAPTURE              ReleaseCapture;
  H2O_WM_GET_SYSTEM_METRICS           GetSystemMetrics;
  H2O_WM_SYSTEM_PARAMETERS_INFO       SystemParametersInfo;
  H2O_WM_GET_DLG_ITEM                 GetDlgItem;
  H2O_WM_ENUM_CHILD_WINDOWS           EnumChildWindows;
  H2O_WM_SET_WINDOW_POS               SetWindowPos;
  H2O_WM_GET_CLIP_BOX                 GetClipBox;
  H2O_WM_SET_DC_PEN_COLOR             SetDCPenColor;
  H2O_WM_SET_DC_BRUSH_COLOR           SetDCBrushColor;
  H2O_WM_REGISTER_HOT_KEY             RegisterHotKey;
  H2O_WM_UNREGISTER_HOT_KEY           UnregisterHotKey;

  //
  // microwindows functions
  //
  H2O_WM_MW_OPEN                      MwOpen;
  H2O_WM_GD_CLOSE_MOUSE               GdCloseMouse;
  H2O_WM_GD_OPEN_MOUSE                GdOpenMouse;
  H2O_WM_MW_FAST_FILL_RECT            MwFastFillRect;
  H2O_WM_GD_DRAW_IMAGE_PART_TO_FIT    GdDrawImagePartToFit;
  H2O_WM_GD_LOAD_IMAGE_FROM_BUFFER    GdLoadImageFromBuffer;
  H2O_WM_GD_LOAD_IMAGE_FROM_BUFFER2   GdLoadImageFromBuffer2;

  H2O_WM_GD_ADD_GOP_DEVICE            GdAddGopDevice;
  H2O_WM_GD_ADD_EFI_KEY_DATA          GdAddEfiKeyData;
  H2O_WM_GD_ADD_POINTER_DATA          GdAddPointerData;

};

extern EFI_GUID gH2OWindowProtocolGuid;

#endif
