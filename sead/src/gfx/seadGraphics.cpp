#include <gfx/seadGraphics.h>

#include <gfx/seadDrawLockContext.h>

namespace sead {

Graphics* Graphics::sInstance = nullptr;

Graphics::DevicePosture Graphics::sDefaultDevicePosture = DevicePosture::eSame;
f32 Graphics::sDefaultDeviceZScale = 1.0f;
f32 Graphics::sDefaultDeviceZOffset = 0.0f;

Graphics::Graphics()
    : IDisposer()
    , mContextLockFunc(nullptr)
    , mDrawLockContext(nullptr)
{
}

Graphics::~Graphics()
{
}

void Graphics::initialize(Heap* heap)
{
    mDrawLockContext = new(heap) DrawLockContext();
    initializeDrawLockContext(heap);

    mDrawLockContext->lock();
    initializeImpl(heap);
    mDrawLockContext->unlock();
}

void Graphics::lockDrawContext()
{
    if (mContextLockFunc)
        mContextLockFunc(true);
    else
        mDrawLockContext->lock();
}

void Graphics::unlockDrawContext()
{
    if (mContextLockFunc)
        mContextLockFunc(false);
    else
        mDrawLockContext->unlock();
}

void Graphics::initializeDrawLockContext(Heap* heap)
{
    mDrawLockContext->initialize(heap);
}

} // namespace sead
