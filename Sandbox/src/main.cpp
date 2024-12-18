#include <framework/win/seadGameFrameworkWinGL.h>
#include <framework/seadCalculateTask.h>
#include <heap/seadExpHeap.h>
#include <heap/seadHeapMgr.h>

#include <RootTask.h>

int main()
{
    sead::Framework::InitializeArg initArg;
    initArg.heap_size = 50 * 1024 * 1024; // 50 MB
    sead::GameFrameworkWinGL::initialize(initArg);

    sead::GameFrameworkWinGL::CreateArg createArg;
    createArg.window_name = "sead";
    createArg.clear_color = sead::Color4f(0.0f, 0.0f, 0.3f, 1.0f);

    sead::GameFrameworkWinGL* framework = nullptr;
    {
        sead::ExpHeap* heap = sead::ExpHeap::create(0, "Framework", sead::HeapMgr::getRootHeap(0));
        framework = new(heap) sead::GameFrameworkWinGL(createArg);
        heap->adjust();
    }

    {
        sead::ExpHeap* heap = sead::ExpHeap::create(0, "GraphicsSystem", sead::HeapMgr::getRootHeap(0));
        framework->initializeGraphicsSystem(heap, sead::Vector2f(1280.0f, 720.0f));
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
    return main();
}
#endif // SEAD_PLATFORM_WINDOWS
