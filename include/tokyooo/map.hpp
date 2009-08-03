#ifndef __TOKYOOO_MAP_HPP__
#define __TOKYOOO_MAP_HPP__

#include <iterator>
#include <tcutil.h>
#include "util.hpp"

namespace tokyooo {

class map
{
private:

  TCMAP * map_;

public:

  map() : map_(tcmapnew()) {}

  map(unsigned int bnum) : map_(tcmapnew2(bnum)) {}

  template< class InputIterator >
  map( InputIterator begin, InputIterator end )
  : map_(tcmapnew())
  {
    put_all(begin, end);
  }

  map(TCMAP * map) : map_(map) {} // take ownership

  map(const map & other)
  : map_( tcmapdup(other.map_) ) {}

  map & operator= (const map & other)
  {
    tcmapdel(map_);
    map_ = tcmapdup(other.map_);
    return *this;
  }

  ~map()
  {
    tcmapdel(map_);
  }

  template< class Key, class Value >
  void put(const Key & key, const Value & value, put_mode_e put_mode = store)
  {
    switch (put_mode)
    {
    case store: tcmapput(map_, ser::cptr(key), ser::len(key), ser::cptr(value), ser::len(value)); break;
    case keep: tcmapputkeep(map_, ser::cptr(key), ser::len(key), ser::cptr(value), ser::len(value)); break;
    case cat: tcmapputcat(map_, ser::cptr(key), ser::len(key), ser::cptr(value), ser::len(value)); break;
    default: err::go("expardon me?"); break;
    }
  }

  template< class ForwardIterator >
  void put_all(ForwardIterator begin, ForwardIterator end, put_mode_e put_mode = put)
  {
    for (; begin != end; ++begin)
      put(begin->first, begin->second, put_mode);
  }

  template<class Key>
  bool out(const Key & key)
  {
    return tcmapout(map_, ser::cptr(key), ser::len(key));
  }

  template<class Key, class Value>
  bool get(const Key & key, Value & value)
  {
    int size = 0;
    const void * p = tcmapget( map_, ser::cptr(key), ser::len(key), &size );
    if (p == NULL)
      return false;
    ser::assign(value, p);
    return true;
  }

  template<class Key>
  bool move(const Key & key, bool head)
  {
    return tcmapmove(map_, ser::cptr(key), ser::len(key), head);
  }

  // TODO: iterators i guess, tcmapiterinit() and friends

  size_type size() const
  {
    return tcmaprnum(map_);
  }

  size_type msize() const
  {
    return tcmapmsiz(map_);
  }

  void clear()
  {
    tcmapclear(map_);
  }

  template<class Key>
  int add(const Key & key, int value)
  {
    return tcmapaddint(map_, ser::cptr(key), ser::len(key), value);
  }

  template<class Key>
  double add(const Key & key, double value)
  {
    return tcmapadddouble(map_, ser::cptr(key), ser::len(key), value);
  }

  void cutfront(int count)
  {
    tcmapcutfront(map_, count);
  }

  void swap(map & other)
  {
    TCMAP * tmp = map_;
    map_ = other.map_;
    other.map_ = tmp;
  }

  TCMAP * native() { return map_; }
};

} // tokyooo

#endif // __TOKYOOO_MAP_HPP__
