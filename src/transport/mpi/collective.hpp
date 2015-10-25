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

#define BOUNDARY_OVERFLOW -1

namespace bind { namespace transport { namespace mpi {

    inline int generate_sid(){
        return (++channel::setup().sid %= channel::setup().tag_ub);
    }

    template<typename T>
    inline void bcast<T>::dispatch(){
        std::pair<rank_t,rank_t> lr = (*channel::setup().trees[size])[self];
        if(!self){ // self == root
            if(lr.first  != BOUNDARY_OVERFLOW) impl &= new request_impl(send_impl, object, list[lr.first],  tags[lr.first]);
            if(lr.second != BOUNDARY_OVERFLOW) impl &= new request_impl(send_impl, object, list[lr.second], tags[lr.second]);
        }else{
            impl &= new request_impl(recv_impl, object, MPI_ANY_SOURCE, tags[self]);
            if(lr.first  != BOUNDARY_OVERFLOW) impl += new request_impl(send_impl, object, list[lr.first],  tags[lr.first]);
            if(lr.second != BOUNDARY_OVERFLOW) impl += new request_impl(send_impl, object, list[lr.second], tags[lr.second]);
        }
    }

    inline collective<typename channel::block_type>::collective(typename channel::block_type& r, rank_t root) 
    : bcast<typename channel::block_type>(r, root), states(bind::num_procs()+1) {
        this->tree.push_back(root);
        this->tags.push_back(-1);
    }

    inline void collective<typename channel::block_type>::append(rank_t rank){
        if(!states[rank]){
            states[rank] = true;
            if(states.back()){
                for(int i = this->tags.size(); i <= bind::num_procs(); i++)
                    this->tags.push_back(generate_sid());
                for(int i = 0; i < bind::num_procs(); i++)
                    this->states[i] = true;
            }else{
                if(rank == bind::rank()) this->self = tree.size();
                this->tags.push_back(channel::setup().sid);
                this->tree.push_back(rank);
            }
        }
        generate_sid();
    }

    inline bool collective<typename channel::block_type>::involved(){
        return states[bind::rank()] || states.back();
    }

    inline bool collective<typename channel::block_type>::test(){
        if(this->once()){
            if(states.back()){
                this->size = bind::num_procs();
                this->list = &channel::setup().circle[root];
                this->self = (size + bind::rank() - root) % size;
            }else{
                this->size = tree.size();
                this->list = &tree[0];
            }
            this->dispatch();
        }
        return this->impl();
    }

    inline collective<typename channel::scalar_type>::collective(typename channel::scalar_type& v, rank_t root)
    : bcast<typename channel::scalar_type>(v, root) {
        tags.reserve(bind::num_procs()+1);
        for(int i = 0; i <= bind::num_procs(); i++)
            this->tags.push_back(generate_sid());
    }

    inline bool collective<typename channel::scalar_type>::test(){
        if(this->once()){
            this->size = bind::num_procs();
            this->list = &channel::setup().circle[root];
            this->self = (size + bind::rank() - root) % size;
            this->dispatch();
        }
        return this->impl();
    }

} } }
