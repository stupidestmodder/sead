#pragma once

#include <basis/seadRawPrint.h>
#include <container/seadPtrArray.h>
#include <heap/seadArena.h>
#include <prim/seadDelegate.h>
#include <prim/seadSafeString.h>
#include <thread/seadAtomic.h>
#include <thread/seadCriticalSection.h>

#define NUM_ROOT_HEAPS_MAX        4
#define NUM_INDEPENDENT_HEAPS_MAX 4

namespace sead {

class WriteStream;

// TODO
class HeapMgr
{
public:
    struct AllocCallbackArg
    {
        Heap* heap;
        void* ptr;
        size_t request_size;
        s32 request_alignment;
        size_t alloc_size;
        s32 alloc_alignment;
    };

    struct AllocFailedCallbackArg
    {
        Heap* heap;
        size_t request_size;
        s32 request_alignment;
        size_t alloc_size;
        s32 alloc_alignment;
    };

    struct FreeCallbackArg
    {
        Heap* heap;
        void* ptr;
    };

    struct CreateCallbackArg
    {
        Heap* heap;
    };

    struct DestroyCallbackArg
    {
        Heap* heap;
    };

    static const u8 cDefaultDebugFillHeapDestroy = 0xAD;
    static const u8 cDefaultDebugFillAlloc = 0xCD;
    static const u8 cDefaultDebugFillFree = 0xDD;
    static const u8 cDefaultDebugFillHeapCreate = 0xFD;

private:
    using IAllocCallback = IDelegate1<const AllocCallbackArg*>;
    using IAllocFailedCallback = IDelegate1<const AllocFailedCallbackArg*>;
    using IFreeCallback = IDelegate1<const FreeCallbackArg*>;
    using ICreateCallback = IDelegate1<const CreateCallbackArg*>;
    using IDestroyCallback = IDelegate1<const DestroyCallbackArg*>;

    using RootHeaps        = FixedPtrArray<Heap, NUM_ROOT_HEAPS_MAX>;
    using IndependentHeaps = FixedPtrArray<Heap, NUM_INDEPENDENT_HEAPS_MAX>;

public:
    HeapMgr();
    virtual ~HeapMgr();

    static void initialize(size_t size);
    static void initialize(Arena* arena);
    static void destroy();

    static bool isInitialized() { return sInstancePtr != nullptr; }
    static HeapMgr* instance() { return sInstancePtr; }

    Heap* findContainHeap(const void* memBlock) const;
    Heap* findHeapByName(const SafeString& name, s32 index) const;
    Heap* getCurrentHeap() const;

    static Heap* getRootHeap(s32 idx) { return sRootHeaps.at(idx); }
    static s32 getRootHeapNum() { return sRootHeaps.size(); }
    static void addRootHeap(Heap* heap) { sRootHeaps.pushBack(heap); }
    static void removeRootHeap(Heap* heap);
    static const Arena* getArena() { return sArena; }
    static IndependentHeaps* getIndependentHeaps() { return &sIndependentHeaps; }
    static bool isContainedInAnyHeap(const void* addr);
    static void dumpTreeYAML(WriteStream& stream);
    static u32 getHeapCheckTag() { return sHeapCheckTag.increment(); }
    static u32 peekHeapCheckTag() { return sHeapCheckTag.getValue(); }

    void setDebugFillHeapCreate(u8);
    void setDebugFillAlloc(u8);
    void setDebugFillFree(u8);
    void setDebugFillHeapDestroy(u8);

    u8 getDebugFillHeapCreate() const { return mDebugFillHeapCreate; }
    u8 getDebugFillAlloc() const { return mDebugFillAlloc; }
    u8 getDebugFillFree() const { return mDebugFillFree; }
    u8 getDebugFillHeapDestroy() const { return mDebugFillHeapDestroy; }

    void setEnableDebugFillHeapCreate(bool);
    bool isEnableDebugFillHeapCreate() { return mIsEnableDebugFillHeapCreate; }
    void setEnableDebugFillAlloc(bool);
    bool isEnableDebugFillAlloc() { return mIsEnableDebugFillAlloc; }
    void setEnableDebugFillFree(bool);
    bool isEnableDebugFillFree() { return mIsEnableDebugFillFree; }
    void setEnableDebugFillHeapDestroy(bool);
    bool isEnableDebugFillHeapDestroy() { return mIsEnableDebugFillHeapDestroy; }

    IAllocCallback* setAllocCallback(IAllocCallback* callback);
    IAllocCallback* getAllocCallback() { return mAllocCallback; }

    IAllocFailedCallback* setAllocFailedCallback(IAllocFailedCallback* callback);
    IAllocFailedCallback* getAllocFailedCallback() { return mAllocFailedCallback; }

    IFreeCallback* setFreeCallback(IFreeCallback* callback);
    IFreeCallback* getFreeCallback() { return mFreeCallback; }

    ICreateCallback* setCreateCallback(ICreateCallback* callback);
    ICreateCallback* getCreateCallback() { return mCreateCallback; }

    IDestroyCallback* setDestroyCallback(IDestroyCallback* callback);
    IDestroyCallback* getDestroyCallback() { return mDestroyCallback; }

    void callCreateCallback_(Heap* heap)
    {
        SEAD_ASSERT(heap);

        if (!mCreateCallback)
            return;

        CreateCallbackArg arg;
        arg.heap = heap;

        mCreateCallback->invoke(&arg);
    }

