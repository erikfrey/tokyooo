#ifndef __TOKYOOO_LIST_HPP__
#define __TOKYOOO_LIST_HPP__

#include <malloc.h>
#include <iterator>
#include <tcutil.h>
#include "util.hpp"

namespace tokyooo {

class list
{
private:

  TCLIST * list_;

public:

  list() : list_(tclistnew()) {}

  list(int num) : list_(tclistnew2(num)) {}

  template< class InputIterator >
  list( InputIterator begin, InputIterator end )
  : list_(tclistnew())
  {
    put_all(begin, end);
  }

  list(TCLIST * list) : list_(list) {} // take ownership

  list(const list & other)
  : list_( tclistdup(other.list_) ) {}

  list & operator= (const list & other)
  {
    tclistdel(list_);
    list_ = tclistdup(other.list_);
    return *this;
  }

  ~list()
  {
    tclistdel(list_);
  }

  size_type size() const
  {
    return tclistnum(list_);
  }

  template<class Value>
  bool get(Value & value, int index)
  {
    int size = 0;
    const void * p = tclistval( list_, index, &size );
    if (p == NULL)
      return false;
    ser::assign(value, p, size);
    return true;
  }

  template<class Value>
  void push(const Value & value)
  {
    tclistpush( list_, ser::cptr(value), ser::len(value) );
  }

  bool pop()
  {
    int size = 0;
    void * p = tclistpop( list_, &size );
    if (p == NULL)
      return false;
    std::free(p);
    return true;
  }

  template<class Value>
  bool pop(Value & value)
  {
    int size = 0;
    const void * p = tclistpop( list_, &size );
    if (p == NULL)
      return false;
    ser::assign(value, p, size);
    std::free(p);
    return true;
  }

  template<class Value>
  void unshift(const Value & value)
  {
    tclistunshift( list_, ser::cptr(value), ser::len(value) );
  }

  bool shift()
  {
    int size = 0;
    void * p = tclistshift( list_, &size );
    if (p == NULL)
      return false;
    std::free(p);
    return true;
  }

  template<class Value>
  bool shift(Value & value)
  {
    int size = 0;
    void * p = tclistshift( list_, &size );
    if (p == NULL)
      return false;
    ser::assign(value, p, size);
    std::free(p);
    return true;
  }

  template<class Value>
  void insert(const Value & value, int index)
  {
    tclistinsert( list_, index, ser::cptr(value), ser::len(value) );
  }

  bool remove(int index)
  {
    int size = 0;
    void * p = tclistremove( list_, index, &size );
    if (p == NULL)
      return false;
    std::free(p);
    return true;
  }

  template<class Value>
  bool remove(Value & value, int index)
  {
    int size = 0;
    const void * p = tclistremove( list_, index, &size );
    if (p == NULL)
      return false;
    ser::assign(value, p, size);
    std::free(p);
    return true;
  }

  template<class Value>
  void set(const Value & value, int index)
  {
    tclistover(list_, index, ser::cptr(value), ser::len(value));
  }

  void sort()
  {
    tclistsort(list_);
  }

  template<class Value>
  int find(const Value & value)
  {
    return tclistsearch(list_, ser::cptr(value), ser::len(value));
  }

  template<class Value>
  int search(const Value & value)
  {
    return tclistbsearch(list_, ser::cptr(value), ser::len(value));
  }

  void clear()
  {
    tclistclear(list_);
  }

  void swap(list & other)
  {
    TCLIST * tmp = list_;
    list_ = other.list_;
    other.list_ = tmp;
  }

  TCLIST * native() { return list_; }
};

} // tokyooo

#endif // __TOKYOOO_LIST_HPP__
