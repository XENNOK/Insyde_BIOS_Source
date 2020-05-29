
#include <Protocol/H2OWindow.h>

H2O_WINDOW_PROTOCOL *gH2OWindow;

LRESULT
DefWindowProc (
  HWND hwnd,
  UINT msg,
  WPARAM wParam,
  LPARAM lParam
  )
{
  return  gH2OWindow->DefWindowProc (
                        hwnd,
                        msg,
                        wParam,
                        lParam
                        );
}

void
MwSetKeyboardTranslator (
  LPFN_KEYBTRANSLATE pFn
  )
{
  gH2OWindow->MwSetKeyboardTranslator (pFn);
}

HDC
GetDCEx (
  HWND hwnd,
  HRGN hrgnClip,
  DWORD flags
  )
{
  return  gH2OWindow->GetDCEx (
                        hwnd,
                        hrgnClip,
                        flags
                        );
}

HDC
GetDC (
  HWND hwnd
  )
{
  return  gH2OWindow->GetDC (
                        hwnd
                        );
}

HDC
GetWindowDC (
  HWND hwnd
  )
{
  return  gH2OWindow->GetWindowDC (
                        hwnd
                        );
}

int
ReleaseDC (
  HWND hwnd,
  HDC hdc
  )
{
  return  gH2OWindow->ReleaseDC (
                        hwnd,
                        hdc
                        );
}

BOOL
DeleteDC (
  HDC hdc
  )
{
  return  gH2OWindow->DeleteDC (
                        hdc
                        );
}

HDC
BeginPaint (
  HWND hwnd,
  LPPAINTSTRUCT lpPaint
  )
{
  return  gH2OWindow->BeginPaint (
                        hwnd,
                        lpPaint
                        );
}

BOOL
EndPaint (
  HWND hwnd,
  CONST PAINTSTRUCT *lpPaint
  )
{
  return  gH2OWindow->EndPaint (
                        hwnd,
                        lpPaint
                        );
}

COLORREF
SetTextColor (
  HDC hdc,
  COLORREF crColor
  )
{
  return  gH2OWindow->SetTextColor (
                        hdc,
                        crColor
                        );
}

COLORREF
GetTextColor (
  HDC hdc
  )
{
  return  gH2OWindow->GetTextColor (
                        hdc
                        );
}

COLORREF
SetBkColor (
  HDC hdc,
  COLORREF crColor
  )
{
  return  gH2OWindow->SetBkColor (
                        hdc,
                        crColor
                        );
}

int
SetBkMode (
  HDC hdc,
  int iBkMode
  )
{
  return  gH2OWindow->SetBkMode (
                        hdc,
                        iBkMode
                        );
}

UINT
SetTextAlign (
  HDC hdc,
  UINT fMode
  )
{
  return  gH2OWindow->SetTextAlign (
                        hdc,
                        fMode
                        );
}

UINT
GetTextAlign (
  HDC hdc
  )
{
  return  gH2OWindow->GetTextAlign (
                        hdc
                        );
}

int
SetROP2 (
  HDC hdc,
  int fnDrawMode
  )
{
  return  gH2OWindow->SetROP2 (
                        hdc,
                        fnDrawMode
                        );
}

COLORREF
GetPixel (
  HDC hdc,
  int x,
  int y
  )
{
  return  gH2OWindow->GetPixel (
                        hdc,
                        x,
                        y
                        );
}

COLORREF
SetPixel (
  HDC hdc,
  int x,
  int y,
  COLORREF crColor
  )
{
  return  gH2OWindow->SetPixel (
                        hdc,
                        x,
                        y,
                        crColor
                        );
}

BOOL
MoveToEx (
  HDC hdc,
  int x,
  int y,
  LPPOINT lpPoint
  )
{
  return  gH2OWindow->MoveToEx (
                        hdc,
                        x,
                        y,
                        lpPoint
                        );
}

BOOL
LineTo (
  HDC hdc,
  int x,
  int y
  )
{
  return  gH2OWindow->LineTo (
                        hdc,
                        x,
                        y
                        );
}

BOOL
Polyline (
  HDC hdc,
  CONST POINT *lppt,
  int cPoints
  )
{
  return  gH2OWindow->Polyline (
                        hdc,
                        lppt,
                        cPoints
                        );
}

