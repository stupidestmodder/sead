#pragma once

#include <geom/seadLine.h>
#include <gfx/seadGraphics.h>
#include <math/seadBoundBox.h>

namespace sead {

class Camera;
class DrawContext;
class LogicalFrameBuffer;
class Projection;

class Viewport : public BoundBox2f
{
public:
    Viewport();
    Viewport(f32 x, f32 y, f32 w, f32 h);
    explicit Viewport(const BoundBox2f& box);
    explicit Viewport(const LogicalFrameBuffer& frameBuffer);

    virtual ~Viewport()
    {
    }

    void setByFrameBuffer(const LogicalFrameBuffer& frameBuffer);

    void apply(DrawContext* drawContext, const LogicalFrameBuffer& frameBuffer) const;
    void applyViewport(DrawContext* drawContext, const LogicalFrameBuffer& frameBuffer) const;
    void applyScissor(DrawContext* drawContext, const LogicalFrameBuffer& frameBuffer) const;

    void getOnFrameBufferPos(Vector2f* dst, const LogicalFrameBuffer& fb) const;
    void getOnFrameBufferSize(Vector2f* dst, const LogicalFrameBuffer& fb) const;

    void project(Vector2f* dst, const Vector3f& screenPos) const;
    void project(Vector2f* dst, const Vector2f& screenPos) const;

    void unproject(Vector3f* dst, const Vector2f& canvasPos, const Projection& projection, const Camera& camera) const;
    void unprojectRay(Ray3f* dst, const Vector2f& canvasPos, const Projection& projection, const Camera& camera) const;

    Graphics::DevicePosture getDevicePosture() const
    {
        return mDevicePos;
    }

    void setDevicePosture(Graphics::DevicePosture pos)
    {
        mDevicePos = pos;
    }

protected:
    void assertAreaIsInFrameBufferPhysicalArea_(const Vector2f& realPos, const Vector2f& realSize, const LogicalFrameBuffer& frameBuffer) const;

protected:
    Graphics::DevicePosture mDevicePos;
};

} // namespace sead
