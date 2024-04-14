/*
* iProlog / C++  [derived from Java version]
* License: Apache 2.0
* Copyright(c) 2017 Paul Tarau
*/

#include <iostream>
#include "index.h"
#include "IMap.h"

namespace iProlog {

    using namespace std;

  bool IMap::put(ClauseNumber cls_no, cell this_cell) {
#define TR if(1)
      TR cout << "      IMap::put(cls_no_box->i=" << cls_no.as_int() << ", this_cell=" << this_cell.show() << ")" << endl;
      TR cout << "      In IMap::put(" << cls_no.as_int() << ", this_cell=" << this_cell.show() << ")" << endl;

      cout << "        map.size() == "<< map.size() << endl;

      TRY{
        if (map.count(this_cell) == 0) {
            cls_no_to_cell c2c;
            map.insert(std::pair<cell, cls_no_to_cell>(this_cell, c2c));
            cout << "           map.at(" << this_cell.show() << ")=" << map.at(this_cell).show() << endl;
        }
      } CATCH("Was trying to make new map")

      bool res;
      TRY{
        res = map.at(this_cell).add_key(cls_no.as_int());
      } CATCH("Was trying to add key to map")

      TR cout << "      IMap::put(...): returning with get_cell_to_cls_no result " << get_cls_no_to_cell(this_cell).show() << endl;

      return res;
#undef TR
  }

  cls_no_to_cell IMap::get_cls_no_to_cell(cell cx) {
#define TR if(0)
      TR cout << "         **** get_clause_no_to_cell(cx=" << cx.show() << ")"  << endl;

      return map.at(cx);
#undef TR
  }

  string IMap::toString() const {
      return "IMap::toString() <stub>";
  }

  // "specialization for array of int maps"

  vector<IMap> IMap::create(int l) {
      IMap first;
      vector<IMap> imaps = vector<IMap>(l);
      imaps[0] = first;
      for (int i = 1; i < l; i++)
          imaps[i] = IMap();
      return imaps;
  }

  string IMap::show() const {
    string s = "IMap:{";
    for (const std::pair<const cell, cls_no_to_cell>& n : map) {
        s += "<k:[" + to_string(n.first.as_int());
        s += "-> v[" + n.second.show();
        s += ">";
    }
    s += "}";
    return s;
  }

  string IMap::show(const vector<IMap> &imaps) {
    // return Arrays.toString(imaps); // Java
      size_t sz = imaps.size();
    string s = "[";
    string sep = "";
    for (size_t i = 0; i < sz; ++i) {
	    s += sep;
	    sep = ",";
	    s += imaps[i].show();
    }
    s += "]";
    return s;
  }

  string IMap::show(vector<int> is) {
      string s = "{";
      for (int i = 0; i < is.size(); ++i)
          s += "<stub>";
      s += "}";

      return s;
  }

  /*
  public static void main(final String[] args) {
    final IMap<Integer>[] imaps = create(3);
    put(imaps, 0, 10, 100);
    put(imaps, 1, 20, 200);
    put(imaps, 2, 30, 777);

    put(imaps, 0, 10, 1000);
    put(imaps, 1, 20, 777);
    put(imaps, 2, 30, 3000);

    put(imaps, 0, 10, 777);
    put(imaps, 1, 20, 20000);
    put(imaps, 2, 30, 30000);

    put(imaps, 0, 10, 888);
    put(imaps, 1, 20, 888);
    put(imaps, 2, 30, 888);

    put(imaps, 0, 10, 0);
    put(imaps, 1, 20, 0);
    put(imaps, 2, 30, 0);

    //Main.pp(show(imaps));

    //final int[] keys = { 10, 20, 30 };
    //Main.pp("get=" + show(get(imaps, keys)));


    final IMap<Integer>[] m = create(4);
    Engine.put(m, new int[] { -3, -4, 0, 0 }, 0);
    Engine.put(m, new int[] { -3, -21, 0, -21 }, 1);
    Engine.put(m, new int[] { -19, 0, 0, 0 }, 2);
    
    final int[] ks = new int[] { -3, -21, -21, 0 };
    Main.pp(show(m));
    Main.pp("ks=" + Arrays.toString(ks));
    
    Main.pp("get=" + show(get(m, ks)));

  }*/
}
