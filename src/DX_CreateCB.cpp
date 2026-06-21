#include "DX.hpp"
#include "d3dx12_core.h"
void DX::CreateCB()
{
    const UINT constantBufferSize = sizeof(SceneConstantBuffer);
    CD3DX12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(constantBufferSize);
    ThrowIfFailed(device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
                                                  D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                  IID_PPV_ARGS(&constantBuffer)));

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = constantBufferSize;
    device->CreateConstantBufferView(&cbvDesc, cusHeap->GetCPUDescriptorHandleForHeapStart());

    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pCbvDataBegin)));
    memcpy(pCbvDataBegin, &cbData, sizeof(cbData));
}
