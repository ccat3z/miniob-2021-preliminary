/* Copyright (c) 2021 Xie Meiyi(xiemeiyi@hust.edu.cn) and OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by wangyunlai.wyl on 2021/6/7.
//

#ifndef __OBSERVER_SQL_PARSER_PARSE_DEFS_H__
#define __OBSERVER_SQL_PARSER_PARSE_DEFS_H__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_NUM 20
#define MAX_REL_NAME 20
#define MAX_ATTR_NAME 20
#define MAX_ERROR_MESSAGE 20
#define MAX_DATA 50

//属性结构体
typedef struct _RelAttr {
  char *relation_name;  // relation name (may be NULL) 表名
  char *attribute_name; // attribute name              属性名
} RelAttr;

typedef enum { CALC_ADD, CALC_MINUS, CALC_MULTI, CALC_DIV } CalcOp;

typedef enum {
  EQUAL_TO,    //"="     0
  LESS_EQUAL,  //"<="    1
  NOT_EQUAL,   //"<>"    2
  LESS_THAN,   //"<"     3
  GREAT_EQUAL, //">="    4
  GREAT_THAN,  //">"     5
  IN_SET,
  NOT_IN_SET,
  IS_NULL,
  IS_NOT_NULL,
  NO_OP
} CompOp;

//属性值类型
typedef enum { UNDEFINED, CHARS, INTS, FLOATS, DATE, TEXT, TYPE_NULL } AttrType;

//属性值
typedef struct _Value {
  AttrType type; // type of value
  void *data;    // value
  bool is_null;
} Value;

typedef enum {
  COND_EXPR_VALUE,
  COND_EXPR_ATTR,
  COND_EXPR_SELECT,
  COND_EXPR_CALC
} ConditionExprType;

typedef struct _ConditonExpr {
  union {
    Value value;
    RelAttr attr;
    struct _Selects *selects;
    struct _ConditionCalcExpr *calc;
  } value;
  ConditionExprType type;
} ConditionExpr;

typedef struct _ConditionCalcExpr {
  ConditionExpr left;
  CalcOp op;
  ConditionExpr right;
} ConditionCalcExpr;

typedef struct _Condition {
  ConditionExpr left_expr;
  CompOp comp; // comparison operator
  ConditionExpr right_expr;
} Condition;

typedef struct {
  Value *value;
  struct _AggExpr *agg;
  RelAttr *attribute;
  struct _SelectCalcExpr *calc;
  char *name;
} SelectExpr;

typedef struct _AggExpr {
  char *agg_func;
  SelectExpr *expr;
  char *name;
} AggExpr;

typedef struct _SelectCalcExpr {
  SelectExpr *left;
  CalcOp op;
  SelectExpr *right;
  char *name;
} SelectCalcExpr;

typedef enum { DIR_ASC, DIR_DESC } OrderDir;

typedef struct {
  OrderDir dir;
  RelAttr *attr;
} OrderBy;

// struct of select
typedef struct _Selects {
  size_t attr_num;                // Length of attrs in Select clause
  SelectExpr attributes[MAX_NUM]; // attrs in Select clause
  size_t relation_num;            // Length of relations in Fro clause
  char *relations[MAX_NUM];       // relations in From clause
  size_t condition_num;           // Length of conditions in Where clause
  Condition conditions[MAX_NUM];  // conditions in Where clause
  size_t join_num;                // Length of relations in Join clause
  char *joins[MAX_NUM];           // relations in Join clause
  size_t order_by_num;            // Length of order attribute
  OrderBy order_by[MAX_NUM];      // order attribute
  size_t group_by_num;            // Length of group attribute
  RelAttr group_by[MAX_NUM];      // group by attribute
} Selects;

// struct of insert
typedef struct {
  char *relation_name;   // Relation to insert into
  size_t value_num;      // Length of values
  size_t tuple_num;      // Length of tuples
  Value values[MAX_NUM]; // values to insert
} Inserts;

// struct of delete
typedef struct {
  char *relation_name;           // Relation to delete from
  size_t condition_num;          // Length of conditions in Where clause
  Condition conditions[MAX_NUM]; // conditions in Where clause
} Deletes;

// struct of update
typedef struct {
  char *relation_name;           // Relation to update
  char *attribute_name;          // Attribute to update
  Value value;                   // update value
  size_t condition_num;          // Length of conditions in Where clause
  Condition conditions[MAX_NUM]; // conditions in Where clause
} Updates;

typedef struct {
  char *name;    // Attribute name
  AttrType type; // Type of attribute
  size_t length; // Length of attribute
  bool nullable;
} AttrInfo;

// struct of craete_table
typedef struct {
  char *relation_name;          // Relation name
  size_t attribute_count;       // Length of attribute
  AttrInfo attributes[MAX_NUM]; // attributes
} CreateTable;

// struct of drop_table
typedef struct {
  char *relation_name; // Relation name
} DropTable;

// struct of create_index
typedef struct {
  char *index_name;      // Index name
  char *relation_name;   // Relation name
  char **attribute_name; // Attribute name
  int attribute_num;
  bool unique;
} CreateIndex;

// struct of  drop_index
typedef struct {
  const char *index_name; // Index name
} DropIndex;

typedef struct {
  const char *relation_name;
} DescTable;

typedef struct {
  const char *relation_name;
  const char *file_name;
} LoadData;

union Queries {
  Selects selection;
  Inserts insertion;
  Deletes deletion;
  Updates update;
  CreateTable create_table;
  DropTable drop_table;
  CreateIndex create_index;
  DropIndex drop_index;
  DescTable desc_table;
  LoadData load_data;
  char *errors;
};

// 修改yacc中相关数字编码为宏定义
enum SqlCommandFlag {
  SCF_ERROR = 0,
  SCF_SELECT,
  SCF_INSERT,
  SCF_UPDATE,
  SCF_DELETE,
  SCF_CREATE_TABLE,
  SCF_DROP_TABLE,
  SCF_CREATE_INDEX,
  SCF_DROP_INDEX,
  SCF_SYNC,
  SCF_SHOW_TABLES,
  SCF_DESC_TABLE,
  SCF_BEGIN,
  SCF_COMMIT,
  SCF_ROLLBACK,
  SCF_LOAD_DATA,
  SCF_HELP,
  SCF_EXIT
};
// struct of flag and sql_struct
typedef struct Query {
  enum SqlCommandFlag flag;
  union Queries sstr;
} Query;

typedef struct {
  uint8_t *values;
  size_t size;
  size_t len;
} List;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void relation_attr_init(RelAttr *relation_attr, const char *relation_name,
                        const char *attribute_name);
void relation_attr_destroy(RelAttr *relation_attr);

void value_init_null(Value *value);
void value_init_integer(Value *value, int v);
void value_init_float(Value *value, float v);
void value_init_string(Value *value, const char *v);
bool value_cast(Value *value, AttrType type);
void value_destroy(Value *value);

void agg_expr_init(AggExpr *expr, const char *func,
                   const SelectExpr *select_expr);
void agg_expr_destroy(AggExpr *expr);
SelectCalcExpr *select_calc_expr_create(SelectExpr *left, CalcOp op,
                                        SelectExpr *right);
void select_calc_expr_add_brace(SelectCalcExpr *expr);
void select_calc_expr_free(SelectCalcExpr *expr);
void select_expr_destroy(SelectExpr *expr);

ConditionCalcExpr *condition_calc_create(ConditionExpr *left, CalcOp op,
                                         ConditionExpr *right);
void condition_calc_expr_destroy(ConditionCalcExpr *expr);
void condition_expr_destroy(ConditionExpr *expr);
void condition_init(Condition *condition, CompOp comp, ConditionExpr *left,
                    ConditionExpr *right);
void condition_destroy(Condition *condition);

void attr_info_init(AttrInfo *attr_info, const char *name, AttrType type,
                    size_t length, bool nullable);
void attr_info_destroy(AttrInfo *attr_info);

void selects_init(Selects *selects, ...);
void selects_append_exprs(Selects *selects, SelectExpr *expr, size_t len);
void selects_append_relation(Selects *selects, const char *relation_name);
void selects_append_relations(Selects *selects, const char **relation_name,
                              size_t len);
void selects_append_join_relation(Selects *selects, const char *relation_name);
void selects_append_conditions(Selects *selects, Condition conditions[],
                               size_t condition_num);
void selects_append_order_attrs(Selects *selects, OrderBy *order_by,
                                size_t len);
void selects_append_group_attrs(Selects *selects, RelAttr *order_by,
                                size_t len);
void selects_destroy(Selects *selects);

void inserts_init(Inserts *inserts, const char *relation_name, Value values[],
                  size_t value_num);
void inserts_destroy(Inserts *inserts);

void deletes_init_relation(Deletes *deletes, const char *relation_name);
void deletes_set_conditions(Deletes *deletes, Condition conditions[],
                            size_t condition_num);
void deletes_destroy(Deletes *deletes);

void updates_init(Updates *updates, const char *relation_name,
                  const char *attribute_name, Value *value,
                  Condition conditions[], size_t condition_num);
void updates_destroy(Updates *updates);

void create_table_append_attribute(CreateTable *create_table,
                                   AttrInfo *attr_info);
void create_table_init_name(CreateTable *create_table,
                            const char *relation_name);
void create_table_destroy(CreateTable *create_table);

void drop_table_init(DropTable *drop_table, const char *relation_name);
void drop_table_destroy(DropTable *drop_table);

void create_index_init(CreateIndex *create_index, const char *index_name,
                       const char *relation_name, const char **attr_name,
                       int attr_num, bool unique);
void create_index_destroy(CreateIndex *create_index);

void drop_index_init(DropIndex *drop_index, const char *index_name);
void drop_index_destroy(DropIndex *drop_index);

void desc_table_init(DescTable *desc_table, const char *relation_name);
void desc_table_destroy(DescTable *desc_table);

void load_data_init(LoadData *load_data, const char *relation_name,
                    const char *file_name);
void load_data_destroy(LoadData *load_data);

void query_init(Query *query);
Query *query_create(); // create and init
void query_reset(Query *query);
void query_destroy(Query *query); // reset and delete

List *list_create(size_t size, size_t max);
void list_append(List *list, void *value);
void list_append_list(List *list, List *append);
void list_free(List *list);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __OBSERVER_SQL_PARSER_PARSE_DEFS_H__