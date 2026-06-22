#pragma once
#include "Win32Platform.hpp"
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
class DX
{
  public:
    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  public:
    DX();
    ~DX();
    void Init();
    void Render();

  private:
    void ThrowIfFailed(HRESULT hr);
    static const UINT frameCount = 2;
    static const UINT textureWidth = 256;
    static const UINT textureHeight = 256;
    static const UINT texturePixelSize = 4;
    ComPtr<IDXGISwapChain3> swapChain;
    ComPtr<ID3D12Device> device;
    ComPtr<ID3D12Resource> renderTargets[frameCount];
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ComPtr<ID3D12GraphicsCommandList> commandList;
    ComPtr<ID3D12CommandQueue> commandQueue;
    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12DescriptorHeap> rtvHeap;
    ComPtr<ID3D12PipelineState> pipelineState;
    ComPtr<ID3D12Resource> depthStencilBuffer;
    ComPtr<ID3D12DescriptorHeap> dsvHeap;
    ComPtr<ID3D12DescriptorHeap> cusHeap;
    ComPtr<ID3D12Resource> constantBuffer;
    ComPtr<ID3D12Resource> texture;
    UINT8* pCbvDataBegin;
    UINT dsvDescriptorSize;
    D3D12_CLEAR_VALUE depthStencilClearValue;
    BOOL useDepthStencil = TRUE;
#ifndef NDEBUG
    Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
#endif
    UINT rtvDescriptorSize;
    ComPtr<ID3D12Resource> vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
    ComPtr<ID3D12Resource> indexBuffer;
    D3D12_INDEX_BUFFER_VIEW indexBufferView;
    UINT indexCount;
    UINT frameIndex;
    ComPtr<ID3D12Fence> fence;
    HANDLE fenceEvent;
    UINT64 fenceValue;
    float aspectRatio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
    D3D12_VIEWPORT viewPort;
    D3D12_RECT scissorRect;
    struct Vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT4 color;
    };
    struct alignas(256) SceneConstantBuffer
    {
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX proj;
    };
    static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");
    SceneConstantBuffer cbData;
    float rotationAngle;
    void LoadPipeline();
    void LoadAssets();
    void PopulateCommandList();
    void Update();
    void WaitForPreviousFrame();
    void CreateDepthStencilView();
    void CreateCUSHeap();
    void CreateCB();
    std::vector<UINT*> GenerateTextureData();
#ifndef NDEBUG
    void FlushDebugMessages();
#endif
};
