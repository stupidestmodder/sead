#pragma once

#if defined(SEAD_USE_GL)
#include <gfx/gl/seadDrawContextGL.h>
#elif defined(SEAD_USE_WGPU)
#include <gfx/wgpu/seadDrawContextWGPU.h>
#else
#error "Unsupported GFX backend"
#endif
