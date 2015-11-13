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

#ifndef BIND_INTERFACE_MODIFIERS_VERSIONED
#define BIND_INTERFACE_MODIFIERS_VERSIONED

#define EXTRACT(var) T& var = *(T*)m->arguments[Arg];

namespace bind {
    using model::functor;
    using model::revision;

    template <class Device, typename T>
    struct versioned_modifier : public singular_modifier<T> {
        template<size_t Arg> 
        static void deallocate(functor* m){
            EXTRACT(o); deallocate_(o);
        }
        template<size_t Arg> 
        static bool pin(functor* m){
            EXTRACT(o); return pin_(o,m);
        }
        template<size_t Arg> 
        static bool ready(functor* m){
            EXTRACT(o); return ready_(o, m);
        }
        template<size_t Arg>
        static void load(functor* m){ 
            EXTRACT(o); load_(o);
        }
        static void deallocate_(T& o){
            revision& parent  = *o.allocator_.before;
            revision& current = *o.allocator_.after;
            current.complete();
            current.release();
            bind::select().squeeze(&parent);
            parent.release();
        }
        template<locality L, size_t Arg>
        static void apply_(T& obj, functor* m){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &obj, sizeof(T)); 
            m->arguments[Arg] = (void*)var;
            bind::select().sync<Device, L>(o->back());
            bind::select().use_revision(o);

            var->allocator_.before = o->current;
            if(o->current->generator != m){
                bind::select().collect(o->back());
                bind::select().add_revision<L>(o, m, bind::rank());
            }
            bind::select().use_revision(o);
            var->allocator_.after = obj.allocator_.after = o->current;
        }
        template<size_t Arg>
        static void apply_remote(T& obj){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            if(o->back()->owner != bind::nodes::which_())
                bind::select().sync<Device, locality::remote>(o->back());
            bind::select().collect(o->back());
            bind::select().add_revision<locality::remote>(o, NULL, bind::nodes::which_()); 
        }
        template<size_t Arg>
        static void apply_local(T& obj, functor* m){
            apply_<locality::local, Arg>(obj, m);
        }
        template<size_t Arg>
        static void apply_common(T& obj, functor* m){
            apply_<locality::common, Arg>(obj, m);
        }
        static bool pin_(T& o, functor* m){
            revision& r = *o.allocator_.before;
            if(r.generator != NULL && r.generator != m){
                (r.generator.load())->queue(m);
                return true;
            }
            return false;
        }
        static bool ready_(T& o, functor* m){
            revision& r = *o.allocator_.before;
            if(r.generator == NULL || r.generator == m) return true;
            return false;
        }
        static void load_(T& o){ 
            revision& c = *o.allocator_.after; if(c.valid()) return;
            revision& p = *o.allocator_.before;
            if(!p.valid()) c.embed(o.allocator_.calloc(c.spec));
            else if(p.locked_once() && !p.referenced() && c.spec.conserves(p.spec)) c.reuse(p);
            else{
                c.embed(o.allocator_.alloc(c.spec));
                memcpy(c.data, p.data, p.spec.extent);
            }
        }
        static constexpr bool ReferenceOnly = true;
    };
    // {{{ compile-time type modifier: const/volatile cases of the versioned types
    template <class Device, typename T>
    struct const_versioned_modifier : public versioned_modifier<Device, T> {
        template<size_t Arg>
        static void deallocate(functor* m){
            EXTRACT(o); deallocate_(o);
        }
        static void deallocate_(T& o){
            revision& r = *o.allocator_.before;
            bind::select().squeeze(&r);
            r.release();
        }
        template<size_t Arg>
        static void load(functor* m){ 
            EXTRACT(o); load_(o);
        }
        static void load_(T& o){
            revision& c = *o.allocator_.before; if(c.valid()) return;
            c.embed(o.allocator_.calloc(c.spec));
        }
        template<locality L, size_t Arg> static void apply_(T& obj, functor* m){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &obj, sizeof(T)); m->arguments[Arg] = (void*)var;
            var->allocator_.before = var->allocator_.after = o->current;
            bind::select().sync<Device, L>(o->back());
            bind::select().use_revision(o);
        }
        template<size_t Arg> static void apply_remote(T& obj){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            if(o->back()->owner != bind::nodes::which_())
                bind::select().sync<Device, locality::remote>(o->back());
        }
        template<size_t Arg> static void apply_local(T& obj, functor* m){
            apply_<locality::local, Arg>(obj, m);
        }
        template<size_t Arg> static void apply_common(T& obj, functor* m){
            apply_<locality::common, Arg>(obj, m);
        }
    };
    template <class Device, typename T>
    struct volatile_versioned_modifier : public versioned_modifier<Device, T> {
        template<size_t Arg> static void load(functor* m){ 
            EXTRACT(o); load_(o);
        }
        static void load_(T& o){
            revision& c = *o.allocator_.after; if(c.valid()) return;
            revision& p = *o.allocator_.before;
            if(p.valid() && p.locked_once() && !p.referenced() && c.spec.conserves(p.spec)) c.reuse(p);
            else c.embed(o.allocator_.alloc(c.spec));
        }
        template<locality L, size_t Arg> static void apply_(T& obj, functor* m){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, (void*)&obj, sizeof(T)); m->arguments[Arg] = (void*)var;
            bind::select().use_revision(o);

            var->allocator_.before = o->current;
            if(o->current->generator != m){
                bind::select().collect(o->back());
                bind::select().add_revision<L>(o, m, bind::rank()); 
            }
            bind::select().use_revision(o);
            var->allocator_.after = obj.allocator_.after = o->current;
        }
        template<size_t Arg> static void apply_remote(T& obj){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            bind::select().collect(o->back());
            bind::select().add_revision<locality::remote>(o, NULL, bind::nodes::which_()); 
        }
        template<size_t Arg> static void apply_local(T& obj, functor* m){
            apply_<locality::local, Arg>(obj, m);
        }
        template<size_t Arg> static void apply_common(T& obj, functor* m){
            apply_<locality::common, Arg>(obj, m);
        }
        template<size_t Arg> static bool pin(functor* m){ return false; }
        template<size_t Arg> static bool ready(functor* m){ return true;  }
        static bool pin_(T&, functor*){ return false; }
        static bool ready_(T&, functor*){ return true; }
    };
    // }}}
}

#undef EXTRACT
#endif
