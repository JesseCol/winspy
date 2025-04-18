//
//  Options.c
//
//  Copyright (c) 2002 by J Brown
//  Freeware
//
//  Implements the Options dialog for WinSpy
//

#include "WinSpy.h"
#include "RegHelper.h"
#include "resource.h"
#include "utils.h"

Options g_opts;

#define REG_BASESTR  L"Software\\Catch22\\WinSpy++ 1.5"

static WCHAR szRegLoc[] = REG_BASESTR;

void LoadSettings(void)
{
    HKEY hkey;

    RegCreateKeyEx(HKEY_CURRENT_USER, szRegLoc, 0, L"", 0, KEY_READ, NULL, &hkey, NULL);

    g_opts.fSaveWinPos = GetSettingBool(hkey, L"SavePosition", TRUE);
    g_opts.fAlwaysOnTop = GetSettingBool(hkey, L"AlwaysOnTop", FALSE);
    g_opts.fMinimizeWinSpy = GetSettingBool(hkey, L"MinimizeWinSpy", TRUE);
    g_opts.fFullDragging = GetSettingBool(hkey, L"FullDragging", TRUE);
    g_opts.fShowHidden = GetSettingBool(hkey, L"ShowHidden", FALSE);
    g_opts.fShowDimmed = GetSettingBool(hkey, L"ShowDimmed", TRUE);
    g_opts.fClassThenText = GetSettingBool(hkey, L"ClassThenText", TRUE);
    g_opts.fPinWindow = GetSettingBool(hkey, L"PinWindow", FALSE);
    g_opts.fShowInCaption = GetSettingBool(hkey, L"ShowInCaption", TRUE);
    g_opts.fEnableToolTips = GetSettingBool(hkey, L"EnableToolTips", FALSE);
    g_opts.fShowDesktopRoot = GetSettingBool(hkey, L"ShowDesktopRoot", FALSE);
    g_opts.fSelectTreeOnDragEnd = GetSettingBool(hkey, L"SelectTreeOnDragEnd", TRUE);
    g_opts.uTreeInclude = GetSettingInt(hkey, L"TreeItems", WINLIST_INCLUDE_ALL);
    g_opts.fShowHiddenInList = GetSettingBool(hkey, L"List_ShowHidden", TRUE);
    g_opts.fEnableHotkey = GetSettingBool(hkey, L"EnableHotkey", FALSE);

    g_opts.uPinnedCorner = GetSettingInt(hkey, L"PinCorner", 0);

    g_opts.ptPinPos.x = GetSettingInt(hkey, L"xpos", CW_USEDEFAULT);
    g_opts.ptPinPos.y = GetSettingInt(hkey, L"ypos", CW_USEDEFAULT);

    // If the hotkey value isn't there, or is invalid, default to tilde '~'

    int value = GetSettingInt(hkey, L"Hotkey", 0);

    if ((value > 0xFFFF) || (value <= 0))
    {
        g_opts.fEnableHotkey = FALSE;
        value = VK_OEM_3;
    }

    g_opts.wHotkey = (WORD)value;

    // Ignore the saved window position if it no longer lies within the
    // bounds of a monitor.

    if (g_opts.fSaveWinPos && (g_opts.ptPinPos.x != CW_USEDEFAULT) && (g_opts.ptPinPos.y != CW_USEDEFAULT))
    {
        if (!MonitorFromPoint(g_opts.ptPinPos, MONITOR_DEFAULTTONULL))
        {
            g_opts.ptPinPos.x = CW_USEDEFAULT;
            g_opts.ptPinPos.y = CW_USEDEFAULT;
        }
    }

    RegCloseKey(hkey);
}

void SaveSettings(void)
{
    HKEY hkey;

    RegCreateKeyEx(HKEY_CURRENT_USER, szRegLoc, 0, L"", 0, KEY_WRITE, NULL, &hkey, NULL);

    WriteSettingBool(hkey, L"SavePosition", g_opts.fSaveWinPos);
    WriteSettingBool(hkey, L"AlwaysOnTop", g_opts.fAlwaysOnTop);
    WriteSettingBool(hkey, L"MinimizeWinSpy", g_opts.fMinimizeWinSpy);
    WriteSettingBool(hkey, L"FullDragging", g_opts.fFullDragging);
    WriteSettingBool(hkey, L"ShowHidden", g_opts.fShowHidden);
    WriteSettingBool(hkey, L"ShowDimmed", g_opts.fShowDimmed);
    WriteSettingBool(hkey, L"ClassThenText", g_opts.fClassThenText);
    WriteSettingBool(hkey, L"PinWindow", g_opts.fPinWindow);
    WriteSettingBool(hkey, L"ShowInCaption", g_opts.fShowInCaption);
    WriteSettingBool(hkey, L"EnableToolTips", g_opts.fEnableToolTips);
    WriteSettingBool(hkey, L"ShowDesktopRoot", g_opts.fShowDesktopRoot);
    WriteSettingBool(hkey, L"List_ShowHidden", g_opts.fShowHiddenInList);
    WriteSettingInt(hkey, L"TreeItems", g_opts.uTreeInclude);
    WriteSettingInt(hkey, L"PinCorner", g_opts.uPinnedCorner);

    WriteSettingInt(hkey, L"xpos", g_opts.ptPinPos.x);
    WriteSettingInt(hkey, L"ypos", g_opts.ptPinPos.y);

    if (g_fFirstInstance)
    {
        WriteSettingBool(hkey, L"EnableHotkey", g_opts.fEnableHotkey);
        WriteSettingInt(hkey, L"Hotkey", g_opts.wHotkey);
    }

    RegCloseKey(hkey);
}

