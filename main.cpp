#include <iostream>

#include <windows.h>

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, PSTR cmdLine, int cmdShow) {
    const char ClassName[] = "Software Renderer";
    WNDCLASS windowClass{};

    windowClass.lpfnWndProc   = windowProc;
    windowClass.hInstance     = instance;
    windowClass.lpszClassName = ClassName;

    RegisterClass(&windowClass);

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

    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}