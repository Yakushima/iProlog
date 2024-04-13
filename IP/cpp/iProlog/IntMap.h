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
#include "FastUtil.h"

#include <iostream>

#include "IMap.h"

namespace iProlog {

    using namespace std;

#if 1
    class IntMap {
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

        // IntMap() : IntMap(1 << 2) { }

        // ****** Make resizing/rehashing irrelevant for now
        IntMap() : IntMap(1 << 5) { }


        IntMap(int size) : IntMap(size, 0.75f) { }
        IntMap(int size, float fillFactor);

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

#else




    // For logic programs with fewer than 256 clauses
    // and Value_ also 8 bit, this could be 16 bits
    // per hash table entry.

    // I haven't found a way to do templating right here.
    // FREE_KEY and NO_VALUE are always unresolved symbols
    // at link time.
    // I think C++ 20's "concepts lite" may offer a way through.
#ifdef TEMPL_INTMAP
    template <class Key_, class Value_>
#else
    typedef cell Key_;
    typedef ClauseNumber Value_;

    class IntMap {
#endif

        // For short logic programs, IntMaps are already pretty bulky
        // compared to their actual indexing maps.
        // Parameterizing for different kinds of maps
        // has an additional cost.
        // Unsatisfactory, but solvable with C++ "concept"?
        // If I stick with m_data size = 2^n, what are some per-instance
        // members now could be separated into a "hash table header" class
        // and each IntMap could have an index into  a table of these.
    public:
        static Value_ NO_VALUE;
        static Key_   FREE_KEY;
    public:
        static Value_ no_value() { return NO_VALUE; }
        static Key_   free_key() { return FREE_KEY; }

        static void init(Key_ free_key_, Value_ no_value_) {
            FREE_KEY = free_key_;
            NO_VALUE = no_value_;
        }

    private:
        /* eventually refactor to this, with "Key key" */
        struct kv_pair {
            Key_   key;
            Value_ val;
        };

        /** "Keys and values" */
        /** (Alternating k0,v0,k1,v1....) */
        std::vector<int> m_data;

        /** "Do we have 'free' key in the map?" */
        bool m_hasFreeKey;
        /** Value of 'free' key */
        Value_ m_freeValue;

        /** "Fill factor, must be between (0 and 1)" */
        float m_fillFactor;
        /** "We will resize a map once it reaches this size" */
        int m_threshold;
        /** "Current map size" */
        /** This is # of entries, not # of ints */
        int m_size;

        /** "Mask to calculate the original position" */
        /** (m_stride == 1 i.e., kv_pair array, will make these two equal */
        size_t m_mask;
        size_t m_mask2;

        /** Switch for int/kv_pair array traversal, allocation, etc. */
        static const int m_stride = 2; // 2=int array, 1=kv_pair array

        inline void move_to_next_entry(int& p) const {
            p = (p + m_stride) & m_mask2;  // masking causes wraparound indexing
        }
        inline void make_masks(size_t cap) { // arg must be 2^n for some n
            m_mask = cap / m_stride - 1;
            m_mask2 = cap - 1;
        }
        inline void alloc(size_t cap) {
            m_data = vector<int>(cap * m_stride);
        }


        // newCapacity should be 2^n for some n

        void rehash(size_t newCapacity) {
            m_threshold = (int)(newCapacity / 2 * m_fillFactor);
            make_masks(newCapacity);

            size_t      oldCapacity = m_data.capacity();
            vector<int> oldData = m_data;

            alloc(newCapacity);
            m_size = m_hasFreeKey ? 1 : 0;

            for (int i = 0; i < oldCapacity; i += m_stride) {
                Key_ oldKey(oldData[i]);
                if (!is_free(oldKey)) {
                    put(oldKey, Value_(oldData[i + 1]));
                }
            }
        }

        inline void maybe_resize() {
            if (m_size >= m_threshold)
                rehash(size_t(m_data.capacity() * 2)); // double each time; "size is set inside"
            else
                ++m_size;
        }
        inline void set_kv(int p, Key_ k, Value_ v) {
            m_data[p] = k.as_int();
            m_data[p + 1] = v.as_int();
        }

        inline void  set_k(int p, Key_ k) { m_data[p] = k.as_int(); }
        inline Key_   get_k(int p) const { Key_ k(m_data[p]); return k; }
        inline void  set_v(int p, Value_ v) { int vx = v.as_int();  m_data[p + 1] = vx; }
        inline Value_ get_v(int p) const { return m_data[p + 1]; }

        inline int hash_pos(Key_ key) const {
            int k = key.as_int();
            return (FastUtil::phiMix(k) & m_mask) << (m_stride - 1);
        }

    public:

        inline Key_ get_key_at(int p) const { return get_k(p); }

        inline int size() const { return m_size; }

        // inline int& operator[](int i)

#if 0
        Value_ get(Key_ key) const;
        Value_ put(Key_ key, Value_ value);
        Value_ remove(Key_ key);
#endif
        inline bool add(Key_ key) { return no_value() != put(key, no_value()); } // was 666
        inline bool contains(Key_ key) const { return no_value() != get(key); }
        inline bool retract(Key_ key) { return no_value() != remove(key); }
        inline bool isEmpty() const { return 0 == m_size; }

