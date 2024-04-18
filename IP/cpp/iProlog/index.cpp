/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

/*
* From the ICLP 2017 conference paper (P. Tarau)
* 
"The indexing algorithm is designed as an independent add-on to be
plugged into the the main Prolog engine."

This C++ code is an attempt to make iProlog indexing more "plug-in".

"For each argument position in the head of a clause
(up to a maximum that can be specified by the programmer)
it associates to each indexable element (symbol, number or arity)
the set of clauses where the indexable element occurs
in that argument position."

MAXIND is that maximum.
*/

#include <iostream>
#include <assert.h>
/// #include "index.h"
#include "Engine.h"

namespace iProlog {

// "Indexing extensions - ony active if [there are] START_INDEX clauses or more."

	t_index_vector index::getIndexables(CellStack &heap, cell goal) {
#define TR if(0)
		TR cout << "getIndexables(...): " << endl;
		int arg_start = 1 + goal.arg();
		int n_args = Engine::getRef(heap,goal).arg();
		int n = min(n_args, MAXIND);

		TR cout << "getIndexables: n_args=" << n_args << " n=" << n << endl;
		t_index_vector index_vector;

		for (int i = 0; i < MAXIND; ++i)
			index_vector[i] = cell::tag(cell::BAD, 0);

		for (int arg_pos = 0; arg_pos < n; arg_pos++) {
			cell arg = Engine::cell_at(heap,arg_start + arg_pos);
			cell c = Engine::deref(heap, arg);
			cell drefd = cell2index(heap,c);

			index_vector[arg_pos] = drefd;

			TR cout << "getIndexables: index_vector[" << arg_pos << "] <- " << index_vector[arg_pos].as_int() << endl;
		}
		TR cout << "getIndexables returning" << endl;
		return index_vector;
	}

    index::index(CellStack &heap, vector<Clause>& clauses) {
#define TR if(0)
		for (int i = 0; i < clauses.size(); ++i) {
			TR cout << "index constr: i=" << i << endl;
				cell hd = clauses.at(i).skeleton.at(0);
			TR cout << "clauses[" << i << "].base = " << clauses[i].base << endl;
				clauses[i].index_vector = getIndexables(heap, hd);
		}

	  // was vcreate in Java version:
		var_maps = vector<cls_no_to_cell>(MAXIND);

		for (int arg_pos = 0; arg_pos < MAXIND; arg_pos++) {
			var_maps[arg_pos] = cls_no_to_cell();
		}
	  // end vcreate inlined

		if (clauses.size() < START_INDEX) {
			imaps = vector<IMap>();
			return;
		}

		imaps = IMap::create(MAXIND);

		for (int i = 0; i < clauses.size(); i++) {
			ClauseIndex ci(i);
			put(clauses[i].index_vector, to_clause_no(ci));  // "$$$ UGLY INC" in Java code
		}
#undef TR
    }

    cell index::cell2index(CellStack &heap, cell c) const {
		cell x = cell::tag(cell::V_,0);
		int t = c.s_tag();
		switch (t) {
			case cell::R_:
				x = Engine::getRef(heap,c);
				break;
			case cell::C_:
			case cell::N_:
				x = c;
				break;
		}
		return x;
    }

/**
 * "Tests if the head of a clause, not yet copied to the heap
 * for execution, could possibly match the current goal, an
 * abstraction of which has been placed in [the index vectors]."
 * ("abstraction of which"???)
 * Supposedly, none of these "abstractions" can be -1
 */
	bool index::possible_match(const Spine* sp,
							   const Clause& cl)
#ifndef COUNTING_MATCHES
											     const
#endif
	{
	if (!indexing) return true;

	// reasonable candidate for loop unrolling:
		for (size_t i = 0; i < MAXIND; i++) {
			cell x = sp->index_vector[i];
			cell y = cl.index_vector[i];

			if (x == cell::tag(cell::V_, 0) || y == cell::tag(cell::V_, 0))
				continue;
			if (x != y)
				break;
		}
#ifdef COUNTING_MATCHES
		n_matches++;
#endif
		return true;
	}
	/* Tarau's IMap.java: K = Integer, a Java object reference.
	 *  "key": index vector element.
	 *  val : clause number (not index)
	 * Java automagically "boxes" key to an Integer.
	 * So we need to hash in imaps on the vector elt?
	 * How does this work with matching clauses in indexing?
	/*
	final boolean put(final K key, final int val) {
		IntMap vals = map.get(key);
		if (null == vals) {
			vals = new IntMap();
			map.put(key, vals);
		}
		return vals.add(val);
	}
	*/

