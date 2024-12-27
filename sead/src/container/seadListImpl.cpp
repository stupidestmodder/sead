#include <container/seadListImpl.h>

#include <basis/seadRawPrint.h>
#include <basis/seadWarning.h>
#include <prim/seadPtrUtil.h>

namespace sead {

void ListNode::insertBack_(ListNode* n)
{
    SEAD_ASSERT_MSG(!n->isLinked(), "node is already linked.");

    ListNode* next = mNext;
    mNext = n;

    n->mPrev = this;
    n->mNext = next;

    if (next)
    {
        next->mPrev = n;
    }
}

void ListNode::insertFront_(ListNode* n)
{
    SEAD_ASSERT_MSG(!n->isLinked(), "node is already linked.");

    ListNode* prev = mPrev;
    mPrev = n;

    n->mPrev = prev;
    n->mNext = this;

    if (prev)
    {
        prev->mNext = n;
    }
}

void ListNode::erase_()
{
    SEAD_ASSERT_MSG(isLinked(), "node is not linked.");

    if (mPrev)
    {
        mPrev->mNext = mNext;
    }

    if (mNext)
    {
        mNext->mPrev = mPrev;
    }

    mPrev = nullptr;
    mNext = nullptr;
}

void ListImpl::reverse()
{
    if (mCount < 2)
    {
        return;
    }

    ListNode* first;
    ListNode* node;
    ListNode* next;

    first = mStartEnd.mNext;
    node = mStartEnd.mPrev;
    do
    {
        next = node->mPrev;
        node->erase_();
        first->insertFront_(node);
        node = next;
    } while (node != first);
}

void ListImpl::shuffle(Random* random)
{
    SEAD_ASSERT(random);

    s32 n = mCount;
    s32 k;
    while (n > 1)
    {
        k = random->getU32(n);
        n--;

        swap(nth(n), nth(k));
    }
}

bool ListImpl::checkLinks() const
{
    if (!mStartEnd.mNext)
    {
        SEAD_WARNING("mStartEnd.mNext is null");
        return false;
    }

    ListNode* node;

    s32 count = 0;
    for (node = mStartEnd.mNext; node != &mStartEnd; node = node->mNext)
    {
        if (!node->mNext)
        {
            SEAD_WARNING("node->mNext is null. count = %d", count);
            return false;
        }

        if (node->mNext->mPrev != node)
        {
            SEAD_WARNING(
                "node->mNext->mPrev[" SEAD_FMT_UINTPTR "] is not equal to node[" SEAD_FMT_UINTPTR "]. count = %d",
                node->mNext->mPrev, node, count
            );
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
    for (node = mStartEnd.mPrev; node != &mStartEnd; node = node->mPrev)
    {
        if (!node->mPrev)
        {
            SEAD_WARNING("node->mPrev is null. countR = %d", countR);
            return false;
        }

        if (node->mPrev->mNext != node)
        {
            SEAD_WARNING(
                "node->mPrev->mNext[" SEAD_FMT_UINTPTR "] is not equal to node[" SEAD_FMT_UINTPTR "]. countR = %d",
                node->mPrev->mNext, node, countR
            );
            return false;
        }

        countR++;
    }

    if (mCount != count)
    {
        SEAD_WARNING("count[%d] is not equal to mCount[%d]", count, mCount);
    }

    if (mCount != countR)
    {
        SEAD_WARNING("countR[%d] is not equal to mCount[%d]", countR, mCount);
    }

    return count == countR && mCount == countR;
}

void ListImpl::sort(s32 offset, CompareCallbackImpl cmp)
{
    if (mCount < 2)
    {
        return;
    }

    ListNode* node = nth(1);
    while (node != &mStartEnd)
    {
        ListNode* next = node->mNext;
        ListNode* prev = node->mPrev;

        if (cmp(PtrUtil::addOffset(prev, -offset), PtrUtil::addOffset(node, -offset)) > 0)
        {
            do
            {
                prev = prev->mPrev;
                if (prev == &mStartEnd)
                {
                    break;
                }
            } while (cmp(PtrUtil::addOffset(prev, -offset), PtrUtil::addOffset(node, -offset)) > 0);

            node->erase_();
            prev->insertBack_(node);
        }

        node = next;
    }
}

void ListImpl::mergeSort(s32 offset, CompareCallbackImpl cmp)
{
    mergeSortImpl(mStartEnd.next(), mStartEnd.prev(), mCount, offset, cmp);
}

ListNode* ListImpl::popBack()
{
    if (mCount < 1)
    {
        return nullptr;
    }

    ListNode* node = mStartEnd.mPrev;
    node->erase_();
    mCount--;

    return node;
}

ListNode* ListImpl::popFront()
{
    if (mCount < 1)
    {
        return nullptr;
    }

    ListNode* node = mStartEnd.mNext;
    node->erase_();
    mCount--;

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
    {
        node = node->mNext;
    }

    return node;
}

s32 ListImpl::indexOf(const ListNode* n) const
{
    ListNode* node = mStartEnd.mNext;

    s32 index = 0;
    while (node != &mStartEnd)
    {
        if (node == n)
        {
            return index;
        }

        node = node->mNext;
        index++;
    }

    return -1;
}

void ListImpl::swap(ListNode* n1, ListNode* n2)
{
    SEAD_ASSERT(n1->mPrev && n1->mNext && n2->mPrev && n2->mNext);

    if (n1 == n2)
    {
        return;
    }

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

void ListImpl::moveAfter(ListNode* basis, ListNode* n)
{
    SEAD_ASSERT(basis->mPrev && basis->mNext && n->mPrev && n->mNext);

    if (basis == n)
    {
        return;
    }

    n->erase_();
    basis->insertBack_(n);
}

void ListImpl::moveBefore(ListNode* basis, ListNode* n)
{
    SEAD_ASSERT(basis->mPrev && basis->mNext && n->mPrev && n->mNext);

    if (basis == n)
    {
        return;
    }

    n->erase_();
    basis->insertFront_(n);
}

ListNode* ListImpl::find(const void* ptr, s32 offset, CompareCallbackImpl cmp) const
{
    for (ListNode* node = mStartEnd.mNext; node != &mStartEnd; node = node->mNext)
    {
        if (cmp(PtrUtil::addOffset(node, -offset), ptr) == 0)
        {
            return node;
        }
    }

    return nullptr;
}

void ListImpl::uniq(s32 offset, CompareCallbackImpl cmp)
{
    if (mCount < 2)
    {
        return;
    }

    for (ListNode* node = mStartEnd.mNext; node != &mStartEnd; node = node->mNext)
    {
        ListNode* checkNode = node->mNext;
        while (checkNode != &mStartEnd)
        {
            if (cmp(PtrUtil::addOffset(node, -offset), PtrUtil::addOffset(checkNode, -offset)) == 0)
            {
                ListNode* temp = checkNode->mNext;
                erase(checkNode);
                checkNode = temp;
            }
            else
            {
                checkNode = checkNode->mNext;
            }
        }
    }
}

void ListImpl::clear()
{
    ListNode* node = mStartEnd.mNext;
    while (node != &mStartEnd)
    {
        ListNode* temp = node->mNext;
        node->init_();
        node = temp;
    }

    mCount = 0;
    mStartEnd.mPrev = &mStartEnd;
    mStartEnd.mNext = &mStartEnd;
}

void ListImpl::mergeSortImpl(ListNode* front, ListNode* back, s32 num, s32 offset, CompareCallbackImpl cmp)
{
    if (num >= 9)
    {
        s32 frontCount;
        ListNode* middle;
        s32 backCount;
        ListNode* mergeTop;
        ListNode* backTop;
        bool takeFromFront;

        frontCount = num / 2;
        middle = front;
        for (s32 i = 1; i < frontCount; i++)
        {
            middle = middle->next();
        }

        backCount = num - frontCount;

        {
            ListNode* temp = front->prev();

            backTop = middle->next();

            mergeSortImpl(front, middle, frontCount, offset, cmp);

            middle = temp;
            mergeTop = temp->next();

            temp = backTop->prev();

            mergeSortImpl(backTop, back, backCount, offset, cmp);

            backTop = temp->next();
        }

        while (frontCount > 0 || backCount > 0)
        {
            if (frontCount == 0)
            {
                takeFromFront = false;
            }
            else if (backCount == 0)
            {
                takeFromFront = true;
            }
            else
            {
                takeFromFront = cmp(PtrUtil::addOffset(mergeTop, -offset), PtrUtil::addOffset(backTop, -offset)) < 1;
            }

            ListNode* temp;
            if (takeFromFront)
            {
                temp = mergeTop->next();
                mergeTop->erase_();
                middle->insertBack_(mergeTop);
                middle = mergeTop;
                frontCount--;
                mergeTop = temp;
            }
            else
            {
                temp = backTop->next();
                backTop->erase_();
                middle->insertBack_(backTop);
                middle = backTop;
                backCount--;
                backTop = temp;
            }
        }
    }
    else if (num > 1)
    {
        ListNode* start = front->prev();
        ListNode* node = front->next();
        ListNode* end = back->next();

        while (node != end)
        {
            ListNode* next = node->mNext;
            ListNode* prev = node->mPrev;

            if (cmp(PtrUtil::addOffset(prev, -offset), PtrUtil::addOffset(node, -offset)) > 0)
            {
                do
                {
                    prev = prev->mPrev;
                    if (prev == start)
                    {
                        break;
                    }
                } while (cmp(PtrUtil::addOffset(prev, -offset), PtrUtil::addOffset(node, -offset)) > 0);

                node->erase_();
                prev->insertBack_(node);
            }

            node = next;
        }
    }
}

} // namespace sead
