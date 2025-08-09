package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestAdd extends TestTerm {
    LPv the_sum_of_(LPv a1, LPv a2, LPv Sum) {  return S_(a1,a2,Sum); } // args: triple
    LPv X,Y,Z,R;
    TestAdd() {
        say_( the_sum_of_(zero_.and_,X.is_,X));
        say_( the_sum_of_(one_plus_(X).and_,Y.is_,one_plus_(Z)) )
                .if_( the_sum_of_(X.and_,Y.is_,Z) );
        say_( good_(R)).if_(the_sum_of_(two_.and_,two_.is_, R));
    }

    LPv one_plus_(LPv x) {  return S_(x); } // args: singleton structure
    LPv zero_ = L_();   // empty list, will show as "nil"
    LPv one_  = one_plus_(zero_);
    LPv two_  = one_plus_(one_);

    @Test
    public void mainTest() {
        String[] answer = {
                "one_plus_(one_plus_(one_plus_(one_plus_(nil))))"
        };
        System.out.println ("Code so far-------------\n" + this.said.toString() + "\n---------");
        try_it(said, answer);
    }
}
