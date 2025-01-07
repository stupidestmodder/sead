#pragma once

#include <container/seadFreeList.h>
#include <container/seadTreeMapImpl.h>
#include <prim/seadDelegate.h>
#include <prim/seadSafeString.h>

namespace sead {

template <s32 KeyStrN, typename Value>
class StrTreeMap : public TreeMapImpl<SafeString>
{
public:
    class Node : public TreeMapNode<SafeString>
    {
    public:
        Node(const SafeString& akey, StrTreeMap* map)
            : TreeMapNode<SafeString>()
            , mValue()
            , mMap(map)
        {
            BufferedSafeString key(mKeyStr, KeyStrN + 1);
            key.copy(akey);
            mKey_ = key;
        }

        Node(const SafeString& akey, const Value& avalue, StrTreeMap* map)
            : TreeMapNode<SafeString>()
            , mValue(avalue)
            , mMap(map)
        {
            BufferedSafeString key(mKeyStr, KeyStrN + 1);
            key.copy(akey);
            mKey_ = key;
        }

    protected:
        void erase_() override
        {
            mValue.~Value();

            mMap->eraseNode_(this);
        }

    public:
        SafeString& key()
        {
            return mKey_;
        }

        Value& value()
        {
            return mValue;
        }

    protected:
        Value mValue;
        StrTreeMap* mMap;
        char mKeyStr[KeyStrN + 1];
    };

    template <typename T>
    struct ForEachConstContext
    {
    public:
        ForEachConstContext(const T& afun)
            : fun(afun)
        {
        }

        void call(TreeMapNode<SafeString>* n)
        {
            Node* node = static_cast<Node*>(n);
            fun(node->key(), node->value());
        }

    private:
        const T& fun;
    };

public:
    StrTreeMap()
        : TreeMapImpl<SafeString>()
        , mFreeList()
        , mSize(0)
        , mNodeMax(0)
    {
    }

    void allocBuffer(s32 nodeMax, s32 alignment = cDefaultAlignment)
    {
        if (!tryAllocBuffer(nodeMax, alignment))
        {
            AllocFailAssert(nullptr, nodeMax * sizeof(Node), alignment);
        }
    }

    void allocBuffer(s32 nodeMax, Heap* heap, s32 alignment = cDefaultAlignment)
    {
        if (!tryAllocBuffer(nodeMax, heap, alignment))
        {
            AllocFailAssert(heap, nodeMax * sizeof(Node), alignment);
        }
    }

    bool tryAllocBuffer(s32 nodeMax, s32 alignment = cDefaultAlignment)
    {
        SEAD_ASSERT(mFreeList.work() == nullptr);

        if (nodeMax <= 0)
        {
            SEAD_ASSERT_MSG(false, "nodeMax[%d] must be larger than zero", nodeMax);
            return false;
        }

        void* buf = new(alignment, std::nothrow) u8[nodeMax * sizeof(Node)];
        if (!buf)
        {
            return false;
        }

        setBuffer(nodeMax, buf);
        return true;
    }

    bool tryAllocBuffer(s32 nodeMax, Heap* heap, s32 alignment = cDefaultAlignment)
    {
        SEAD_ASSERT(mFreeList.work() == nullptr);

        if (nodeMax <= 0)
        {
            SEAD_ASSERT_MSG(false, "nodeMax[%d] must be larger than zero", nodeMax);
            return false;
        }

        void* buf = new(heap, alignment, std::nothrow) u8[nodeMax * sizeof(Node)];
        if (!buf)
        {
            return false;
        }

        setBuffer(nodeMax, buf);
        return true;
    }

    void freeBuffer()
    {
        if (isBufferReady())
        {
            clear();

            delete[] mFreeList.work();

            mNodeMax = 0;
            mFreeList.cleanup();
        }
    }

