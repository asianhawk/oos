/*
 * This file is part of OpenObjectStore OOS.
 *
 * OpenObjectStore OOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenObjectStore OOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenObjectStore OOS. If not, see <http://www.gnu.org/licenses/>.
 */

#include "database/mssql/mssql_statement.hpp"
#include "database/mssql/mssql_database.hpp"
#include "database/mssql/mssql_exception.hpp"
#include "database/mssql/mssql_result.hpp"

#include "object/object_ptr.hpp"

#include "tools/varchar.hpp"

#include <stdexcept>
#include <iostream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <sstream>

namespace oos {

namespace mssql {

mssql_statement::mssql_statement(mssql_database &db)
  : db_(db)
{
//  std::cout << "creating mssql statement " << this << "\n";
  if (!db()) {
    throw_error("mssql", "no odbc connection established");
  }
  // create statement handle
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, db(), &stmt_);
  throw_error(ret, SQL_HANDLE_DBC, db(), "mssql", "error on creating sql statement");
}

mssql_statement::mssql_statement(mssql_database &db, const sql &s)
  : db_(db)
{
//  std::cout << "creating mssql statement " << this << "\n";
  if (!db()) {
    throw_error("mssql", "no odbc connection established");
  }
  // create statement handle
  SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, db(), &stmt_);
  throw_error(ret, SQL_HANDLE_DBC, db(), "mssql", "error on creating sql statement");
  prepare(s);
}
mssql_statement::~mssql_statement()
{
  clear();
}

void mssql_statement::prepare(const sql &s)
{
  reset();
  
  str(s.prepare());

  SQLRETURN ret = SQLPrepare(stmt_, (SQLCHAR*)str().c_str(), SQL_NTS);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, str());
}

void mssql_statement::reset()
{
  while (!host_data_.empty()) {
    delete host_data_.back();
    host_data_.pop_back();
  }
}

void mssql_statement::clear()
{
//  std::cout << "destroying mssql statement " << this << "\n";
  reset();
  SQLFreeHandle(SQL_HANDLE_STMT, stmt_);
}

result* mssql_statement::execute()
{
//  std::cout << str() << "\n";

  SQLRETURN ret = SQLExecute(stmt_);

  // check result
  throw_error(ret, SQL_HANDLE_STMT, stmt_, str(), "error on query execute");

  return new mssql_result(stmt_, false);
}

void mssql_statement::write(const char *, char x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, short x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, int x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, long x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, unsigned char x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, unsigned short x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, unsigned int x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, unsigned long x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, bool x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, float x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, double x)
{
  bind_value(x, ++host_index);
}

void mssql_statement::write(const char *, const char *x, int s)
{
  bind_value(x, s, ++host_index);
}

void mssql_statement::write(const char *, const std::string &x)
{
  bind_value(x.data(), x.size(), ++host_index);
}

void mssql_statement::write(const char *, const varchar_base &x)
{
  bind_value(x.c_str(), x.size() + 1, ++host_index);
}

void mssql_statement::write(const char *, const object_base_ptr &x)
{
  bind_value(x.id(), ++host_index);
}

void mssql_statement::write(const char *, const object_container &)
{
}

void mssql_statement::bind_value(unsigned long val, int index)
{
  value_t *v = new value_t(true, SQL_NTS);
  v->data = new char[NUMERIC_LEN];
  
  host_data_.push_back(v);

#if defined(_MSC_VER)
  size_t size = (int)_snprintf_s(static_cast<char*>(v->data), NUMERIC_LEN, NUMERIC_LEN, "%lu", val);
#else
  size_t size = (int)snprintf(static_cast<char*>(v->data), NUMERIC_LEN, "%lu", val);
#endif

  SQLRETURN ret = SQLBindParameter(stmt_, index, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, size, 0, v->data, 0, &v->len);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
}

void mssql_statement::bind_value(const char *val, int /*size*/, int index)
{
  size_t s = strlen(val);
  value_t *v = new value_t(false, SQL_NTS);
  v->data = new char[s+1];
  v->data = strncpy((char*)v->data, val, s);
  ((char*)v->data)[s++] = '\0';

  host_data_.push_back(v);

//  std::cout << "binding character string [" << val << "] (size: " << strlen(val) << ", max: " << size << ")\n";
  SQLRETURN ret = SQLBindParameter(stmt_, index, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, s, 0, v->data, 0, &v->len);
  throw_error(ret, SQL_HANDLE_STMT, stmt_, "mssql", "couldn't bind parameter");
}

database& mssql_statement::db()
{
  return db_;
}

const database& mssql_statement::db() const
{
  return db_;
}

long mssql_statement::type2int(data_type_t type)
{
  switch(type) {
    case type_char:
      return SQL_C_STINYINT;
    case type_short:
      return SQL_C_SSHORT;
    case type_int:
      return SQL_C_SLONG;
    case type_long:
      return SQL_C_SLONG;
    case type_unsigned_char:
      return SQL_C_UTINYINT;
    case type_unsigned_short:
      return SQL_C_USHORT;
    case type_unsigned_int:
      return SQL_C_ULONG;
    case type_unsigned_long:
      return SQL_C_CHAR;
    case type_bool:
      return SQL_C_USHORT;
    case type_float:
      return SQL_C_FLOAT;
    case type_double:
      return SQL_C_DOUBLE;
    case type_char_pointer:
      return SQL_C_CHAR;
    case type_varchar:
      return SQL_C_CHAR;
    case type_text:
      return SQL_C_CHAR;
    default:
      {
        throw std::logic_error("mssql statement: unknown type");
      }
    }
}

long mssql_statement::type2sql(data_type_t type)
{
  switch(type) {
    case type_char:
      return SQL_SMALLINT;
    case type_short:
      return SQL_SMALLINT;
    case type_int:
      return SQL_INTEGER;
    case type_long:
      return SQL_BIGINT;
    case type_unsigned_char:
      return SQL_SMALLINT;
    case type_unsigned_short:
      return SQL_INTEGER;
    case type_unsigned_int:
      return SQL_BIGINT;
    case type_unsigned_long:
      return SQL_NUMERIC;
    case type_bool:
      return SQL_INTEGER;
    case type_float:
      return SQL_FLOAT;
    case type_double:
      return SQL_DOUBLE;
    case type_char_pointer:
      return SQL_VARCHAR;
    case type_varchar:
      return SQL_VARCHAR;
    case type_text:
      return SQL_VARCHAR;
    default:
      {
        throw std::logic_error("mssql statement: unknown type");
      }
    }
}

}

}
