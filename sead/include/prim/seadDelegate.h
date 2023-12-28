#pragma once

#include <basis/seadNew.h>

namespace sead {

template <typename TObject, typename TMethodPtr, typename TIDelegate>
class DelegateBase : public TIDelegate
{
protected:
    using MethodPtr = TMethodPtr;

public:
    DelegateBase()
        : mObject(nullptr)
        , mMethod(nullptr)
    {
    }

    DelegateBase(TObject* obj, MethodPtr method)
        : mObject(obj)
        , mMethod(method)
    {
    }

    void bind(TObject* obj, MethodPtr method)
    {
        mObject = obj;
        mMethod = method;
    }

    void bind(TObject* obj)
    {
        mObject = obj;
    }

    void bind(MethodPtr method)
    {
        mMethod = method;
    }

    void unbind()
    {
        mObject = nullptr;
        mMethod = nullptr;
    }

    TObject* getObject() const
    {
        return mObject;
    }

protected:
    TObject* mObject;
    MethodPtr mMethod;
};

template <typename TObject, typename TResult>
class DelegateTraits
{
public:
    using MethodPtr = TResult (TObject::*)();
};

template <typename TObject, typename TResult>
class DelegateTraitsConst
{
public:
    using MethodPtr = TResult (TObject::*)() const;
};

template <typename TResult>
class DelegateTraits<void, TResult>
{
public:
    using MethodPtr = TResult (*)();
};

class IDelegate
{
public:
    virtual void invoke() = 0;
    virtual IDelegate* clone(Heap* heap) const = 0;

    void operator()()
    {
        return invoke();
    }
};

template <typename TObject>
class Delegate : public DelegateBase<TObject, typename DelegateTraits<TObject, void>::MethodPtr, IDelegate>
{
public:
    using Base = DelegateBase<TObject, typename DelegateTraits<TObject, void>::MethodPtr, IDelegate>;
    using MethodPtr = typename Base::MethodPtr;

public:
    Delegate()
        : Base()
    {
    }

    Delegate(TObject* obj, MethodPtr method)
        : Base(obj, method)
    {
    }

    void invoke() override
    {
        if (this->mObject && this->mMethod)
            (this->mObject->*this->mMethod)();
    }

    Delegate* clone(Heap* heap) const override
    {
        return new(heap) Delegate(*this);
    }

    void invoke() const
    {
        if (this->mObject && this->mMethod)
            (this->mObject->*this->mMethod)();
    }

    void operator()() const
    {
        invoke();
    }
};

template <>
class Delegate<void> : public DelegateBase<void, typename DelegateTraits<void, void>::MethodPtr, IDelegate>
{
public:
    using Base = DelegateBase<void, typename DelegateTraits<void, void>::MethodPtr, IDelegate>;
    using MethodPtr = typename Base::MethodPtr;

public:
    Delegate()
        : Base()
    {
    }

    explicit Delegate(MethodPtr method)
        : Base(nullptr, method)
    {
    }

    void invoke() override
    {
        if (this->mMethod)
            (this->mMethod)();
    }

    Delegate* clone(Heap* heap) const override
    {
        return new(heap) Delegate(*this);
    }

    void invoke() const
    {
        if (this->mMethod)
            (this->mMethod)();
    }

    void operator()() const
    {
        invoke();
    }
};

template <typename TObject>
class DelegateConst : public DelegateBase<const TObject, typename DelegateTraitsConst<TObject, void>::MethodPtr, IDelegate>
{
public:
    using Base = DelegateBase<const TObject, typename DelegateTraitsConst<TObject, void>::MethodPtr, IDelegate>;
    using MethodPtr = typename Base::MethodPtr;

public:
    DelegateConst()
        : Base()
    {
    }

    DelegateConst(const TObject* obj, MethodPtr method)
        : Base(obj, method)
    {
    }

    void invoke() override
    {
        if (this->mObject && this->mMethod)
            (this->mObject->*this->mMethod)();
    }

    DelegateConst* clone(Heap* heap) const override
    {
        return new(heap) DelegateConst(*this);
    }

