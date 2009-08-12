#ifndef __TOKYOOO_HDB_HPP__
#define __TOKYOOO_HDB_HPP__

#include <string>
#include <map>
#include <exception>
#include <limits>
#include <malloc.h>
#include <cmath>

#include <boost/noncopyable.hpp>
#include <boost/cstdint.hpp>

#include <tcutil.h>
#include <tchdb.h>
#include "util.hpp"
#include "map.hpp"

namespace tokyooo {

class hdb : public boost::noncopyable
{
private:

  TCHDB * hdb_;

public:

  enum tune_options_e
  {
    tune_default = 0,
    large = HDBTLARGE,
    deflate = HDBTDEFLATE,
    bzip = HDBTBZIP,
    tcbs = HDBTTCBS
  };
  enum open_options_e
  {
    open_default = 0,
    writer = HDBOWRITER,
    reader = HDBOREADER,
    create = HDBOCREAT,
    trunc = HDBOTRUNC,
    synchronize = HDBOTSYNC,
    no_lock = HDBONOLCK,
    lock_nb = HDBOLCKNB
  };

  hdb( bool mutexed = false,
       boost::int64_t bnum = 131071,
       char apow = 4,
       char fpow = 10,
       tune_options_e opts = tune_default,
       int rcnum = 0,
       boost::int64_t xmsize = 67108864 )
  : hdb_(tchdbnew())
  {
    if (mutexed)
      set_mutexed();
    tune(bnum, apow, fpow, opts);
    set_cache(rcnum);
    set_extra_mm(xmsize);
  }

  hdb( const std::string & path,
       open_options_e options = open_default,
       bool mutexed = false,
       boost::int64_t bnum = 131071,
       char apow = 4,
       char fpow = 10,
       tune_options_e opts = tune_default,
       int rcnum = 0,
       boost::int64_t xmsize = 67108864 )
  : hdb_(tchdbnew())
  {
    if (mutexed)
      set_mutexed();
    tune(bnum, apow, fpow, opts);
    set_cache(rcnum);
    set_extra_mm(xmsize);
    open(path, options);
  }

  ~hdb()
  {
    tchdbclose(hdb_);
    tchdbdel(hdb_);
  }

  void set_mutexed()
  {
    tchdbsetmutex(hdb_) || err::go(hdb_);
  }

  void tune( boost::int64_t bnum, char apow, char fpow, tune_options_e opts )
  {
    tchdbtune(hdb_, bnum, apow, fpow, opts) || err::go(hdb_);
  }

  void set_cache(int rcnum)
  {
    tchdbsetcache(hdb_, rcnum) || err::go(hdb_);
  }

  void set_extra_mm(boost::int64_t xmsize)
  {
    tchdbsetxmsiz(hdb_, xmsize) || err::go(hdb_);
  }

  void open(const std::string & path, open_options_e options = open_default)
  {
    tchdbopen(hdb_, path.c_str(), options) || err::go(hdb_);
  }

  void close()
  {
    tchdbclose(hdb_) || err::go(hdb_);
  }

  // using this for structs and stuff - maybe alignment problems?  ymmv
  template<class Key, class Value>
  void put(const Key & key, const Value & value, put_mode_e put_mode = store)
  {
    switch (put_mode)
    {
    case store: tchdbput(hdb_, ser::cptr(key), ser::len(key), ser::cptr(value), ser::len(value)) || err::go(hdb_); break;
    case keep: tchdbputkeep(hdb_, ser::cptr(key), ser::len(key), ser::cptr(value), ser::len(value)) || err::go(hdb_); break;
    case cat: tchdbputcat(hdb_, ser::cptr(key), ser::len(key), ser::cptr(value), ser::len(value)) || err::go(hdb_); break;
    case async: tchdbputasync(hdb_, ser::cptr(key), ser::len(key), ser::cptr(value), ser::len(value) ) || err::go(hdb_); break;
    default: err::go("expardon me?"); break;
    }
  }

  template<class Key>
  void out(const Key & key)
  {
    tchdbout(hdb_, ser::cptr(key), ser::len(key)) || err::go(hdb_);
  }

  template<class Key, class Value>
  bool get(const Key & key, Value & value)
  {
    int size = 0;
    void * p = tchdbget( hdb_, ser::cptr(key), ser::len(key), &size );
    if (p == NULL)
      return false;
    ser::assign(value, p);
    std::free(p);
    return true;
  }

  template<class Key>
  int vsize(const Key & key)
  {
    int ret_val = tchdbvsiz( hdb_, ser::cptr(key), ser::len(key) );
    (ret_val != -1) || err::go(hdb_);
    return ret_val;
  }

  // TODO: iterators

  template<class Key>
  int add(const Key & key, int value)
  {
    int ret_val = tchdbaddint(hdb_, ser::cptr(key), ser::len(key), value);
    ( ret_val == std::numeric_limits<int>::min() ) || err::go(hdb_);
    return ret_val;
  }

  template<class Key>
  int add(const Key & key, double value)
  {
    double ret_val = tchdbadddouble(hdb_, ser::cptr(key), ser::len(key), value);
    !std::isnan(ret_val) || err::go(hdb_);
    return ret_val;
  }

  void sync()
  {
    tchdbsync(hdb_) || err::go(hdb_);
  }

  void optimize( boost::int64_t bnum, char apow, char fpow, tune_options_e opts )
  {
    tchdboptimize(hdb_, bnum, apow, fpow, opts) || err::go(hdb_);
  }

  void vanish()
  {
    tchdbvanish(hdb_) || err::go(hdb_);
  }

  void copy(const std::string & path)
  {
    tchdbcopy(hdb_, path.c_str()) || err::go(hdb_);
  }

  std::string path()
  {
    return std::string( tchdbpath(hdb_) );
  }

  size_type size()
  {
    return tchdbrnum(hdb_);
  }

  size_type fsize()
  {
    return tchdbfsiz(hdb_);
  }

  TCHDB * native()
  {
    return hdb_;
  }

};

} // tokyooo

#endif // __TOKYOOO_HDB_HPP__
