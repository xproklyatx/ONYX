#include "GeometryManager.hpp"
void GeometryManager::Draw(Geometry* geometry)
{
    commandList->IASetVertexBuffers(0, 1, &geometry->vbv);
    commandList->IASetIndexBuffer(&geometry->ibv);
    commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    commandList->DrawIndexedInstanced(geometry->indexCount, 1, 0, 0, 0);
}
