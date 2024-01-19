#pragma once

namespace sead { namespace RuntimeTypeInfo {

class Interface
{
public:
    Interface()
    {
    }

    virtual bool isDerived(const Interface* other) const = 0;
};

class Root : public Interface
{
public:
    Root()
    {
    }

    bool isDerived(const Interface* other) const override
    {
        return this == other;
    }
};

template <typename Super>
class Derive : public Interface
{
public:
    Derive()
    {
    }

    bool isDerived(const Interface* other) const override
    {
        if (this == other)
            return true;

        return Super::getRuntimeTypeInfoStatic()->isDerived(other);
    }
};

} // namespace RuntimeTypeInfo

template <typename DerivedType, typename Type>
inline bool IsDerivedTypes(const Type* ptr)
{
    const RuntimeTypeInfo::Interface* typeinfo = DerivedType::getRuntimeTypeInfoStatic();
    return ptr && ptr->checkDerivedRuntimeTypeInfo(typeinfo);
}

template <typename DerivedType, typename Type>
inline DerivedType* DynamicCast(Type* ptr)
{
    if (IsDerivedTypes<DerivedType, Type>(ptr))
        return static_cast<DerivedType*>(ptr);

    return nullptr;
}

} // namespace sead

#define SEAD_RTTI_BASE(CLASS)                                                                           \
public:                                                                                                 \
    static const sead::RuntimeTypeInfo::Interface* getRuntimeTypeInfoStatic()                           \
    {                                                                                                   \
        static const sead::RuntimeTypeInfo::Root typeInfo;                                              \
        return &typeInfo;                                                                               \
    }                                                                                                   \
                                                                                                        \
    static bool checkDerivedRuntimeTypeInfoStatic(const sead::RuntimeTypeInfo::Interface* type)         \
    {                                                                                                   \
        return type == CLASS::getRuntimeTypeInfoStatic();                                               \
    }                                                                                                   \
                                                                                                        \
    virtual bool checkDerivedRuntimeTypeInfo(const sead::RuntimeTypeInfo::Interface* type) const        \
    {                                                                                                   \
        return checkDerivedRuntimeTypeInfoStatic(type);                                                 \
    }                                                                                                   \
                                                                                                        \
    virtual const sead::RuntimeTypeInfo::Interface* getRuntimeTypeInfo() const                          \
    {                                                                                                   \
        return getRuntimeTypeInfoStatic();                                                              \
    }

#define SEAD_RTTI_OVERRIDE(CLASS, BASE)                                                                 \
public:                                                                                                 \
    static const sead::RuntimeTypeInfo::Interface* getRuntimeTypeInfoStatic()                           \
    {                                                                                                   \
        static const sead::RuntimeTypeInfo::Derive<BASE> typeInfo;                                      \
        return &typeInfo;                                                                               \
    }                                                                                                   \
                                                                                                        \
    static bool checkDerivedRuntimeTypeInfoStatic(const sead::RuntimeTypeInfo::Interface* type)         \
    {                                                                                                   \
        if (type == CLASS::getRuntimeTypeInfoStatic())                                                  \
            return true;                                                                                \
                                                                                                        \
        return BASE::checkDerivedRuntimeTypeInfoStatic(type);                                           \
    }                                                                                                   \
                                                                                                        \
    bool checkDerivedRuntimeTypeInfo(const sead::RuntimeTypeInfo::Interface* type) const override       \
    {                                                                                                   \
        return checkDerivedRuntimeTypeInfoStatic(type);                                                 \
    }                                                                                                   \
                                                                                                        \
    const sead::RuntimeTypeInfo::Interface* getRuntimeTypeInfo() const override                         \
    {                                                                                                   \
        return getRuntimeTypeInfoStatic();                                                              \
    }
