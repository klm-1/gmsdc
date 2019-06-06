#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstring>
#include <vector>
#include <string>
#include <set>
#include <map>

#include "algext.h"

#define CASE_RETURN(_it)                 case (_it): return #_it;
#define CASE_RETURN_SCOPED(_scope, _it)  case (_scope :: _it): return #_it;
#define ASSERT(_stat)                    if (!(_stat)) throw std::runtime_error("Assertion failed: " #_stat);
#define STR_SWITCH(_str)                 const char* __switch = _str;
#define STR_CASE(_str)                   if (!strcmp(__switch, _str))
#define STR_CASE_RETURN(_str, _ret)      STR_CASE(_str) { return _ret; }

#define HEADER_SIZE 8
#define REFDEF_SIZE 12


typedef unsigned char byte;


void string_replace_char(std::string& s, char c, const std::string& rep);

template<class V>
auto vector_pop(V& v)
{
    auto ret = v.back();
    v.pop_back();
    return ret;
}

template<class T>
std::string to_string(const T& t)
{
    std::ostringstream tmp;
    tmp << std::setprecision(16) << t;
    return tmp.str();
}

template<class T>
void replace_insert(std::vector<T>& v, const T& val, const std::vector<T>& r)
{
    for (size_t i = v.size() - 1; i--;) 
	{
        if (v[i] == val) 
		{
            size_t vsize = v.size();
            v.resize(vsize + r.size() - 1);
            std::copy_backward(v.begin() + i + 1, v.begin() + vsize, v.end());
            std::copy(r.begin(), r.end(), v.begin() + i);
        }
    }
}

template<class T>
void vector_find_and_erase(std::vector<T>& v, const T& val)
{
    auto it = std::find(v, val);
    if (it != v.end()) { v.erase(it); }
}

template<class T>
void vector_deduplicate(std::vector<T>& v)
{
    std::set<T> uni;
    for (size_t i = 0; i < v.size();)
	{
        if (!uni.insert(v[i]).second) { v.erase(v.begin() + i); } 
		else { ++i; }
    }
}

template< class T >
T pop_back(std::vector<T>& v)
{
    if (v.empty()) 
	{
        throw std::runtime_error("Stack underflow");
    }
    T tmp = std::move(v.back());
    v.pop_back();
    return std::move(tmp);
}

template<class K, class V>
V lookup(const std::map<K, V>& m, const K& k)
{
    auto it = m.find(k);
    if (it == m.end()) 
	{
        throw std::runtime_error("Key not found");
    }
    return it->second;
}

template<class S>
void string_lower(S& s)
{
    std::transform(std::begin(s), std::end(s), std::begin(s), ::tolower);
}

template<class V>
class reversed_ 
{
public:
    reversed_(V& v): v_(v) {}

    auto begin()         { return std::rbegin(v_); }
	auto end()           { return std::rend(v_); }
	const auto cbegin()  { return std::crbegin(v_); }
	const auto cend()    { return std::crend(v_); }

private:
    V& v_;
};

template<class V>
reversed_<V> reversed(V& v)
{
    return reversed_<V>(v);
}

#endif // TYPES_H_INCLUDED
