/*
 * iProlog/C++ [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */


#include "Engine.h"

namespace iProlog {

 /**
  * Implements execution mechanism
  *
  * "an array representation
  *  with variables on the left side of our equations
  *  turned into indices pointing to compound terms
  *  at higher addresses in the same array."
  *
  * add _0 Y _1 and _0 holds(=) s X and _1 holds(=) s Z if add X Y Z:
  *
  **[5]  a: 4        -- because |[add,_0,Y,_1]| = 4
  * [6]  c: ->"add"
  * [7]  r: 10       -- link to subterm: _0 holds(=) s X
  * [8]  v: 8        -- Y
  * [9]  r: 13       -- link to next subterm: _1 holds(=) s Z
  *
  *                  -- _0 holds(=) s X:
  **[10] a: 2
  * [11] c: ->"s"
  * [12] v: 12       -- X
  *
  *                  -- _1 holds(=) s Z:
  **[13] a: 2
  * [14] c: ->"s"
  * [15] v: 15       -- Z
  *
  *                  -- add X Y Z:
  **[16] a: 4
  * [17] c: ->"add"
  * [18] u: 12       -- X
  * [19] u: 8        -- Y
  * [20] u: 15       -- Z
  */

Engine::~Engine() { }

/**
 * unfold - "transforms a spine [G] containing references to choice point and
 * immutable list of goals into a new spine, by reducing the
 * first goal in the list with a clause that successfully
 * unifies with it - in which case places the goals of the
 * clause at the top of the new list of goals, in reverse order"
 */
Spine* Engine::unfold(Spine *G) {
#define TR if(0)
    assert(G != nullptr);

    if (CellList::isEmpty(G->the_goals))
        return nullptr;

    int tot = trail.getTop();    // top of trail
    int saved_heap_top = heap.getTop();
    int base = saved_heap_top + 1;
    cell goal = CellList::head(G->the_goals);

    TR cout << "unfold: goal=" << goal.as_int() << " goal.tag= " << goal.s_tag() << " goal.arg = " << goal.arg() << endl;
    TR cout << "unfold: about to call makeIndexArgs with current G->unifiables[0]=" << G->unifiables[0].as_int() << endl;

    Ip->makeIndexArgs(heap, G, goal); //  goal == CellList::head(G->goals)

    if (G->unifiables.size() > 0)
        TR cout << "unfold: after makeIndexArgs with current G->unifiables[0]=" << G->unifiables[0].as_int() << endl;

    size_t last = G->unifiables.size();
    TR cout << "unfold: last=" << to_string(last) << endl;

    TR cout << "G->last_clause_tried=" << G->last_clause_tried << endl;
    for (int k = G->last_clause_tried; k < last; k++) {
        TR cout << "G->unifiables[" << k << "]=" << G->unifiables[k].as_int() << endl;

        Clause& C0 = clauses[G->unifiables[k].as_int()];

        if (!Ip->possible_match(G, C0))
            continue;

        cell b = cell::tag(cell::V_, base - C0.base);
        cell head = pushHeadtoHeap(b, C0);
 
        unify_stack.clear();  // "set up unification stack" [Engine.java]
        unify_stack.push(head);
        unify_stack.push(goal); // from  CellList::head(G->goals)

        if (!unify(base)) {
            unwindTrail(tot);
            heap.setTop(saved_heap_top);
            continue;
        }

        int len = (int)C0.skeleton_size;

        if (len == 0) cout << "unfold: len == 0" << endl;

#ifdef RAW_GOALS_LIST
        //  "Unlike _alloca, which doesn't require or permit a call
        //  to free to free the memory so allocated, _malloca requires
        //  the use of _freea to free memory."
        // https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/malloca?view=msvc-170
        goals_list goals = (cell*)_malloca(len * sizeof(cell));
#else
        goals_list goals(len);
#endif
        pushBody(goals, len, b, head, C0);

        TR cout << "$$$$$$$$$$$ goals after pushBody:" << endl;
        // for (int i = 0; i < goals.size(); ++i)
        //    cout << " " << showCell(goals[i]) << endl;

        CL_p tl = CellList::tail(G->the_goals);
            // meaning I could free up the head of G->goals?
        G->last_clause_tried = k + 1;

        Spine* spr;

        if (len != 0 || tl != nullptr)
            spr = new Spine(goals, len, base, tl, tot, 0, clause_list);
        else
            spr = answer(tot);

        // mystery: I never see this output trace, even though
        // setting a breakpoint at the return statement causes
        // a break.
        TR cout << "unfold: trail_top=" << to_string(tot) << " returning answer" << endl;
#ifdef RAW_GOALS_LIST
        _freea(goals);
#endif
        return spr;
    }
    return nullptr;
#undef TR
}

bool Engine::unify(int base) {
    while (!unify_stack.isEmpty()) {
        cell x1 = deref(unify_stack.pop());
        cell x2 = deref(unify_stack.pop());

        // cout << "unify: x1=" << x1.as_int() << ",x2=" << x2.as_int() << endl;

        if (x1.as_int() != x2.as_int()) {
            int t1 = x1.s_tag();
            int t2 = x2.s_tag();
            int w1 = x1.arg();
            int w2 = x2.arg();
            if (x1.is_var()) {                  /* "unb. var. v1" */
                if (x2.is_var() && w2 > w1) {   /* "unb. var. v2" */
                    set_cell(w2,x1);
                    if (w2 <= base) {
                        trail.push(x2);
                    }
                } else {                            // "x2 nonvar or older"
                    set_cell(w1,x2);
                    if (w1 <= base) {
                        trail.push(x1);
                    }
                }
            } else if (x2.is_var()) {           /* "x1 is NONVAR" */
                set_cell(w2,x1);
                if (w2 <= base) {
                    trail.push(x2);
                }
            } else if (cell::R_ == t1 && cell::R_ == t2) {  // "both should be R"
                if (!unify_args(w1, w2)) {
                    return false;
                }
            }
            else {
                return false;
            }
        }
    }
    // cout << "unify succeeded" << endl;
    return true;
}

bool Engine::unify_args(int w1, int w2) { // w1 & w1 already detagged in unify()
    assert(cell_at(w1).is_offset() && cell_at(w2).is_offset());

    cell v1 = cell_at(w1);
    cell v2 = cell_at(w2);

    // "both should be A:"

    if (!v1.is_offset()) abort();
    if (!v2.is_offset()) abort();

    int n1 = v1.arg();
    int n2 = v2.arg();

    if (n1 != n2)
        return false;

    int b1 = 1 + w1;
    int b2 = 1 + w2;

    for (int i = n1 - 1; i >= 0; i--) {
        int i1 = b1 + i;
        int i2 = b2 + i;

        cell u1 = cell_at(i1);
        cell u2 = cell_at(i2);

        if (u1 == u2)
            continue;

        unify_stack.push(u2);
        unify_stack.push(u1);
    }
    return true;
}

void Engine::clear() {
    heap.setTop(-1);
}

