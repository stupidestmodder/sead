#pragma once

#include <hostio/seadHostIONode.h>

namespace sead {

class HostIORoot : public hostio::Node
{
public:
    HostIORoot()
        : hostio::Node()
    {
    }

#if defined(SEAD_TARGET_DEBUG)
    HostIORoot(Heap* disposerHeap, IDisposer::HeapNullOption option)
        : hostio::Node(disposerHeap, option)
    {
    }

    void listenPropertyEvent(const hostio::PropertyEvent* ev) override;
    void genMessage(hostio::Context* ctx) override;
#endif // SEAD_TARGET_DEBUG
};

} // namespace sead
