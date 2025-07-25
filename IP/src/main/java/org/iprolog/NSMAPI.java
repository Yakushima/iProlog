package org.iprolog;

import java.util.LinkedList;

public class NSMAPI extends JLPAPI {
    public LinkedList<Clause> said;

    public Clause say_(Clause cl) {
        assert said != null;
        said.add (cl);
        return cl;
    }

    public Clause say_(LPvar hd) {
        assert said != null;
        Term t = hd.run.fn();
        assert t != null;
        Clause cl = yes_(hd.run.fn());
        assert cl != null;
        said.add(cl);
        return cl;
    }
    public Clause say_(Term hd)   {
        assert hd != null;
        assert said != null;
        Clause cl = yes_(hd);
        assert cl != null;
        if (said == null) System.out.println ("!!!!!!!!! null said !!!!");
        said.add (cl);
        return cl;
    }

    public Prog compile() {
        StringBuilder asm_txt;
        Term.reset_gensym();
        Term.set_TarauLog();

        // Main.println ("   ===== try_it(): flattening transform =======");
        asm_txt = new StringBuilder();
        for (Clause cl : said) {
            cl.flatten();
            asm_txt.append(cl.toString()).append(System.lineSeparator());
        }
        // Main.println ("   ===== try_it(): after flattening =======");
        // Main.println ("asm_txt = \n" + asm_txt);

        // Main.println ("   ===== try_it(): Calling new Prog: ===============");
        return new Prog(asm_txt.toString(), false);
    }
}
