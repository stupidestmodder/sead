#pragma once

#include <framework/seadFramework.h>
#include <framework/seadProcessMeterBar.h>
#include <gfx/seadGraphics.h>

namespace sead {

class InfLoopCheckerThread;

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

    void createSystemTasks(TaskBase* rootTask, const CreateSystemTaskArg& arg) override;

    virtual void createControllerMgr(TaskBase* rootTask);
    virtual void createHostIOMgr(TaskBase* rootTask, HostIOMgr::Parameter* parameter, Heap* heap);
    virtual void createProcessMeter(TaskBase* rootTask);
    virtual void createSeadMenuMgr(TaskBase* rootTask);
    virtual void createInfLoopChecker(TaskBase* rootTask, const TickSpan& infLoopSpan, s32 infLoopThreadStackSize);

    virtual f32 calcFps() = 0;

    virtual void saveScreenShot(const sead::SafeString& filename) { SEAD_UNUSED(filename); }
    virtual bool isScreenShotBusy() const { return false; }

    DisplayState getDisplayState() const { return mDisplayState; }
    MultiProcessMeterBar<32>& getCalcMeter() { return mCalcMeter; }
    MultiProcessMeterBar<32>& getDrawMeter() { return mDrawMeter; }
    MultiProcessMeterBar<32>& getGPUMeter() { return mGPUMeter; }

protected:
    virtual void waitStartDisplayLoop_();
    virtual void initHostIO_();

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
