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

#ifndef BIND_TRANSPORT_NOP
#define BIND_TRANSPORT_NOP

namespace bind { namespace transport { namespace nop {

    template<class T> struct collective {
        bool test(){ return true; }
        void append(rank_t rank){}
        bool involved(){ return true; }
    };

    class channel {
    public:
        typedef typename model::revision block_type;
        typedef typename model::any scalar_type;
        template<class T> using collective_type = collective<T>;
        size_t dim() const { return 1; }
        static void barrier(){}
        collective<block_type>* get(block_type& r){ return NULL; }
        collective<block_type>* set(block_type& r){ return NULL; }
        collective<scalar_type>* bcast(scalar_type& v, rank_t root){ return NULL; }
        collective<scalar_type>* bcast(scalar_type& v){ return NULL; }
        static constexpr rank_t rank = 0;
    };

} } }

#endif