BOOL
Rectangle (
  HDC hdc,
  int nLeft,
  int nTop,
  int nRight,
  int nBottom
  )
{
  return  gH2OWindow->Rectangle (
                        hdc,
                        nLeft,
                        nTop,
                        nRight,
                        nBottom
                        );
}
//
//BOOL
//Ellipse (
//HDC hdc,
//int nLeftRect,
//int nTopRect,
//int nRightRect,
//int nBottomRect
//)
//{
//return  gH2OWindow->Ellipse (
//                      hdc,
//                      nLeftRect,
//                      nTopRect,
//                      nRightRect,
//                      nBottomRect
//                      );
//}

BOOL
Polygon (
  HDC hdc,
  CONST POINT *lpPoints,
  int nCount
  )
{
  return  gH2OWindow->Polygon (
                        hdc,
                        lpPoints,
                        nCount
                        );
}

BOOL
PolyPolygon (
  HDC hdc,
  CONST POINT *lpPoints,
  LPINT lpPolyCounts,
  int nCount
  )
{
  return  gH2OWindow->PolyPolygon (
                        hdc,
                        lpPoints,
                        lpPolyCounts,
                        nCount
                        );
}

int
FillRect (
  HDC hdc,
  CONST RECT *lprc,
  HBRUSH hbr
  )
{
  return  gH2OWindow->FillRect (
                        hdc,
                        lprc,
                        hbr
                        );
}

void
MwSetTextCoding (
  long mode
  )
{
	gH2OWindow->MwSetTextCoding (
                  mode
                  );
}

BOOL
TextOut  (
  HDC Hdc,
  int x,
  int y,
  LPCWSTR String,
  int cbString
  )
{
  return  gH2OWindow->TextOut  (
                        Hdc,
                        x,
                        y,
                        String,
                        cbString
                        );
}
//
//BOOL
//TextOutW (
//HDC hdc,
//int x,
//int y,
//LPCWSTR lpszString,
//int cbString
//)
//{
//return  gH2OWindow->TextOutW (
//                      hdc,
//                      x,
//                      y,
//                      lpszString,
//                      cbString
//                      );
//}

//BOOL
//ExtTextOutW (
//HDC hdc,
//int x,
//int y,
//UINT fuOptions,
//CONST RECT *lprc
//)
//{
//gH2OWindow->ExtTextOutW (
//              hdc,
//              x,
//              y,
//              fuOptions,
//              lprc
//              );
//}

//int
//DrawTextA (
//HDC hdc,
//LPCSTR lpString,
//int nCount,
//LPRECT lpRect,
//UINT uFormat
//)
//{
//return  gH2OWindow->DrawTextA (
//                      hdc,
//                      lpString,
//                      nCount,
//                      lpRect,
//                      uFormat
//                      );
//}

int
DrawText  (
  HDC hdc,
  LPCWSTR lpString,
  int nCount,
  LPRECT lpRect,
  UINT uFormat
  )
{
  return  gH2OWindow->DrawText  (
                        hdc,
                        lpString,
                        nCount,
                        lpRect,
                        uFormat
                        );
}

DWORD
GetSysColor (
  int nIndex
  )
{
  return  gH2OWindow->GetSysColor (
                        nIndex
                        );
}

COLORREF
SetSysColor (
  int nIndex,
  COLORREF crColor
  )
{
  return  gH2OWindow->SetSysColor (
                        nIndex,
                        crColor
                        );
}

HBRUSH
GetSysColorBrush (
  int nIndex
  )
{
  return  gH2OWindow->GetSysColorBrush (
                        nIndex
                        );
}

HGDIOBJ
GetStockObject (
  int nObject
  )
{
  return  gH2OWindow->GetStockObject (
                        nObject
                        );
}

HGDIOBJ
SelectObject (
  HDC hdc,
  HGDIOBJ hObject
  )
{
  return  gH2OWindow->SelectObject (
                        hdc,
                        hObject
                        );
}

BOOL
DeleteObject (
  HGDIOBJ hObject
  )
{
  return  gH2OWindow->DeleteObject (
                        hObject
                        );
}

