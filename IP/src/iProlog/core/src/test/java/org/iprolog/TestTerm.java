package org.iprolog;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertFalse;


import java.util.LinkedList;

public class TestTerm {

    private static Term v_(String s) { return Term.variable(s); }
    private static Term c_(String s) { return Term.constant(s); }
    private static Term s_(String s) { return Term.compound(s); }
    private static Term s_(String s, Term t) { return Term.compound(s,t); }
    private static Term s_(String s, LinkedList<Term> llt) { return Term.compound(s,llt); }
    private static Term e_(Term lhs, Term rhs) { return Term.equation (lhs,rhs); }

    // Want to add arg for expected output, and compare
    // Expected output could be Object or Object[]

    // not clear whether Object.deepEquals would do all this . . . .
    private boolean same (Object o1, Object o2) {

        return o1 == o2; // WRONG JUST HOLDING ON TIL WRITTEN
    }

    private void run(Prog P, int n_answers, Object[] expected) {
        int ctr = 0;
        int MAX_OUTPUT_LINES = 5;
    
        for (;; ctr++) {
          final int i = P.ask_raw();
          if (P.no_more(i)) break;
          final Object A = P.exportTerm(i);
          if(ctr<MAX_OUTPUT_LINES) {
            Prog.println("[" + ctr + "] " + "*** ANSWER=" + /* P.showTerm( */ A /*)*/ );
          }
          if (A instanceof Object[]) {
            Prog.println ("... of type Object[], looks like");
            int k = 0;
            for (Object o : (Object[]) A) {
                Prog.println ("... A[" + k + "] = " + o);
                ++k;
            }
          }
          if (expected == null) 
            Prog.println ("expected == null");
           else 
            assert same (A, expected[ctr]);
        }
        if(ctr>MAX_OUTPUT_LINES)
          Prog.println("...");
        Prog.println("TOTAL ANSWERS=" + ctr);

        assert ctr == n_answers;
      }

    String out = "";

    private void emit_as_fact(Term t) {
        assert t != null;
        out += t.as_fact() + "\n";
        Main.println (t.as_fact());
    }
    private void emit_as_head(Term t) {
        assert t != null;
        out += t.as_head() + "\n";
        Main.println (t.as_head());
    }
    private void emit_as_head(LinkedList<Term> eqns) {
        int n = eqns.size();
        for (Term t : eqns) {
            if (--n == 0)
                emit_as_head (t);
            else
                emit_as_expr (t, Term.and_op);
        }
    }
    private void emit_as_body(LinkedList<Term> eqns) {
        int n = eqns.size();
        for (Term t : eqns) {
            if (--n == 0)
                emit_as_fact (t);
            else
                emit_as_expr (t, Term.and_op);
        }
    }
    private void emit_as_expr(Term t, String ended_with) {
        assert t != null;
        out += t.as_expr(ended_with) + "\n";
        Main.println (t.as_expr(ended_with));
    }

    private void try_t() {
        Main.println ("\n==== try_t ====");
        /*
         * live_(i_).
         * live_(you_).
         * good_(Person) :- live_(Person).
         * goal(Person):-good_(Person).
         * 
                * live_ i_ .
                * live_ you_ .
                * good_ Person
                * if
                *   live_ Person .
                *
                * goal Person
                * if
                *   good_ Person .

         */
        out = "";
        Term.reset_gensym();

        Main.println ("\n Construct data structures for try_t() case ===");

        Term vP = v_("person");

        Term live_of_i_   = s_("live_", c_("I"));
        Term live_of_you_ = s_("live_", c_("you"));

        Term good_Person  = s_("good_", vP);
        Term live_Person  = s_("live_", vP);
        Term goal_Person  = s_("goal", vP);

        Main.println ("\n===Generate Tarau assembly language from it ===");

        Term.set_TarauLog();
        
        emit_as_fact (live_of_i_);
        emit_as_fact (live_of_you_);
        emit_as_head (good_Person);
        emit_as_expr (live_Person, Term.clause_end);
        emit_as_head (goal_Person);
        emit_as_expr (good_Person, Term.clause_end);

        Prog P = new Prog(out, false);

        Main.println ("\n=== Pretty-print Prolog from it ===");

        Term.set_Prolog();

        P.ppCode();

        Main.println ("\n===<<< Starting to run >>>===");

        run(P, 2, null);
    }

