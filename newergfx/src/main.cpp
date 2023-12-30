#include <basis/seadRawPrint.h>
#include <framework/seadTask.h>
#include <framework/win/seadConsoleFrameworkWin.h>
#include <heap/seadExpHeap.h>
#include <heap/seadHeapMgr.h>
#include <stream/seadBinaryStreamFormat.h>
#include <stream/seadPrintStream.h>

class AppFramework : public sead::ConsoleFrameworkWin
{
public:
    AppFramework()
        : sead::ConsoleFrameworkWin()
    {
    }

    static void initialize(const InitializeArg& arg)
    {
        sead::ConsoleFrameworkWin::initialize(arg);
    }
};

class RootTask : public sead::Task
{
public:
    RootTask(const sead::TaskConstructArg& arg)
        : sead::Task(arg, "RootTask")
    {
    }

    void prepare() override
    {
        adjustHeapAll();
    }

    void calc() override
    {
        SEAD_PRINT("CALC !!!");
    }

    void draw() override
    {
        SEAD_PRINT("DRAW !!!");
    }
};

int main()
{
    sead::Framework::InitializeArg initArg;
    initArg.heap_size = 0xA00000; // 10 MB
    AppFramework::initialize(initArg);

    AppFramework* framework = nullptr;
    {
        sead::Heap* heap = sead::ExpHeap::create(0, "AppFramework", sead::HeapMgr::getRootHeap(0));
        framework = new(heap) AppFramework();
        heap->adjust();
    }

    sead::TaskBase::CreateArg taskArg(&sead::TTaskFactory<RootTask>);
    sead::Framework::RunArg runArg;
    framework->run(sead::HeapMgr::getRootHeap(0), taskArg, runArg);

    delete framework;

    return 0;
}
