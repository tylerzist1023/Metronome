#include <windows.h>
#include <stdio.h>
#include "resource.h"
#include "metronome.h"

int tempo_to_marking(int tempo)
{
    if(tempo <= TEMPO_MARKING_VALUES[0])
        return 0;
    if(tempo >= TEMPO_MARKING_VALUES[TEMPO_MARKINGS_SIZE-1])
        return TEMPO_MARKINGS_SIZE-1;

    for(int i = 0; i < TEMPO_MARKINGS_SIZE; i++)
    {
        if(tempo <= TEMPO_MARKING_VALUES[i])
            return i;
    }

    return -1;
}

// source: https://jonasjohanssongamedev.tumblr.com/post/109695847591/win32-center-window-on-screen
static void center_window(HWND window, DWORD style, DWORD exstyle)
{
    int screen_width = GetSystemMetrics(SM_CXSCREEN);
    int screen_height = GetSystemMetrics(SM_CYSCREEN);
    RECT client_rect;
    GetClientRect(window, &client_rect);
    AdjustWindowRectEx(&client_rect, style, FALSE, exstyle);    
    int client_width = client_rect.right - client_rect.left;
    int client_height = client_rect.bottom - client_rect.top;
    SetWindowPos(window, NULL,
        screen_width / 2 - client_width / 2,
        screen_height / 2 - client_height / 2,
        client_width, client_height, 0
    );
}

struct TempoDialogResult
{
    int tempo_def;
    int tempo;
    bool ok;
};

