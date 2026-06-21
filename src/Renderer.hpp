#pragma once 
#include "DX.hpp"
class Renderer
{
public:
	Renderer();
	~Renderer();
	void Run();
private:
    DX dx;
	void Init();
	void MainLoop();
};