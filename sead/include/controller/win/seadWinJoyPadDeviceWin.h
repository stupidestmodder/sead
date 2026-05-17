#pragma once

#include <container/seadBuffer.h>
#include <controller/seadControlDevice.h>

namespace sead {

// TODO
class WinJoyPadDevice : public ControlDevice
{
    SEAD_RTTI_OVERRIDE(WinJoyPadDevice, ControlDevice);

public:
    struct PadStatus
    {
        PadStatus()
            : isEnable(false)
            , id(0)
        {
        }

        u32 info[1];
        u32 caps[1];
        bool isEnable;
        u32 id;
    };

    static const s32 cNumMax = 4;

public:
    explicit WinJoyPadDevice(ControllerMgr* mgr);

    void initialize(Heap* heap);

    void calc() override;

    const PadStatus& getPads(s32 joyPadNumber) { return mPads[joyPadNumber]; }
    s32 getPadNum() const { return mPads.size(); }

protected:
    Buffer<PadStatus> mPads;
};

} // namespace sead
