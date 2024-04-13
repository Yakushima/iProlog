


#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "cell.h"
#include "index.h"
#include "IMap.h"

namespace iProlog {
#if 0
	Key_ IntMap::FREE_KEY = Key_(0);
	Value_ IntMap::NO_VALUE = Value_(0);

#ifdef TEMPL_INTMAP

	template class IntMap<ClauseNumber, cell>;
#endif

#endif

    void IntMap::alloc(size_t cap) {
        m_data = vector<int>(cap * stride());
    }


    IntMap::IntMap(int size, float fillFactor) {
        if (fillFactor <= 0 || fillFactor >= 1)
            throw new logic_error("FillFactor must be in (0, 1)");
        if (size <= 0)
            throw new logic_error("Size must be positive!");
        int capacity = arraySize(size, fillFactor);
        alloc(capacity);
        m_mask = capacity - 1;
        m_mask2 = capacity * 2 - 1;
        m_fillFactor = fillFactor;

        m_data_length = capacity * 2;
        m_data = vector<int>(m_data_length);

        m_threshold = (int)(capacity * fillFactor);
        m_size = 0;
    }

    void IntMap::rehash(int newCapacity) {
        cout << "                      !!!!!!!!!!!!! calling rehash !!!!!!!!!!!!!!!!!!" << endl;
        m_threshold = (int)(newCapacity / 2 * m_fillFactor);
        m_mask = newCapacity / 2 - 1;
        m_mask2 = newCapacity - 1;

        int oldCapacity = m_data_length;
        vector<int> oldData = m_data;

        m_data = vector<int>(newCapacity);
        m_size = m_hasFreeKey ? 1 : 0;

        for (int i = 0; i < oldCapacity; i += 2) {
            int oldKey = oldData[i];
            if (oldKey != FREE_KEY) {
                put(oldKey, oldData[i + 1]);
            }
        }
    }

    int IntMap::get(int key) {
#define TR if(0)
        int ptr = (phiMix(key) & m_mask) << 1;

        TR cout << "      IntMap::get(" << key << "), ptr=" << ptr << " m_data.size()=" << m_data.size() << " m_mask=" << m_mask << endl;

        if (key == FREE_KEY) {
            int r = m_hasFreeKey ? m_freeValue : NO_VALUE;
            TR cout << "       key==FREE_KEY m_hasFreeKey=" << m_hasFreeKey << endl;
            return r;
        }
        int k = m_data[ptr];

        if (k == FREE_KEY) {
            TR cout << "       k==m_data[" << ptr << "]==" << k << " == FREE_KEY" << ", m_data[ptr + 1] = " << m_data[ptr + 1] << endl;
            return NO_VALUE; //end of chain already
        }

        if (k == key) { //we check FREE prior to this call
            TR cout << "       k==key, returning m_data[" << ptr + 1 << "]=" << m_data[ptr + 1] << endl;
            return m_data[ptr + 1];
        }

        while (true) {
            ptr = ptr + 2 & m_mask2; //that's next index
            k = m_data[ptr];
            if (k == FREE_KEY) {
                TR cout << "       k=m_data[" << ptr << "]==FREE_KEY " << FREE_KEY << " m_data[ptr+1]="<< m_data[ptr+1] << endl;
                return NO_VALUE;
            }
            if (k == key) {
                TR cout << "       k==key at end of chain==" << k << endl;
                return m_data[ptr + 1];
            }
        }
#undef TR
    }

