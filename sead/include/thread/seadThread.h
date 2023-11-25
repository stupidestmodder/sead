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
public:
    // TODO
    enum class State
    {
    };

public:
    ThreadListNode* getThreadListNode() { return &mListNode; }
    Heap* getCurrentHeap() const { return mCurrentHeap; }

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
        mList.pushBack(thread->getThreadListNode());
    }

    void removeThread_(Thread* thread)
    {
        ScopedLock<CriticalSection> lock(&mIterateLockCS);
        mList.erase(thread->getThreadListNode());
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
