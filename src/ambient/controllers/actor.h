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

#ifndef AMBIENT_CONTROLLERS_ACTOR
#define AMBIENT_CONTROLLERS_ACTOR

namespace ambient {


    class actor {
    protected:
        typedef controllers::controller controller_type;
        actor(){}
    public:
       ~actor();
        actor(scope::const_iterator it);
        bool remote() const;
        bool local()  const;
        bool common() const;
        rank_t which()  const;
        friend class context_mt;
        friend class backbone;
    protected:
        int factor;
        int round;
        rank_t rank;
        ambient::locality state;
        controller_type* controller;
    };

    class actor_common : public actor {
    public:
        actor_common();
    };

    class actor_auto : public actor {
    public:
        actor_auto(typename actor::controller_type* c);
        void set(rank_t r);
        void set(scope::const_iterator it);
        void schedule();
        void intend_read(model::revision* o);
        void intend_write(model::revision* o);
        mutable std::vector<rank_t> stakeholders;
        mutable std::vector<int> scores;
    };

}

#endif
