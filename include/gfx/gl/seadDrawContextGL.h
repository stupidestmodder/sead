#pragma once

#include <gfx/seadColor.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class Framework;

class DrawContext
{
    SEAD_RTTI_BASE(DrawContext);

public:
    DrawContext() {}
    virtual ~DrawContext() {}
};

} // namespace sead
