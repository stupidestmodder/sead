#pragma once

#include <container/seadTreeNode.h>
#include <hostio/seadHostIOReflexible.h>

namespace sead { namespace hostio {

class Node : public Reflexible
{
public:
    NodeClassType getNodeClassType() const override
    {
        return NodeClassType::eNode;
    }

#ifdef SEAD_DEBUG
public:
    Node();
    Node(Heap* disposerHeap, IDisposer::HeapNullOption option);

    ~Node() override
    {
        disposeHostIOImpl_();
    }

    void disposeHostIO() override
    {
        disposeHostIOImpl_();
        this->Reflexible::disposeHostIO();
    }

    Reflexible* searchNode(const SafeString& url) override;

    void calcURL(BufferedSafeString* url) const override
    {
        calcURLImpl_(url, true);
    }

    void genChildNode(Context* mc) override;

    bool isHaveChild() const override
    {
        return mTreeNode.child() != nullptr;
    }

    void appendChild(Node* node);
    void insertAfterSelf(Node* node);
    void insertBeforeSelf(Node* node);
    void detachAll();
    void detach();

    void destroy()
    {
        detachAll();
    }

    Node* getParentNode() const;
    Node* getChildNode() const;
    Node* getNextNode() const;
    Node* getPrevNode() const;

    bool isAppended() const;

private:
    void disposeHostIOImpl_();
    void calcURLImpl_(BufferedSafeString* url, bool isOwnNode) const;

private:
    TTreeNode<Node*> mTreeNode;
#endif // SEAD_DEBUG
};

} } // namespace sead::hostio
