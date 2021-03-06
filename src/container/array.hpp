/*
 * Copyright Institute for Theoretical Physics, ETH Zurich 2015.
 * Distributed under the Boost Software License, Version 1.0.
 *
 * Permission is hereby granted, free of charge, to any person or organization
 * obtaining a copy of the software and accompanying documentation covered by
 * this license (the "Software") to use, reproduce, display, distribute,
 * execute, and transmit the Software, and to prepare derivative works of the
 * Software, and to permit third-parties to whom the Software is furnished to
 * do so, all subject to the following:
 *
 * The copyright notices in the Software and this entire statement, including
 * the above license grant, this restriction and the following disclaimer,
 * must be included in all copies of the Software, in whole or in part, and
 * all derivative works of the Software, unless such copies or derivative
 * works are solely in the form of machine-executable object code generated by
 * a source language processor.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
 * SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
 * FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef BIND_CONTAINER_ARRAY
#define BIND_CONTAINER_ARRAY

namespace bind {

    // {{{ array helper functions
    template<class T, class Allocator> class array;
    namespace detail {
        template<class T, class Allocator>
        void fill_array(volatile bind::array<T,Allocator>& a, T& value){
            for(size_t i = 0; i < a.size(); ++i) a[i] = value;
        }
        template<class T, class Allocator, class OtherAllocator = Allocator>
        void copy_array(volatile bind::array<T,Allocator>& dst, const bind::array<T,OtherAllocator>& src, const size_t& n){
            for(size_t i = 0; i < n; ++i) dst[i] = src[i];
        }
    }
    // }}}

    template <class T, class Allocator = bind::snapshot>
    class array {
    public:
        void* operator new (size_t size, void* ptr){ return ptr; }
        void  operator delete (void*, void*){ /* doesn't throw */ }
        void* operator new (size_t sz){ return memory::cpu::standard::malloc<sizeof(array)>(); }
        void operator delete (void* ptr){ memory::cpu::standard::free(ptr); }
    public:
        using allocator_type = Allocator;
        using value_type = T;
        using size_type = size_t;
        using volatile_iterator = proxy_iterator<volatile array>;
        using const_iterator = proxy_iterator<const array>;
        using iterator = proxy_iterator<array>;
        explicit array(){}
        array(const array& a) = default;
        explicit array(size_t n) : allocator_(n*sizeof(T)), size_(n) {}

        array& operator = (const array& rhs){
            array c(rhs);
            this->swap(c);
            return *this;
        }
        template<class OtherAllocator>
        array& operator = (const array<T,OtherAllocator>& rhs){
            array resized(rhs.size());
            this->swap(resized);
            if(!bind::weak(rhs)) bind::cpu(detail::copy_array<T,Allocator,OtherAllocator>, *this, rhs, this->size_);
            return *this;
        }
        void fill(T value){
            bind::cpu(detail::fill_array<T,Allocator>, *this, value);
        }
        void swap(array<T,Allocator>& r){
            std::swap(this->size_, r.size_);
            std::swap(this->allocator_.after->data, r.allocator_.after->data); // fixme
        }
        size_t size() const volatile {
            return size_;
        }
        bool empty() const volatile {
            return ((size() == 0) || bind::weak(*this));
        }
        value_type* data() volatile {
            return (value_type*)allocator_.data();
        }
        const value_type* data() const volatile {
            return (value_type*)allocator_.data();
        }
        value_type& operator[](size_t i) volatile {
            return data()[ i ];
        }
        const value_type& operator[](size_t i) const volatile {
            return data()[ i ];
        }
        value_type& at(size_type i) volatile {
            if(i >= size()) throw std::out_of_range("array::out_of_range");
            return (*this)[i];
        }
        const value_type& at(size_type i) const volatile {
            if(i >= size()) throw std::out_of_range("array::out_of_range");
            return (*this)[i];
        }
        value_type& front() volatile {
            return (*this)[0];
        }
        const value_type& front() const volatile {
            return (*this)[0];
        }
        value_type& back() volatile {
            return (*this)[size()-1];
        }
        const value_type& back() const volatile {
            return (*this)[size()-1];
        }
        iterator begin() volatile {
            return iterator(const_cast<array&>(*this), 0);
        }
        iterator end() volatile {
            return iterator(const_cast<array&>(*this), size());
        }
        const_iterator begin() const volatile {
            return cbegin();
        }
        const_iterator end() const volatile {
            return cend();
        }
        const_iterator cbegin() const volatile {
            return const_iterator(const_cast<const array&>(*this), 0);
        }
        const_iterator cend() const volatile {
            return const_iterator(const_cast<const array&>(*this), size());
        }
        volatile_iterator vbegin() volatile {
            return volatile_iterator(*this, 0);
        }
        volatile_iterator vend() volatile {
            return volatile_iterator(*this, size());
        }
    private:
        mutable size_t size_;
    public:
        mutable allocator_type allocator_;
    };

}

#endif
