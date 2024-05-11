/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "spine.h"

namespace iProlog {

    using namespace std;

    // "append CellList Ys to CellList made from int array xs, return result"
    // (was in CellList but goals_list is not defined there. So far, it's
    // only called here.)
    inline static CL_p concat(goals_list& xs, int len, CL_p Ys) {
        if (len == 0)
            return Ys;
        CL_p Zs = Ys;
        for (int i = len - 1; i >= 0; i--)
            Zs = CellList::cons(xs[size_t(i)], Zs);
        return Zs;
    }

    /**
     * Creates a spine - as a snapshot of some runtime elements.
     */
    Spine::Spine(
        goals_list &goal_refs_0,   // was gs0/goal_stack_0 [Java]
                                    // temporary in unfold(); allocated in pushBody()
        int goal_refs_len_0,
        int base_0,                 // base
        CL_p goals_0,               // was gs[Java]; tail of G->goals in unfold()
        int trail_top_0,
        int last_clause_tried_0,
        vector<ClauseNumber> &unifiables_0)
    {
#define TR if(0)
        head = goal_refs_0[0];
        base = base_0;
        trail_top = trail_top_0;

        for (int i = 0; i < MAXIND; ++i)
            index_vector[i] = cell::BAD;
        last_clause_tried = last_clause_tried_0;

        // "tail" because head is saved above?
        // if so, can discard head of goal_refs_0?

        goal_refs_len = goal_refs_len_0;
        CL_p x = concat(goal_refs_0, goal_refs_len_0, goals_0);
        the_goals = CellList::tail(x);

        TR cout << "  resulting goals =";
        for (CL_p clp = the_goals; clp != nullptr; clp = CellList::tail(clp))
            TR cout << " " << CellList::head(clp).show();
        TR cout << endl;

        unifiables = unifiables_0;
#undef TR
    }

    /**
     * "Creates a specialized spine returning an answer (with no goals left to solve)." {Spine.java]
     */
    Spine::Spine(cell h, int tt) {
        head = h;
        base = 0;
        the_goals = nullptr;
        goal_refs_len = 0;
        trail_top = tt;
        last_clause_tried = -1;
        for (int i = 0; i < MAXIND; ++i)
            index_vector[i] = cell::BAD;
    }

    string Spine::show() const {
        string s = "Spine: {";
        s += "\n  head=" + to_string(head.as_int());
        s += "\n  base=" + to_string(base);
        if (the_goals == nullptr)
            s += "\n  goals=<null>";
        else
            s += "\n  goals=<stub>" /* + goals.show()*/;
        s += "\n  trail_top=" + to_string(trail_top);
        s += "\n  last_clause_tried=" + to_string(last_clause_tried);
        if (unifiables.empty())
            s += "\n  unifiables=<null>";
        else
            s += "\n  unifiables=<stub>" /* + unifiables.show()*/;
        s += "\n}";
        return s;
    }

    Spine::~Spine() {
        CL_p clp = the_goals;
        int k = goal_refs_len-1;
        while (k-- > 0)
            clp = CellList::collect_first(clp);
    }

} // end namespace