    //    was iota(clause_list.begin(), clause_list.end(), 0);
    vector<ClauseIndex> Engine::toNums(vector<Clause> clauses) {
        int l = (int) clauses.size();
        vector<ClauseIndex> cls = vector<ClauseIndex>(l);
        for (int i = 0; i < l; i++) {
            ClauseIndex x(i);
            cls[i] = x;
        }
        return cls;
    }

 /**
  * Extracts a query - by convention of the form
  * goal(Vars):-body to be executed by the engine
  */
Clause Engine::getQuery() {
    return clauses[clauses.size() - 1];
}

/**
 * "Returns the initial spine built from the query from which execution starts."
 */
Spine *Engine::init() {
    int base = heap_size();
    Clause G = getQuery();
    Spine *Q = new Spine(G.skeleton, G.skeleton_size, base, nullptr, trail.getTop(), 0, clause_list);

    spines.push_back(Q);
    return Q;
}

/**
 * answer - "Returns an answer as a Spine while recording in it
 * the top of the trail to allow the caller to retrieve
 * more answers by forcing backtracking."
 */
Spine* Engine::answer(int trail_top) {
    return new Spine(spines[0]->head, trail_top);
}

/**
 * hasClauses - "Detects availability of alternative clauses for the
 * top goal of this spine."
 */
bool Engine::hasClauses(const Spine* S) const {
    return S->last_clause_tried < S->unifiables.size();
}

Object Engine::exportTerm(cell x) const {
#define TR if(0)
    if (x == cell::tag(cell::BAD, 0))
        return Object();

    x = deref(x);
    TR cout << "exportTerm: x=" << x.show() << endl;
    int w = x.arg();

    switch (x.s_tag()) {
    case cell::C_: return Object(symTab.getSym(w));
    case cell::N_: return Object(w);
    case cell::V_: return Object(cstr("V") + w);
        /*case U_:*/
    case cell::R_: {
        TR cout << "R_: " << endl;
        cell a = cell_at(w);
        if (!a.is_offset())
            throw logic_error(cstr("*** should be A, found=") + showCell(a));
        int n = a.arg();
        vector<Object> arr;
        int k = w + 1;   // "offset to embedded array" [Engine.java]
        for (int i = 0; i < n; i++) {
            int j = k + i;
            cell c = cell_at(j);
            TR cout << "cell_at(" << j << ")=" << c.show() << endl;
            Object o = exportTerm(c);
            TR cout << "exportTerm(cell " << c.show() << ")="
                << exportTerm(c).toString() << endl;
            arr.push_back(o);
        }
        return Object(arr);
    }
    default:
        throw logic_error(cstr("*BAD TERM*") + showCell(x));
    }
#undef TR
}

/**
 * ask - "Retrieves an answer and ensures the engine can be resumed
 * by unwinding the trail of the query Spine.
 * Returns an external "human readable" representation of the answer.
 *
 * "A key element in the interpreter loop is to ensure that
 * after an Engine yields an answer, it can, if asked to,
 * resume execution and work on computing more answers. [...]
 * A variable 'query' of type Spine, contains the top of the trail
 * as it was before evaluation of the last goal,
 * up to where bindings of the variables will have to be undone,
 * before resuming execution." [HHG doc]
 * 
 * MISTAKEN CHANGE:
 * Moving to prog.cpp: "It also unpacks the actual answer term
 * (by calling the method exportTerm) to a tree representation of a term,
 * consisting of recursively embedded arrays hosting as leaves,
 * an external representation of symbols, numbers and variables." [HHG doc]
 */
Object Engine::ask() {
#define TR if(0)
#if 0
    set_engine(this);   // for static checkit, usable in other scopes(?)
    checkit();
#endif
    query = yield();
    if (nullptr == query)
	    return Object();

    TR cout << "ask: query->trail_top=" << query->trail_top << endl;
    Spine *ans = answer(query->trail_top);

    TR cout << "yield: " << ans->show() << endl;
    cell result = ans->head;
    /////////////////////////
    Object R = exportTerm(result);
    unwindTrail(query->trail_top);
    delete ans;
    delete query;    // leaky to delete this?
    query = nullptr;

    return R;
#undef TR
}
/**
 * unwindTrail - "Removes binding for variable cells
 * above savedTop." [Engine.java]
 */
void Engine::unwindTrail(int savedTop) {
    while (savedTop < trail.getTop()) {
        cell href = trail.pop();
        int x = href.arg();
        setRef(href, href);
    }
}

/**
 * popSpine - "Removes this spine from the spine stack and
 * resets trail and heap to where they were at its
 * creation time - while undoing variable binding
 * up to that point." [Engine.java]
 */
void Engine::popSpine() {

    Spine *G = spines.back();
    int new_base = int(G->base) - 1;
    int savedTop = G->trail_top;
    spines.pop_back();
    delete G;
    
    unwindTrail(savedTop);
    heap.setTop(new_base);
}

/**
 * yield "Main interpreter loop: starts from a spine and works
 * though a stream of answers, returned to the caller one
 * at a time, until the spines stack is empty - when it
 * returns null." [Engine.java]
 */
Spine* Engine::yield() {
    while (!spines.empty()) {
        Spine* G = spines.back(); // was "peek()" in Java

        if (!hasClauses(G)) {
            popSpine();
            continue;
        }
        Spine *C = unfold(G);
        if (nullptr == C) {
            popSpine(); // no matches
            continue;
        }
        if (C->hasGoals()) {
            spines.push_back(C);
            continue;
        }
        return C; // answer
    }
    return nullptr;
}

string Engine::showCell(cell w) const {
    int t = w.s_tag();
    int val = w.arg();
    string s = "";

    switch (t) {
        case cell::V_:    s = cstr("v:") + val;        break;
        case cell::U_:    s = cstr("u:") + val;        break;
        case cell::N_:    s = cstr("n:") + val;        break;
        case cell::C_:    s = cstr("c:") + symTab.getSym(val); break;
        case cell::R_:    s = cstr("r:") + val;        break;
        case cell::A_:    s = cstr("a:") + val;        break;
        default:    s = cstr("*BAD*=") + w.as_int();
    }
    return s;
}

/**
 * "Copies and relocates body of clause at offset from heap to heap
 * while also placing head as the first element of array 'goals' that,
 * when returned, contains references to the toplevel spine of the clause."
 * 
 * The goals list is only temporary in unfold(), passed to make
 * a CellList in a new Spine. If I go to relative addressing, 
 * simply passing the Clause to new Spine() could be enough, and
 * save buffering cost. As things stand (i.e., absolute addressing,
 * passing the "b" offset and "len" instead of "goals" to new Spine()
 * means that what is now the concat() operation could do the relocation.
 */
void Engine::pushBody(goals_list &goals, int len, cell b, cell head, const Clause& C) {
#define TR if(0)
    CellStack::pushCells(heap, b, C.neck, C.len, C.base);
#ifdef RAW_GOALS_LIST
    cell* src = C.skeleton;
    cell* dst = goals;
#else
    const cell* src = C.skeleton.data();
    cell* dst = goals.data();
#endif
    goals[0] = head;
    TR cout << "pushBody: goals[0]=" << head.show() << endl;
    if (is_raw)
        cell::cp_cells(b, src + 1, dst + 1, len - 1);
    else
        for (int k = 1; k < len; k++) {
            goals[k] = C.skeleton[k].relocated_by(b);
            TR cout << "pushBody: goals[" << k << "]="
                << goals[k].as_int() << endl;
        }
#undef TR
}

/**
 * Copies and relocates the head of clause C from heap to heap.
 */
cell Engine::pushHeadtoHeap(cell b, const Clause& C) {
#define TR if(0)
    TR cout << "push HeadtoHeap entered" << endl;
    CellStack::pushCells(heap, b, 0, C.neck, C.base);
    cell head = C.skeleton[0];
    cell reloc_head = head.relocated_by(b);
    return reloc_head;
#undef TR
}


} // namespace
