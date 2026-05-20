#include <heap/seadDisposer.h>

#include <basis/seadAssert.h>
#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>

static sead::Heap* const cDestructedFlag = reinterpret_cast<sead::Heap*>(1);

namespace sead {

IDisposer::IDisposer()
    : mDisposerHeap(nullptr)
    , mListNode()
{
    if (!HeapMgr::instance())
        return;

    mDisposerHeap = HeapMgr::instance()->findContainHeap(this);
    if (mDisposerHeap)
        mDisposerHeap->appendDisposer_(this);
}

IDisposer::IDisposer(Heap* disposerHeap, HeapNullOption option)
    : mDisposerHeap(disposerHeap)
    , mListNode()
{
    if (mDisposerHeap)
    {
        mDisposerHeap->appendDisposer_(this);
        return;
    }

    switch (option)
    {
    case HeapNullOption::eNotAllow:
        SEAD_ASSERT_MSG(false, "disposerHeap must not be nullptr");

    case HeapNullOption::eFindContainHeap:
        if (!HeapMgr::instance())
            return;

        mDisposerHeap = HeapMgr::instance()->findContainHeap(this);
        if (mDisposerHeap)
            mDisposerHeap->appendDisposer_(this);

        return;

    case HeapNullOption::eNotDispose:
        return;

    case HeapNullOption::eUseCurrentHeap:
        if (!HeapMgr::instance())
            return;

        mDisposerHeap = HeapMgr::instance()->getCurrentHeap();
        if (mDisposerHeap)
            mDisposerHeap->appendDisposer_(this);

        return;

    default:
        SEAD_ASSERT_MSG(false, "illegal option[%d]", static_cast<s32>(option));
        return;
    }
}

IDisposer::~IDisposer()
{
    if (mDisposerHeap == cDestructedFlag)
    {
        SEAD_ASSERT_MSG(false, "Destruct twice. [0x%p] Your class has possibilities for wrong order of multiple inheritance.", this);
        return;
    }

    if (mDisposerHeap)
        mDisposerHeap->removeDisposer_(this);

    mDisposerHeap = cDestructedFlag;
}

} // namespace sead
