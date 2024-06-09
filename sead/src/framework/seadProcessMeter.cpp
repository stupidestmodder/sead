#include <framework/seadProcessMeter.h>

#include <framework/seadFramework.h>
#include <framework/seadProcessMeterBar.h>
#include <gfx/seadCamera.h>
#include <gfx/seadDrawContext.h>
#include <gfx/seadPrimitiveDrawer.h>
#include <gfx/seadProjection.h>
#include <gfx/seadTextWriter.h>

namespace sead {

SEAD_TASK_SINGLETON_DISPOSER_IMPL(ProcessMeter);

ProcessMeter::ProcessMeter(const TaskConstructArg& arg)
    : Task(arg, "sead::ProcessMeter")
    , mViewport(*getFramework()->getMethodLogicalFrameBuffer(2))
    , mSectionTime(TickSpan::makeFromMicroSeconds(16666))
    , mSectionNum(0)
    , mFrameBegin()
    , mFrameSpan(0)
    , mCurFrameBegin()
    , mBarList()
    , mVisible(false)
    , mTextVisible(true)
    , mVerticalMode(false)
    , mShowQuaterMark(true)
    , mBarHeight(16.0f)
    , mTextFix(true)
    , mBorderColor(Color4f::cWhite)
    , mBackColor(0.0f, 0.0f, 0.0f, 0.5f)
    , mTextColor(Color4f::cWhite)
    , mCurSectionNum(mSectionNum)
    , mAdjustTime(60)
    , mAdjustCounter(mAdjustTime)
    , mMinSectionNum(1)
    , mMaxSectionSeparatorNum(3)
{
    mViewport.getMax().y = 32.0f;
    mBarList.initOffset(ProcessMeterBarBase::getListNodeOffset());
}

ProcessMeter::~ProcessMeter()
{
    while (!mBarList.isEmpty())
    {
        detachProcessMeterBar(mBarList.front());
    }
}

void ProcessMeter::calc()
{
    if (mSectionNum != 0)
        return;

    u32 curNum = calcMaxSectionNum_();
    if (curNum >= mCurSectionNum)
    {
        mAdjustCounter = mAdjustTime;
        mCurSectionNum = curNum;
    }
    else
    {
        if (mAdjustCounter > 0)
            mAdjustCounter--;
        else
            mCurSectionNum = curNum;
    }
}

void ProcessMeter::draw()
{
    DrawContext drawContext;
    drawContext.setDefaultCommandBufferFromFramework(getFramework());

    if (!mVisible)
        return;

    TextWriter::setupGraphics(&drawContext);

    Viewport vp = mViewport;
    if (mBarHeight > 0.0f)
    {
        f32 height = mBarList.size() > 0 ? mBarHeight * mBarList.size() : mBarHeight;

        if (mVerticalMode)
        {
            if (mViewport.getMax().x < mViewport.getMin().x + height)
                vp.setMax(Vector2f(mViewport.getMin().x + height, mViewport.getMax().y));
        }
        else
        {
            if (mViewport.getMax().y < mViewport.getMin().y + height)
                vp.setMax(Vector2f(mViewport.getMax().x, mViewport.getMin().y + height));
        }
    }

    PrimitiveDrawer drawer(&drawContext);

    OrthoProjection proj(0.0f, 1.0f, vp);
    vp.apply(&drawContext, *getFramework()->getMethodLogicalFrameBuffer(2));
    drawer.setProjection(&proj);

    OrthoCamera cam(proj);
    cam.updateMatrix();
    drawer.setCamera(&cam);

    TextWriter writer(&drawContext, &vp);
    writer.setColor(mTextColor);

    if (mVerticalMode)
        drawVerticalMode_(vp, &drawer, &writer);
    else
        drawHorizontalMode_(vp, &drawer, &writer);
}

void ProcessMeter::measureBeginFrame()
{
    mCurFrameBegin.setNow();
}

void ProcessMeter::measureEndFrame()
{
    for (ProcessMeterBarBase& bar : mBarList)
    {
        bar.onEndFrame();
    }

    mFrameSpan = mCurFrameBegin.diffToNow();
    mFrameBegin = mCurFrameBegin;
}

void ProcessMeter::attachProcessMeterBar(ProcessMeterBarBase* meter)
{
    SEAD_ASSERT(meter);

    if (!meter->mParent)
    {
        mBarList.pushBack(meter);
        meter->setParentProcessMeter(this);
    }
}

void ProcessMeter::detachProcessMeterBar(ProcessMeterBarBase* meter)
{
    SEAD_ASSERT(meter);

    mBarList.erase(meter);
    meter->setParentProcessMeter(nullptr);
}

void ProcessMeter::setVisible(bool visible)
{
    mVisible = visible;
}

f32 ProcessMeter::calcTimeLinePos_(const Viewport& vp, u32 sectionNum, TickTime t)
{
    return calcTimeLineWidth_(vp, sectionNum, t.diff(mFrameBegin));
}

f32 ProcessMeter::calcTimeLineWidth_(const Viewport& vp, u32 sectionNum, TickSpan t)
{
    SEAD_ASSERT(sectionNum > 0);

    TickSpan totalSpan = mSectionTime * static_cast<f32>(sectionNum);

    f32 size = mVerticalMode ? vp.getSizeY() : vp.getSizeX();

    return static_cast<f32>(t.toS64()) * size / static_cast<f32>(totalSpan.toS64());
}

u32 ProcessMeter::calcMaxSectionNum_()
{
    u32 sectionNum = static_cast<u32>(mFrameSpan.toS64() / mSectionTime.toS64()) + 1;

    for (ProcessMeterBarBase& bar : mBarList)
    {
        TickSpan span = bar.getLastFinalEnd() - mFrameBegin;

        if (span > 0)
        {
            u32 secnum = static_cast<u32>(span.toS64() / mSectionTime.toS64()) + 1;
            if (sectionNum < secnum)
                sectionNum = secnum;
        }
    }

    return sectionNum;
}

void ProcessMeter::drawHorizontalMode_(const Viewport& viewport, PrimitiveDrawer* drawer, TextWriter* writer)
{
    u32 sectionNum = mSectionNum;

    if (sectionNum == 0)
        sectionNum = mCurSectionNum;

    if (sectionNum < mMinSectionNum)
        sectionNum = mMinSectionNum;

    drawer->begin();
    {
        {
            drawer->drawQuad(
                PrimitiveDrawer::QuadArg().setCornerAndSize(
                    -viewport.getHalfSizeX(), -viewport.getHalfSizeY(), 0.0f,
                    calcTimeLineWidth_(viewport, sectionNum, mFrameSpan), viewport.getSizeY()
                ).setColor(mBackColor)
            );
        }

        if (mBarList.size() > 0)
        {
            f32 height = viewport.getSizeY() / static_cast<f32>(mBarList.size());

            s32 count = 0;
            for (const ProcessMeterBarBase& bar : mBarList)
            {
                s32 secNum = bar.getLastSectionNum();
                for (s32 i = 0; i < secNum; i++)
                {
                    const ProcessMeterBarBase::Section& sec = bar.getLastResult(i);

                    f32 barHeight = height * (sec.parent != -1 ? 0.8f : 1.0f);

                    drawer->drawQuad(
                        PrimitiveDrawer::QuadArg().setCornerAndSize(
                            -viewport.getHalfSizeX() + calcTimeLinePos_(viewport, sectionNum, sec.begin),
                            -viewport.getHalfSizeY() + height * count,
                            0.0f,
                            calcTimeLineWidth_(viewport, sectionNum, sec.span),
                            barHeight
                        ).setColor(sec.color, sec.color * Color4f(0.5f, 0.5f, 0.5f, 1.0f))
                    );
                }

                count++;
            }
        }

        {
            drawer->drawBox(
                PrimitiveDrawer::QuadArg().setSize(
                    viewport.getSizeX() - 1.0f, viewport.getSizeY() - 1.0f
                ).setColor(mBorderColor)
            );
        }

        {
            TickTime t = mFrameBegin;

            u32 sectionLineNum = sectionNum;
            if (mMaxSectionSeparatorNum != 0 && sectionLineNum > mMaxSectionSeparatorNum)
                sectionLineNum = mMaxSectionSeparatorNum;

            u32 prevTimelinePos = 0;

            for (u32 i = 0; i < sectionLineNum; i++)
            {
                t += mSectionTime;

                f32 timelinePos = calcTimeLinePos_(viewport, sectionNum, t);

                drawer->drawLine(
                    Vector3f(-viewport.getHalfSizeX() + timelinePos, -viewport.getHalfSizeY(), 0.0f),
                    Vector3f(-viewport.getHalfSizeX() + timelinePos,  viewport.getHalfSizeY(), 0.0f),
                    mBorderColor
                );

                if (mShowQuaterMark)
                {
                    // TODO
                    SEAD_UNUSED(prevTimelinePos);
                }
            }
        }
    }
    drawer->end();

    if (mTextVisible && mBarList.size() > 0)
    {
        f32 height = viewport.getSizeY() / static_cast<f32>(mBarList.size());

        f32 scale = height / writer->getLineHeight();
        writer->setScale(Vector2f(1.0f, 1.0f) * Mathf::min(scale, 1.0f));

        s32 count = 0;
        for (const ProcessMeterBarBase& bar : mBarList)
        {
            // TODO
            SEAD_UNUSED(bar);
            count++;
        }
    }
}

void ProcessMeter::drawVerticalMode_(const Viewport& viewport, PrimitiveDrawer* drawer, TextWriter* writer)
{
    // TODO
    SEAD_UNUSED(viewport);
    SEAD_UNUSED(drawer);
    SEAD_UNUSED(writer);
}

} // namespace sead
