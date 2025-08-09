package org.iprolog;

public class LPv {
    TermFn run;
    // stupid sugar:
    LPv is_ = this;
    LPv and_ = this;

    LPv() { run = null; }
    LPv(TermFn r) { run = r; }
}
