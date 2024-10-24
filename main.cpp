#include <iostream>

#if RAND_MAX == 32767
#define Rand32() ((rand() << 16) + (rand() << 1) + (rand() & 1))
#else
#define Rand32() rand()
#endif

#include <cinttypes>
#include <windows.h>

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
            NULL,
            NULL,
            instance,
            NULL
    );
    if (window_handle == NULL) {
        return -1;
    }

    //frameData = new uint32_t[1280 * 720];
    while (!quit) {
        static MSG message = {0};
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE)) { DispatchMessage(&message); }

        static unsigned int p              = 0;
        frameData[(p++) % (frameWidth * frameHeight)]    = Rand32();
        frameData[Rand32() % (frameWidth * frameHeight)] = 0;

        InvalidateRect(window_handle, NULL, FALSE);
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

        if (bitmap)
            DeleteObject(bitmap);
        bitmap = CreateDIBSection(NULL, &bitmapInfo, DIB_RGB_COLORS, (void**) &frameData, 0, 0);
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