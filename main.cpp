#include <iostream>

#include <windows.h>

#include <vector>

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static char* frameData;
static BITMAPINFO bitmapInfo;
static HBITMAP bitmapHandle;
static HDC frameDeviceContext;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, PSTR cmdLine, int cmdShow) {
    const char ClassName[] = "Software Renderer";
    WNDCLASS windowClass{};

    windowClass.lpfnWndProc   = windowProc;
    windowClass.hInstance     = instance;
    windowClass.lpszClassName = ClassName;

    RegisterClass(&windowClass);

    bitmapInfo.bmiHeader.biSize        = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biPlanes      = 1;
    bitmapInfo.bmiHeader.biBitCount    = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    frameDeviceContext                 = CreateCompatibleDC(0);

    HWND windowHandle = CreateWindowExA(
            0,
            ClassName,
            "Software Renderer",
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            1280,
            720,
            nullptr,
            nullptr,
            instance,
            nullptr
    );

    if (windowHandle == nullptr) {
        return 0;
    }

    ShowWindow(windowHandle, cmdShow);

    frameData = new char[1280 * 720 * 4];
    for (int i = 0; i < 720; i++) {
        for (int j = 0; j < 1280; j++) {
            int index            = i * 1280 + j * 4;
            frameData[index + 1] = i;
            frameData[index + 2] = j;
            frameData[index + 3] = 32;
        }
    }

    MSG msg{};
    while (GetMessageA(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);

        //for (int i = 0; i < 720; i++) {
        //    for (int j = 0; j < 1280; j++) {
        //        int index            = i * 1280 + j * 4;
        //        frameData[index + 1] = i;
        //        frameData[index + 2] = j;
        //        frameData[index + 3] = 32;
        //    }
        //}

        //InvalidateRect(windowHandle, nullptr, FALSE);
        //UpdateWindow(windowHandle);
    }

    return 0;
}

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        static PAINTSTRUCT paint;
        static HDC deviceContext;
        deviceContext = BeginPaint(hwnd, &paint);
        BitBlt(deviceContext,
               paint.rcPaint.left,
               paint.rcPaint.top,
               paint.rcPaint.right - paint.rcPaint.left,
               paint.rcPaint.bottom - paint.rcPaint.top,
               frameDeviceContext,
               paint.rcPaint.left,
               paint.rcPaint.top,
               SRCCOPY);
        return 0;
    case WM_SIZE:
        bitmapInfo.bmiHeader.biWidth  = LOWORD(lParam);
        bitmapInfo.bmiHeader.biHeight = HIWORD(lParam);

        if (bitmapHandle)
            DeleteObject(bitmapHandle);
        bitmapHandle =
                CreateDIBSection(nullptr, &bitmapInfo, DIB_RGB_COLORS,
               (void**)&frameData, 0, 0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}