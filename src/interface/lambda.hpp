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

#ifndef BIND_INTERFACE_LAMBDA
#define BIND_INTERFACE_LAMBDA

namespace bind {

    template<class Device, typename F, typename... T>
    struct lambda_kernel : public kernel<Device, lambda_kernel<Device, F, T...> > {
        typedef void(*ftype)(T..., F&);
        static void fw(T... args, F& func){ func(args...); }
        static constexpr ftype c = &fw;
    };

    template <typename Function>
    struct function_traits : public function_traits<decltype(&Function::operator())> {};

    template <typename ClassType, typename ReturnType, typename... Args>
    struct function_traits<ReturnType(ClassType::*)(Args...) const> {
        typedef ReturnType (*pointer)(Args...);
        typedef const std::function<ReturnType(Args...)> function;
        template<class Device>
        using kernel_type = lambda_kernel<Device, const std::function<ReturnType(Args...)>, Args... >;
    };

    template <typename Function>
    typename function_traits<Function>::function to_function(Function& lambda){
        return static_cast<typename function_traits<Function>::function>(lambda);
    }

    template <class Device, class L>
    struct overload_lambda : L {
        overload_lambda(L l) : L(l) {}
        template <typename... T>
        void operator()(T&& ... values){
            function_traits<L>::template kernel_type<Device>::spawn(std::forward<T>(values)... , to_function(*(L*)this));
        }
    };

    template <class Device, class L>
    overload_lambda<Device, L> lambda(L l){
        return overload_lambda<Device, L>(l);
    }

    template <class L, class... Args>
    void cpu(L l, Args&& ... args){
        lambda<devices::cpu>(l)(std::forward<Args>(args)...);
    }

    template <class... L, class R, class... Args>
    void cpu(R(*l)(L...), Args&& ... args){
        bind::cpu(std::function<R(L...)>(l), std::forward<Args>(args)...);
    }

    template <class L, class... Args>
    void gpu(L l, Args&& ... args){
        lambda<devices::gpu>(l)(std::forward<Args>(args)...);
    }

    template <class... L, class R, class... Args>
    void gpu(R(*l)(L...), Args&& ... args){
        bind::gpu(std::function<R(L...)>(l), std::forward<Args>(args)...);
    }
}

#endif

