#pragma once
#include <DirectXMath.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <wrl.h>
#include <vector>
struct Geometry
{
    D3D12_VERTEX_BUFFER_VIEW vbv;
    D3D12_INDEX_BUFFER_VIEW ibv;
    UINT indexCount;
};
struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT4 color;
    DirectX::XMFLOAT2 uv;
};

class GeometryManager
{
  public:
    template <typename T> using ComPtr = Microsoft::WRL::ComPtr<T>;

  public:
    GeometryManager();
    ~GeometryManager();
    void Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
    Geometry CreateGeometry(const std::vector<Vertex>& vertices, const std::vector<UINT16>& indices);
    void Draw(Geometry* geometry);
    void ShutDown();

  private:
    ID3D12Device* device;
    ID3D12GraphicsCommandList* commandList;
    struct GeometryEntry
    {
        ComPtr<ID3D12Resource> vb;
        ComPtr<ID3D12Resource> ib;
    };
    std::vector<GeometryEntry> geometries;
};
