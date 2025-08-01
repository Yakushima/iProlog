package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestJapaneseCode extends TestTerm {
        LPv いきる(LPv x)    {  return S_(x);      }
        LPv いいです(LPv x) {  return S_(x);      }
        LPv いい(LPv x)     {  return S_(x);      }
        LPv goal(LPv x)    {  return S_(x);      }
        LPv 人;

        @Test
        public void mainTest() {
            start_new_test();

            String expected[] = {"私", "あなた"};
            for (String s : expected) {
                say_(いきる(C_(s)));
                say_(いい(C_(s)));
            }
            say_(いいです (人)).if_( いきる (人),いい(人));
            say_(goal(人)).if_(いいです (人));

            try_it(said, expected);
        }
}
