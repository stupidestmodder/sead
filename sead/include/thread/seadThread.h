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
    // TODO: SEAD_ENUM(State)
    enum class State
    {
        eInitialized = 0,
        eRunning,
        eQuitting,
        eTerminated,
        eReleased
    };

    static const s32 cDefaultSeadPriority;
    static const s32 cDefaultPriority;

public:
    Thread(const SafeString& name, Heap* heap, s32 platformPriority, MessageQueue::BlockType blockType,
           MessageQueue::Element quitMsg, s32 stackSize, s32 msgQueueSize);

protected:
#ifdef SEAD_PLATFORM_WINDOWS
    Thread(Heap* heap, HANDLE handle, u32 id);
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS

public:
    ~Thread() override;

    virtual void destroy() { waitDone(); }
    virtual bool sendMessage(MessageQueue::Element msg, MessageQueue::BlockType blockType);
    virtual MessageQueue::Element recvMessage(MessageQueue::BlockType blockType);
    virtual const MessageQueue& getMessageQueue() const { return mMessageQueue; }
    virtual bool start();
    virtual void quit(bool isJam);

    bool isDone() const { return mState == State::eTerminated || mState == State::eReleased; }

    virtual void waitDone();
    virtual void quitAndDestroySingleThread(bool isJam) { quitAndWaitDoneSingleThread(isJam); }
    virtual void quitAndWaitDoneSingleThread(bool isJam);

    static void sleep(TickSpan span);
    static void yield();

    u32 getID() const;
    Heap* getCurrentHeap() const { return mCurrentHeap; }

    virtual void setPriority(s32 platformPriority);
    virtual s32 getPriority() const;
    virtual MessageQueue::BlockType getBlockType() const { return mBlockType; }
    virtual s32 getStackSize() const { return mStackSize; }
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
    virtual void calc_(MessageQueue::Element) = 0;

    void initStackCheck_();
    void initStackCheckWithCurrentStackPointer_();
    virtual u32* getStackCheckStartAddress_() const;

#ifdef SEAD_PLATFORM_WINDOWS
    static u32 __stdcall winThreadFunc_(void* arg);
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
    // Affinity
#ifdef SEAD_PLATFORM_WINDOWS
    HANDLE mHandle;
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS

};

class ThreadMgr
{
    SEAD_SINGLETON_DISPOSER(ThreadMgr);

public:
    ThreadMgr();
    virtual ~ThreadMgr();

    void initialize(Heap* heap);
    void destroy();

    Thread* getCurrentThread() const { return reinterpret_cast<Thread*>(mThreadPtrTLS.getValue()); }
    Thread* getMainThread() const { return mMainThread; }
    bool isMainThread() const;

    ThreadList::constIterator constBegin() const { return mList.constBegin(); }
    ThreadList::constIterator constEnd() const { return mList.constEnd(); }

    CriticalSection* getIterateLockCS() { return &mIterateLockCS; }

    static void quitAndDestroyMultipleThread(Thread** threads, s32 num, bool isJam)
    {
        ThreadMgr::quitAndWaitDoneMultipleThread(threads, num, isJam);
    }

    static void quitAndWaitDoneMultipleThread(Thread** threads, s32 num, bool isJam);
    static void waitDoneMultipleThread(Thread* const* threads, s32 num);
    static void checkCurrentThreadStackOverFlow(const char* pos, s32 line);
    static void checkCurrentThreadStackEndCorruption(const char* pos, s32 line);
    static void checkCurrentThreadStackPointerOverFlow(const char* pos, s32 line);

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

    friend class Thread;

    void initMainThread_(Heap* heap);
    void destroyMainThread_();

    static u32 getCurrentThreadID_();

protected:
    ThreadList mList;
    CriticalSection mIterateLockCS;
    Thread* mMainThread;
    ThreadLocalStorage mThreadPtrTLS;
};

class MainThread : public Thread
{
public:
#ifdef SEAD_PLATFORM_WINDOWS
    MainThread(Heap* heap, HANDLE thread, u32 id)
        : Thread(heap, thread, id)
    {
    }
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
    ~MainThread() override
    {
        mState = State::eTerminated;
    }

    void destroy() override { SEAD_ASSERT_MSG(false, "Main thread can not destroy"); }
    void quit(bool) override { SEAD_ASSERT_MSG(false, "Main thread can not quit"); }
    void waitDone() override { SEAD_ASSERT_MSG(false, "Main thread can not waitDone"); }
    void quitAndDestroySingleThread(bool) override { SEAD_ASSERT_MSG(false, "Main thread can not quit"); }
    void setPriority(s32) override { SEAD_ASSERT_MSG(false, "Main thread priority can not set"); }

protected:
    void calc_(MessageQueue::Element) override { }
};

} // namespace sead
