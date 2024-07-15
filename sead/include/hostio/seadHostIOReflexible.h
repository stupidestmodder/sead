#pragma once

#include <hostio/seadHostIOEventListener.h>
#include <prim/seadSafeString.h>

namespace sead { namespace hostio {

struct GenEvent;

class Context;

class Reflexible : public NodeEventListener
{
public:
    enum class NodeClassType
    {
        eReflexible,
        eNode,
        eUser
    };

    virtual NodeClassType getNodeClassType() const
    {
        return NodeClassType::eReflexible;
    }

#ifdef SEAD_DEBUG
protected:
    enum AllocFlg
    {
        eName = 1 << 0,
        eMeta = 1 << 1
    };

    using ApplyEventDataToMemoryCallback = bool (*)(const PropertyEvent* ev);

    static ApplyEventDataToMemoryCallback sApplyEventDataToMemoryCallback;

public:
    Reflexible();
    Reflexible(Heap* disposerHeap, IDisposer::HeapNullOption option);

    ~Reflexible() override
    {
        disposeHostIOImpl_();
    }

    void disposeHostIO() override
    {
        disposeHostIOImpl_();
        this->NodeEventListener::disposeHostIO();
    }

    void listenNodeEvent(const NodeEvent* event) override
    {
        SEAD_UNUSED(event);
    }

    virtual void genMessage(Context* mc)
    {
        SEAD_UNUSED(mc);
    }

    virtual SafeString getMetaFilename()
    {
        return SafeString::cEmptyString;
    }

    virtual void genObjectInfo(const GenEvent* event, u32 createIdOR);

    virtual Reflexible* searchNode(const SafeString& url)
    {
        SEAD_UNUSED(url);
        return nullptr;
    }

    virtual void calcURL(BufferedSafeString* url) const
    {
        url->copy("");
    }

    virtual void calcNodeURL(const Reflexible* node, BufferedSafeString* url)
    {
        node->calcURL(url);
    }

    virtual void genChildNode(Context* mc)
    {
        SEAD_UNUSED(mc);
    }

    virtual bool isHaveChild() const
    {
        return false;
    }

    void callGenMessage(Context* mc, u32 createIdOR);
    void correctChildNodeInfo(Context* mc);
    void baseListen(const PropertyEvent* ev);

    const SafeString getNodeName() const
    {
        return mName ? mName : "";
    }

    const SafeString getNodeMeta() const
    {
        return mMeta ? mMeta : "";
    }

    void setNodeName(const SafeString& name);
    void setNodeNameCopyString(const SafeString& name, Heap* heap);
    void setNodeMeta(const SafeString& meta);
    void setNodeMetaCopyString(const SafeString& meta, Heap* heap);

    static void applyEventDataToMemory(const PropertyEvent* ev);

private:
    void safeDelete_(AllocFlg flg);
    const char* createStringBuffer_(AllocFlg flg, const SafeString& str, Heap* heap);

    void disposeHostIOImpl_();

private:
    const char* mName;
    const char* mMeta;
    bool mIsGenerated;
    u8 mAllocFlg;
#endif // SEAD_DEBUG
};

} } // namespace sead::hostio
