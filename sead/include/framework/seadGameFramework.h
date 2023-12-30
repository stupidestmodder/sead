#pragma once

#include <framework/seadFramework.h>
#include <framework/seadProcessMeterBar.h>
#include <gfx/seadGraphics.h>

namespace sead {

class InfLoopCheckerThread;

// TODO
class GameFramework : public Framework
{
    SEAD_RTTI_OVERRIDE(GameFramework, Framework);

public:
    enum class DisplayState
    {
        eHide = 0,
        eReady,
        eShow
    };

    using ProcDrawCallback = void (*)(bool);

public:
    GameFramework();
    ~GameFramework() override;

    static void initialize(const InitializeArg& arg);

    void startDisplay();

    void lockFrameDrawContext();
    void unlockFrameDrawContext();

    void createSystemTasks(TaskBase*, const CreateSystemTaskArg&) override;

    virtual void createControllerMgr(TaskBase*);
    virtual void createProcessMeter(TaskBase*);
    virtual void createSeadMenuMgr(TaskBase*);
    virtual void createInfLoopChecker(TaskBase*, const TickSpan&, s32);

    virtual f32 calcFps() = 0;

    virtual void saveScreenShot(const sead::SafeString&);
    virtual bool isScreenShotBusy() const;

protected:
    virtual void waitStartDisplayLoop_();

protected:
    DisplayState mDisplayState;
    MultiProcessMeterBar<32> mCalcMeter;
    MultiProcessMeterBar<32> mDrawMeter;
    MultiProcessMeterBar<32> mGPUMeter;
    InfLoopCheckerThread* mCheckerThread;
    Graphics::LockFunc mFrameLockFunc;
    ProcDrawCallback mProcDrawCallback;
};

} // namespace sead
