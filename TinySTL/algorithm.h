﻿#pragma once

#include <cassert>          // for assert
#include <cmath>            // for std::ceil
#include <initializer_list> // for std::initializer_list
#include <random>

#include "functional.h"
#include "iterator.h"

namespace tiny_stl 
{

template <typename InIter, typename UnaryPred>
inline bool all_of(InIter first, InIter last, UnaryPred pred)
{
    for (; first != last; ++first)
        if (!pred(*first))
            return false;

    return true;
}

template <typename InIter, typename UnaryPred>
inline bool any_of(InIter first, InIter last, UnaryPred pred)
{
    for (; first != last; ++first)
        if (pred(*first))
            return true;

    return false;
}

template <typename InIter, typename UnaryPred>
inline bool none_of(InIter first, InIter last, UnaryPred pred)
{
    for (; first != last; ++first)
        if (pred(*first))
            return false;

    return true;
}

template <typename InIter, typename UnaryFunc>
inline UnaryFunc for_each(InIter first, InIter last, UnaryFunc f)
{
    for (; first != last; ++first)
        f(*first);

    return tiny_stl::move(f);
}

template <typename InIter, typename UnaryPred>
inline typename iterator_traits<InIter>::difference_type
count_if(InIter first, InIter last, UnaryPred pred)
{
    typename iterator_traits<InIter>::difference_type c = 0;
    for (; first != last; ++first)
        if (pred(*first))
            ++c;

    return c;
}

template <typename InIter, typename T>
inline typename iterator_traits<InIter>::difference_type
count(InIter first, InIter last, const T& val)
{
    return count_if(first, last, [&val](const auto& v) { return v == val; });
}

template <typename InIter1, typename InIter2, typename BinPred>
inline pair<InIter1, InIter2> 
mismatch(InIter1 first1, InIter1 last1, InIter2 first2, BinPred pred)
{
    while (first1 != last1 && pred(*first1, *first2))
    {
        ++first1; 
        ++first2;
    }

    return make_pair(first1, first2);
}

template <typename InIter1, typename InIter2>
inline pair<InIter1, InIter2> 
mismatch(InIter1 first1, InIter1 last1, InIter2 first2)
{
    while (first1 != last1 && *first1 != *first2)
    {
        ++first1;
        ++first2;
    }

    return make_pair(first1, first2);
}

template <typename InIter1, typename InIter2, typename BinPred>
inline pair<InIter1, InIter2>
mismatch(InIter1 first1, InIter1 last1, InIter2 first2, InIter2 last2, BinPred pred)
{
    while (first1 != last1 && first2 != last2 && pred(*first1, *first2))
    {
        ++first1;
        ++first2;
    }

    return make_pair(first1, first2);
}

template <typename InIter1, typename InIter2>
inline pair<InIter1, InIter2>
mismatch(InIter1 first1, InIter1 last1, InIter2 first2, InIter2 last2)
{
    while (first1 != last1 && first2 != last2 && *first1 != *first2)
    {
        ++first1;
        ++first2;
    }

    return make_pair(first1, first2);
}

template <typename InIter, typename T>
inline InIter find(InIter first, InIter last, const T& val)
{
    for (; first != last; ++first)
        if (*first == val)
            return first;

    return last;
}

template <typename InIter, typename UnaryPred>
inline InIter find_if(InIter first, InIter last, UnaryPred pred)
{
    for (; first != last; ++first)
        if (pred(*first))
            return first;

    return last;
}

template <typename InIter, typename UnaryPred>
inline InIter find_if_not(InIter first, InIter last, UnaryPred pred)
{
    for (; first != last; ++first)
        if (!pred(*first))
            return first;

    return last;
}


template <typename FwdIter, typename T>
struct _Fill_memset_is_safe_helper 
{
    using _Value_type = typename iterator_traits<FwdIter>::value_type;
    using type = typename conjunction<
        is_pointer<FwdIter>,
        disjunction<
            conjunction<
                _Is_character<T>,
                _Is_character<_Value_type>>,
            conjunction<
                is_same<bool, T>,
                is_same<bool, _Value_type>>
        >>::type;
};

template <typename FwdIter, typename T>
inline typename _Fill_memset_is_safe_helper<FwdIter, T>::type
_Fill_memset_is_safe(const FwdIter&, const T&) 
{
    return {};
}

template <typename OutIter, typename Diff, typename T>
inline OutIter _Fill_n(OutIter dest, Diff n, const T& val, 
                        true_type /* fill memset is safe*/) 
{
    if (n > 0) 
    {
        _CSTD memset(dest, val, n);
        return dest + n;
    }
    return dest;
}

template <typename OutIter, typename Diff, typename T>
inline OutIter _Fill_n(OutIter dest, Diff n, const T& val, 
                        false_type) 
{
    for (; n > 0; --n, ++dest)
        *dest = val;
    return dest;
}

template <typename OutIter, typename Diff, typename T>
inline OutIter fill_n(OutIter dest, Diff n, const T& val) 
{
    return _Fill_n(dest, n, val, _Fill_memset_is_safe(dest, val));
}

template <typename FwdIter, typename T>
inline void _Fill(FwdIter first, FwdIter last, const T& val, true_type) 
{
    _CSTD memset(first, last - first, val);
}

template <typename FwdIter, typename T>
inline void _Fill(FwdIter first, FwdIter last, const T& val, false_type) 
{
    for (; first != last; ++first)
        *first = val;
}

template <typename FwdIter, typename T>
inline void fill(FwdIter first, FwdIter last, const T& val) 
{
    _Fill(first, last, val, _Fill_memset_is_safe(first, val));
}


template <typename FwdIter, typename Func>
void generate(FwdIter first, FwdIter last, Func f)
{
    for (; first != last; ++first)
    {
        *first = f();
    }
}

template <typename OutIter, typename Size, typename Func>
OutIter generate_n(OutIter first, Size n, Func f)
{
    for (Size i = 0; i < n; ++i)
    {
        *first++ = f();
    }

    return first;
}

template <typename InIter, typename OutIter, typename UnaryOp>
OutIter transform(InIter first1, InIter last1, OutIter dst_first, UnaryOp op)
{
    for (; first1 != last1; ++first1, ++dst_first)
        *dst_first = op(*first1);

    return dst_first;
}

template <typename InIter, typename OutIter, typename BinOp>
OutIter transform(InIter first1, InIter last1, 
                  InIter first2, OutIter dst_first, BinOp op)
{
    for (; first1 != last1; ++first1, ++first2, ++dst_first)
        *dst_first = op(*first1, *first2);

    return dst_first;
}

template <typename InIter, typename OutIter, typename UnaryPred>
inline OutIter copy_if(InIter first, InIter last, OutIter dest, UnaryPred pred) 
{
    for (; first != last; ++first) 
        if (pred(*first))
            *dest++ = *first;

    return dest;        // new last
}

template <typename InIter, typename OutIter>
inline OutIter copy(InIter first, InIter last, OutIter dest) 
{
    for (; first != last; ++first) 
        *dest++ = *first;

    return dest;
}

template <typename InIter, typename Size, typename OutIter>
OutIter copy_n(InIter src, Size count, OutIter dst)
{
    if (count > 0)
    {
        for (Size i = 0; i < count; ++i)
            *dst = *src;
    }

    return dst;
}

template <typename BidIter1, typename BidIter2>
inline BidIter2 copy_backward(BidIter1 first, BidIter1 last, BidIter2 dest_last) 
{
    for (; first != last; ) 
        *(--dest_last) = *(--last);

    return dest_last;
}

template <typename InIter, typename OutIter>
inline OutIter move(InIter first, InIter last, OutIter dest_first) 
{
    for (; first != last; ) 
        *(dest_first++) = tiny_stl::move(*first++);

    return dest_first;
}


template <typename BidIter1, typename BidIter2>
inline BidIter2 move_backward(BidIter1 first, BidIter1 last, BidIter2 dest_last) 
{
    for (; first != last; ) 
        *(--dest_last) = tiny_stl::move(*(--last));

    return dest_last;
}

template <typename FwdIter1, typename FwdIter2>
inline FwdIter2 swap_ranges(FwdIter1 first1, FwdIter1 last1, FwdIter2 first2) 
{
    for (; first1 != last1; ++first1, ++first2)
        tiny_stl::iter_swap(first1, first2);
    return first2;
}

template <typename InIter1, typename InIter2, typename BinPred>
inline bool equal(InIter1 first1, InIter1 last1,
                 InIter2 first2, BinPred pred) 
{
    for (; first1 != last1; ++first1, ++first2) 
        if (!pred(*first1, *first2)) 
            return false;
        
    return true;
}

template <typename InIter1, typename InIter2>
inline bool equal(InIter1 first1, InIter1 last1, InIter2 first2) 
{
    return equal(first1, last1, first2, tiny_stl::equal_to<>());
}

template <typename InIter1, typename InIter2, typename BinPred>
inline bool equal(InIter1 first1, InIter1 last1, 
                InIter2 first2, InIter2 last2, BinPred pred) 
{
    // for input iterator
    for (; first1 != last1 && first2 != last2; ++first1, ++first2)
        if (!pred(*first1, *first2))
            return false;

    return first1 == last1 && first2 == last2;
}

template <typename InIter1, typename InIter2>
inline bool equal(InIter1 first1, InIter1 last1,
                InIter2 first2, InIter2 last2) 
{
    return equal(first1, last1, first2, last2, tiny_stl::equal_to<>{});
}

template <typename FwdIter, typename Cmp>
constexpr FwdIter min_element(FwdIter first, FwdIter last, Cmp cmp) 
{
    if (first == last)
        return last;
    FwdIter max_iter = first++;

    for (; first != last; ++first) 
        if (cmp(*first, *max_iter))
            max_iter = first;

    return max_iter;
}

template <typename FwdIter>
constexpr FwdIter min_element(FwdIter first, FwdIter last) 
{
    return min_element(first, last, tiny_stl::less<>{});
}

template <typename T, typename Cmp>
constexpr const T& min(const T&a, const T& b, Cmp cmp) 
{
    return cmp(a, b) ? a : b;
}

template <typename T>
constexpr const T& min(const T& a, const T& b) 
{
    return a < b ? a : b;
}

template <typename T, typename Cmp>
constexpr T min(std::initializer_list<T> ilist, Cmp cmp) 
{
    return *min_element(ilist.begin(), ilist.end(), cmp);
}

template <typename T>
constexpr T min(std::initializer_list<T> ilist) 
{
    return tiny_stl::min(ilist, tiny_stl::less<>{});
}


template <typename FwdIter, typename Cmp>
constexpr FwdIter max_element(FwdIter first, FwdIter last, Cmp cmp) 
{
    if (first == last)
        return last;
    FwdIter max_iter = first++;

    for (; first != last; ++first) 
        if (cmp(*max_iter, *first))
            max_iter = first;

    return max_iter;
}

template <typename FwdIter>
constexpr FwdIter max_element(FwdIter first, FwdIter last) 
{
    return max_element(first, last, tiny_stl::less<>{});
}

template <typename T, typename Cmp>
constexpr const T& max(const T&a, const T& b, Cmp cmp) 
{
    return cmp(a, b) ? b : a;
}

template <typename T>
constexpr const T& max(const T& a, const T& b) 
{
    return a < b ? b : a;
}

template <typename T, typename Cmp>
constexpr T max(std::initializer_list<T> ilist, Cmp cmp) 
{
    return *max_element(ilist.begin(), ilist.end(), cmp);
}

template <typename T>
constexpr T max(std::initializer_list<T> ilist) 
{
    return tiny_stl::max(ilist, tiny_stl::less<>{});
}

template <typename T, typename Compare>
inline pair<const T&, const T&> minmax(const T& a, const T& b, Compare cmp)
{
    return cmp(a, b) ? pair<const T&, const T&>(a, b)
                     : pair<const T&, const T&>(b, a);
}

template <typename T>
inline pair<const T&, const T&> minmax(const T& a, const T& b)
{
    return minmax(a, b, tiny_stl::less<>{});
}

template <typename FwdIter, typename Compare>
inline pair<FwdIter, FwdIter> minmax_element(FwdIter first, FwdIter last, Compare cmp)
{
    // return { The smallest first element, The largest last element }
    pair<FwdIter, FwdIter> ret{ first, first };

    if (first == last)                      // empty range
        return ret;

    if (++first == last)                    // there is only one element
        return ret;

    if (cmp(*first, *ret.first))            // compare *(first+1) with result
        ret.first = first;
    else
        ret.second = first;

    while (++first != last)                 // compare [first+2, last) with result
    {
        FwdIter iter = first;
        if (++first == last)                // the last
        {
            if (cmp(*iter, *ret.first))
                ret.first = iter;
            else if (!(cmp(*iter, *ret.second)))
                ret.second = iter;
            break;
        }
        else                                // not the last
        {
            if (cmp(*first, *iter))         // *first is smaller if cmp is less
            {
                if (cmp(*first, *ret.first))
                    ret.first = first;
                if (!cmp(*iter, *ret.second))
                    ret.second = iter;
            }
            else                            // *iter is smaller if cmp is less
            {
                if (cmp(*iter, *ret.first))
                    ret.first = iter;
                if (!cmp(*first, *ret.second))
                    ret.second = first;
            }
        }
    }

    return ret;
}

template <typename FwdIter>
inline pair<FwdIter, FwdIter> minmax_element(FwdIter first, FwdIter last)
{
    return minmax_element(first, last, tiny_stl::less<>{});
}


template <typename T>
inline pair<T, T> minmax(std::initializer_list<T> ilist)
{
    auto p = minmax_element(ilist.begin(), ilist.end(), tiny_stl::less<>{});
    return make_pair(*p.first, *p.second);
}

template <typename T, typename Compare>
inline pair<T, T> minmax(std::initializer_list<T> ilist, Compare cmp)
{
    auto p = minmax_element(ilist.begin(), ilist.end(), cmp);
    return make_pair(*p.first, *p.second);
}


template <typename InIter1, typename InIter2, typename BinPred>
inline bool _Lexicographical_compare(InIter1 first1, InIter1 last1,
                                    InIter2 first2, InIter2 last2, 
                                    BinPred pred) 
{
    for (; first1 != last1 && first2 != last2; ++first1, ++first2) 
    {
        if (pred(*first1, *first2))
            return true;
        if (pred(*first2, *first1))
            return false;
    }

    // If one range is a prefix of another, the shorter 
    // range is lexicographically less than the other.
    return (first1 == last1 && first2 != last2);
}



template <typename InIter1, typename InIter2, typename BinPred>
inline bool lexicographical_compare(InIter1 first1, InIter1 last1,
                    InIter2 first2, InIter2 last2, BinPred pred) 
{
    return _Lexicographical_compare(first1, last1, first2, last2, pred);
}


template <typename InIter1, typename InIter2>
inline bool lexicographical_compare(InIter1 first1, InIter1 last1,
                    InIter2 first2, InIter2 last2) 
{
    return tiny_stl::lexicographical_compare(
        first1, last1, first2, last2, less<>());
}


template <typename FwdIter>
inline FwdIter rotate(FwdIter first, FwdIter mid, FwdIter last) 
{
    // return first + (last - mid)
    if (first == mid) return last;
    if (mid == last)  return first;

    FwdIter next = mid;
    do  // left
    {  
        iter_swap(first++, next++);
        if (first == mid) mid = next;
    } while (next != last);

    FwdIter ret = first;    // this_first = old_first + (last - mid)

    for (next = mid; next != last;) // right
    {  
        iter_swap(first++, next++);
        if (first == mid) mid = next;
        else if (next == last) next = mid;
    }

    return ret;
}

template <typename BidIter>
inline void reverse(BidIter first, BidIter last) 
{
    for (; first != last && first != --last; ++first) 
        tiny_stl::iter_swap(first, last);
}


template <typename RanIter, typename Diff, typename T, typename Cmp>
inline void _Push_heap(RanIter first, Diff hole, Diff top, 
                        T&& val, Cmp& cmp) 
{
    Diff i = (hole - 1) / 2;
    for (; top < hole && cmp(*(first + i), val); i = (hole - 1) / 2) 
    {      
        // parent < val
        *(first + hole) = tiny_stl::move(*(first + i)); // move down parent 
        hole = i;                                       // update the hole
    }

    *(first + hole) = tiny_stl::move(val);              // move val to the hole
}

template <typename RanIter, typename Cmp>
inline void push_heap(RanIter first, RanIter last, Cmp cmp) 
{
    using Diff = typename iterator_traits<RanIter>::difference_type;
    Diff count = last - first;
    if (count >= 2) 
    {
        auto val = tiny_stl::move(*--last);
        _Push_heap(first, --count, static_cast<Diff>(0), 
                tiny_stl::move(val), cmp);
    }
}

template <typename RanIter> 
inline void push_heap(RanIter first, RanIter last) // O(logn)
{    
    tiny_stl::push_heap(first, last, tiny_stl::less<>{});
}

template <typename RanIter, typename Diff, typename T, typename Cmp>
inline void _Adjust_heap(RanIter first, Diff hole, Diff len, T&& val, Cmp& cmp) 
{
    Diff top = hole;    // 0
    Diff rightChild = hole * 2 + 2; // init right child to 2

    while (rightChild < len) // have a right child 
    {   
        if (*(first + rightChild) < *(first + (rightChild - 1))) // left child > right child
            --rightChild;
        *(first + hole) = tiny_stl::move(*(first + rightChild)); // move the bigger one to the hole
        hole = rightChild;                  // update the hole
        rightChild = rightChild * 2 + 2;    // update the right child
    }
    
    if (rightChild == len)  // only left child
    {   
         // move the left child to the hole
        *(first + hole) = tiny_stl::move(*(first + (rightChild - 1)));  

        hole = rightChild - 1;
    }

    // val is the origin last element. move it to hole
    _Push_heap(first, hole, top, tiny_stl::move(val), cmp);
}

template <typename RanIter, typename T, typename Cmp>
inline void _Pop_heap(RanIter first, RanIter last, RanIter dest, T&& val, Cmp& cmp) 
{
    using Diff = typename iterator_traits<RanIter>::difference_type;
    *dest = tiny_stl::move(*first);     // move the first to tail, hole is 0
    
    _Adjust_heap(first, static_cast<Diff>(0), static_cast<Diff>(last - first),
        tiny_stl::move(val), cmp);
}

template <typename RanIter, typename Cmp>
inline void pop_heap(RanIter first, RanIter last, Cmp cmp) 
{
    --last;
    auto val = tiny_stl::move(*last);
    _Pop_heap(first, last, last, tiny_stl::move(val), cmp);
}

template <typename RanIter>                             // O(logn)
inline void pop_heap(RanIter first, RanIter last) 
{
    pop_heap(first, last, tiny_stl::less<>{});
}

template <typename RanIter, typename Cmp> 
inline void sort_heap(RanIter first, RanIter last, Cmp cmp) 
{
    RanIter origin = last;
    for (; last - first > 1; --last) 
        pop_heap(first, last);
}

template <typename RanIter>
inline void sort_heap(RanIter first, RanIter last) 
{
    sort_heap(first, last, tiny_stl::less<>{});
}

template <typename RanIter, typename Cmp> 
inline void make_heap(RanIter first, RanIter last, Cmp cmp) 
{
    if (last - first < 2)
        return;

    using Diff = typename iterator_traits<RanIter>::difference_type;
    Diff len = last - first;
    Diff parent = (len - 2) / 2;

    while (true) 
    {
        auto val = tiny_stl::move(*(first + parent));
        _Adjust_heap(first, parent, len, val, cmp);
        if (parent-- == 0)
            return;
    }
}

template <typename RanIter>
inline void make_heap(RanIter first, RanIter last) 
{
    make_heap(first, last, tiny_stl::less<>{});
}

template <typename RanIter, typename Cmp>
inline RanIter is_heap_until(RanIter first, RanIter last, Cmp cmp) 
{
    using Diff = typename iterator_traits<RanIter>::difference_type;
    
    Diff len = last - first;
    if (len >= 2) 
        for (Diff offset = 1; offset < len; ++offset) 
            if (cmp(*(first + (offset - 1) / 2), *(first + offset))) // parent < child
                return first + offset;
           
    return last;
}

template <typename RanIter, typename Cmp>
inline RanIter is_heap_until(RanIter first, RanIter last) 
{
    return is_heap_until(first, last, tiny_stl::less<>{});
}

template <typename RanIter, typename Cmp>
inline bool is_heap(RanIter first, RanIter last, Cmp cmp) 
{
    return is_heap_until(first, last, cmp) == last;
}

template <typename RanIter>
inline bool is_heap(RanIter first, RanIter last) 
{
    return is_heap(first, last, tiny_stl::less<>{});
}

template <typename FwdIter, typename Cmp>
inline FwdIter is_sorted_until(FwdIter first, FwdIter last, Cmp cmp) 
{
    if (first != last) 
        for (FwdIter next = first; ++next != last; ++first) 
            if (cmp(*next, *first))     // *next < *cur
                return next;
   
    return last;
}

template <typename FwdIter>
inline FwdIter is_sorted_until(FwdIter first, FwdIter last) 
{
    return tiny_stl::is_sorted_until(first, last, tiny_stl::less<>{});
}

template <typename FwdIter, typename Cmp>
inline bool is_sorted(FwdIter first, FwdIter last, Cmp cmp) 
{
    return tiny_stl::is_sorted_until(first, last, cmp) == last;
}

template <typename FwdIter>
inline bool is_sorted(FwdIter first, FwdIter last) 
{
    return tiny_stl::is_sorted_until(first, last) == last;
}

namespace
{

// insert_sort
template <typename RanIter, typename Compare>
void _Insert_sort(RanIter first, RanIter last, Compare& cmp)
{
    if (first == last) return;

    for (RanIter i = first + 1; i != last; ++i)
    {
        auto key = tiny_stl::move(*i);
        RanIter j = i;
        for (; j > first && cmp(key, (*(j - 1))); --j)
        {
            *j = tiny_stl::move(*(j - 1));
        }
            
        *j = tiny_stl::move(key);
    }
}


// quick_sort
template <typename RanIter>
inline auto _Get_random(RanIter first, RanIter last)
{
    using Diff = typename iterator_traits<RanIter>::difference_type;
    static std::mt19937 e;
    std::uniform_int_distribution<Diff> u{ 0, (last - first - 1) };

    return u(e);
}

template <typename RanIter, typename Compare>
inline RanIter _Partition(RanIter first, RanIter last, Compare& cmp)
{
    using Diff = typename iterator_traits<RanIter>::difference_type;
    Diff randomPos = _Get_random(first, last);
    iter_swap(first + randomPos, last - 1);
    auto key = *(last - 1);

    RanIter i = first;
    for (RanIter j = first; j < last - 1; ++j)
    {
        if (cmp(*j, key))
        {
            iter_swap(i++, j);
        }
    }
    iter_swap(i, last - 1);

    return i;
}

const std::ptrdiff_t _INSERT_SORT_MAX = 32;

template <typename RanIter, typename Compare>
inline void _Quick_sort(RanIter first, RanIter last, _Iter_diff_t<RanIter> diff, Compare& cmp)
{
    _Iter_diff_t<RanIter> count = last - first;
    if (count > _INSERT_SORT_MAX && diff > 0)
    {
        RanIter miditer = _Partition(first, last, cmp);

        diff = (diff >> 1) + (diff >> 2);

        _Quick_sort(first, miditer, diff, cmp);
        first = miditer;

        _Quick_sort(miditer + 1, last, diff, cmp);
        last = miditer + 1;
    }

    if (count > _INSERT_SORT_MAX)
    {
        make_heap(first, last, cmp);
        sort_heap(first, last, cmp);
    }

    else if (count >= 2)
    {
        _Insert_sort(first, last, cmp);
    }
}

} // unnamed namespace

template <typename RanIter, typename Compare>
inline void sort(RanIter first, RanIter last, Compare cmp)
{
    if (last - first - 1 > 0)
    {
        _Quick_sort(first, last, last - first, cmp);
    }
}

template <typename RanIter>
inline void sort(RanIter first, RanIter last)
{
    sort(first, last, tiny_stl::less<>{});
}


template <typename FwdIter, typename T, typename Compare>
inline FwdIter lower_bound(FwdIter first,           // >=
    FwdIter last, const T& val, Compare cmp)
{
    assert(is_sorted(first, last, cmp));

    FwdIter ret = first;
    using Diff = typename iterator_traits<FwdIter>::difference_type;
    Diff size = distance(first, last);
    Diff step = 0;

    while (size > 0)
    {
        ret = first;
        step = size >> 1;
        advance(ret, step);
        if (!cmp(*ret, val))    // left
        {
            size = step;
        }
        else                    // right
        {
            first = ++ret;
            size -= (step + 1);
        }
    }

    return ret;
}

template <typename FwdIter, typename T>
inline FwdIter lower_bound(FwdIter first, FwdIter last, const T& val)
{
    return lower_bound(first, last, val, tiny_stl::less<>{});
}

template <typename FwdIter, typename T, typename Compare>
inline FwdIter upper_bound(FwdIter first,
    FwdIter last, const T& val, Compare cmp)
{
    assert(is_sorted(first, last, cmp));

    FwdIter ret = first;
    using Diff = typename iterator_traits<FwdIter>::difference_type;
    Diff size = distance(first, last);
    Diff step = 0;

    while (size > 0)
    {
        ret = first;
        step = size >> 1;
        advance(ret, step);
        if (cmp(val, *ret))     // left
        {
            size = step;
        }
        else                    // right
        {
            first = ++ret;
            size -= (step + 1);
        }
    }

    return ret;
}

template <typename FwdIter, typename T>
inline FwdIter upper_bound(FwdIter first, FwdIter last, const T& val)
{
    return upper_bound(first, last, val, tiny_stl::less<>{});
}

template <typename FwdIter, typename T, typename Compare>
inline FwdIter binary_search(FwdIter first, 
    FwdIter last, const T& val, Compare cmp)
{
    first = lower_bound(first, last, val, cmp);
    return (!(first == last)) && !(cmp(val, *first));
}

template <typename FwdIter, typename T>
inline FwdIter binary_search(FwdIter first, FwdIter last, const T& val)
{
    first = lower_bound(first, last, val);
    return (!(first == last)) && !(val < *first);
}

template <typename FwdIter, typename T, typename Compare>
inline pair<FwdIter, FwdIter> 
equal_range(FwdIter first, FwdIter last, const T& val, Compare cmp)
{
    return make_pair(lower_bound(first, last, val, cmp),
                     upper_bound(first, last, val, cmp));
}

template <typename FwdIter, typename T>
inline pair<FwdIter, FwdIter> 
equal_range(FwdIter first, FwdIter last, const T& val)
{
    return make_pair(lower_bound(first, last, val),
                     upper_bound(first, last, val));
}

}   // namespace tiny_stl 