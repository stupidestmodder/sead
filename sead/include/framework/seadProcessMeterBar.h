#pragma once

#include <container/seadBuffer.h>
#include <container/seadSafeArray.h>
#include <gfx/seadColor.h>
#include <heap/seadDisposer.h>
#include <prim/seadNamable.h>
#include <time/seadTickTime.h>

namespace sead {

class ProcessMeter;

class ProcessMeterBarBase : public IDisposer, public INamable
{
public:
    struct Section
    {
        TickTime begin;
        TickSpan span;
        Color4f color;
        s32 parent;
    };

public:
    ProcessMeterBarBase(Section* buffer, s32 sectionNum, const SafeString& name, const Color4f& color);
    ~ProcessMeterBarBase() override;

    void measureBegin();
    void measureBegin(const TickTime& t);
    void measureBegin(const Color4f& c);
    void measureBegin(const TickTime& t, const Color4f& c);

    void measureEnd();
    void measureEnd(const TickTime& arg);

    const TickTime& getLastFirstBegin() const;
    TickSpan getLastTotalSpan() const;

    const TickTime& getLastFinalEnd() const
    {
        return mFinalEnd[1 - mCurBuffer];
    }

    const Section& getLastResult(s32 idx) const
    {
        return *mSectionList[1 - mCurBuffer].get(idx);
    }

    s32 getLastSectionNum() const
    {
        return mSectionNum[1 - mCurBuffer];
    }

    void setColor(const Color4f& color)
    {
        mColor = color;
    }

    const Color4f& getColor() const
    {
        return mColor;
    }

    void onEndFrame();
    void setParentProcessMeter(ProcessMeter* parent);

    static u32 getListNodeOffset() { return offsetof(ProcessMeterBarBase, mListNode); }

protected:
    void measureBeginImpl_(const TickTime& t, Color4f color);
    void measureEndImpl_(const TickTime& arg);

    void addSection_(const TickTime& t, Color4f color, s32 parent);
    void endSection_(s32 idx, const TickTime& t);

    Section* getCurSection_(s32 idx);

protected:
    ListNode mListNode;
    ProcessMeter* mParent;
    Color4f mColor;
    SafeArray<Buffer<Section>, 2> mSectionList;
    SafeArray<TickTime, 2> mFinalEnd;
    SafeArray<s32, 2> mSectionNum;
    s32 mCurBuffer;
    s32 mTopSection;
    s32 mOverNum;
    bool mMesureEnable;

    friend class ProcessMeter;
};

template <s32 N>
class MultiProcessMeterBar : public ProcessMeterBarBase
{
public:
    MultiProcessMeterBar(const SafeString& name, const Color4f& color)
        : ProcessMeterBarBase(mBuffer, N, name, color)
        , mBuffer()
    {
    }

private:
    Section mBuffer[N * 2];
};

} // namespace sead
