#include "ICam.hpp"
#include <stdexcept>

ICam* ICam::pInstance = nullptr;

ICam& ICam::GetInstance()
{
    if (!pInstance)
    {
        throw std::runtime_error("Camera not set! Call ICam::Set() first.");
    }
    return *pInstance;
}

void ICam::Set(ICam* pCam)
{
    pInstance = pCam;
}
