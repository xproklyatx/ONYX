#include "DX.hpp"
void DX::CreateCUSHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC cusHeapDesc = {};
    cusHeapDesc.NumDescriptors = 2;
    cusHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cusHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    ThrowIfFailed(device->CreateDescriptorHeap(&cusHeapDesc, IID_PPV_ARGS(&cusHeap)));
    cusSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