int
SelectClipRgn (
  HDC hdc,
  HRGN hrgn
  )
{
  return  gH2OWindow->SelectClipRgn (
                        hdc,
                        hrgn
                        );
}

int
ExtSelectClipRgn (
  HDC hdc,
  HRGN hrgn,
  int fnMode
  )
{
  return  gH2OWindow->ExtSelectClipRgn (
                        hdc,
                        hrgn,
                        fnMode
                        );
}

int
GetUpdateRgn (
  HWND hwnd,
  HRGN hrgn,
  BOOL bErase
  )
{
  return  gH2OWindow->GetUpdateRgn (
                        hwnd,
                        hrgn,
                        bErase
                        );
}

BOOL
GetUpdateRect (
  HWND hwnd,
  LPRECT lpRect,
  BOOL bErase
  )
{
  return  gH2OWindow->GetUpdateRect (
                        hwnd,
                        lpRect,
                        bErase
                        );
}

HBRUSH
CreateSolidBrush (
  COLORREF crColor
  )
{
  return  gH2OWindow->CreateSolidBrush (
                        crColor
                        );
}

HPEN
CreatePen (
  int nPenStyle,
  int nWidth,
  COLORREF crColor
  )
{
  return  gH2OWindow->CreatePen (
                        nPenStyle,
                        nWidth,
                        crColor
                        );
}

HBITMAP
CreateCompatibleBitmap (
  HDC hdc,
  int nWidth,
  int nHeight
  )
{
  return  gH2OWindow->CreateCompatibleBitmap (
                        hdc,
                        nWidth,
                        nHeight
                        );
}

HDC
CreateCompatibleDC (
  HDC hdc
  )
{
  return  gH2OWindow->CreateCompatibleDC (
                        hdc
                        );
}

BOOL
BitBlt (
  HDC hdcDest,
  int nXDest,
  int nYDest,
  int nWidth,
  int nHeight,
  HDC hdcSrc,
  int nXSrc,
  int nYSrc,
  DWORD dwRop
  )
{
  return  gH2OWindow->BitBlt (
                        hdcDest,
                        nXDest,
                        nYDest,
                        nWidth,
                        nHeight,
                        hdcSrc,
                        nXSrc,
                        nYSrc,
                        dwRop
                        );
}

BOOL
StretchBlt (
  HDC hdcDest,
  int nXOriginDest,
  int nYOriginDest,
  int nWidthDest,
  int nHeightDest,
  HDC hdcSrc,
  int nXOriginSrc,
  int nYOriginSrc,
  int nWidthSrc,
  int nHeightSrc,
  DWORD dwRop
  )
{
  return  gH2OWindow->StretchBlt (
                        hdcDest,
                        nXOriginDest,
                        nYOriginDest,
                        nWidthDest,
                        nHeightDest,
                        hdcSrc,
                        nXOriginSrc,
                        nYOriginSrc,
                        nWidthSrc,
                        nHeightSrc,
                        dwRop
                        );
}
//
//UINT
//GetSystemPaletteEntries (
//HDC hdc,
//UINT iStartIndex,
//UINT nEntries
//)
//{
//gH2OWindow->GetSystemPaletteEntries (
//              hdc,
//              iStartIndex,
//              nEntries
//              );
//}

int
GetDeviceCaps (
  HDC hdc,
  int nIndex
  )
{
  return  gH2OWindow->GetDeviceCaps (
                        hdc,
                        nIndex
                        );
}

//BOOL
//DrawFocusRect (
//HDC hdc,
//LPRECT prect
//)
//{
//return  gH2OWindow->DrawFocusRect (
//                      hdc,
//                      prect
//                      );
//}
//
//LONG
//TabbedTextOut (
//HDC hdc,
//int x,
//int y,
//LPCTSTR lpszString,
//int cbString
//)
//{
//gH2OWindow->TabbedTextOut (
//              hdc,
//              x,
//              y,
//              lpszString,
//              cbString
//              );
//}

//DWORD
//GetTabbedTextExtent (
//HDC hdc,
//int x,
//int y,
//LPCTSTR lpszString,
//int cbString
//)
//{
//gH2OWindow->GetTabbedTextExtent (
//              hdc,
//              x,
//              y,
//              lpszString,
//              cbString
//              );
//}

