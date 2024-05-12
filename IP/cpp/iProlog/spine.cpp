/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "spine.h"

namespace iProlog {

    using namespace std;

    Spine *Spine::free_list = nullptr;

    /**
     * "Creates a spine - as a snapshot of some runtime elements."
     */
    Spine *Spine::new_Spine(
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
        Spine* sp = Spine::alloc();
        sp->head = goal_refs_0[0];
        sp->base = base_0;
        sp->trail_top = trail_top_0;
        sp->last_clause_tried = last_clause_tried_0;
        sp->goal_refs_len = goal_refs_len_0;
        sp->unifiables = unifiables_0;

        for (int i = 0; i < MAXIND; ++i)
            sp->index_vector[i] = cell::BAD;

        CL_p acc = goals_0;

        for (int i = sp->goal_refs_len - 1; i > 0; i--)
            acc = CellList::cons(goal_refs_0[size_t(i)], acc);

        sp->the_goals = acc;
        return sp;
#undef TR
    }

    /**
     * "Creates a specialized spine returning an answer (with no goals left to solve)." {Spine.java]
     */
    Spine *Spine::new_Spine(cell h, int tt) {
        Spine* sp = Spine::alloc();
        sp->head = h;
        sp->base = 0;
        sp->the_goals = nullptr;
        sp->goal_refs_len = 0;
        sp->trail_top = tt;
        sp->last_clause_tried = -1;
        for (int i = 0; i < MAXIND; ++i)
            sp->index_vector[i] = cell::BAD;
        return sp;
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
