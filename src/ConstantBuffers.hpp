#pragma once
#include <DirectXMath.h>
namespace ONYX
{
struct alignas(16) CB_M
{
    DirectX::XMMATRIX mat;
};
struct alignas(16) CB_DTT
{
    float DT;
    float T;
    float pad[2];
};

} // namespace ONYX
