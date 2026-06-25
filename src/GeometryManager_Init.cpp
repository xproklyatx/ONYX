#include "GeometryManager.hpp"
void GeometryManager::Init(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
    this->device = device;
    this->commandList = commandList;
}
