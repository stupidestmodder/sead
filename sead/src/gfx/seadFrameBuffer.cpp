#include <gfx/seadFrameBuffer.h>

namespace sead {

void FrameBuffer::copyToDisplayBuffer(DrawContext*, const DisplayBuffer*) const
{
}

void FrameBuffer::clearMRT(DrawContext*, u32, const Color4f&) const
{
}

void FrameBuffer::bind(DrawContext* drawContext) const
{
    bindImpl_(drawContext);
}

} // namespace sead
