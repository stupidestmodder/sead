// TODO: Switch to a base 'desktop' framework class?
#if defined(SEAD_PLATFORM_WINDOWS)
#include <framework/win/seadGameFrameworkWinGL.h>
#elif defined(SEAD_PLATFORM_SDL)
#include <framework/sdl/seadGameFrameworkSDLGL.h>
#endif
#include <framework/seadCalculateTask.h>
#include <heap/seadExpHeap.h>
#include <heap/seadHeapMgr.h>

#include <RootTask.h>

int main()
{
    sead::Framework::InitializeArg initArg;
    initArg.heap_size = 200 * 1024 * 1024; // 200 MB

    #if defined(SEAD_PLATFORM_WINDOWS)
    sead::GameFrameworkWinGL::initialize(initArg);
    #elif defined(SEAD_PLATFORM_SDL)
    sead::GameFrameworkSDLGL::initialize(initArg);
    #endif

    {
        sead::ExpHeap* heap = sead::ExpHeap::create(50 * 1024 * 1024, "NonSeadThreadHeap", sead::HeapMgr::getRootHeap(0), sead::Heap::eForward, true);
        sead::HeapMgr::instance()->setAllocFromNotSeadThreadHeap(heap);
    }
    
    #if defined(SEAD_PLATFORM_WINDOWS)
    sead::GameFrameworkWinGL::CreateArg createArg;
    #elif defined(SEAD_PLATFORM_SDL)
    sead::GameFrameworkSDLGL::CreateArg createArg;
    #endif
    createArg.window_name = "sead";
    createArg.clear_color = sead::Color4f(0.0f, 0.0f, 0.3f, 1.0f);

    #if defined(SEAD_PLATFORM_WINDOWS)
    sead::GameFrameworkWinGL* framework = nullptr;
    #elif defined(SEAD_PLATFORM_SDL)
    sead::GameFrameworkSDLGL* framework = nullptr;
    #endif

    {
        sead::ExpHeap* heap = sead::ExpHeap::create(0, "Framework", sead::HeapMgr::getRootHeap(0));
        #if defined(SEAD_PLATFORM_WINDOWS)
        framework = new(heap) sead::GameFrameworkWinGL(createArg);
        #elif defined(SEAD_PLATFORM_SDL)
        framework = new(heap) sead::GameFrameworkSDLGL(createArg);
        #endif
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

#if defined(SEAD_PLATFORM_WINDOWS)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{
    return main();
}
#endif // SEAD_PLATFORM_WINDOWS
