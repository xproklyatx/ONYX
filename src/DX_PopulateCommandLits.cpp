#include "DX.hpp"
void DX::PopulateCommandList()
{
    ThrowIfFailed(commandList->Reset(commandAllocator.Get(), pipelineState.Get()));

    commandList->SetGraphicsRootSignature(rootSignature.Get());
    ID3D12DescriptorHeap* ppHeaps[] = {cusHeap.Get()};
    commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
    CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(cusHeap->GetGPUDescriptorHandleForHeapStart(),
                                            0, // 0
                                            cusSize);
    CD3DX12_GPU_DESCRIPTOR_HANDLE srvHandle(cusHeap->GetGPUDescriptorHandleForHeapStart(),
                                            1, // 1
                                            cusSize);
    commandList->SetGraphicsRootDescriptorTable(0, cbvHandle);
    commandList->SetGraphicsRootDescriptorTable(1, srvHandle);
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
    commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
    commandList->IASetIndexBuffer(&indexBufferView);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
    CD3DX12_RESOURCE_BARRIER presentBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        renderTargets[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    commandList->ResourceBarrier(1, &presentBarrier);
    ThrowIfFailed(commandList->Close());
}
