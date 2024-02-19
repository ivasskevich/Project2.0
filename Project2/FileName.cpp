#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include <map>

BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE hInst1;
std::map<int, HWND> statics;
int rectCount = 0;
POINT startPoint;

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpszCmdLine, int nCmdShow)
{
    hInst1 = hInst;
    MSG msg;
    HWND hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, (DLGPROC)DlgProc);
    ShowWindow(hDialog, nCmdShow);
    while (GetMessage(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT mes, WPARAM wp, LPARAM lp)
{
    switch (mes)
    {
    case WM_CLOSE:
        DestroyWindow(hWnd);
        PostQuitMessage(0);
        return TRUE;
    case WM_CONTEXTMENU:
    {
        POINT pt;
        pt.x = LOWORD(lp);
        pt.y = HIWORD(lp);
        ScreenToClient(hWnd, &pt);

        HWND targetStatic = NULL;
        int highestOrder = 0;
        for (auto it = statics.rbegin(); it != statics.rend(); ++it) {
            RECT rect;
            GetWindowRect(it->second, &rect);
            MapWindowPoints(NULL, hWnd, (LPPOINT)&rect, 2);
            if (PtInRect(&rect, pt)) {
                targetStatic = it->second;
                highestOrder = it->first;
                break;
            }
        }

        if (targetStatic != NULL) {
            TCHAR info[100];
            RECT rect;
            GetWindowRect(targetStatic, &rect);
            MapWindowPoints(NULL, hWnd, (LPPOINT)&rect, 2);
            wsprintf(info, TEXT("Static %d: Weidth %d, Hight %d, x&y (%d, %d)"), highestOrder, rect.right - rect.left, rect.bottom - rect.top, rect.left, rect.top);
            SetWindowText(hWnd, info);
        }
    }
    break;
    case WM_LBUTTONDOWN:
    {
        startPoint.x = LOWORD(lp);
        startPoint.y = HIWORD(lp);
        SetCapture(hWnd);
    }
    break;
    case WM_MOUSEMOVE:
    {
        if (wp & MK_LBUTTON)
        {
            int currentX = LOWORD(lp);
            int currentY = HIWORD(lp);
            RECT previewRect;

            if (currentX > startPoint.x) {
                previewRect.left = startPoint.x;
                previewRect.right = currentX;
            }
            else {
                previewRect.left = currentX;
                previewRect.right = startPoint.x;
            }

            if (currentY > startPoint.y) {
                previewRect.top = startPoint.y;
                previewRect.bottom = currentY;
            }
            else {
                previewRect.top = currentY;
                previewRect.bottom = startPoint.y;
            }

            HDC hdc = GetDC(hWnd);
            DrawFocusRect(hdc, &previewRect);
            ReleaseDC(hWnd, hdc);
        }
    }
    break;
    case WM_LBUTTONUP:
    {
        ReleaseCapture();

        int endX = LOWORD(lp);
        int endY = HIWORD(lp);
        RECT finalRect;

        if (endX > startPoint.x) {
            finalRect.left = startPoint.x;
            finalRect.right = endX;
        }
        else {
            finalRect.left = endX;
            finalRect.right = startPoint.x;
        }

        if (endY > startPoint.y) {
            finalRect.top = startPoint.y;
            finalRect.bottom = endY;
        }
        else {
            finalRect.top = endY;
            finalRect.bottom = startPoint.y;
        }

        if ((finalRect.right - finalRect.left) < 10 || (finalRect.bottom - finalRect.top) < 10) {
            MessageBox(hWnd, TEXT("Min 10õ10"), TEXT("WARNING"), MB_OK | MB_ICONWARNING);
            return TRUE;
        }

        rectCount++;
        TCHAR str[10];
        wsprintf(str, TEXT("%d"), rectCount);
        HWND mystatic = CreateWindowEx(0, TEXT("STATIC"), str,
            WS_CHILD | WS_VISIBLE | SS_CENTER | WS_BORDER | WS_EX_CLIENTEDGE,
            finalRect.left, finalRect.top,
            finalRect.right - finalRect.left,
            finalRect.bottom - finalRect.top,
            hWnd, 0, hInst1, 0);

        statics[rectCount] = mystatic;
    }
    break;
    }
    return FALSE;
}
