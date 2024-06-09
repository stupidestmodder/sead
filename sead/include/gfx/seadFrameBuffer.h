#pragma once

#include <gfx/seadColor.h>
#include <math/seadBoundBox.h>
#include <math/seadVector.h>
#include <prim/seadRuntimeTypeInfo.h>

namespace sead {

class DisplayBuffer;
class DrawContext;
class Heap;

class LogicalFrameBuffer
{
    SEAD_RTTI_BASE(LogicalFrameBuffer);

public:
    LogicalFrameBuffer()
        : mVirtualSize(1.0f, 1.0f)
        , mPhysicalArea(0.0f, 0.0f, 1.0f, 1.0f)
    {
    }

    LogicalFrameBuffer(const Vector2f& virtualSize, const BoundBox2f& physicalArea)
        : mVirtualSize(virtualSize)
        , mPhysicalArea(physicalArea)
    {
    }

    LogicalFrameBuffer(const Vector2f& virtualSize, f32 physicalX, f32 physicalY, f32 physicalW, f32 physicalH)
        : mVirtualSize(virtualSize)
        , mPhysicalArea(physicalX, physicalY, physicalX + physicalW, physicalY + physicalH)
    {
    }

    virtual ~LogicalFrameBuffer()
    {
    }

    const Vector2f& getVirtualSize() const
    {
        return mVirtualSize;
    }

    const BoundBox2f& getPhysicalArea() const
    {
        return mPhysicalArea;
    }

    void setVirtualSize(f32 x, f32 y)
    {
        mVirtualSize.set(x, y);
    }

    void setVirtualSize(const Vector2f& virtualSize)
    {
        mVirtualSize = virtualSize;
    }

    void setPhysicalArea(const BoundBox2f& phSize)
    {
        mPhysicalArea = phSize;
    }

    void setPhysicalArea(f32 x, f32 y, f32 w, f32 h)
    {
        mPhysicalArea.set(x, y, x + w, y + h);
    }

protected:
    Vector2f mVirtualSize;
    BoundBox2f mPhysicalArea;
};

class FrameBuffer : public LogicalFrameBuffer
{
    SEAD_RTTI_OVERRIDE(FrameBuffer, LogicalFrameBuffer);

public:
    enum ClearFlag
    {
        eNone    = 0,
        eColor   = 1 << 0,
        eDepth   = 1 << 1,
        eStencil = 1 << 2,
        eAll     = eColor | eDepth | eStencil
    };

public:
    FrameBuffer()
        : LogicalFrameBuffer()
    {
    }

    FrameBuffer(const Vector2f& virtualSize, const BoundBox2f& physicalArea)
        : LogicalFrameBuffer(virtualSize, physicalArea)
    {
    }

    FrameBuffer(const Vector2f& virtualSize, f32 physicalX, f32 physicalY, f32 physicalW, f32 physicalH)
        : LogicalFrameBuffer(virtualSize, physicalX, physicalY, physicalW, physicalH)
    {
    }

    virtual void copyToDisplayBuffer(DrawContext* drawContext, const DisplayBuffer* displayBuffer) const;
    virtual void clear(DrawContext* drawContext, u32 clrFlag, const Color4f& color, f32 depth, u32 stencil) const = 0;
    virtual void clearMRT(DrawContext* drawContext, u32 target, const Color4f& color) const;

    void bind(DrawContext* drawContext) const;

protected:
    virtual void bindImpl_(DrawContext* drawContext) const = 0;
};

class DisplayBuffer
{
    SEAD_RTTI_BASE(DisplayBuffer);

public:
    DisplayBuffer()
        : mWidth(0.0f)
        , mHeight(0.0f)
    {
    }

    virtual ~DisplayBuffer()
    {
    }

    void initialize(f32 width, f32 height, Heap* heap)
    {
        mWidth = width;
        mHeight = height;

        initializeImpl_(heap);
    }

    f32 getWidth() const
    {
        return mWidth;
    }

    f32 getHeight() const
    {
        return mHeight;
    }

protected:
    virtual void initializeImpl_(Heap* heap) = 0;

protected:
    f32 mWidth;
    f32 mHeight;
};

} // namespace sead
