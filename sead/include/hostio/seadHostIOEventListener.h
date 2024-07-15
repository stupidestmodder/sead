#pragma once

#include <heap/seadDisposer.h>

#include <type_traits> //? For std::aligned_storage_t

namespace sead { namespace hostio {

struct NodeEvent;
struct PropertyEvent;

class LifeCheckable
{
#ifdef SEAD_DEBUG
    SEAD_NO_COPY(LifeCheckable);

private:
    class DisposeHostIOCaller : public IDisposer
    {
    public:
        explicit DisposeHostIOCaller(LifeCheckable* instance)
            : IDisposer()
            , mInstance(instance)
        {
        }

        DisposeHostIOCaller(LifeCheckable* instance, Heap* disposerHeap, IDisposer::HeapNullOption option)
            : IDisposer(disposerHeap, option)
            , mInstance(instance)
        {
        }

        ~DisposeHostIOCaller() override;

        void clearInstance()
        {
            mInstance = nullptr;
        }

        bool isValidInstance() const
        {
            return mInstance != nullptr;
        }

    private:
        LifeCheckable* mInstance;
    };

    static u32 sCurrentCreateID;
    static LifeCheckable* sTopInstance;

public:
    LifeCheckable()
        : mCreateID(0)
        , mPrev(nullptr)
        , mNext(nullptr)
        , mDisposeCaller(nullptr)
    {
        static_assert(sizeof(mDisposeCallerBuf) >= sizeof(DisposeHostIOCaller));
        static_assert(alignof(decltype(mDisposeCallerBuf)) % alignof(DisposeHostIOCaller) == 0);
        mDisposeCaller = new(&mDisposeCallerBuf) DisposeHostIOCaller(this);

        initialize_();
    }

    LifeCheckable(Heap* disposerHeap, IDisposer::HeapNullOption option)
        : mCreateID(0)
        , mPrev(nullptr)
        , mNext(nullptr)
        , mDisposeCaller(nullptr)
    {
        static_assert(sizeof(mDisposeCallerBuf) >= sizeof(DisposeHostIOCaller));
        static_assert(alignof(decltype(mDisposeCallerBuf)) % alignof(DisposeHostIOCaller) == 0);
        mDisposeCaller = new(&mDisposeCallerBuf) DisposeHostIOCaller(this, disposerHeap, option);

        initialize_();
    }

    virtual ~LifeCheckable()
    {
        if (mDisposeCaller && mDisposeCaller->isValidInstance())
        {
            mDisposeCaller->clearInstance();
            mDisposeCaller->~DisposeHostIOCaller();
            disposeHostIOImpl_();
        }

        mDisposeCaller = nullptr;
    }

    virtual void disposeHostIO()
    {
        disposeHostIOImpl_();
    }

    u32 getCreateID() const
    {
        return mCreateID;
    }

    static LifeCheckable* searchInstanceFromCreateID(u32 createID);

private:
    void initialize_();
    void disposeHostIOImpl_();

private:
    u32 mCreateID;
    LifeCheckable* mPrev;
    LifeCheckable* mNext;
    DisposeHostIOCaller* mDisposeCaller;
    std::aligned_storage_t<sizeof(DisposeHostIOCaller), alignof(DisposeHostIOCaller)> mDisposeCallerBuf;
#endif // SEAD_DEBUG
};

class PropertyEventListener : public LifeCheckable
{
#ifdef SEAD_DEBUG
public:
    PropertyEventListener()
        : LifeCheckable()
    {
    }

    PropertyEventListener(Heap* disposerHeap, IDisposer::HeapNullOption option)
        : LifeCheckable(disposerHeap, option)
    {
    }

    virtual void listenPropertyEvent(const PropertyEvent* event) = 0;
#endif // SEAD_DEBUG
};

class NodeEventListener : public PropertyEventListener
{
#ifdef SEAD_DEBUG
public:
    NodeEventListener()
        : PropertyEventListener()
    {
    }

    NodeEventListener(Heap* disposerHeap, IDisposer::HeapNullOption option)
        : PropertyEventListener(disposerHeap, option)
    {
    }

    void listenPropertyEvent(const PropertyEvent* event) override
    {
        SEAD_UNUSED(event);
    }

    virtual void listenNodeEvent(const NodeEvent* event) = 0;
#endif // SEAD_DEBUG
};

// TODO
class PaletteEventListener : public LifeCheckable
{
public:
};

} } // namespace sead::hostio