//int
//MulDiv (
//int nMultiplicand,
//int nMultiplier,
//int nDivisor
//)
//{
//gH2OWindow->MulDiv (
//              nMultiplicand,
//              nMultiplier,
//              nDivisor
//              );
//}

BOOL
GetTextExtentPoint32 (
  HDC hdc,    /* handle to DC*/
  LPCWSTR lpszStr,  /* character string*/
  int cchString,    /* number of characters*/
  LPSIZE lpSize    /* string dimensions*/
  )
{
  return  gH2OWindow->GetTextExtentPoint32 (
                        hdc,    /* handle to DC*/
                        lpszStr,  /* character string*/
                        cchString,    /* number of characters*/
                        lpSize    /* string dimensions*/
                        );
}

UINT64
GetTickCount (
  VOID
  )
{
  return  gH2OWindow->GetTickCount ();
}
//
//VOID
//Sleep (
//DWORD dwMilliseconds
//)
//{
//gH2OWindow->Sleep (
//              dwMilliseconds
//              );
//}

INT
OffsetRgn (
  HRGN hrgn,
  INT x,
  INT y
  )
{
  return  gH2OWindow->OffsetRgn (
                        hrgn,
                        x,
                        y
                        );
}

INT
GetRgnBox (
  HRGN hrgn,
  LPRECT rect
  )
{
  return  gH2OWindow->GetRgnBox (
                        hrgn,
                        rect
                        );
}

HRGN
CreateRectRgn (
  INT left,
  INT top,
  INT right,
  INT bottom
  )
{
  return  gH2OWindow->CreateRectRgn (
                        left,
                        top,
                        right,
                        bottom
                        );
}

HRGN
CreateRectRgnIndirect (
  const RECT* rect
  )
{
  return  gH2OWindow->CreateRectRgnIndirect (
                        rect
                        );
}

VOID
SetRectRgn (
  HRGN hrgn,
  INT left,
  INT top,
  INT right,
  INT bottom
  )
{
  gH2OWindow->SetRectRgn (
                hrgn,
                left,
                top,
                right,
                bottom
                );
}

HRGN
CreateRoundRectRgn (
  INT left,
  INT top,
  INT right,
  INT bottom,
  INT ellipse_width,
  INT ellipse_height
  )
{
  return  gH2OWindow->CreateRoundRectRgn (
                        left,
                        top,
                        right,
                        bottom,
                        ellipse_width,
                        ellipse_height
                        );
}

HRGN
CreateEllipticRgn (
  INT left,
  INT top,
  INT right,
  INT bottom
  )
{
  return  gH2OWindow->CreateEllipticRgn (
                        left,
                        top,
                        right,
                        bottom
                        );
}

HRGN
CreateEllipticRgnIndirect (
  const RECT *rect
  )
{
  return  gH2OWindow->CreateEllipticRgnIndirect (
                        rect
                        );
}

HRGN
CreatePolygonRgn (
  const POINT *points,
  INT count,
  INT mode
  )
{
  return  gH2OWindow->CreatePolygonRgn (
                        points,
                        count,
                        mode
                        );
}

DWORD
GetRegionData (
  HRGN hrgn,
  DWORD count,
  LPRGNDATA rgndata
  )
{
  return  gH2OWindow->GetRegionData (
                        hrgn,
                        count,
                        rgndata
                        );
}
//
//HRGN
//ExtCreateRegion (
//const XFORM* lpXform,
//DWORD dwCount,
//const RGNDATA* rgndata
//)
//{
//gH2OWindow->ExtCreateRegion (
//              lpXform,
//              dwCount,
//              rgndata
//              );
//}

BOOL
PtInRegion (
  HRGN hrgn,
  INT x,
  INT y
  )
{
  return  gH2OWindow->PtInRegion (
                        hrgn,
                        x,
                        y
                        );
}

BOOL
RectInRegion (
  HRGN hrgn,
  const RECT *rect
  )
{
  return  gH2OWindow->RectInRegion (
                        hrgn,
                        rect
                        );
}

BOOL
EqualRgn (
  HRGN hrgn1,
  HRGN hrgn2
  )
{
  return  gH2OWindow->EqualRgn (
                        hrgn1,
                        hrgn2
                        );
}

