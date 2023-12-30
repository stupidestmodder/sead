#pragma once

#include <heap/seadDisposer.h>
#include <prim/seadNamable.h>

namespace sead {

// TODO
class ProcessMeterBarBase : public IDisposer, public INamable
{
};

// TODO
template <s32>
class MultiProcessMeterBar : public ProcessMeterBarBase
{
};

} // namespace sead
