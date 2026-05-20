#pragma once

#include <gfx/seadColor.h>
#include <math/seadVector.h>

// TODO
namespace sead { namespace PrimitiveDrawUtil {

struct Vertex
{
    Vector3f pos;
    Vector2f uv;
    Color4f color;
};

void setQuadVertex(Vertex* vtx, u16* idx);
void setLineVertex(Vertex* vtx, u16* idx);
void setCubeVertex(Vertex* vtx, u16* idx);
void setWireCubeVertex(Vertex* vtx, u16* idx);
void setSphereVertex(Vertex* vtx, u16* idx, s32 sizeX, s32 sizeY);
void setDiskVertex(Vertex* vtx, u16* idx, s32 divNum);
void setCylinderVertex(Vertex* vtx, u16* idx, s32 divNum);

inline s32 calcSphereVertexNum(s32 x, s32 y) { return (x * y) + 2; }
inline s32 calcSphereIndexNum(s32 x, s32 y) { return 3 * (x + x * 2 * (y - 1) + x); }
// ...

} } // namespace sead::PrimitiveDrawUtil
