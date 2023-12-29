#pragma once

#include <heap/seadDisposer.h>
#include <thread/seadCriticalSection.h>

namespace sead {

class Thread;

// TODO
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

    using LockFunc = void (*)(bool);

public:
    Graphics();
    ~Graphics() override;

    void initialize();

    void lockDrawContext();
    void unlockDrawContext();

protected:
    static Graphics* sInstance;

    static DevicePosture sDefaultDevicePosture;
    static f32 sDefaultDeviceZScale;
    static f32 sDefaultDeviceZOffset;

protected:
    Thread* mContextHolderThread;
    s32 mContextRefCounter;
    CriticalSection mContextCriticalSection;
    LockFunc mContextLockFunc;
};

} // namespace sead
