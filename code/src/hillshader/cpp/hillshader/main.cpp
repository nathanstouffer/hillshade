#include <memory>

#define NOMINMAX
#include <Windows.h>
#include <crtdbg.h>

#include "hillshader/application.hpp"
#include "hillshader/camera/config.hpp"
#include "hillshader/camera/controllers/animators/animator.hpp"
#include "hillshader/camera/controllers/animators/orbit.hpp"
#include "hillshader/camera/controllers/animators/zoom.hpp"

static std::unique_ptr<hillshader::application> s_app = nullptr;

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
            return 0;

        case WM_LBUTTONUP:
            ReleaseCapture();
            s_app->io().AddMouseButtonEvent(0, false);
            return 0;

        case WM_RBUTTONDOWN:
            SetCapture(wnd);
            s_app->io().AddMouseButtonEvent(1, true);
            return 0;

        case WM_RBUTTONUP:
            ReleaseCapture();
            s_app->io().AddMouseButtonEvent(1, false);
            return 0;

        case 0x020A:    // TODO figure out why we need this (it seems to be an older event code for WM_MOUSEWHEEL)
        case WM_MOUSEHWHEEL:
            s_app->io().AddMouseWheelEvent(0.f, GET_WHEEL_DELTA_WPARAM(w_param));
            return 0;

        case WM_SIZE: // window size has been changed
            if (s_app) { s_app->resize(LOWORD(l_param), HIWORD(l_param)); }
            return 0;

        case WM_KEYDOWN:
            if (s_app->io().WantCaptureKeyboard)
            {
                switch (w_param)
                {
                    case VK_BACK:   s_app->io().AddKeyEvent(ImGuiKey_Backspace,  true); break;
                    case VK_DELETE: s_app->io().AddKeyEvent(ImGuiKey_Delete,     true); break;
                    case VK_SPACE:  s_app->io().AddKeyEvent(ImGuiKey_Space,      true); break;
                    case VK_ESCAPE: s_app->io().AddKeyEvent(ImGuiKey_Escape,     true); break;
                    case VK_RETURN: s_app->io().AddKeyEvent(ImGuiKey_Enter,      true); break;
                    case VK_LEFT:   s_app->io().AddKeyEvent(ImGuiKey_LeftArrow,  true); break;
                    case VK_RIGHT:  s_app->io().AddKeyEvent(ImGuiKey_RightArrow, true); break;
                    case VK_UP:     s_app->io().AddKeyEvent(ImGuiKey_UpArrow,    true); break;
                    case VK_DOWN:   s_app->io().AddKeyEvent(ImGuiKey_DownArrow,  true); break;
                }
            }
            return 0;

        case WM_KEYUP:
            if (s_app->io().WantCaptureKeyboard)
            {
                switch (w_param)
                {
                    case VK_BACK:   s_app->io().AddKeyEvent(ImGuiKey_Backspace,  false); break;
                    case VK_DELETE: s_app->io().AddKeyEvent(ImGuiKey_Delete,     false); break;
                    case VK_SPACE:  s_app->io().AddKeyEvent(ImGuiKey_Space,      false); break;
                    case VK_ESCAPE: s_app->io().AddKeyEvent(ImGuiKey_Escape,     false); break;
                    case VK_RETURN: s_app->io().AddKeyEvent(ImGuiKey_Enter,      false); break;
                    case VK_LEFT:   s_app->io().AddKeyEvent(ImGuiKey_LeftArrow,  false); break;
                    case VK_RIGHT:  s_app->io().AddKeyEvent(ImGuiKey_RightArrow, false); break;
                    case VK_UP:     s_app->io().AddKeyEvent(ImGuiKey_UpArrow,    false); break;
                    case VK_DOWN:   s_app->io().AddKeyEvent(ImGuiKey_DownArrow,  false); break;
                }
            }
            return 0;

        case WM_CHAR:
            if (!s_app->io().WantCaptureKeyboard)
            {
                if (w_param == VK_ESCAPE) { PostQuitMessage(0); }
                if (w_param == L'q') { PostQuitMessage(0); }
                // hide/show ui
                if (w_param == L'U' || w_param == L'u') { s_app->toggle_ui(); }
                // zooming in
                if (w_param == L'+') { s_app->zoom(hillshader::camera::config::c_zoom_factor, hillshader::focus::center); }
                if (w_param == L'=') { s_app->zoom(hillshader::camera::config::c_zoom_factor, hillshader::focus::center); }
                // zooming out
                if (w_param == L'-') { s_app->zoom(1.f / hillshader::camera::config::c_zoom_factor, hillshader::focus::center); }
                if (w_param == L'_') { s_app->zoom(1.f / hillshader::camera::config::c_zoom_factor, hillshader::focus::center); }
                // wasd movement
                if (w_param == L'w') { s_app->orbit(0.f, hillshader::camera::config::c_delta_phi,    hillshader::focus::center); }
                if (w_param == L'a') { s_app->orbit(-hillshader::camera::config::c_delta_theta, 0.f, hillshader::focus::center); }
                if (w_param == L's') { s_app->orbit(0.f, -hillshader::camera::config::c_delta_phi,   hillshader::focus::center); }
                if (w_param == L'd') { s_app->orbit(hillshader::camera::config::c_delta_theta, 0.f,  hillshader::focus::center); }
                // north up
                if (w_param == L'n') { s_app->orbit_to(stff::constants::pi_halves, stff::constants::pi, hillshader::focus::center); }
                // orbit attract
                if (w_param == L'o') { s_app->orbit_attract(3.f * stff::constants::pi / 4.f, -stff::constants::two_pi / 30'000.0f, hillshader::focus::center); }
                // reset camera
                if (w_param == L'r') { s_app->reset_camera(); }
                // capture frame
                if (w_param == L'c') { s_app->capture(); }
                // record orbit
                if (w_param == L'R') { s_app->record_orbit_attract(3.f * stff::constants::pi / 4.f, -stff::constants::two_pi / 30'000.0f, hillshader::focus::center); }
            }
            else
            {
                s_app->io().AddInputCharacter(static_cast<unsigned int>(w_param));
            }
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
    LONG window_height = 720;
    RECT rct = { 0, 0, window_width, window_height };
    AdjustWindowRect(&rct, WS_OVERLAPPEDWINDOW, FALSE);
    //AdjustWindowRect(&rct, WS_POPUP, FALSE);  // use to remove top bar
    HWND wnd = CreateWindow("Hillshade", "Hillshader", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rct.right - rct.left, rct.bottom - rct.top, NULL, NULL, hInstance, NULL);
    if (!wnd)
    {
        MessageBox(NULL, "Cannot create window", "Error", MB_OK | MB_ICONERROR);
        return 0;
    }
    ShowWindow(wnd, nShowCmd);
    UpdateWindow(wnd);

    s_app = std::make_unique<hillshader::application>();
    if (!s_app->initialize(wnd))
        return -1;

    s_app->resize(window_width, window_height);

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