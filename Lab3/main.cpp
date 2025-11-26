// Test.cpp
//
#define STRICT
#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers

#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include "resource1.h"

#define SCLASS "a;eohgqeruiopugoqeig"
#define WINDOW_WIDTH 0.5
#define WINDOW_HEIGHT 0.5

#define ELEMENTS 1000
#define FIGURE_WIDTH 3000
#define FIGURE_HEIGHT 2000
#define ELEMENTS_RADIUS 40
#define STEP 50

struct Figure {
    int figureArrayX[ELEMENTS];
    int figureArrayY[ELEMENTS];
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI  WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE, // hPrevInstance,
    _In_ LPSTR, // lpCmdLine,
    _In_ int nCmdShow
)
{
    LPCTSTR szClass = SCLASS;

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = szClass;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    ::RegisterClass(&wc);

    double screenWidth = GetSystemMetrics(SM_CXSCREEN);
    double screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Create Main window
    HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));
    HWND overlappedWindow = ::CreateWindow(szClass, "MainWindow",
        WS_OVERLAPPEDWINDOW | WS_HSCROLL | WS_VSCROLL,
        (int)((screenWidth - (screenWidth * WINDOW_WIDTH)) / 2),
        (int)((screenHeight - (screenHeight * WINDOW_HEIGHT)) / 2),
        (int)(screenWidth * WINDOW_WIDTH), (int)(screenHeight * WINDOW_HEIGHT),
        NULL, hMenu, hInstance, NULL);
    if (overlappedWindow == NULL) {
        return -1;
    }

    ::ShowWindow(overlappedWindow, nCmdShow);

    Figure fg;
    srand((int)time(NULL));
    for (int i = 0; i < ELEMENTS; i++)
    {
        fg.figureArrayX[i] = rand() % FIGURE_WIDTH;
        fg.figureArrayY[i] = rand() % FIGURE_HEIGHT;
    }
    SetWindowLongPtr(overlappedWindow, GWLP_USERDATA, (LONG_PTR)&fg);

    MSG msg;
    while (::GetMessage(&msg, NULL, 0, 0)) { // idle
        ::DispatchMessage(&msg);
    }

    return 0;
}
//=========================================================

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
    case WM_COMMAND: {
        switch (LOWORD(wParam)) {
        case ID_FILE_EXIT: {
            DestroyWindow(hWnd);
            return 0;
        }
            
        case ID_FIGURES_SQUARE: {
            CheckMenuItem(GetMenu(hWnd), ID_FIGURES_SQUARE, MF_CHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_FIGURES_CIRCLE, MF_UNCHECKED);
            InvalidateRect(hWnd, NULL, TRUE);
            return 0;
        }
            
        case ID_FIGURES_CIRCLE: {
            CheckMenuItem(GetMenu(hWnd), ID_FIGURES_SQUARE, MF_UNCHECKED);
            CheckMenuItem(GetMenu(hWnd), ID_FIGURES_CIRCLE, MF_CHECKED);
            InvalidateRect(hWnd, NULL, TRUE);
            return 0;
        }
            
        }
        return 0;
    }
        
    case WM_CREATE: {
        CheckMenuItem(GetMenu(hWnd), ID_FIGURES_SQUARE, MF_CHECKED);
        CheckMenuItem(GetMenu(hWnd), ID_FIGURES_CIRCLE, MF_UNCHECKED);
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }
        
    case WM_SIZE: {
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        SCROLLINFO vScroll = { 0 };
        vScroll.cbSize = sizeof(SCROLLINFO);
        vScroll.fMask = SIF_RANGE;
        vScroll.nMin = 0;
        vScroll.nMax = FIGURE_HEIGHT - (clientRect.bottom - clientRect.top);
        SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);

        SCROLLINFO hScroll = { 0 };
        hScroll.cbSize = sizeof(SCROLLINFO);
        hScroll.fMask = SIF_RANGE;
        hScroll.nMin = 0;
        hScroll.nMax = FIGURE_WIDTH - (clientRect.right - clientRect.left);
        SetScrollInfo(hWnd, SB_HORZ, &hScroll, TRUE);

        InvalidateRect(hWnd, NULL, true);
        return 0;
    }

    case WM_PAINT:{
        SCROLLINFO hScroll = { 0 };
        hScroll.cbSize = sizeof(SCROLLINFO);
        hScroll.fMask = SIF_POS;
        GetScrollInfo(hWnd, SB_HORZ, &hScroll);

        SCROLLINFO vScroll = { 0 };
        vScroll.cbSize = sizeof(SCROLLINFO);
        vScroll.fMask = SIF_POS;
        GetScrollInfo(hWnd, SB_VERT, &vScroll);


        PAINTSTRUCT ps = { 0 };
        HDC hdc = BeginPaint(hWnd, &ps);
        HBRUSH pinkBrush = CreateSolidBrush(RGB(205, 0, 116));
        HBRUSH redBrush = CreateSolidBrush(RGB(255, 0, 0));
        HGDIOBJ oldBrush = SelectObject(hdc, pinkBrush);



        Figure* figure = (Figure*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
        for (int i = 0; i < ELEMENTS; i++) {
            if (i % 2 == 0) {
                oldBrush = SelectObject(hdc, pinkBrush);
            }
            else {
                oldBrush = SelectObject(hdc, redBrush);
            }

            RECT figureCoordinates = {
            figureCoordinates.left = figure->figureArrayX[i] - ELEMENTS_RADIUS - hScroll.nPos,
            figureCoordinates.top = figure->figureArrayY[i] - ELEMENTS_RADIUS - vScroll.nPos,
            figureCoordinates.right = figure->figureArrayX[i] + ELEMENTS_RADIUS - hScroll.nPos,
            figureCoordinates.bottom = figure->figureArrayY[i] + ELEMENTS_RADIUS - vScroll.nPos,
            };

            RECT rc;
            if (IntersectRect(&rc, &ps.rcPaint, &figureCoordinates)) {
                HMENU hMenu = GetMenu(hWnd);
                if (GetMenuState(hMenu, ID_FIGURES_SQUARE, MF_BYCOMMAND) == MF_CHECKED) {
                    Rectangle(hdc, figureCoordinates.left, figureCoordinates.top, figureCoordinates.right, figureCoordinates.bottom);
                }
                else if (GetMenuState(hMenu, ID_FIGURES_CIRCLE, MF_BYCOMMAND) == MF_CHECKED) {
                    Ellipse(hdc, figureCoordinates.left, figureCoordinates.top, figureCoordinates.right, figureCoordinates.bottom);
                }
            }
        }

        DeleteObject(pinkBrush);
        DeleteObject(redBrush);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_VSCROLL: {
        SCROLLINFO vScroll = { 0 };
        vScroll.cbSize = sizeof(SCROLLINFO);
        vScroll.fMask = SIF_RANGE |
            SIF_POS |
            SIF_TRACKPOS;
        int shift = 0;
        int step = STEP;
        int lastPos = vScroll.nPos;
        RECT rect;

        GetClientRect(hWnd, &rect);
        GetScrollInfo(hWnd, SBS_VERT, &vScroll);

        switch (LOWORD(wParam)) {
        case SB_LINEDOWN:
        case SB_PAGEDOWN: {
            shift = vScroll.nMax - vScroll.nPos;
            if (!shift) {
                return 0;
            }
            if (shift > step) {
                shift = step;
            }
            vScroll.nPos += shift;
            break;
        }
        case SB_LINEUP:
        case SB_PAGEUP: {
            shift = vScroll.nPos;
            if (!shift) {
                return 0;
            }
            if (shift > step) {
                shift = step;
            }
            shift = -shift;
            vScroll.nPos += shift;
            break;
        }
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK: {
            shift = vScroll.nTrackPos - vScroll.nPos;
            vScroll.nPos = vScroll.nTrackPos;
            break;
        }
        }

        SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);
        if (shift > 0) {
            rect.top = rect.bottom - shift;
        }
        else {
            rect.bottom = rect.top - shift;
        }
        // ≈сли позици€ изменилась, прокручиваем окно
        if (vScroll.nPos != lastPos) {
            ScrollWindowEx(hWnd, 0, -shift, NULL, NULL, NULL, NULL, SW_ERASE);
        }
        InvalidateRect(hWnd, &rect, TRUE);
        return 0;
    }

    case WM_HSCROLL: {
        SCROLLINFO hScroll = { 0 };
        hScroll.cbSize = sizeof(SCROLLINFO);
        hScroll.fMask = SIF_RANGE | SIF_POS | SIF_TRACKPOS;
        int shift = 0;
        int step = STEP;
        int lastPos = hScroll.nPos;
        RECT rect = { 0 };

        GetClientRect(hWnd, &rect);
        GetScrollInfo(hWnd, SBS_HORZ, &hScroll);

        switch (LOWORD(wParam)) {
        case SB_LINELEFT: 
        case SB_PAGELEFT: { 
            shift = hScroll.nPos;
            if (!shift) {
                return 0;
            }
            if (shift > step) {
                shift = step;
            }
            hScroll.nPos -= shift;
            break;
        }
        case SB_LINERIGHT:
        case SB_PAGERIGHT: {
            shift = hScroll.nMax - hScroll.nPos;
            if (!shift) {
                return 0;
            }
            if (shift > step) {
                shift = step;
            }
            hScroll.nPos += shift;
            break;
        }
        case SB_THUMBPOSITION:
        case SB_THUMBTRACK: {
            shift = hScroll.nTrackPos - hScroll.nPos;
            hScroll.nPos = hScroll.nTrackPos;
            break;
        }
        }

        SetScrollInfo(hWnd, SB_HORZ, &hScroll, TRUE);
        if (shift > 0) {
            rect.left = rect.right - shift;
        }
        else {
            rect.right = rect.left - shift;
        }

        if (hScroll.nPos != lastPos) {
            ScrollWindowEx(hWnd, -shift, 0, NULL, NULL, NULL, NULL, SW_ERASE);
            UpdateWindow(hWnd);
        }
        InvalidateRect(hWnd, &rect, TRUE);
        return 0;
    }

    case WM_MOUSEWHEEL: {
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        SCROLLINFO vScroll = { 0 };
        vScroll.cbSize = sizeof(SCROLLINFO);
        vScroll.fMask = SIF_RANGE | SIF_POS | SIF_TRACKPOS;
        int shift = STEP;
        GetScrollInfo(hWnd, SB_VERT, &vScroll);

        if (zDelta > 0) {
            shift = vScroll.nPos;
            if (!shift) {
                return 0;
            }
            if (shift > STEP) {
                shift = STEP;
            }
            vScroll.nPos -= shift;
        }
        else {
            shift = vScroll.nMax - vScroll.nPos;
            if (!shift) {
                return 0;
            }
            if (shift > STEP) {
                shift = STEP;
            }
            vScroll.nPos += shift;
        }
        SetScrollPos(hWnd, SB_VERT, vScroll.nPos, TRUE);
        ScrollWindowEx(hWnd, 0, shift, NULL, NULL, NULL, NULL, SW_ERASE);
        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }

    case WM_KEYDOWN: {
        SCROLLINFO hScroll, vScroll;

        hScroll.cbSize = sizeof(SCROLLINFO);
        hScroll.fMask = SIF_RANGE |
            SIF_POS |
            SIF_TRACKPOS;
        vScroll.cbSize = sizeof(SCROLLINFO);
        vScroll.fMask = SIF_RANGE |
            SIF_POS |
            SIF_TRACKPOS;

        GetScrollInfo(hWnd, SB_HORZ, &hScroll);
        GetScrollInfo(hWnd, SB_VERT, &vScroll);

        int shift = 0;
        int step = STEP;

        switch (wParam) {

        case VK_UP: {
            shift = vScroll.nPos;
            if (!shift) {
                return 0;
            }
            if (shift > step) {
                shift = step;
            }
            vScroll.nPos -= shift;
            break;
        }
        case VK_DOWN: {
            shift = vScroll.nMax - vScroll.nPos;
            if (!shift) {
                return 0;
            }
            if (shift > step) {
                shift = step;
            }
            vScroll.nPos += shift;
            break;
        }
        case VK_NEXT: {
            shift = vScroll.nMax;
            vScroll.nPos = shift;
            break;
        }
        case VK_PRIOR: {
            shift = vScroll.nMin;
            vScroll.nPos = shift;
            break;
        }
        case VK_RIGHT: {
            shift = hScroll.nMax - hScroll.nPos;
            if (!shift) {
                return 0;
            }
            if (shift > step) {
                shift = step;
            }
            hScroll.nPos += shift;
            break;
        }
        case VK_LEFT: {
            shift = hScroll.nPos;
            if (!shift) {
                return 0;
            }
            if (shift > step) {
                shift = step;
            }
            hScroll.nPos -= shift;
            break;
        }
        case VK_HOME: {
            shift = hScroll.nMin;
            hScroll.nPos = shift;
            break;
        }
        case VK_END: {
            shift = hScroll.nMax;
            if (!shift) {
                return 0;
            }
            hScroll.nPos = shift;
            break;
        }
        case VK_ESCAPE: {
            DestroyWindow(hWnd);
            return 0;
        }
        }

        if ((wParam == VK_RIGHT) || (wParam == VK_LEFT || wParam == VK_HOME || wParam == VK_END)) {
            ScrollWindowEx(hWnd, shift, 0, NULL, NULL, NULL, NULL, SW_ERASE);
            SetScrollInfo(hWnd, SB_HORZ, &hScroll, TRUE);
        }
        else {
            ScrollWindowEx(hWnd, 0, shift, NULL, NULL, NULL, NULL, SW_ERASE);
            SetScrollInfo(hWnd, SB_VERT, &vScroll, TRUE);
        }

        InvalidateRect(hWnd, NULL, TRUE);
        return 0;
    }
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }

    return ::DefWindowProc(hWnd, message, wParam, lParam);
}
//=========================================================
