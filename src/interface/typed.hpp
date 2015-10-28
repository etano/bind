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

#define EXTRACT(var) T* var = (T*)m->arguments[arg];

namespace bind {
    template<typename T> class ptr;
    using model::functor;
    using model::revision;

    // {{{ compile-time type info: singular types + inplace and ptr specializations
    template <typename T> struct singular_info {
        template<size_t arg> static void deallocate     (functor* m){                        }
        template<size_t arg> static bool pin            (functor* m){ return false;          }
        template<size_t arg> static bool ready          (functor* m){ return true;           }
        template<size_t arg> static T&   revised        (functor* m){ EXTRACT(o); return *o; }
        template<size_t arg> static void modify (T& obj, functor* m){
            m->arguments[arg] = (void*)new(memory::cpu::instr_bulk::malloc<sizeof(T)>()) T(obj); 
        }
        template<size_t arg> static void modify_remote(T& obj)      {                        }
        template<size_t arg> static void modify_local(T& obj, functor* m){
            m->arguments[arg] = (void*)new(memory::cpu::instr_bulk::malloc<sizeof(T)>()) T(obj);
        }
        static constexpr bool ReferenceOnly = false;
    };
    template <typename T> struct singular_inplace_info : public singular_info<T> {
        template<size_t arg> static T& revised(functor* m){ return *(T*)&m->arguments[arg]; }
        template<size_t arg> static void modify_remote(T& obj){ }
        template<size_t arg> static void modify_local(T& obj, functor* m){ *(T*)&m->arguments[arg] = obj; }
        template<size_t arg> static void modify(T& obj, functor* m){ *(T*)&m->arguments[arg] = obj; }
    };
    template <typename T> struct ptr_info : public singular_info<T> {
        template<size_t arg> static void deallocate(functor* m){
            EXTRACT(o); o->impl->generator = NULL;
        }
        template<size_t arg> static void modify_remote(T& obj){
            bind::select().rsync(obj.impl);
        }
        template<size_t arg> static void modify_local(const T& obj, functor* m){
            obj.impl->generator = m;
            bind::select().lsync(obj.impl);
            m->arguments[arg] = memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy(m->arguments[arg], &obj, sizeof(T)); 
        }
        template<size_t arg> static void modify(const T& obj, functor* m){
            m->arguments[arg] = memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy(m->arguments[arg], &obj, sizeof(T)); 
        }
        static constexpr bool ReferenceOnly = true;
    };
    template <typename T> struct read_ptr_info : public ptr_info<T> {
        template<size_t arg> static void deallocate(functor* m){ }
        template<size_t arg> static void modify_remote(T& obj){ }
        template<size_t arg> static void modify_local(const T& obj, functor* m){
            m->arguments[arg] = memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy(m->arguments[arg], &obj, sizeof(T)); 
        }
    };
    // }}}
    // {{{ compile-time type info: iteratable derived types
    template <typename T> struct iteratable_info : public singular_info<T> {
        template<size_t arg> 
        static void deallocate(functor* m){
            EXTRACT(o);
            revision& parent  = *o->bind_allocator.before;
            revision& current = *o->bind_allocator.after;
            current.complete();
            current.release();
            bind::select().squeeze(&parent);
            parent.release();
        }
        template<size_t arg>
        static void modify_remote(T& obj){
            auto o = obj.bind_allocator.desc;
            bind::select().touch(o, bind::rank());
            if(o->back()->owner != bind::nodes::which())
                bind::select().rsync(o->back());
            bind::select().collect(o->back());
            bind::select().add_revision<locality::remote>(o, NULL, bind::nodes::which()); 
        }
        template<size_t arg>
        static void modify_local(T& obj, functor* m){
            auto o = obj.bind_allocator.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &obj, sizeof(T)); 
            m->arguments[arg] = (void*)var;
            bind::select().lsync(o->back());
            bind::select().use_revision(o);

            var->bind_allocator.before = o->current;
            if(o->current->generator != m){
                bind::select().collect(o->back());
                bind::select().add_revision<locality::local>(o, m, bind::rank());
            }
            bind::select().use_revision(o);
            var->bind_allocator.after = o->current;
        }
        template<size_t arg>
        static void modify(T& obj, functor* m){
            auto o = obj.bind_allocator.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &obj, sizeof(T)); m->arguments[arg] = (void*)var;
            bind::select().sync(o->back());
            bind::select().use_revision(o);

