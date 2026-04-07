#include <hostio/seadHostIOFramework.h>

#include <hostio/seadHostIOConfig.h>
#include <hostio/seadHostIONode.h>
#include <hostio/seadHostIOServer.h>
#include <hostio/seadHostIOThreadLock.h>

namespace sead { namespace hostio {

TmpContext::TmpContext(Command command)
{
    ThreadLock::lock();
    mContext = Server::getInstance()->attachContext(command);
}

TmpContext::~TmpContext()
{
    Server::getInstance()->releaseContext(mContext);
    mContext = nullptr;
    ThreadLock::unlock();
}

void Init(Heap* heap, const Config* argConfig)
{
    const Config* config = argConfig;

    Config defaultConfig;
    if (!config)
    {
        config = &defaultConfig;
    }

    Server::createInstance(heap, config);
    Server::getInstance()->createPortDirectConnect(config);
}

void Finalize()
{
    Server* server = Server::getInstance();
    SEAD_ASSERT(server != nullptr);

    Server::deleteInstance();
}

bool IsInitialized()
{
    return Server::isInitialized();
}

void MessageLoop()
{
    Server* server = Server::getInstance();
    ThreadLock lock;

    while (server->dispatchMessage())
    {
    }

    server->polling();
}

void AddNode(Node* parent, const SafeString& name, Node* node, const SafeString& meta)
{
    ThreadLock lock;

    SEAD_ASSERT(parent != nullptr);
    SEAD_ASSERT(node != nullptr);

    node->setNodeName(name);
    node->setNodeMeta(meta);

    parent->appendChild(node);
}

void SetRootNode(const SafeString& name, Reflexible* node, const SafeString& meta)
{
    ThreadLock lock;

    Server* server = Server::getInstance();
    server->setRootNode(name, node, meta);
}

} } // namespace sead::hostio
