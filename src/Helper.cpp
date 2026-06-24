#include "Helper.hpp"
#include <sstream>
void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
    {
#ifndef NDEBUG
        std::ostringstream oss;
        oss << "HRESULT Failed: 0x" << std::hex << std::uppercase << hr << "\n";
        PRINT_ERROR(oss.str().c_str());
#endif
        throw std::exception();
    }
}
