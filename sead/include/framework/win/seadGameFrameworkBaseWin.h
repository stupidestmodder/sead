#pragma once

#include <basis/win/seadWindows.h>
#include <framework/seadGameFramework.h>

namespace sead {

class GameFrameworkBaseWin : public GameFramework
{
    SEAD_RTTI_OVERRIDE(GameFrameworkBaseWin, GameFramework);

public:
    struct CreateArg
    {
        CreateArg()
            : width(1280)
            , height(720)
            , wait_vblank(1)
            , fps(60.0f)
            , window_name("GameFrameworkWin")
            , clear_color(Color4f::cGray)
            , create_default_framebuffer(true)
        {
        }

        s32 width;
        s32 height;
        u32 wait_vblank;
        f32 fps;
        SafeString window_name;
        Color4f clear_color;
        bool create_default_framebuffer;
    };

public:
    explicit GameFrameworkBaseWin(const CreateArg& arg);
    ~GameFrameworkBaseWin() override;

    FrameBuffer* getMethodFrameBuffer(s32 methodType) const override;
    const LogicalFrameBuffer* getMethodLogicalFrameBuffer(s32 methodType) const override;
    bool setProcessPriority(ProcessPriority priority) override;

    void createControllerMgr(TaskBase* rootTask) override;

    f32 calcFps() override
    {
        if (mLastDiffTime.toS64() == 0)
            return 0.0f;
        else
            return static_cast<f32>(mFrameTime.toS64()) * mArg.fps / static_cast<f32>(mLastDiffTime.toS64());
    }

    void setFps(f32 fps)
    {
        mFrameTime = TickSpan::makeFromMicroSeconds(static_cast<s64>(1000000 / fps));
        mArg.fps = fps;
    }

    virtual void setCaption(const SafeString& caption);

    bool setWaitVBlankInterval(u32 interval);

    HWND getWindowHandle() const
    {
        return mHWnd;
    }

    void setMsgProcCallback(WNDPROC userCallback)
    {
        mMsgProcCallback = userCallback;
    }

    WNDPROC getMsgProcCallback() const
    {
        return mMsgProcCallback;
    }

    const FrameBuffer* getDefaultFrameBuffer() const
    {
        return mDefaultFrameBuffer;
    }

    s32 getWidth() const
    {
        return mArg.width;
    }

    s32 getHeight() const
    {
        return mArg.height;
    }

    s32 getMouseWheel() const
    {
        return mMouseWheel;
    }

    bool isExiting() const
    {
        return mExit;
    }

protected:
    void runImpl_() override;
    MethodTreeMgr* createMethodTreeMgr_(Heap* heap) override;

    virtual void createWindow_();
    virtual void mainLoop_();
    virtual void procFrame_() = 0;
    virtual void procDraw_() = 0;
    virtual void procCalc_();

    virtual void resize_(f32 width, f32 height) = 0;

    LRESULT msgProcImpl_(HWND hWnd, u32 msg, WPARAM wParam, LPARAM lParam);

    static LRESULT __stdcall msgProc_(HWND hWnd, u32 msg, WPARAM wParam, LPARAM lParam);

private:
    static GameFrameworkBaseWin* sInstance;

protected:
    CreateArg mArg;
    HWND mHWnd;
    TickTime mLastUpdateTime;
    TickSpan mFrameTime;
    TickSpan mLastDiffTime;
    bool mExit;
    WNDPROC mMsgProcCallback;
    FrameBuffer* mDefaultFrameBuffer;
    LogicalFrameBuffer mDefaultLogicalFrameBuffer;
    s32 mMouseWheel;
};

} // namespace sead
