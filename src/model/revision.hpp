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

#ifndef BIND_MODEL_REVISION
#define BIND_MODEL_REVISION

namespace bind { namespace model {

    class revision : public memory::cpu::use_fixed_new<revision> {
    public:
        revision(size_t extent, functor* g, locality l, rank_t owner)
        : spec(extent), generator(g), state(l), 
          data(NULL), users(0), owner(owner)
        {
        }

        void embed(void* ptr){
            data = ptr;
        }

        void reuse(revision& r){
            data = r.data;
            spec.reuse(r.spec);
        }

        void use(){
            ++users;
        }

        void release(){
            --users;
        }

        void complete(){
            generator = NULL;
        }

        void invalidate(){
            data = NULL;
        }

        bool locked() const {
            return (users != 0);
        }

        bool locked_once() const {
            return (users == 1);
        }

        bool valid() const {
            return (data != NULL);
        }

        std::atomic<functor*> generator;
        void* data;
        rank_t owner;
        std::atomic<int> users;
        locality state;
        std::pair<size_t, functor*> assist;
        memory::descriptor spec;
    };

    inline bool local(const revision* r){
        return (r->state == locality::local);
    }
    
    inline bool remote(const revision* r){
        return (r->state == locality::remote);
    }
    
    inline bool common(const revision* r){
        return (r->state == locality::common);
    }
    
    inline rank_t owner(const revision* r){
        return r->owner;
    }

} }

#endif
