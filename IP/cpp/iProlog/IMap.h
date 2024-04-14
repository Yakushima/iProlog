#pragma once
/*
* iProlog / C++  [derived from Java version]
* License: Apache 2.0
* Copyright(c) 2017 Paul Tarau
*/

// for hashing implementation with a template, see:
//  https://marknelson.us/posts/2011/09/03/hash-functions-for-c-unordered-containers.html

#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include <unordered_map>
#include "cell.h"
#include "index.h"
#include "IntMap.h"

namespace iProlog {

    using namespace std; 

    static const unsigned NBUCKETS_2_exp = 2;
    static const int NBUCKETS = 1 << NBUCKETS_2_exp;
    static const int NBUCKETS_mask = NBUCKETS - 1;

    struct CellHash
    {
        std::size_t operator()(const cell& s) const noexcept
        {
            return NBUCKETS_mask & (s.as_int() ^ (s.as_int() >> 8));
        }
    };


    // 3rd arg: free_key, 4th no_value
    // currently cell value is initialized with 666
    // this could overlap valid cell values
    typedef IntMap
#ifdef TEMPL_INTMAP
        <cell, ClauseNumber>
#endif
                    cls_no_to_cell;

class IMap {
public:
    unordered_map<cell, cls_no_to_cell, CellHash> map;

    static const unsigned NBUCKETS_2_exp = 2;
    static const int NBUCKETS = 1 << NBUCKETS_2_exp;
    static const int NBUCKETS_mask = NBUCKETS - 1;

    IMap() {
        map.reserve(NBUCKETS);
    }

      // static vector<IMap> create(int l);

    static vector<IMap> create(int l);

      bool put(ClauseNumber, cell this_cell);
      cls_no_to_cell get_cls_no_to_cell(cell cx);
      size_t amount() const;

// refactor out in a subclass, for micro version
// or keep it here, but conditionally compiled for that version:

      string toString() const;
      string show() const;
      static string show(const vector<IMap> &imaps);

      static string show(const vector<int> is);
};
} // end namespace
