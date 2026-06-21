#include "DX.hpp"
void DX::WaitForPreviousFrame()
{
	const UINT64 sFence = fenceValue;
	ThrowIfFailed(commandQueue->Signal(fence.Get(), sFence));
	fenceValue++;
	if (fence->GetCompletedValue() < sFence)
	{
		ThrowIfFailed(fence->SetEventOnCompletion(sFence, fenceEvent));
		WaitForSingleObject(fenceEvent, INFINITE);
	}
	frameIndex = swapChain->GetCurrentBackBufferIndex();
}