static INT_PTR CALLBACK tempo_dialog_proc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) 
{
    static TempoDialogResult* tdr_ptr = 0;
    HWND edit1 = GetDlgItem(hwnd, IDC_EDIT1);
    HWND combo1 = GetDlgItem(hwnd, IDC_COMBO1);

    switch(message)
    {
    case WM_INITDIALOG:
        CheckRadioButton(hwnd, IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
        for(int i = 0; i < TEMPO_MARKINGS_SIZE; i++)
        {
            SendMessage(combo1, CB_ADDSTRING, 0, (LPARAM)TEMPO_MARKINGS[i]);
        }
        tdr_ptr = (TempoDialogResult*)lp;

        char buf[32];
        snprintf(buf, 32, "%d", tdr_ptr->tempo_def);
        SetWindowText(edit1, buf);

        SendMessage(combo1, CB_SETCURSEL, (WPARAM)tempo_to_marking(tdr_ptr->tempo_def), 0);
        EnableWindow(combo1, false);
        center_window(hwnd, WS_CAPTION, 0);

        SetFocus(edit1);
		SendDlgItemMessage(hwnd, IDC_EDIT1, EM_SETSEL, 0, -1);

        return FALSE;
    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;
    case WM_COMMAND:
    {
        int ctl = LOWORD(wp);
        int event = HIWORD(wp);
        if(ctl == IDCANCEL && event == BN_CLICKED)
        {
            tdr_ptr->ok = false;

            DestroyWindow(hwnd);
            return TRUE;
        }
        else if(ctl == IDOK && event == BN_CLICKED)
        {
            tdr_ptr->ok = true;

            if(IsDlgButtonChecked(hwnd, IDC_RADIO1) == BST_CHECKED)
            {
                HWND hc = edit1;
                int n = GetWindowTextLength(hc) + 1;
                char s[n];
                GetWindowText(hc, s, n);
                tdr_ptr->tempo = atoi(s);
                if(tdr_ptr->tempo == 0)
                    MessageBox(hwnd, "Invalid tempo", "Error", MB_ICONERROR);
                else
                    DestroyWindow(hwnd);
            }
            else if(IsDlgButtonChecked(hwnd, IDC_RADIO2) == BST_CHECKED)
            {
                LRESULT index = SendMessage(combo1, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
                if(index == CB_ERR)
                    MessageBox(hwnd, "Invalid tempo", "Error", MB_ICONERROR);
                else
                {
                    tdr_ptr->tempo = TEMPO_MARKING_VALUES[index];
                    DestroyWindow(hwnd);
                }
            }

            return TRUE;
        }
        else if(ctl == IDC_RADIO1 && event == BN_CLICKED) // number
        {
            EnableWindow(edit1, true);
            EnableWindow(combo1, false);

            return TRUE;
        }
        else if(ctl == IDC_RADIO2 && event == BN_CLICKED) // marking
        {
            EnableWindow(edit1, false);
            EnableWindow(combo1, true);

            return TRUE;
        }

        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return TRUE;
    }
    return FALSE;
}

bool show_tempo_dialog(int def, int* result)
{
    TempoDialogResult tdr = {0};
    tdr.tempo_def = def;

    HWND dlg = CreateDialogParam(0, MAKEINTRESOURCE(IDD_DIALOG1), 0, tempo_dialog_proc, (LPARAM)&tdr);
    ShowWindow(dlg, SW_SHOW);

    MSG msg;
    while(GetMessage(&msg, 0, 0, 0))
    {
        if(!IsDialogMessage(dlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    if(tdr.ok)
    {
        *result = tdr.tempo;
        return true;
    }
    else
    {
        return false;
    }
}

struct SignatureDialogResult
{
    int beats, beats_def;
    NoteValue value, value_def;
    int ok;
};

static INT_PTR CALLBACK signature_dialog_proc(HWND hwnd, UINT message, WPARAM wp, LPARAM lp) 
{
    static SignatureDialogResult* sdr_ptr = 0;
    HWND hbeats = GetDlgItem(hwnd, IDC_BEATS);
    HWND hvalue = GetDlgItem(hwnd, IDC_VALUE);

    switch(message)
    {
    case WM_INITDIALOG:
        for(int i = 0; i < NOTE_NAMES_SIZE; i++)
        {
            SendMessage(hvalue, CB_ADDSTRING, 0, (LPARAM)NOTE_NAMES[i]);
        }
        sdr_ptr = (SignatureDialogResult*)lp;

        char buf[32];
        snprintf(buf, 32, "%d", sdr_ptr->beats_def);
        SetWindowText(hbeats, buf);
        SendMessage(hvalue, CB_SETCURSEL, (WPARAM)sdr_ptr->value_def, 0);

        center_window(hwnd, WS_CAPTION, 0);

        SetFocus(hbeats);
		SendDlgItemMessage(hwnd, IDC_BEATS, EM_SETSEL, 0, -1);

        return FALSE;
    case WM_DESTROY:
        PostQuitMessage(0);
        return TRUE;
    case WM_COMMAND:
    {
        int ctl = LOWORD(wp);
        int event = HIWORD(wp);
        if(ctl == IDCANCEL && event == BN_CLICKED)
        {
            sdr_ptr->ok = false;

            DestroyWindow(hwnd);
            return TRUE;
        }
        else if(ctl == IDOK && event == BN_CLICKED)
        {
            sdr_ptr->ok = true;

            HWND hc = hbeats;
            int n = GetWindowTextLength(hc) + 1;
            char s[n];
            GetWindowText(hc, s, n);
            sdr_ptr->beats = atoi(s);
            if(sdr_ptr->beats == 0)
            {
                MessageBox(hwnd, "Invalid beat", "Error", MB_ICONERROR);
                return TRUE;
            }

            LRESULT index = SendMessage(hvalue, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
            if(index == CB_ERR)
            {
                MessageBox(hwnd, "Invalid note value", "Error", MB_ICONERROR);
                return TRUE;
            }
            else
            {
                sdr_ptr->value = (NoteValue)index;
                DestroyWindow(hwnd);
            }

            return TRUE;
        }

        break;
    }
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return TRUE;
    }
    return FALSE;
}

bool show_signature_dialog(int beats_def, NoteValue value_def, int* beats, NoteValue* value)
{
    SignatureDialogResult sdr = {0};
    sdr.beats_def = beats_def;
    sdr.value_def = value_def;

    HWND dlg = CreateDialogParam(0, MAKEINTRESOURCE(IDD_DIALOG2), 0, signature_dialog_proc, (LPARAM)&sdr);
    ShowWindow(dlg, SW_SHOW);

    MSG msg;
    while(GetMessage(&msg, 0, 0, 0))
    {
        if(!IsDialogMessage(dlg, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    if(sdr.ok)
    {
        *beats = sdr.beats;
        *value = sdr.value;
        return true;
    }
    else
    {
        return false;
    }
}