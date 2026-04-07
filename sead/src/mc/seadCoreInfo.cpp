#include <mc/seadCoreInfo.h>

#include <basis/seadRawPrint.h>

namespace sead {

u32 CoreInfo::sNumCores = 1;
u32 CoreInfo::sPlatformCoreId[32];
CoreId CoreInfo::sCoreIdFromPlatformCoreIdTable[32];

struct CoreInfoInitializer
{
    CoreInfoInitializer()
    {
        CoreInfo::configure();
    }
};

static CoreInfoInitializer sCoreInfoInitializer;

void CoreInfo::configure()
{
#if defined(SEAD_PLATFORM_WINDOWS)
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    sNumCores = sysInfo.dwNumberOfProcessors;

    sPlatformCoreId[CoreId::cMain] = 0;
    for (u32 i = 1; i < sNumCores; i++)
    {
        sPlatformCoreId[i] = i;
    }
#else
    #error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS

    for (u32 i = 0; i < sNumCores; i++)
    {
        u32 id = sPlatformCoreId[i];
        sCoreIdFromPlatformCoreIdTable[id] = static_cast<CoreId>(i);
    }
}

void CoreInfo::dump()
{
    SEAD_PRINT("* num cores %d\n", sNumCores);

    for (u32 i = 0; i < sNumCores; i++)
    {
        SEAD_PRINT("  [%d] : %s : PlatformCoreId=%d\n", i, i == 0 ? "Main" : "Sub ", sPlatformCoreId[i]);
    }

    SEAD_PRINT("all mask : %p\n", static_cast<u32>(getMaskAll()));
    SEAD_PRINT("all sub mask : %p\n", static_cast<u32>(getMaskSubAll()));
}

u32 CoreInfo::getPlatformMask(CoreId id)
{
    return 1 << getPlatformCoreId(id);
}

} // namespace sead
