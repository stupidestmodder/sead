#pragma once

#include <basis/seadTypes.h>

#if defined(SEAD_PLATFORM_WINDOWS)
#include <basis/win/seadWindows.h>
#endif // SEAD_PLATFORM_WINDOWS

namespace sead {

// TODO: SEAD_ENUM()
enum CoreId
{
    cMain = 0,
    cSub1,
    cSub2,
    cSub3,
    cSub4,
    cSub5,
    cSub6,
    cSub7,
    cSub8,
    cSub9,
    cSub10,
    cSub11,
    cSub12,
    cSub13,
    cSub14,
    cSub15,
    cSub16,
    cSub17,
    cSub18,
    cSub19,
    cSub20,
    cSub21,
    cSub22,
    cSub23,
    cSub24,
    cSub25,
    cSub26,
    cSub27,
    cSub28,
    cSub29,
    cSub30,
    cSub31,
    cSub32,
    cUndef
};

class CoreIdMask
{
public:
    CoreIdMask()
        : mMask(0)
    {
    }

    explicit CoreIdMask(CoreId id)
    {
        mMask = getCoreMask_(id);
    }

    CoreIdMask(CoreId id1, CoreId id2)
    {
        mMask = getCoreMask_(id1) |
                getCoreMask_(id2);
    }

    CoreIdMask(CoreId id1, CoreId id2, CoreId id3)
    {
        mMask = getCoreMask_(id1) |
                getCoreMask_(id2) |
                getCoreMask_(id3);
    }

    void set(u32 mask) volatile
    {
        mMask = mask;
    }

    operator u32() const volatile
    {
        return mMask;
    }

    volatile CoreIdMask& OR(CoreId id) volatile
    {
        mMask |= getCoreMask_(id);
        return *this;
    }

    bool isOn(const CoreId& id) const volatile
    {
        return mMask & getCoreMask_(id);
    }

    void setOff(CoreId id) volatile
    {
        mMask &= ~getCoreMask_(id);
    }

    void setOn(CoreId id) volatile
    {
        mMask |= getCoreMask_(id);
    }

    void clear() volatile
    {
        mMask = 0;
    }

    u32 countOnBits() const;

protected:
    u32 getCoreMask_(CoreId id) const volatile
    {
        return 1u << static_cast<s32>(id);
    }

protected:
    u32 mMask;
};

class CoreInfo
{
public:
    static void configure();
    static void dump();

    static u32 getNumCores()
    {
        return sNumCores;
    }

    static u32 getPlatformMainCoreId()
    {
        return sPlatformCoreId[CoreId::cMain];
    }

    static u32 getPlatformCoreId(CoreId id)
    {
        return sPlatformCoreId[id];
    }

    static CoreId getCurrentCoreId()
    {
#if defined(SEAD_PLATFORM_WINDOWS)
        return getCoreIdFromPlatformCoreId(GetCurrentProcessorNumber());
#else
    #error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
    }

    static u32 getPlatformMask(CoreId id);

    static CoreId getCoreIdFromPlatformCoreId(u32 id)
    {
        return sCoreIdFromPlatformCoreIdTable[id];
    }

    static u32 getPlatformMaskFromCoreIdMask(const CoreIdMask& mask)
    {
        return static_cast<u32>(mask);
    }

    static CoreIdMask getMaskAll()
    {
        CoreIdMask m;
        m.set((1 << sNumCores) - 1);
        return m;
    }

    static CoreIdMask getMaskSubAll()
    {
        CoreIdMask m;
        m.set(static_cast<u32>(getMaskAll()) - 1);
        return m;
    }

protected:
    static u32 sNumCores;
    static u32 sPlatformCoreId[32];
    static CoreId sCoreIdFromPlatformCoreIdTable[32];
};

} // namespace sead
