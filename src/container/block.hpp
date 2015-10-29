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

#ifndef BIND_CONTAINER_BLOCK
#define BIND_CONTAINER_BLOCK

namespace bind {
     
    template<typename T, class Allocator = bind::allocator> class block;
    namespace detail { 
        template<typename T>
        void fill_value(volatile block<T>& a, T& value){
            block<T>& a_ = const_cast<block<T>&>(a);
            size_t size = a_.num_rows()*a_.num_cols();
            T* ad = a_.data();
            for(size_t i = 0; i < size; ++i) ad[i] = value;
        }
    }

    template <class T, class Allocator>
    class block {
    public:
        typedef Allocator allocator_type;
        typedef T value_type;
        block(size_t m, size_t n) : allocator_(sizeof(T)*m*n), rows(m), cols(n) {}
        void init(T value){
            bind::cpu(detail::fill_value<T>, *this, value);
        }
        value_type& operator()(size_t i, size_t j){
            return data()[ j*rows + i ];
        }
        const value_type& operator()(size_t i, size_t j) const {
            return data()[ j*rows + i ];
        }
        value_type* data() volatile {
            return (value_type*)allocator_.data();
        }
        const value_type* data() const volatile {
            return (value_type*)allocator_.data();
        }
        size_t num_rows() const {
            return rows;
        }
        size_t num_cols() const {
            return cols;
        }
        size_t rows;
        size_t cols;
        mutable allocator_type allocator_;
    };

}

#endif
