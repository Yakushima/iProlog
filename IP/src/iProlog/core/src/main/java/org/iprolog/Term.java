package org.iprolog;

import java.util.LinkedList;

import javax.lang.model.element.Element;
import javax.lang.model.util.ElementScanner6;

import java.util.Iterator;

// Prolog "term", with lexical conventions abstracted away.
// E.g., variables don't have to start with a capital letter
// or underscore, and can have embedded blanks. Maybe better
// to call it a Structure, which may align better with some
// established Prolog nomenclature.

// To be consistent with Paul Tarau's strategy of
// pretending Java classes are like C structs,
// I won't try to make this an interface
// for free-standing Java classes for constants,
// variables and compound terms.

// In C it could probably contain a tagged union. However,
// because the members built up through the API get
// turned into Tarau's compiled form, their size doesn't
// matter much. They'll be used once paged out of caches/RAM
// during any long logic-program execution; if their size
// somehow becomes a memory or speed bottleneck, the
// can be garbage-collected; as long as the symbol
// table info is retained, something similar enough
// to the original can be reconstructed from Tarau's
// compiled form.

// For tags, note the correpondence to Engine's V, R, and C.
// There may be a reason to merge the tags in Engine with the tags here.
// For now, tolerate smelly redundancy. I need to look at whether
// the equations (e.g., "_1=x") can be Compounds (like ""=(_1,x)"")
// internally; if so, this redundancy in coding types could be
// eliminated.

public class Term implements Iterable<Term> {

    class CustomIterator implements Iterator<Term> {
        Term start;
        Term cursor;
        CustomIterator(Term obj) {      // constructor -- initialize cursor
            start = cursor = obj;
        }
        public boolean hasNext() {      // Checks if the next element exists
            return cursor.next != null;
        }
        public Term next() {            // moves the cursor/iterator to next element
            // should probably throw exception if !hasnext() 
            return cursor.next;
        }
    }
    public Iterator<Term> iterator() {
        return new Term.CustomIterator(this);
    }

    final private static int Variable = 1;   // correponds to Engine.U (unbound variable)
    final private static int Compound = 2;   // correponds to Engine.R (reference)
    final private static int Constant = 3;   // correponds to Engine.C (constant)
    final private static int TermList = 4;   // Not in Engine tags because lists expand
    final private static int TermPair = 5;   // cons/dotted-pair for list construction

    // "TermPair" can be used in the "natural assembly language"; "list" is the
    // corresponding reserved word, "lists" being a plurality of termpairs
    // arranged as, well, a list. Yeah, I found it cofusing at first myself.
    // If I ever change the "natural assembly language", I might go with "pair"
    // and maybe "just" when the second argument is nil.

    // hacky: if a variable presented through the API doesn't
    // start with upper case or underscore, prefix it with
    // something that does; likewise, if a constant starts
    // with upper case, prefix it with something lower case;
    // filter these prefixes back out at a later stage of
    // token processing.

    final public static String Var_prefix = "V__";
    final public static String Const_prefix = "c__";

    /*final*/ private int tag;      // mutable for in-place rewriting
                                    // which I admit may turn out to be
                                    // a bad idea
    
    
    public String v() { return V_; };
    private String V_;
    String set_v(String v) { V_ = v; return V_; }
    public String c() { return C_; }
    private String C_;
    String set_c(String c) { C_ = c; return C_; }
    private LinkedList<Term> terms() { return Terms; }   // What's in "(...)" in a compound,
                                            //  or (hacky) the list [lhs,rhs] if equation
    private LinkedList<Term> Terms;
    private LinkedList<Term> set_terms(LinkedList<Term> Ts) { Terms = Ts; return Ts; }

    Term next;

    LinkedList<Term> args() {
        assert tag == Compound;
        return terms();
    }

