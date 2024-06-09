#pragma once

#include <container/seadOffsetList.h>
#include <controller/seadControllerBase.h>
#include <controller/seadControllerDefine.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class ControllerAddon;
class ControllerMgr;
class ControllerWrapperBase;

class Controller : public ControllerBase
{
    SEAD_RTTI_OVERRIDE(Controller, ControllerBase);

public:
    enum PadIdx
    {
        eA = 0,
        eB,
        eC,
        eX,
        eY,
        eZ,
        e2,
        e1,
        eHome,
        eMinus,
        ePlus,
        eStart,
        eSelect,
        eL,
        eR,
        eTouch,
        eUp,
        eDown,
        eLeft,
        eRight,
        eLeftStickUp,
        eLeftStickDown,
        eLeftStickLeft,
        eLeftStickRight,
        eRightStickUp,
        eRightStickDown,
        eRightStickLeft,
        eRightStickRight,
        eMax
    };

    enum PadMask
    {
        eMaskA               = 1 << PadIdx::eA,
        eMaskB               = 1 << PadIdx::eB,
        eMaskC               = 1 << PadIdx::eC,
        eMaskX               = 1 << PadIdx::eX,
        eMaskY               = 1 << PadIdx::eY,
        eMaskZ               = 1 << PadIdx::eZ,
        eMask2               = 1 << PadIdx::e2,
        eMask1               = 1 << PadIdx::e1,
        eMaskHome            = 1 << PadIdx::eHome,
        eMaskMinus           = 1 << PadIdx::eMinus,
        eMaskPlus            = 1 << PadIdx::ePlus,
        eMaskStart           = 1 << PadIdx::eStart,
        eMaskSelect          = 1 << PadIdx::eSelect,
        eMaskL               = 1 << PadIdx::eL,
        eMaskR               = 1 << PadIdx::eR,
        eMaskTouch           = 1 << PadIdx::eTouch,
        eMaskUp              = 1 << PadIdx::eUp,
        eMaskDown            = 1 << PadIdx::eDown,
        eMaskLeft            = 1 << PadIdx::eLeft,
        eMaskRight           = 1 << PadIdx::eRight,
        eMaskLeftStickUp     = 1 << PadIdx::eLeftStickUp,
        eMaskLeftStickDown   = 1 << PadIdx::eLeftStickDown,
        eMaskLeftStickLeft   = 1 << PadIdx::eLeftStickLeft,
        eMaskLeftStickRight  = 1 << PadIdx::eLeftStickRight,
        eMaskRightStickUp    = 1 << PadIdx::eRightStickUp,
        eMaskRightStickDown  = 1 << PadIdx::eRightStickDown,
        eMaskRightStickLeft  = 1 << PadIdx::eRightStickLeft,
        eMaskRightStickRight = 1 << PadIdx::eRightStickRight,
    };

public:
    explicit Controller(ControllerMgr* mgr);

    virtual ~Controller()
    {
    }

    virtual void calc();
    virtual bool isConnected() const { return true; }

    ControllerDefine::ControllerId getId() const { return mId; }
    ControllerAddon* getAddon(ControllerDefine::AddonId id) const;
    ControllerAddon* getAddonByOrder(ControllerDefine::AddonId id, s32 index) const;

protected:
    virtual void calcImpl_() = 0;
    virtual bool isIdle_();
    virtual void setIdle_();

protected:
    ControllerDefine::ControllerId mId;
    ControllerMgr* mMgr;
    OffsetList<ControllerAddon> mAddons;
    OffsetList<ControllerWrapperBase> mWrappers;

    friend class ControllerMgr;
    friend class ControllerWrapper;
    friend class ControllerWrapperBase;
};

} // namespace sead
