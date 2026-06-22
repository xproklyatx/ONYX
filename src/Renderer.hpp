#pragma once
#include "DX.hpp"
#include "Camera6DOF.hpp"
class Renderer
{
  public:
    Renderer();
    ~Renderer();
    void Run();

  private:
    DX dx;
    Camera6DOF camera;
    void Init();
    void MainLoop();
};
