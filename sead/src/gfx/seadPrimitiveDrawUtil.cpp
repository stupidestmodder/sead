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

// ...

void setSphereVertex(Vertex* vtx, u16* idx, s32 sizeX, s32 sizeY)
{
    if (vtx)
    {
        for (s32 y = 0; y < sizeY; y++)
        {
            f32 angle_y = ((y + 1) / (sizeY + 1.0f) - 0.5f) * Mathf::deg2rad(180);

            f32 pos_y = Mathf::sin(angle_y) * 0.5f;
            f32 radius = Mathf::cos(angle_y) * 0.5f;

            for (s32 x = 0; x < sizeX; x++)
            {
                s32 pos = y * sizeX + x;
                f32 angle_x = Mathf::deg2rad(360) * x / sizeX;
                if (y % 2 == 0)
                    angle_x -= Mathf::deg2rad(360) / sizeX / 2;

                f32 pos_x = Mathf::cos(angle_x) * radius;
                f32 pos_z = Mathf::sin(angle_x) * radius;

                vtx[pos].pos.set(pos_x, pos_y, pos_z);
                vtx[pos].uv.set(pos_y + 0.5f, static_cast<f32>(x) / sizeX);
                vtx[pos].color.r = -pos_y + 0.5f;
            }
        }

        {
            s32 pos = sizeX * sizeY;

            vtx[pos].pos.set(0.0f, -0.5f, 0.0f);
            vtx[pos].uv.set(0.0f, 0.5f);
            vtx[pos].color.r = 1.0f;
        }

        {
            s32 pos = sizeX * sizeY + 1;

            vtx[pos].pos.set(0.0f, 0.5f, 0.0f);
            vtx[pos].uv.set(1.0f, 0.5f);
            vtx[pos].color.r = 0.0f;
        }
    }

    if (idx)
    {
        for (s32 i = 0; i < sizeX; i++)
        {
          idx[i * 3 + 0] = sizeX * sizeY;
          idx[i * 3 + 1] = i;
          idx[i * 3 + 2] = (i + 1) % sizeX;
        }

        for (s32 y = 0; y < sizeY - 1; y++)
        {
            for (s32 x = 0; x < sizeX; x++)
            {
                s32 offset = y % 2;
                s32 pos = (y * sizeX * 6) + x * 6 + sizeX * 3;

                idx[pos + 0] = y * sizeX + x;
                idx[pos + 1] = (y + 1) * sizeX + ((x + offset) % sizeX);
                idx[pos + 2] = y * sizeX + ((x + 1) % sizeX);
                idx[pos + 3] = (y + 1) * sizeX + ((x + offset) % sizeX);
                idx[pos + 4] = (y + 1) * sizeX + ((x + 1 + offset) % sizeX);
                idx[pos + 5] = y * sizeX + ((x + 1) % sizeX);
            }
        }

        for (s32 i = 0; i < sizeX; i++)
        {
            s32 posOffs = 3 * sizeX * (sizeY - 1) * 2 + sizeX * 3;

            idx[i * 3 + 0 + posOffs] = sizeX * sizeY + 1;
            idx[i * 3 + 1 + posOffs] = sizeX * (sizeY - 1) + ((i + 1) % sizeX);
            idx[i * 3 + 2 + posOffs] = sizeX * (sizeY - 1) + i;
        }
    }
}

} } // namespace sead::PrimitiveDrawUtil
