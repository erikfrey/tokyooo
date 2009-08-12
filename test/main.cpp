#include <iostream>
#include <tokyooo/map.hpp>
#include <tokyooo/rdb.hpp>
#include <tokyooo/hdb.hpp>
#include <tokyooo/list.hpp>
#include <tokyooo/query.hpp>

using namespace tokyooo;

int main(int argc, char * argv[])
{
  rdb r("localhost", 1978);

  map row;
  row.put("xy", "10"); r.tbl_put(1, row);
  row.put("xy", "20 20"); r.tbl_put(2, row);
  row.put("xy", "-500 20"); r.tbl_put(3, row);
  row.put("xy", "95671"); r.tbl_put(4, row);

  list keys;

  query q(r);

  q.cond("xy", query::str_has_some, "20").order("xy", query::str_desc).limit(10, 0).search_keys(keys);

  std::cout << q.hint() << std::endl;

  int x;
  for (int i = 0; keys.get(x, i); ++i )
    std::cout << x << std::endl;

  hdb h("hdb_test");

}
