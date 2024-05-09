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
            if (free_list == nullptr)
#ifdef RAW_CELL_LIST
                return new CellList(X);
#else
                return make_shared<CellList>(X);
#endif
            CL_p r = free_list;
            free_list = tail(r);
            return r;
        }

        inline static CL_p mk_shared() {
            return mk_shared(cell::BAD);
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

        // append CellList Ys to CellList made from int array xs, return result
        inline static CL_p concat(vector<cell> &xs, CL_p Ys) {
            int sx = int(xs.size());
            if (sx == 0)
                return Ys;
            CL_p Zs = Ys;
            for (int i = sx - 1; i >= 0; i--)
                Zs = cons(xs[size_t(i)], Zs);
            return Zs;
        }

        string toString();
    };

} // end namespace
