package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestProg extends TestTerm {

    Term pred(LPv t) {
        return s_(m_(),t.run.fn());
    }

    LPv Nothing;
    LPv Something;
    LPv some_struct(LPv x)            { return S_(x);     }
    LPv other_struct(LPv x, LPv y)    { return S_(x,y);   }
    LPv pair(LPv x, LPv y)            { return P_(x,y);   }
    LPv is_zero(LPv x)                { return S_(x); }
    LPv X;

    void try_matching (String output, LPv f) {
        assert output != null;
        assert f != null;
        assert output.compareTo(f.run.fn().toString()) == 0;
    }

    @Test
    public void mainTest() {

        Term.set_Prolog();

        assert Something != null;
        assert Something.run != null;
        try_matching ("Something",Something);
        assert Nothing != null;
        assert Nothing.run != null;
        try_matching ("Nothing",Nothing);
        Term result = pred(Nothing);
        assert "pred(Nothing)".compareTo(result.toString()) == 0;
        try_matching ("some_struct(Nothing)",some_struct(Nothing));
        try_matching ("some_struct(some_struct(Something))",some_struct(some_struct(Something)));
        try_matching ("other_struct(Something,Nothing)",other_struct(Something,Nothing));
        try_matching ("nil",L_());
        try_matching ("[Something,Nothing]",L_(Something,Nothing));
        try_matching ("[Something|Nothing]",pair(Something,Nothing));
        try_matching ("[0|[Something|Nothing]]",P_(C_("0"),Something,Nothing));

        LPv zero = C_("0");
        say_(is_zero(zero));
        say_(good_(X)).if_(is_zero(X));

        String expected[] = { "0" };

        try_it (said, expected, true);
    }
}