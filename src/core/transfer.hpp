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

#ifndef BIND_TRANSPORT_CUDA_TRANSFER_HPP
#define BIND_TRANSPORT_CUDA_TRANSFER_HPP

namespace bind { namespace transport { namespace cuda {

    namespace detail {
        template<device From, device To>
        struct transfer_impl {};
        
        template<>
        struct transfer_impl<device::cpu, device::gpu> {
            using memory_type = memory::gpu::standard;
            static void invoke(void* dst, void* src, size_t sz){
                cudaMemcpy(dst, src, sz, cudaMemcpyHostToDevice);
            }
        };
        
        template<>
        struct transfer_impl<device::gpu, device::cpu> {
            using memory_type = memory::cpu::standard;
            static void invoke(void* dst, void* src, size_t sz){
                cudaMemcpy(dst, src, sz, cudaMemcpyDeviceToHost);
            }
        };
    }

    template<device From, device To>
    void transfer<From, To>::spawn(revision* r, revision*& s){
        s = new revision(r->spec.extent, NULL, r->state, To, r->owner);
        s->generator = new transfer(*r, *s);
    }

    template<device From, device To>
    transfer<From, To>::transfer(revision& r, revision& s) : r(r), s(s) {
        r.use(); s.use();
        s.embed(s.spec.hard_malloc<typename detail::transfer_impl<From, To>::memory_type>());
        if(r.generator != NULL) (r.generator.load())->queue(this);
        else bind::select().queue(this);
    }

    template<device From, device To>
    void transfer<From, To>::invoke(){
        detail::transfer_impl<From, To>::invoke(s.data, r.data, r.spec.extent);

        bind::select().squeeze(&r); r.release();
        bind::select().squeeze(&s); s.release();
        s.complete();
    }

    template<device From, device To>
    bool transfer<From, To>::ready(){
        return (r.generator == NULL);
    }

} } }

#endif
