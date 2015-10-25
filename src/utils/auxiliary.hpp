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

#ifndef BIND_UTILS_AUXILIARY
#define BIND_UTILS_AUXILIARY

namespace bind {

    inline void sync(){
        bind::select().sync();
    }

    inline int num_procs(){
        return bind::select().get_num_procs();
    }

    inline int num_threads(){
        static int n = __cilkrts_get_nworkers(); return n;
    }

    inline int get_sid(){
        return bind::select().get_sid();
    }

    inline int generate_sid(){
        return bind::select().generate_sid();
    }

    inline rank_t rank(){
        return bind::select().get_rank();
    }

    template<typename T>
    inline void destroy(T* o){ 
        bind::select().collect(o); 
    }

    template<typename V>
    inline bool weak(const V& obj){
        return obj.bind_allocator.desc->weak();
    }

    template<typename V>
    inline dim2 get_dim(const V& obj){
        return obj.bind_allocator.desc->dim;
    }
    
    template<typename V> 
    inline size_t extent(V& obj){ 
        return obj.bind_allocator.desc->extent;
    }

    template<typename V>
    inline rank_t get_owner(const V& o){
        return o.bind_allocator.desc->current->owner;
    }

    template<typename V>
    inline bool locked_once(const V& o){
        return o.bind_allocator.before->locked_once();
    }

}

#endif
