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

    Clause::Clause(int len_0, goals_list skeleton_0, size_t skel_size_0, int base_0, int neck_0) {
        skeleton = skeleton_0;
        skeleton_size = skel_size_0;
        if (skel_size_0 == 0) abort();
        base = base_0;
        len = len_0;
        neck = neck_0;
        for (int i = 0; i < IV_LEN; ++i)
            index_vector[i] = cell::null();
    }
}
