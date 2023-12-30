#pragma once

#include <container/seadOffsetList.h>
#include <framework/seadTask.h>
#include <time/seadTickTime.h>

namespace sead {

class ProcessMeterBarBase;

// TODO
class ProcessMeter : public Task
{
    // TASK_INSTANCE();

public:
    using BarList = OffsetList<ProcessMeterBarBase>;

public:
    explicit ProcessMeter(const TaskConstructArg& arg);
    ~ProcessMeter() override;

    void calc() override;
    void draw() override;

    void measureBeginFrame();
    void measureEndFrame();
    void attachProcessMeterBar(ProcessMeterBarBase*);
    void detachProcessMeterBar(ProcessMeterBarBase*);
    void setVisible(bool);
/*
protected:
    f32 calcTimeLinePos_(const Viewport&, u32, TickTime);
    f32 calcTimeLineWidth_(const Viewport&, u32, TickSpan);
    u32 calcMaxSectionNum_();

    void drawHorizontalMode_(const Viewport&, PrimitiveDrawer*);
    void drawVerticalMode_(const Viewport&, PrimitiveDrawer*);

protected:
    GraphicsContext mContext;
    Viewport mViewport;
    TickSpan mSectionTime;
    u32 mSectionNum;
    TickTime mFrameBegin;
    TickSpan mFrameSpan;
    TickTime mCurFrameBegin;
    BarList mBarList;
    bool mVisible;
    bool mTextVisible;
    bool mVerticalMode;
    bool mShowQuaterMark;
    f32 mBarHeight;
    bool mTextFix;
    Color4f mBorderColor;
    Color4f mBackColor;
    Color4f mTextColor;
    u32 mCurSectionNum;
    s32 mAdjustTime;
    s32 mAdjustCounter;
    u32 mMinSectionNum;
    u32 mMaxSectionSeparatorNum;
*/
};

} // namespace sead
