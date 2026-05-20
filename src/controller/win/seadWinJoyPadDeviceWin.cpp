#include <controller/win/seadWinJoyPadDeviceWin.h>

#include <basis/win/seadWindows.h>
#include <container/seadSafeArray.h>

#include <mmsystem.h>

namespace sead {

WinJoyPadDevice::WinJoyPadDevice(ControllerMgr* mgr)
    : ControlDevice(mgr)
    , mPads()
{
    mId = ControllerDefine::DeviceId::eWinJoyPad;
}

void WinJoyPadDevice::initialize(Heap* heap)
{
    // mPads.freeBuffer();

    // SafeArray<JOYCAPSA, cNumMax> jcs;
    // s32 enablePadNum = 0;
    // for (s32 i = 0; i < cNumMax; i++)
    // {
    //     if (joyGetDevCapsA(i, &jcs[i], sizeof(JOYCAPSA)) == JOYERR_NOERROR && jcs[i].wNumButtons > 0)
    //     {
    //         enablePadNum++;
    //     }
    //     else
    //     {
    //         jcs[i].wNumButtons = 0;
    //     }
    // }

    // if (enablePadNum > 0)
    // {
    //     mPads.allocBuffer(enablePadNum, heap);

    //     u32 padNum = 0;
    //     for (s32 i = 0; i < cNumMax; i++)
    //     {
    //         if (jcs[i].wNumButtons > 0)
    //         {
    //             PadStatus& status = mPads[padNum];

    //             static_assert(sizeof(status.caps) >= sizeof(JOYCAPSA));
    //             static_assert(sizeof(status.info) >= sizeof(JOYINFOEX));

    //             *(reinterpret_cast<JOYCAPSA*>(status.caps)) = jcs[i];
    //             reinterpret_cast<JOYINFOEX*>(status.info)->dwSize = sizeof(JOYINFOEX);
    //             reinterpret_cast<JOYINFOEX*>(status.info)->dwFlags = JOY_RETURNALL;
    //             status.isEnable = false;
    //             status.id = i;

    //             padNum++;
    //             if (padNum >= enablePadNum)
    //             {
    //                 break;
    //             }
    //         }
    //     }
    // }
}

void WinJoyPadDevice::calc()
{
    // for (s32 i = 0; i < mPadNum; i++)
    // {
    //     mPads[i].isEnable = joyGetPosEx(mPads[i].id, reinterpret_cast<JOYINFOEX*>(mPads[i].info)) == JOYERR_NOERROR;
    // }
}

} // namespace sead
