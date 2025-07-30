package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestLambdas extends TestTerm {

    LPv X,Xs,Vs,N,A,B,T,L,N1,N2,N3,Lam,Size;
    LPv _;

    LPv zero()         {  return C_("0");                }

    LPv l(LPv a, LPv b)       { return S_(a,b);     }
    LPv s(LPv a)                { return S_(a);       }
    LPv a(LPv x, LPv y)       { return S_(x,y);     }

    LPv genLambda(LPv a, LPv b, LPv c, LPv d)
                                    { return S_(a,b,c,d); }
    LPv memb(LPv a, LPv b)     { return S_(a,b);     }
    LPv genClosedLambdaTerm(LPv L, LPv T)
                                    { return S_(L,T);     }
    LPv some(LPv x)             { return S_(x);       }

    LPv goal(LPv Lam)           { return S_(Lam);    }

    @Test
    public void mainTest() {
        start_new_test();

        say_(genLambda(X,Vs,N,N))
                .if_(   memb(X,Vs)  );
        say_(genLambda(l(X,A), Vs,s(N1),N2))
                .if_(   genLambda(A,P_(X,Vs),N1,N2)  );
        say_(genLambda(a(A,B), Vs, s(N1),N3))
                .if_(   genLambda(A,Vs,N1,N2),
                        genLambda(B,Vs,N2,N3)  );

        say_(memb(X,P_(X,_)));
        say_(memb(X,P_(_,Xs)))
                .if_(   memb(X,Xs)  );

        say_(genClosedLambdaTerm(L,T))
                .if_(   genLambda(T,L_(),L,zero())  );

        say_(some(s(s(zero()))));

        say_(goal(Lam))
                .if_(   some(Size),
                        genClosedLambdaTerm(Size,Lam) );

        String[] these_answers = {
                "l(V134,l(V157,V157))",
                "l(V134,l(V157,V134))",
                "l(V134,a(V134,V134))"
        };

        try_it(said, these_answers,false);
    }
}
