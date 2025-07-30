package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestAdd extends TestTerm {

    LPv the_sum_of(LPv a1, LPv a2, LPv Sum)    {  return S_(a1,a2,Sum);  }
    LPv the_successor_of(LPv x)                    {  return S_(x);          }
    LPv R;
    LPv X,Y,Z;
    LPv goal(LPv x)                                {  return S_(x);          }

    @Test
    public void mainTest() {
        start_new_test();

        LPv zero = C_("0");
        LPv one = the_successor_of(zero);
        LPv two = the_successor_of(one);

        say_( the_sum_of(zero, X, X));
        say_( the_sum_of(the_successor_of(X), Y, the_successor_of(Z)) )
                .if_( the_sum_of(X,Y,Z) );

        say_( goal(R)).if_(the_sum_of(two, two, R));

        String[] answer = {
                "the_successor_of(the_successor_of(the_successor_of(the_successor_of(0))))"
        };

        try_it(said, answer);
    }
}
