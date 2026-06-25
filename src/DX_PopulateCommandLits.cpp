#include "DX.hpp"
void DX::PopulateCommandList()
{
    ThrowIfFailed(commandList->Reset(commandAllocator.Get(), pipelineState.Get()));

    commandList->SetGraphicsRootSignature(rootSignature.Get());
    ID3D12DescriptorHeap* ppHeaps[] = {descriptorManager.GetHeap()};
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    descriptorManager.SetTable<ONYX_CUS_TYPE::CBV>(0, cbInfo.index);
    descriptorManager.SetTable<ONYX_CUS_TYPE::SRV>(1, 0);
    commandList->RSSetViewports(1, &viewPort);
    commandList->RSSetScissorRects(1, &scissorRect);

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &barrier);
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex,
                                            rtvDescriptorSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart());
    const float clearColor[] = {0.02f, 0.02f, 0.02f, 1.0f};
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    if (useDepthStencil)
    {
        commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
    }
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    geometryManager.Draw(&geo);
    CD3DX12_RESOURCE_BARRIER presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    commandList->ResourceBarrier(1, &presentBarrier);
    ThrowIfFailed(commandList->Close());
}
