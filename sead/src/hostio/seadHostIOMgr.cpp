#include <hostio/seadHostIOMgr.h>

#include <hostio/seadHostIOFramework.h>
#include <hostio/seadHostIORoot.h>

namespace sead {

SEAD_TASK_SINGLETON_DISPOSER_IMPL(HostIOMgr);

HostIOMgr::HostIOMgr(const TaskConstructArg& arg)
    : CalculateTask(arg, "sead::hostio::HostIOMgr")
    , mSeadRoot(nullptr)
{
}

HostIOMgr::HostIOMgr()
    : CalculateTask(TaskConstructArg(), "sead::hostio::HostIOMgr")
    , mSeadRoot(nullptr)
{
}

void HostIOMgr::prepare()
{
    Parameter* parameter = getParameter<Parameter>();

    mSeadRoot = new HostIORoot();

    bool isAutoRootNode = true;
    if (parameter)
    {
        hostio::Init(nullptr, parameter->getConfig());
        isAutoRootNode = parameter->isAutoRootNode();
    }
    else
    {
        hostio::Init(nullptr, nullptr);
    }

    if (isAutoRootNode)
    {
        hostio::SetRootNode("sead", mSeadRoot, "Icon=SEAD , Order=-1");
    }
}

void HostIOMgr::exit()
{
    hostio::Finalize();

    delete mSeadRoot;
    mSeadRoot = nullptr;
}

void HostIOMgr::calc()
{
    hostio::MessageLoop();
}

} // namespace sead
