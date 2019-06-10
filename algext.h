#ifndef ALGEXT_H_INCLUDED
#define ALGEXT_H_INCLUDED

#include <algorithm>
#include <iterator>
#include <sstream>

namespace std
{

template< class V, class T >
typename V::const_iterator find(const V& v, const T& t)
{
    return find(begin(v), end(v), t);
}

template< class V, class T >
auto find(V&& v, T&& t)
{
    return find(begin(v), end(v), t);
}

template< class V, class Pred >
auto find_if(const V& v, Pred p)
{
    return find_if(cbegin(v), cend(v), p);
}

template< class V, class Pred >
auto find_if(V&& v, Pred p)
{
    return find_if(begin(v), end(v), p);
}

template< class V >
void sort(V& v)
{
    sort(begin(v), end(v));
}

template< class V, class Cmp >
void sort(V&& v, Cmp cmp)
{
    sort(begin(v), end(v), cmp);
}

template< class V >
void reverse(V& v)
{
    reverse(begin(v), end(v));
}

template< class V, class Pred >
auto min_element(const V& v, Pred p)
{
    return min_element(begin(v), end(v), p);
}

template< class V, class Cmp >
auto max_element(V&& v, Cmp cmp)
{
    return max_element(begin(v), end(v), cmp);
}

template< class V, class T >
void replace(V& v, const T& val, T new_val)
{
    replace(begin(v), end(v), val, new_val);
}

template< class V, class It >
void copy(V&& v, It dest)
{
    copy(begin(v), end(v), dest);
}

template< class V, class It >
void move(V&& v, It dest)
{
    move(begin(v), end(v), dest);
}

template< class V >
auto unique(V&& v)
{
    return unique(begin(v), end(v));
}

}

#endif // ALGEXT_H_INCLUDED
