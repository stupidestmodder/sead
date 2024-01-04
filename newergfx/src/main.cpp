#include <heap/seadExpHeap.h>
#include <heap/seadHeapMgr.h>

#include "AppFramework.h"
#include "RootTask.h"

int main()
{
    sead::Framework::InitializeArg initArg;
    initArg.heap_size = 0xA00000; // 10 MB
    AppFramework::initialize(initArg);

    AppFramework* framework = nullptr;
    {
        sead::ExpHeap* heap = sead::ExpHeap::create(0, "AppFramework", sead::HeapMgr::getRootHeap(0));
        framework = new(heap) AppFramework();
        heap->adjust();
    }

    sead::TaskBase::CreateArg taskArg(&sead::TTaskFactory<RootTask>);
    sead::Framework::RunArg runArg;
    framework->run(sead::HeapMgr::getRootHeap(0), taskArg, runArg);

    delete framework;
}
