#include "DX.hpp"
#include "ICam.hpp"
void DX::Update()
{
    using namespace DirectX;

    XMMATRIX rotY = XMMatrixRotationY(rotationAngle);
    XMMATRIX rotX = XMMatrixRotationX(rotationAngle * 0.5f);

    XMMATRIX model = rotX * rotY;

    XMMATRIX view = ICam::GetInstance().GetViewMatrix();

    float aspectRatio = static_cast<float>(SCR_WIDTH) / static_cast<float>(SCR_HEIGHT);
    XMMATRIX projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), aspectRatio, 0.1f, 100.0f);

    cbData.model = XMMatrixTranspose(model);
    cbData.view = XMMatrixTranspose(view);
    cbData.proj = XMMatrixTranspose(projection);

    memcpy(pCbvDataBegin, &cbData, sizeof(cbData));

    rotationAngle += 0.02f;
}
