#pragma once

#include <basis/seadAssert.h>
#include <basis/seadNew.h>
#include <container/seadListImpl.h>

namespace sead {

class IDisposer
{
    SEAD_NO_COPY(IDisposer);

public:
    enum class HeapNullOption
    {
        eNotAllow = 0,
        eFindContainHeap,
        eNotDispose,
        eUseCurrentHeap
    };

public:
    IDisposer();
    IDisposer(Heap* disposerHeap, HeapNullOption option);
    virtual ~IDisposer();

    static u32 getListNodeOffset() { return offsetof(IDisposer, mListNode); }

protected:
    Heap* getDisposerHeap_() const { return mDisposerHeap; }

private:
    Heap* mDisposerHeap;
    ListNode mListNode;

    friend class Heap;
};

} // namespace sead

#define SEAD_SINGLETON_DISPOSER(CLASS)                                                  \
protected:                                                                              \
    class SingletonDisposer_ : public sead::IDisposer                                   \
    {                                                                                   \
    public:                                                                             \
        SingletonDisposer_(sead::Heap* heap)                                            \
            : sead::IDisposer(heap, sead::IDisposer::HeapNullOption::eUseCurrentHeap)   \
        {                                                                               \
        }                                                                               \
                                                                                        \
        ~SingletonDisposer_() override;                                                 \
                                                                                        \
        static SingletonDisposer_* sStaticDisposer;                                     \
    };                                                                                  \
                                                                                        \
public:                                                                                 \
    static CLASS* instance() { return sInstance; }                                      \
    static CLASS* createInstance(sead::Heap* heap);                                     \
    static void deleteInstance();                                                       \
                                                                                        \
    SEAD_NO_COPY(CLASS);                                                                \
                                                                                        \
protected:                                                                              \
    u32 mSingletonDisposerBuf_[sizeof(SingletonDisposer_) / sizeof(u32)];               \
                                                                                        \
    static CLASS* sInstance;                                                            \
                                                                                        \
    friend class SingletonDisposer_

#define SEAD_CREATE_SINGLETON_INSTANCE(CLASS)                                                               \
    CLASS* CLASS::createInstance(sead::Heap* heap)                                                          \
    {                                                                                                       \
        if (!sInstance)                                                                                     \
        {                                                                                                   \
            u8* buffer = new(heap, alignof(CLASS)) u8[sizeof(CLASS)];                                       \
                                                                                                            \
            SEAD_ASSERT_MSG(!SingletonDisposer_::sStaticDisposer, "Create Singleton Twice (%s).", #CLASS);  \
            u8* disposerBuffer = buffer + offsetof(CLASS, mSingletonDisposerBuf_);                          \
                                                                                                            \
            SingletonDisposer_::sStaticDisposer = new(disposerBuffer) SingletonDisposer_(heap);             \
            sInstance = new(buffer) CLASS;                                                                  \
        }                                                                                                   \
        else                                                                                                \
        {                                                                                                   \
            SEAD_ASSERT_MSG(false, "Create Singleton Twice (%s) : addr 0x%p", #CLASS, sInstance);           \
        }                                                                                                   \
                                                                                                            \
        return sInstance;                                                                                   \
    }

#define SEAD_DELETE_SINGLETON_INSTANCE(CLASS)                                           \
    void CLASS::deleteInstance()                                                        \
    {                                                                                   \
        if (SingletonDisposer_::sStaticDisposer)                                        \
        {                                                                               \
            SingletonDisposer_* staticDisposer = SingletonDisposer_::sStaticDisposer;   \
                                                                                        \
            SingletonDisposer_::sStaticDisposer = nullptr;                              \
            staticDisposer->~SingletonDisposer_();                                      \
                                                                                        \
            if (sInstance)                                                              \
                delete sInstance;                                                       \
                                                                                        \
            sInstance = nullptr;                                                        \
        }                                                                               \
    }

#define SEAD_SINGLETON_DISPOSER_IMPL(CLASS)                                             \
    CLASS* CLASS::sInstance = nullptr;                                                  \
    CLASS::SingletonDisposer_* CLASS::SingletonDisposer_::sStaticDisposer = nullptr;    \
                                                                                        \
    SEAD_CREATE_SINGLETON_INSTANCE(CLASS)                                               \
    SEAD_DELETE_SINGLETON_INSTANCE(CLASS)                                               \
                                                                                        \
    CLASS::SingletonDisposer_::~SingletonDisposer_()                                    \
    {                                                                                   \
        if (this == sStaticDisposer)                                                    \
        {                                                                               \
            sStaticDisposer = nullptr;                                                  \
            CLASS::sInstance->~CLASS();                                                 \
            CLASS::sInstance = nullptr;                                                 \
        }                                                                               \
    }
