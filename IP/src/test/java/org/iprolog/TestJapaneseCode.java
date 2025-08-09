package org.iprolog;

import org.junit.jupiter.api.Test;

public class TestJapaneseCode extends TestTerm {
        TestJapaneseCode() {
            for (String s : expected) {
                say_(いきる(C_(s)));
                say_(いい(C_(s)));
            }
            say_(いいです (人)).if_( いきる (人),いい(人));
            say_(good_(人)).if_(いいです (人));
        }
        LPv いきる(LPv x)    {  return S_(x);      }
        LPv いいです(LPv x)  {  return S_(x);      }
        LPv いい(LPv x)     {  return S_(x);      }
        LPv 人;
        String expected[] = {"私", "あなた"};

        @Test
        public void mainTest() {
            try_it(said, expected);
        }
}