void OptionsDlg_UpdateHotkeyControls(HWND hwndDlg)
{
    if (!g_fFirstInstance)
    {
        EnableDlgItem(hwndDlg, IDC_OPTIONS_WIN_LABEL, FALSE);
        EnableDlgItem(hwndDlg, IDC_OPTIONS_ENABLE_HOTKEY, FALSE);
        EnableDlgItem(hwndDlg, IDC_HOTKEY, FALSE);
    }
    else
    {
        BOOL fEnabled = IsDlgButtonChecked(hwndDlg, IDC_OPTIONS_ENABLE_HOTKEY);

        EnableDlgItem(hwndDlg, IDC_OPTIONS_WIN_LABEL, fEnabled);
        EnableDlgItem(hwndDlg, IDC_HOTKEY, fEnabled);
    }
}


INT_PTR CALLBACK OptionsDlgProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    static HWND hwndTarget;

    switch (iMsg)
    {
    case WM_INITDIALOG:
        CheckDlgButton(hwnd, IDC_OPTIONS_SAVEPOS, g_opts.fSaveWinPos);
        CheckDlgButton(hwnd, IDC_OPTIONS_FULLDRAG, g_opts.fFullDragging);
        CheckDlgButton(hwnd, IDC_OPTIONS_DIR, g_opts.fClassThenText);
        CheckDlgButton(hwnd, IDC_OPTIONS_SHOWHIDDEN, g_opts.fShowDimmed);
        CheckDlgButton(hwnd, IDC_OPTIONS_SHOWINCAPTION, g_opts.fShowInCaption);
        CheckDlgButton(hwnd, IDC_OPTIONS_TOOLTIPS, g_opts.fEnableToolTips);
        CheckDlgButton(hwnd, IDC_OPTIONS_DESKTOPROOT, g_opts.fShowDesktopRoot);
        CheckDlgButton(hwnd, IDC_OPTIONS_LIST_SHOWHIDDEN, g_opts.fShowHiddenInList);
        CheckDlgButton(hwnd, IDC_OPTIONS_ENABLE_HOTKEY, g_opts.fEnableHotkey);

        CheckDlgButton(hwnd, IDC_OPTIONS_INCHANDLE,
            (g_opts.uTreeInclude & WINLIST_INCLUDE_HANDLE) ? TRUE : FALSE);

        CheckDlgButton(hwnd, IDC_OPTIONS_INCCLASS,
            (g_opts.uTreeInclude & WINLIST_INCLUDE_CLASS) ? TRUE : FALSE);

        OptionsDlg_UpdateHotkeyControls(hwnd);

        SendDlgItemMessage(hwnd, IDC_HOTKEY, HKM_SETHOTKEY, g_opts.wHotkey, 0);

        return TRUE;

    case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDC_OPTIONS_ENABLE_HOTKEY:
            OptionsDlg_UpdateHotkeyControls(hwnd);
            return TRUE;

        case IDOK:

            g_opts.fSaveWinPos = IsDlgButtonChecked(hwnd, IDC_OPTIONS_SAVEPOS);
            g_opts.fFullDragging = IsDlgButtonChecked(hwnd, IDC_OPTIONS_FULLDRAG);
            g_opts.fClassThenText = IsDlgButtonChecked(hwnd, IDC_OPTIONS_DIR);
            g_opts.fShowDimmed = IsDlgButtonChecked(hwnd, IDC_OPTIONS_SHOWHIDDEN);
            g_opts.fShowInCaption = IsDlgButtonChecked(hwnd, IDC_OPTIONS_SHOWINCAPTION);
            g_opts.fEnableToolTips = IsDlgButtonChecked(hwnd, IDC_OPTIONS_TOOLTIPS);
            g_opts.fShowDesktopRoot = IsDlgButtonChecked(hwnd, IDC_OPTIONS_DESKTOPROOT);
            g_opts.fShowHiddenInList = IsDlgButtonChecked(hwnd, IDC_OPTIONS_LIST_SHOWHIDDEN);
            g_opts.fEnableHotkey = IsDlgButtonChecked(hwnd, IDC_OPTIONS_ENABLE_HOTKEY);
            g_opts.wHotkey = (WORD)SendDlgItemMessage(hwnd, IDC_HOTKEY, HKM_GETHOTKEY, 0, 0);

            g_opts.uTreeInclude = 0;

            if (IsDlgButtonChecked(hwnd, IDC_OPTIONS_INCHANDLE))
                g_opts.uTreeInclude |= WINLIST_INCLUDE_HANDLE;

            if (IsDlgButtonChecked(hwnd, IDC_OPTIONS_INCCLASS))
                g_opts.uTreeInclude |= WINLIST_INCLUDE_CLASS;

            EndDialog(hwnd, 0);
            return TRUE;

        case IDCANCEL:
            EndDialog(hwnd, 0);
            return TRUE;
        }

        return FALSE;
    }

    return FALSE;
}


void ShowOptionsDlg(HWND hwndParent)
{
    DialogBox(g_hInst, MAKEINTRESOURCE(IDD_OPTIONS), hwndParent, OptionsDlgProc);

    UpdateMainWindowText();

    UpdateGlobalHotkey();

    SendMessage(g_hwndToolTip, TTM_ACTIVATE, g_opts.fEnableToolTips, 0);
}
