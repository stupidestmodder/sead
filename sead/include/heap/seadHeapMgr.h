#pragma once

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
    struct AllocCallbackArg;
    struct AllocFailedCallbackArg;
    struct FreeCallbackArg;
    struct CreateCallbackArg;
    struct DestroyCallbackArg;

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

    Heap* findContainHeap(const void* ptr) const;
    Heap* findHeapByName(const SafeString&, s32) const;
    Heap* getCurrentHeap() const;

    static Heap* getRootHeap(s32 idx);
    static s32 getRootHeapNum() { return sRootHeaps.size(); }
    static void addRootHeap(Heap* heap);
    static const Arena* getArena();
    static IndependentHeaps* getIndependentHeaps();
    static bool isContainedInAnyHeap(const void* addr);
    static void dumpTreeYAML(WriteStream&);
    static u32 getHeapCheckTag() { return sHeapCheckTag.increment(); }
    static u32 peekHeapCheckTag();

    void setDebugFillHeapCreate(u8);
    void setDebugFillAlloc(u8);
    void setDebugFillFree(u8);
    void setDebugFillHeapDestroy(u8);

    u8 getDebugFillHeapCreate() const;
    u8 getDebugFillAlloc() const;
    u8 getDebugFillFree() const;
    u8 getDebugFillHeapDestroy() const;

    void setEnableDebugFillHeapCreate(bool);
    bool isEnableDebugFillHeapCreate();
    void setEnableDebugFillAlloc(bool);
    bool isEnableDebugFillAlloc();
    void setEnableDebugFillFree(bool);
    bool isEnableDebugFillFree();
    void setEnableDebugFillHeapDestroy(bool);
    bool isEnableDebugFillHeapDestroy();

    IAllocCallback* setAllocCallback(IAllocCallback*);
    IAllocCallback* getAllocCallback();
    IAllocFailedCallback* setAllocFailedCallback(IAllocFailedCallback*);
    IAllocFailedCallback* getAllocFailedCallback();
    IFreeCallback* setFreeCallback(IFreeCallback*);
    IFreeCallback* getFreeCallback();
    ICreateCallback* setCreateCallback(ICreateCallback*);
    ICreateCallback* getCreateCallback();
    IDestroyCallback* setDestroyCallback(IDestroyCallback*);
    IDestroyCallback* getDestroyCallback();

    void callCreateCallback_(Heap*);
    void callDestroyCallback_(Heap*);
    void callFreeCallback_(const FreeCallbackArg&);

    static CriticalSection* getHeapTreeLockCS_() { return &sHeapTreeLockCS; }

    static void removeFromFindContainHeapCache_(Heap* heap);

protected:
    Heap* setCurrentHeap_(Heap* heap);

    friend class CurrentHeapSetter;

    static void createRootHeap_();
    static void initializeImpl_();
    static Heap* findHeapByName_(Heap*, const SafeString&, s32*);

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
    IAllocFailedCallback* mAllocFailedCallback;
};

class CurrentHeapSetter
{
public:
    CurrentHeapSetter(Heap* heap)
        : mPrevHeap(nullptr)
    {
        if (!heap)
            mPrevHeap = reinterpret_cast<Heap*>(1);
        else
            mPrevHeap = HeapMgr::instance()->setCurrentHeap_(heap);
    }

    ~CurrentHeapSetter()
    {
        if (mPrevHeap != reinterpret_cast<Heap*>(1))
            HeapMgr::instance()->setCurrentHeap_(mPrevHeap);
    }

private:
    Heap* mPrevHeap;
};

// TODO
struct FindContainHeapCache
{
private:
#ifdef SEAD_PLATFORM_WINDOWS
    using AtomicPtrValue = AtomicU64;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS

    friend class FindContainHeapCacheAccessor;

public:
    FindContainHeapCache();

    void setHeap(Heap* targetHeap);
    Heap* getHeap() const;
    bool tryRemoveHeap(Heap* targetHeap);

#ifdef SEAD_DEBUG
    void dumpStatistics();
    void clearStatistics();
#endif // SEAD_DEBUG

    AtomicPtrValue heap;
};

class FindContainHeapCacheAccessor
{
public:
    FindContainHeapCacheAccessor(FindContainHeapCache::AtomicPtrValue* atomic)
        : mAtomicValue(atomic)
        , mHeap(reinterpret_cast<Heap*>(atomic->orMask(1)))
    {
    }

    ~FindContainHeapCacheAccessor()
    {
        mAtomicValue->andMask(~1U);
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
