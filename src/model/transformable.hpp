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

#ifndef BIND_MODEL_TRANSFORMABLE
#define BIND_MODEL_TRANSFORMABLE

namespace bind { namespace model {

    template<typename T>
    constexpr size_t sizeof_64(){
        return bind::memory::aligned_64< sizeof(T) >();
    }

    template<typename T>
    constexpr size_t sizeof_transformable(){
        return (sizeof(void*) + sizeof(size_t) + sizeof_64<T>());
    }

    class transformable {
    public:
        // WARNING: the correct allocation of sizeof_transformable required
        void* operator new (size_t, void* place){ return place; }
        void operator delete (void*, void*){}

        template<typename T>
        transformable(T val) : size(sizeof_64<T>()) { *this = val; }
        template<typename T> operator T& (){ return *(T*)&value;  }
        template<typename T> void operator = (T val){ *(T*)&value = val; }

        functor* generator;
        size_t size;
        int value;
    };

} }

#endif