    void setBuffer(s32 nodeMax, void* buf)
    {
        if (nodeMax <= 0)
        {
            SEAD_ASSERT_MSG(false, "nodeMax[%d] must be larger than zero", nodeMax);
            return;
        }

        if (!buf)
        {
            SEAD_ASSERT_MSG(false, "buf is null");
            return;
        }

        mNodeMax = nodeMax;
        mFreeList.init(buf, sizeof(Node), nodeMax);
    }

    bool isBufferReady() const { return mFreeList.work() != nullptr; }
    bool isEmpty() const { return mSize == 0; }
    bool isFull() const { return mSize >= mNodeMax; }

    s32 size() const { return mSize; }
    s32 getSize() const { return mSize; }
    s32 maxSize() const { return mNodeMax; }

    Value* find(const SafeString& key) const
    {
        Node* node = static_cast<Node*>(TreeMapImpl<SafeString>::find(key));
        if (!node)
        {
            return nullptr;
        }

        return &node->value();
    }

    bool contains(const SafeString& key) const
    {
        return find(key) != nullptr;
    }

    Value* insert(const SafeString& key)
    {
        if (isFull())
        {
            Value* insertedValue = find(key);
            if (!insertedValue)
            {
                SEAD_ASSERT_MSG(false, "map is full.");
                return nullptr;
            }

            insertedValue->~Value();
            new(insertedValue) Value();

            return insertedValue;
        }

        Node* node = new(mFreeList.get()) Node(key, this);
        mSize++;
        TreeMapImpl<SafeString>::insert(node);

        return &node->value();
    }

    Value* insert(const SafeString& key, const Value& value)
    {
        if (isFull())
        {
            Value* insertedValue = find(key);
            if (!insertedValue)
            {
                SEAD_ASSERT_MSG(false, "map is full.");
                return nullptr;
            }

            insertedValue->~Value();
            new(insertedValue) Value(value);

            return insertedValue;
        }

        Node* node = new(mFreeList.get()) Node(key, value, this);
        mSize++;
        TreeMapImpl<SafeString>::insert(node);

        return &node->value();
    }

    void clear()
    {
        TreeMapImpl<SafeString>::forEach(
            DelegateCreator<StrTreeMap, TreeMapNode<SafeString>*>(
                this,
                &StrTreeMap::eraseNodeForClear_
            )
        );

        mSize = 0;
        mRoot = nullptr;
    }

    template <typename T>
    void forEach(const T& fun) const
    {
        ForEachConstContext<T> context(fun);

        TreeMapImpl<SafeString>::forEach(
            DelegateCreator<ForEachConstContext<T>, TreeMapNode<SafeString>*>(
                &context,
                &ForEachConstContext<T>::call
            )
        );
    }

protected:
    void eraseNode_(Node* node)
    {
        mFreeList.put(node);
        mSize--;
    }

    void eraseNodeForClear_(TreeMapNode<SafeString>* n)
    {
        Node* node = static_cast<Node*>(n);
        node->value().~Value();

        mFreeList.put(node);
    }

protected:
    FreeList mFreeList;
    s32 mSize;
    s32 mNodeMax;
};

template <s32 KeyStrN, typename Value, s32 N>
class FixedStrTreeMap : public StrTreeMap<KeyStrN, Value>
{
public:
    FixedStrTreeMap()
        : StrTreeMap<KeyStrN, Value>()
    {
        StrTreeMap<KeyStrN, Value>::setBuffer(N, mWork);
    }

    void allocBuffer(s32 nodeMax, s32 alignment = cDefaultAlignment) = delete;
    void allocBuffer(s32 nodeMax, Heap* heap, s32 alignment = cDefaultAlignment) = delete;
    bool tryAllocBuffer(s32 nodeMax, s32 alignment = cDefaultAlignment) = delete;
    bool tryAllocBuffer(s32 nodeMax, Heap* heap, s32 alignment = cDefaultAlignment) = delete;
    void freeBuffer() = delete;
    void setBuffer(s32 nodeMax, void* buf) = delete;

protected:
    u8 mWork[N * sizeof(StrTreeMap<KeyStrN, Value>::Node)];
};

} // namespace sead
