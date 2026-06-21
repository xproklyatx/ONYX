#include "DX.hpp"
void DX::LoadAssets()
{

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;
	//serializing and creating root signature
	ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	ThrowIfFailed(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));



	//creating vertex shader and pixel shader
	Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;

	//depending on debug mode or not setting the compiler flags to be there or not.
#ifndef NDEBUG
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else 
	UINT compileFlags = 0;
#endif 
	//compiling the shaders
	ThrowIfFailed(D3DCompileFromFile(L"res/shaders/shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
	ThrowIfFailed(D3DCompileFromFile(L"res/shaders/shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));
	//setting how input elements are interpreted to the shader
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0}
	};
	{
		//setting the pipeline state
		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
		//giving it our input layout we created
		psoDesc.InputLayout = { inputElementDescs,_countof(inputElementDescs) };
		//giving it the root signature
		psoDesc.pRootSignature = rootSignature.Get();
		//giving it our vertex and pixel shader
		psoDesc.VS = { reinterpret_cast<UINT8*>(vertexShader->GetBufferPointer()),vertexShader->GetBufferSize() };
		psoDesc.PS = { reinterpret_cast<UINT8*>(pixelShader->GetBufferPointer()),pixelShader->GetBufferSize() };
		//setting the rasterizer as the default one
		psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		//and also the blending state 
		psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		//disabling depth and stencil buffer for now
		psoDesc.DepthStencilState.DepthEnable = FALSE;
		psoDesc.DepthStencilState.StencilEnable = FALSE;
		psoDesc.SampleMask = UINT_MAX;
		//setting the render type to render in triangles for our pipeline
		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		psoDesc.NumRenderTargets = 1;
		//the format for our render target. we set this before in swapchain too
		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		//remember multisampling stuff i said before? they are here too
		psoDesc.SampleDesc.Count = 1;
		psoDesc.SampleDesc.Quality = 0;
		//creating the pipeline
		ThrowIfFailed(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
	}
	//creating the command list with our information
	ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), pipelineState.Get(), IID_PPV_ARGS(&commandList)));
	//and closing it for later that we do stuff with it
	ThrowIfFailed(commandList->Close());

	{
		//beautiful triangle vertices
		Vertex triangleVertices[] =
		{
			{ { 0.0f, 0.25f * aspectRatio, 0.0f }, { 0.8f, 0.0f, 0.8f, 1.0f } },

			{ { 0.25f, -0.25f * aspectRatio, 0.0f }, { 0.2f, 0.0f, 0.6f, 1.0f } },

			{ { -0.25f, -0.25f * aspectRatio, 0.0f }, { 0.6f, 0.0f, 0.2f, 1.0f } }
		};
		//setting the size of our vertex buffer
		const UINT vertexBufferSize = sizeof(triangleVertices);
		//this type isn't efficient as MSDN notes cuz it has tons of overhead. but a triangle is simple eh?
		//we are setting how descriptor heap uploads stuff
		CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
		//setting resource description for our vertex buffer
		auto desc = CD3DX12_RESOURCE_DESC::Buffer(vertexBufferSize);
		ThrowIfFailed(device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexBuffer)));
		//a pointer to where vertex data even begins
		UINT8* pVertexDataBegin;
		//making a renge
		CD3DX12_RANGE readRange(0, 0);
		//mapping that range
		ThrowIfFailed(vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
		//copying the triangle verteices to our array of vertex data. or vertex data pointer i shall say.
		//array is just the pointer to the beginning of an arranged memory anyway XDD
		memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
		//unmapping the vertex buffer
		vertexBuffer->Unmap(0, nullptr);

		// Initialize the vertex buffer view.
		vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
		//setting our strides. as we know we have an array of Vertex structs.
		//so stride for our data has the same size as it.
		vertexBufferView.StrideInBytes = sizeof(Vertex);
		//we got the size before from Vertex struct sizeof
		vertexBufferView.SizeInBytes = vertexBufferSize;
	}
	{
		//creating fence for handling CPU-GPU scheduling
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

