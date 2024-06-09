#include <controller/win/seadWinControllerWin.h>

#include <controller/win/seadKeyboardMouseDeviceWin.h>
#include <controller/seadControllerMgr.h>

namespace sead {

const u8 WinController::cDefaultVKeyConfig[Controller::PadIdx::eMax] = {
    'A',        // PadIdx::eA
    'B',        // PadIdx::eB
    'C',        // PadIdx::eC
    'X',        // PadIdx::eX
    'Y',        // PadIdx::eY
    'Z',        // PadIdx::eZ
    '2',        // PadIdx::e2
    '1',        // PadIdx::e1
    'H',        // PadIdx::eHome
    'M',        // PadIdx::eMinus
    'P',        // PadIdx::ePlus
    'S',        // PadIdx::eStart
    'D',        // PadIdx::eSelect
    'L',        // PadIdx::eL
    'R',        // PadIdx::eR
    VK_LBUTTON, // PadIdx::eTouch
    VK_UP,      // PadIdx::eUp
    VK_DOWN,    // PadIdx::eDown
    VK_LEFT,    // PadIdx::eLeft
    VK_RIGHT,   // PadIdx::eRight
    0,          // PadIdx::eLeftStickUp
    0,          // PadIdx::eLeftStickDown
    0,          // PadIdx::eLeftStickLeft
    0,          // PadIdx::eLeftStickRight
    0,          // PadIdx::eRightStickUp
    0,          // PadIdx::eRightStickDown
    0,          // PadIdx::eRightStickLeft
    0           // PadIdx::eRightStickRight
};

const u8 WinController::cDefaultJoyPadConfig[Controller::PadIdx::eMax] = {
    4,  // PadIdx::eA
    3,  // PadIdx::eB
    7,  // PadIdx::eC
    2,  // PadIdx::eX
    1,  // PadIdx::eY
    8,  // PadIdx::eZ
    10, // PadIdx::e2
    9,  // PadIdx::e1
    0,  // PadIdx::eHome
    0,  // PadIdx::eMinus
    0,  // PadIdx::ePlus
    12, // PadIdx::eStart
    11, // PadIdx::eSelect
    5,  // PadIdx::eL
    6,  // PadIdx::eR
    0,  // PadIdx::eTouch
    0,  // PadIdx::eUp
    0,  // PadIdx::eDown
    0,  // PadIdx::eLeft
    0,  // PadIdx::eRight
    1,  // PadIdx::eLeftStickUp
    0,  // PadIdx::eLeftStickDown
    0,  // PadIdx::eLeftStickLeft
    0,  // PadIdx::eLeftStickRight
    1,  // PadIdx::eRightStickUp
    0,  // PadIdx::eRightStickDown
    0,  // PadIdx::eRightStickLeft
    0   // PadIdx::eRightStickRight
};

WinController::WinController(ControllerMgr* mgr, s32 joyPadNumber)
    : Controller(mgr)
    , mJoyPadNumber(joyPadNumber)
    , mStickClampMin(cStickClampMinDefault)
    , mStickClampMax(cStickClampMaxDefault)
    , mFlag(FlagMask::eIsJoyPadEnableWhenWindowInactive | FlagMask::eNormalizeAnalogStickAsCircle)
    , mCursorCenterOriginBound(1.0f)
{
    mId = ControllerDefine::ControllerId::eWin;

    setVKeyConfigDefault();
    setJoyPadConfigDefault();
}

void WinController::setVKeyConfig(const u8* keyConfig)
{
    MemUtil::copy(mVKeyConfig, keyConfig, PadIdx::eMax);
}

void WinController::setVKeyConfig(PadIdx bit, u8 vkey)
{
    if (bit >= PadIdx::eMax)
    {
        SEAD_ASSERT_MSG(false, "range over[%d]\n", static_cast<s32>(bit));
        return;
    }

    mVKeyConfig[bit] = vkey;
}

void WinController::setVKeyConfigDefault()
{
    MemUtil::copy(mVKeyConfig, cDefaultVKeyConfig, PadIdx::eMax);
}

bool WinController::isJoyPadEnable() const
{
    // TODO
    return false;
}

void WinController::getStickClamp(s32* min, s32* max) const
{
    SEAD_ASSERT(min);
    SEAD_ASSERT(max);

    *min = mStickClampMin;
    *max = mStickClampMax;
}

void WinController::setStickClamp(s32 min, s32 max)
{
    if (cStickClampValueMin <= min && min <= cStickClampValueMax &&
        cStickClampValueMin <= max && max <= cStickClampValueMax &&
        min < max)
    {
        mStickClampMin = min;
        mStickClampMax = max;
    }
    else
    {
        SEAD_ASSERT_MSG(false, "illegal value[%d/%d]", min, max);
    }
}

void WinController::setJoyPadConfig(const u8* joyPadConfig)
{
    MemUtil::copy(mJoyPadConfig, joyPadConfig, PadIdx::eMax);
}

void WinController::setJoyPadConfig(PadIdx bit, u8 no)
{
    if (bit >= PadIdx::eMax)
    {
        SEAD_ASSERT_MSG(false, "range over[%d]\n", static_cast<s32>(bit));
        return;
    }

    mJoyPadConfig[bit] = no;
}

void WinController::setJoyPadConfigDefault()
{
    MemUtil::copy(mJoyPadConfig, cDefaultJoyPadConfig, PadIdx::eMax);
}

void WinController::calcImpl_()
{
    KeyboardMouseDevice* kbDevice = DynamicCast<KeyboardMouseDevice>(mMgr->getControlDevice(ControllerDefine::DeviceId::eKeyboardMouse));
    WinJoyPadDevice* jpDevice = DynamicCast<WinJoyPadDevice>(mMgr->getControlDevice(ControllerDefine::DeviceId::eWinJoyPad));

    if (!kbDevice)
        return;

    mPadHold.makeAllZero();

    bool isActive = kbDevice->getActiveWindow() != KeyboardMouseDevice::ActiveWindow::eNone;

    if (isActive && kbDevice->isKeyEnable())
    {
        for (s32 i = 0; i <= PadIdx::eRight; i++)
        {
            if (kbDevice->isVKeyHold(mVKeyConfig[i]))
                mPadHold.setBit(i);
        }
    }

    bool isJoypadEnable = false;

    if (isJoyPadEnableWhenWindowInactive() || isActive)
    {
        // TODO
        SEAD_UNUSED(jpDevice);
    }

    if (!isJoypadEnable)
    {
        mLeftStick = Vector2f::zero;
        mRightStick = Vector2f::zero;
    }

    mLeftAnalogTrigger = 0.0f;
    mRightAnalogTrigger = 0.0f;

    if (kbDevice->getActiveWindow() == KeyboardMouseDevice::ActiveWindow::eMain)
    {
        if (isCursorCenterOrigin())
        {
            bool isPointerOn = false;

            f32 cursorX = kbDevice->getCursorClientAreaPosCenterOrigin().x;
            f32 cursorY = kbDevice->getCursorClientAreaPosCenterOrigin().y;
            if (-mCursorCenterOriginBound <= cursorX && cursorX <= mCursorCenterOriginBound &&
                -mCursorCenterOriginBound <= cursorY && cursorY <= mCursorCenterOriginBound)
            {
                isPointerOn = true;
            }

            setPointerWithBound_(isPointerOn, mPadHold.isOnBit(PadIdx::eTouch), kbDevice->getCursorClientAreaPosCenterOrigin());
        }
        else
        {
            setPointerWithBound_(kbDevice->isCursorOnClientArea(), mPadHold.isOnBit(PadIdx::eTouch), kbDevice->getCursorClientAreaPos());
        }
    }
    else
    {
        mPointerFlag.makeAllZero();

        mPointer = ControllerBase::cInvalidPointer;
        mPointerS32 = ControllerBase::cInvalidPointerS32;

        mPadHold.resetBit(mTouchKeyBit);
    }
}

} // namespace sead
