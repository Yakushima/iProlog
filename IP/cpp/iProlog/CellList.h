#pragma once
/*
* iProlog / C++  [derived from Java version]
* License: Apache 2.0
* Copyright(c) 2017 Paul Tarau
*/

#include "defs.h"
#include <assert.h>

#include "cell.h"

namespace iProlog {

    using namespace std;

#ifdef RAW_CELL_LIST
#define CL_p CellList *
#else
#define CL_p shared_ptr<CellList>
#endif

    class CellList {
        static CL_p free_list;

    private:
        cell head_;
        CL_p tail_;
       
    public:
        static int n_alloced;

        // Singleton list
        CellList() { head_ = 0; tail_ = nullptr; ++n_alloced; }
  
        CellList(cell h) : head_(h), tail_(nullptr) { ++n_alloced; }

        ~CellList() { --n_alloced; }

        static int alloced();

        inline static bool isEmpty(const CL_p Xs) { return nullptr == Xs; }

        inline static cell head(const CL_p Xs) {
            assert(Xs != nullptr);
            return Xs->head_;
        }

        inline static CL_p tail(const CL_p Xs) {
            return Xs->tail_;
        }

        inline static CL_p mk_shared(cell X) {
            if (free_list == nullptr) {
#ifdef RAW_CELL_LIST
                return new CellList(X);
#else
                ++n_alloced;
                return make_shared<CellList>(X);
#endif
            }
            CL_p r = free_list;
            free_list = tail(r);
            r->tail_ = nullptr;
            r->head_ = X;
            return r;
        }

        inline static CL_p mk_shared() {
            return mk_shared(cell::BAD);
        }

        inline static CL_p collect_first(CL_p to_dump) {
            CL_p tl = to_dump->tail_;
            to_dump->tail_ = free_list;
            free_list = to_dump;
            return tl;
        }

        inline static CL_p cons(cell X, /*const*/CL_p Xs) {
            CL_p cl = mk_shared(X);
            cl->tail_ = Xs;
            return cl;
        }

        // O(n)
        size_t size() const {
            if (this == nullptr) return 0;
            size_t sum = 1;
            for (CL_p p = tail_; p != nullptr; p = p->tail_)
                ++sum;
            return sum;
        }

        string toString();
    };

} // end namespace
