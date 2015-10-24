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

#ifndef BIND
#define BIND
#ifndef NDEBUG
#define NDEBUG
#define BIND_NO_DEBUG
#endif

#ifndef BIND_INSTR_BULK_CHUNK
#define BIND_INSTR_BULK_CHUNK     16777216 // 16 MB
#endif
#ifndef BIND_DATA_BULK_CHUNK
#define BIND_DATA_BULK_CHUNK      67108864 // 64 MB
#endif
#ifndef BIND_COMM_BULK_CHUNK
#define BIND_COMM_BULK_CHUNK      67108864 // 64 MB
#endif

// {{{ system includes
#ifdef BIND_MPI
#include <mpi.h>
#endif
#include <complex>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string>
#include <limits>
#include <vector>
#include <stack>
#include <set>
#include <map>
#include <list>
#include <memory.h>
#include <stdarg.h>
#include <ctype.h>
#include <iostream>
#include <fstream>
#include <sys/time.h>
#include <algorithm>
#include <execinfo.h>
#include <stdexcept>
#include <type_traits>
#include <functional>
#include <utility>
#include <atomic>
#include <tuple>
#include <sys/mman.h>
#include <chrono>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
// }}}
// {{{ utils package
#include "utils/rss.hpp"
#include "bind/utils/index_sequence.hpp"
#include "bind/utils/io.hpp"
#include "bind/utils/mutex.hpp"
#include "bind/utils/rank_t.hpp"
#include "bind/utils/dim2.hpp"
#include "bind/utils/env.hpp"
#include "bind/utils/guard_once.hpp"
// }}}
// {{{ memory package
#include "bind/memory/types.h"
#include "bind/memory/factory.hpp"
#include "bind/memory/region.hpp"
    // {{{ memory::cpu package
    #include "bind/memory/cpu/bulk.h"
    #include "bind/memory/cpu/data_bulk.hpp"
    #include "bind/memory/cpu/comm_bulk.hpp"
    #include "bind/memory/cpu/instr_bulk.hpp"
    #include "bind/memory/cpu/standard.hpp"
    #include "bind/memory/cpu/fixed.hpp"
    #include "bind/memory/cpu/new.hpp"
    // }}}
    // {{{ memory::gpu package
    // }}}
#include "bind/memory/delegated.h"
#include "bind/memory/descriptor.hpp"
// }}}
// {{{ model package
#include "bind/model/locality.hpp"
#include "bind/model/functor.hpp"
#include "bind/model/revision.hpp"
#include "bind/model/history.hpp"
#include "bind/model/transformable.hpp"
// }}}
// {{{ transport package (requires :model)
#ifdef BIND_MPI
#define BIND_CHANNEL_NAME mpi
#include "bind/transport/mpi/group.hpp"
#include "bind/transport/mpi/tree.hpp"
#include "bind/transport/mpi/channel.h"
#include "bind/transport/mpi/request.h"
#include "bind/transport/mpi/collective.h"

#include "bind/transport/mpi/request.hpp"
#include "bind/transport/mpi/channel.hpp"

namespace bind {
    inline rank_t rank();
    inline int num_procs();
    inline int get_sid();
    inline int generate_sid();
}

#include "bind/transport/mpi/collective.hpp"
#else
#include "bind/transport/nop/channel.hpp"
#endif
// }}}
// {{{ core package (requires :model :transport)
#include "bind/core/collector.h"
#include "bind/core/collector.hpp"

#include "bind/core/scope.h"
#include "bind/core/actor.h"

#include "bind/core/controller.h"
#include "bind/core/get.h"
#include "bind/core/set.h"
#include "bind/core/controller.hpp"
namespace bind {
    inline rank_t which(){
        return bind::select().get_actor().which();
    }
}
#include "bind/core/get.hpp"
#include "bind/core/set.hpp"

#include "bind/core/scope.hpp"
#include "bind/core/actor.hpp"
// }}}
// {{{ interface package (requires :model :transport :core)
#include "bind/utils/auxiliary.hpp"
#include "bind/interface/typed.hpp"
#include "bind/interface/allocator.hpp"
#include "bind/interface/kernel_inliner.hpp"
#include "bind/interface/kernel.hpp"
#include "bind/interface/access.hpp"
#include "bind/interface/lambda.hpp"
// }}}
// {{{ bonus utils package
#include "bind/utils/timer.hpp"
// }}}
#ifdef BIND_NO_DEBUG
#undef NDEBUG
#endif
#endif
