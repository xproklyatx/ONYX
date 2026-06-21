#ifndef NDEBUG
#include "DX.hpp"
#include "Console.hpp"
void DX::FlushDebugMessages()
{
    if (!infoQueue)
        return;

    UINT64 numMessages = infoQueue->GetNumStoredMessages();

    for (UINT64 i = 0; i < numMessages; ++i)
    {
        SIZE_T messageSize = 0;
        infoQueue->GetMessage(i, nullptr, &messageSize);

        std::vector<BYTE> buffer(messageSize);
        D3D12_MESSAGE* message = reinterpret_cast<D3D12_MESSAGE*>(buffer.data());
        infoQueue->GetMessage(i, message, &messageSize);

        switch (message->Severity)
        {
        case D3D12_MESSAGE_SEVERITY_CORRUPTION:
        case D3D12_MESSAGE_SEVERITY_ERROR:
            PRINT_ERROR(message->pDescription);
            break;
        case D3D12_MESSAGE_SEVERITY_WARNING:
            PRINT_WARN(message->pDescription);
            break;
        case D3D12_MESSAGE_SEVERITY_INFO:
            PRINT_LOG(message->pDescription);
            break;
        default:
            PRINT_DIAG(message->pDescription);
            break;
        }
    }
    infoQueue->ClearStoredMessages();
}
#endif
