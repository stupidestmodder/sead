#pragma once

#include <container/seadTList.h>
#include <heap/seadDisposer.h>
#include <heap/seadHeapMgr.h>
#include <prim/seadNamable.h>
#include <prim/seadScopedLock.h>
#include <thread/seadCriticalSection.h>
#include <thread/seadMessageQueue.h>
#include <thread/seadThreadLocalStorage.h>

namespace sead {

class Thread;

using ThreadList = TList<Thread*>;
using ThreadListNode = TListNode<Thread*>;

// TODO
class Thread : public IDisposer, public INamable
{
    SEAD_NO_COPY(Thread);

public:
    // TODO
    enum class State
    {
    };

    static const s32 cDefaultSeadPriority;
    static const s32 cDefaultPriority;

public:
    Thread(const SafeString& name, Heap* heap, s32, MessageQueue::BlockType, MessageQueue::Element, s32, s32);

protected:
#ifdef SEAD_PLATFORM_WINDOWS
    // TODO
    Thread(Heap* heap);
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS

public:
    ~Thread() override;

    virtual void destroy();
    virtual bool sendMessage(MessageQueue::Element, MessageQueue::BlockType);
    virtual MessageQueue::Element recvMessage(MessageQueue::BlockType);
    virtual const MessageQueue& getMessageQueue() const;
    virtual bool start();
    virtual void quit(bool);

    bool isDone() const;

    virtual void waitDone();
    virtual void quitAndDestroySingleThread(bool);
    virtual void quitAndWaitDoneSingleThread(bool);

    static void sleep(TickSpan span);
    static void yield();

    u32 getID() const;
    Heap* getCurrentHeap() const { return mCurrentHeap; }

    virtual void setPriority(s32);
    virtual s32 getPriority() const;
    virtual MessageQueue::BlockType getBlockType() const;
    virtual s32 getStackSize() const;
    virtual s32 calcStackUsedSizePeak() const;
    void checkStackOverFlow(const char*, s32) const;
    void checkStackEndCorruption(const char*, s32) const;
    void checkStackPointerOverFlow(const char*, s32) const;

    bool isQuitting() const;
    void setStackOverflowExceptionEnable(bool);

protected:
    ThreadListNode* getListNode() { return &mListNode; }
    void setCurrentHeap_(Heap* heap) { mCurrentHeap = heap; }
    FindContainHeapCache* getFindContainHeapCache() { return &mFindContainHeapCache; }
    const FindContainHeapCache* getFindContainHeapCache() const { return &mFindContainHeapCache; }

    void run_();
    virtual void calc_(MessageQueue::Element);

    void initStackCheck_();
    void initStackCheckWithCurrentStackPointer_();
    virtual u32* getStackCheckStartAddress_() const;

#ifdef SEAD_PLATFORM_WINDOWS
    static void* winThreadFunc_(void*);
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS

    friend class ThreadMgr;
    friend class HeapMgr;

public:
    uintptr_t getStackBase() const;

protected:
    MessageQueue mMessageQueue;
    s32 mStackSize;
    ThreadListNode mListNode;
    Heap* mCurrentHeap;
    FindContainHeapCache mFindContainHeapCache;
    MessageQueue::BlockType mBlockType;
    MessageQueue::Element mQuitMsg;
    u32 mID;
    State mState;
#ifdef SEAD_PLATFORM_WINDOWS

#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS

};

// TODO
class ThreadMgr
{
    SEAD_SINGLETON_DISPOSER(ThreadMgr);

public:
    ThreadMgr();
    ~ThreadMgr();

    void initialize(Heap* heap);
    void destroy();

    Thread* getCurrentThread() const { return reinterpret_cast<Thread*>(mThreadPtrTLS.getValue()); }
    Thread* getMainThread() const { return mMainThread; }
    bool isMainThread() const { return getCurrentThread() == mMainThread; }

    ThreadList::constIterator constBegin() const { return mList.constBegin(); }
    ThreadList::constIterator constEnd() const { return mList.constEnd(); }

    CriticalSection* getIterateLockCS() { return &mIterateLockCS; }

protected:
    void addThread_(Thread* thread)
    {
        ScopedLock<CriticalSection> lock(&mIterateLockCS);
        mList.pushBack(thread->getListNode());
    }

    void removeThread_(Thread* thread)
    {
        ScopedLock<CriticalSection> lock(&mIterateLockCS);
        mList.erase(thread->getListNode());
    }

    void initMainThread_(Heap* heap);
    void destroyMainThread_();

    // TODO
    static u32 getCurrentThreadID_();

protected:
    ThreadList mList;
    CriticalSection mIterateLockCS;
    Thread* mMainThread;
    ThreadLocalStorage mThreadPtrTLS;
};

} // namespace sead
