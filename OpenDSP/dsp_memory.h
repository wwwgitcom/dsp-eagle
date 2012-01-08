// xmemory internal header (from <memory>)
#pragma once
#include <xmemory>
// TEMPLATE CLASS _ALLOCATOR
template<class _Ty>
class dsp_allocator
    : public _Allocator_base<_Ty>
{	// generic allocator for objects of class _Ty
public:
    typedef _Allocator_base<_Ty> _Mybase;
    typedef typename _Mybase::value_type value_type;

    typedef value_type _FARQ *pointer;
    typedef value_type _FARQ& reference;
    typedef const value_type _FARQ *const_pointer;
    typedef const value_type _FARQ& const_reference;

    typedef _SIZT size_type;
    typedef _PDFT difference_type;

    template<class _Other>
    struct rebind
    {	// convert this type to _ALLOCATOR<_Other>
        typedef dsp_allocator<_Other> other;
    };

    pointer address(reference _Val) const
    {	// return address of mutable _Val
        return ((pointer) &(char&)_Val);
    }

    const_pointer address(const_reference _Val) const
    {	// return address of nonmutable _Val
        return ((const_pointer) &(char&)_Val);
    }

    dsp_allocator() _THROW0()
    {	// construct default allocator (do nothing)
    }

    dsp_allocator(const dsp_allocator<_Ty>&) _THROW0()
    {	// construct by copying (do nothing)
    }

    template<class _Other>
    dsp_allocator(const dsp_allocator<_Other>&) _THROW0()
    {	// construct from a related allocator (do nothing)
    }

    template<class _Other>
    dsp_allocator<_Ty>& operator=(const dsp_allocator<_Other>&)
    {	// assign from a related allocator (do nothing)
        return (*this);
    }

    void deallocate(pointer _Ptr, size_type)
    {	// deallocate object at _Ptr, ignore size
        _aligned_free(_Ptr);
        //::operator delete(_Ptr);
    }

    pointer allocate(size_type _Count)
    {	// allocate array of _Count elements
        return (_Ty *)_aligned_malloc(_Count * sizeof(_Ty), 64);
        //return (_Allocate(_Count, (pointer)0));
    }

    pointer allocate(size_type _Count, const void _FARQ *)
    {	// allocate array of _Count elements, ignore hint
        return (allocate(_Count));
    }

    void construct(pointer _Ptr, const _Ty& _Val)
    {	// construct object at _Ptr with value _Val
        _Construct(_Ptr, _Val);
    }

    void construct(pointer _Ptr, _Ty&& _Val)
    {	// construct object at _Ptr with value _Val
        ::new ((void _FARQ *)_Ptr) _Ty(_STD forward<_Ty>(_Val));
    }

    template<class _Other>
    void construct(pointer _Ptr, _Other&& _Val)
    {	// construct object at _Ptr with value _Val
        ::new ((void _FARQ *)_Ptr) _Ty(_STD forward<_Other>(_Val));
    }

    void destroy(pointer _Ptr)
    {	// destroy object at _Ptr
        _Destroy(_Ptr);
    }

    _SIZT max_size() const _THROW0()
    {	// estimate maximum array size
        _SIZT _Count = (_SIZT)(-1) / sizeof (_Ty);
        return (0 < _Count ? _Count : 1);
    }
};

// CLASS _ALLOCATOR<void>
template<> class dsp_allocator<void>
{	// generic _ALLOCATOR for type void
public:
    typedef void _Ty;
    typedef _Ty _FARQ *pointer;
    typedef const _Ty _FARQ *const_pointer;
    typedef _Ty value_type;

    template<class _Other>
    struct rebind
    {	// convert this type to an _ALLOCATOR<_Other>
        typedef dsp_allocator<_Other> other;
    };

    dsp_allocator() _THROW0()
    {	// construct default allocator (do nothing)
    }

    dsp_allocator(const dsp_allocator<_Ty>&) _THROW0()
    {	// construct by copying (do nothing)
    }

    template<class _Other>
    dsp_allocator(const dsp_allocator<_Other>&) _THROW0()
    {	// construct from related allocator (do nothing)
    }

    template<class _Other>
    dsp_allocator<_Ty>& operator=(const dsp_allocator<_Other>&)
    {	// assign from a related allocator (do nothing)
        return (*this);
    }
};

//////////////////////////////////////////////////////////////////////////