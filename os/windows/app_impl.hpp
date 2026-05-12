#pragma once
#include "../../../app.hpp"
#include "start/assets.hpp"
#include <windows.h>
#include <wil/com.h>
#include <WebView2.h>
#include <wrl.h>
#include <string>
#include <memory>

using namespace Microsoft::WRL;

namespace start::windows
{
  class WindowsApp : public start::App
  {
  public:
    WindowsApp(HINSTANCE instance, std::string title, int width, int height)
        : instanceHandle(instance), appTitle(title),
          windowWidth(width), windowHeight(height) {}

    int run() override
    {
      this->on_launch();

      if (!initMainWindow())
        return 0;
      initWebView();

      MSG msg;
      while (GetMessage(&msg, NULL, 0, 0))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
      }

      this->on_close();
      return static_cast<int>(msg.wParam);
    }

  protected:
    wil::com_ptr<ICoreWebView2> webview;

  private:
    HINSTANCE instanceHandle;
    HWND mainWindow{nullptr};
    std::string appTitle;
    int windowWidth;
    int windowHeight;
    wil::com_ptr<ICoreWebView2Controller> webviewController;

    auto initMainWindow() -> bool
    {
      std::wstring wideTitle(appTitle.begin(), appTitle.end());
      WNDCLASSEXW wcex = {sizeof(WNDCLASSEXW)};
      wcex.style = CS_HREDRAW | CS_VREDRAW;
      wcex.lpfnWndProc = staticWndProc;
      wcex.hInstance = instanceHandle;
      wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
      wcex.lpszClassName = L"StartWindowClass"; // Start!
      RegisterClassExW(&wcex);

      mainWindow = CreateWindowExW(
          0, L"StartWindowClass", wideTitle.c_str(), // Start!
          WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
          windowWidth, windowHeight,
          NULL, NULL, instanceHandle, this);

      if (!mainWindow)
        return false;
      ShowWindow(mainWindow, SW_SHOW);
      UpdateWindow(mainWindow);
      return true;
    }

    auto initWebView() -> void
    {
      CreateCoreWebView2EnvironmentWithOptions(nullptr, nullptr, nullptr,
                                               Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
                                                   [this](HRESULT result, ICoreWebView2Environment *env) -> HRESULT
                                                   {
                                                     env->CreateCoreWebView2Controller(mainWindow,
                                                                                       Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
                                                                                           [this](HRESULT res, ICoreWebView2Controller *controller) -> HRESULT
                                                                                           {
                                                                                             if (controller)
                                                                                             {
                                                                                               webviewController = controller;
                                                                                               webviewController->get_CoreWebView2(&webview);
                                                                                               RECT bounds;
                                                                                               GetClientRect(mainWindow, &bounds);
                                                                                               webviewController->put_Bounds(bounds);

                                                                                               // インライン化された index.html を読み込む
                                                                                               auto html = start::assets::all.at("index.html");
                                                                                               std::wstring wideHtml(html.begin(), html.end());
                                                                                               webview->NavigateToString(wideHtml.c_str());
                                                                                             }
                                                                                             return S_OK;
                                                                                           })
                                                                                           .Get());
                                                     return S_OK;
                                                   })
                                                   .Get());
    }

    static LRESULT CALLBACK staticWndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
    {
      WindowsApp *app = (msg == WM_NCCREATE)
                            ? reinterpret_cast<WindowsApp *>(reinterpret_cast<CREATESTRUCT *>(lp)->lpCreateParams)
                            : reinterpret_cast<WindowsApp *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
      if (msg == WM_NCCREATE)
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(app));
      return (app) ? app->handleMessage(hWnd, msg, wp, lp) : DefWindowProcW(hWnd, msg, wp, lp);
    }

    auto handleMessage(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp) -> LRESULT
    {
      if (msg == WM_SIZE && webviewController)
      {
        RECT bounds;
        GetClientRect(hWnd, &bounds);
        webviewController->put_Bounds(bounds);
      }
      else if (msg == WM_DESTROY)
      {
        PostQuitMessage(0);
      }
      return DefWindowProcW(hWnd, msg, wp, lp);
    }
  };
}