    // compares two terms
    // really need to sort out what null list means
    // is it empty list?
    // null pointer?
    // either?
    Boolean is_same_as (Term t) {
        if (tag != t.tag
         || (c() != null && c().compareTo(t.c()) != 0)
         || (v() != null && v().compareTo(t.v()) != 0)) return false;

        if (terms() != null && t.terms() == null) return false;
        if (terms() == null && t.terms() != null) return false;

        if (t.terms() == null) {
            assert terms() == null;
            return true;
        }
        Iterator<Term> itl = t.terms().iterator();
        if (!itl.hasNext() && (terms() == null || terms().isEmpty()))  return true;
        if (!itl.hasNext() && (terms() != null || !terms().isEmpty())) return false;
        Term tt = itl.next();

        for (Term t1 : terms()) {
            if (tt == null) return false;
            if (!t1.is_same_as(tt)) return false;
            if (itl.hasNext()) tt = itl.next();
            else tt = null;
        }
        return true;
    }

    Term a_term (int tag, String thing, Term... ts) {
        LinkedList<Term> tl = new LinkedList<Term>();
        for (Term t : ts)
            tl.add (t);
        return new Term (tag, thing, tl);
    }

    Term (int tag, String thing, LinkedList<Term> Ts) {

        this.tag = tag;

        switch (tag) {
            case Variable: set_v(thing); set_terms(null); set_c(null);  return;
            case Compound: set_v(null); set_terms(Ts); set_c(thing); return;
            case Constant: set_v(null); set_terms(null); set_c(thing); return;
            case TermList: set_v(null); set_terms(Ts); set_c(null);  return;
            case TermPair: set_v(null); set_terms(Ts); set_c(null);  return;
        }

// I should really raise some exception here
        set_v(null);
        set_c(null);
        set_terms(null);
    }

    public Boolean is_a_variable() {  return tag == Variable;  }
    public Boolean is_a_compound() {  return tag == Compound;  }
    public Boolean is_a_constant() {  return tag == Constant;  }
    public Boolean is_a_termlist() {  return tag == TermList;}
    public Boolean is_an_equation(){  return tag == Compound && terms().size() == 2 && c() == "=";  }
    public Boolean is_a_termpair() {  return tag == TermPair; }

    public static String remove_any_Var_prefix(String s) {
        if (s.startsWith(Var_prefix))
            s = s.substring(Var_prefix.length());

        return s;
    }
    public static String remove_any_Const_prefix(String s) {
        if (s.startsWith(Const_prefix))
            s = s.substring(Const_prefix.length());

        return s;
    }
    
    // Embarrassingly hacky Prolog fakeout:
    //  Var_prefix is prepended when a variable ID is lower case.
    // Maybe in getword() this could be detected and removed,
    // but the v tag kept.
    // Similar hacky treatment for constants starting upper case.
    public static Term variable(String v) {
        if (Character.isLowerCase(v.charAt(0))) v = Var_prefix + v;
        return new Term (Variable, v, null);
    }
    public static Term constant(String c) {
        if (Character.isUpperCase(c.charAt(0))) c = Const_prefix + c;
        return new Term (Constant, c, null);
    }
    public static Term compound(String f) {
        return new Term (Compound, f, new LinkedList<Term>());
    }
    public static Term compound(String f, LinkedList<Term> terms) {
        return new Term (Compound, f, terms);
    }
    public static Term compound(String f, Term t) {
        LinkedList<Term> llt = new LinkedList<>();
        llt.add (t);
        return compound (f, llt);
    }
    public static Term equation(Term lhs, Term rhs) {
        LinkedList<Term> ll = new LinkedList<Term>();
        ll.add (lhs);
        ll.add (rhs);
        return new Term(Compound, "=", ll);
    }

    public static Term termlist(Term... ts) {
        LinkedList<Term> ll = new LinkedList<Term>();
        for (Term t : ts)
            ll.add(t);
        return termlist(ll);
    }

