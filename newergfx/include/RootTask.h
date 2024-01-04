#pragma once

#include <framework/seadTask.h>

class RootTask : public sead::Task
{
    SEAD_RTTI_OVERRIDE(RootTask, sead::Task);

public:
    RootTask(const sead::TaskConstructArg& arg);

    void prepare() override;
    void calc() override;
    void draw() override;
};
