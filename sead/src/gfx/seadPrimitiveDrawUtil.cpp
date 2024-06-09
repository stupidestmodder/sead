#include <gfx/seadPrimitiveDrawUtil.h>

#include <prim/seadMemUtil.h>

namespace sead { namespace PrimitiveDrawUtil {

void setQuadVertex(Vertex* vtx, u16* idx)
{
    static const Vertex cVtx[4] = {
        { Vector3f(-0.5f,  0.5f, 0.0f), Vector2f(0.0f, 1.0f), Color4f(0.0f, 0.0f, 0.0f, 0.0f) },
        { Vector3f( 0.5f,  0.5f, 0.0f), Vector2f(1.0f, 1.0f), Color4f(0.0f, 0.0f, 0.0f, 0.0f) },
        { Vector3f(-0.5f, -0.5f, 0.0f), Vector2f(0.0f, 0.0f), Color4f(1.0f, 0.0f, 0.0f, 0.0f) },
        { Vector3f( 0.5f, -0.5f, 0.0f), Vector2f(1.0f, 0.0f), Color4f(1.0f, 0.0f, 0.0f, 0.0f) }
    };

    static const u16 cIdx[6] = {
        0, 2, 1,
        1, 2, 3
    };

    if (vtx)
        MemUtil::copy(vtx, cVtx, sizeof(cVtx));

    if (idx)
        MemUtil::copy(idx, cIdx, sizeof(cIdx));
}

void setLineVertex(Vertex* vtx, u16* idx)
{
    static const Vertex cVtx[2] = {
        { Vector3f(-0.5f,  0.0f, 0.0f), Vector2f(0.0f, 0.5f), Color4f(0.0f, 0.0f, 0.0f, 0.0f) },
        { Vector3f( 0.5f,  0.0f, 0.0f), Vector2f(1.0f, 0.5f), Color4f(1.0f, 0.0f, 0.0f, 0.0f) }
    };

    static const u16 cIdx[2] = {
        0, 1
    };

    if (vtx)
        MemUtil::copy(vtx, cVtx, sizeof(cVtx));

    if (idx)
        MemUtil::copy(idx, cIdx, sizeof(cIdx));
}

} } // namespace sead::PrimitiveDrawUtil
