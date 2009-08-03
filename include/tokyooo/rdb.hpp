#ifndef __TOKYOOO_RDB_HPP__
#define __TOKYOOO_RDB_HPP__

#include <string>
#include <map>
#include <exception>
#include <limits>
#include <malloc.h>
#include <cmath>

#include <boost/noncopyable.hpp>

#include <tcutil.h>
#include <tcrdb.h>
#include "util.hpp"
#include "map.hpp"

namespace tokyooo {

class rdb : public boost::noncopyable
{
private:

  TCRDB * rdb_;

public:

  enum open_options_e
  {
    open_default = 0,
    reconnect = RDBTRECON
  };
  enum ext_options_e
  {
    ext_default = 0,
    record_lock = RDBXOLCKREC,
    global_lock = RDBXOLCKGLB
  };
  enum restore_options_e
  {
    restore_default = 0,
    check_consistency = RDBROCHKCON
  };
  enum index_options_e
  {
    lexical = RDBITLEXICAL,
    decimal = RDBITDECIMAL,
    token = RDBITTOKEN,
    qgram = RDBITQGRAM,
    optimized = RDBITOPT,
    remove = RDBITVOID,
    keep = RDBITKEEP
  };

  rdb()
  : rdb_(tcrdbnew()) {}

  rdb(const std::string & host, int port, double timeout = 0, open_options_e options = open_default)
  : rdb_(tcrdbnew())
  {
    open(host, port, timeout, options);
  }

  void open(const std::string & host, int port, double timeout = 0, open_options_e options = open_default)
  {
    tcrdbtune(rdb_, timeout, options );
    tcrdbopen(rdb_, host.c_str(), port) || err::go(rdb_);
  }

  void close()
  {
    tcrdbclose(rdb_) || err::go(rdb_);
  }

  // using this for structs and stuff - maybe alignment problems?  ymmv
  template<class Key, class Value>
  void put(const Key & key, const Value & value, put_mode_e put_mode = store, int width = 0)
  {
    switch (put_mode)
    {
    case store: tcrdbput(rdb_, ser::cptr(key), ser::len(key), ser::cptr(value), ser::len(value)) || err::go(rdb_); break;
    case keep: tcrdbputkeep(rdb_, ser::cptr(key), ser::len(key), ser::cptr(value), ser::len(value)) || err::go(rdb_); break;
    case cat: tcrdbputcat(rdb_, ser::cptr(key), ser::len(key), ser::cptr(value), ser::len(value)) || err::go(rdb_); break;
    case shl: tcrdbputshl(rdb_, ser::cptr(key), ser::len(key), ser::cptr(value), ser::len(value), width) || err::go(rdb_); break;
    case nr: tcrdbputnr(rdb_, ser::cptr(key), ser::len(key), ser::cptr(value), ser::len(value)) || err::go(rdb_); break;
    default: err::go("expardon me?"); break;
    }
  }

  template<class Key>
  void out(const Key & key)
  {
    tcrdbout(rdb_, ser::cptr(key), ser::len(key)) || err::go(rdb_);
  }

  template<class Key, class Value>
  bool get(const Key & key, Value & value)
  {
    int size = 0;
    void * p = tcrdbget( rdb_, ser::cptr(key), ser::len(key), &size );
    if (p == NULL)
      return false;
    ser::assign(value, p);
    std::free(p);
    return true;
  }

  void get(map & m)
  {
    tcrdbget3(rdb_, m.native()) || err::go(rdb_);
  }

  template<class Key>
  int vsize(const Key & key)
  {
    int ret_val = tcrdbvsiz( rdb_, ser::cptr(key), ser::len(key) );
    (ret_val != -1) || err::go(rdb_);
    return ret_val;
  }

  // TODO: iterators

  template<class Key>
  int add(const Key & key, int value)
  {
    int ret_val = tcrdbaddint(rdb_, ser::cptr(key), ser::len(key), value);
    ( ret_val == std::numeric_limits<int>::min() ) || err::go(rdb_);
    return ret_val;
  }

  template<class Key>
  int add(const Key & key, double value)
  {
    double ret_val = tcrdbadddouble(rdb_, ser::cptr(key), ser::len(key), value);
    !std::isnan(ret_val) || err::go(rdb_);
    return ret_val;
  }

  template<class Key, class Value, class Result>
  void ext(const std::string & name, const Key & key, const Value & value, Result & result,
      ext_options_e options = ext_default)
  {
    int size = 0;
    void * p = tcrdbext( rdb_, name.c_str(), options, ser::cptr(key), ser::len(key), ser::cptr(value),
        ser::len(value), &size );
    p || err::go(rdb_);
    ser::assign(value, p);
    std::free(p);
  }

  void sync()
  {
    tcrdbsync(rdb_) || err::go(rdb_);
  }

  void optimize(const std::string & params = "")
  {
    if (params.empty())
      tcrdboptimize(rdb_, NULL) || err::go(rdb_);
    else
      tcrdboptimize(rdb_, params.c_str()) || err::go(rdb_);
  }

  void vanish()
  {
    tcrdbvanish(rdb_) || err::go(rdb_);
  }

  void copy(const std::string & path)
  {
    tcrdbcopy(rdb_, path.c_str()) || err::go(rdb_);
  }

  void restore(const std::string & path, size_type timestamp, restore_options_e options = restore_default )
  {
    tcrdbrestore(rdb_, path.c_str(), timestamp, options) || err::go(rdb_);
  }

  void set_master(const std::string & host, int port, size_type timestamp, restore_options_e options = restore_default)
  {
    tcrdbsetmst(rdb_, host.c_str(), port, timestamp, options) || err::go(rdb_);
  }

  size_type size()
  {
    return tcrdbrnum(rdb_);
  }

  size_type msize()
  {
    return tcrdbsize(rdb_);
  }

  std::string status()
  {
    std::string ret_val;
    char * p = tcrdbstat(rdb_);
    p || err::go(rdb_);
    ret_val = p;
    std::free(p);
    return ret_val;
  }

  // table api:
  template<class Key>
  void tbl_put( const Key & key, map & row, put_mode_e mode = store )
  {
    switch (mode)
    {
    case store: tcrdbtblput(rdb_, ser::cptr(key), ser::len(key), row.native() ) || err::go(rdb_); break;
    case keep: tcrdbtblputkeep(rdb_, ser::cptr(key), ser::len(key), row.native() ) || err::go(rdb_); break;
    case cat: tcrdbtblputcat(rdb_, ser::cptr(key), ser::len(key), row.native() ) || err::go(rdb_); break;
    default: err::go("expardon me?"); break;
    }
  }

  template<class Key>
  void tbl_out(const Key & key)
  {
    tcrdbtblout(rdb_, ser::cptr(key), ser::len(key)) || err::go(rdb_);
  }

  template<class Key>
  bool tbl_get(const Key & key, map & m)
  {
    map tmp( tcrdbtblget( rdb_, ser::cptr(key), ser::len(key) ) );
    if (!tmp.native())
      return false;
    m.swap(tmp);
    return true;
  }

  void set_index( const std::string & name, index_options_e options )
  {
    tcrdbtblsetindex(rdb_, name.c_str(), options) || err::go(rdb_);
  }

  uid genuid()
  {
    return tcrdbtblgenuid(rdb_);
  }

  TCRDB * native()
  {
    return rdb_;
  }

};

} // tokyooo

#endif // __TOKYOOO_RDB_HPP__