    void invoke() const
    {
        if (this->mObject && this->mMethod)
            (this->mObject->*this->mMethod)();
    }

    void operator()() const
    {
        invoke();
    }
};

class StaticDelegate : public Delegate<void>
{
public:
    StaticDelegate()
        : Delegate<void>()
    {
    }

    explicit StaticDelegate(typename Delegate<void>::MethodPtr method)
        : Delegate<void>(method)
    {
    }
};

class AnyDelegate
{
private:
    class Dummy
    {
    };

    using DelegateDummy = Delegate<Dummy>;

public:
    AnyDelegate()
        : mStorage()
        , mDelegate(nullptr)
    {
    }

    template <typename TObject>
    void bind(TObject* obj, typename Delegate<TObject>::MethodPtr method)
    {
        static_assert(sizeof(mStorage) >= sizeof(Delegate<TObject>));
        static_assert(alignof(decltype(mStorage)) % alignof(Delegate<TObject>) == 0);

        mDelegate = new(&mStorage) Delegate<TObject>(obj, method);
    }

    void bind(typename Delegate<void>::MethodPtr method)
    {
        static_assert(sizeof(mStorage) >= sizeof(Delegate<void>));
        static_assert(alignof(decltype(mStorage)) % alignof(Delegate<void>) == 0);

        mDelegate = new(&mStorage) Delegate<void>(method);
    }

    void invoke()
    {
        if (mDelegate)
            mDelegate->invoke();
    }

    void operator()()
    {
        return invoke();
    }

private:
    /*
     * We can't assume pointer to member function have the same size and alignment as they're implementation specific
     * So we hardcode some values here and hope it's good
     * Plus the static_asserts should be good enough
    */
    std::aligned_storage_t<sizeof(DelegateDummy) + cPtrSize, 8> mStorage;
    IDelegate* mDelegate;
};

template <typename TObject>
inline Delegate<TObject> DelegateCreator(TObject* obj, typename Delegate<TObject>::MethodPtr method)
{
    return Delegate<TObject>(obj, method);
}

inline Delegate<void> FunctionDelegateCreator(typename Delegate<void>::MethodPtr method)
{
    return Delegate<void>(method);
}

template <typename TObject, typename TArg1, typename TResult>
class DelegateTraits1
{
public:
    using MethodPtr = TResult (TObject::*)(TArg1);
};

template <typename TObject, typename TArg1, typename TResult>
class DelegateTraits1Const
{
public:
    using MethodPtr = TResult (TObject::*)(TArg1) const;
};

template <typename TArg1, typename TResult>
class DelegateTraits1<void, TArg1, TResult>
{
public:
    using MethodPtr = TResult (*)(TArg1);
};

template <typename TArg1>
class IDelegate1
{
public:
    virtual void invoke(TArg1 arg1) = 0;
    virtual IDelegate1* clone(Heap* heap) const = 0;

    void operator()(TArg1 arg1)
    {
        return invoke(arg1);
    }
};

template <typename TObject, typename TArg1>
class Delegate1 : public DelegateBase<TObject, typename DelegateTraits1<TObject, TArg1, void>::MethodPtr, IDelegate1<TArg1>>
{
public:
    using Base = DelegateBase<TObject, typename DelegateTraits1<TObject, TArg1, void>::MethodPtr, IDelegate1<TArg1>>;
    using MethodPtr = typename Base::MethodPtr;

public:
    Delegate1()
        : Base()
    {
    }

    Delegate1(TObject* obj, MethodPtr method)
        : Base(obj, method)
    {
    }

    void invoke(TArg1 arg1) override
    {
        if (this->mObject && this->mMethod)
            (this->mObject->*this->mMethod)(arg1);
    }

    Delegate1* clone(Heap* heap) const override
    {
        return new(heap) Delegate1(*this);
    }

    void invoke(TArg1 arg1) const
    {
        if (this->mObject && this->mMethod)
            (this->mObject->*this->mMethod)(arg1);
    }

