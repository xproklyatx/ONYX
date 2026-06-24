#include "DescriptorManager.hpp"
#include "Console.hpp"
void DescriptorManager::ShutDown()
{
    CBVs.clear();
    CBVs.shrink_to_fit();

    SRVs.clear();
    SRVs.shrink_to_fit();

    if (cusHeap)
    {
        cusHeap.Reset();
    }

    device = nullptr;
    commandList = nullptr;

    currentCBVIndex = 0;
    currentUAVIndex = 0;
    currentSRVIndex = 0;
    currentSamplerIndex = 0;

    maxCBV = 0;
    maxUAV = 0;
    maxSRV = 0;

    PRINT_LOG("DescriptorManager shutdown complete");
}
