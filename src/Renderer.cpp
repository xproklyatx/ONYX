#include "Win32Platform.hpp"
#include "Renderer.hpp"
#include "DeltaTime.hpp"
#include "Input.hpp"
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

    Input::Initialize();
    ICam::Set(&camera);
    dx.Init();
}
void Renderer::MainLoop()
{
    Win32Platform& win32Instance = Win32Platform::GetInstance();
    while (!win32Instance.CheckClose())
    {
        DeltaTime::Update();
        win32Instance.PollEvents();
        camera.Update();
        dx.Render();
    }
}
