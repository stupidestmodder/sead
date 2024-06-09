#pragma once

#include <container/seadOffsetList.h>
#include <container/seadPtrArray.h>
#include <controller/seadControllerDefine.h>
#include <framework/seadCalculateTask.h>
#include <framework/seadTaskMgr.h>
#include <framework/seadTaskParameter.h>

namespace sead {

class ControlDevice;
class Controller;
class ControllerAddon;

class ControllerMgr : public CalculateTask
{
    SEAD_TASK_SINGLETON_DISPOSER(ControllerMgr);

public:
    struct Parameter : public TaskParameter
    {
        SEAD_RTTI_OVERRIDE(Parameter, TaskParameter);

    public:
        Parameter()
            : TaskParameter()
            , controllerMax(0)
            , proc(nullptr)
        {
        }

        s32 controllerMax;
        IDelegate1<ControllerMgr*>* proc;
    };

public:
    explicit ControllerMgr(const TaskConstructArg& arg);
    ControllerMgr();

    void prepare() override;

    void initialize(s32 controllerMax, Heap* heap);
    void finalize();

    void initializeDefault(Heap* heap);
    void finalizeDefault();

    void calc() override;

    Controller* getController(s32 port) const
    {
        return mControllers.at(port);
    }

    PtrArray<Controller>& getControllerArray()
    {
        return mControllers;
    }

    Controller* getControllerByOrder(ControllerDefine::ControllerId id, s32 index) const;
    s32 findControllerPort(const Controller* controller) const;
    ControlDevice* getControlDevice(ControllerDefine::DeviceId id) const;
    ControllerAddon* getControllerAddon(s32 port, ControllerDefine::AddonId id) const;
    ControllerAddon* getControllerAddonByOrder(s32 port, ControllerDefine::AddonId id, s32 index) const;

    Framework* getFramework() const;

public:
    OffsetList<ControlDevice> mDevices;
    PtrArray<Controller> mControllers;
};

} // namespace sead
