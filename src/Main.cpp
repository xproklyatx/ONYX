#include "Win32Platform.hpp"
#include "Renderer.hpp"
#include "Console.hpp"
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
#ifndef NDEBUG
	Console::Init();
#endif 
	try
	{
		Win32Platform::GetInstance().InitWindow(hInstance, nCmdShow);
        Renderer renderer;
		renderer.Run();
	}
	catch (const std::exception& e)
	{
		std::cerr << "ERROR: " << e.what() << std::endl;
#ifndef NDEBUG
			Console::Destroy();
#endif 
			return EXIT_FAILURE;
	}
#ifndef NDEBUG
	Console::Destroy();
#endif 
	return EXIT_SUCCESS;
}