#include <hostio/seadHostIORoot.h>

#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include <hostio/seadHostIOContext.h>
#include <prim/seadEndian.h>

// TODO: How to handle this ???
#define SEAD_VERSION_MAJOR  1
#define SEAD_VERSION_MINOR  0
#define SEAD_VERSION_MICRO  0
#define SEAD_VERSION_BUGFIX 0

namespace sead {

void HostIORoot::genMessage(hostio::Context* ctx)
{
    const s32 cResultSize = 1024;
    const s32 cTmpSize = 128;

    char result[cResultSize];
    char tmp[cTmpSize];

    BufferedSafeString t(tmp, cTmpSize);
    BufferedSafeString r(result, cResultSize);

    r.format("");
    t.format("<h1>sead</h1>");

    r.append(t);
    r.append("<table>");

    t.format("<tr><td>sead Version</td><td>%d.%d.%d.%d</td></tr>", SEAD_VERSION_MAJOR, SEAD_VERSION_MINOR, SEAD_VERSION_MICRO, SEAD_VERSION_BUGFIX);
    r.append(t);

    if (Endian::getHostEndian() == Endian::eBig)
    {
        r.append("<tr><td>endian</td><td>Big</td></tr>");
    }
    else
    {
        r.append("<tr><td>endian</td><td>Little</td></tr>");
    }

#ifdef SEAD_DEBUG
    r.append("<tr><td>build target</td><td>Debug</td></tr>");
#elif
    r.append("<tr><td>build target</td><td>Release</td></tr>");
#endif // SEAD_DEBUG

    t.format("<tr><td>data model</td><td>INT(%d), LONG(%d), PTR(%d)</td></tr>", sizeof(int) * 8, sizeof(long) * 8, sizeof(void*) * 8);
    r.append(t);

    if (HeapMgr::instance()->getRootHeap(0))
    {
        t.format("<tr><td>memory size</td><td>%d [byte]</td></tr>", HeapMgr::instance()->getRootHeap(0)->getSize());
        r.append(t);
    }

    r.append("</table>");

    ctx->startLayout("Layout = Stack , Dir = X");
    ctx->genHTMLLabel(r, "");
    ctx->endLayout();

    // TODO
}

} // namespace sead
