#include <gfx/seadViewport.h>

#include <gfx/seadFrameBuffer.h>
#include <gfx/seadProjection.h>

namespace sead {

Viewport::Viewport()
    : BoundBox2f()
    , mDevicePos(Graphics::getDefaultDevicePosture())
{
}

Viewport::Viewport(f32 x, f32 y, f32 w, f32 h)
    : BoundBox2f(x, y, x + w, y + h)
    , mDevicePos(Graphics::getDefaultDevicePosture())
{
}

Viewport::Viewport(const BoundBox2f& box)
    : BoundBox2f(box)
    , mDevicePos(Graphics::getDefaultDevicePosture())
{
}

Viewport::Viewport(const LogicalFrameBuffer& frameBuffer)
    : BoundBox2f()
    , mDevicePos(Graphics::getDefaultDevicePosture())
{
    setByFrameBuffer(frameBuffer);
}

void Viewport::setByFrameBuffer(const LogicalFrameBuffer& frameBuffer)
{
    switch (mDevicePos)
    {
        case Graphics::DevicePosture::eSame:
        case Graphics::DevicePosture::eFlipXY:
        case Graphics::DevicePosture::eFlipX:
        case Graphics::DevicePosture::eFlipY:
            set(0.0f, 0.0f, frameBuffer.getVirtualSize().x, frameBuffer.getVirtualSize().y);
            break;

        case Graphics::DevicePosture::eRotateRight:
        case Graphics::DevicePosture::eRotateLeft:
            set(0.0f, 0.0f, frameBuffer.getVirtualSize().y, frameBuffer.getVirtualSize().x);
            break;

        default:
            SEAD_ASSERT_MSG(false, "Undefined DevicePosture(%d)", static_cast<s32>(mDevicePos));
            break;
    }
}

void Viewport::apply(DrawContext* drawContext, const LogicalFrameBuffer& frameBuffer) const
{
#if SEAD_GFX_BACKEND == GL_GFX_GL
    SEAD_UNUSED(drawContext);

    Vector2f realPos;
    getOnFrameBufferPos(&realPos, frameBuffer);

    Vector2f realSize;
    getOnFrameBufferSize(&realSize, frameBuffer);

    assertAreaIsInFrameBufferPhysicalArea_(realPos, realSize, frameBuffer);

    //realPos.y = (frameBuffer.getPhysicalArea().getSizeY() - realSize.y) - realPos.y;

    Graphics::instance()->setViewportRealPosition(realPos.x, realPos.y, realSize.x, realSize.y);
    Graphics::instance()->setScissorRealPosition(realPos.x, realPos.y, realSize.x, realSize.y);
#else
#error "Unsupported GFX backend"
#endif
}

void Viewport::applyViewport(DrawContext* drawContext, const LogicalFrameBuffer& frameBuffer) const
{
#if SEAD_GFX_BACKEND == GL_GFX_GL
    SEAD_UNUSED(drawContext);

    Vector2f realPos;
    getOnFrameBufferPos(&realPos, frameBuffer);

    Vector2f realSize;
    getOnFrameBufferSize(&realSize, frameBuffer);

    assertAreaIsInFrameBufferPhysicalArea_(realPos, realSize, frameBuffer);

    //realPos.y = (frameBuffer.getPhysicalArea().getSizeY() - realSize.y) - realPos.y;

    Graphics::instance()->setViewportRealPosition(realPos.x, realPos.y, realSize.x, realSize.y);
#else
#error "Unsupported GFX backend"
#endif
}

void Viewport::applyScissor(DrawContext* drawContext, const LogicalFrameBuffer& frameBuffer) const
{
#if SEAD_GFX_BACKEND == GL_GFX_GL
    SEAD_UNUSED(drawContext);

    Vector2f realPos;
    getOnFrameBufferPos(&realPos, frameBuffer);

    Vector2f realSize;
    getOnFrameBufferSize(&realSize, frameBuffer);

    assertAreaIsInFrameBufferPhysicalArea_(realPos, realSize, frameBuffer);

    //realPos.y = (frameBuffer.getPhysicalArea().getSizeY() - realSize.y) - realPos.y;

    Graphics::instance()->setScissorRealPosition(realPos.x, realPos.y, realSize.x, realSize.y);
#else
#error "Unsupported GFX backend"
#endif
}

void Viewport::getOnFrameBufferPos(Vector2f* dst, const LogicalFrameBuffer& fb) const
{
    *dst = getMin();

    switch (mDevicePos)
    {
        case Graphics::DevicePosture::eSame:
            break;

        case Graphics::DevicePosture::eRotateRight:
            dst->set(dst->y, (fb.getVirtualSize().y - getSizeX()) - dst->x);
            break;

        case Graphics::DevicePosture::eRotateLeft:
            dst->set((fb.getVirtualSize().x - getSizeY()) - dst->y, dst->x);
            break;

        case Graphics::DevicePosture::eFlipXY:
            dst->set((fb.getVirtualSize().x - getSizeX()) - dst->x, (fb.getVirtualSize().y - getSizeY()) - dst->y);
            break;

        case Graphics::DevicePosture::eFlipX:
            dst->set((fb.getVirtualSize().x - getSizeX()) - dst->x, dst->y);
            break;

        case Graphics::DevicePosture::eFlipY:
            dst->set(dst->x, (fb.getVirtualSize().y - getSizeY()) - dst->y);
            break;

        default:
            SEAD_ASSERT_MSG(false, "Undefined DevicePosture(%d)", static_cast<s32>(mDevicePos));
    }

    dst->div(fb.getVirtualSize());

    dst->x *= fb.getPhysicalArea().getSizeX();
    dst->y *= fb.getPhysicalArea().getSizeY();

    dst->add(fb.getPhysicalArea().getMin());
}

void Viewport::getOnFrameBufferSize(Vector2f* dst, const LogicalFrameBuffer& fb) const
{
    dst->set(getSizeX(), getSizeY());

    switch (mDevicePos)
    {
        case Graphics::DevicePosture::eSame:
        case Graphics::DevicePosture::eFlipXY:
        case Graphics::DevicePosture::eFlipX:
        case Graphics::DevicePosture::eFlipY:
            break;

        case Graphics::DevicePosture::eRotateRight:
        case Graphics::DevicePosture::eRotateLeft:
            dst->set(dst->y, dst->x);
            break;

        default:
            SEAD_ASSERT_MSG(false, "Undefined DevicePosture(%d)", static_cast<s32>(mDevicePos));
    }

    dst->div(fb.getVirtualSize());

    dst->x *= fb.getPhysicalArea().getSizeX();
    dst->y *= fb.getPhysicalArea().getSizeY();
}

/*void Viewport::project(Vector2f* dst, const Vector3f& screenPos) const
{
    Vector2f center = getCenter();

    dst->x = getHalfSizeX() * screenPos.x;
    dst->y = getHalfSizeY() * screenPos.y;
}

void Viewport::project(Vector2f* dst, const Vector2f& screenPos) const
{
    Vector2f center = getCenter();

    dst->x = getHalfSizeX() * screenPos.x;
    dst->y = getHalfSizeY() * screenPos.y;
}

void Viewport::unproject(Vector3f* dst, const Vector2f& canvasPos, const Projection& projection, const Camera& camera) const
{
    Vector3f screenPos;
    screenPos.x = canvasPos.x / getHalfSizeX();
    screenPos.y = canvasPos.y / getHalfSizeY();
    screenPos.z = 0.0f;

    projection.unproject(dst, screenPos, camera);
}

void Viewport::unprojectRay(Ray3f* dst, const Vector2f& canvasPos, const Projection& projection, const Camera& camera) const
{
    Vector3f screenPos;
    screenPos.x = canvasPos.x / getHalfSizeX();
    screenPos.y = canvasPos.y / getHalfSizeY();
    screenPos.z = 0.0f;

    projection.unprojectRay(dst, screenPos, camera);
}*/

void Viewport::assertAreaIsInFrameBufferPhysicalArea_(const Vector2f& realPos, const Vector2f& realSize, const LogicalFrameBuffer& frameBuffer) const
{
    SEAD_ASSERT_MSG(frameBuffer.getPhysicalArea().isInside(realPos) && frameBuffer.getPhysicalArea().isInside(realPos + realSize),
                    "illegal area and size. realPos = (%u, %u) realSize = (%u, %u) physicalArea = ((%f, %f)-(%f, %f))",
                    static_cast<u32>(realPos.x), static_cast<u32>(realPos.y), static_cast<u32>(realSize.x), static_cast<u32>(realSize.y),
                    frameBuffer.getPhysicalArea().getMin().x, frameBuffer.getPhysicalArea().getMin().y,
                    frameBuffer.getPhysicalArea().getMax().x, frameBuffer.getPhysicalArea().getMax().y);
}

} // namespace sead
