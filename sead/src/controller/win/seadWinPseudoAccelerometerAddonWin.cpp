#include <controller/win/seadWinPseudoAccelerometerAddonWin.h>

#include <controller/win/seadKeyboardMouseDeviceWin.h>
#include <controller/seadController.h>
#include <controller/seadControllerMgr.h>

namespace sead {

WinPseudoAccelerometerAddon::WinPseudoAccelerometerAddon(Controller* controller)
    : AccelerometerAddon(controller)
    , mUnkFloat(100.0f)
    , mUnkButton1('Q')
    , mUnkButton2('W')
    , mUnkVec1(0, 0)
    , mUnkVec2(0, 0)
{
}

bool WinPseudoAccelerometerAddon::calc()
{
    KeyboardMouseDevice* keyboardMouseDevice = DynamicCast<KeyboardMouseDevice>(mController->getMgr()->getControlDevice(ControllerDefine::DeviceId::eKeyboardMouse));
    if (!keyboardMouseDevice || !keyboardMouseDevice->isKeyEnable() || !keyboardMouseDevice->isCursorEnable())
    {
        mIsEnable = false;
        return false;
    }

    if (keyboardMouseDevice->isVkeyTrig(mUnkButton1))
    {
        mAcceleration.x = 0.0f;
        mAcceleration.y = 0.0f;
        mUnkVec1.set(
            static_cast<s32>(keyboardMouseDevice->getCursorScreenPos().x),
            static_cast<s32>(keyboardMouseDevice->getCursorScreenPos().y)
        );
    }
    else if (keyboardMouseDevice->isVkeyHold(mUnkButton1))
    {
        mAcceleration.x = (keyboardMouseDevice->getCursorScreenPos().x - mUnkVec1.x) / mUnkFloat;
        mAcceleration.y = (keyboardMouseDevice->getCursorScreenPos().y - mUnkVec1.y) / mUnkFloat;
    }

    if (keyboardMouseDevice->isVkeyTrig(mUnkButton2))
    {
        mAcceleration.z = 0.0f;
        mUnkVec2.set(
            static_cast<s32>(keyboardMouseDevice->getCursorScreenPos().x),
            static_cast<s32>(keyboardMouseDevice->getCursorScreenPos().y)
        );
    }
    else if (keyboardMouseDevice->isVkeyHold(mUnkButton2))
    {
        mAcceleration.z = (keyboardMouseDevice->getCursorScreenPos().y - mUnkVec2.y) / mUnkFloat;
    }

    mIsEnable = true;
    return false;
}

} // namespace sead