    public static Term termlist(LinkedList<Term> llt) {
        Term t = new Term (TermList, "[...]", llt);

        return t;
    }
    public static Term termpair(LinkedList<Term> car_cdr) {
        int sz = car_cdr.size();

        assert sz <= 2 && sz > 0;

        return new Term (TermPair, "", car_cdr);
    }
    // Not clear what to do about termpairs. Maybe default to
    // first elt of LinkedList is car, the rest is cdr?
    public static Term termpair(Term car, Term cdr) {
        LinkedList<Term> car_cdr = new LinkedList<Term>();
        car_cdr.add (car);
        car_cdr.add (cdr);
        return termpair (car_cdr);
    }

    public Term lhs() {
        assert this.is_an_equation();
        return terms().peekFirst();
    }

    public Term rhs() {
        assert this.is_an_equation();
        return terms().peekLast();
    }

    // The following differences in lexicalization
    // may be better managed with a class for
    // lexicals + subclassing.

    public static boolean in_Prolog_mode = set_Prolog();
    

    protected static String and_op; 
    protected static String args_start;
    protected static String arg_sep;
    protected static String args_end;
    protected static String clause_end;
    protected static String if_sym;
    protected static String holds_op;
    protected static String list_start;
    protected static String list_elt_sep;
    protected static String list_end;
    protected static String cons;
    
    // See Toks; there, I squeeze out whitespace
    // from these. Used for pretty-printing the
    // Tarau "assembly language" and (indirectly)
    // in the toSentences lexeme tagger.
    public static void set_TarauLog() {

        and_op = " and ";
        args_start = " ";
        arg_sep = " ";
        args_end = " ";
        clause_end = ".";
        if_sym = "\nif ";
        holds_op = " holds ";
        list_start = "lists ";
        list_elt_sep = " ";
        list_end = " ";
        cons = "list ";

        in_Prolog_mode = false;
    }

    public static boolean set_Prolog() {

        and_op = ",";
        args_start = "(";
        arg_sep = ",";
        args_end = ")";
        clause_end = ".";
        if_sym = ":-";
        holds_op = "=";
        list_start = "[";
        list_elt_sep = ",";
        list_end = "]";
        cons = "(.)";

        return in_Prolog_mode = true;
    }

    public String as_fact() { return this + clause_end; }
    public String as_head() { return this + if_sym; }
    public String as_expr(String ended_with) {
        return "  " + this + ended_with;
    }

    // Problem with conflating equations/expressions and terms:
    // - for compounds it's still comma-separated;
    // - between expressions, it's Tarau's "and";
    // - so allow for either.
    private String terms_to_str(String sep) {

        String delim = "";
        String s = "";

        for (Term t : terms()) {
            s = s + delim;
            if (t == null)
                s = s + " nil ";
            else
                s = s + t;
            delim = sep;
        }
        return s;
    }

    private boolean is_lists() {
        return tag == Compound && this.c() == "lists";
    }

    public String toString() {
        String r = "<unassigned>";

        switch (tag) {
            case Variable: r = v(); break;
            case Constant: r = c(); break;
            case Compound: if (c() != "=") {
                                
                                if (c() == "lists") {
                                    // Main.println("  Term.toString:      seeing lists case  !!!!!!!!!!!!!!");
                                    // Main.println ("  Term.toString:         terms = " + terms_to_str(arg_sep));
                                }
                                 
                                 r =      c()
                                        + args_start
                                        + terms_to_str(arg_sep)
                                        + args_end;
                            } else {
                                assert c() == "=";
                                if (rhs().is_lists()) {
                                    Term rhs_car = rhs().terms().peekFirst();
                                    assert rhs_car != null;
                                    // Main.println ("  Term.toString:       rhs_car = " + rhs_car);
                                    if (rhs().terms().size() == 1)
                                        // Main.println ("  Term.toString:      rhs_cdr == null")
                                        ;
                                    else {
                                        Term rhs_cdr = rhs().terms().get(1);
                                        assert rhs_cdr != null;
                                        // Main.println ("  Term.toString:      rhs_cdr = " + rhs_cdr);
                                    }                                    
                                    r = lhs().toString() + " " + rhs().toString();
                                    // Main.println ("  Term.toString:      holds lists reduced to r = " + r + "!!!!!");
                                } else {
                                    r =   lhs()
                                        + holds_op
                                        + rhs();
                                }
                            }
                            break;
            case TermList:  // if (terms == null)
                            //    Main.println ("  Term.toString:      Term.toString: list terms = null !!!!");
                            
                            r =  list_start + terms_to_str(list_elt_sep) + list_end;
                            break;
            case TermPair:  r = cons + terms_to_str(list_elt_sep);
                            break;
        }
        return r;
    }

