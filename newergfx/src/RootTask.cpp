#include "RootTask.h"

#include <basis/seadRawPrint.h>

RootTask::RootTask(const sead::TaskConstructArg& arg)
    : sead::Task(arg, "RootTask")
{
}

void RootTask::prepare()
{
    adjustHeapAll();
}

void RootTask::calc()
{
    SEAD_PRINT("CALC !!!");
}

void RootTask::draw()
{
    SEAD_PRINT("DRAW !!!");
}
