#pragma once

#include <vector>

template<class _Ty,
class _Ax = allocator<_Ty> >
class dsp_vector
{	// varying size array of values
public:
    typedef dsp_vector<_Ty, _Ax> _Myt;
    typedef _Ax _Alloc;

    typedef _Alloc allocator_type;
    typedef typename _Alloc::size_type size_type;
    typedef typename _Alloc::difference_type difference_type;
    typedef typename _Alloc::pointer pointer;
    typedef typename _Alloc::const_pointer const_pointer;
    typedef typename _Alloc::reference reference;
    typedef typename _Alloc::const_reference const_reference;
    typedef typename _Alloc::value_type value_type;

    //////////////////////////////////////////////////////////////////////////
    
    typedef _Ty* iterator;
    typedef _Ty* const_iterator;

    dsp_vector()
    {	// construct empty vector
        this->_Myfirst = this->_Mylast = this->_Myend;
    }

    explicit dsp_vector(const _Alloc& _Al)
    {	// construct empty vector with allocator
        this->_Alval = _Al;
    }

    explicit dsp_vector(size_type _Count)
    {	// construct from _Count
        this->_Myfirst = this->_Alval.allocate(_Count);
        this->_Mylast  = this->_Myfirst + _Count - 1;
        this->_Myend   = this->_Myfirst + _Count;
    }

    ~dsp_vector()
    {	// destroy the object
        _Tidy();
    }


    size_type capacity() const
    {	// return current length of allocated storage
        return (this->_Myend - this->_Myfirst);
    }

    iterator begin()
    {	// return iterator for beginning of mutable sequence
        return this->_Myfirst;
    }

    const_iterator begin() const
    {	// return iterator for beginning of nonmutable sequence
        return (const_iterator(this->_Myfirst));
    }

    iterator end()
    {	// return iterator for end of mutable sequence
        return (iterator(this->_Myend));
    }

    const_iterator end() const
    {	// return iterator for end of nonmutable sequence
        return (const_iterator(this->_Myend));
    }

    iterator _Make_iter(const_iterator _Where) const
    {	// make iterator from const_iterator
        return (iterator(_Where._Ptr));
    }

#if _HAS_CPP0X
    const_iterator cbegin() const
    {	// return iterator for beginning of nonmutable sequence
        return (((const _Myt *)this)->begin());
    }

    const_iterator cend() const
    {	// return iterator for end of nonmutable sequence
        return (((const _Myt *)this)->end());
    }
#endif /* _HAS_CPP0X */

    size_type size() const
    {	// return length of sequence
        return (this->_Myend - this->_Myfirst);
    }

    //bool empty() const
    //{	// test if sequence is empty
    //    return (this->_Myfirst == this->_Mylast);
    //}

    const_reference operator[](size_type _Pos) const
    {	// subscript nonmutable sequence
        return (*(this->_Myfirst + _Pos));
    }

    reference operator[](size_type _Pos)
    {	// subscript mutable sequence
        return (*(this->_Myfirst + _Pos));
    }

#if _HAS_CPP0X
    pointer data()
    {	// return address of first element
        return (this->_Myfirst);
    }

    const_pointer data() const
    {	// return address of first element
        return (this->_Myfirst);
    }
#endif /* _HAS_CPP0X */

    reference front()
    {	// return first element of mutable sequence
        return (*begin());
    }

    const_reference front() const
    {	// return first element of nonmutable sequence
        return (*begin());
    }

    reference back()
    {	// return last element of mutable sequence
        return (*(end() - 1));
    }

    const_reference back() const
    {	// return last element of nonmutable sequence
        return (*(end() - 1));
    }

protected:

    void _Tidy()
    {	// free all storage
        if (this->_Myfirst != 0)
        {	// something to free, destroy and deallocate it
            this->_Alval.deallocate(this->_Myfirst,
                this->_Myend - this->_Myfirst);
        }
        this->_Myfirst = 0;
        this->_Mylast = 0;
        this->_Myend = 0;
    }
private:
    _Myt& operator=(_Myt&& _Right){};
    
    iterator _Myfirst;
    iterator _Mylast;
    iterator _Myend;
    allocator_type _Alval;
};
