#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
constexpr uint32_t SCR_WIDTH = 800;
constexpr uint32_t SCR_HEIGHT = 600;
constexpr const char* SCR_TITLE = "ONYX";
constexpr const char* CLASS_NAME = "ONYX";
class Win32Platform
{
  public:
    Win32Platform(const Win32Platform&) = delete;
    Win32Platform& operator=(const Win32Platform&) = delete;
    Win32Platform(Win32Platform&&) = delete;
    Win32Platform& operator=(Win32Platform&&) = delete;
    static Win32Platform& GetInstance();
    void InitWindow(HINSTANCE hPrevInstance, int nCmdShow);
    void PollEvents();
    HWND GetHWND();
    bool CheckClose();
    void CloseWindow();
    uint32_t GetWindowWidth();
    uint32_t GetWindowHeight();
    using ResizeCallback = void (*)(void* userdata, uint32_t width, uint32_t height);
    void SetResizeCallback(ResizeCallback callback, void* userdata);

  private:
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM);
    Win32Platform();
    ~Win32Platform();
    HWND hWnd;
    MSG msg;
    static uint32_t width;
    static uint32_t height;
    bool windowClose;
    static ResizeCallback onResize;
    static void* callbackUserdata;
};