    private void try_add() {
        Main.println ("\n==== try_add() ====");
        /*
        the_sum_of(0,X,X).
        the_sum_of(the_successor_of(X),Y,the_successor_of(Z)):-the_sum_of(X,Y,Z).
        goal(R):-
         the_sum_of(the_successor_of(the_successor_of(0)),the_successor_of(the_successor_of(0)),R).

            the_sum_of 0 X X .

            the_sum_of _0 Y _1 and
              _0 holds the_successor_of X and
              _1 holds the_successor_of Z
            if
            the_sum_of X Y Z .

            goal R
            if
              the_sum_of _0 _1 R and
              _0 holds the_successor_of _2 and
              _2 holds the_successor_of 0 and
              _1 holds the_successor_of _3 and
              _3 holds the_successor_of 0 .
         */

        out = "";
        Term.reset_gensym();

        Main.println ("\n=== Try adding 2+2 in unary, generating TarauLog ===");

        Term.set_TarauLog();

        LinkedList<Term> args_0_X_X = new LinkedList<Term>();
        Term c0 = c_("0");
        args_0_X_X.add (c0);
        Term vX = v_("X");
        args_0_X_X.add (vX);
        args_0_X_X.add (vX);

        Term the_sum_of_0_X_X = s_("the_sum_of",args_0_X_X);

        emit_as_fact(the_sum_of_0_X_X);

        // Build this then flatten it:
        //  the_sum_of(the_successor_of(X),Y,the_successor_of(Z))
        Term vY = v_("Y");
        Term vZ = v_("Z");
        Term succ_X = s_("the_successor_of", vX);
        Term succ_Z = s_("the_successor_of", vZ);
        LinkedList<Term> args_list = new LinkedList<>();
        args_list.add (succ_X);
        args_list.add (vY);
        args_list.add (succ_Z);

        Term hd_start = s_("the_sum_of",args_list);
 
        LinkedList<Term> ll = hd_start.flatten();
        emit_as_head(ll);
        Term body = s_("the_sum_of");
        body.takes_this (vX);
        body.takes_this (vY);
        body.takes_this (vZ);
        emit_as_fact(body);

        Term vR = v_("R");
        Term goal = s_("goal");
        goal.takes_this(vR);
        emit_as_head (goal);

        // the_sum_of(the_successor_of(the_successor_of(0)),the_successor_of(the_successor_of(0)),R)
        Term the_sum_of = s_("the_sum_of");
        Term s_of_0 = s_("the_successor_of", c0);
        Term s_of_s_of_0 = s_("the_successor_of", s_of_0);
        the_sum_of.takes_this (s_of_s_of_0);
        the_sum_of.takes_this (s_of_s_of_0);
        the_sum_of.takes_this (vR);

        Term.reset_gensym();

        LinkedList<Term> add_s_s_0_s_s_0_R = the_sum_of.flatten();

        emit_as_body(add_s_s_0_s_s_0_R);

        // comes out with different gensym sequencing
        // maybe because of DFS rather than BFS?

        Prog P = new Prog(out, false);

        Term.set_Prolog();

        Main.println ("\n=== Pretty-print as Prolog ===");

        P.ppCode();

        Main.println ("\n===<<< Starting to run >>>===");

        run (P, 1, null);
    }

    @Test
    public void mainTest() {
        Main.println ("Start Term test");
      
        String gs = Term.gensym();
        assert gs.compareTo("_0") == 0;
        String gs1 = Term.gensym();
        assert gs1.compareTo("_1") == 0;
        Term.reset_gensym();

        String var_s = "X";
        String const_s = "ooh";
        String cmpnd_fnctr = "compound";
        String cmpnd_fnctr1 = "compendium";

        Term v = v_(var_s);
        assert v.is_a_variable();

        Term c = c_(const_s);
        assert c.is_a_constant();

        LinkedList<Term> tl = new LinkedList<Term>();
        tl.add(v);
        tl.add(c);
        Term C = s_(cmpnd_fnctr, tl);
        assert C.is_a_compound();

        // Main.println ("C=" + C);
        String CC = cmpnd_fnctr + "(" + var_s + "," + const_s + ")";
        assert C.toString().compareTo(CC.toString()) == 0;

        LinkedList<Term> fv = v.flatten();
        assert fv.size() == 1;
        assert fv.peekFirst().v == v.v;

        LinkedList<Term> fc = c.flatten();
        assert fc.size() == 1;
        assert fc.peekFirst().c == c.c;

        LinkedList<Term> fC = C.flatten();
        assert fC.size() == 1;
        assert fC.peekFirst().c == C.c;
        // Main.println ("Term C = " + C + ", C.is_flat()=" + C.is_flat());

        Term C1 = s_ (cmpnd_fnctr1);
        assert C1.is_a_compound();
        C1.takes_this (C);
        LinkedList<Term> fC1 = C1.flatten();
        // Main.println ("fC1.size() = " + fC1.size());
        // for (Term t : fC1) Main.println ("a flattened term t=" + t + ", t.is_flat()=" + t.is_flat());

        Term eqn1 = e_ (v,c);
        assert eqn1 != null;

        // Main.println ("eqn1 = " + eqn1);

        Term v1 = v_("Y");
        assert v1.is_a_variable();

        LinkedList<Term> lC1 = new LinkedList<Term>();
        lC1.add(C1);
        Term C2 = s_("foo",lC1);
        // Main.println ("\n\n\nC2 = " + C2 + ", C2.is_flat() = " + C2.is_flat());

        Term eqn2 = e_ (v1, C2);
        // Main.println ("eqn2 = " + eqn2 + ", eqn2.is_flat() = " + eqn2.is_flat());

        LinkedList<Term> flatcat = eqn2.flatten();
        // Main.println ("Flattened: ");
        // for (Term t : flatcat) Main.println ("  -- " + t);

        Main.println ("-----------------------------------------");

        String sum = "the_sum_of";
        String s = "the_successor_of";
        Term an_X = v_("X");
        Term a_Y  = v_("Y");
        Term s_of_X = s_ (s, an_X);
        Term s_of_Y = s_ (s, a_Y);
        LinkedList<Term> lll = new LinkedList<Term>();
        lll.add(s_of_X);
        lll.add(an_X);
        lll.add(s_of_Y);
        Term summer_head = s_(sum,lll);

        Main.println ("summer_head = "+ summer_head);

        LinkedList<Term> flattery = summer_head.flatten();
        for (Term t : flattery) Main.println ("flattery has " + t);

        try_t();
        try_add();

        Main.println ("\n========\nEnd Term test");
    }
}
