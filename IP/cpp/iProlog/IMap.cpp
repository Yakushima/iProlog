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

  bool IMap::put(Integer *cls_no_box, cell this_cell) {
#define TR if(1)
      // assert(map.size() > 0);
      assert(cls_no_box != nullptr);
      TR cout << "      IMap::put(cls_no_box->i=" << cls_no_box->as_int() << ", this_cell=" << this_cell.as_int() << ")" << endl;
      Integer* cell_box = new Integer(this_cell);
#ifndef UOMAP
      int b = phash(cell_box);
#endif
      TR cout << "      In IMap::put(" << cls_no_box->as_int() << ", cls_no=" << this_cell.as_int() << ")" << endl;
#ifndef UOMAP
      if (map[b].uninit()) {
          // TR cout << "         In IMap::put making bucket ..." << endl;
          map[b] = bucket(cell_box, cls_no_to_cell());
          assert(!map[b].uninit());
          assert(map[b].cell_box != nullptr);
          assert(map[b].cell_box == cell_box);
          // TR cout << "==== new IMap entry with intmap size=" << map[b].cls_no_2_cell.size() << endl;
          // TR cout << "         In IMap::put bucket["<<b<<"] init done, result: " << map[b].show() << endl;
      }
#endif
      // TR cout << "      In IMap::put, about to insert intmap key as this_cell " << cell_box->as_int() << endl;
#ifndef UOMAP
      bool res = map[b].cls_no_2_cell.add_key(cls_no_box->as_int());
#else
      cell c = cell_box->as_int();

      cout << "        map.size() == "<< map.size() << endl;

      TRY{
        if (map.count(c) == 0) {
            cell c = cell_box->as_int();
            cls_no_to_cell c2c;
            map.insert(std::pair<cell, cls_no_to_cell>(c, c2c));
            cout << "           map.at(" << c.as_int() << ")=" << map.at(c).show() << endl;
        }
      } CATCH("Was trying to make new map")

      bool res;
      TRY{
        res = map.at(c).add_key(cls_no_box->as_int());
      } CATCH("Was trying to add key to map")

#endif

#ifndef UOMAP
      // TR cout << "      In IMap::put: rslt after insert for key=cell " << cell_box->as_int() << ": " << map[b].cls_no_2_cell.show() << endl;
      // TR cout << "    ... returning from IMap::put(...)" << endl;
      // TR cout << "    ... with map[" << b << "]=" << map[b].show() << endl;
      assert(!map[b].uninit());
#endif
      TR cout << "      IMap::put(...): returning with get_cell_to_cls_no result " << get_cls_no_to_cell(cell_box).show() << endl;

      return res;
#undef TR
  }

  cls_no_to_cell IMap::get_cls_no_to_cell(Integer* cell_box) {
#define TR if(0)
      assert(cell_box != nullptr);
#ifndef UOMAP
      int b = phash(cell_box);
#endif
      TR cout << "         **** get_clause_no_to_cell(ip->i=" << cell_box->as_int() << ")"  << endl;
#ifndef UOMAP
      if (!map[b].uninit()) {
          TR cout << "          ***** Got init'ed bucket, with map[" << b << "] intmap: " << map[b].cls_no_2_cell.show() << endl;
      }

      TRY{ if (map[b].uninit()) {
                // TR cout << "          Making new cls_no_to_cell to go into map[" << b << "]" << endl;
                cls_no_to_cell the_intmap = cls_no_to_cell();
                // TR cout << "          Made blank intmap" << endl;
                bucket xb = bucket(cell_box, the_intmap);
                // TR cout << "          Made bucket with it using cell_box->i" << cell_box->as_int() << endl;
                map[b] = xb;
                // TR cout << "          assigned to map[" << b << "]" << endl;
                assert(!map[b].uninit());
           }
      } CATCH("    IMap::get - exception on map[b].uninit() test or s = cl_no_to_cell constructor - ")
#endif

#ifndef UOMAP
      TR cout << "         ***** get_cls_no_to_cell: result map[" << b << "] intmap:" << map[b].cls_no_2_cell.show() << endl;
      return map[b].cls_no_2_cell;
#else
      return map.at(cell_box->as_int());
#endif
#undef TR
  }

#ifndef UOMAP
  // N.B.: O(n)
  size_t IMap::amount() const {
    size_t s = 0;
    for (bucket b : map) {
        s += b.cls_no_2_cell.size();
    }
    return s;
  }
#endif

#if 0
  set<Integer *> IMap::keySet() {
      set <Integer*> s;
      for (bucket b : map)
          if (b.key != nullptr)
              s.insert(b.key);
    return s;
  }
#endif

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

#ifndef UOMAP
  string IMap::show(const bucket &b) {
    string s = "@";
    return s;
  }
#endif

  string IMap::show() const {
    string s = "IMap:{";
#ifndef UOMAP
    string sep = "";
    for (int i = 0; i < (int) map.size(); ++i) {
        if (map[i].uninit())
            continue;
	    s += sep;
	    sep = ", ";
	    s += map[i].show();
    }
#else
    for (const std::pair<const cell, cls_no_to_cell>& n : map) {
        s += "<k:[" + to_string(n.first.as_int());
        s += "-> v[" + n.second.show();
        s += ">";
    }
#endif
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

  string IMap::show(vector<Integer *> is) {
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
