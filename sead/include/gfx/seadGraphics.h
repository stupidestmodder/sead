#pragma once

#include <heap/seadDisposer.h>
#include <thread/seadCriticalSection.h>

namespace sead {

class DrawLockContext;

class Graphics : public IDisposer
{
public:
    enum class DevicePosture
    {
        eSame = 0,
        eRotateRight,
        eRotateLeft,
        eRotateHalfAround,
        eFlipX,
        eFlipY,
        eFlipXY = 3,
        eInvalid
    };

    using LockFunc = void (*)(bool isLock);

public:
    static Graphics* instance() { return sInstance; }
    static void setInstance(Graphics* impl) { sInstance = impl; }

public:
    Graphics();
    ~Graphics() override;

    void initialize(Heap* heap);

    void lockDrawContext();
    void unlockDrawContext();

    static DevicePosture getDefaultDevicePosture() { return sDefaultDevicePosture; }
    static f32 getDefaultDeviceZScale() { return sDefaultDeviceZScale; }
    static f32 getDefaultDeviceZOffset() { return sDefaultDeviceZOffset; }

protected:
    virtual void initializeDrawLockContext(Heap* heap);
    virtual void initializeImpl(Heap* heap) = 0;
    virtual void setViewportImpl(f32 x, f32 y, f32 w, f32 h) = 0;
    virtual void setScissorImpl(f32 x, f32 y, f32 w, f32 h) = 0;
    // TODO: Add remaining methods

protected:
    static Graphics* sInstance;

    static DevicePosture sDefaultDevicePosture;
    static f32 sDefaultDeviceZScale;
    static f32 sDefaultDeviceZOffset;

protected:
    LockFunc mContextLockFunc;
    DrawLockContext* mDrawLockContext;
};

} // namespace sead