    int IntMap::put(int key, int value) {
#define TR if(0)
        TR cout << "       IntMap::put(key=" << key << ", value=" << value << ")" << endl;

        if (key == FREE_KEY) {
            int ret = m_freeValue;
            if (!m_hasFreeKey) {
                ++m_size;
            }
            m_hasFreeKey = true;
            m_freeValue = value;
            TR cout << "         (1) key == FREE_KEY, returning " << ret << endl;
            return ret;
        }

        int ptr = (phiMix(key) & m_mask) << 1;
        int k = m_data[ptr];
        if (k == FREE_KEY) //end of chain already
        {
            m_data[ptr] = key;
            m_data[ptr + 1] = value;
            if (m_size >= m_threshold) {
                TR cout << "**** about to call rehash, m_size=" << m_size << " m_threshold=" << m_threshold << endl;
                rehash(m_data_length * 2); //size is set inside
            }
            else {
                ++m_size;
            }
            TR cout << "         (2) k == FREE_KEY, returning NO_VALUE" << endl;
            return NO_VALUE;
        }
        else if (k == key) //we check FREE prior to this call
        {
            int ret = m_data[ptr + 1];
            m_data[ptr + 1] = value;
            TR cout << "         (3) k == key, m_data[ptr+1]<-value==" << m_data[ptr+1] << " returning past value " << ret << endl;
            return ret;
        }

        while (true) {
            ptr = ptr + 2 & m_mask2; //that's next index calculation
            k = m_data[ptr];
            if (k == FREE_KEY) {
                m_data[ptr] = key;
                m_data[ptr + 1] = value;
                if (m_size >= m_threshold) {
                    TR cout << "     needing to rehash" << endl;
                    rehash(m_data_length * 2); //size is set inside
                }
                else {
                    ++m_size;
                }
                TR cout << "         (4) at end of chain, returning NO_VALUE" << endl;
                return NO_VALUE;
            }
            else if (k == key) {
                int ret = m_data[ptr + 1];
                m_data[ptr + 1] = value;
                TR cout << "         (5) found something, returning " << ret << endl;
                return ret;
            }
        }
    }

#if 0 // not called
    int IntMap::remove(int key) {
        if (key == FREE_KEY) {
            if (!m_hasFreeKey)
                return NO_VALUE;
            m_hasFreeKey = false;
            --m_size;
            return m_freeValue; //value is not cleaned
        }

        int ptr = (phiMix(key) & m_mask) << 1;
        int k = m_data[ptr];
        if (k == key) //we check FREE prior to this call
        {
            int res = m_data[ptr + 1];
            shiftKeys(ptr);
            --m_size;
            return res;
        }
        else if (k == FREE_KEY)
            return NO_VALUE; //end of chain already
        while (true) {
            ptr = ptr + 2 & m_mask2; //that's next index calculation
            k = m_data[ptr];
            if (k == key) {
                int res = m_data[ptr + 1];
                shiftKeys(ptr);
                --m_size;
                return res;
            }
            else if (k == FREE_KEY)
                return NO_VALUE;
        }
    }

    int IntMap::shiftKeys(int pos) {
        // Shift entries with the same hash.
        int last, slot;
        int k;
        vector<int> data = m_data;
        while (true) {
            pos = (last = pos) + 2 & m_mask2;
            while (true) {
                if ((k = data[pos]) == FREE_KEY) {
                    data[last] = FREE_KEY;
                    return last;
                }
                slot = (phiMix(k) & m_mask) << 1; //calculate the starting slot for the current key
                if (last <= pos ? last >= slot || slot > pos : last >= slot && slot > pos) {
                    break;
                }
                pos = pos + 2 & m_mask2; //go to the next entry
            }
            data[last] = k;
            data[last + 1] = data[pos + 1];
        }
    }
#endif

    /** Taken from FastUtil implementation */

/** Return the least power of two greater than or equal to the specified value.
 *
 * <p>Note that this function will return 1 when the argument is 0.
 *
 * @param x a long integer smaller than or equal to 2<sup>62</sup>.
 * @return the least power of two greater than or equal to the specified value.
 */
    long IntMap::nextPowerOfTwo(long x) {
        if (x == 0)
            return 1;
        x--;
        x |= x >> 1;
        x |= x >> 2;
        x |= x >> 4;
        x |= x >> 8;
        x |= x >> 16;
        return (x | x >> 32) + 1;
    }

    /** Returns the least power of two smaller than or equal to 2<sup>30</sup>
     * and larger than or equal to <code>Math.ceil( expected / f )</code>.
     *
     * @param expected the expected number of elements in a hash table.
     * @param f the load factor.
     * @return the minimum possible size for a backing array.
     * @throws IllegalArgumentException if the necessary size is larger than 2<sup>30</sup>.
     */
    int IntMap::arraySize(int expected, float f) {
        long s = std::max(2L, IntMap::nextPowerOfTwo((long)ceil(expected / f)));
        if (s > 1 << 30) {
            string emsg = "Too large " + expected;
            // emsg += " expected elements with load factor " + f;
            throw new logic_error(emsg);
        }
        return (int)s;
    }

    //taken from FastUtil
    static const int INT_PHI = 0x9E3779B9;

    int IntMap::phiMix(int x) {
        int h = x * INT_PHI;
        return h ^ h >> 16;
    }

    // @Override
string IntMap::show() const {
    //return java.util.Arrays.toString(m_data);
    string b = "{";
    int l = m_data_length;
    bool first = true;
    for (int i = 0; i < l; i += 2) {
        int v = m_data[i];
        if (v != FREE_KEY) {

            if (!first) {
                b += ", ";
            }
            first = false;
            b += "[" + to_string(i);
            b += "]:#" + to_string(v);
            b += "->c" + to_string(m_data[i + 1]);
        }
    }
    b += "}";
    return b;
}
}