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

#ifndef BIND_INTERFACE_TYPED
#define BIND_INTERFACE_TYPED

#define EXTRACT(var) T& var = *(T*)m->arguments[arg];

namespace bind {
    using model::functor;
    using model::revision;

    template <typename T> struct info;
    template <typename T> class iterator;
    template <typename T> class ptr;

    // {{{ compile-time type info: singular types
    template <typename T, bool Compact = false> struct singular_info {
        template<size_t arg> static void deallocate   (functor* ){ }
        template<size_t arg> static bool pin          (functor* ){ return false; }
        template<size_t arg> static bool ready        (functor* ){ return true; }
        template<size_t arg> static T&   revised      (functor* m){ EXTRACT(o); return o; }
        template<size_t arg> static void modify_remote(T&){ }
        template<size_t arg> static void modify_local(T& o, functor* m){
            m->arguments[arg] = memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy(m->arguments[arg], &o, sizeof(T));
        }
        template<size_t arg> static void modify (T& o, functor* m){
            m->arguments[arg] = memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy(m->arguments[arg], &o, sizeof(T));
        }
        static constexpr bool ReferenceOnly = false;
    };

    template <typename T> struct singular_info<T, true> : public singular_info<T> {
        template<size_t arg> static T& revised(functor* m){ return *(T*)&m->arguments[arg]; }
        template<size_t arg> static void modify_local(T& o, functor* m){ *(T*)&m->arguments[arg] = o; }
        template<size_t arg> static void modify(T& o, functor* m){
            *(T*)&m->arguments[arg] = o;
        }
    };
    // }}}
    // {{{ compile-time type info: ptr types
    template <typename T> struct const_ptr_info : public singular_info<T> {
        template<size_t arg> static bool ready(functor* m){
            EXTRACT(o);
            if(o.impl->origin && o.impl->origin->generator != NULL) return false;
            return (o.impl->generator == m || o.impl->generator == NULL);
        }
        template<size_t arg> static bool pin(functor* m){
            EXTRACT(o);
            if(o.impl->generator == NULL) return false;
            (o.impl->generator.load())->queue(m);
            return true;
        }
        static constexpr bool ReferenceOnly = true;
    };
    template <typename T> struct ptr_info : public const_ptr_info<T> {
        template<size_t arg> static void deallocate(functor* m){
            EXTRACT(o); o.impl->complete();
        }
        template<size_t arg> static bool pin(functor* m){
            EXTRACT(o);
            if(!o.impl->origin || o.impl->origin->generator == NULL) return false;
            (o.impl->origin->generator.load())->queue(m);
            return true;
        }
        template<size_t arg> static void modify_remote(T& o){
            o.resit();
            bind::select().rsync(o.impl);
        }
        template<size_t arg> static void modify_local(const T& o, functor* m){
            if(o.impl->generator != m){
                o.resit();
                o.impl->generator = m;
            }
            bind::select().lsync(o.impl);
            m->arguments[arg] = memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy(m->arguments[arg], &o, sizeof(T)); 
        }
        template<size_t arg> static void modify(const T& o, functor* m){
            if(o.impl->generator != m){
                o.resit();
                o.impl->generator = m;
            }
            m->arguments[arg] = memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy(m->arguments[arg], &o, sizeof(T)); 
        }
        template<size_t arg> static T& revised(functor* m){
            EXTRACT(o);
            if(o.impl->origin){
                *o.impl = (typename T::element_type&)*o.impl->origin;
                o.impl->origin = NULL;
            }
            return o;
        }
    };
    // }}}
    // {{{ compile-time type info: iterator types
    template <typename T> struct iterator_info : public singular_info<T> {
        typedef typename info<typename T::container_type>::typed typed;
        typedef typename T::container_type container_type;

        template<size_t arg> 
        static void deallocate(functor* m){
            EXTRACT(o); typed::deallocate_(*o.container);
        }
        template<size_t arg>
        static void modify_remote(T& o){
            typed::modify_remote<arg>(*o.container);
        }
        template<size_t arg>
        static void modify_local(T& o, functor* m){
            typed::modify_local<arg>(*o.container, m);
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &o, sizeof(T));
            var->container = (container_type*)m->arguments[arg]; m->arguments[arg] = (void*)var;
        }
        template<size_t arg>
        static void modify(T& o, functor* m){
            typed::modify<arg>(*o.container, m);
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &o, sizeof(T));
            var->container = (container_type*)m->arguments[arg]; m->arguments[arg] = (void*)var;
        }
        template<size_t arg>
        static T& revised(functor* m){
            EXTRACT(o); revise(*o.container); return o;
        }
        template<size_t arg> 
        static bool pin(functor* m){ 
            EXTRACT(o); return typed::pin_(*o.container, m);
        }
        template<size_t arg> 
        static bool ready(functor* m){
            EXTRACT(o); return typed::ready_(*o.container, m);
        }
    };
    // }}}
    // {{{ compile-time type info: versioned types
    template <typename T> struct versioned_info : public singular_info<T> {
        template<size_t arg> 
        static void deallocate(functor* m){
            EXTRACT(o); deallocate_(o);
        }
        template<size_t arg> 
        static bool pin(functor* m){
            EXTRACT(o); return pin_(o,m);
        }
        template<size_t arg> 
        static bool ready(functor* m){
            EXTRACT(o); return ready_(o, m);
        }
        template<size_t arg>
        static T& revised(functor* m){ 
            EXTRACT(o); revise(o);
            return o;
        }
        static void deallocate_(T& o){
            revision& parent  = *o.allocator_.before;
            revision& current = *o.allocator_.after;
            current.complete();
            current.release();
            bind::select().squeeze(&parent);
            parent.release();
        }
        template<size_t arg>
        static void modify_remote(T& obj){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            if(o->back()->owner != bind::nodes::which_())
                bind::select().rsync(o->back());
            bind::select().collect(o->back());
            bind::select().add_revision<locality::remote>(o, NULL, bind::nodes::which_()); 
        }
        template<size_t arg>
        static void modify_local(T& obj, functor* m){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &obj, sizeof(T)); 
            m->arguments[arg] = (void*)var;
            bind::select().lsync(o->back());
            bind::select().use_revision(o);

            var->allocator_.before = o->current;
            if(o->current->generator != m){
                bind::select().collect(o->back());
                bind::select().add_revision<locality::local>(o, m, bind::rank());
            }
            bind::select().use_revision(o);
            var->allocator_.after = obj.allocator_.after = o->current;
        }
        template<size_t arg>
        static void modify(T& obj, functor* m){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &obj, sizeof(T)); m->arguments[arg] = (void*)var;
            bind::select().sync(o->back());
            bind::select().use_revision(o);

            var->allocator_.before = o->current;
            if(o->current->generator != m){
                bind::select().collect(o->back());
                bind::select().add_revision<locality::common>(o, m, bind::rank()); 
            }
            bind::select().use_revision(o);
            var->allocator_.after = obj.allocator_.after = o->current;
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
        static constexpr bool ReferenceOnly = true;
    };
    // {{{ compile-time type info: const/volatile cases of the versioned types
    template <typename T> struct const_versioned_info : public versioned_info<T> {
        template<size_t arg>
        static void deallocate(functor* m){
            EXTRACT(o); deallocate_(o);
        }
        template<size_t arg> 
        static bool pin(functor* m){ 
            EXTRACT(o); return pin_(o,m);
        }
        static void deallocate_(T& o){
            revision& r = *o.allocator_.before;
            bind::select().squeeze(&r);
            r.release();
        }
        template<size_t arg> static void modify_remote(T& obj){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            if(o->back()->owner != bind::nodes::which_())
                bind::select().rsync(o->back());
        }
        template<size_t arg> static void modify_local(T& obj, functor* m){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &obj, sizeof(T)); m->arguments[arg] = (void*)var;
            var->allocator_.before = var->allocator_.after = o->current;
            bind::select().lsync(o->back());
            bind::select().use_revision(o);
        }
        template<size_t arg> static void modify(T& obj, functor* m){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &obj, sizeof(T)); m->arguments[arg] = (void*)var;
            var->allocator_.before = var->allocator_.after = o->current;
            bind::select().sync(o->back());
            bind::select().use_revision(o);
        }
        static bool pin_(T& o, functor* m){
            revision& r = *o.allocator_.before;
            if(r.generator != NULL){
                (r.generator.load())->queue(m);
                return true;
            }
            return false;
        }
    };
    template <typename T> struct volatile_versioned_info : public versioned_info<T> {
        template<size_t arg> static void modify_remote(T& obj){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            bind::select().collect(o->back());
            bind::select().add_revision<locality::remote>(o, NULL, bind::nodes::which_()); 
        }
        template<size_t arg> static void modify_local(T& obj, functor* m){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, (void*)&obj, sizeof(T)); m->arguments[arg] = (void*)var;

            bind::select().use_revision(o);
            bind::select().collect(o->back());

            var->allocator_.before = o->current;
            if(o->current->generator != m){
                bind::select().add_revision<locality::local>(o, m, bind::rank()); 
                bind::select().use_revision(o);
            }
            var->allocator_.after = obj.allocator_.after = o->current;
        }
        template<size_t arg> static void modify(T& obj, functor* m){
            auto o = obj.allocator_.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, (void*)&obj, sizeof(T)); m->arguments[arg] = (void*)var;
            bind::select().use_revision(o);
            bind::select().collect(o->back());

            var->allocator_.before = o->current;
            if(o->current->generator != m){
                bind::select().add_revision<locality::common>(o, m, bind::rank()); 
                bind::select().use_revision(o);
            }
            var->allocator_.after = obj.allocator_.after = o->current;
        }
        template<size_t arg> static bool pin(functor* m){ return false; }
        template<size_t arg> static bool ready(functor* m){ return true;  }
        static bool pin_(T&, functor*){ return false; }
        static bool ready_(T&, functor*){ return true; }
    };
    // }}}
    // }}}
    // {{{ compile-time type info: specialization for forwarded types
    namespace detail {
        template<typename T>
        constexpr bool compact(){ return sizeof(T) <= sizeof(void*); }

        template <typename T> struct has_versioning {
            template <typename T1> static typename T1::allocator_type::bind_type test(int);
            template <typename> static void test(...);
            enum { value = !std::is_void<decltype(test<T>(0))>::value };
        };
        template <bool Versioned, typename T> struct get_info { typedef singular_info< T, compact<T>() > type; };
        template<typename T> struct get_info<true, T> { typedef versioned_info< T > type; };

        template <bool Versioned, typename T> struct const_get_info { typedef singular_info< const T, compact<T>() > type; };
        template<typename T> struct const_get_info<true, T> { typedef const_versioned_info< const T > type; };

        template <bool Versioned, typename T> struct volatile_get_info { typedef singular_info< volatile T, compact<T>() > type; };
        template<typename T> struct volatile_get_info<true, T> { typedef volatile_versioned_info< volatile T > type; };
    }

    template <typename T> struct info {
        typedef typename detail::get_info<detail::has_versioning<T>::value,T>::type typed;
    };
    template <typename T> struct info <const T> {
        typedef typename detail::const_get_info<detail::has_versioning<T>::value,T>::type typed;
    };
    template <typename T> struct info <volatile T> {
        typedef typename detail::volatile_get_info<detail::has_versioning<T>::value,T>::type typed;
    };
    template <typename S> struct info < ptr<S> > {
        typedef ptr_info<ptr<S> > typed; 
    };
    template <typename S> struct info < const ptr<S> > {
        typedef const_ptr_info<const ptr<S> > typed; 
    };
    template <typename S> struct info < iterator<S> > {
        typedef iterator_info<iterator<S> > typed;
    };
    // }}}
}

#undef EXTRACT
#endif
