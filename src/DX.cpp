#include "DX.hpp"
#include <Console.hpp>
DX::DX() : cbData({}), rotationAngle(0.0f), geo({})
{
}
DX::~DX()
{
    descriptorManager.ShutDown();
}
void DX::Init()
{
    LoadPipeline();
    LoadAssets();
}
void DX::Render()
{

    Update();
    PopulateCommandList();
    ID3D12CommandList* ppCommandList[] = {commandList.Get()};
    commandQueue->ExecuteCommandLists(_countof(ppCommandList), ppCommandList);
    ThrowIfFailed(swapChain->Present(1, 0));
    WaitForPreviousFrame();
#ifndef NDEBUG
    FlushDebugMessages();
#endif
}
