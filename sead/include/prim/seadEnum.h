#pragma once

#include <basis/seadTypes.h>

namespace sead {

// TODO
#define SEAD_ENUM(NAME, ...)                                    \
class NAME                                                      \
{                                                               \
public:                                                         \
    NAME();                                                     \
    NAME(ValueType);                                            \
    NAME(s32);                                                  \
    NAME(const NAME&);                                          \
    NAME(const volatile NAME&);                                 \
                                                                \
    ValueType value() const;                                    \
    ValueType value() const volatile;                           \
                                                                \
    s32* valuePtr();                                            \
    s32* indexPtr();                                            \
    s32 getRelativeIndex() const;                               \
    s32 getRelativeIndex() const volatile;                      \
    void setRelativeIndex(s32);                                 \
    void setRelativeIndex(s32) volatile;                        \
                                                                \
    operator s32() const volatile;                              \
                                                                \
    bool operator==(ValueType) const;                           \
    bool operator==(ValueType) const volatile;                  \
                                                                \
    static s32 getStart();                                      \
    static s32 size();                                          \
    static s32 getSize();                                       \
                                                                \
    static iterator begin();                                    \
    static iterator end();                                      \
                                                                \
private:                                                        \
    static ValueType compileAssertNoSetValue_();                \
                                                                \
public:                                                         \
    NAME& operator=(ValueType);                                 \
    volatile NAME& operator=(ValueType) volatile;               \
    NAME& operator=(const NAME&);                               \
    NAME& operator=(const volatile NAME&);                      \
    volatile NAME& operator=(const NAME&) volatile;             \
    volatile NAME& operator=(const volatile NAME&) volatile;    \
                                                                \
protected:                                                      \
    s32 mValue;                                                 \
};

} // namespace sead
