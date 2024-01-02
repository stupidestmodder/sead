#pragma once

#include <basis/seadRawPrint.h>
#include <framework/seadTask.h>

class RootTask : public sead::Task
{
    SEAD_RTTI_OVERRIDE(RootTask, sead::Task);

public:
    RootTask(const sead::TaskConstructArg& arg)
        : sead::Task(arg, "RootTask")
    {
    }

    void prepare() override
    {
        adjustHeapAll();
    }

    void calc() override
    {
        SEAD_PRINT("CALC !!!");
    }

    void draw() override
    {
        SEAD_PRINT("DRAW !!!");
    }
};
