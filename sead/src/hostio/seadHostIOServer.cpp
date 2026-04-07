#include <hostio/seadHostIOServer.h>

#include <basis/seadWarning.h>
#include <hostio/seadHostIOConfig.h>
#include <hostio/seadHostIOEvent.h>
#include <hostio/seadHostIOFramework.h>
#include <hostio/seadHostIOReflexible.h>
#include <prim/seadFormatPrint.h>

#if defined(SEAD_PLATFORM_WINDOWS)
#include <hostio/win/seadHostIOPortDirectConnectWin.h>
#endif // SEAD_PLATFORM_WINDOWS

#include <basis/seadRawPrint.h>

static const u32 MAX_SEND_BUFFER_SIZE = 0x800000;

namespace sead { namespace hostio {

Server* Server::sInstance = nullptr;

void Server::createInstance(Heap* heap, const Config* config)
{
    SEAD_ASSERT(sInstance == nullptr);
    SEAD_ASSERT(config->getSendBufferSize() <= MAX_SEND_BUFFER_SIZE);

    u8* workBuf = new(heap) u8[config->getSendBufferSize()];

    sInstance = new(heap) Server(heap, config, workBuf, config->getSendBufferSize());
    sInstance->mHeap = heap;
}

void Server::deleteInstance()
{
    SEAD_ASSERT(sInstance != nullptr);

    delete sInstance;
    sInstance = nullptr;
}

Server* Server::getInstance()
{
    SEAD_ASSERT(sInstance != nullptr);

    return sInstance;
}

bool Server::isInitialized()
{
    return sInstance != nullptr;
}

void Server::createPortDirectConnect(const Config* config)
{
    if (mPort)
    {
        return;
    }

#if defined(SEAD_PLATFORM_WINDOWS)
    mPort = new(mHeap) WinPortDirectConnect(mHeap, config->getRecvBufferSize(), config->getWinTCPPortNumber(), "HostIO RecvThread");
#else
#error "Unsupported platform"
#endif // SEAD_PLATFORM_WINDOWS
}

void Server::polling()
{
    if (!mPort)
    {
        return;
    }

    if (mIsRootNodeUpdate)
    {
        sendRootNode_();
    }

    mPort->polling();
}

bool Server::dispatchMessage()
{
    bool isReceiveData = false;

    if (mPort)
    {
        SEAD_ASSERT(mRecvBufCur < cRECV_BUFF_COUNT);

        s32 recvBufIx = mRecvBufCur;
        mRecvBufCur++;

        if (!mAnalysisPacket)
        {
            Command receiveCommand = Command::eNumCommand;

            mPort->lock();

            s32 available = mPort->getAvailableSize();
            if (available >= sizeof(PacketHeader))
            {
                mPort->recv(&mHeader, sizeof(PacketHeader));
                SEAD_ASSERT_MSG(mHeader.dataSize < mRecvBufSize, "[HostIO] receive data buffer overflow!! Refer hostio::Config::setRecvMaxDataSize() .");
                if (mHeader.dataSize == 0)
                {
                    receiveCommand = Command(mHeader.command);
                }
                else
                {
                    mAnalysisPacket = true;
                }
            }

            mPort->unlock();

            if (receiveCommand != Command::eNumCommand)
            {
                receiveData(receiveCommand, nullptr, 0);
                isReceiveData = true;
            }

            mReadingDataSize[recvBufIx] = 0;
        }

        if (mAnalysisPacket)
        {
            Command receiveCommand = Command::eNumCommand;

            mPort->lock();

            s32 recvSize;
            s32 available = mPort->getAvailableSize();
            if (available > 0)
            {
                s32 restData = mHeader.dataSize - mReadingDataSize[recvBufIx];

                recvSize = restData <= available ? restData : available;
                SEAD_ASSERT(recvSize >= 0);

                recvSize = mPort->recv(mRecvBuf[recvBufIx] + mReadingDataSize[recvBufIx], recvSize);

                mReadingDataSize[recvBufIx] += recvSize;
                if (mReadingDataSize[recvBufIx] == mHeader.dataSize)
                {
                    receiveCommand = Command(mHeader.command);
                    mAnalysisPacket = false;
                }
            }

            mPort->unlock();

            if (receiveCommand != Command::eNumCommand)
            {
                receiveData(receiveCommand, mRecvBuf[recvBufIx], mReadingDataSize[recvBufIx]);
                isReceiveData = true;
            }
        }

        mRecvBufCur--;
        SEAD_ASSERT(mRecvBufCur >= 0);
    }

    return isReceiveData;
}

void Server::setRootNode(const SafeString& name, Reflexible* obj, const SafeString& meta)
{
    for (s32 i = 0; i < cMAX_ROOT_NODE; i++)
    {
        if (!getRootNode(i))
        {
            mRootNodeCreateID[i] = obj->getCreateID();
            obj->setNodeName(name);
            obj->setNodeMeta(meta);
            mIsRootNodeUpdate = true;
            return;
        }
    }

    SEAD_ASSERT_MSG(false, "too many root node!!");
}

Reflexible* Server::getRootNode(s32 ix)
{
    SEAD_ASSERT(ix >= 0 && ix < cMAX_ROOT_NODE);
    return static_cast<Reflexible*>(Reflexible::searchInstanceFromCreateID(mRootNodeCreateID[ix]));
}

Reflexible* Server::searchNode(const SafeString& url, BufferedSafeString* restUrl)
{
    SafeString tmpUrl;
    if (url.include("ORNode://"))
    {
        tmpUrl = url.getPart(9);
    }
    else
    {
        tmpUrl = url.getPart(0);
    }

    for (s32 i = 0; i < cMAX_ROOT_NODE; i++)
    {
        if (!getRootNode(i))
        {
            continue;
        }

        Reflexible* node = getRootNode(i)->searchNode(tmpUrl);
        if (!node)
        {
            continue;
        }

        if (restUrl)
        {
            FixedSafeString<256> nodeURL("ORNode://");
            node->calcURL(&nodeURL);

            restUrl->copy(url.getPart(nodeURL.calcLength()));

            s32 tabIx = restUrl->findIndex(":");
            if (tabIx > -1)
            {
                restUrl->getBuffer()[tabIx] = '/';
            }

            if (restUrl->isEqual("/パラメータ")) // Parameters
            {
                restUrl->copy("");
            }
        }

        return node;
    }

    return nullptr;
}

Context* Server::attachContext(Command command)
{
    SEAD_ASSERT_MSG(mAttachedCommand == Command::eNumCommand, "multi context attach forbidden");
    mAttachedCommand = command;

    if (mExternalContext)
    {
        mExternalContext->bufInit(command);
        return mExternalContext;
    }
    else
    {
        mContext.bufInit(command);
        return &mContext;
    }
}

void Server::releaseContext(Context* context)
{
    if (context->msgSize() > sizeof(PacketHeader))
    {
        sendData(mAttachedCommand, context->msgPtr(), context->msgSize());
    }

    mAttachedCommand = Command::eNumCommand;
}

void Server::entryPalette(const SafeString& name, const SafeString& func, PaletteEventListener* listener, const SafeString& meta, const SafeString& metaFile)
{
    for (s32 i = 0; i < cMAX_PALETTE_ENTRY; i++)
    {
        PaletteEntry& entry = mPaletteEntries[i];
        if (entry.name.isEqual(name))
        {
            (BufferingPrintFormatter() << "[hostio::Server] Warning!! palette {%s} already entry!!", name.cstr()).flushWithLineBreak();
            return;
        }
    }

    for (s32 i = 0; i < cMAX_PALETTE_ENTRY; i++)
    {
        PaletteEntry& entry = mPaletteEntries[i];
        if (entry.name.cstr() == SafeString::cEmptyString)
        {
            entry.name = name;
            entry.listenerID = listener != nullptr ? listener->getCreateID() : 0;
            entry.func = func;
            entry.meta = meta;
            entry.metaFile = metaFile;

            TmpContext mc(Command::eAddPalette);
            mc->addPalette(name, func, meta, metaFile);

            return;
        }
    }

    SEAD_ASSERT_MSG(false, "[hostio::Server] ApplicationPalette entry buffer overflow!!");
}

void Server::removePalette(const SafeString& name)
{
    for (s32 i = 0; i < cMAX_PALETTE_ENTRY; i++)
    {
        PaletteEntry& entry = mPaletteEntries[i];
        if (entry.name == name)
        {
            TmpContext mc(Command::eRemovePalette);
            mc->removePalette(entry.name);

            entry.name = SafeString::cEmptyString;
        }
    }
}

void Server::updatePaletteMeta(const SafeString& name, const SafeString& meta, const SafeString& metaFile)
{
    for (s32 i = 0; i < cMAX_PALETTE_ENTRY; i++)
    {
        PaletteEntry& entry = mPaletteEntries[i];
        if (entry.name == name)
        {
            TmpContext mc(Command::eUpdatePaletteMeta);
            mc->updatePaletteMeta(name, meta, metaFile);

            entry.meta = meta;
            entry.metaFile = metaFile;
            //! Forgot to update name ?

            return;
        }
    }

    SEAD_ASSERT_MSG(false, "[hostio::Server] Not found palette %s!!", name.cstr());
}

void Server::addSecurityUnlockFile(const SafeString& path)
{
    for (s32 i = 0; i < cMAX_SECURITY_UNLOCK_FILE; i++)
    {
        if (mSecurityUnlockFiles[i].isEqual(""))
        {
            mSecurityUnlockFiles[i].copy(path);

            TmpContext mc(Command::eAddSecurityUnlockFile);
            mc->addSecurityUnlockFile(path);

            return;
        }
    }

    SEAD_ASSERT_MSG(false, "SecurityUnlockFile buffer overflow");
}

Context* Server::setExternalContext(Context* context)
{
    SEAD_ASSERT_MSG(mAttachedCommand == Command::eNumCommand, "cannot set external context in context attached");

    Context* old = mExternalContext;
    mExternalContext = context;
    return old;
}

Server::Server(Heap* heap, const Config* config, void* sendBuffer, s32 sendBufferSize)
    : mPort(nullptr)
    , mAnalysisPacket(false)
    , mHeader()
    , mAttachedCommand(Command::eNumCommand)
    , mHeap(nullptr)
    , mContext(sendBuffer, sendBufferSize)
    , mExternalContext(nullptr)
    , mIsRootNodeUpdate(false)
  //, mRecvBufSize()
    , mRecvBufCur(0)
    , mPaletteEntries()
    , mSecurityUnlockFiles()
{
    for (s32 i = 0; i < cMAX_ROOT_NODE; i++)
    {
        mRootNodeCreateID[i] = 0;
    }

    mRecvBufSize = config->getRecvMaxDataSize();

    for (s32 i = 0; i < cRECV_BUFF_COUNT; i++)
    {
        mRecvBuf[i] = new(heap) s8[mRecvBufSize];
    }

    for (s32 i = 0; i < cMAX_SECURITY_UNLOCK_FILE; i++)
    {
        mSecurityUnlockFiles[i].copy("");
    }

    mContext.setORBehavior(config->getORBehavior());
}

Server::~Server()
{
    SEAD_ASSERT(mPort != nullptr);
    delete mPort;
    mPort = nullptr;
}

void Server::receiveData(Command command, s8* buf, s32 len)
{
    RamReadStream mis(buf, len, Stream::Modes::eBinary);

    SEAD_PRINT("[HostIO] received command: %u\n", command);

    // TODO
    switch (command)
    {
        case Command::ePing:
        {
            break;
        }

        case Command::eGetRootNode:
        {
            sendRootNode_();
            sendSecurityUnlockFilePath_();
            sendExtensionFuncPalatteEntry_();
            break;
        }

        case Command::eGetObjInfo:
        {
            u32 createID = mis.readU32();
            u32 createIDOR = mis.readU32();
            getObjInfo_(createID, createIDOR);
            break;
        }

        case Command::ePropertyEvent:
        {
            u32 createID = mis.readU32();
            Reflexible* node = static_cast<Reflexible*>(Reflexible::searchInstanceFromCreateID(createID));
            if (node)
            {
                PropertyEvent ev;
                ev.localNodeID = mis.readU32();
                ev.flag = mis.readU32();
                ev.id = mis.readU32();
                ev.eventListenerID = mis.readU32();
                ev.extFlg = mis.readU32();
                ev.dataSize = mis.readU32();
                ev.data = mis.getCurrentAddres();

                if (mRecvBufCur < cRECV_BUFF_COUNT)
                {
                    node->baseListen(&ev);
                }
                else
                {
                    SEAD_WARNING("hostio::Server[Command::ePropertyEvent]  skip listenPropertyEvent ( CreateID = %d ).", createID);
                }
            }
            else
            {
                SEAD_WARNING("hostio::Server[Command::ePropertyEvent]  can't find node ( CreateID = %d )", createID);

                TmpContext mc(Command::ePutObjInfo);
                mc->notFoundObj(createID);
            }

            break;
        }

        default:
        {
            SEAD_ASSERT(false);
            break;
        }
    }
}

s32 Server::sendData(Command command, const void* buf, s32 len)
{
    PacketHeader header;

    PacketHeader* pkt = static_cast<PacketHeader*>(const_cast<void*>(buf));
    if (!pkt)
    {
        pkt = &header;
        len += sizeof(PacketHeader);
    }

    pkt->magic = Endian::fromHostU32(Endian::eLittle, 0xBEAF2929);
    pkt->dataSize = Endian::fromHostS32(Endian::eLittle, len);
    pkt->command = Endian::fromHostU32(Endian::eLittle, static_cast<u32>(command));
    pkt->extra = 0;

    s32 ret = mPort->send(pkt, len);
    return ret;
}

void Server::sendRootNode_()
{
    {
        TmpContext mc(Command::eSetLiteralEncoding);
        mc->genSetLiteralEncoding("utf8");
    }

    for (s32 i = 0; i < cMAX_ROOT_NODE; i++)
    {
        Reflexible* rootNode = getRootNode(i);
        if (rootNode)
        {
            bool isAutoExpand = i != 0;

            TmpContext mc(Command::ePutRootNode); // TODO: Switch uses 27 (Command::eUnk)
            mc->genRootNode(rootNode->getNodeName(), rootNode, rootNode->getNodeMeta(), isAutoExpand);
        }
    }

    mIsRootNodeUpdate = false;
}

void Server::getObjInfo_(u32 createID, u32 createIDOR)
{
    Reflexible* node = static_cast<Reflexible*>(Reflexible::searchInstanceFromCreateID(createID));
    if (node)
    {
        GenEvent ev;
        node->genObjectInfo(&ev, createIDOR);
    }
    else
    {
        SEAD_WARNING("hostio::Server[Command::eGetObjInfo]  can't find node ( CreateID = %d )", createID);

        TmpContext mc(Command::ePutObjInfo);
        mc->notFoundObj(createID);
    }
}

void Server::sendExtensionFuncPalatteEntry_()
{
    for (s32 i = 0; i < cMAX_PALETTE_ENTRY; i++)
    {
        PaletteEntry& entry = mPaletteEntries[i];
        if (entry.name.cstr() != SafeString::cEmptyString)
        {
            TmpContext mc(Command::eAddPalette);
            mc->addPalette(entry.name, entry.func, entry.meta, entry.metaFile);
        }
    }
}

void Server::callPaletteEventListenr_(RamReadStream* reader, PaletteEventType type, PaletteEntry* entry, PaletteEventListener* listener)
{
    // PaletteEvent ev;

    // {
    //     const s32 MAX_FILE;

    //     data[32];
    //     numFile;

    //     for (i)
    //     {
    //         strSize;
    //         path;
    //     }
    // }

    // {
    //     data;
    //     strSize;
    //     name;
    // }
}

void Server::sendSecurityUnlockFilePath_()
{
    for (s32 i = 0; i < cMAX_SECURITY_UNLOCK_FILE; i++)
    {
        if (!mSecurityUnlockFiles[i].isEqual(""))
        {
            TmpContext mc(Command::eAddSecurityUnlockFile);
            mc->addSecurityUnlockFile(mSecurityUnlockFiles[i]);
        }
    }
}

} } // namespace sead::hostio
