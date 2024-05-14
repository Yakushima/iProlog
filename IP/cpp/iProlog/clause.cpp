/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "defs.h"
#include "cell.h"
#include "index.h"
#include "clause.h"
#include "spine.h"

namespace iProlog {

    using namespace std;

    Clause::Clause(int len_0, unfolding hga_0, size_t hg_len_0, int base_0, int neck_0) {
        hga = hga_0;
        hg_len = hg_len_0;
        if (hg_len == 0) abort();
        base = base_0;
        len = len_0;
        neck = neck_0;
        for (int i = 0; i < IV_LEN; ++i)
            index_vector[i] = cell::null();
    }
}