	void index::put(const t_index_vector &iv, ClauseNumber cls_no) {
#define TR if(0)
		cout << "    index::put(iv, cls_no)" << endl;
		for (int arg_pos = 0; arg_pos < MAXIND; arg_pos++) {
			cell vec_elt = iv[arg_pos];

			if (vec_elt != cell::BAD && !vec_elt.is_var()) {  // in Java code, basically: != tag(V_,0) 

				// INDEX PARTLY FAILED BEFORE WHEN CELL SIGN BIT ON
				// Probably because 0 is tag(V_,0) with sign bit off

				TR cout << "    about to call imaps[" << arg_pos << "]->put(new Integer("
					<< cls_no.as_int() << "),  vec_elt=" << vec_elt.as_int() << " )" << endl;

				imaps[arg_pos].put(cls_no, vec_elt);
			}
			else { // this can include vec_elt == cell::BAD case, but maybe that's OK
				/* "If [var_maps[arg_pos]] denotes the set of clauses
				 * having variables in position [arg_pos], then any of them
				 * can also unify with our goal element"
				 */
				TR cout << "    *** var_maps[" << arg_pos << "].add_key(cls_no->i=" << cls_no.as_int() << ")" << endl;
				var_maps[arg_pos].add_key(cls_no.as_int());
				assert(var_maps[arg_pos].contains(cls_no.as_int()));
				TR cout << "    *** var_maps[" << arg_pos << "] is now " << var_maps[arg_pos].show() << endl;
			}
		}

		TR cout << "\n    &&&&& imaps & var_maps in index::put after processing:" << endl;
		TR cout << this->show_index() << endl;
		TR cout << "    &&&&&&" << endl;

		cout << "    index::put exiting........" << endl;
#undef TR
    }

/**
 * "Makes, if needed, registers associated to top goal of a Spine.
 * These registers will be reused when matching with candidate clauses.
 * Note that [index_vector] contains dereferenced cells - this is done once for
 * each goal's toplevel subterms." [Engine.java]
 */
    void index::makeIndexArgs(CellStack &heap, Spine *G, cell goal) {
#define TR if(0)
		if (!indexing) return;

		if (G->index_vector.at(0).s_tag() != cell::BAD
		// || !G->hasGoals()
		)
			return;

		int arg_start = 1 + goal.arg(); // point to # of args of goal
		int n_args = Engine::getRef(heap, goal).arg();
		int n = min(MAXIND, n_args); // # args to compare

		for (int arg_pos = 0; arg_pos < n; arg_pos++) {
			cell arg = Engine::cell_at(heap, arg_start + arg_pos);
			cell arg_val = Engine::deref(heap,arg);
			G->index_vector[arg_pos] = cell2index(heap,arg_val);
		}

		TR cout << "in makeIndexArgs, before matching_clauses call, G->unifiables[0]=" << G->unifiables[0].as_int() << endl;
		G->unifiables = matching_clauses_(G->index_vector);
		if (G->unifiables.size() > 0)
			TR cout << "in makeIndexArgs, AFTER matching_clauses call, G->unifiables[0]=" << G->unifiables[0].as_int() << endl;
#undef TR
    }

