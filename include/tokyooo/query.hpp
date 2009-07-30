#ifndef __TOKYOOO_QUERY_HPP__
#define __TOKYOOO_QUERY_HPP__

#include <tcrdb.h>
#include "rdb.hpp"
#include "list.hpp"

#include <boost/noncopyable.hpp>

namespace tokyooo {

class query : public boost::noncopyable
{
private:

  TCRDB * rdb_;

  RDBQRY * qry_;

public:

  enum op_e
  {
    str_equal = RDBQCSTREQ,         /* string is equal to */
    str_included_in = RDBQCSTRINC,  /* string is included in */
    str_begins = RDBQCSTRBW,        /* string begins with */
    str_ends = RDBQCSTREW,          /* string ends with */
    str_has_all = RDBQCSTRAND,      /* string includes all tokens in */
    str_has_some = RDBQCSTROR,      /* string includes at least one token in */
    str_equal_some = RDBQCSTROREQ,  /* string is equal to at least one token in */
    str_regex = RDBQCSTRRX,         /* string matches regular expressions of */
    num_equal = RDBQCNUMEQ,         /* number is equal to */
    num_greater = RDBQCNUMGT,       /* number is greater than */
    num_greater_equal = RDBQCNUMGE, /* number is greater than or equal to */
    num_less = RDBQCNUMLT,          /* number is less than */
    num_less_equal = RDBQCNUMLE,    /* number is less than or equal to */
    num_between = RDBQCNUMBT,       /* number is between two tokens of */
    num_equal_some = RDBQCNUMOREQ,  /* number is equal to at least one token in */
    text = RDBQCFTSPH,              /* full-text search with the phrase of */
    text_all = RDBQCFTSAND,         /* full-text search with all tokens in */
    text_some = RDBQCFTSOR,         /* full-text search with at least one token in */
    text_expression = RDBQCFTSEX,   /* full-text search with the compound expression of */
    op_not = RDBQCNEGATE,           /* negation flag */
    op_noindex = RDBQCNOIDX         /* no index flag */
  };

  enum order_e
  {
    str_asc  = RDBQOSTRASC,         /* string ascending */
    str_desc = RDBQOSTRDESC,        /* string descending */
    num_asc  = RDBQONUMASC,         /* number ascending */
    num_desc = RDBQONUMDESC         /* number descending */
  };

  query( rdb & rdb ) : rdb_(rdb.native()), qry_( tcrdbqrynew(rdb_) ) {}

  ~query()
  {
    tcrdbqrydel( qry_ );
  }

  query & cond(const std::string & name, op_e op, const std::string expr)
  {
    tcrdbqryaddcond(qry_, name.c_str(), op, expr.c_str());
    return *this;
  }

  query & order(const std::string & name, order_e order)
  {
    tcrdbqrysetorder(qry_, name.c_str(), order);
    return * this;
  }

  query & limit(int max, int skip)
  {
    tcrdbqrysetlimit(qry_, max, skip);
    return *this;
  }

  void search_keys( list & keys )
  {
    list tmp( tcrdbqrysearch(qry_) );
    tmp.native() || err(rdb_);
    keys.swap(tmp);
  }

  void search_rows( list & rows )
  {
    list tmp( tcrdbqrysearchget(qry_) );
    tmp.native() || err(rdb_);
    rows.swap(tmp);
  }

  void out()
  {
    tcrdbqrysearchout(qry_) || err(rdb_);
  }

  int count()
  {
    return tcrdbqrysearchcount(qry_);
  }

  std::string hint()
  {
    const char * ret_val = tcrdbqryhint(qry_);
    return std::string(ret_val);
  }
};

} // tokyooo

#endif // __TOKYOOO_QUERY_HPP__
