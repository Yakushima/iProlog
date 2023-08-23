package org.iprolog;

import org.junit.jupiter.api.Test;

import java.lang.reflect.Field;
import java.lang.reflect.Method;

public class TestProg extends TestTerm {

    Term pred(LPvar t) {
        return s_(m_(),t.run.fn());
    }

    LPvar Nothing;
    LPvar Something;
    LPvar some_struct(LPvar x) { return S_(x); }
    LPvar other_struct(LPvar x, LPvar y)  { return S_(x,y); }
    LPvar goal(LPvar x) { return S_(x); }
    LPvar goal(String c) { return S_(C_(c)); }

    Term foo(Term x) { return (s_(m_(),x)); }
    LPvar is_zero(LPvar x) { return S_(x); }
    LPvar X;

    @Test
    public void mainTest() {

        start_new_test();

        // show_LPvar_fields();
        // show_LPvar_methods();

        Term.set_Prolog();

        assert Something != null;
        assert Something.run != null;
        assert "Something".compareTo(Something.run.fn().toString()) == 0;
        assert Nothing != null;
        assert Nothing.run != null;
        assert "Nothing".compareTo(Nothing.run.fn().toString()) == 0;
        Term result = pred(Nothing);
        assert "pred(Nothing)".compareTo(result.toString()) == 0;
        LPvar r = some_struct(Nothing);
        assert "some_struct(Nothing)".compareTo(r.run.fn().toString()) == 0;
        r = some_struct(some_struct(Something));
        assert "some_struct(some_struct(Something))".compareTo(r.run.fn().toString()) == 0;
        r = other_struct(Something,Nothing);
        assert "other_struct(Something,Nothing)".compareTo(r.run.fn().toString()) == 0;
        r = L_();
        assert "nil".compareTo(r.run.fn().toString()) == 0;
        r = L_(Something,Nothing);
        assert "[Something,Nothing]".compareTo(r.run.fn().toString()) == 0;

        say_(is_zero(C_("0")));
        say_(goal(X)).if_(is_zero(X));

        String expected[] = { "0" };
        Main.println ("Starting try_it in TestProg");
        try_it (said, expected, true);
    }
}