INT
CombineRgn (
  HRGN hDest,
  HRGN hSrc1,
  HRGN hSrc2,
  INT mode
  )
{
  return  gH2OWindow->CombineRgn (
                        hDest,
                        hSrc1,
                        hSrc2,
                        mode
                        );
}

LRESULT
CallWindowProc (
  WNDPROC lpPrevWndFunc,
  HWND hwnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam
  )
{
  return  gH2OWindow->CallWindowProc (
                        lpPrevWndFunc,
                        hwnd,
                        Msg,
                        wParam,
                        lParam
                        );
}

LRESULT
SendMessage (
  HWND hwnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam
  )
{
  return  gH2OWindow->SendMessage (
                        hwnd,
                        Msg,
                        wParam,
                        lParam
                        );
}

BOOL
PostMessage (
  HWND hwnd,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam
  )
{
  return  gH2OWindow->PostMessage (
                        hwnd,
                        Msg,
                        wParam,
                        lParam
                        );
}

BOOL
PostThreadMessage (
  DWORD dwThreadId,
  UINT Msg,
  WPARAM wParam,
  LPARAM lParam
  )
{
  return  gH2OWindow->PostThreadMessage (
                        dwThreadId,
                        Msg,
                        wParam,
                        lParam
                        );
}

VOID
PostQuitMessage (
  int nExitCode
  )
{
  gH2OWindow->PostQuitMessage (
                nExitCode
                );
}

BOOL
PeekMessage (
  LPMSG lpMsg,
  HWND hwnd,
  UINT uMsgFilterMin,
  UINT uMsgFilterMax,
  UINT32 wRemoveMsg
  )
{
  return  gH2OWindow->PeekMessage (
                        lpMsg,
                        hwnd,
                        uMsgFilterMin,
                        uMsgFilterMax,
                        wRemoveMsg
                        );
}

BOOL
GetMessage (
  LPMSG lpMsg,
  HWND hwnd,
  UINT wMsgFilterMin,
  UINT wMsgFilterMax
  )
{
  return  gH2OWindow->GetMessage (
                        lpMsg,
                        hwnd,
                        wMsgFilterMin,
                        wMsgFilterMax
                        );
}

BOOL
TranslateMessage (
  CONST MSG *lpMsg
  )
{
  return  gH2OWindow->TranslateMessage (
                        lpMsg
                        );
}

LRESULT
DispatchMessage (
  CONST MSG *lpMsg
  )
{
  return  gH2OWindow->DispatchMessage (
                        lpMsg
                        );
}

ATOM
RegisterClass (
  CONST WNDCLASS *lpWndClass
  )
{
  return  gH2OWindow->RegisterClass (
                        lpWndClass
                        );
}

BOOL
UnregisterClass (
  LPCWSTR lpClassName,
  HINSTANCE hInstance
  )
{
  return  gH2OWindow->UnregisterClass (
                        lpClassName,
                        hInstance
                        );
}

HWND
CreateWindowEx (
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
  )
{
  return gH2OWindow->CreateWindowEx (
                       dwExStyle,
                       lpClassName,
                       lpWindowName,
                       dwStyle,
                       x,
                       y,
                       nWidth,
                       nHeight,
                       hwndParent,
                       hMenu,
                       hInstance,
                       lpParam
                       );
}

BOOL
DestroyWindow (
  HWND hwnd
  )
{
  return  gH2OWindow->DestroyWindow (
                        hwnd
                        );
}

BOOL
IsWindow (
  HWND hwnd
  )
{
  return  gH2OWindow->IsWindow (
                        hwnd
                        );
}

BOOL
ShowWindow (
  HWND hwnd,
  int nCmdShow
  )
{
  return  gH2OWindow->ShowWindow (
                        hwnd,
                        nCmdShow
                        );
}

BOOL
InvalidateRect (
  HWND hwnd,
  CONST RECT *lpRect,
  BOOL bErase
  )
{
  return  gH2OWindow->InvalidateRect (
                        hwnd,
                        lpRect,
                        bErase
                        );
}

BOOL
InvalidateRgn (
  HWND hwnd,
  HRGN hrgn,
  BOOL bErase
  )
{
  return  gH2OWindow->InvalidateRgn (
                        hwnd,
                        hrgn,
                        bErase
                        );
}