    void operator()(TArg1 arg1) const
    {
        invoke(arg1);
    }
};

template <typename TArg1>
class Delegate1<void, TArg1> : public DelegateBase<void, typename DelegateTraits1<void, TArg1, void>::MethodPtr, IDelegate1<TArg1>>
{
public:
    using Base = DelegateBase<void, typename DelegateTraits1<void, TArg1, void>::MethodPtr, IDelegate1<TArg1>>;
    using MethodPtr = typename Base::MethodPtr;

public:
    Delegate1()
        : Base()
    {
    }

    explicit Delegate1(MethodPtr method)
        : Base(nullptr, method)
    {
    }

    void invoke(TArg1 arg1) override
    {
        if (this->mMethod)
            (this->mMethod)(arg1);
    }

    Delegate1* clone(Heap* heap) const override
    {
        return new(heap) Delegate1(*this);
    }

    void invoke(TArg1 arg1) const
    {
        if (this->mMethod)
            (this->mMethod)(arg1);
    }

    void operator()(TArg1 arg1) const
    {
        invoke(arg1);
    }
};

template <typename TObject, typename TArg1>
class Delegate1Const : public DelegateBase<const TObject, typename DelegateTraits1Const<TObject, TArg1, void>::MethodPtr, IDelegate1<TArg1>>
{
public:
    using Base = DelegateBase<const TObject, typename DelegateTraits1Const<TObject, TArg1, void>::MethodPtr, IDelegate1<TArg1>>;
    using MethodPtr = typename Base::MethodPtr;

public:
    Delegate1Const()
        : Base()
    {
    }

    Delegate1Const(const TObject* obj, MethodPtr method)
        : Base(obj, method)
    {
    }

    void invoke(TArg1 arg1) override
    {
        if (this->mObject && this->mMethod)
            (this->mObject->*this->mMethod)(arg1);
    }

    Delegate1Const* clone(Heap* heap) const override
    {
        return new(heap) Delegate1Const(*this);
    }

    void invoke(TArg1 arg1) const
    {
        if (this->mObject && this->mMethod)
            (this->mObject->*this->mMethod)(arg1);
    }

    void operator()(TArg1 arg1) const
    {
        invoke(arg1);
    }
};

template <typename TArg1>
class StaticDelegate1 : public Delegate1<void, TArg1>
{
public:
    StaticDelegate1()
        : Delegate1<void, TArg1>()
    {
    }

    explicit StaticDelegate1(typename Delegate1<void, TArg1>::MethodPtr method)
        : Delegate1<void, TArg1>(method)
    {
    }
};

template <typename TArg1>
class AnyDelegate1
{
private:
    class Dummy
    {
    };

    using DelegateDummy = Delegate1<Dummy, TArg1>;

public:
    AnyDelegate1()
        : mStorage()
        , mDelegate(nullptr)
    {
    }

    template <typename TObject>
    void bind(TObject* obj, typename Delegate1<TObject, TArg1>::MethodPtr method)
    {
        static_assert(sizeof(mStorage) >= sizeof(Delegate1<TObject, TArg1>));
        static_assert(alignof(decltype(mStorage)) % alignof(Delegate1<TObject, TArg1>) == 0);

        mDelegate = new(&mStorage) Delegate1<TObject, TArg1>(obj, method);
    }

    void bind(typename Delegate1<void, TArg1>::MethodPtr method)
    {
        static_assert(sizeof(mStorage) >= sizeof(Delegate1<void, TArg1>));
        static_assert(alignof(decltype(mStorage)) % alignof(Delegate1<void, TArg1>) == 0);

        mDelegate = new(&mStorage) Delegate1<void, TArg1>(method);
    }

    void invoke(TArg1 arg1)
    {
        if (mDelegate)
            mDelegate->invoke(arg1);
    }

