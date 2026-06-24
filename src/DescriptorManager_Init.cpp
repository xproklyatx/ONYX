#include "DescriptorManager.hpp"
#include "Helper.hpp"
void DescriptorManager::Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, UINT cbvNum, UINT uavNum,
                             UINT srvNum, UINT samplerNum)
{
    // TODO: add sampler heap stuff later.
    this->device = device;
    this->commandList = commandList;
    D3D12_DESCRIPTOR_HEAP_DESC cusHeapDesc = {};
    cusHeapDesc.NumDescriptors = cbvNum + uavNum + srvNum;
    cusHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cusHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    ThrowIfFailed(device->CreateDescriptorHeap(&cusHeapDesc, IID_PPV_ARGS(&cusHeap)));
    cusHeapSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    cbvStartOffset = 0;
    uavStartOffset = cbvNum;
    srvStartOffset = cbvNum + uavNum;
    currentCBVIndex = 0;
    currentUAVIndex = 0;
    currentSRVIndex = 0;
    maxCBV = cbvNum;
    maxSRV = srvNum;
    maxUAV = uavNum;
}
