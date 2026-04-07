#pragma once

#include <hostio/seadHostIODefine.h>
#include <hostio/seadHostIOReflexible.h>
#include <stream/seadRamStream.h>

namespace sead { namespace hostio {

struct ExtensionFuncEvent;

// TODO
class Context
{
public:
    class ContextBufferAccessor
    {
    public:
        ContextBufferAccessor()
            : mBuffer(nullptr)
            , mMaxSize(0)
        {
        }

        void* getBuffer() const
        {
            return mBuffer;
        }

        u32 getMaxSize() const
        {
            return mMaxSize;
        }

        void reset();
        void set(void* buf, u32 maxSize);

    private:
        void* mBuffer;
        u32 mMaxSize;
    };

public:
    virtual void startNode(const SafeString& name, const SafeString& meta, u32 localNodeID, NodeEventListener* listener);
    virtual void endNode();
    virtual void genNode(const SafeString& name, Reflexible* node, const SafeString& meta);
    virtual void invalidateNode(Reflexible* node, u32 info);
    virtual void removeNode(Reflexible* node);
    virtual void startTab(const SafeString& name, const SafeString& meta);
    virtual void endTab();

    //void genTextBox();
    //void genTextBox();
    //void genUserControl();
    void genButton(const SafeString& label, u32 id, const SafeString& meta, PropertyEventListener* eL);
    //void genImageViewer();
    void genLabel(const SafeString& label, u32 id, const SafeString& meta);

    void genHTMLLabel(const SafeString& label, const SafeString& meta)
    {
        genHTMLLabel(label, 0, meta);
    }

    void genHTMLLabel(const SafeString& html, u32 id, const SafeString& meta);

    virtual void genGuiTemplate(void* data, u32 dataSize, const SafeString& meta, const SafeString& metaFilename, PropertyEventListener* eL);
    virtual void startLayout(const SafeString& meta);
    virtual void endLayout();

    //void genNull();
    //void genSeparator();

    virtual void sendMetaFilename(const SafeString& filename);
    virtual void execExtensionFunc(ExtensionFuncEvent* retVal, const SafeString& funcName, const SafeString& meta, const SafeString& metaFilename);
    //virtual void fileDialog();
    //virtual void folderDialog();
    //virtual void forceSetFolderCache();
    //virtual void messageBox();
    //virtual void inputBox();
    //virtual void updateMemory();
    //virtual void updateLocalNodeMemory();
    //virtual void updateLabel();
    //virtual void updateMeta();
    //virtual void updateMeta();
    //virtual void updateMeta();
    //virtual void updateMeta();

    void addPalette(const SafeString& name, const SafeString& func, const SafeString& meta, const SafeString& metaFilename);
    void removePalette(const SafeString& name);
    void updatePaletteMeta(const SafeString& name, const SafeString& meta, const SafeString& metaFilename);

    void addSecurityUnlockFile(const SafeString& path);

    //virtual void selectNode();
    //virtual void selectLocalNode();
    //virtual void expandNode();
    //virtual void expandLocalNode();
    //virtual void collapseNode();
    //virtual void collapseLocalNode();
    //virtual void selectNode();
    //virtual void expandNode();
    //virtual void collapseNode();
    virtual void genRootNode(const SafeString& name, Reflexible* node, const SafeString& meta, bool isAutoExpand);
    virtual void genSetLiteralEncoding(const SafeString& encoding);
    virtual void startGenMessage(const SafeString& name, Reflexible* node, const SafeString& meta, u32 createIdOR);
    virtual void endGenMessage();
    virtual void notFoundObj(u32 nodeCreateID);

private:
    virtual void genNodeSub_(const SafeString& name, Reflexible* obj, const SafeString& meta, NodeEventListener* eL, u32 localNodeID = 0);

    //void genSliderSub_();
    //void genAngleSub_();
    //void genCheckBoxSub_();
    //void genCheckBoxIDSub_();
    //void genVectorSub_();
    //void genColorSub_();
    //void startRadioBtnSub_();
    //void startRadioBtnIDSub_();
    //void startComboBoxSub_();
    //void startComboBoxIDSub_();
    //void startCheckListSub_();
    //void genCurveSub_();

    //virtual void endPropertySub_();
    virtual bool gen_or_start_UserControl_(
        bool isGen, const SafeString& name, const SafeString& ctrlName, const SafeString& typeName,
        void* data, u32 dataSize, const SafeString& meta, PropertyEventListener* eL, u32 bitMask, u32 addFlg, u32 id
    );
    //virtual void genParamOption_();
    //virtual void updateItemMetaSub_();

    bool genLabelSub_(const SafeString& ctrl, const SafeString& label, u32 id, const SafeString& meta);

    virtual ContextBufferAccessor* beginLabelSub_(const SafeString& ctrl, u32 id, const SafeString& meta);
    virtual void endLabelSub_(u32 usedSize);

    void correctNodeInfo_(Reflexible* node);

    virtual void putLifeCheckable_(LifeCheckable* lifecheckable)
    {
        if (lifecheckable)
        {
            mStream << lifecheckable->getCreateID();
        }
        else
        {
            mStream << static_cast<u32>(0);
        }
    }

    void putNode_(Reflexible* node)
    {
        putLifeCheckable_(node);
    }

    void putPropertyEventListener_(PropertyEventListener* listener)
    {
        putLifeCheckable_(listener);
    }

    void putNodeEventListener_(NodeEventListener* listener)
    {
        putLifeCheckable_(listener);
    }

public:
    const void* msgPtr()
    {
        return mStreamBuffer;
    }

    u32 msgSize()
    {
        return mStream.getCurrentPosition();
    }

    Context(void* sendBuffer, s32 sendBufferSize);

    void setORBehavior(u32 ORBehavior)
    {
        mORBehavior = ORBehavior;
    }

    virtual void bufInit(Command currentCommand);

private:
    virtual void writeString_(const SafeString& str);
    virtual bool checkCallTiming_(Command validCommand);

public:
    //void startComboBox(const SafeString&, s32*, const SafeString&, PropertyEventListener*);
    //void genComboBoxItem(const SafeString&, s32, const SafeString&);

    ContextBufferAccessor* beginHTMLLabel(const SafeString& meta)
    {
        return beginHTMLLabel(0, meta);
    }

    ContextBufferAccessor* beginHTMLLabel(u32 id, const SafeString& meta)
    {
        return beginLabelSub_("html_label", id, meta);
    }

    void endHTMLLabel(u32 useSize)
    {
        endLabelSub_(useSize);
    }

    void setChildNodeCorrect(bool isChildCorrect)
    {
        mIsChildNodeCorrect = isChildCorrect;
    }

//private:
    //void startComboBoxSub_(const SafeString&, u32, const SafeString&, void*, const SafeString&, PropertyEventListener*);

private:
    RamWriteStream mStream;
    s32 mStreamSize;
    void* mStreamBuffer;
    bool mIsNodeOnly;
    bool mIsOpenTab;
    bool mIsChildNodeCorrect;
    s32 mNullNodeCnt;
    Command mCurrentCommand;
    ContextBufferAccessor mBufferAccessor;
    u32 mORBehavior;
    s32 mGenMessageDepth;
};

} } // namespace sead::hostio