	/* "When looking for the clauses matching an element of
     * the list of goals to solve, for an indexing element x occurring in position i,
     * we fetch the set Cx,i of clauses associated to it.
     * If Vi denotes the set of clauses having variables in position i,
     * then any of them can also unify with our goal element.
     * Thus we would need to compute the union of the sets Cx,i and Vi
     * for each position i, and then intersect them
     * to obtain the set of matching clauses.
     * We will not actually compute the unions, however.
     * Instead, for each element of the set of clauses corresponding to
     * the gpredicate nameh (position 0), we retain only those which are
     * either in Cx,i or in Vi for each i > 0.
     * We do the same for each element for the set V0 of clauses
     * having variables in predicate positions (if any)." [HHG/ICLP 2017]
     */
	void intersect0(
		cls_no_to_cell & m,
		vector<cls_no_to_cell>& maps,
		vector<cls_no_to_cell>& vmaps,
		vector<ClauseNumber>& cls_nos) {
#define TR if(0)
		assert(vmaps.size() <= maps.size());

		TR cout << "intersect0(m.size()=" << m.size() << ", maps.size()=" << maps.size()
			<< ", vmaps.size()=" << vmaps.size() << ", cls_nos.size()=" << cls_nos.size() << ")" << endl;

			for (int k = 0; k < m.capacity(); k += m.stride()) {
				if (m.is_free(k))
					continue;
	
				ClauseNumber cn = m.get_key_at(k);
	
				TR cout << "    ***nonzero cn=" << cn.as_int() << " m.is_free(cn.as_int()=" << m.is_free(cn.as_int()) << endl;
				TR cout << "   cn=m.get_key_at(" << k << ")=" << cn.as_int() << endl;
				TR cout << "     ....not free, check if in maps[0.." << maps.size() << "]" << endl;

				bool found = true;
				for (int i = 1; i < maps.size(); i++) {
					ClauseNumber v = maps[i].get(cn.as_int());
					TR cout << "       at maps[" << i << "] v=" << v.as_int() << endl;
					if (v == ClauseNumber(cls_no_to_cell::no_value())) {
						TR cout << "         at maps[" << i << "]  map.get(" << cn.as_int() << ") == NO_VALUE" << endl;
						ClauseNumber vcval = vmaps[i].get(cn.as_int());
						if (vcval == cls_no_to_cell::no_value()) {
							TR cout << "         at vmaps[" << i << "] vmap.get(" << cn.as_int() << ") == NO_VALUE, break from i-loop" << endl;
							found = false;
							break;
						}
					}
				}
				if (found) {
					TR cout << "   at data[" << k << "] found key=" << cn.as_int() << " to push to result" << endl;
					TR cout << "   before push cls_nos.size() == " << cls_nos.size() << endl;
					cls_nos.push_back(cn);
				}
			}
#undef TR
	}

