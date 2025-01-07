#pragma once

#include <container/seadFreeList.h>
#include <container/seadTreeMapImpl.h>

namespace sead {

template <typename Key>
struct TreeMapKeyImpl
{
    TreeMapKeyImpl()
        : key()
    {
    }

    TreeMapKeyImpl(const Key& akey)
        : key(akey)
    {
    }

    TreeMapKeyImpl& operator=(const Key& akey)
    {
        key = akey;
        return *this;
    }

    s32 compare(const TreeMapKeyImpl& rhs) const
    {
        if (key < rhs.key)
        {
            return -1;
        }

        if (rhs.key < key)
        {
            return 1;
        }

        return 0;
    }

    Key key;
};

template <typename Key, typename Value>
class TreeMap : public TreeMapImpl<TreeMapKeyImpl<Key>>
{
public:
    class Node : public TreeMapNode<TreeMapKeyImpl<Key>>
    {
    public:
        Node(const Key& akey, TreeMap* map)
            : TreeMapNode<TreeMapKeyImpl<Key>>()
            , mValue()
            , mMap(map)
        {
            this->mKey_ = akey;
        }

        Node(const Key& akey, const Value& avalue, TreeMap* map)
            : TreeMapNode<TreeMapKeyImpl<Key>>()
            , mValue(avalue)
            , mMap(map)
        {
            this->mKey_ = akey;
        }

    protected:
        void erase_() override
        {
            mValue.~Value();

            mMap->eraseNode_(this);
        }

    public:
        Key& key()
        {
            return this->mKey_.key;
        }

        Value& value()
        {
            return mValue;
        }

    protected:
        Value mValue;
        TreeMap* mMap;
    };

    template <typename T>
    struct ForEachConstContext
    {
        ForEachConstContext(const T& afun)
            : fun(afun)
        {
        }

        void call(TreeMapNode<TreeMapKeyImpl<Key>>* n)
        {
            Node* node = static_cast<Node*>(n);
            fun(node->key(), node->value());
        }

    private:
        const T& fun;
    };

public:
    TreeMap()
        : TreeMapImpl<TreeMapKeyImpl<Key>>()
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

    Value* find(const Key& key) const
    {
        Node* node = static_cast<Node*>(TreeMapImpl<TreeMapKeyImpl<Key>>::find(key));
        if (!node)
        {
            return nullptr;
        }

        return &node->value();
    }

    bool contains(const Key& key) const
    {
        return find(key) != nullptr;
    }

    Value* insert(const Key& key)
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
        TreeMapImpl<TreeMapKeyImpl<Key>>::insert(node);

        return &node->value();
    }

    Value* insert(const Key& key, const Value& value)
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
        TreeMapImpl<TreeMapKeyImpl<Key>>::insert(node);

        return &node->value();
    }

    void clear()
    {
        TreeMapImpl<TreeMapKeyImpl<Key>>::forEach(
            DelegateCreator<TreeMap, TreeMapNode<TreeMapKeyImpl<Key>>*>(
                this,
                &TreeMap::eraseNodeForClear_
            )
        );

        mSize = 0;
        this->mRoot = nullptr;
    }

    template <typename T>
    void forEach(const T& fun) const
    {
        ForEachConstContext<T> context(fun);

        TreeMapImpl<TreeMapKeyImpl<Key>>::forEach(
            DelegateCreator<ForEachConstContext<T>, TreeMapNode<TreeMapKeyImpl<Key>>*>(
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

    void eraseNodeForClear_(TreeMapNode<TreeMapKeyImpl<Key>>* n)
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

template <typename Key, typename Value, s32 N>
class FixedTreeMap : public TreeMap<Key, Value>
{
public:
    FixedTreeMap()
        : TreeMap<Key, Value>()
    {
        TreeMap<Key, Value>::setBuffer(N, mWork);
    }

    void allocBuffer(s32 nodeMax, s32 alignment = cDefaultAlignment) = delete;
    void allocBuffer(s32 nodeMax, Heap* heap, s32 alignment = cDefaultAlignment) = delete;
    bool tryAllocBuffer(s32 nodeMax, s32 alignment = cDefaultAlignment) = delete;
    bool tryAllocBuffer(s32 nodeMax, Heap* heap, s32 alignment = cDefaultAlignment) = delete;
    void freeBuffer() = delete;
    void setBuffer(s32 nodeMax, void* buf) = delete;

protected:
    u8 mWork[N * sizeof(TreeMap<Key, Value>::Node)];
};

} // namespace sead
