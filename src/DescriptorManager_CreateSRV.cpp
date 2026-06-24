#include "DescriptorManager.hpp"
#include "Helper.hpp"
#include "stb/stb_image.h"

size_t DescriptorManager::CreateSRVHandle(const char* pathToTexture)
{

    if (!commandList)
    {
        std::cout << "ERROR: commandList is nullptr in CreateSRVHandle!\n";
        return SIZE_MAX;
    }

    if (!device)
    {
        std::cout << "ERROR: device is nullptr in CreateSRVHandle!\n";
        return SIZE_MAX;
    }

    if (!cusHeap)
    {
        std::cout << "ERROR: cusHeap is nullptr in CreateSRVHandle!\n";
        return SIZE_MAX;
    }

    if (currentSRVIndex >= maxSRV)
    {
        std::cout << "SRVs are full bro please!\n";
        return SIZE_MAX;
    }

    int width, height, channels;
    UINT8* data = stbi_load(pathToTexture, &width, &height, &channels, 4);
    if (!data)
    {
        std::cout << "Failed to load texture: " << pathToTexture << "\n";
        return SIZE_MAX;
    }

    SRV_ENTRY srv;
    D3D12_RESOURCE_DESC textureDesc = {};
    textureDesc.MipLevels = 1;
    textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    textureDesc.Width = width;
    textureDesc.Height = height;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
    textureDesc.DepthOrArraySize = 1;
    textureDesc.SampleDesc.Count = 1;
    textureDesc.SampleDesc.Quality = 0;
    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

    {
        CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        HRESULT hr =
            device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &textureDesc,
                                            D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&srv.texture));
        if (FAILED(hr))
        {
            std::cout << "Failed to create texture resource!\n";
            stbi_image_free(data);
            return SIZE_MAX;
        }
    }

    const UINT64 uploadBufferSize = GetRequiredIntermediateSize(srv.texture.Get(), 0, 1);
    {
        CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
        HRESULT hr = device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
                                                     D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                     IID_PPV_ARGS(&srv.textureUploadHeap));
        if (FAILED(hr))
        {
            std::cout << "Failed to create upload heap!\n";
            stbi_image_free(data);
            return SIZE_MAX;
        }
    }

    D3D12_SUBRESOURCE_DATA textureData = {};
    textureData.pData = data;
    textureData.RowPitch = width * 4;
    textureData.SlicePitch = textureData.RowPitch * height;

    if (!commandList)
    {
        std::cout << "ERROR: commandList is nullptr before UpdateSubresources!\n";
        stbi_image_free(data);
        return SIZE_MAX;
    }

    UpdateSubresources(commandList, srv.texture.Get(), srv.textureUploadHeap.Get(), 0, 0, 1, &textureData);

    CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
        srv.texture.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
    commandList->ResourceBarrier(1, &barrier);

    UINT heapOffset = srvStartOffset + currentSRVIndex;

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(cusHeap->GetCPUDescriptorHandleForHeapStart(), heapOffset, cusHeapSize);

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = textureDesc.Format;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    device->CreateShaderResourceView(srv.texture.Get(), &srvDesc, cpuHandle);

    SRVs.push_back(std::move(srv));

    size_t srvIndex = currentSRVIndex;
    currentSRVIndex++;

    stbi_image_free(data);

    return srvIndex;
}
