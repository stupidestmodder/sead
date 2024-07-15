#pragma once

#include <hostio/seadHostIODefine.h>

namespace sead { namespace hostio {

class PropertyEventListener;
class Reflexible;

// TODO
class Context
{
private:
    class ContextBufferAccessor
    {
    public:
        ContextBufferAccessor();

    private:
        void* mBuffer;
        u32 mMaxSize;
    };

public:
    Context();

    virtual void startNode();
    virtual void endNode();
    virtual void genNode(const SafeString&, Reflexible*, const SafeString&);
    virtual void invalidateNode();
    virtual void removeNode();
    virtual void startTab();
    virtual void endTab();
    virtual void genGuiTemplate();
    virtual void startLayout(const SafeString&);
    virtual void endLayout();
    virtual void sendMetaFilename(const SafeString&);
    virtual void execExtensionFunc();
    virtual void fileDialog();
    virtual void folderDialog();
    virtual void forceSetFolderCache();
    virtual void messageBox();
    virtual void inputBox();
    virtual void updateMemory();
    virtual void updateLocalNodeMemory();
    virtual void updateLabel();
    virtual void updateMeta();
    //virtual void updateMeta();
    //virtual void updateMeta();
    //virtual void updateMeta();
    virtual void selectNode();
    virtual void selectLocalNode();
    virtual void expandNode();
    virtual void expandLocalNode();
    virtual void collapseNode();
    virtual void collapseLocalNode();
    //virtual void selectNode();
    //virtual void expandNode();
    //virtual void collapseNode();
    virtual void genRootNode();
    virtual void genSetLiteralEncoding();
    virtual void startGenMessage(const SafeString&, Reflexible*, const SafeString&, u32);
    virtual void endGenMessage();
    virtual void notFoundObj();

private:
    virtual void genNodeSub_();
    virtual void endPropertySub_();
    virtual void gen_or_start_UserControl_();
    virtual void genParamOption_();
    virtual void updateItemMetaSub_();
    virtual void beginLabelSub_();
    virtual void endLabelSub_();
    virtual void putLifeCheckable_();
    virtual void bufInit();
    virtual void writeString_();
    virtual void checkCallTiming_();

public:
    void startComboBox(const SafeString&, s32*, const SafeString&, PropertyEventListener*);
    void genComboBoxItem(const SafeString&, s32, const SafeString&);
    ContextBufferAccessor* beginHTMLLabel(u32, const SafeString&);

    void genHTMLLabel(const SafeString& label, const SafeString& meta)
    {
        genHTMLLabel(label, 0, meta);
    }

    void genHTMLLabel(const SafeString& html, u32 id, const SafeString& meta);

    void setChildNodeCorrect(bool isChildCorrect)
    {
        mIsChildNodeCorrect = isChildCorrect;
    }

private:
    void startComboBoxSub_(const SafeString&, u32, const SafeString&, void*, const SafeString&, PropertyEventListener*);

private:
    //RamWriteStream mStream;
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
