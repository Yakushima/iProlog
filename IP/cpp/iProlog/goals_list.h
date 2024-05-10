#pragma once

#include "defs.h"
#include "cell.h"
#include <vector>

namespace iProlog {
	using namespace std;
#ifdef RAW_GOALS_LIST
	typedef cell* goals_list;
#else
	typedef vector<cell> goals_list;
#endif
}
