package org.iprolog;

import java.lang.reflect.Field;
import java.lang.reflect.Method;
import java.util.LinkedList;

// The JLPAPI class supports construction of logic programs in the
// slightly more English-like syntax Paul Tarau defined for iProlog
// by defining some Java construction primitives. At this writing
// (July 2024) there is still no more direct way to construct the
// internal representation. This API forces the construction to
// go through a text-parsing step, which may be a significant
// performance-limiter in some applications that generate code.

public class JLPAPI {
    public static Term v_(String s) {    return Term.variable(s); } // v for variable term
    public String m_() {                                            // m for method name
        return Thread.currentThread().getStackTrace()[2].getMethodName();
    }
    public Term v_()                {    return v_(m_());         }  // v for variable term

    public static Term _()          {    return v_("_");       }  // _ is Prolog-ish for unnamed var
    public static Term c_(String s) {    return Term.constant(s); } // c for constant
    public static Term s_(String s) {    return Term.compound(s); } // s for structure
    public static Term s_(String s, Term... ts) {
        Term xt = Term.compound(s);
        for (Term t : ts)
            xt = xt.takes_this(t);
        return xt;
    }
    public static Term e_(Term lhs, Term rhs) {         // e for equation
        return Term.equation (lhs,rhs);
    }
    public static Term l_(Term... ts) {                 // l for list
        if (ts.length == 0)
            return c_("nil");
        return Term.termlist(ts);
    }
    static Term pal_(Term[] tal, int i) {
        if (i == tal.length-2)
            return Term.termpair(tal[i], tal[i+1]);
        return Term.termpair (tal[i], pal_(tal, i+1));
    }
    public static Term p_(Term... Ts) { return pal_(Ts, 0);  }  // p for pair

    public Clause yes_ (Term hd) {     return Clause.f__(hd); }

    public String f_() {
        return Thread.currentThread().getStackTrace()[3].getMethodName();
    }

    public Term call(Term f, Term... ts) { return s_(f.v(),ts); }

    public LPvar call(LPvar f, LPvar... ls)       {
        LPvar r = new LPvar();
        r.run = ()->s_(f.run.fn().toString(), make_xts(ls));
        return r;
    }

    protected Term[] make_xts(LPvar[] xs) {
        Term[] xts = new Term[xs.length];
        int i = 0;
        for (LPvar x : xs) {
            xts[i] = xs[i].run.fn();
            ++i;
        }
        return xts;
    }

    public Term true_()                  { return null;         }  // ????

    public Term c0() { return c_("0"); }
    public Term c1() { return c_("1"); }
    public Term c2() { return c_("2"); }
    public Term c3() { return c_("3"); }
    public Term c4() { return c_("4"); }

    Term V()  { return v_(m_()); }
    Term X()  { return v_(m_()); }
    Term Y()  { return v_(m_()); }
    Term Z()  { return v_(m_()); }

    Term Xs() { return v_(m_()); }
    Term Ys() { return v_(m_()); }
    Term Zs() { return v_(m_()); }

    public Term goal(Term x)  { return s_(m_(),x); }

    // make a structure from a list of arguments
    public LPvar S_(LPvar... xs) {
        String nm = f_();  // misses the correct stack frame if called as arg to s_()
        return new LPvar(()->s_(nm,make_xts(xs)));
    }

    // make a constant from a string
    public LPvar C_(String c) { return new LPvar(()->c_(c)); }

    public LPvar C_(int n) { return new LPvar(()->c_(Integer.toString(n)));}

    // make a list from the list of arguments
    public LPvar L_(LPvar... xs) { return new LPvar(()->l_(make_xts(xs)));  }

    // make a pair from x and y
    public LPvar P_(LPvar x, LPvar y) {
        LPvar r = new LPvar();
        r.run = ()->p_(x.run.fn(),y.run.fn());
        return r;
    }
    LPvar paf_ (LPvar[] taf, int i) {
        LPvar r = new LPvar();
        if (i == taf.length-2) {
            r.run = () -> (Term.termpair(taf[i].run.fn(), taf[i + 1].run.fn()));
            return r;
        }
        r.run = ()->(Term.termpair (taf[i].run.fn(), paf_(taf, i+1).run.fn()));
        return r;
    }
    public LPvar P_(LPvar... Fs) {  return paf_ (Fs, 0);  }

    public void init_LPvars (Class<?> tc) {
        // Main.println ("Entering init_LPvars, class: " + tc.getName());
        Field[] fs = tc.getDeclaredFields();

        try {
            for (Field f : fs)
                if (f.getType().getName().endsWith("LPvar")) {
                    LPvar x = new LPvar();
                    x.run = () -> v_(f.getName());
                    f.setAccessible(true); // TODO - turn off after running? Security!
                    f.set(this, x);
                    // Main.println (f.getName());
                }
        } catch (IllegalAccessException x) {
            x.printStackTrace();
        }
    }

    public void init_LPvars() {
        Class tc = this.getClass();
        init_LPvars(tc);
        tc = tc.getSuperclass();
        init_LPvars(tc);
    }

    public void show_LPvar_methods() {
        Method[] ms = this.getClass().getDeclaredMethods();
        StringBuilder s = new StringBuilder();
        String sep = "";
        for (Method m : ms) {
            String method_type = m.getReturnType().getName();
            if (method_type.endsWith("LPvar")) {
                s.append(sep).append(m.getName());
                sep = ", ";
            }
        }
        Main.println ("LPvar methods are: " + s);
    }

    public void show_LPvar_fields() {
        Field[] fs = this.getClass().getDeclaredFields();
        for (Field f : fs)
            if (f.getType().getName().endsWith("LPvar"))
                Main.println("   field name: " + f.getName());
    }

    JLPAPI() {
        // do nothing
    }
}
