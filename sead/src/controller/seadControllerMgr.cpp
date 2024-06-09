#include <controller/seadControllerMgr.h>

#include <controller/seadControlDevice.h>
#include <controller/seadController.h>

#ifdef SEAD_PLATFORM_WINDOWS
#include <controller/win/seadKeyboardMouseDeviceWin.h>
#include <controller/win/seadWinControllerWin.h>
#include <controller/win/seadWinJoyPadDeviceWin.h>
#endif // SEAD_PLATFORM_WINDOWS

class TemporallyTaskConstructArg
{
public:
    TemporallyTaskConstructArg()
        : mTaskConstructArg()
        , mHeapArray()
    {
        mTaskConstructArg.heap_array = &mHeapArray;
    }

    const sead::TaskConstructArg& getArg() const { return mTaskConstructArg; }

private:
    sead::TaskConstructArg mTaskConstructArg;
    sead::HeapArray mHeapArray;
};

namespace sead {

SEAD_TASK_SINGLETON_DISPOSER_IMPL(ControllerMgr);

ControllerMgr::ControllerMgr(const TaskConstructArg& arg)
    : CalculateTask(arg, "sead::ControllerMgr")
    , mDevices()
    , mControllers()
{
    mDevices.initOffset(offsetof(ControlDevice, mListNode));
}

ControllerMgr::ControllerMgr()
    : CalculateTask(TemporallyTaskConstructArg().getArg(), "sead::ControllerMgr")
    , mDevices()
    , mControllers()
{
    mDevices.initOffset(offsetof(ControlDevice, mListNode));
}

void ControllerMgr::prepare()
{
    Parameter* param = getParameter<Parameter>();
    if (param)
    {
        initialize(param->controllerMax, nullptr);
        if (param->proc)
            param->proc->invoke(this);
    }
    else
    {
        initializeDefault(nullptr);
    }
}

void ControllerMgr::initialize(s32 controllerMax, Heap* heap)
{
    mControllers.allocBuffer(controllerMax, heap);
}

void ControllerMgr::finalize()
{
    mControllers.freeBuffer();
}

void ControllerMgr::initializeDefault(Heap* heap)
{
#ifdef SEAD_PLATFORM_WINDOWS
    initialize(5, heap);

    {
        mDevices.pushBack(new(heap) KeyboardMouseDevice(this));

        WinJoyPadDevice* jpDevice = new(heap) WinJoyPadDevice(this);
        jpDevice->initialize(heap);
        mDevices.pushBack(jpDevice);
    }

    {
        WinController* ctrl = new(heap) WinController(this, 0);
        mControllers.pushBack(ctrl);
        // TODO
        //ctrl->mAddons.pushBack(new(heap) WinPseudoAccelerometerAddon(ctrl));
    }
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
}

void ControllerMgr::finalizeDefault()
{
#ifdef SEAD_PLATFORM_WINDOWS
    // TODO
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
}

void ControllerMgr::calc()
{
    for (ControlDevice& device : mDevices)
    {
        device.calc();
    }

    for (Controller& ctrl : mControllers)
    {
        ctrl.calc();
    }
}

Controller* ControllerMgr::getControllerByOrder(ControllerDefine::ControllerId id, s32 index) const
{
    for (Controller& ctrl : mControllers)
    {
        if (ctrl.getId() == id)
        {
            if (index == 0)
                return &ctrl;

            index--;
        }
    }

    return nullptr;
}

s32 ControllerMgr::findControllerPort(const Controller* controller) const
{
    SEAD_ASSERT(controller);

    s32 i = 0;
    for (Controller& ctrl : mControllers)
    {
        if (&ctrl == controller)
            return i;

        i++;
    }

    return -1;
}

ControlDevice* ControllerMgr::getControlDevice(ControllerDefine::DeviceId id) const
{
    for (ControlDevice& device : mDevices)
    {
        if (device.getId() == id)
            return &device;
    }

    return nullptr;
}

ControllerAddon* ControllerMgr::getControllerAddon(s32 port, ControllerDefine::AddonId id) const
{
    Controller* controller = mControllers.at(port);
    if (!controller)
        return nullptr;

    return controller->getAddon(id);
}

ControllerAddon* ControllerMgr::getControllerAddonByOrder(s32 port, ControllerDefine::AddonId id, s32 index) const
{
    Controller* controller = mControllers.at(port);
    if (!controller)
        return nullptr;

    return controller->getAddonByOrder(id, index);
}

Framework* ControllerMgr::getFramework() const
{
    if (!mTaskMgr)
        return nullptr;

    return mTaskMgr->getFramework();
}

} // namespace sead