BOOL
ValidateRect (
  HWND hwnd,
  CONST RECT *lprc
  )
{
  return  gH2OWindow->ValidateRect (
                        hwnd,
                        lprc
                        );
}

BOOL
ValidateRgn (
  HWND hwnd,
  HRGN hrgn
  )
{
  return  gH2OWindow->ValidateRgn (
                        hwnd,
                        hrgn
                        );
}

BOOL
UpdateWindow (
  HWND hwnd
  )
{
  return  gH2OWindow->UpdateWindow (
                        hwnd
                        );
}

HWND
GetFocus (
  VOID
  )
{
  return  gH2OWindow->GetFocus ();

}

HWND
SetFocus (
  HWND hwnd
  )
{
  return  gH2OWindow->SetFocus (
                        hwnd
                        );
}

BOOL
SetForegroundWindow (
  HWND hwnd
  )
{
  return  gH2OWindow->SetForegroundWindow (
                        hwnd
                        );
}

HWND
SetActiveWindow (
  HWND hwnd
  )
{
  return  gH2OWindow->SetActiveWindow (
                        hwnd
                        );
}

HWND
GetActiveWindow (
  VOID
  )
{
  return  gH2OWindow->GetActiveWindow ();
}

BOOL
BringWindowToTop (
  HWND hwnd
  )
{
  return  gH2OWindow->BringWindowToTop (
                        hwnd
                        );
}

HWND
GetDesktopWindow (
  VOID
  )
{
  return  gH2OWindow->GetDesktopWindow ();
}

HWND
GetParent (
  HWND hwnd
  )
{
  return  gH2OWindow->GetParent (
                        hwnd
                        );
}

BOOL
EnableWindow (
  HWND hwnd,
  BOOL bEnable
  )
{
  return  gH2OWindow->EnableWindow (
                        hwnd,
                        bEnable
                        );
}

BOOL
AdjustWindowRectEx (
  LPRECT lpRect,
  DWORD dwStyle,
  BOOL bMenu,
  DWORD dwExStyle
  )
{
  return  gH2OWindow->AdjustWindowRectEx (
                        lpRect,
                        dwStyle,
                        bMenu,
                        dwExStyle
                        );
}

BOOL
GetClientRect (
  HWND hwnd,
  LPRECT lpRect
  )
{
  return  gH2OWindow->GetClientRect (
                        hwnd,
                        lpRect
                        );
}

BOOL
GetWindowRect (
  HWND hwnd,
  LPRECT lpRect
  )
{
  return  gH2OWindow->GetWindowRect (
                        hwnd,
                        lpRect
                        );
}

BOOL
ClientToScreen (
  HWND hwnd,
  LPPOINT lpPoint
  )
{
  return  gH2OWindow->ClientToScreen (
                        hwnd,
                        lpPoint
                        );
}

BOOL
ScreenToClient (
  HWND hwnd,
  LPPOINT lpPoint
  )
{
  return  gH2OWindow->ScreenToClient (
                        hwnd,
                        lpPoint
                        );
}

int
MapWindowPoints (
  HWND hwndFrom,
  HWND hwndTo,
  LPPOINT lpPoints,
  UINT cPoints
  )
{
  return  gH2OWindow->MapWindowPoints (
                        hwndFrom,
                        hwndTo,
                        lpPoints,
                        cPoints
                        );
}


//
//LONG
//GetWindowLong (
//HWND hwnd,
//int nIndex
//)
//{
//return  gH2OWindow->GetWindowLong (
//                      hwnd,
//                      nIndex
//                      );
//}

INTN
GetWindowLongPtr  (
  HWND hwnd,
  INT32 nIndex
  )
{
  return  gH2OWindow->GetWindowLongPtr  (
                        hwnd,
                        nIndex
                        );
}
//
//LONG
//SetWindowLong (
//HWND hwnd,
//int nIndex,
//LONG lNewLong
//)
//{
//return  gH2OWindow->SetWindowLong (
//                      hwnd,
//                      nIndex,
//                      lNewLong
//                      );
//}

INTN
SetWindowLongPtr  (
  HWND hwnd,
  INT32 nIndex,
  INTN lNewLong
  )
{
  return  gH2OWindow->SetWindowLongPtr  (
                        hwnd,
                        nIndex,
                        lNewLong
                        );
}