    void operator()(TArg1 arg1)
    {
        return invoke(arg1);
    }

private:
    /*
     * We can't assume pointer to member function have the same size and alignment as they're implementation specific
     * So we hardcode some values here and hope it's good
     * Plus the static_asserts should be good enough
    */
    std::aligned_storage_t<sizeof(DelegateDummy) + cPtrSize, 8> mStorage;
    IDelegate1<TArg1>* mDelegate;
};

template <typename TObject, typename TArg1>
inline Delegate1<TObject, TArg1> DelegateCreator(TObject* obj, typename Delegate1<TObject, TArg1>::MethodPtr method)
{
    return Delegate1<TObject, TArg1>(obj, method);
}

template <typename TArg1>
inline Delegate1<void, TArg1> FunctionDelegateCreator(typename Delegate1<void, TArg1>::MethodPtr method)
{
    return Delegate1<void, TArg1>(method);
}

template <typename TObject, typename TArg1, typename TArg2, typename TResult>
class DelegateTraits2
{
public:
    using MethodPtr = TResult (TObject::*)(TArg1, TArg2);
};

template <typename TObject, typename TArg1, typename TArg2, typename TResult>
class DelegateTraits2Const
{
public:
    using MethodPtr = TResult (TObject::*)(TArg1, TArg2) const;
};

template <typename TArg1, typename TArg2, typename TResult>
class DelegateTraits2<void, TArg1, TArg2, TResult>
{
public:
    using MethodPtr = TResult (*)(TArg1, TArg2);
};

template <typename TArg1, typename TArg2>
class IDelegate2
{
public:
    virtual void invoke(TArg1 arg1, TArg2 arg2) = 0;
    virtual IDelegate2* clone(Heap* heap) const = 0;

    void operator()(TArg1 arg1, TArg2 arg2)
    {
        return invoke(arg1, arg2);
    }
};

template <typename TObject, typename TArg1, typename TArg2>
class Delegate2 : public DelegateBase<TObject, typename DelegateTraits2<TObject, TArg1, TArg2, void>::MethodPtr, IDelegate2<TArg1, TArg2>>
{
public:
    using Base = DelegateBase<TObject, typename DelegateTraits2<TObject, TArg1, TArg2, void>::MethodPtr, IDelegate2<TArg1, TArg2>>;
    using MethodPtr = typename Base::MethodPtr;

public:
    Delegate2()
        : Base()
    {
    }

    Delegate2(TObject* obj, MethodPtr method)
        : Base(obj, method)
    {
    }

    void invoke(TArg1 arg1, TArg2 arg2) override
    {
        if (this->mObject && this->mMethod)
            (this->mObject->*this->mMethod)(arg1, arg2);
    }

    Delegate2* clone(Heap* heap) const override
    {
        return new(heap) Delegate2(*this);
    }

    void invoke(TArg1 arg1, TArg2 arg2) const
    {
        if (this->mObject && this->mMethod)
            (this->mObject->*this->mMethod)(arg1, arg2);
    }

    void operator()(TArg1 arg1, TArg2 arg2) const
    {
        invoke(arg1, arg2);
    }
};

template <typename TArg1, typename TArg2>
class Delegate2<void, TArg1, TArg2> : public DelegateBase<void, typename DelegateTraits2<void, TArg1, TArg2, void>::MethodPtr, IDelegate2<TArg1, TArg2>>
{
public:
    using Base = DelegateBase<void, typename DelegateTraits2<void, TArg1, TArg2, void>::MethodPtr, IDelegate2<TArg1, TArg2>>;
    using MethodPtr = typename Base::MethodPtr;

public:
    Delegate2()
        : Base()
    {
    }

    explicit Delegate2(MethodPtr method)
        : Base(nullptr, method)
    {
    }

    void invoke(TArg1 arg1, TArg2 arg2) override
    {
        if (this->mMethod)
            (this->mMethod)(arg1, arg2);
    }

    Delegate2* clone(Heap* heap) const override
    {
        return new(heap) Delegate2(*this);
    }

    void invoke(TArg1 arg1, TArg2 arg2) const
    {
        if (this->mMethod)
            (this->mMethod)(arg1, arg2);
    }

