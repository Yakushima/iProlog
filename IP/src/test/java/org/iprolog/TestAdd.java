package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestAdd extends TestTerm {
    LPv zero = L_();   // should show as "nil"

    LPv one_plus(LPv x)                    {  return S_(x);          }
    LPv one = one_plus(zero);
    LPv two = one_plus(one);

    LPv the_sum_of(LPv a1, LPv a2, LPv Sum)    {  return S_(a1,a2,Sum);  }
    LPv X,Y,Z;
    void define_the_sum_of() {
        say_( the_sum_of(zero.and_, X.is_, X));
        say_( the_sum_of(one_plus(X).and_, Y.is_, one_plus(Z)) )
                .if_( the_sum_of(X.and_,Y.is_,Z) );
    }

    LPv goal(LPv x)                                {  return S_(x);         }
    LPv R;

    @Test
    public void mainTest() {
        start_new_test();

        define_the_sum_of();

        say_( goal(R)).if_(the_sum_of(two, two, R));

        String[] answer = {
                "one_plus(one_plus(one_plus(one_plus(nil))))"
        };

        System.out.println ("Code so far-------------\n" + this.said.toString() + "\n---------");

        try_it(said, answer);
    }
}
