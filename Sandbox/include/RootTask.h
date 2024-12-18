#pragma once

#include <framework/seadCalculateTask.h>

class RootTask : public sead::CalculateTask
{
public:
    explicit RootTask(const sead::TaskConstructArg& arg);

    void prepare() override;
    void enter() override;
    void exit() override;
    void calc() override;
};
