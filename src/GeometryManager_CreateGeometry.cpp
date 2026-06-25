#include "GeometryManager.hpp"
#include "Helper.hpp"
Geometry GeometryManager::CreateGeometry(const std::vector<Vertex>& vertices, const std::vector<UINT16>& indices)
{
    Geometry geometry{};
    GeometryEntry entry{};

    {
        const UINT vertexBufferSize = static_cast<UINT>(vertices.size() * sizeof(Vertex));
        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        auto desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
        ThrowIfFailed(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                      D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                      IID_PPV_ARGS(&entry.vb)));
        UINT8* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);
        ThrowIfFailed(entry.vb->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, vertices.data(), vertexBufferSize);
        entry.vb->Unmap(0, nullptr);

        geometry.vbv.BufferLocation = entry.vb->GetGPUVirtualAddress();
        geometry.vbv.StrideInBytes = sizeof(Vertex);
        geometry.vbv.SizeInBytes = vertexBufferSize;
    }

    {
        geometry.indexCount = static_cast<UINT>(indices.size());
        const UINT indexBufferSize = static_cast<UINT>(indices.size() * sizeof(UINT16));
        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
        auto desc = CD3DX12_RESOURCE_DESC::Buffer(indexBufferSize);
        ThrowIfFailed(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc,
                                                      D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                      IID_PPV_ARGS(&entry.ib)));

        UINT8* pIndexDataBegin;
        CD3DX12_RANGE readRange(0, 0);
        ThrowIfFailed(entry.ib->Map(0, &readRange, reinterpret_cast<void**>(&pIndexDataBegin)));
        memcpy(pIndexDataBegin, indices.data(), indexBufferSize);
        entry.ib->Unmap(0, nullptr);

        geometry.ibv.BufferLocation = entry.ib->GetGPUVirtualAddress();
        geometry.ibv.Format = DXGI_FORMAT_R16_UINT;
        geometry.ibv.SizeInBytes = indexBufferSize;
    }

    geometries.push_back(std::move(entry));
    return geometry;
}
