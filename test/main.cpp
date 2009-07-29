#include <iostream>
#include <tokyooo/map.hpp>
#include <tokyooo/rdb.hpp>
#include <tokyooo/list.hpp>
#include <tokyooo/query.hpp>

using namespace tokyooo;

int main(int argc, char * argv[])
{
  map m;

  m.put( 3, "pooppppp" );

  std::string ret;

  m.get(3, ret);

  std::cout << ret << std::endl;

  list l;

  l.push(3);
}
