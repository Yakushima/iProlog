#include "defs.h"
#include <assert.h>

#include "cell.h"
#include "CellList.h"

namespace iProlog {

    using namespace std;

    int CellList::n_alloced = 0;
    CL_p CellList::free_list = nullptr;

    int CellList::alloced() {
        return n_alloced;
    }
#if 0
    // push Zs CellList onto new stack, return stack (tos = last)
    vector<cell> toCells(CL_p &Xs) {
        vector<cell> is = vector<cell>();
        while (!CellList::isEmpty(Xs)) {
            cell c = CellList::head(Xs);
            is.push_back(c);
            Xs = CellList::tail(Xs);
        }
        return is;
    }

    string CellList::toString() {
        string s = "[";
        string sep = "";
#if 0
        CL_p start = mk_shared(head(), tail());
        for (CellList* cln = this; cln != nullptr; cln = CellList::tail(cln) {
            s += sep;
            sep = ",";
            s += "<toString cell dummy>"; // showCell(x);
        }
#else
        s += " *** CellList::toString() STUB ***";
#endif
        s += "]";
        return s;
    }
#endif
}

