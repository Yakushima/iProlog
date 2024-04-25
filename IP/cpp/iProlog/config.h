#pragma once

// #define NDEBUG
#include <assert.h>

#include "Inty.h"

namespace iProlog {
	const int MIN_HEAP_SIZE = 1 << 10;

#define COUNTING_MATCHES

	// For tuning speed/space tradeoffs.
	// See https://en.cppreference.com/w/cpp/types/integer

	typedef int cell_int;		// could go down to int_least_16_t
	typedef int cls_no_int;		// could go down to uint_least8_t
								// for logic programs with <256 clauses

	typedef int sym_idx_int;

	// Tarau's code uses Java Integer handles as hash keys,
	// and unlike C++, Java obligingly hashes on them in its
	// hash table collections. I have to sort of fake that.
	// thus the "hashable Integer pointer int" type here.

	typedef int hashable_Integer_ptr_int;

	typedef Inty<cls_no_int> ClauseNumber; /* 1 ... clause array size */

	typedef Inty<cls_no_int> ClauseIndex;

	/* RAW=true says to go with a less-safe, faster implementation
	 *    than STL vectors, with no bounds check, and less header info, to
	 *    save a little space. The fast-copy cell heap-to-heap relocation may
	 *    end up in this class eventually.
	 * "if (is_raw)..." ensures that code code gets checked by the
	 * compiler, except there there's some #ifdef conditional
	 * compilation. Constant-folding and dead code elimination
	 * does the rest.
	 */
// #define RAW
#ifdef RAW
	const bool is_raw = true;
#else
	const bool is_raw = false;
#endif

	// Some way to pass these on compiler command line????

	const int MAXIND = 1;       // "number of index args" [Engine.java]
	const int START_INDEX = 1;	// "if # of clauses < START_INDEX,
								// turn off indexing" [Engine.java]

	const bool indexing = (MAXIND > 0);

	const int INTMAP_INIT_SIZE = (1 << 2);
	const float INTMAP_FILL_FACTOR = 0.75f;

#define USE_SIGN_BIT false;

	const bool integrity_check = false;
	void checkit_();
	inline void checkit() { if (integrity_check) checkit_(); }

#define COUNTING_MATCHES

#define TRY_CATCHING

}