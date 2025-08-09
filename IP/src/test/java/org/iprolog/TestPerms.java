package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestPerms extends TestTerm {

    String expected[] = {""};

    LPv X,Xs,Y,Ys,Z,Zs;

    TestPerms() {

        int difficulty = 2;

        say_( eq(X,X) );
        say_( sel(X, P_(X, Xs), Xs) );
        say_( sel(X, P_(Y, Xs), P_(Y, Ys))).
                if_(    sel(X, Xs, Ys) );
        say_( perm(L_(), L_()) );
        say_( perm(P_(X, Xs), Zs)).
                if_(    perm(Xs, Ys),
                        sel(X, Zs, Ys) );
        say_( app(L_(), Xs, Xs) );
        say_( app(P_(X, Xs), Ys, P_(X, Zs))).
                if_(app(Xs, Ys, Zs) );
        say_( nrev(L_(), L_()));
        say_( nrev(P_(X, Xs), Zs)).
                if_(    nrev(Xs, Ys),
                        app(Ys, L_(X), Zs) );

        assert difficulty > 0;
        Term nlist = l_(c1());
        assert nlist.is_a_termlist();
        String output = "1";
        for (Integer i = 2; i <= difficulty; ++i) {
            nlist.takes_this(c_(i.toString()));
            output = i.toString() + "," + output;
        }
        output = "[" + output + "]";
        expected[0] = output;
        LPv nL = new LPv();
        nL.run = ()->(nlist);
        say_( input(nL) );

        say_(good_(Y)).
                if_(    input(X),
                        nrev(X, Y),
                        perm(X, Y),
                        perm(Y, X)  );
    }

    LPv sel(LPv a, LPv b, LPv c)   { return S_(a, b, c); }
    LPv perm(LPv x, LPv y)         { return S_(x, y);    }
    LPv app(LPv a, LPv b, LPv c)   { return S_(a, b, c); }
    LPv nrev(LPv x, LPv y)         { return S_(x, y);    }
    LPv input(LPv x)               { return S_(x);       }
    LPv eq(LPv x, LPv y)           { return S_(x,y);     }

    @Test
    public void mainTest() {
        try_it(said, expected);
    }
}