	/*
	 * This translation is from IMap.get, with ArrayList for ms & vms 
	 */
	vector<ClauseIndex> index::matching_clauses_(t_index_vector& iv) {
#define TR if(0)
		TR cout << "Entering matching_clauses" << endl;

		vector<cls_no_to_cell> ms;
		vector<cls_no_to_cell> vms;
		TR cout << " matching_clauses: entering iv loop" << endl;

		for (int i = 0; i < imaps.size(); i++)
			if (iv[i].as_int() == 0 || iv[i] == cell::BAD) { // "index vectors are null-terminated if < MAXIND"

				// This was "== 0" in Java code. In that code, since V_ cells
				// are never zero (even though V_ == 0) zero may be a kind of null-
				// terminator for index vectors that are shorter than MAXIND.
				// This works because V_ cells can't be at index 0 in the heap.
				// 
				// This complicates the idea of changing to relative addressing
				// in variables, with a zero offset indicating that dereferencing
				// has ended at a variable. I'll keep cell::BAD for now.
				// If this is true, the mystery is why there's no loop break
				// on zero.
				TR cout << "  i = " << i << " key iv[" << i << "] = " << iv[i].as_int() << " - skipping..." << endl;
				continue;
			}
			else {
				  TR cout << "imaps.at(" << i << ")=" << imaps.at(i).show() << " i=" << i << endl;
				  TR cout << " in imaps[" << i <<"], count for vec elt " << iv[i].as_int() << " is " << imaps[i].map.count(iv[i]) << endl;
					cls_no_to_cell m = imaps[i].map[iv[i]];  // get_cls_no_to_cell(ip);
				  TR cout << "  m=" << m.show() << endl;
					ms.emplace_back(m);  // ms will start empty
					vms.emplace_back(var_maps.at(i));
				  TR cout << "  matching_clauses() iv loop: imaps[" << i << "] pushing " << endl;
			}
		TR cout << " matching_clauses: exited iv loop, ms.size()=" << ms.size() << " vms.size()=" << vms.size() << endl;

		TR cout << endl;
		TR cout << "  matching_clauses: ms[0.." << ms.size() << "]" << endl;
		for (int i = 0; i < ms.size(); ++i)
			TR cout << "     [" << i << "]: " << ms[i].show() << endl;
		TR cout << "  matching_clauses: vms[0.." << vms.size() << "]" << endl;
		for (int i = 0; i < vms.size(); ++i)
			TR cout << "     [" << i << "]: " << vms[i].show() << endl;
		TR cout << endl;

		vector<cls_no_to_cell> ims = vector<cls_no_to_cell>(ms.size());
		vector<cls_no_to_cell> vims = vector<cls_no_to_cell>(vms.size());
		assert(ims.size() == vims.size());
		assert(ims.size() > 0);

		for (int i = 0; i < ms.size(); i++) { // ims.length in Java code
			ims[i] = ms.at(i);
			vims[i] = vms.at(i);
		}
		vector<ClauseNumber> cs; // "$$$ add vmaps here"
		// cs: receives the clause numbers:

		// was IntMap.java intersect, expanded here:

			TR cout << "  ims: " << endl;
			for (int i = 0; i < ims.size(); ++i) TR cout << "   [" << i << "]: " << ims[i].show() << endl;
			TR cout << "  vims: " << endl;
			for (int i = 0; i < vims.size(); ++i) TR cout << "   [" << i << "]: " << vims[i].show() << endl;

			TR cout << "intersect0(ims[0], ims, vims, cs)" << endl;
			intersect0(ims[0], ims, vims, cs);
			TR cout << "&&&& after first intersect0, r=[";
			for (int i = 0; i < cs.size(); ++i) TR cout << cs[i].as_int() << " ";
			TR cout << "]" << endl;
			TR cout << "intersect0(vims[0], ims, vims, cs)" << endl;
			intersect0(vims[0], ims, vims, cs);

			// is: clause numbers converted to indices
			vector<ClauseIndex> is;  /*= cs.toArray() in Java, emulated here but
										* with conversion to indices. Could
										* probably be done on-the-fly in intersect0. */

			for (int i = 0; i < cs.size(); ++i)
				is.push_back(to_clause_idx(cs.at(i)));


			/* "Finally we sort the resulting set of clause numbers and
				* hand it over to the main Prolog engine for unification
				* and possible unfolding in case of success."
				*
				* I.e., respect standard Prolog clause ordering.
				*/
			std::sort(is.begin(), is.end());

			TR cout << " is:" << endl;

			for (int i = 0; i < is.size(); ++i)
				TR cout << "    is[" << i << "] = " << is.at(i).as_int() << endl;

			TR cout << "matching_clauses: exiting safely" << endl;

			return is;
#undef TR
	}

	string index::show(const t_index_vector& iv) const {
		string s = "";
		char d = '<';
		for (int arg_pos = 0; arg_pos < MAXIND; ++arg_pos) {
			s += d;
			s += to_string(iv[arg_pos].as_int());
			d = ',';
		}
		s += ">";
		return s;
	}

	string index::show_index() const {
		string s = "index::show():";

		for (int i = 0; i < MAXIND; ++i) {
			s += "\n  INDEX[" + to_string(i);
			s += "]";
			s += "\n    imaps:    " + imaps[i].show();
			s += "\n    var_maps: " + var_maps[i].show();
		}

		s += "\n";
		return s;
	}
} // namespace
