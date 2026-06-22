#include "DX.hpp"
std::vector<UINT8> DX::GenerateTextureData()
{
    const UINT rowPitch = textureWidth * texturePixelSize;
    const UINT cellPitch = rowPitch >> 3;
    const UINT cellHeight = textureWidth >> 3;
    const UINT textureSize = rowPitch * textureHeight;

    std::vector<UINT8> data(textureSize);
    UINT8* pData = &data[0];

    for (UINT n = 0; n < textureSize; n += texturePixelSize)
    {
        UINT x = n % rowPitch;
        UINT y = n / rowPitch;
        UINT i = x / cellPitch;
        UINT j = y / cellHeight;

        if (i % 2 == j % 2)
        {
            pData[n] = 0x00;
            pData[n + 1] = 0x00;
            pData[n + 2] = 0x00;
            pData[n + 3] = 0xff;
        }
        else
        {
            pData[n] = 0xff;
            pData[n + 1] = 0xff;
            pData[n + 2] = 0xff;
            pData[n + 3] = 0xff;
        }
    }

    return data;
}
