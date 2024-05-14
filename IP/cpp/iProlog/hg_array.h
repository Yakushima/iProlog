#pragma once

#include "defs.h"
#include "cell.h"
#include <vector>

namespace iProlog {
	using namespace std;

#ifdef RAW_HG_ARR
#define hga_data(hgax) hgax
	typedef cell* hg_array;
#else
#define hga_data(hgax) hgax.data()
	typedef vector<cell> hg_array;
#endif
}

