#pragma once

#include <container/seadTList.h>
#include <heap/seadDisposer.h>
#include <prim/seadDelegate.h>

namespace sead {

template <typename TArg>
class DelegateEvent
{
public:
    class Slot;

private:
    using SlotList = TList<Slot*>;
    using SlotListNode = TListNode<Slot*>;

public:
    class Slot : public IDisposer
    {
    public:
        Slot()
            : IDisposer()
            , mNode(this)
            , mDelegate()
            , mConnectedToDelegateEvent(false)
        {
        }

        template <typename UObject>
        Slot(UObject* obj, typename Delegate1<UObject, TArg>::MethodPtr method)
            : IDisposer()
            , mNode(this)
            , mDelegate()
            , mConnectedToDelegateEvent(false)
        {
            mDelegate.bind(obj, method);
        }

        Slot(typename Delegate1<void, TArg>::MethodPtr method)
            : IDisposer()
            , mNode(this)
            , mDelegate()
            , mConnectedToDelegateEvent(false)
        {
            mDelegate.bind(method);
        }

        ~Slot() override
        {
            release();
        }

        template <typename UObject>
        void bind(UObject* obj, typename Delegate1<UObject, TArg>::MethodPtr method)
        {
            mDelegate.bind(obj, method);
        }

        void bind(typename Delegate1<void, TArg>::MethodPtr method)
        {
            mDelegate.bind(method);
        }

        void release()
        {
            if (mConnectedToDelegateEvent)
            {
                mNode.erase();
                mConnectedToDelegateEvent = false;
            }
        }

        bool isConnectedToDelegateEvent() const
        {
            return mConnectedToDelegateEvent;
        }

    protected:
        void invoke_(TArg arg)
        {
            mDelegate.invoke(arg);
        }

    protected:
        SlotListNode mNode;
        AnyDelegate1<TArg> mDelegate;
        bool mConnectedToDelegateEvent;

        friend class DelegateEvent;
    };

public:
    DelegateEvent()
        : mList()
    {
    }

    virtual ~DelegateEvent()
    {
        for (auto it = mList.begin(); it != mList.end(); )
        {
            Slot* s = *it;
            ++it;
            s->release();
        }
    }

    void connect(Slot& slot)
    {
        slot.release();
        mList.pushBack(&slot.mNode);
        slot.mConnectedToDelegateEvent = true;
    }

    void disconnect(Slot& slot)
    {
        slot.release();
    }

    DelegateEvent& operator+=(Slot& slot)
    {
        connect(slot);
        return *this;
    }

    DelegateEvent& operator-=(Slot& slot)
    {
        disconnect(slot);
        return *this;
    }

    void fire(TArg arg)
    {
        for (Slot*& slot : mList)
            slot->invoke_(arg);
    }

    s32 getSlotLength() const
    {
        return mList.size();
    }

protected:
    SlotList mList;
};

} // namespace sead
