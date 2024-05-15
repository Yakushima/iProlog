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
     * 
     * disaggregate hga_0 into reloced_head, reloced_body, and body_len instead
     * of hg_len. Delete unfold.h. Maybe the relocations can even be done
     * in parallel with building the CellList.
     */
    Spine* Spine::new_Spine(
        Clause& C0,
        cell b,
#if 0
        unfolding &skel_0,   // was gs0/goal_stack_0 [Java]
                                    // temporary in unfold(); allocated in pushBody()
        int skel_len_0,
#endif
        int base_0,                 // base
        CL_p goals_0,               // was gs[Java]; tail of G->goals in unfold()
        int trail_top_0,
        vector<ClauseNumber> &unifiables_0)
    {
#define TR if(0)
        Spine* sp = Spine::alloc();

        sp->base = base_0;
        sp->trail_top = trail_top_0;
        sp->last_clause_tried = 0;
        sp->unifiables = unifiables_0;

        for (int i = 0; i < MAXIND; ++i)
            sp->index_vector[i] = cell::BAD;

        sp->skel_len = (int)C0.skel_len;
        unfolding new_skel = alloc_skel(sp->skel_len); // _malloca() !!!!!!

        if (has_raw_cell_heap)
            cell::cp_cells(b, skel_data(C0.skel), skel_data(new_skel), sp->skel_len);
        else
            for (int k = 0; k < sp->skel_len; k++)
                new_skel[k] = C0.skel[k].relocated_by(b);

        sp->head = new_skel[0];
        CL_p acc = goals_0;
        for (int i = sp->skel_len - 1; i > 0; i--)
            acc = CellList::cons(new_skel[size_t(i)], acc);
        sp->the_goals = acc;
        free_skel(new_skel);

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
        sp->skel_len = 0;
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
        abort();
    }

} // end namespace
