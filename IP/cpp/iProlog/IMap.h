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
#include "Integer.h"

#define UOMAP  // should be in config.h

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
#ifdef UOMAP
public:
    unordered_map<cell, cls_no_to_cell, CellHash> map;
#else
        static const unsigned NBUCKETS_2_exp = 2;
        static const int NBUCKETS = 1 << NBUCKETS_2_exp;
        static const int NBUCKETS_mask = NBUCKETS - 1;

         static int phash(const Integer *s) {
             // size_t x = (size_t) s;
             // return NBUCKETS_mask  & ((x >> 10) ^ (x >> 2));
             // cout << "                 phash: s->as_int() =" << s->as_int() << endl;
             return NBUCKETS_mask & (s->as_int() ^ (s->as_int() >> 8));
         }

         /* I might have this wrong. The IMap bucket should
         * perhaps contain a list of cell_box / cls_no_to pairs
         * rather than just being such a pair.
         */
         struct bucket {
             Integer*       cell_box;
             cls_no_to_cell cls_no_2_cell;

             bucket() : cell_box(nullptr) { cls_no_2_cell = cls_no_to_cell(); }
             bucket(Integer* ip, cls_no_to_cell im) : cell_box(ip), cls_no_2_cell(im) { }
             inline bool uninit() const { return cell_box == nullptr;  }

             string show() const {
                 string s = "<";
                 if (cell_box == nullptr)
                     s += "_";
                 else
                     s += to_string(cell_box->as_int());
                 s += ":" + cls_no_2_cell.show();
                 s += ">";
                 return s;
             }
         };
         array<bucket, NBUCKETS> map;
#endif

public:
#ifdef UOMAP
    IMap() {
        cout << "================ Entered IMap ctor ===============" << endl;
        map.reserve(NBUCKETS);
    }
#endif
      // static vector<IMap> create(int l);

    static vector<IMap> create(int l);

      bool put(Integer* cls_no_box, cell this_cell);
      cls_no_to_cell get_cls_no_to_cell(Integer* cellbox);
      size_t amount() const;

// refactor out in a subclass, for micro version
// or keep it here, but conditionally compiled for that version:

      string toString() const;
      string show() const;
      static string show(const vector<IMap> &imaps);
#ifndef UOMAP
      static string show(const bucket &b);
#endif
      static string show(const vector<Integer *> is);
};
} // end namespace
