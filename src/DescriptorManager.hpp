#pragma once
#include <Windows.h>
#include <wrl.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <vector>
struct CB_INFO
{
    size_t index;
    UINT8* pCBVDataBegin;
    UINT size;
};
enum ONYX_CUS_TYPE
{
    CBV,
    UAV,
    SRV,
    DIRECT
};
class DescriptorManager
{
  public:
    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  public:
    DescriptorManager();
    ~DescriptorManager();
    void Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, UINT cbvNum, UINT uavNum, UINT srvNum,
              UINT samplerNum);
    CB_INFO CreateCBHandle(size_t sizeOfConstantBuffer);
    size_t CreateSRVHandle(const char* pathToTexture);
    template <ONYX_CUS_TYPE Type> void SetTable(size_t rootRegisterNum, size_t descIndex)
    {
        UINT heapOffset = 0;

        if constexpr (Type == ONYX_CUS_TYPE::CBV)
        {
            heapOffset = cbvStartOffset + static_cast<UINT>(descIndex);
        }
        else if constexpr (Type == ONYX_CUS_TYPE::SRV)
        {
            heapOffset = srvStartOffset + static_cast<UINT>(descIndex);
        }
        else if constexpr (Type == ONYX_CUS_TYPE::UAV)
        {
            heapOffset = uavStartOffset + static_cast<UINT>(descIndex);
        }
        else if constexpr (Type == ONYX_CUS_TYPE::DIRECT)
        {
            heapOffset = static_cast<UINT>(descIndex);
        }

        CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(cusHeap->GetGPUDescriptorHandleForHeapStart(), heapOffset, cusHeapSize);

        commandList->SetGraphicsRootDescriptorTable(static_cast<UINT>(rootRegisterNum), gpuHandle);
    }
    void ShutDown();
    ID3D12DescriptorHeap* GetHeap() const
    {
        return cusHeap.Get();
    }

  private:
    struct SRV_ENTRY
    {
        ComPtr<ID3D12Resource> texture;
        ComPtr<ID3D12Resource> textureUploadHeap;
    };
    ID3D12Device* device;
    ID3D12GraphicsCommandList* commandList;
    ComPtr<ID3D12DescriptorHeap> cusHeap;
    UINT cusHeapSize;
    UINT cbvStartOffset;
    UINT uavStartOffset;
    UINT srvStartOffset;
    UINT currentSRVIndex;
    UINT currentUAVIndex;
    UINT currentCBVIndex;
    UINT maxCBV;
    UINT maxUAV;
    UINT maxSRV;
    UINT currentSamplerIndex;
    std::vector<ComPtr<ID3D12Resource>> CBVs;
    std::vector<SRV_ENTRY> SRVs;
};
