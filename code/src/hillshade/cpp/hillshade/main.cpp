#include <memory>

#define NOMINMAX
#include <Windows.h>
#include <crtdbg.h>

#include "hillshade/application.hpp"

static std::unique_ptr<hillshade::application> s_app = nullptr;

static float constexpr c_small_zoom_factor = 1.05f;
static float constexpr c_big_zoom_factor = 2.f;
static float constexpr c_small_pan_factor = 0.01f;
static float constexpr c_big_pan_factor = 0.1f;

// called every time the NativeNativeAppBase receives a message
LRESULT CALLBACK MessageProc(HWND wnd, UINT message, WPARAM w_param, LPARAM l_param)
{
    switch (message)
    {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            BeginPaint(wnd, &ps);
            EndPaint(wnd, &ps);
            return 0;
        }

        case WM_LBUTTONDOWN:
            SetCapture(wnd);
            s_app->io().AddMouseButtonEvent(0, true);
            s_app->update_focus();
            return 0;

        case WM_LBUTTONUP:
            ReleaseCapture();
            s_app->io().AddMouseButtonEvent(0, false);
            return 0;

        case WM_RBUTTONDOWN:
            SetCapture(wnd);
            s_app->io().AddMouseButtonEvent(1, true);
            s_app->update_focus();
            return 0;

        case WM_RBUTTONUP:
            ReleaseCapture();
            s_app->io().AddMouseButtonEvent(1, false);
            return 0;

        case 0x020A:    // TODO (stouff) figure out why we need this (it seems to be an older event code for WM_MOUSEWHEEL)
        case WM_MOUSEHWHEEL:
            s_app->io().AddMouseWheelEvent(0.f, GET_WHEEL_DELTA_WPARAM(w_param));
            s_app->update_focus();
            return 0;

        case WM_SIZE: // window size has been changed
            if (s_app) { s_app->resize(LOWORD(l_param), HIWORD(l_param)); }
            return 0;

        case WM_CHAR:
            if (w_param == VK_ESCAPE) { PostQuitMessage(0); }
            if (w_param == L'q') { PostQuitMessage(0); }
            // hide/show ui
            if (w_param == L'U' || w_param == L'u') { s_app->toggle_ui(); }
            // small zooming
            if (w_param == L'=') { s_app->zoom_in (c_small_zoom_factor); }
            if (w_param == L'-') { s_app->zoom_out(c_small_zoom_factor); }
            // big zooming
            if (w_param == L'+') { s_app->zoom_in (c_big_zoom_factor); }
            if (w_param == L'_') { s_app->zoom_out(c_big_zoom_factor); }
            // wasd movement (small)
            if (w_param == L'w') { s_app->pan(stff::vec2(               0.0f, c_small_pan_factor )); }
            if (w_param == L'a') { s_app->pan(stff::vec2(-c_small_pan_factor, 0.0f               )); }
            if (w_param == L's') { s_app->pan(stff::vec2(               0.0f, -c_small_pan_factor)); }
            if (w_param == L'd') { s_app->pan(stff::vec2( c_small_pan_factor, 0.0f               )); }
            // WASD movment (big)
            if (w_param == L'W') { s_app->pan(stff::vec2(0.0f,              c_big_pan_factor )); }
            if (w_param == L'A') { s_app->pan(stff::vec2(-c_big_pan_factor, 0.0f             )); }
            if (w_param == L'S') { s_app->pan(stff::vec2(0.0f,              -c_big_pan_factor)); }
            if (w_param == L'D') { s_app->pan(stff::vec2( c_big_pan_factor, 0.0f             )); }
            // reset camera
            if (w_param == L'r') { s_app->reset_camera(); }
            return 0;

        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_GETMINMAXINFO:
        {
            LPMINMAXINFO range = (LPMINMAXINFO)l_param;
            range->ptMinTrackSize.x = 320;
            range->ptMinTrackSize.y = 240;
            return 0;
        }

        default:
            return DefWindowProc(wnd, message, w_param, l_param);
    }
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
#if defined(_DEBUG) || defined(DEBUG)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

    // register window
    WNDCLASSEX wcex = { sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, MessageProc, 0L, 0L, hInstance, NULL, NULL, NULL, NULL, "Hillshade", NULL };
    RegisterClassEx(&wcex);

    // create a window
    LONG window_width = 1280;
    LONG window_height = 1024;
    RECT rct = { 0, 0, window_width, window_height };
    AdjustWindowRect(&rct, WS_OVERLAPPEDWINDOW, FALSE);
    HWND wnd = CreateWindow("Hillshade", "Hillshader", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rct.right - rct.left, rct.bottom - rct.top, NULL, NULL, hInstance, NULL);
    if (!wnd)
    {
        MessageBox(NULL, "Cannot create window", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    ShowWindow(wnd, nShowCmd);
    UpdateWindow(wnd);

    s_app = std::make_unique<hillshade::application>();
    if (!s_app->initialize(wnd))
        return -1;
    
    // Main message loop
    MSG msg = { 0 };
    while (WM_QUIT != msg.message)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            s_app->render();
            s_app->present();
        }
    }

    s_app.reset();

    return (int)msg.wParam;
}