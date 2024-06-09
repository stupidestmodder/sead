#include <framework/seadProcessMeterBar.h>

#include <framework/seadProcessMeter.h>

namespace sead {

ProcessMeterBarBase::ProcessMeterBarBase(Section* buffer, s32 sectionNum, const SafeString& name, const Color4f& color)
    : IDisposer()
    , INamable(name)
    , mListNode()
    , mParent(nullptr)
    , mColor(color)
    , mSectionList()
    , mFinalEnd()
    , mSectionNum()
    , mCurBuffer(0)
    , mTopSection(-1)
    , mOverNum(0)
    , mMesureEnable(false)
{
    mSectionList[0].setBuffer(sectionNum, buffer);
    mSectionNum[0] = 0;

    mSectionList[1].setBuffer(sectionNum, buffer + sectionNum);
    mSectionNum[1] = 0;
}

ProcessMeterBarBase::~ProcessMeterBarBase()
{
    if (mParent)
        mParent->detachProcessMeterBar(this);
}

void ProcessMeterBarBase::measureBegin()
{
    if (mMesureEnable)
    {
        TickTime t;
        measureBeginImpl_(t, mColor);
    }
}

void ProcessMeterBarBase::measureBegin(const TickTime& t)
{
    if (mMesureEnable)
        measureBeginImpl_(t, mColor);
}

void ProcessMeterBarBase::measureBegin(const Color4f& c)
{
    if (mMesureEnable)
    {
        TickTime t;
        measureBeginImpl_(t, c);
    }
}

void ProcessMeterBarBase::measureBegin(const TickTime& t, const Color4f& c)
{
    if (mMesureEnable)
        measureBeginImpl_(t, c);
}

void ProcessMeterBarBase::measureEnd()
{
    if (mMesureEnable)
    {
        TickTime t;
        measureEndImpl_(t);
    }
}

void ProcessMeterBarBase::measureEnd(const TickTime& arg)
{
    if (mMesureEnable)
        measureEndImpl_(arg);
}

const TickTime& ProcessMeterBarBase::getLastFirstBegin() const
{
    return mSectionList[1 - mCurBuffer].get(0)->begin;
}

TickSpan ProcessMeterBarBase::getLastTotalSpan() const
{
    TickSpan ret(0);

    for (s32 i = 0; i < mSectionNum[1 - mCurBuffer]; i++)
    {
        if (mSectionList[1 - mCurBuffer].get(i)->parent == -1)
            ret += mSectionList[1 - mCurBuffer].get(i)->span;
    }

    return ret;
}

void ProcessMeterBarBase::onEndFrame()
{
    SEAD_ASSERT(mTopSection == -1);
    SEAD_ASSERT(mOverNum == 0);

    mCurBuffer = 1 - mCurBuffer;
    mTopSection = -1;
    mOverNum = 0;
    mSectionNum[mCurBuffer] = 0;
    mMesureEnable = mParent != nullptr;
}

void ProcessMeterBarBase::setParentProcessMeter(ProcessMeter* parent)
{
    SEAD_ASSERT(mParent == nullptr || parent == nullptr);

    mParent = parent;
}

void ProcessMeterBarBase::measureBeginImpl_(const TickTime& t, Color4f color)
{
    addSection_(t, color, mTopSection);
}

void ProcessMeterBarBase::measureEndImpl_(const TickTime& arg)
{
    TickTime t = arg;
    mFinalEnd[mCurBuffer] = t;

    if (mOverNum > 0)
    {
        mOverNum--;
        return;
    }

    Section* last = getCurSection_(mTopSection);
    if (t - last->begin < 0)
        t = last->begin;

    SEAD_ASSERT_MSG(mTopSection >= 0, "Unmatching measureBegin / measureEnd.");
    endSection_(mTopSection, t);
    mTopSection = getCurSection_(mTopSection)->parent;
}

void ProcessMeterBarBase::addSection_(const TickTime& t, Color4f color, s32 parent)
{
    if (mSectionNum[mCurBuffer] >= mSectionList[0].getSize())
    {
        mOverNum++;
        return;
    }

    Section* sec = getCurSection_(mSectionNum[mCurBuffer]);
    sec->begin = t;
    sec->span = -1;
    sec->color = color;
    sec->parent = parent;

    mTopSection = mSectionNum[mCurBuffer];
    mSectionNum[mCurBuffer]++;
}

void ProcessMeterBarBase::endSection_(s32 idx, const TickTime& t)
{
    SEAD_ASSERT(idx >= 0 && idx < mSectionList[0].getSize());

    Section* sec = getCurSection_(idx);
    SEAD_ASSERT(sec->span.toS64() == -1);

    sec->span = t.diff(sec->begin);
}

ProcessMeterBarBase::Section* ProcessMeterBarBase::getCurSection_(s32 idx)
{
    SEAD_ASSERT(idx >= 0 && idx < mSectionList[0].getSize());
    return mSectionList[mCurBuffer].get(idx);
}

} // namespace sead
