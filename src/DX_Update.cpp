#include "DX.hpp"
void DX::Update()
{
    using namespace DirectX;

    XMMATRIX rotY = XMMatrixRotationY(rotationAngle);
    XMMATRIX rotX = XMMatrixRotationX(rotationAngle * 0.5f);

    XMMATRIX model = rotX * rotY;

    XMVECTOR eye = XMVectorSet(0.0f, 0.0f, -3.0f, 0.0f);
    XMVECTOR at = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(eye, at, up);

    float aspectRatio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
    XMMATRIX projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), aspectRatio, 0.1f, 100.0f);

    cbData.model = XMMatrixTranspose(model);
    cbData.view = XMMatrixTranspose(view);
    cbData.proj = XMMatrixTranspose(projection);

    memcpy(pCbvDataBegin, &cbData, sizeof(cbData));

    rotationAngle += 0.02f;
}
