#pragma once
/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "spine.h"
#include "hg_array.h"

namespace iProlog {
    using namespace std;

    /**
     * "representation of a clause" [Clause.java].
     */

    struct Clause {
        // Skeletal elements for compiled form:

        int len;            // length of heap slice
        hg_array hga;    // "head+goals pointing to cells in clauses"
        int hg_len;       // (for RAW_HG_ARR  pointer implementation)
        int base;           // the point in the heap where this clause starts
        int neck;           // first after the end of the head (=length of the head)
        t_index_vector index_vector; // indexables in head. In the video, this is described as
                          // "the index vector containing dereferenced constants,
                          // numbers or array sizes as extracted from the outermost
                          // term of the head of the clause, with zero values
                          // marking variable positions."
                          // Should it be "outermost termS"?

        Clause() : len(size_t(0)), hg_len(0), base(size_t(0)), neck(size_t(0)) {
            for (int i = 0; i < IV_LEN; ++i) index_vector[i] = cell::null();
        }

        Clause(int len_0, hg_array hga, size_t hg_len, int base_0, int neck_0);
    };

} // end namespace
