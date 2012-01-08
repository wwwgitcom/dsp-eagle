#pragma once

template<typename elem_type>
struct v_iterator
{
    typedef v_iterator<elem_type> _Myt;

    _Myt(){}

    _Myt(const _Myt & it)
    {
        elem_ptr = (it.elem_ptr);
    }

    _Myt(elem_type* _elem_ptr)
        : elem_ptr(_elem_ptr)
    {
    }

    _Myt& operator=(const _Myt& it)
    {
        elem_ptr = it.elem_ptr;
        return (*this);
    }

    bool operator<(const _Myt& it)
    {
        return (elem_ptr < it.elem_ptr);
    }

    bool operator<=(const _Myt& it)
    {
        return (elem_ptr <= it.elem_ptr);
    }

    bool operator==(const _Myt& it)
    {
        return (elem_ptr < it.elem_ptr);
    }

    bool operator>(const _Myt& it)
    {
        return (elem_ptr > it.elem_ptr);
    }

    bool operator>=(const _Myt& it)
    {
        return (elem_ptr >= it.elem_ptr);
    }

    _Myt& operator=(elem_type * _elem_ptr)
    {
        elem_ptr = _elem_ptr;
        return (*this);
    }

    _Myt& operator++()
    { // preinc
        ++elem_ptr;
        return (*this);
    }

    _Myt operator++(int)
    { // postinc
        _Myt _temp = *this;
        ++elem_ptr;
        return _temp;
    }

    _Myt& operator+(int n)
    {
        elem_ptr += n;
        return (*this);
    }

    elem_type& operator*()
    {
        return *elem_ptr;
    }

    elem_type* operator->()
    {
        return elem_ptr;
    }

    elem_type* elem_ptr;

};