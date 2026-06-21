#include "DX.hpp"
#include <Console.hpp>
#include <sstream>
DX::DX()
{

}
DX::~DX()
{

}
void DX::Init()
{
	LoadPipeline();
	LoadAssets();
}
void DX::Render()
{
	PopulateCommandList();
	ID3D12CommandList* ppCommandList[] = { commandList.Get() };
	commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
	ThrowIfFailed(swapChain->Present(1, 0));
	WaitForPreviousFrame();
#ifndef NDEBUG
	FlushDebugMessages();
#endif
	Update();
}
void DX::ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
#ifndef NDEBUG
		std::ostringstream oss;
		oss << "HRESULT Failed: 0x" << std::hex << std::uppercase << hr << "\n";
		PRINT_ERROR(oss.str().c_str());
#endif
		throw std::exception();
	}
}