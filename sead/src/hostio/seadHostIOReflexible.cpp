#include <hostio/seadHostIOReflexible.h>

#include <heap/seadHeap.h>
#include <heap/seadHeapMgr.h>
#include <hostio/seadHostIOContext.h>
#include <hostio/seadHostIOEvent.h>
#include <hostio/seadHostIOFramework.h>
#include <prim/seadFormatPrint.h>
#include <prim/seadPtrUtil.h>

namespace sead { namespace hostio {

Reflexible::ApplyEventDataToMemoryCallback Reflexible::sApplyEventDataToMemoryCallback = nullptr;

Reflexible::Reflexible()
    : NodeEventListener()
    , mIsGenerated(false)
    , mAllocFlg(0)
{
    setNodeName("");
    setNodeMeta("");
}

Reflexible::Reflexible(Heap* disposerHeap, IDisposer::HeapNullOption option)
    : NodeEventListener(disposerHeap, option)
    , mIsGenerated(false)
    , mAllocFlg(0)
{
    setNodeName("");
    setNodeMeta("");
}

void Reflexible::genObjectInfo(const GenEvent* event, u32 createIdOR)
{
    SEAD_UNUSED(event);

    if (getMetaFilename() != SafeString::cEmptyString)
    {
        TmpContext mc(Command::ePutObjInfo);
        mc->sendMetaFilename(getMetaFilename());
    }

    mIsGenerated = true;

    {
        TmpContext mc(Command::ePutObjInfo);
        callGenMessage(mc, createIdOR);
    }
}

void Reflexible::callGenMessage(Context* mc, u32 createIdOR)
{
    mc->startGenMessage(getNodeName(), this, getNodeMeta(), createIdOR);
    genChildNode(mc);
    genMessage(mc);
    mc->endGenMessage();
}

void Reflexible::correctChildNodeInfo(Context* mc)
{
    mc->startGenMessage(getNodeName(), this, getNodeMeta(), 0);

    if (isHaveChild())
    {
        genChildNode(mc);
    }
    else
    {
        mc->setChildNodeCorrect(true);
        genMessage(mc);
        mc->setChildNodeCorrect(false);
    }

    mc->endGenMessage();
}

void Reflexible::baseListen(const PropertyEvent* ev)
{
    if ((ev->flag & 1) == 0)
    {
        if ((ev->flag & 2) == 0)
        {
            Reflexible::applyEventDataToMemory(ev);
        }

        listenPropertyEvent(ev);
    }
    else
    {
        PropertyEvent* _ev = const_cast<PropertyEvent*>(ev);
        _ev->flag &= ~1;

        PropertyEventListener* listener = static_cast<PropertyEventListener*>(LifeCheckable::searchInstanceFromCreateID(ev->eventListenerID));
        if (listener)
        {
            listener->listenPropertyEvent(_ev);
        }
        else
        {
            (BufferingPrintFormatter() << "[hostio::Reflexible]  can't find property event listener ( CreateID = %d )", ev->eventListenerID).flushWithLineBreak(); // Cringe
        }

        _ev->flag |= 1;
    }
}

void Reflexible::setNodeName(const SafeString& name)
{
    if (mName != name.cstr())
    {
        safeDelete_(AllocFlg::eName);
    }

    SEAD_ASSERT_MSG(!MemUtil::isStack(name.cstr()), "%p is in stack", name.cstr());
    mName = name.cstr();
}

void Reflexible::setNodeNameCopyString(const SafeString& name, Heap* heap)
{
    mName = createStringBuffer_(AllocFlg::eName, name, heap);
}

void Reflexible::setNodeMeta(const SafeString& meta)
{
    if (mMeta != meta.cstr())
    {
        safeDelete_(AllocFlg::eMeta);
    }

    SEAD_ASSERT_MSG(!MemUtil::isStack(meta.cstr()), "%p is in stack", meta.cstr());
    mMeta = meta.cstr();
}

void Reflexible::setNodeMetaCopyString(const SafeString& meta, Heap* heap)
{
    mMeta = createStringBuffer_(AllocFlg::eMeta, meta, heap);
}

// TODO: Check how accurate this is
void Reflexible::applyEventDataToMemory(const PropertyEvent* ev)
{
    void* idAsPtr = reinterpret_cast<void*>(ev->id);

    if (sApplyEventDataToMemoryCallback)
    {
        bool ret = sApplyEventDataToMemoryCallback(ev);
        if (!ret)
        {
            return;
        }
    }

    if ((ev->flag & 4))
    {
        switch (ev->dataSize)
        {
            case sizeof(u8):
            {
                u8* ptr = static_cast<u8*>(idAsPtr);
                const u8* dat = ev->data;
                u8 mask = ev->extFlg;

                *ptr &= ~mask;
                *ptr |= *dat & mask;

                break;
            }

            case sizeof(u16):
            {
                u16* ptr = static_cast<u16*>(idAsPtr);
                const u16* dat = reinterpret_cast<const u16*>(ev->data);
                u16 mask = ev->extFlg;

                *ptr &= ~mask;
                *ptr |= *dat & mask;

                break;
            }

            case sizeof(u32):
            {
                u32* ptr = static_cast<u32*>(idAsPtr);
                const u32* dat = reinterpret_cast<const u32*>(ev->data);
                u32 mask = ev->extFlg;

                *ptr &= ~mask;
                *ptr |= *dat & mask;

                break;
            }

            case sizeof(u64):
            {
                u64* ptr = static_cast<u64*>(idAsPtr);
                const u64* dat = reinterpret_cast<const u64*>(ev->data);
                u64 mask = ev->extFlg;

                *ptr &= ~mask;
                *ptr |= *dat & mask;

                break;
            }
        }
    }
    else
    {
        u8* ptr = static_cast<u8*>(idAsPtr);

        for (s32 i = 0; i < ev->dataSize; i++)
        {
            ptr[i] = ev->data[i];
        }
    }
}

void Reflexible::safeDelete_(AllocFlg flg)
{
    const void* buf = nullptr;

    if (mAllocFlg & flg)
    {
        if (flg == AllocFlg::eName)
        {
            buf = mName;
        }
        else if (flg == AllocFlg::eMeta)
        {
            buf = mMeta;
        }

        mAllocFlg &= ~flg;
    }

    if (buf)
    {

        Heap* heapPtr = *static_cast<Heap**>(PtrUtil::addOffset(buf, -static_cast<std::intptr_t>(cPtrSize))); // Get the start of the allocated buffer which is a Heap pointer (see createStringBuffer_())
        heapPtr->free(&heapPtr);
    }
}

const char* Reflexible::createStringBuffer_(AllocFlg flg, const SafeString& str, Heap* heap)
{
    safeDelete_(flg);

    s32 length = str.calcLength();

    if (!heap)
    {
        heap = HeapMgr::instance()->getCurrentHeap();
        SEAD_ASSERT(heap);
    }

    void* buf = heap->alloc((length + 1) + cPtrSize); // Allocate str length + null terminator + Heap pointer
    *static_cast<Heap**>(buf) = heap; // Save the Heap pointer so we can get it in safeDelete_()

    char* charBuf = static_cast<char*>(PtrUtil::addOffset(buf, cPtrSize));
    MemUtil::copy(charBuf, str.cstr(), length);
    charBuf[length] = '\0';

    mAllocFlg |= flg;

    return charBuf;
}

void Reflexible::disposeHostIOImpl_()
{
    safeDelete_(AllocFlg::eName);
    safeDelete_(AllocFlg::eMeta);
}

} } // namespace sead::hostio
