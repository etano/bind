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

#ifndef BIND_CORE_COLLECTOR
#define BIND_CORE_COLLECTOR

namespace bind{ namespace core {

    using model::revision;
    using model::any;

    class collector {
    public:
        void squeeze(revision* r) const {
            if((!r->referenced() || model::remote(r)) && r->locked_once()) r->spec.free(r->data);
        }
        void push_back(revision* r){
            r->weaken();
            if(!r->referenced()){ // squeeze
                if(!r->locked()) r->spec.free(r->data); // artifacts or last one
                this->rs.push_back(r);
            }
        }
        void push_back(any* o){
            this->as.push_back(o);
        }
        void clear(){
            for(auto r : rs){ r->spec.free(r->data); delete r; }
            for(auto a : as) memory::cpu::standard::free(a);
            rs.clear();
            as.clear();
        }
    private:
        std::vector<revision*> rs;
        std::vector<any*> as;
    };

} }

#endif
