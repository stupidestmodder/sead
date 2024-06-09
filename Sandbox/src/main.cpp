#include <framework/win/seadConsoleFrameworkWin.h>
#include <framework/seadCalculateTask.h>
#include <heap/seadExpHeap.h>
#include <heap/seadHeapMgr.h>

class AppFramework : public sead::ConsoleFrameworkWin
{
public:
    AppFramework()
        : sead::ConsoleFrameworkWin()
    {
    }
};

class RootTask : public sead::CalculateTask
{
public:
    RootTask(const sead::TaskConstructArg& arg)
        : sead::CalculateTask(arg)
    {
    }

    void enter() override
    {
        SEAD_PRINT("Enter!\n");
    }
};

int main()
{
    sead::Framework::InitializeArg initArg;
    initArg.heap_size = 10 * 1024 * 1024; // 10 MB
    AppFramework::initialize(initArg);

    //sead::HeapMgr::createUnboundHeap();

    AppFramework* framework = nullptr;
    {
        sead::ExpHeap* heap = sead::ExpHeap::create(0, "AppFramework", sead::HeapMgr::getRootHeap(0));
        framework = new(heap) AppFramework();
        heap->adjust();
    }

    sead::TaskBase::CreateArg rootArg(&sead::TTaskFactory<RootTask>);
    sead::Framework::RunArg runArg;
    framework->run(sead::HeapMgr::getRootHeap(0), rootArg, runArg);

    delete framework;
}

#ifdef SEAD_PLATFORM_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    main();
    return 0;
}
#endif // SEAD_PLATFORM_WINDOWS