    void operator()(TArg1 arg1, TArg2 arg2) const
    {
        invoke(arg1, arg2);
    }
};

template <typename TObject, typename TArg1, typename TArg2>
class Delegate2Const : public DelegateBase<const TObject, typename DelegateTraits2Const<TObject, TArg1, TArg2, void>::MethodPtr, IDelegate2<TArg1, TArg2>>
{
public:
    using Base = DelegateBase<const TObject, typename DelegateTraits2Const<TObject, TArg1, TArg2, void>::MethodPtr, IDelegate2<TArg1, TArg2>>;
    using MethodPtr = typename Base::MethodPtr;

public:
    Delegate2Const()
        : Base()
    {
    }

    Delegate2Const(const TObject* obj, MethodPtr method)
        : Base(obj, method)
    {
    }

    void invoke(TArg1 arg1, TArg2 arg2) override
    {
        if (this->mObject && this->mMethod)
            (this->mObject->*this->mMethod)(arg1, arg2);
    }

    Delegate2Const* clone(Heap* heap) const override
    {
        return new(heap) Delegate2Const(*this);
    }

    void invoke(TArg1 arg1, TArg2 arg2) const
    {
        if (this->mObject && this->mMethod)
            (this->mObject->*this->mMethod)(arg1, arg2);
    }

    void operator()(TArg1 arg1, TArg2 arg2) const
    {
        invoke(arg1, arg2);
    }
};

template<typename TArg1, typename TArg2>
class StaticDelegate2 : public Delegate2<void, TArg1, TArg2>
{
public:
    StaticDelegate2()
        : Delegate2<void, TArg1, TArg2>()
    {
    }

    explicit StaticDelegate2(typename Delegate2<void, TArg1, TArg2>::MethodPtr method)
        : Delegate2<void, TArg1, TArg2>(method)
    {
    }
};

template <typename TArg1, typename TArg2>
class AnyDelegate2
{
private:
    class Dummy
    {
    };

    using DelegateDummy = Delegate2<Dummy, TArg1, TArg2>;

public:
    AnyDelegate2()
        : mStorage()
        , mDelegate(nullptr)
    {
    }

    template <typename TObject>
    void bind(TObject* obj, typename Delegate2<TObject, TArg1, TArg2>::MethodPtr method)
    {
        static_assert(sizeof(mStorage) >= sizeof(Delegate2<TObject, TArg1, TArg2>));
        static_assert(alignof(decltype(mStorage)) % alignof(Delegate2<TObject, TArg1, TArg2>) == 0);

        mDelegate = new(&mStorage) Delegate2<TObject, TArg1, TArg2>(obj, method);
    }

    void bind(typename Delegate2<void, TArg1, TArg2>::MethodPtr method)
    {
        static_assert(sizeof(mStorage) >= sizeof(Delegate2<void, TArg1, TArg2>));
        static_assert(alignof(decltype(mStorage)) % alignof(Delegate2<void, TArg1, TArg2>) == 0);

        mDelegate = new(&mStorage) Delegate2<void, TArg1, TArg2>(method);
    }

    void invoke(TArg1 arg1, TArg2 arg2)
    {
        if (mDelegate)
            mDelegate->invoke(arg1, arg2);
    }

    void operator()(TArg1 arg1, TArg2 arg2)
    {
        return invoke(arg1, arg2);
    }

private:
    /*
     * We can't assume pointer to member function have the same size and alignment as they're implementation specific
     * So we hardcode some values here and hope it's good
     * Plus the static_asserts should be good enough
    */
    std::aligned_storage_t<sizeof(DelegateDummy) + cPtrSize, 8> mStorage;
    IDelegate2<TArg1, TArg2>* mDelegate;
};

template <typename TObject, typename TArg1, typename TArg2>
inline Delegate2<TObject, TArg1, TArg2> DelegateCreator(TObject* obj, typename Delegate2<TObject, TArg1, TArg2>::MethodPtr method)
{
    return Delegate2<TObject, TArg1, TArg2>(obj, method);
}

template <typename TArg1, typename TArg2>
inline Delegate2<void, TArg1, TArg2> FunctionDelegateCreator(typename Delegate2<void, TArg1, TArg2>::MethodPtr method)
{
    return Delegate2<void, TArg1, TArg2>(method);
}

} // namespace sead
