#include "DX.hpp"
void DX::LoadAssets()
{
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData = {};

    featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

    if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
    {
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
    }

    CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
    CD3DX12_ROOT_PARAMETER1 rootParameters[2];

    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
    rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);
    rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);

    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
                                                    D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                                                    D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                                                    D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
    D3D12_STATIC_SAMPLER_DESC sampler = {};
    sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
    sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    sampler.MipLODBias = 0;
    sampler.MaxAnisotropy = 0;
    sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
    sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    sampler.MinLOD = 0.0f;
    sampler.MaxLOD = D3D12_FLOAT32_MAX;
    sampler.ShaderRegister = 0;
    sampler.RegisterSpace = 0;
    sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
    CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init_1_1(_countof(rootParameters), rootParameters, 1, &sampler, rootSignatureFlags);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ThrowIfFailed(
        D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
    ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(),
                                              IID_PPV_ARGS(&rootSignature)));
    Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
    Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;
#ifndef NDEBUG
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif
    ThrowIfFailed(D3DCompileFromFile(L"res/shaders/shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0,
                                     &vertexShader, nullptr));
    ThrowIfFailed(D3DCompileFromFile(L"res/shaders/shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0,
                                     &pixelShader, nullptr));
    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, pos),
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex, color),
         D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex, uv), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
         0}};
    {
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = {inputElementDescs, _countof(inputElementDescs)};
        psoDesc.pRootSignature = rootSignature.Get();
        psoDesc.VS = {reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize()};
        psoDesc.PS = {reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize()};
        CD3DX12_RASTERIZER_DESC rasterDesc(D3D12_DEFAULT);
        rasterDesc.CullMode = D3D12_CULL_MODE_NONE;
        psoDesc.RasterizerState = rasterDesc;
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);

        psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        psoDesc.DepthStencilState.StencilEnable = FALSE;

        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleDesc.Quality = 0;
        ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
    }
    ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(),
                                            pipelineState.Get(), IID_PPV_ARGS(&commandList)));
    {
        descriptorManager.Init(device.Get(), commandList.Get(), 10, 0, 10, 0);
        cbInfo = descriptorManager.CreateCBHandle(sizeof(SceneConstantBuffer));
        memcpy(cbInfo.pCBVDataBegin, &cbData, sizeof(cbData));

        descriptorManager.CreateSRVHandle("res/textures/cutie.jpg");
    }
    {

        std::vector<Vertex> vertices = {{{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                                        {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
                                        {{0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
                                        {{0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},

                                        {{-0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                                        {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
                                        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
                                        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},

                                        {{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
                                        {{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
                                        {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
                                        {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

                                        {{-0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
                                        {{-0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
                                        {{0.5f, -0.5f, 0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
                                        {{0.5f, -0.5f, -0.5f}, {1.0f, 1.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},

                                        {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
                                        {{0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
                                        {{0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
                                        {{0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},

                                        {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
                                        {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
                                        {{-0.5f, 0.5f, 0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
                                        {{-0.5f, 0.5f, -0.5f}, {1.0f, 0.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}};
        std::vector<UINT16> indices = {0,  1,  2,  0,  2,  3,

                                       4,  6,  5,  4,  7,  6,

                                       8,  9,  10, 8,  10, 11,

                                       12, 14, 13, 12, 15, 14,

                                       16, 17, 18, 16, 18, 19,

                                       20, 22, 21, 20, 23, 22};
        geometryManager.Init(device.Get(), commandList.Get());
        geo = geometryManager.CreateGeometry(vertices, indices);
        ThrowIfFailed(commandList->Close());
        ID3D12CommandList* ppCommandLists[] = {commandList.Get()};
        commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
    }
    {
        ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
        fenceValue = 1;
        fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        WaitForPreviousFrame();
    }
}
