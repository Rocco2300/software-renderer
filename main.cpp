#include <cinttypes>
#include <windows.h>

#include "math/vec.hpp"

static bool quit = false;

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

static int frameWidth{};
static int frameHeight{};
static uint32_t* frameData;
static BITMAPINFO bitmapInfo;
static HBITMAP bitmap;
static HDC frameDeviceContext;

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, PSTR cmdLine, int cmdShow) {
    const char window_class_name[] = "My Window Class";
    static WNDCLASS window_class   = {0};
    window_class.lpfnWndProc       = windowProc;
    window_class.hInstance         = instance;
    window_class.lpszClassName     = window_class_name;
    RegisterClass(&window_class);

    bitmapInfo.bmiHeader.biSize        = sizeof(bitmapInfo.bmiHeader);
    bitmapInfo.bmiHeader.biPlanes      = 1;
    bitmapInfo.bmiHeader.biBitCount    = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    frameDeviceContext                 = CreateCompatibleDC(0);

    static HWND window_handle;
    window_handle = CreateWindow(
            window_class_name,
            reinterpret_cast<LPCSTR>(L"Drawing Pixels"),
            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
            640,
            300,
            1280,
            720,
            nullptr,
            nullptr,
            instance,
            nullptr
    );
    if (window_handle == nullptr) {
        return -1;
    }

    vec2 a{2.f, 3.f};
    vec2 b{4.1f, 3.2f};
    auto add = a + b;
    auto sub = a - b;
    auto mul1 = 2.3f * a;
    auto mul2 = a * 2.3f;
    auto div = a / 2.f;
    auto dot = vec::dot(a, b);
    auto len = vec::length(a);
    auto norm = vec::normalize(a);

    while (!quit) {
        static MSG message = {0};
        while (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) { DispatchMessage(&message); }

        for (int i = 0; i < frameHeight; i++) {
            for (int j = 0; j < frameWidth; j++) {
                uint32_t pixel = 0;
                pixel |= ((int) (j / (float) frameWidth * 255) << 16);
                pixel |= ((int) (i / (float) frameHeight * 255) << 8);
                pixel |= 25;
                frameData[i * frameWidth + j] = pixel;
            }
        }

        InvalidateRect(window_handle, nullptr, FALSE);
        UpdateWindow(window_handle);
    }

    return 0;
}

LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_QUIT:
    case WM_DESTROY: {
        quit = true;
    } break;

    case WM_PAINT: {
        static PAINTSTRUCT paint;
        static HDC device_context;
        device_context = BeginPaint(hwnd, &paint);
        BitBlt(device_context,
               paint.rcPaint.left,
               paint.rcPaint.top,
               paint.rcPaint.right - paint.rcPaint.left,
               paint.rcPaint.bottom - paint.rcPaint.top,
               frameDeviceContext,
               paint.rcPaint.left,
               paint.rcPaint.top,
               SRCCOPY);
        EndPaint(hwnd, &paint);
    } break;

    case WM_SIZE: {
        bitmapInfo.bmiHeader.biWidth  = LOWORD(lParam);
        bitmapInfo.bmiHeader.biHeight = HIWORD(lParam);

        if (bitmap) {
            DeleteObject(bitmap);
        }
        bitmap = CreateDIBSection(nullptr, &bitmapInfo, DIB_RGB_COLORS, (void**) &frameData, 0, 0);
        SelectObject(frameDeviceContext, bitmap);

        frameWidth  = LOWORD(lParam);
        frameHeight = HIWORD(lParam);
    } break;

    default: {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    }
    return 0;
}