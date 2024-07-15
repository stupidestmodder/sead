#include <hostio/seadHostIONode.h>

#include <hostio/seadHostIOContext.h>
#include <hostio/seadHostIOThreadLock.h>
#include <prim/seadFormatPrint.h>

namespace sead { namespace hostio {

Node::Node()
    : Reflexible()
    , mTreeNode(this)
{
}

Node::Node(Heap* disposerHeap, IDisposer::HeapNullOption option)
    : Reflexible(disposerHeap, option)
    , mTreeNode(this)
{
}

// TODO: Is this correct ?
Reflexible* Node::searchNode(const SafeString& url)
{
    s32 separatePos = url.findIndex("/");

    FixedSafeString<128> checkName;
    checkName.copy(url, separatePos > 0 ? separatePos : -1);

    s32 tabPos = checkName.findIndex(":");
    if (tabPos > -1)
    {
        checkName.trim(tabPos);
    }

    FixedSafeString<128> nodeName(getNodeName());

    for (s32 i = 0, len = nodeName.calcLength(); i < len; i++)
    {
        if (nodeName.at(i) == ':')
        {
            nodeName.getBuffer()[i] = '_';
        }
    }

    if (nodeName.isEqual(checkName))
    {
        sead::SafeString childUrl = url.getPart(separatePos + 1);
        if (separatePos < 0)
        {
            return this;
        }

        TTreeNode<Node*>* treeNode = mTreeNode.child();
        for (; treeNode; treeNode = treeNode->next())
        {
            Node* child = treeNode->val();

            Reflexible* childResult = child->searchNode(childUrl);
            if (childResult)
            {
                return childResult;
            }
        }

        return this;
    }

    return nullptr;
}

void Node::genChildNode(Context* mc)
{
    TTreeNode<Node*>* treeNode = mTreeNode.child();
    for (; treeNode; treeNode = treeNode->next())
    {
        Node* node = treeNode->val();

        mc->genNode(node->getNodeName(), node, node->getNodeMeta());
    }
}

void Node::appendChild(Node* node)
{
    ThreadLock lock;

    if (!node->isAppended())
    {
        mTreeNode.pushBackChild(&node->mTreeNode);
    }
    else
    {
        (BufferingPrintFormatter() << "[hostio::Node::appendChild] Warning!! this instance already appended!! (name:%s)", node->getNodeName().cstr()).flushWithLineBreak();
    }
}

void Node::insertAfterSelf(Node* node)
{
    ThreadLock lock;

    if (!node->isAppended())
    {
        mTreeNode.insertAfterSelf(&node->mTreeNode);
    }
    else
    {
        (BufferingPrintFormatter() << "[hostio::Node::insertChildAfterSelf] Warning!! this instance already appended!! (name:%s)", node->getNodeName().cstr()).flushWithLineBreak();
    }
}

void Node::insertBeforeSelf(Node* node)
{
    ThreadLock lock;

    if (!node->isAppended())
    {
        mTreeNode.insertBeforeSelf(&node->mTreeNode);
    }
    else
    {
        (BufferingPrintFormatter() << "[hostio::Node::insertChildBeforeSelf] Warning!! this instance already appended!! (name:%s)", node->getNodeName().cstr()).flushWithLineBreak();
    }
}

void Node::detachAll()
{
    ThreadLock lock;
    mTreeNode.detachAll();
}

void Node::detach()
{
    ThreadLock lock;
    mTreeNode.detachSubTree();
}

Node* Node::getParentNode() const
{
    if (!mTreeNode.parent())
    {
        return nullptr;
    }

    return mTreeNode.parent()->val();
}

Node* Node::getChildNode() const
{
    if (!mTreeNode.child())
    {
        return nullptr;
    }

    return mTreeNode.child()->val();
}

Node* Node::getNextNode() const
{
    if (!mTreeNode.next())
    {
        return nullptr;
    }

    return mTreeNode.next()->val();
}

Node* Node::getPrevNode() const
{
    if (mTreeNode.parent() && mTreeNode.parent()->child() == &mTreeNode)
    {
        return nullptr;
    }

    if (!mTreeNode.prev())
    {
        return nullptr;
    }

    return mTreeNode.prev()->val();
}

bool Node::isAppended() const
{
    return mTreeNode.parent() || mTreeNode.next() || mTreeNode.prev();
}

void Node::disposeHostIOImpl_()
{
    destroy();
}

void Node::calcURLImpl_(BufferedSafeString* url, bool isOwnNode) const
{
    if (getParentNode())
    {
        getParentNode()->calcURLImpl_(url, false);
    }

    s32 beginIndex = url->calcLength();
    url->copyAtWithTerminate(beginIndex, getNodeName());

    for (s32 i = beginIndex, len = url->calcLength(); i < len; i++)
    {
        if (url->at(i) == ':')
        {
            url->getBuffer()[i] = '_';
        }
    }

    if (!isOwnNode)
    {
        url->append("/");
    }
}

} } // namespace sead::hostio