WORD
GetWindowWord (
  HWND hwnd,
  int nIndex
  )
{
  return  gH2OWindow->GetWindowWord (
                        hwnd,
                        nIndex
                        );
}

WORD
SetWindowWord (
  HWND hwnd,
  int nIndex,
  WORD wNewWord
  )
{
  return  gH2OWindow->SetWindowWord (
                        hwnd,
                        nIndex,
                        wNewWord
                        );
}

//ATOM
//GlobalFindAtom (
//LPCWSTR lpString
//)
//{
//return  gH2OWindow->GlobalFindAtom (
//                      lpString
//                      );
//}
//
//ATOM
//GlobalAddAtom (
//LPCWSTR lpString
//)
//{
//return  gH2OWindow->GlobalAddAtom (
//                      lpString
//                      );
//}

BOOL
SetProp (
  HWND hWnd,
  LPCWSTR lpString,
  HANDLE hData
  )
{
  return  gH2OWindow->SetProp (
                        hWnd,
                        lpString,
                        hData
                        );
}

HANDLE
GetProp (
  HWND hWnd,
  LPCWSTR lpString
  )
{
  return  gH2OWindow->GetProp (
                        hWnd,
                        lpString
                        );
}

HANDLE
RemoveProp (
  HWND hWnd,
  LPCWSTR lpString
  )
{
  return  gH2OWindow->RemoveProp (
                        hWnd,
                        lpString
                        );
}

//DWORD
//GetClassLong (
//HWND hwnd,
//int nIndex
//)
//{
//return  gH2OWindow->GetClassLong (
//                      hwnd,
//                      nIndex
//                      );
//}

UINTN
GetClassLongPtr (
  HWND hwnd,
  int nIndex
  )
{
  return  gH2OWindow->GetClassLongPtr (
                        hwnd,
                        nIndex
                        );
}

int
GetWindowTextLength (
  HWND hwnd
  )
{
  return  gH2OWindow->GetWindowTextLength (
                        hwnd
                        );
}

int
GetWindowText (
  HWND hwnd,
  LPWSTR lpString,
  int nMaxCount
  )
{
  return  gH2OWindow->GetWindowText (
                        hwnd,
                        lpString,
                        nMaxCount
                        );
}

BOOL
SetWindowText (
  HWND hwnd,
  LPCWSTR lpString
  )
{
  return  gH2OWindow->SetWindowText (
                        hwnd,
                        lpString
                        );
}

BOOL
MoveWindow (
  HWND hwnd,
  int x,
  int y,
  int nWidth,
  int nHeight,
  BOOL bRepaint
  )
{
  return  gH2OWindow->MoveWindow (
                        hwnd,
                        x,
                        y,
                        nWidth,
                        nHeight,
                        bRepaint
                        );
}

BOOL
GetCursorPos (
  LPPOINT lpPoint
  )
{
  return  gH2OWindow->GetCursorPos (
                        lpPoint
                        );
}

HWND
GetCapture (
  VOID
  )
{
  return  gH2OWindow->GetCapture ();
}

HWND
SetCapture (
  HWND hwnd
  )
{
  return  gH2OWindow->SetCapture (
                        hwnd
                        );
}

BOOL
ReleaseCapture (
  VOID
  )
{
  return  gH2OWindow->ReleaseCapture ();
}

UINT
SetTimer (
  HWND hwnd,
  UINT idTimer,
  UINT uTimeout,
  TIMERPROC lpTimerFunc
  )
{
  return  gH2OWindow->SetTimer (
                        hwnd,
                        idTimer,
                        uTimeout,
                        lpTimerFunc
                        );
}

BOOL
KillTimer (
  HWND hwnd,
  UINT idTimer
  )
{
  return  gH2OWindow->KillTimer (
                        hwnd,
                        idTimer
                        );
}

int
GetSystemMetrics (
  int nIndex
  )
{
  return  gH2OWindow->GetSystemMetrics (
                        nIndex
                        );
}

BOOL
SystemParametersInfo  (
  UINT uiAction,
  UINT uiParam,
  PVOID pvParam,
  UINT fWinIni
  )
{
  return  gH2OWindow->SystemParametersInfo  (
                        uiAction,
                        uiParam,
                        pvParam,
                        fWinIni
                        );
}

