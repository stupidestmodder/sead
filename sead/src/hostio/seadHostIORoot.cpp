#include <hostio/seadHostIORoot.h>

#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include <hostio/seadHostIOContext.h>
#include <hostio/seadHostIOEvent.h>
#include <prim/seadEndian.h>

// TODO: How to handle this ???
#define SEAD_VERSION_MAJOR  1
#define SEAD_VERSION_MINOR  0
#define SEAD_VERSION_MICRO  0
#define SEAD_VERSION_BUGFIX 0

namespace sead {

void HostIORoot::listenPropertyEvent(const hostio::PropertyEvent* ev)
{
    switch (ev->id)
    {
        case 'asrt':
        {
            //SEAD_ASSERT_MSG(false, "SEAD_ASSERT_MSGで止めるテスト");
            SEAD_ASSERT_MSG(false, "Test that stops at SEAD_ASSERT_MSG");
            break;
        }

        case 'dtex':
        {
            // ...
            break;
        }

        case 'nnab':
        {
            // ...
            break;
        }
    }
}

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

#if defined(SEAD_TARGET_DEBUG)
    r.append("<tr><td>build target</td><td>Debug</td></tr>");
#elif
    r.append("<tr><td>build target</td><td>Release</td></tr>");
#endif // SEAD_TARGET_DEBUG

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

    ctx->startNode("Debug", "", 0, nullptr);
    {
        //ctx->genButton("【取り扱い注意】SEAD_ASSERT_MSGで止める", 'asrt', "", nullptr);
        ctx->genButton("【Caution】Stop with SEAD_ASSERT_MSG", 'asrt', "", nullptr);
        //ctx->genButton("【取り扱い注意】データアクセス例外で止める", 'dtex', "", nullptr);
        ctx->genButton("【Caution】Stop with data access exception", 'dtex', "", nullptr);
        //ctx->genButton("【取り扱い注意】NN_ABORTで止める", 'nnab', "", nullptr);
        ctx->genButton("【Caution】Stop with NN_ABORT", 'nnab', "", nullptr);
    }
    ctx->endNode();
}

} // namespace sead
