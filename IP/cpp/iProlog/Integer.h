#pragma once
// Had this class (Java code) and it relates to
// a need for a unique (pointer) handle for hashing.
// But the reason for these "boxed" values is that
// the low order bits of the pointer is supposedly
// fairly random. This may not be the case. Java
// has automatic hashing of pointers for its collection
// classes, but this is tricky to implement in C++.
// Some kind of pseudorandom number generator would
// probably be better.

#include "Inty.h"

#include <iostream>

namespace iProlog {
	using namespace std;
	class Integer : public Inty<hashable_Integer_ptr_int> {
	public:
		inline Integer() : Inty<hashable_Integer_ptr_int>() {  }
		inline Integer(int x) : Inty<hashable_Integer_ptr_int>(x) {  }
		inline Integer(Inty x) : Inty<hashable_Integer_ptr_int>(x.as_int()) {  }

		inline Integer& operator =(const hashable_Integer_ptr_int &x) { return *this = x; }
		bool operator ==(const Integer &x) const { return as_int() == x.as_int(); }
		inline operator unsigned long () { return (unsigned long) as_int(); }
	};
}