HWND
GetDlgItem (
  HWND hDlg,
  int nIDDlgItem
  )
{
  return  gH2OWindow->GetDlgItem (
                        hDlg,
                        nIDDlgItem
                        );
}

BOOL
EnumChildWindows (
  HWND hWndParent,
  WNDENUMPROC lpEnumFunc,
  LPARAM lParam
  )
{
  return  gH2OWindow->EnumChildWindows (
                        hWndParent,
                        lpEnumFunc,
                        lParam
                        );
}


BOOL
SetWindowPos (
  HWND hwnd,
  HWND hwndInsertAfter,
  int x,
  int y,
  int cx,
  int cy,
  UINT fuFlags
  )
{
  return gH2OWindow->SetWindowPos (
                       hwnd,
                       hwndInsertAfter,
                       x,
                       y,
                       cx,
                       cy,
                       fuFlags
                       );
}


int
GetClipBox (
  HDC   Hdc,
  RECT  *Rc
  )
{
  return gH2OWindow->GetClipBox (
                       Hdc,
                       Rc
                       );
}

COLORREF
SetDCPenColor (
  HDC Hdc,
  COLORREF Color
  )
{
  return gH2OWindow->SetDCPenColor (
                       Hdc,
                       Color
                       );

}

COLORREF
SetDCBrushColor (
  HDC Hdc,
  COLORREF Color
  )
{
  return gH2OWindow->SetDCBrushColor (
                       Hdc,
                       Color
                       );

}


BOOL
MwRegisterHotKey (
  HWND   hWnd,
  INT32  id,
  UINT32 fsModifiers,
  UINT32 vk
  )
{
  return gH2OWindow->RegisterHotKey (
                       hWnd,
                       id,
                       fsModifiers,
                       vk
                       );
}

BOOL
UnregisterHotKey (
  HWND   hWnd,
  INT32  id
  )
{
  return gH2OWindow->UnregisterHotKey (
                       hWnd,
                       id
                       );
}


//
// microwindows functions
//
int
MwOpen(void)
{
  return gH2OWindow->MwOpen ();
}

void
GdCloseMouse(void)
{
  gH2OWindow->GdCloseMouse ();
}

int
GdOpenMouse(void)
{
  return gH2OWindow->GdOpenMouse ();
}

int
MwFastFillRect (
  IN HDC        hDC,
  IN CONST RECT *lprc,
  IN COLORREF   crColor
  )
{
  return gH2OWindow->MwFastFillRect (
                       hDC,
                       lprc,
                       crColor
                       );
}


VOID
GdDrawImagePartToFit (
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
  )
{
  gH2OWindow->GdDrawImagePartToFit (
                psd,
                x,
                y,
                width,
                height,
                sx,
                sy,
                swidth,
                sheight,
                pmd
                );


}

PSD
GdLoadImageFromBuffer (
  IN  EFI_IMAGE_INPUT      *Image
  )
{
  return gH2OWindow->GdLoadImageFromBuffer (
                       Image
                       );

}

PSD
GdLoadImageFromBuffer2 (
  IN VOID                       *Buffer,
  IN INT32                      Size,
  IN INT32                      Flags
  )
{
  return gH2OWindow->GdLoadImageFromBuffer2 (
                       Buffer,
                       Size,
                       Flags
                       );

}


EFI_STATUS
GdAddGopDevice (
  EFI_GRAPHICS_OUTPUT_PROTOCOL  *Gop
  )
{
  return gH2OWindow->GdAddGopDevice (
                       Gop
                       );

}

EFI_STATUS
GdAddEfiKeyData (
  EFI_KEY_DATA *KeyData
  )
{
  return gH2OWindow->GdAddEfiKeyData (
                       KeyData
                       );
}

EFI_STATUS
EFIAPI
GdAddPointerData (
  EFI_SIMPLE_POINTER_STATE      *SimplePointerState    OPTIONAL,
  EFI_ABSOLUTE_POINTER_STATE    *AbsolutePointerState  OPTIONAL
  )
{
  return gH2OWindow->GdAddPointerData (
                       SimplePointerState,
                       AbsolutePointerState
                       );
}


