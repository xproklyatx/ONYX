#include "Win32Platform.hpp"
#include "Renderer.hpp"
#include "DeltaTime.hpp"
#include "Input.hpp"
#include "Audio.hpp"
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
    Audio::Initialize();
    Audio::AddWav("res/music/onyx.wav", "onyx");
    Audio::PlaySound("onyx", true);
    Input::Initialize();
    ICam::Set(&camera);
    camera.SetFocus(true);
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
        Audio::Update();
    }
}
