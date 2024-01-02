#pragma once

#include <framework/win/seadConsoleFrameworkWin.h>

class AppFramework : public sead::ConsoleFrameworkWin
{
    SEAD_RTTI_OVERRIDE(AppFramework, sead::ConsoleFrameworkWin);

public:
    AppFramework()
        : sead::ConsoleFrameworkWin()
    {
    }

    static void initialize(const InitializeArg& arg)
    {
        sead::ConsoleFrameworkWin::initialize(arg);
    }
};
