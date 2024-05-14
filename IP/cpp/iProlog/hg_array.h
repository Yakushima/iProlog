#pragma once

#include "defs.h"
#include "cell.h"
#include <vector>

namespace iProlog {
	using namespace std;
#ifdef RAW_HG_ARR
	typedef cell* hg_array;
#else
	typedef vector<cell> hg_array;
#endif
}

