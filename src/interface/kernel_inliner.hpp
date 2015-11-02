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

#ifndef BIND_INTERFACE_KERNEL_INLINER
#define BIND_INTERFACE_KERNEL_INLINER

namespace bind {

    template<typename T>
    struct check_if_not_reference {
        template<bool C, typename F>  struct fail_if_true { typedef F type; };
        template<typename F> struct fail_if_true<true, F> { };
        typedef typename fail_if_true<modifier<T>::type::ReferenceOnly, T>::type type; // T can be passed only by reference
    };
 
    template<typename T>
    struct check_if_not_reference<T&> {
        typedef T type;
    };

    template <typename T>
    using remove_reference = typename std::remove_reference< typename check_if_not_reference< T >::type >::type;
    using model::functor;

    template<int N> void expand_modify_remote(){}
    template<int N> void expand_modify_local(functor* o){}
    template<int N> void expand_modify(functor* o){}
    template<int N> bool expand_pin(functor* o){ return false; }
    template<int N> void expand_deallocate(functor* o){ }
    template<int N> bool expand_ready(functor* o){ return true; }

    template<int N, typename T, typename... TF>
    void expand_modify_remote(T& arg, TF&... other){
        modifier<T>::type::template apply_remote<N>(arg);
        expand_modify_remote<N+1>(other...);
    }
    template<int N, typename T, typename... TF>
    void expand_modify_local(functor* o, T& arg, TF&... other){
        modifier<T>::type::template apply_local<N>(arg, o);
        expand_modify_local<N+1>(o, other...);
    }
    template<int N, typename T, typename... TF>
    void expand_modify(functor* o, T& arg, TF&... other){
        modifier<T>::type::template apply<N>(arg, o);
        expand_modify<N+1>(o, other...);
    }
    template<int N, typename T, typename... TF>
    bool expand_pin(functor* o){
        return modifier<remove_reference<T> >::type::template pin<N>(o) ||
               expand_pin<N+1,TF...>(o);
    }
    template<int N, typename T, typename... TF>
    void expand_deallocate(functor* o){
        modifier<remove_reference<T> >::type::template deallocate<N>(o);
        expand_deallocate<N+1,TF...>(o);
    }
    template<int N, typename T, typename... TF>
    bool expand_ready(functor* o){
        return modifier<remove_reference<T> >::type::template ready<N>(o) &&
               expand_ready<N+1,TF...>(o);
    }

    template<typename FP, FP fp>
    struct kernel_inliner {};

    template< typename... TF , void(*fp)( TF... )>
    struct kernel_inliner<void(*)( TF... ), fp> {
        static const int arity = sizeof...(TF);

        static inline void latch(functor* o, TF&... args){
            #ifndef BIND_TRANSPORT_NOP
            if(bind::select().get_node().remote())   { expand_modify_remote<0>(args...); return; }
            else if(bind::select().get_node().local()) expand_modify_local<0>(o, args...);
            else
            #endif
            expand_modify<0>(o, args...);
            expand_pin<0,TF...>(o) || bind::select().queue(o);
        }
        static inline void cleanup(functor* o){
            expand_deallocate<0,TF...>(o);
        }
        static inline bool ready(functor* o){
            return expand_ready<0,TF...>(o);
        }
        template<size_t...I>
        static void expand_invoke(std::index_sequence<I...>, functor* o){
            (*fp)(modifier<remove_reference<TF> >::type::template load<I>(o)...);
        }
        static inline void invoke(functor* o){
            expand_invoke(std::make_index_sequence<sizeof...(TF)>(), o);
        }
    };

}

#endif

