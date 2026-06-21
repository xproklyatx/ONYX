#pragma once
#include "Win32Platform.hpp"
#include <wrl.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <d3dcompiler.h>
#include <DirectXMath.h>
class DX
{
public:
	template<typename T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
public:
	DX();
	~DX();
	void Init();
	void Render();
private:
	void ThrowIfFailed(HRESULT hr);
	static const UINT frameCount = 2;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12Resource> renderTargets[frameCount];
	ComPtr<ID3D12CommandAllocator> commandAllocator;
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12DescriptorHeap> rtvHeap;
	ComPtr<ID3D12PipelineState> pipelineState;
#ifndef NDEBUG
	Microsoft::WRL::ComPtr<ID3D12InfoQueue> infoQueue;
#endif
	UINT rtvDescriptorSize;
	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
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
	void LoadPipeline();
	void LoadAssets();
	void PopulateCommandList();
	void Update();
	void WaitForPreviousFrame();
#ifndef NDEBUG
	void FlushDebugMessages();
#endif
};
