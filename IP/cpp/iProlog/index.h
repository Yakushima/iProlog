#pragma once
/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

using namespace std;

#include <vector>
#include <array>
#include "cell.h"

namespace iProlog {


    typedef array<cell, MAXIND> t_index_vector; // deref'd cells
}

#include "IMap.h"
#include "clause.h"
#include "spine.h"
#include "CellStack.h"

namespace iProlog {

class Spine;
class Engine;

#define BAD_VAL 0

class index {

public:

  /* "For each argument position in the head of a clause
   * (up to a maximum that can be specified by the programmer [MAXIND])
   * it associates to each indexable element(symbol, number or arity)
   * the set of clauses [indicated by clause number]
   * where the indexable element occurs in that argument position."
   */
    vector<IMap> imaps;

  /* "The clauses having variables in an indexed argument position are also
   * collected in a separate set for each argument position."
   */
    vector<cls_no_set> var_maps;

    long n_matches;

    index(CellStack &heap, vector<Clause>& clauses);

#define COUNTING_MATCHES

    bool possible_match(const Spine *sp,
                        const Clause& cl)
#ifndef COUNTING_MATCHES
                                                    const
#endif
                                                         ; 

    void put(const t_index_vector& iv, ClauseNumber clause_no);

    void makeIndexArgs(CellStack &heap, Spine *G, cell goal);

    vector<ClauseIndex> matching_clauses_(t_index_vector& iv);

    static inline ClauseNumber to_clause_no(ClauseIndex i) { ClauseNumber x(i.as_int()+1); return x; }

    static inline bool is_cls_no(ClauseNumber cls_no) { return cls_no.as_int() != 0; }

    static inline ClauseIndex  to_clause_idx(ClauseNumber cl_no) { return cl_no.dec(); }

    t_index_vector getIndexables(CellStack &heap, cell goal);

    cell cell2index(CellStack &, cell c) const;

    string show(const t_index_vector& iv) const;

    string show_index() const;
};

} // namespace
