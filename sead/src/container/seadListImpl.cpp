#include <container/seadListImpl.h>

#include <basis/seadAssert.h>
#include <basis/seadWarning.h>
#include <prim/seadPtrUtil.h>
#include <random/seadRandom.h>

namespace sead {

void ListNode::insertBack_(ListNode* node)
{
    SEAD_ASSERT_MSG(!node->isLinked(), "node is already linked.");

    ListNode* next = mNext;

    mNext = node;
    node->mPrev = this;
    node->mNext = next;

    if (next)
        next->mPrev = node;
}

void ListNode::insertFront_(ListNode* node)
{
    SEAD_ASSERT_MSG(!node->isLinked(), "node is already linked.");

    ListNode* prev = mPrev;

    mPrev = node;
    node->mPrev = prev;
    node->mNext = this;

    if (prev)
        prev->mNext = node;
}

void ListNode::erase_()
{
    SEAD_ASSERT_MSG(isLinked(), "node is not linked.");

    if (mPrev)
        mPrev->mNext = mNext;

    if (mNext)
        mNext->mPrev = mPrev;

    mPrev = nullptr;
    mNext = nullptr;
}

void ListImpl::reverse()
{
    if (mCount < 2)
        return;

    ListNode* prev = mStartEnd.mPrev;
    ListNode* next = mStartEnd.mNext;
    ListNode* node;

    do
    {
        node = prev->mPrev;

        prev->erase_();
        next->insertFront_(prev);

        prev = node;
    } while (node != next);
}

void ListImpl::shuffle()
{
    Random random;
    shuffle(&random);
}

void ListImpl::shuffle(Random* random)
{
    SEAD_ASSERT(random);

    s32 count = mCount;
    while (count > 1)
    {
        u32 value = random->getU32(count);
        count--;

        ListNode* n1 = nth(count);
        ListNode* n2 = nth(value);
        swap(n1, n2);
    }
}

bool ListImpl::checkLinks() const
{
    if (!mStartEnd.mNext)
    {
        SEAD_WARNING("mStartEnd.mNext is null");
        return false;
    }

    s32 count = 0;
    for (ListNode* node = mStartEnd.mNext; node != &mStartEnd; node = node->mNext)
    {
        if (!node->mNext)
        {
            SEAD_WARNING("node->mNext is null. count = %d", count);
            return false;
        }

        if (node->mNext->mPrev != node)
        {
            SEAD_WARNING("node->mNext->mPrev[0x%p] is not equal to node[0x%p]. count = %d", node->mNext->mPrev, node, count);
            return false;
        }

        count++;
    }

    if (!mStartEnd.mPrev)
    {
        SEAD_WARNING("mStartEnd.mPrev is null");
        return false;
    }

    s32 countR = 0;
    for (ListNode* node = mStartEnd.mPrev; node != &mStartEnd; node = node->mPrev)
    {
        if (!node->mPrev)
        {
            SEAD_WARNING("node->mPrev is null. countR = %d", countR);
            return false;
        }

        if (node->mPrev->mNext != node)
        {
            SEAD_WARNING("node->mPrev->mNext[0x%p] is not equal to node[0x%p]. countR = %d", node->mPrev->mNext, node, countR);
            return false;
        }

        countR++;
    }

    if (count != mCount)
        SEAD_WARNING("count[%d] is not equal to mCount[%d]", count, mCount);

    if (countR != mCount)
        SEAD_WARNING("countR[%d] is not equal to mCount[%d]", countR, mCount);

    return count == countR && countR == mCount;
}

void ListImpl::sort(s32 offset, CompareCallbackImpl cmp)
{
    if (mCount < 2)
        return;

    for (ListNode* node = nth(1); node != &mStartEnd; node = node->mNext)
    {
        ListNode* prev = node->mPrev;

        s32 result = cmp(PtrUtil::addOffset(prev, -offset), PtrUtil::addOffset(node, -offset));
        if (result > 0)
        {
            do
            {
                prev = prev->mPrev;
                if (prev == &mStartEnd)
                    break;

                result = cmp(PtrUtil::addOffset(prev, -offset), PtrUtil::addOffset(node, -offset));
            } while (result > 0);

            node->erase_();
            prev->insertBack_(node);
        }
    }
}

void ListImpl::mergeSort(s32 offset, CompareCallbackImpl cmp)
{
    mergeSortImpl(mStartEnd.next(), mStartEnd.prev(), mCount, offset, cmp);
}

ListNode* ListImpl::popBack()
{
    ListNode* node = back();
    if (node)
       erase(node);

    return node;
}

ListNode* ListImpl::popFront()
{
    ListNode* node = front();
    if (node)
       erase(node);

    return node;
}

ListNode* ListImpl::nth(s32 index) const
{
    if (static_cast<u32>(index) >= static_cast<u32>(mCount))
    {
        SEAD_ASSERT_MSG(false, "index exceeded[%d/%d]", index, mCount);
        return nullptr;
    }

    ListNode* node = mStartEnd.mNext;
    for (s32 i = 0; i < index; i++)
        node = node->mNext;

    return node;
}

s32 ListImpl::indexOf(const ListNode* node) const
{
    ListNode* n = mStartEnd.mNext;

    s32 index = 0;
    while (n != &mStartEnd)
    {
        if (n == node)
            return index;

        index++;
        n = n->mNext;
    }

    return -1;
}

void ListImpl::swap(ListNode* n1, ListNode* n2)
{
    SEAD_ASSERT(n1->mPrev && n1->mNext && n2->mPrev && n2->mNext);

    if (n1 == n2)
        return;

    ListNode* n1Prev = n1->mPrev;
    ListNode* n2Prev = n2->mPrev;

    if (n2Prev != n1)
    {
        n1->erase_();
        n2Prev->insertBack_(n1);
    }

    if (n1Prev != n2)
    {
        n2->erase_();
        n1Prev->insertBack_(n2);
    }
}

void ListImpl::moveAfter(ListNode* basis, ListNode* node)
{
    SEAD_ASSERT(basis->mPrev && basis->mNext && node->mPrev && node->mNext);

    if (basis == node)
        return;

    node->erase_();
    basis->insertBack_(node);
}

void ListImpl::moveBefore(ListNode* basis, ListNode* node)
{
    SEAD_ASSERT(basis->mPrev && basis->mNext && node->mPrev && node->mNext);

    if (basis == node)
        return;

    node->erase_();
    basis->insertFront_(node);
}

ListNode* ListImpl::find(const void* ptr, s32 offset, CompareCallbackImpl cmp) const
{
    for (ListNode* node = mStartEnd.mNext; node != &mStartEnd; node = node->mNext)
    {
        if (cmp(PtrUtil::addOffset(node, -offset), ptr) == 0)
            return node;
    }

    return nullptr;
}

void ListImpl::uniq(s32 offset, CompareCallbackImpl cmp)
{
    if (mCount < 2)
        return;

    for (ListNode* node = mStartEnd.mNext; node != &mStartEnd; node = node->mNext)
    {
        for (ListNode* next = node->mNext; next != &mStartEnd; )
        {
            if (cmp(PtrUtil::addOffset(node, -offset), PtrUtil::addOffset(next, -offset)) == 0)
            {
                ListNode* tmp = next->mNext;
                erase(next);
                next = tmp;
            }
            else
            {
                next = next->mNext;
            }
        }
    }
}

void ListImpl::clear()
{
    ListNode* node = mStartEnd.mNext;
    while (node != &mStartEnd)
    {
        ListNode* next = node->mNext;
        node->init_();
        node = next;
    }

    mCount = 0;
    mStartEnd.mPrev = &mStartEnd;
    mStartEnd.mNext = &mStartEnd;
}

void ListImpl::unsafeClear()
{
    mCount = 0;
    mStartEnd.mPrev = &mStartEnd;
    mStartEnd.mNext = &mStartEnd;
}

// TODO: Better var names
void ListImpl::mergeSortImpl(ListNode* front, ListNode* back, s32 num, s32 offset, CompareCallbackImpl cmp)
{
    if (num < 9)
    {
        if (num > 1)
        {
            ListNode* frontPrev = front->prev();
            ListNode* frontNext = front->next();
            ListNode* backNext = back->next();

            for (ListNode* node = frontNext; node != backNext; node = node->mNext)
            {
                ListNode* prev = node->mPrev;

                s32 result = cmp(PtrUtil::addOffset(prev, -offset), PtrUtil::addOffset(node, -offset));
                if (result > 0)
                {
                    do
                    {
                        prev = prev->mPrev;
                        if (prev == frontPrev)
                            break;

                        result = cmp(PtrUtil::addOffset(prev, -offset), PtrUtil::addOffset(node, -offset));
                    } while (result > 0);

                    node->erase_();
                    prev->insertBack_(node);
                }
            }
        }
    }
    else
    {
        s32 numHalf = num / 2;

        ListNode* node = front;
        for (s32 i = 1; i < numHalf; i++)
            node = node->next();

        s32 numMinusHalf = num - numHalf;

        ListNode* frontPrev = front->prev();
        ListNode* nodeNext = node->next();

        mergeSortImpl(front, node, numHalf, offset, cmp);

        ListNode* frontPrevNext = frontPrev->next();
        ListNode* nodeNextPrev = nodeNext->prev();

        mergeSortImpl(nodeNext, back, numMinusHalf, offset, cmp);

        ListNode* nodeNextPrevNext = nodeNextPrev->next();

        while (numHalf > 0 || numMinusHalf > 0)
        {
            bool b;

            if (numHalf == 0)
            {
                b = false;
            }
            else if (numMinusHalf == 0)
            {
                b = true;
            }
            else
            {
                s32 result = cmp(PtrUtil::addOffset(frontPrevNext, -offset), PtrUtil::addOffset(nodeNextPrevNext, -offset));
                if (result < 1)
                    b = true;
                else
                    b = false;
            }

            if (b)
            {
                nodeNext = frontPrevNext->next();
                frontPrevNext->erase_();
                frontPrev->insertBack_(frontPrevNext);
                frontPrev = frontPrevNext;
                numHalf--;
                frontPrevNext = nodeNext;
            }
            else
            {
                nodeNext = nodeNextPrevNext->next();
                nodeNextPrevNext->erase_();
                frontPrev->insertBack_(nodeNextPrevNext);
                frontPrev = nodeNextPrevNext;
                numMinusHalf--;
                nodeNextPrevNext = nodeNext;
            }
        }
    }
}

} // namespace sead
