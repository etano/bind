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

#ifndef BIND_CONTAINER_VECTOR_VECTOR_H
#define BIND_CONTAINER_VECTOR_VECTOR_H

namespace bind {

    template<class T, class Allocator>
    class vector_async;

    template <class T, class Allocator = bind::allocator>
    class vector {
    public:
        void* operator new (size_t size, void* ptr){ return ptr; }
        void  operator delete (void*, void*){ /* doesn't throw */ }
        void* operator new (size_t sz){ return bind::memory::malloc<bind::memory::cpu::fixed,vector>(); }
        void operator delete (void* ptr){ bind::memory::free<bind::memory::cpu::fixed,sizeof(vector)>(ptr); }
    public:
        typedef vector_async<T,Allocator> async_type;
        typedef Allocator allocator_type;
        typedef T value_type;
        typedef size_t size_type;
        typedef size_t difference_type;
        typedef T* iterator;
        typedef const T* const_iterator;
        explicit vector(){}

        size_t capacity() const;
        size_t cached_size() const;

        /* prohibited in async mode (sync mode only) */

        explicit vector(size_t n, T value = T());
        vector(const vector& a);
        vector& operator = (const vector& rhs);
        template<class OtherAllocator>
        vector& operator = (const vector<T,OtherAllocator>& rhs);

        void init(T value);
        void auto_reserve();

        virtual void reserve(size_t n);
        virtual void shrink_to_fit();
        virtual size_t measure() const; // causes sync, updates cached size
        virtual void load() const;      // causes sync, sets right pointers

        /* using cached size */

        virtual void swap(vector<T,Allocator>& r);
        virtual size_t size() const;
        virtual bool empty() const;
        virtual void resize(size_t sz);
        virtual void clear();

        /* using data-access (load required if not async) */

        value_type* data();
        value_type& operator[](size_t i);
        value_type& at(size_type i);
        value_type& front();
        value_type& back();
        iterator begin();
        iterator end();

        const value_type* data() const;
        const value_type& operator[](size_t i) const;
        const value_type& at(size_type i) const;
        const value_type& front() const;
        const value_type& back() const;
        const_iterator cbegin() const;
        const_iterator cend() const;

        virtual void push_back(value_type value);
        virtual void pop_back();
        virtual iterator insert(const_iterator position, value_type val);
        virtual iterator erase(const_iterator position);
    private:
        mutable size_t cached_size_;
    public:
    BIND_DELEGATE(
        size_t size_;
        value_type data[ BIND_VAR_LENGTH ]; 
    )};

}

namespace std {

    template<typename T>
    class vector<T, bind::allocator> : public bind::vector<T, bind::allocator> {
    public:
        vector(int n){
            printf("my vector!\n");
        }
    };
}

#endif
