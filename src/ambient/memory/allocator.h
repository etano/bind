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

#ifndef AMBIENT_MEMORY_ALLOCATOR_H
#define AMBIENT_MEMORY_ALLOCATOR_H

namespace ambient {

    template <class T>
    class default_allocator {
    public:
        typedef T value_type;
        template <class U> struct rebind { typedef default_allocator<U> other; };
        default_allocator() throw() { }
        default_allocator(const default_allocator&) throw() { }
        template<typename U> default_allocator(const default_allocator<U>&) throw() { }
       ~default_allocator() throw() { }

        static T* allocate(std::size_t n){ throw; }
        static void deallocate(T* ptr, std::size_t n){ throw; }

        static void* alloc(memory::descriptor& spec); 
        static void* calloc(memory::descriptor& spec);
        static void free(void* ptr, memory::descriptor& spec);
    };

}

#endif
