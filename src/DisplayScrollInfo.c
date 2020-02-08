//
//  DisplayScrollInfo.c
//  Copyright (c) 2002 by J Brown
//  Freeware
//
//  void SetScrollInfo(HWND hwnd)
//
//  Fill the scrollbar-tab-pane with scrollbar info for the
//  specified window
//

#include "WinSpy.h"

#include "resource.h"

void SetInfo(HWND hwndDlg, HWND hwnd, BOOL fValid, BOOL fVert, LPCTSTR ach, DWORD dwStyle)
{
    SCROLLINFO si;
    DWORD bartype = fVert ? SB_VERT : SB_HORZ;
    int idc_state = fVert ? IDC_VSTATE : IDC_HSTATE;

    if (fValid)
    {
        si.cbSize = sizeof(SCROLLINFO);
        si.fMask = SIF_ALL;

        if (lstrcmpi(ach, _T("ScrollBar")) == 0)
        {
            static_assert(SBS_HORZ == SB_HORZ && SBS_VERT == SB_VERT, "");
            if ((dwStyle & SBS_DIR_MASK) == bartype)
                bartype = SB_CTL;

            SetDlgItemText(hwndDlg, idc_state, _T("Visible"));
        }
        else
        {
            SetDlgItemText(hwndDlg, idc_state, dwStyle & ((fVert ? WS_VSCROLL : WS_HSCROLL)) ? _T("Visible") : _T("Disabled"));
        }
    }

    if (fValid && GetScrollInfo(hwnd, bartype, &si))
    {
        SetDlgItemInt(hwndDlg, fVert ? IDC_VMIN : IDC_HMIN, si.nMin, TRUE);
        SetDlgItemInt(hwndDlg, fVert ? IDC_VMAX : IDC_HMAX, si.nMax, TRUE);
        SetDlgItemInt(hwndDlg, fVert ? IDC_VPOS : IDC_HPOS, si.nPos, TRUE);
        SetDlgItemInt(hwndDlg, fVert ? IDC_VPAGE : IDC_HPAGE, si.nPage, TRUE);

        if (bartype != SB_CTL)
        {
            SetDlgItemText(hwndDlg, idc_state, dwStyle & ((fVert ? WS_VSCROLL : WS_HSCROLL)) ? _T("Visible") : _T("Hidden"));
        }
    }
    else
    {
        SetDlgItemText(hwndDlg, fVert ? IDC_VMIN : IDC_HMIN, fValid ? _T("") : ach);
        SetDlgItemText(hwndDlg, fVert ? IDC_VMAX : IDC_HMAX, fValid ? _T("") : ach);
        SetDlgItemText(hwndDlg, fVert ? IDC_VPOS : IDC_HPOS, fValid ? _T("") : ach);
        SetDlgItemText(hwndDlg, fVert ? IDC_VPAGE : IDC_HPAGE, fValid ? _T("") : ach);
        SetDlgItemText(hwndDlg, idc_state, fValid ? _T("Disabled") : ach);
    }
}

void SetScrollbarInfo(HWND hwnd)
{
    DWORD dwStyle = 0;
    TCHAR  ach[256];
    HWND   hwndDlg = WinSpyTab[PROPERTY_TAB].hwnd;

    *ach = 0;

    BOOL fValid = hwnd != NULL;
    if (hwnd && !IsWindow(hwnd))
    {
        fValid = FALSE;
        _tcscpy_s(ach, ARRAYSIZE(ach), szInvalidWindow);
    }

    if (fValid)
    {
        GetClassName(hwnd, ach, ARRAYSIZE(ach));

        dwStyle = GetWindowLong(hwnd, GWL_STYLE);
    }

    SetInfo(hwndDlg, hwnd, fValid, FALSE, ach, dwStyle);
    SetInfo(hwndDlg, hwnd, fValid, TRUE, ach, dwStyle);
}