            var->bind_allocator.before = o->current;
            if(o->current->generator != m){
                bind::select().collect(o->back());
                bind::select().add_revision<locality::common>(o, m, bind::rank()); 
            }
            bind::select().use_revision(o);
            var->bind_allocator.after = o->current;
        }
        template<size_t arg>
        static T& revised(functor* m){ 
            EXTRACT(o); revise(*o);
            return *o;
        }
        template<size_t arg> 
        static bool pin(functor* m){ 
            EXTRACT(o);
            revision& r = *o->bind_allocator.before;
            if(r.generator != NULL && r.generator != m){
                (r.generator.load())->queue(m);
                return true;
            }
            return false;
        }
        template<size_t arg> 
        static bool ready(functor* m){
            EXTRACT(o);
            revision& r = *o->bind_allocator.before;
            if(r.generator == NULL || r.generator == m) return true;
            return false;
        }
        static constexpr bool ReferenceOnly = true;
    };
    // }}}
    // {{{ compile-time type info: only read/write iteratable derived types

    template <typename T> struct read_iteratable_info : public iteratable_info<T> {
        template<size_t arg> static void deallocate(functor* m){
            EXTRACT(o);
            revision& r = *o->bind_allocator.before;
            bind::select().squeeze(&r);
            r.release();
        }
        template<size_t arg> static void modify_remote(T& obj){
            auto o = obj.bind_allocator.desc;
            bind::select().touch(o, bind::rank());
            if(o->back()->owner != bind::nodes::which())
                bind::select().rsync(o->back());
        }
        template<size_t arg> static void modify_local(T& obj, functor* m){
            auto o = obj.bind_allocator.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &obj, sizeof(T)); m->arguments[arg] = (void*)var;
            var->bind_allocator.before = var->bind_allocator.after = o->current;
            bind::select().lsync(o->back());
            bind::select().use_revision(o);
        }
        template<size_t arg> static void modify(T& obj, functor* m){
            auto o = obj.bind_allocator.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, &obj, sizeof(T)); m->arguments[arg] = (void*)var;
            var->bind_allocator.before = var->bind_allocator.after = o->current;
            bind::select().sync(o->back());
            bind::select().use_revision(o);
        }
        template<size_t arg> 
        static bool pin(functor* m){ 
            EXTRACT(o);
            revision& r = *o->bind_allocator.before;
            if(r.generator != NULL){
                (r.generator.load())->queue(m);
                return true;
            }
            return false;
        }
    };
    template <typename T> struct write_iteratable_info : public iteratable_info<T> {
        template<size_t arg> static void modify_remote(T& obj){
            auto o = obj.bind_allocator.desc;
            bind::select().touch(o, bind::rank());
            bind::select().collect(o->back());
            bind::select().add_revision<locality::remote>(o, NULL, bind::nodes::which()); 
        }
        template<size_t arg> static void modify_local(T& obj, functor* m){
            auto o = obj.bind_allocator.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, (void*)&obj, sizeof(T)); m->arguments[arg] = (void*)var;

            bind::select().use_revision(o);
            bind::select().collect(o->back());

            var->bind_allocator.before = o->current;
            bind::select().add_revision<locality::local>(o, m, bind::rank()); 
            bind::select().use_revision(o);
            var->bind_allocator.after = o->current;
        }
        template<size_t arg> static void modify(T& obj, functor* m){
            auto o = obj.bind_allocator.desc;
            bind::select().touch(o, bind::rank());
            T* var = (T*)memory::cpu::instr_bulk::malloc<sizeof(T)>(); memcpy((void*)var, (void*)&obj, sizeof(T)); m->arguments[arg] = (void*)var;
            bind::select().use_revision(o);
            bind::select().collect(o->back());

            var->bind_allocator.before = o->current;
            bind::select().add_revision<locality::common>(o, m, bind::rank()); 
            bind::select().use_revision(o);
            var->bind_allocator.after = o->current;
        }
        template<size_t arg> static bool pin(functor* m){ return false; }
        template<size_t arg> static bool ready (functor* m){ return true;  }
    };
    // }}}

    // {{{ compile-time type info: specialization for forwarded types

    template <typename T>
    struct is_polymorphic {
        template <typename T1> static typename T1::async_type test(int);
        template <typename>    static void test(...);
        enum { value = !std::is_void<decltype(test<T>(0))>::value };
    };
    template <bool HAS, typename T> struct checked_get_async_type { typedef int type; };
    template <typename T> struct checked_get_async_type<true, T>  { typedef typename T::async_type type; };
    template <typename T> struct get_async_type { typedef typename checked_get_async_type<is_polymorphic<T>::value, T>::type type; };

    template <typename T> struct has_versioning {
        template <typename T1> static typename T1::bind_type_structure test(int);
        template <typename>    static void test(...);
        enum { value = !std::is_void<decltype(test<T>(0))>::value };
    };

    template <bool V, typename T> struct versioned_info { };
    template<typename T> struct versioned_info<true, T> { typedef iteratable_info< T > type; };
    template<typename T> struct versioned_info<false, T> { typedef singular_info< T > type; };

    template <bool V, typename T> struct const_versioned_info { };
    template<typename T> struct const_versioned_info<true, T> { typedef read_iteratable_info< const T > type; };
    template<typename T> struct const_versioned_info<false, T> { typedef singular_info< const T > type; };

    template <typename T>
    struct info {
        typedef typename versioned_info<has_versioning<T>::value,T>::type typed;
    };
    template <typename T>
    struct info <const T> {
        typedef typename const_versioned_info<has_versioning<T>::value,T>::type typed;
    };

    template <typename T>
    struct info <volatile T> {
        typedef write_iteratable_info<volatile T> typed;
    };

    template <typename S>
    struct info < ptr<S> > {
        typedef ptr_info<ptr<S> > typed; 
    };

    template <typename S>
    struct info < const ptr<S> > { 
        typedef read_ptr_info<const ptr<S> > typed; 
    };

    template <>
    struct info < size_t > {
        typedef singular_inplace_info<size_t> typed; 
    };

    // }}}

    #define BIND_DELEGATE(...)       struct  bind_type_structure { __VA_ARGS__ }; \
                                     mutable allocator_type bind_allocator;

    #define BIND_VAR_LENGTH 1
}

#undef EXTRACT
#endif
