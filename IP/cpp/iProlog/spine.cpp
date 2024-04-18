/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "spine.h"

namespace iProlog {

    using namespace std;

    /**
     * Creates a spine - as a snapshot of some runtime elements.
     */
    Spine::Spine(
        vector<cell> goal_refs_0, // was gs0/goal_stack_0 [Java]
        int base_0,               // base
        shared_ptr<CellList> goals_0,        // was gs/goal_stack [Java]
        int trail_top_0,
        int last_clause_tried_0,
        vector<ClauseNumber> unifiables_0)
    {
        head = goal_refs_0[0];
        base = base_0;
        trail_top = trail_top_0;
        for (int i = 0; i < MAXIND; ++i)
            index_vector[i] = cell::BAD;
        last_clause_tried = last_clause_tried_0;
        goals = CellList::tail(CellList::concat(goal_refs_0, goals_0));
        unifiables = unifiables_0;
    }

    /**
     * "Creates a specialized spine returning an answer (with no goals left to solve)." {Spine.java]
     */
    Spine::Spine(cell h, int tt) {
        head = h;
        base = 0;
        goals = nullptr;
        trail_top = tt;
        last_clause_tried = -1;
        for (int i = 0; i < MAXIND; ++i)
            index_vector[i] = cell::BAD;
    }

    string Spine::show() const {
        string s = "Spine: {";
        s += "\n  head=" + to_string(head.as_int());
        s += "\n  base=" + to_string(base);
        if (goals == nullptr)
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

} // end namespace