    public void takes_this(Term t) {
        assert tag == Compound;
        terms().add (t);
    }

    public void takes_this(LinkedList<Term> llt) {
        assert llt != null;
        terms().addAll(llt);
    }

    private static int    gensym_i = 0;
    public  static void   reset_gensym() { gensym_i = 0; }
    public  static String gensym() { return "_" + gensym_i++; }

    private Boolean is_simple() {
        return tag == Variable || tag == Constant;
    }

    public Boolean is_flat() {
        if (this.is_simple()) return true;

        // With lhs and rhs coded in Term as two-element list
        // this step is not even really necessary; you've
        // got either a compound, or a list:
        // if (c == "=")
        //    return lhs().is_flat() && rhs().is_flat();

        // depends on representing lhs+rhs as terms list in eqns:
        for (Term t : terms())
            if (!t.is_simple())
                return false;

        return true;
    }

    /*
 * https://stackoverflow.com/questions/64814365/flattened-form-in-wam
 * ... build the arguments before you build the outer terms.
 * For example, you must build a(K, C) before you can build h(..., a(K, C), ...),
 * and you must build that before you can build p(..., h(..., a(K, C), ...), ...).
 * Here is one legal order for p(Z,h(Y,a(K,C),K),f(C)):

          _4 = a(K, C)
     _2 = h(Y, _4, K)
     _3 = f(C)
_1 = p(Z, _2, _3)
 */

// A flatten that returns residue "squeezed out".
// this transforms clause structures.
// it's applied first to the head, then to the body
// Making it DFS in what it flattens.
// Trying to make it work in-place -- i.e., it
// rewrites the structure given to it.
// The original should be recoverable (P. Tarau)
// Not sure any of this is right.
// Basically, loop until residue gone.

// flatten a term, with (mutable?) todo list passed in.
// Has shared-structure result for already-flattened terms
// It can also munge what's originally passed to it, so
// watch out for linked list side effects on terms list,
// at point of call.

// think of holds/'=' and list/'.' as functors of compounds
// just always two arguments a1,a2, either or both of which
// could be flat themselves.

/*
 * a(b(c)) -> a(_0), todo = [_0 = b(c)]
 * But if treated as compound ['='(_0,b(c))] in further iteration,
 * recursion would be infinite.
 * So equations really need to be a special case
 */

 /* flatten this */

    public LinkedList<Term> flatten() {
        return this.flatten1 (true);
    }
    private LinkedList<Term> flatten1 (Boolean first) {

        LinkedList<Term> r = new LinkedList<Term>();

        if (this.is_simple()) {
            r.add (this);
            return r;
        }
 
        LinkedList<Term> todo = new LinkedList<Term>();
        LinkedList<Term> new_terms = new LinkedList<Term>();

        if (this.is_an_equation()) {
            for (Term t : terms()) {   // will traverse lhs and rhs
                if (!t.is_flat())
                    todo.addAll (t.flatten1(false));
                new_terms.add (t);
            }
            set_terms(new_terms);
            r.add (this);
        }
        else {
            for (Term t : terms()) {
                if (t.is_simple()) {
                    new_terms.add (t);
                } 
                else {
                    Term v = variable(Term.gensym());
                    new_terms.add(v);
                    Term e = equation (v, t);
                    todo.add(e);
                }
            }
            set_terms(new_terms);

            if (first)
                r.add (this);
        }
        
        for (Term t :  todo)
            r.addAll (t.flatten1(false));

        return r;
    }

}



