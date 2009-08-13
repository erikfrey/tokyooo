#ifndef __TOKYOPP_UTIL_HPP__
#define __TOKYOPP_UTIL_HPP__

#include <vector>

#include <tcrdb.h>
#include <tchdb.h>
#include <string>
#include <cstring>
#include <stdexcept>
#include <boost/cstdint.hpp>

namespace tokyooo {

enum put_mode_e
{
  store,  // store a record
  keep,   // store a new record
  cat,    // concatenate a value at the end of the existing record
  shl,    // concatenate a value at the end of the existing record and shift it to the left
  nr,     // store a record without response from the server
  async,  // store in an asynchronous fashion
};

struct err
{
  static bool go(TCRDB * rdb)
  {
    throw std::runtime_error( tcrdberrmsg( tcrdbecode ( rdb ) ) );
    return true;
  }

  static bool go(TCHDB * rdb)
  {
    throw std::runtime_error( tchdberrmsg( tchdbecode ( rdb ) ) );
    return true;
  }

  static bool go(const std::string & e)
  {
    throw std::runtime_error( e.c_str() );
    return true;
  }
};

// TODO: a more formal approach here, with probably default support for stream operator, and optional boost::archive
struct ser
{
  static const void * cptr(const std::string & value) { return value.c_str(); }
  static const void * cptr(const char * value) { return value; }
  template<class T> static const void * cptr( const std::vector<T> & value ) { return &value[0]; }
  template<class T> static const void * cptr(const T & value) { return &value; }
  static int len(const std::string & value) { return value.size(); }
  static int len(const char * value) { return strlen(value); }
  template<class T> static int len(const std::vector<T> & value) { return value.size() * sizeof(T); }
  template<class T> static int len(const T & value) { return sizeof(T); }
  static void assign(std::string & value, const void * p, int len) { value = reinterpret_cast<const char *>(p); }
  template<class T> static void assign(std::vector<T> & value, const void * p, int len)
  { value.assign(*reinterpret_cast<const T*>(p), *reinterpret_cast<const T*>(p + len)); }
  template<class T> static void assign(T & value, const void * p, int len) { value = *reinterpret_cast<const T*>(p); }
};

typedef boost::uint64_t size_type;

typedef boost::int64_t uid;

} // tokyooo

#endif // __TOKYOPP_UTIL_HPP__
