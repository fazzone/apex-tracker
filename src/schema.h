#ifndef ___SCHEMA_H
#define ___SCHEMA_H

namespace schema_tables {
  const char *create_fix_result = "create table fix_result(id integer primary key, inserted_at datetime, x integer, y integer, residual real)";
}

#endif