        // some kind of inlined "bool get_next_key(int &p, Key_ &k)" semi-iterator
        // would be better than this -- it could hide these three, until the
        // kv_pair rewrite is worked out for IntMap.
        inline size_t capacity() const { return m_data.capacity(); }
        inline int stride() const { return m_stride; }

        bool is_free(Key_ k) const { return k == free_key(); }
#if 0
        IntMap() : IntMap(1 << 2) { cout << "IntMap()" << endl; }

        IntMap(int size) : IntMap(size, 0.75f) { cout << "IntMap(" << size << ")" << endl; };
#endif
        IntMap() {
            int size = INTMAP_INIT_SIZE;
            float fillFactor = INTMAP_FILL_FACTOR;
            if (fillFactor <= 0 || fillFactor >= 1)
                throw std::invalid_argument("FillFactor must be in (0, 1)");
            if (size <= 0)
                throw std::invalid_argument("Size must be positive!");

            int capacity = FastUtil::arraySize(size, fillFactor);
            make_masks(capacity);
            m_fillFactor = fillFactor;
            TRY{
                // cout << "////// about to call alloc in IntMap with capacity=" << capacity << endl;
                alloc(capacity);
            } CATCH("in IntMap constructor on alloc " + capacity)
            
            m_threshold = (int)(capacity * fillFactor);
            m_size = 0; // added -- MT
            m_hasFreeKey = false;

            cout << "IntMap ctor:" << endl;
            cout << "   m_size=" << m_size << endl;
            cout << "   m_threshold=" << m_threshold << endl;
            cout << "   m_hasFreeKey=" << m_hasFreeKey << endl;
            cout << "   m_data.size()=" << m_data.size() << endl;
        }

        Value_ get(Key_ key) const {
            if (is_free(key))
                return m_hasFreeKey ? m_freeValue : no_value();

            int ptr = hash_pos(key);
            Key_ k = get_k(ptr);

            if (is_free(k))
                return no_value(); // "end of chain already"
            if (k == key) // "we check FREE prior to this call" ???
                return (Value_)get_v(ptr);

            while (true) {
                move_to_next_entry(ptr);
                k = get_k(ptr);
                if (is_free(k))
                    return no_value();
                if (k == key)
                    return get_v(ptr);
            }
        }

        Value_ put(Key_ key, Value_ value) {
            if (is_free(key)) {
                Value_ ret = m_freeValue;
                if (!m_hasFreeKey)
                    ++m_size;
                m_hasFreeKey = true;
                m_freeValue = value;
                return ret;
            }

            int ptr = hash_pos(key);
            Key_ k = get_k(ptr);
            if (is_free(k)) { // "end of chain already"
                set_kv(ptr, key, value);
                maybe_resize();
                return no_value();
            }
            else
                if (k == key) { // "we check FREE prior to this call"
                    Value_ ret = get_v(ptr);
                    set_v(ptr, value);
                    return ret;
                }

            while (true) {
                move_to_next_entry(ptr); //that's next index calculation
                k = get_k(ptr);
                if (is_free(k)) {
                    set_kv(ptr, key, value);
                    maybe_resize();
                    return no_value();
                }
                else
                    if (k == key) {
                        Value_ ret = get_v(ptr);
                        set_v(ptr, value);
                        return ret;
                    }
            }
            cout << endl;
        }

        Value_ remove(Key_ key) {
            if (is_free(key)) {
                if (!m_hasFreeKey)
                    return no_value();
                m_hasFreeKey = false;
                --m_size;
                return m_freeValue; //value is not cleaned
            }

            int ptr = hash_pos(key);
            Key_ k = get_k(ptr);
            if (k == key) { // "we check FREE prior to this call" ???
                Value_ res = get_v(ptr);
                shiftKeys(ptr);
                --m_size;
                return res;
            }
            else
                if (is_free(k))
                    return no_value(); // "end of chain already"

            while (true) {
                move_to_next_entry(ptr);
                k = get_k(ptr);
                if (k == key) {
                    Value_ res = get_v(ptr);
                    shiftKeys(ptr);
                    --m_size;
                    return res;
                }
                else
                    if (is_free(k))
                        return no_value();
            }
        }

        void shiftKeys(int pos) {
            // "Shift entries with the same hash."
            int last, slot;
            Key_ k;

            while (true) {
                last = pos;
                move_to_next_entry(pos);
                while (true) {
                    k = get_k(pos);
                    if (is_free(k)) {
                        set_k(last, free_key());
                        return;
                    }
                    slot = hash_pos(k);
                    if (last <= pos ? (last >= slot || slot > pos) : (last >= slot && slot > pos)) {
                        break;
                    }
                    move_to_next_entry(pos);
                }
                set_kv(last, k, get_v(pos));
            }
        }

        string toString() {
            //return java.util.Arrays.toString(m_data);
            string b = string("{");
            size_t l = m_data.size() / m_stride;
            int first = true;
            for (int i = 0; i < l; i += m_stride) {
                Key_ k = get_k(i);
                if (!is_free(k)) {
                    if (!first) {
                        b.append(",");
                    }
                    first = false;
                    b.append(to_string(k.as_int()));
                }
            }
            b.append("}");
            return b;
        }
    };
}
#endif


