#include <hostio/seadHostIOContext.h>

#include <hostio/seadHostIOPortBase.h>
#include <prim/seadFormatPrint.h>

namespace sead { namespace hostio {

void Context::ContextBufferAccessor::reset()
{
    mBuffer = nullptr;
    mMaxSize = 0;
}

void Context::ContextBufferAccessor::set(void* buf, u32 maxSize)
{
    mBuffer = buf;
    mMaxSize = maxSize;
}

void Context::startNode(const SafeString& name, const SafeString& meta, u32 localNodeID, NodeEventListener* listener)
{
    if (!checkCallTiming_(Command::ePutObjInfo))
    {
        return;
    }

    mStream << static_cast<u32>(0);
    genNodeSub_(name, nullptr, meta, listener, localNodeID);
    mNullNodeCnt++;
}

void Context::endNode()
{
    if (!checkCallTiming_(Command::ePutObjInfo))
    {
        return;
    }

    mStream << static_cast<u32>(1);
}

void Context::genNode(const SafeString& name, Reflexible* node, const SafeString& meta)
{
    if (!checkCallTiming_(Command::ePutObjInfo))
    {
        return;
    }

    SEAD_ASSERT(node != nullptr);

    mStream << static_cast<u32>(3);
    genNodeSub_(name, node, meta, node);
    correctNodeInfo_(node);
}

void Context::invalidateNode(Reflexible* node, u32 info)
{
    if (!checkCallTiming_(Command::ePutUpdateProperties))
    {
        return;
    }

    SEAD_ASSERT(node != nullptr);

    mStream << static_cast<u32>(14);
    putNode_(node);
    mStream << info;

    node->resetGenerated();
}

void Context::removeNode(Reflexible* node)
{
    if (!checkCallTiming_(Command::ePutUpdateProperties))
    {
        return;
    }

    SEAD_ASSERT(node != nullptr);

    mStream << static_cast<u32>(15);
    putNode_(node);

    node->resetGenerated();
}

void Context::startTab(const SafeString& name, const SafeString& meta)
{
    if (mIsNodeOnly || !checkCallTiming_(Command::ePutObjInfo))
    {
        return;
    }

    SEAD_ASSERT(!mIsOpenTab);
    mIsOpenTab = true;

    mStream << static_cast<u32>(9);
    writeString_(name);
    writeString_(meta);
}

void Context::endTab()
{
    if (mIsNodeOnly || !checkCallTiming_(Command::ePutObjInfo))
    {
        return;
    }

    SEAD_ASSERT(mIsOpenTab);
    mIsOpenTab = false;

    mStream << static_cast<u32>(10);
}

void Context::genButton(const SafeString& label, u32 id, const SafeString& meta, PropertyEventListener* eL)
{
    gen_or_start_UserControl_(true, label, "button", "none", nullptr, 0, meta, eL, 0, 1 << 1, id);
}

void Context::genLabel(const SafeString& label, u32 id, const SafeString& meta)
{
    genLabelSub_("label", label, id, meta);
}

void Context::genHTMLLabel(const SafeString& html, u32 id, const SafeString& meta)
{
    genLabelSub_("html_label", html, id, meta);
}

void Context::genGuiTemplate(void* data, u32 dataSize, const SafeString& meta, const SafeString& metaFilename, PropertyEventListener* eL)
{
    if (mIsNodeOnly || !checkCallTiming_(Command::ePutObjInfo))
    {
        return;
    }

    mStream << static_cast<u32>(23);
    mStream << dataSize;
    mStream << reinterpret_cast<uintptr_t>(data);
    writeString_(meta);
    writeString_(metaFilename);
    putPropertyEventListener_(eL);
    mStream.writeMemBlock(data, dataSize);
}

void Context::startLayout(const SafeString& meta)
{
    if (mIsNodeOnly || !checkCallTiming_(Command::ePutObjInfo))
    {
        return;
    }

    mStream << static_cast<u32>(11);
    writeString_(meta);
}

void Context::endLayout()
{
    if (mIsNodeOnly || !checkCallTiming_(Command::ePutObjInfo))
    {
        return;
    }

    mStream << static_cast<u32>(12);
}

void Context::sendMetaFilename(const SafeString& filename)
{
    if (!checkCallTiming_(Command::ePutObjInfo))
    {
        return;
    }

    mStream << static_cast<u32>(13);
    writeString_(filename);
}

void Context::execExtensionFunc(ExtensionFuncEvent* retVal, const SafeString& funcName, const SafeString& meta, const SafeString& metaFilename)
{
    if (!checkCallTiming_(Command::eExecExtensionFunc))
    {
        return;
    }

    mStream << reinterpret_cast<uintptr_t>(retVal);
    writeString_(funcName);
    writeString_(meta);
    writeString_(metaFilename);
}

void Context::addPalette(const SafeString& name, const SafeString& func, const SafeString& meta, const SafeString& metaFilename)
{
    if (!checkCallTiming_(Command::eAddPalette))
    {
        return;
    }

    writeString_(name);
    writeString_(func);
    writeString_(meta);
    writeString_(metaFilename);
}

void Context::removePalette(const SafeString& name)
{
    if (!checkCallTiming_(Command::eRemovePalette))
    {
        return;
    }

    writeString_(name);
}

void Context::updatePaletteMeta(const SafeString& name, const SafeString& meta, const SafeString& metaFilename)
{
    if (!checkCallTiming_(Command::eUpdatePaletteMeta))
    {
        return;
    }

    writeString_(name);
    writeString_(meta);
    writeString_(metaFilename);
}

void Context::addSecurityUnlockFile(const SafeString& path)
{
    if (!checkCallTiming_(Command::eAddSecurityUnlockFile))
    {
        return;
    }

    writeString_(path);
}

void Context::genRootNode(const SafeString& name, Reflexible* node, const SafeString& meta, bool isAutoExpand)
{
    genNodeSub_(name, node, meta, node);
    mStream << static_cast<u32>(isAutoExpand);
    mStream << mORBehavior;
}

void Context::genSetLiteralEncoding(const SafeString& encoding)
{
    writeString_(encoding);
}

void Context::startGenMessage(const SafeString& name, Reflexible* node, const SafeString& meta, u32 createIdOR)
{
    if (!checkCallTiming_(Command::ePutObjInfo))
    {
        return;
    }

    mGenMessageDepth++;

    mStream << static_cast<u32>(27);
    genNodeSub_(name, node, meta, node);
    mStream << createIdOR;
}

void Context::endGenMessage()
{
    if (!checkCallTiming_(Command::ePutObjInfo))
    {
        return;
    }

    mGenMessageDepth--;

    mStream << static_cast<u32>(28);
}

void Context::notFoundObj(u32 nodeCreateID)
{
    mStream << static_cast<u32>(24);
    mStream << nodeCreateID;
}

void Context::genNodeSub_(const SafeString& name, Reflexible* obj, const SafeString& meta, NodeEventListener* eL, u32 localNodeID)
{
    writeString_(name);
    putNode_(obj);

    mStream << mNullNodeCnt;

    writeString_(meta);
    putNodeEventListener_(eL);

    mStream << localNodeID;

    if (obj)
    {
        writeString_(obj->getMetaFilename());
    }
    else
    {
        writeString_("nofile");
    }
}

bool Context::gen_or_start_UserControl_(
    bool isGen, const SafeString& name, const SafeString& ctrlName, const SafeString& typeName,
    void* data, u32 dataSize, const SafeString& meta, PropertyEventListener* eL, u32 bitMask, u32 addFlg, u32 id
)
{
    if (mIsNodeOnly || !checkCallTiming_(Command::ePutObjInfo))
    {
        return false;
    }

    u32 flg = (eL != nullptr) | addFlg;

    if (isGen)
    {
        mStream << static_cast<u32>(2);
    }
    else
    {
        mStream << static_cast<u32>(4);
    }

    writeString_(ctrlName);
    writeString_(typeName);

    mStream << flg;
    mStream << dataSize;
    
    if (flg >> 1 & 1)
    {
        mStream << id;
    }
    else
    {
        mStream << reinterpret_cast<uintptr_t>(data);
    }

    writeString_(meta);
    putPropertyEventListener_(eL);
    mStream << bitMask;
    writeString_(name);
    mStream.writeMemBlock(data, dataSize);

    return true;
}

bool Context::genLabelSub_(const SafeString& ctrl, const SafeString& label, u32 id, const SafeString& meta)
{
    return gen_or_start_UserControl_(true, label, ctrl, "none", nullptr, 0, meta, nullptr, 0, 1 << 1, id);
}

Context::ContextBufferAccessor* Context::beginLabelSub_(const SafeString& ctrl, u32 id, const SafeString& meta)
{
    SEAD_ASSERT(mBufferAccessor.getBuffer() == nullptr);
    mBufferAccessor.reset();

    if (genLabelSub_(ctrl, "", id, meta))
    {
        mBufferAccessor.set(mStream.getCurrentAddres(), mStreamSize - mStream.getCurrentPosition());
        return &mBufferAccessor;
    }
    else
    {
        return nullptr;
    }
}

void Context::endLabelSub_(u32 usedSize)
{
    SEAD_ASSERT(mBufferAccessor.getBuffer() != nullptr);
    SEAD_ASSERT(usedSize < 0xFFFF);

    u16* sizeAddr = static_cast<u16*>(mBufferAccessor.getBuffer()) - 1;
    *sizeAddr = static_cast<u16>(usedSize);

    mStream.skip(usedSize);
    mBufferAccessor.reset();
}

void Context::correctNodeInfo_(Reflexible* node)
{
    if ((mORBehavior >> 4 & 1) ||
       ((mORBehavior & 1) == 0 && mGenMessageDepth >= 2))
    {
        return;
    }

    SEAD_ASSERT(node != nullptr);

    bool save = mIsNodeOnly;
    s32 saveCnt = mNullNodeCnt;

    mIsNodeOnly = true;
    mNullNodeCnt = 0;
    node->correctChildNodeInfo(this);
    mNullNodeCnt = saveCnt;
    mIsNodeOnly = save;
}

Context::Context(void* sendBuffer, s32 sendBufferSize)
    : mStream(sendBuffer, sendBufferSize, Stream::Modes::eBinary)
{
    mIsOpenTab = false;
    mIsNodeOnly = false;
    mStreamBuffer = sendBuffer;
    mStreamSize = sendBufferSize;
    mORBehavior = 13;
    mIsChildNodeCorrect = false;
    mCurrentCommand = Command::eNumCommand;
    mGenMessageDepth = 0;
    mNullNodeCnt = 0;
}

void Context::bufInit(Command currentCommand)
{
    mStream.rewind();
    mStream.skip(sizeof(PacketHeader), 1);

    mIsOpenTab = false;
    mIsNodeOnly = false;
    mNullNodeCnt = 0;
    mGenMessageDepth = 0;
    mCurrentCommand = currentCommand;
}

void Context::writeString_(const SafeString& str)
{
    s32 len = str.calcLength();
    SEAD_ASSERT(len < 0xFFFF);

    mStream << static_cast<u16>(len);
    mStream.writeString(str, len);
}

bool Context::checkCallTiming_(Command validCommand)
{
    if (mCurrentCommand == validCommand)
    {
        SEAD_ASSERT_MSG(mStreamSize - mStream.getCurrentPosition() > 256, "OR-SendBuffer not enough. Please call hostio::Config::setSendBufferSize().");
        return true;
    }
    else
    {
        (BufferingPrintFormatter() << "[hostio::Context] Warning!! can't call this function now !!").flushWithLineBreak();
        return false;
    }
}

} } // namespace sead::hostio
