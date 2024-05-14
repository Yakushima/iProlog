#pragma once

#include "defs.h"
#include "cell.h"
#include <vector>

namespace iProlog {
	using namespace std;

#ifdef RAW_HG_ARR
#define hga_data(hgax) hgax
	typedef cell* unfolding;
#else
#define hga_data(hgax) hgax.data()
	typedef vector<cell> unfolding;
#endif
}