    void callDestroyCallback_(Heap* heap)
    {
        SEAD_ASSERT(heap);

        if (!mDestroyCallback)
            return;

        DestroyCallbackArg arg;
        arg.heap = heap;

        mDestroyCallback->invoke(&arg);
    }

    void callFreeCallback_(const FreeCallbackArg& arg)
    {
        if (!mFreeCallback)
            return;

        mFreeCallback->invoke(&arg);
    }

    static CriticalSection* getHeapTreeLockCS_() { return &sHeapTreeLockCS; }

    static void removeFromFindContainHeapCache_(Heap* heap);

#ifdef SEAD_DEBUG
    static void dumpFindContainHeapCacheStatistics();
    static void clearFindContainHeapCacheStatistics();
#endif // SEAD_DEBUG

protected:
    Heap* setCurrentHeap_(Heap* heap);

    friend class CurrentHeapSetter;

    static void createRootHeap_();
    static void initializeImpl_();
    static Heap* findHeapByName_(Heap* heap, const SafeString& name, s32* index);

protected:
    static HeapMgr sInstance;
    static HeapMgr* sInstancePtr;
    static Arena* sArena;
    static Arena sDefaultArena;
    static AtomicU32 sHeapCheckTag;
    static RootHeaps sRootHeaps;
    static CriticalSection sHeapTreeLockCS;
    static IndependentHeaps sIndependentHeaps;

protected:
#ifdef SEAD_DEBUG
    u8 mDebugFillHeapCreate;
    u8 mDebugFillAlloc;
    u8 mDebugFillFree;
    u8 mDebugFillHeapDestroy;
    bool mIsEnableDebugFillHeapCreate;
    bool mIsEnableDebugFillAlloc;
    bool mIsEnableDebugFillFree;
    bool mIsEnableDebugFillHeapDestroy;
    IAllocCallback* mAllocCallback;
    IAllocFailedCallback* mAllocFailedCallback;
    IFreeCallback* mFreeCallback;
    ICreateCallback* mCreateCallback;
    IDestroyCallback* mDestroyCallback;
#else
    IAllocFailedCallback* mAllocFailedCallback;
#endif // SEAD_DEBUG
};

class CurrentHeapSetter
{
public:
    explicit CurrentHeapSetter(Heap* heap)
        : mPrevHeap(nullptr)
    {
        if (heap)
            mPrevHeap = HeapMgr::instance()->setCurrentHeap_(heap);
        else
            mPrevHeap = reinterpret_cast<Heap*>(1);
    }

    ~CurrentHeapSetter()
    {
        if (mPrevHeap != reinterpret_cast<Heap*>(1))
            HeapMgr::instance()->setCurrentHeap_(mPrevHeap);
    }

private:
    Heap* mPrevHeap;
};

struct FindContainHeapCache
{
private:
    using AtomicPtrValue = AtomicBase32<uintptr_t>;

    friend class FindContainHeapCacheAccessor;

public:
    FindContainHeapCache()
        : heap(0)
#ifdef SEAD_DEBUG
        , miss(0)
        , hit(0)
        , nolockhit(0)
        , notfound(0)
        , call(0)
        , sleep(0)
#endif // SEAD_DEBUG
    {
    }

    void setHeap(Heap* targetHeap)
    {
        heap.setValue(reinterpret_cast<uintptr_t>(targetHeap));
    }

    Heap* getHeap() const
    {
        return reinterpret_cast<Heap*>(heap.getValue());
    }

    bool tryRemoveHeap(Heap* targetHeap)
    {
        uintptr_t value;
        if (heap.compareAndSwapAndGetOriginal(reinterpret_cast<uintptr_t>(targetHeap), 0, &value))
            return true;

        if (reinterpret_cast<Heap*>(value & ~static_cast<uintptr_t>(1)) == targetHeap)
            return false;

        return true;
    }

#ifdef SEAD_DEBUG
    void dumpStatistics()
    {
        SEAD_PRINT("Miss:%d, Hit:%d, NoLockHit:%d, NotFound:%d, Call:%d, Sleep:%d\n",
                    miss, hit, nolockhit, notfound, call, sleep);
    }

    void clearStatistics()
    {
        miss = 0;
        hit = 0;
        nolockhit = 0;
        notfound = 0;
        call = 0;
        sleep = 0;
    }
#endif // SEAD_DEBUG

    AtomicPtrValue heap;
#ifdef SEAD_DEBUG
    s32 miss;
    s32 hit;
    s32 nolockhit;
    s32 notfound;
    s32 call;
    s32 sleep;
#endif // SEAD_DEBUG
};

class FindContainHeapCacheAccessor
{
public:
    explicit FindContainHeapCacheAccessor(FindContainHeapCache::AtomicPtrValue* atomic)
        : mAtomicValue(atomic)
        , mHeap(reinterpret_cast<Heap*>(atomic->orMask(static_cast<uintptr_t>(1))))
    {
    }

    ~FindContainHeapCacheAccessor()
    {
        mAtomicValue->andMask(~static_cast<uintptr_t>(1));
    }

    Heap* getHeap()
    {
        return mHeap;
    }

private:
    FindContainHeapCache::AtomicPtrValue* mAtomicValue;
    Heap* mHeap;
};

} // namespace sead

#undef NUM_ROOT_HEAPS_MAX
#undef NUM_INDEPENDENT_HEAPS_MAX
