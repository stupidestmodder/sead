#include <framework/seadFramework.h>

#include <heap/seadExpHeap.h>
#include <heap/seadHeapMgr.h>
#include <random/seadGlobalRandom.h>
#include <thread/seadThread.h>

namespace sead {

Framework::InitializeArg::InitializeArg()
    : heap_size(0x3000000)
    , arena(nullptr)
{
}

Framework::RunArg::RunArg()
    : prepare_stack_size(0)
    , prepare_priority(-1)
{
}

void Framework::initialize(const InitializeArg& arg)
{
    if (arg.arena)
        HeapMgr::initialize(arg.arena);
    else
        HeapMgr::initialize(arg.heap_size);

    Heap* rootHeap = HeapMgr::getRootHeap(0);

    {
        Heap* heap = ExpHeap::create(0, "sead::ThreadMgr", rootHeap);

        ThreadMgr::createInstance(heap);
        ThreadMgr::instance()->initialize(heap);

        heap->adjust();
    }

    GlobalRandom::createInstance(rootHeap);
}

} // namespace sead
