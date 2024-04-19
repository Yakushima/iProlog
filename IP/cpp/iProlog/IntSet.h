#pragma once

/*
 * iProlog / C++  [derived from Java version]
 * Original (Java) author: Mikhail Vorontsov
 * License: public domain
 *    https://github.com/mikvor/hashmapTest/blob/master/LICENSE
 * Java code modified by Paul Tarau
 * 
 * derived from code at https://github.com/mikvor/hashmapTest
 *
 * Article by Vorontsov:
 * https://web.archive.org/web/20170606024914/http://java-performance.info/implementing-world-fastest-java-int-to-int-hash-map/
 */

#include <cstddef>
#include <assert.h>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <system_error>
#include "defs.h"

#include <iostream>

#include "IMap.h"

namespace iProlog {

    using namespace std;

    // For short logic programs, these are already pretty bulky
    // compared to their actual indexing maps.
    // Parameterizing for different kinds of maps
    // has an additional cost.
    // Unsatisfactory, but solvable with C++ "concept"?
    // If I stick with m_data size = 2^n, what are some per-instance
    // members now could be separated into a "hash table header" class
    // and each could have an index into  a table of these.

    class IntSet {
      private:
        static const  int FREE_KEY = 0;
        static const  int NO_VALUE = 0;

        /** Keys and values */
        vector<int> m_data;
        int m_data_length;
    private:
        /** Do we have 'free' key in the map? */
        bool m_hasFreeKey;
        /** Value of 'free' key */
        int m_freeValue;

        /** Fill factor, must be between (0 and 1) */
        float m_fillFactor;
        /** We will resize a map once it reaches this size */
        int m_threshold;
        /** Current map size */
        int m_size;

        /** Mask to calculate the original position */
        int m_mask;
        int m_mask2;
    public:
        // adding for index.cpp:
        int capacity() const         { return m_data_length;         }
        int stride() const           { return 2;                     }
        inline int get_key_at(int i) { return m_data[i];             }
        inline bool is_free(int i) const { return m_data[i] == FREE_KEY; }
        inline static int no_value() { return NO_VALUE; }

        static void init(int bad_cell, int no_val) {
            // FREE_KEY = bad_cell;
            // NO_VALUE = no_val;
        }

        IntSet() : IntSet(1 << 2) { }

        // ****** Make resizing/rehashing irrelevant for now
        // IntSet() : IntSet(1 << 5) { }


        IntSet(int size) : IntSet(size, 0.75f) { }
        IntSet(int size, float fillFactor);

    public:
        // "for use as IntSet" - Paul Tarau

        inline bool contains(int key) { return NO_VALUE != get(key);         }
        inline bool add_key(int key)  { return NO_VALUE != put(key, 666);    }
        inline bool delete_(int key)  { return NO_VALUE != remove(key);      }
        inline bool isEmpty() const   { return 0 == m_size;                  }
        inline int  size()            { return m_size;                       }

        int get(int key);
        int put(int key, int value);
        int remove(int key);

    private:
        static long nextPowerOfTwo(long x);
        static int arraySize(int expected, float f);
        int shiftKeys(int pos);
        void rehash(int newCapacity);
        int phiMix(int x);
        void alloc(size_t cap);
    public:
        string show() const;
  };
}