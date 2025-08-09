package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestSud4x extends TestTerm {

    LPv X,Xs,Y,Ys,Z,Zs, Xss,Xsss,F;
    LPv _;

    TestSud4x() {
        say_(s4x4(L_(
                L_(
                        L_(S11,S12, S13,S14),
                        L_(S21,S22, S23,S24),

                        L_(S31,S32, S33,S34),
                        L_(S41,S42, S43,S44)
                ),
                L_(
                        L_(S11,S21, S31,S41),
                        L_(S12,S22, S32,S42),

                        L_(S13,S23, S33,S43),
                        L_(S14,S24, S34,S44)
                ),
                L_(
                        L_(S11,S12, S21,S22),
                        L_(S13,S14, S23,S24),

                        L_(S31,S32, S41,S42),
                        L_(S33,S34, S43,S44)
                )
        )))
                .if_(true_());

        say_(sudoku(Xss))
                .if_(   s4x4(P_(Xss,Xsss)),
                        map11(  C_("permute"),
                                L_(C_("1"),C_("2"),C_("3"),C_("4")),
                                P_(Xss,Xsss)  )  );

        say_(map1x(_,_,L_()));
        say_(map1x(F,Y,P_(X,Xs)))
                .if_(   call(F,Y,X),map1x(F,Y,Xs)
                );

        say_(map11(_,_,L_()));
        say_(map11(F,X,P_(Y,Ys)))
                .if_(   map1x(F,X,Y),
                        map11(F,X,Ys) );

        say_(permute(L_(),L_()));
        say_(permute(P_(X,Xs),Zs))
                .if_(   permute(Xs,Ys),ins(X,Ys,Zs)  );

        say_(ins(X,Xs,P_(X,Xs)));
        say_(ins(X,P_(Y,Xs),P_(Y,Ys)))
                .if_(   ins(X,Xs,Ys)  );

        say_(good_(Xss)).if_(  sudoku(Xss));
    }

    LPv S11,S12,S13,S14;
    LPv S21,S22,S23,S24;

    LPv S31,S32,S33,S34;
    LPv S41,S42,S43,S44;

    LPv s4x4(LPv a)                 {return S_(a); }

    LPv sudoku(LPv a)               {return S_(a); }
    LPv map1x(LPv a, LPv b, LPv c)  {return S_(a,b,c);}
    LPv map11(LPv a, LPv b, LPv c)  {return S_(a,b,c);}
    LPv permute(LPv a, LPv b)       {return S_(a,b);}
    LPv ins(LPv a, LPv b, LPv c)    {return S_(a,b,c);}
    LPv good_(LPv a)                {return S_(a); }

    @Test
    public void mainTest() {

        String expected[] = {
                "[[1,2,3,4],[3,4,1,2],[2,3,4,1],[4,1,2,3]]",
                "[[1,2,3,4],[3,4,1,2],[2,1,4,3],[4,3,2,1]]",
                "[[1,2,3,4],[3,4,1,2],[4,1,2,3],[2,3,4,1]]",
                "[[1,2,3,4],[3,4,1,2],[4,3,2,1],[2,1,4,3]]",
                "[[1,2,3,4],[3,4,2,1],[2,1,4,3],[4,3,1,2]]"
        };

        try_it(said, expected, false);
    }
}
