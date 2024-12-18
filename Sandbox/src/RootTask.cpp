#include <RootTask.h>

RootTask::RootTask(const sead::TaskConstructArg& arg)
    : sead::CalculateTask(arg, "RootTask")
{
}

void RootTask::prepare()
{
    adjustHeapAll();
}

void RootTask::enter()
{
}

void RootTask::exit()
{
}

void RootTask::calc()
{
    SEAD_PRINT("RootTask::calc()\n");
}
