#pragma once

namespace sead {

template <typename T>
struct BaseVec2
{
    BaseVec2(T _x, T _y)
        : x(_x)
        , y(_y)
    {
    }

    union
    {
        struct
        {
            T x;
            T y;
        };

        struct
        {
            T e[2];
        };

        struct
        {
            T u;
            T v;
        };
    };
};

template <typename T>
struct BaseVec3
{
    BaseVec3(T _x, T _y, T _z)
        : x(_x)
        , y(_y)
        , z(_z)
    {
    }

    union
    {
        struct
        {
            T x;
            T y;
            T z;
        };

        struct
        {
            T e[3];
        };

        struct
        {
            T u;
            T v;
            T w;
        };
    };
};

template <typename T>
struct BaseVec4
{
    BaseVec4(T _x, T _y, T _z, T _w)
        : x(_x)
        , y(_y)
        , z(_z)
        , w(_w)
    {
    }

    union
    {
        struct
        {
            T x;
            T y;
            T z;
            T w;
        };

        struct
        {
            T e[4];
        };
    };
};

template <typename T>
struct BaseQuat
{
    BaseQuat(T _x, T _y, T _z, T _w)
        : x(_x)
        , y(_y)
        , z(_z)
        , w(_w)
    {
    }

    T x;
    T y;
    T z;
    T w;
};

template <typename T>
struct BaseMtx22
{
    union
    {
        struct
        {
            T _00;
            T _01;
            T _10;
            T _11;
        };

        struct
        {
            T m[2][2];
        };

        struct
        {
            T a[4];
        };
    };
};

template <typename T>
struct BaseMtx33
{
    union
    {
        struct
        {
            T _00;
            T _01;
            T _02;
            T _10;
            T _11;
            T _12;
            T _20;
            T _21;
            T _22;
        };

        struct
        {
            T m[3][3];
        };

        struct
        {
            T a[9];
        };
    };
};

template <typename T>
struct BaseMtx34
{
    union
    {
        struct
        {
            T _00;
            T _01;
            T _02;
            T _03;
            T _10;
            T _11;
            T _12;
            T _13;
            T _20;
            T _21;
            T _22;
            T _23;
        };

        struct
        {
            T m[3][4];
        };

        struct
        {
            T a[12];
        };
    };
};

template <typename T>
struct BaseMtx44
{
    union
    {
        struct
        {
            T _00;
            T _01;
            T _02;
            T _03;
            T _10;
            T _11;
            T _12;
            T _13;
            T _20;
            T _21;
            T _22;
            T _23;
            T _30;
            T _31;
            T _32;
            T _33;
        };

        struct
        {
            T m[4][4];
        };

        struct
        {
            T a[16];
        };
    };
};

} // namespace sead
