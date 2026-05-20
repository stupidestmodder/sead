#pragma once

#include <container/seadTreeNode.h>
#include <heap/seadDisposer.h>
#include <prim/seadBitFlag.h>
#include <prim/seadDelegate.h>
#include <prim/seadNamable.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class CriticalSection;

class MethodTreeNode : public TTreeNode<MethodTreeNode*>, public INamable, public IDisposer
{
    SEAD_RTTI_BASE(MethodTreeNode);

public:
    // TODO
    struct Condition
    {
    };

    enum PauseFlag
    {
        eNone  = 0,
        eSelf  = 1 << 0,
        eChild = 1 << 1,
        eBoth  = eSelf | eChild
    };

    using PauseEventDelegate = IDelegate2<MethodTreeNode*, PauseFlag>;

public:
    explicit MethodTreeNode(CriticalSection* cs)
        : TTreeNode(this)
        , INamable()
        , IDisposer()
        , mDelegate()
        , mCriticalSection(cs)
        , mPriority(0)
        , mPauseFlag(eNone)
        , mPauseEventDelegate(nullptr)
        , mUserID(nullptr)
    {
    }

    ~MethodTreeNode() override
    {
        detachAll();
    }

    template <typename T>
    void bind(T* object, typename Delegate<T>::MethodPtr method, const char* name)
    {
        lock_();
        mDelegate.bind(object, method);
        unlock_();

        if (name)
            setName(name);
    }

    void call();
    void detachAll();
    void pushBackChild(MethodTreeNode* o);
    void pushFrontChild(MethodTreeNode* o);

    // TODO
    MethodTreeNode* find(Condition& condition);

    void setPauseFlag(PauseFlag f)
    {
        lock_();
        if (mPauseEventDelegate)
            mPauseEventDelegate->invoke(this, f);

        mPauseFlag.setDirect(f);
        unlock_();
    }

    //* Uncertain but probably exists
    void setPauseEvent(PauseEventDelegate* event)
    {
        lock_();
        mPauseEventDelegate = event;
        unlock_();
    }

    //* Uncertain but probably exists
    void setUserID(void* id)
    {
        lock_();
        mUserID = id;
        unlock_();
    }

    //* Uncertain but probably exists
    void* getUserID() const
    {
        return mUserID;
    }

    MethodTreeNode* getParent()
    {
        return parent() ? parent()->val() : nullptr;
    }

    MethodTreeNode* getParent() const
    {
        return parent() ? parent()->val() : nullptr;
    }

    MethodTreeNode* getChild()
    {
        return child() ? child()->val() : nullptr;
    }

    MethodTreeNode* getChild() const
    {
        return child() ? child()->val() : nullptr;
    }

    MethodTreeNode* getNext()
    {
        return next() ? next()->val() : nullptr;
    }

    MethodTreeNode* getNext() const
    {
        return next() ? next()->val() : nullptr;
    }

    MethodTreeNode* getPrev()
    {
        return prev() ? prev()->val() : nullptr;
    }

    MethodTreeNode* getPrev() const
    {
        return prev() ? prev()->val() : nullptr;
    }

protected:
    void callRec_();
    void attachMutexRec_(CriticalSection* m) const;

    void lock_();
    void unlock_();

protected:
    AnyDelegate mDelegate;
    mutable CriticalSection* mCriticalSection;
    u32 mPriority; //! Unused
    BitFlag32 mPauseFlag;
    PauseEventDelegate* mPauseEventDelegate;
    void* mUserID;
};

} // namespace sead
