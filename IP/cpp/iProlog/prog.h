#pragma once
/*
 * iProlog/C++  [derived from Java version]
 * License: Apache 2.0
 * Copyright (c) 2017 Paul Tarau
 */

#include "Engine.h"

using namespace std;

namespace iProlog {
	class Prog : Engine {
	public:
		void run(bool print_ans);
		string stats() const;
		Prog(string s) : Engine(s) {};
		void ppCode();
		void ppc(Spine &S);
		string showClause(const Clause& s);
		string showTerm(Object O);

		void ppGoals(shared_ptr<CellList> bs);
		void pp(string s);
	        void pp(unordered_map<string, Integer*> syms);
		void ppTrail();

	private:
		static string maybeNull(const Object& O);
		static inline bool isListCons(cstr name);
		static inline bool isOp(cstr name);
		static string st0(const vector<Object>& args);
	};
};
