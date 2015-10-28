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

#ifndef BIND_INTERFACE_ACCESS
#define BIND_INTERFACE_ACCESS

namespace bind {

    using model::revision;

    template <typename T> static T& load(T& obj){ 
        bind::select().touch(obj.bind_allocator.desc, bind::rank());
        bind::sync(); 
        revision& c = *obj.bind_allocator.desc->current;
        assert(c.state == locality::local || c.state == locality::common);
        if(!c.valid()) c.embed(obj.bind_allocator.calloc(c.spec));
        obj.bind_allocator.after = obj.bind_allocator.desc->current;
        return obj;
    }

    template <typename T> static auto delegated(T& obj) -> typename T::bind_type_structure& {
        return *(typename T::bind_type_structure*)(*obj.bind_allocator.after);
    }

    template <typename T> static void revise(const T& obj){
        revision& c = *obj.bind_allocator.before; if(c.valid()) return;
        c.embed(obj.bind_allocator.calloc(c.spec));
    }

    template <typename T> static void revise(volatile T& obj){
        revision& c = *obj.bind_allocator.after; if(c.valid()) return;
        revision& p = *obj.bind_allocator.before;
        if(p.valid() && p.locked_once() && !p.referenced() && c.spec.conserves(p.spec)) c.reuse(p);
        else c.embed(obj.bind_allocator.alloc(c.spec));
    }

    template <typename T> static void revise(T& obj){
        revision& c = *obj.bind_allocator.after; if(c.valid()) return;
        revision& p = *obj.bind_allocator.before;
        if(!p.valid()) c.embed(obj.bind_allocator.calloc(c.spec));
        else if(p.locked_once() && !p.referenced() && c.spec.conserves(p.spec)) c.reuse(p);
        else{
            c.embed(obj.bind_allocator.alloc(c.spec));
            memcpy((T*)c, (T*)p, p.spec.extent);
        }
    }
}

#endif
