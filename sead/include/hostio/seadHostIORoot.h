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

    HostIORoot(Heap* disposerHeap, IDisposer::HeapNullOption option)
        : hostio::Node(disposerHeap, option)
    {
    }

    //void listenPropertyEvent(const hostio::PropertyEvent*) override; // TODO
    void genMessage(hostio::Context* ctx) override;
};

} // namespace sead
