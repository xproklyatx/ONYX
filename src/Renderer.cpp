#include "Win32Platform.hpp"
#include "Renderer.hpp"
Renderer::Renderer()
{
}
Renderer::~Renderer()
{
}
void Renderer::Run()
{
    Init();
    MainLoop();
}
void Renderer::Init()
{
    dx.Init();
}
void Renderer::MainLoop()
{
    Win32Platform& win32Instance = Win32Platform::GetInstance();
    while (!win32Instance.CheckClose())
    {
        win32Instance.PollEvents();
        dx.Render();
    }
}
