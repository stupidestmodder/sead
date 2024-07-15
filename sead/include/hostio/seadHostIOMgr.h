#pragma once

#include <framework/seadCalculateTask.h>
#include <framework/seadTaskMgr.h>
#include <framework/seadTaskParameter.h>
#include <hostio/seadHostIOConfig.h>

namespace sead {

class HostIORoot;

class HostIOMgr : public CalculateTask
{
    SEAD_TASK_SINGLETON_DISPOSER(HostIOMgr);

public:
    class Parameter : public TaskParameter
    {
        SEAD_RTTI_OVERRIDE(Parameter, TaskParameter);

    public:
        Parameter(); // TODO

        hostio::Config* getConfig()
        {
            return &mConfig;
        }

        bool isAutoRootNode() const
        {
            return mIsAutoRootNode;
        }

    private:
        hostio::Config mConfig;
        bool mIsAutoRootNode;
    };

public:
    HostIOMgr(const TaskConstructArg& arg);
    HostIOMgr();

    void prepare() override;
    void exit() override;
    void calc() override;

private:
    HostIORoot* mSeadRoot;
};

} // namespace sead
