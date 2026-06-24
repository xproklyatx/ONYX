#include "DescriptorManager.hpp"
#include "Helper.hpp"
CB_INFO DescriptorManager::CreateCBHandle(size_t sizeOfConstantBuffer)
{
    if (currentCBVIndex >= maxCBV)
    {
        std::cout << "BRO! NO MORE CBVs!\n";
        return CB_INFO{};
    }
    CB_INFO info{};
    info.size = sizeOfConstantBuffer;
    ComPtr<ID3D12Resource> constantBuffer;
    CD3DX12_HEAP_PROPERTIES props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(info.size);
    ThrowIfFailed(device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc,
                                                  D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                  IID_PPV_ARGS(&constantBuffer)));

    CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(cusHeap->GetCPUDescriptorHandleForHeapStart(),
                                            cbvStartOffset + currentCBVIndex, cusHeapSize);
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = constantBuffer->GetGPUVirtualAddress();
    cbvDesc.SizeInBytes = info.size;
    device->CreateConstantBufferView(&cbvDesc, cbvHandle);

    CD3DX12_RANGE readRange(0, 0);
    ThrowIfFailed(constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&info.pCBVDataBegin)));
    info.index = currentCBVIndex;
    currentCBVIndex++;
    CBVs.push_back(std::move(constantBuffer));
    return info;
}
