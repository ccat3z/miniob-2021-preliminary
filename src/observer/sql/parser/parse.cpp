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
// Created by Longda on 2021/4/13.
//

#include "sql/parser/parse.h"
#include "common/log/log.h"
#include "common/time/datetime.h"
#include "rc.h"
#include <mutex>

RC parse(char *st, Query *sqln);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
void relation_attr_init(RelAttr *relation_attr, const char *relation_name,
                        const char *attribute_name) {
  if (relation_name != nullptr) {
    relation_attr->relation_name = strdup(relation_name);
  } else {
    relation_attr->relation_name = nullptr;
  }
  relation_attr->attribute_name = strdup(attribute_name);
}

void relation_attr_destroy(RelAttr *relation_attr) {
  free(relation_attr->relation_name);
  free(relation_attr->attribute_name);
  relation_attr->relation_name = nullptr;
  relation_attr->attribute_name = nullptr;
}

void value_init_null(Value *value) {
  value->type = TYPE_NULL;
  value->data = malloc(4);
  memset(value->data, 0, 4);
  value->is_null = true;
}
void value_init_integer(Value *value, int v) {
  value->type = INTS;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
  value->is_null = false;
}
void value_init_float(Value *value, float v) {
  value->type = FLOATS;
  value->data = malloc(sizeof(v));
  memcpy(value->data, &v, sizeof(v));
  value->is_null = false;
}
void value_init_string(Value *value, const char *v) {
  value->type = CHARS;
  value->data = strdup(v);
  value->is_null = false;
}
bool value_cast(Value *value, AttrType type) {
  if (value->type == type)
    return true;

  if (value->type == TYPE_NULL) {
    value->type = type;
    return true;
  }

  if (value->type == CHARS && type == DATE) {
    common::Date date;
    if (!date.parse((char *)value->data)) {
      return false;
    }

    free(value->data);

    value->type = DATE;
    value->data = malloc(sizeof(int));
    int julian = date.julian();
    memcpy(value->data, &julian, sizeof(int));
    return true;
  }

  if (value->type == CHARS && type == TEXT) {
    value->type = TEXT;
    return true;
  }

  return false;
}
void value_destroy(Value *value) {
  value->type = UNDEFINED;
  free(value->data);
  value->data = nullptr;
}

ConditionCalcExpr *condition_calc_create(ConditionExpr *left, CalcOp op,
                                         ConditionExpr *right) {
  ConditionCalcExpr *expr =
      (ConditionCalcExpr *)malloc(sizeof(ConditionCalcExpr));
  expr->left = *left;
  expr->right = *right;
  expr->op = op;

  return expr;
}
void condition_calc_expr_destroy(ConditionCalcExpr *expr) {
  condition_expr_destroy(&expr->left);
  condition_expr_destroy(&expr->right);
}
void condition_expr_destroy(ConditionExpr *expr) {
  switch (expr->type) {
  case COND_EXPR_ATTR:
    relation_attr_destroy(&expr->value.attr);
    break;
  case COND_EXPR_VALUE:
    value_destroy(&expr->value.value);
    break;
  case COND_EXPR_SELECT:
    selects_destroy(expr->value.selects);
    free(expr->value.selects);
    break;
  case COND_EXPR_CALC:
    condition_calc_expr_destroy(expr->value.calc);
    free(expr->value.calc);
    break;
  default:
    throw std::logic_error("Unreachable code");
  }
}
void condition_init(Condition *condition, CompOp comp, ConditionExpr *left,
                    ConditionExpr *right) {
  condition->left_expr = *left;
  condition->right_expr = *right;
  condition->comp = comp;
}
void condition_destroy(Condition *condition) {
  condition_expr_destroy(&condition->left_expr);
  condition_expr_destroy(&condition->right_expr);
}

void attr_info_init(AttrInfo *attr_info, const char *name, AttrType type,
                    size_t length, bool nullable) {
  attr_info->name = strdup(name);
  attr_info->type = type;
  attr_info->length = length;
  attr_info->nullable = nullable;
}
void attr_info_destroy(AttrInfo *attr_info) {
  free(attr_info->name);
  attr_info->name = nullptr;
}

void agg_expr_init(AggExpr *expr, const char *func,
                   const SelectExpr *select_expr) {
  expr->agg_func = strdup(func);
  expr->expr = (SelectExpr *)malloc(sizeof *expr->expr);
  *expr->expr = *select_expr;

  std::stringstream ss;
  ss << expr->agg_func << '(' << *expr->expr << ')';
  expr->name = strdup(ss.str().c_str());
}

void agg_expr_destroy(AggExpr *expr) {
  if (expr == nullptr)
    return;
  free(expr->agg_func);
  select_expr_destroy(expr->expr);
  free(expr->expr);
  free(expr->name);
}

SelectCalcExpr *select_calc_expr_create(SelectExpr *left, CalcOp op,
                                        SelectExpr *right) {
  SelectCalcExpr *expr = (SelectCalcExpr *)malloc(sizeof(SelectCalcExpr));

  // Operation
  expr->op = op;

  // Left expression
  expr->left = (SelectExpr *)malloc(sizeof(SelectExpr));
  if (left == nullptr) {
    if (op != CALC_MINUS) {
      throw std::invalid_argument(
          "left expr can only be omitted in minus calc");
    }

    memset(expr->left, 0, sizeof *expr->left);
    expr->left->value = (Value *)malloc(sizeof *expr->left->value);
    value_init_integer(expr->left->value, 0);
  } else {
    *expr->left = *left;
  }

  // Right expression
  expr->right = (SelectExpr *)malloc(sizeof(SelectExpr));
  assert(right != nullptr);
  *expr->right = *right;

  // Name
  std::stringstream ss;
  if (left != nullptr) {
    ss << *left;
  }
  switch (op) {
  case CALC_ADD:
    ss << '+';
    break;
  case CALC_MINUS:
    ss << '-';
    break;
  case CALC_MULTI:
    ss << '*';
    break;
  case CALC_DIV:
    ss << '/';
    break;
  default:
    throw std::logic_error("Unreachable code: 258");
    break;
  }
  ss << *right;
  expr->name = strdup(ss.str().c_str());

  return expr;
}

void select_calc_expr_add_brace(SelectCalcExpr *expr) {
  std::stringstream ss;
  ss << '(' << expr->name << ')';
  free(expr->name);
  expr->name = strdup(ss.str().c_str());
}

void select_calc_expr_free(SelectCalcExpr *expr) {
  select_expr_destroy(expr->left);
  free(expr->left);
  select_expr_destroy(expr->right);
  free(expr->right);
  free(expr->name);
  free(expr);
}

void select_expr_destroy(SelectExpr *expr) {
  if (expr == nullptr)
    return;
  if (expr->value != nullptr) {
    value_destroy(expr->value);
    free(expr->value);
  }
  if (expr->agg != nullptr) {
    agg_expr_destroy(expr->agg);
    free(expr->agg);
  }
  if (expr->attribute != nullptr) {
    relation_attr_destroy(expr->attribute);
    free(expr->attribute);
  }
  if (expr->calc != nullptr) {
    select_calc_expr_free(expr->calc);
  }
  if (expr->name != nullptr) {
    free(expr->name);
  }
}

void selects_init(Selects *selects, ...);
void selects_append_exprs(Selects *selects, SelectExpr *expr, size_t len) {
  for (size_t i = 0; i < len; i++) {
    selects->attributes[selects->attr_num++] = expr[i];
  }
}
void selects_append_relation(Selects *selects, const char *relation_name) {
  selects->relations[selects->relation_num++] = strdup(relation_name);
}
void selects_append_relations(Selects *selects, const char **relation_name,
                              size_t len) {
  for (size_t i = 0; i < len; i++) {
    selects_append_relation(selects, relation_name[i]);
  }
}
void selects_append_join_relation(Selects *selects, const char *relation_name) {
  selects->joins[selects->join_num++] = strdup(relation_name);
}

void selects_append_conditions(Selects *selects, Condition conditions[],
                               size_t condition_num) {
  assert(selects->condition_num + condition_num <=
         sizeof(selects->conditions) / sizeof(selects->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    selects->conditions[selects->condition_num + i] = conditions[i];
  }
  selects->condition_num += condition_num;
}

void selects_append_order_attrs(Selects *selects, OrderBy *order_by,
                                size_t len) {
  for (size_t i = 0; i < len; i++) {
    selects->order_by[selects->order_by_num++] = order_by[i];
  }
}
void selects_append_group_attrs(Selects *selects, RelAttr *order_by,
                                size_t len) {
  for (int i = len - 1; i >= 0; i--) {
    selects->group_by[selects->group_by_num++] = order_by[i];
  }
}

void selects_destroy(Selects *selects) {
  for (size_t i = 0; i < selects->attr_num; i++) {
    select_expr_destroy(&selects->attributes[i]);
  }
  selects->attr_num = 0;

  for (size_t i = 0; i < selects->relation_num; i++) {
    free(selects->relations[i]);
    selects->relations[i] = NULL;
  }
  selects->relation_num = 0;

  for (size_t i = 0; i < selects->join_num; i++) {
    free(selects->joins[i]);
    selects->joins[i] = NULL;
  }
  selects->join_num = 0;

  for (size_t i = 0; i < selects->condition_num; i++) {
    condition_destroy(&selects->conditions[i]);
  }
  selects->condition_num = 0;
}

void inserts_init(Inserts *inserts, const char *relation_name, Value values[],
                  size_t value_num) {
  assert(value_num <= sizeof(inserts->values) / sizeof(inserts->values[0]));

  inserts->relation_name = strdup(relation_name);
  for (int i = value_num - 1; i >= 0; i--) {
    inserts->values[value_num - i - 1] = values[i];
  }
  inserts->value_num = value_num;
}
void inserts_destroy(Inserts *inserts) {
  free(inserts->relation_name);
  inserts->relation_name = nullptr;

  for (size_t i = 0; i < inserts->value_num; i++) {
    value_destroy(&inserts->values[i]);
  }
  inserts->value_num = 0;
}

void deletes_init_relation(Deletes *deletes, const char *relation_name) {
  deletes->relation_name = strdup(relation_name);
}

void deletes_set_conditions(Deletes *deletes, Condition conditions[],
                            size_t condition_num) {
  assert(condition_num <=
         sizeof(deletes->conditions) / sizeof(deletes->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    deletes->conditions[i] = conditions[i];
  }
  deletes->condition_num = condition_num;
}
void deletes_destroy(Deletes *deletes) {
  for (size_t i = 0; i < deletes->condition_num; i++) {
    condition_destroy(&deletes->conditions[i]);
  }
  deletes->condition_num = 0;
  free(deletes->relation_name);
  deletes->relation_name = nullptr;
}

void updates_init(Updates *updates, const char *relation_name,
                  const char *attribute_name, Value *value,
                  Condition conditions[], size_t condition_num) {
  updates->relation_name = strdup(relation_name);
  updates->attribute_name = strdup(attribute_name);
  updates->value = *value;

  assert(condition_num <=
         sizeof(updates->conditions) / sizeof(updates->conditions[0]));
  for (size_t i = 0; i < condition_num; i++) {
    updates->conditions[i] = conditions[i];
  }
  updates->condition_num = condition_num;
}

void updates_destroy(Updates *updates) {
  free(updates->relation_name);
  free(updates->attribute_name);
  updates->relation_name = nullptr;
  updates->attribute_name = nullptr;

  value_destroy(&updates->value);

  for (size_t i = 0; i < updates->condition_num; i++) {
    condition_destroy(&updates->conditions[i]);
  }
  updates->condition_num = 0;
}

void create_table_append_attribute(CreateTable *create_table,
                                   AttrInfo *attr_info) {
  create_table->attributes[create_table->attribute_count++] = *attr_info;
}
void create_table_init_name(CreateTable *create_table,
                            const char *relation_name) {
  create_table->relation_name = strdup(relation_name);
}
void create_table_destroy(CreateTable *create_table) {
  for (size_t i = 0; i < create_table->attribute_count; i++) {
    attr_info_destroy(&create_table->attributes[i]);
  }
  create_table->attribute_count = 0;
  free(create_table->relation_name);
  create_table->relation_name = nullptr;
}

void drop_table_init(DropTable *drop_table, const char *relation_name) {
  drop_table->relation_name = strdup(relation_name);
}
void drop_table_destroy(DropTable *drop_table) {
  free(drop_table->relation_name);
  drop_table->relation_name = nullptr;
}

void create_index_init(CreateIndex *create_index, const char *index_name,
                       const char *relation_name, const char **attr_name,
                       int attr_num, bool unique) {
  create_index->index_name = strdup(index_name);
  create_index->relation_name = strdup(relation_name);
  create_index->attribute_name = (char **)malloc(sizeof(char *) * attr_num);
  for (int i = attr_num - 1; i >= 0; i--) {
    create_index->attribute_name[i] = strdup(attr_name[i]);
  }
  create_index->attribute_num = attr_num;
  create_index->unique = unique;
}
void create_index_destroy(CreateIndex *create_index) {
  free(create_index->index_name);
  free(create_index->relation_name);
  for (int i = 0; i < create_index->attribute_num; i++) {
    free(create_index->attribute_name[i]);
  }
  free(create_index->attribute_name);

  create_index->index_name = nullptr;
  create_index->relation_name = nullptr;
  create_index->attribute_name = nullptr;
}

void drop_index_init(DropIndex *drop_index, const char *index_name) {
  drop_index->index_name = strdup(index_name);
}
void drop_index_destroy(DropIndex *drop_index) {
  free((char *)drop_index->index_name);
  drop_index->index_name = nullptr;
}

void desc_table_init(DescTable *desc_table, const char *relation_name) {
  desc_table->relation_name = strdup(relation_name);
}

void desc_table_destroy(DescTable *desc_table) {
  free((char *)desc_table->relation_name);
  desc_table->relation_name = nullptr;
}

void load_data_init(LoadData *load_data, const char *relation_name,
                    const char *file_name) {
  load_data->relation_name = strdup(relation_name);

  if (file_name[0] == '\'' || file_name[0] == '\"') {
    file_name++;
  }
  char *dup_file_name = strdup(file_name);
  int len = strlen(dup_file_name);
  if (dup_file_name[len - 1] == '\'' || dup_file_name[len - 1] == '\"') {
    dup_file_name[len - 1] = 0;
  }
  load_data->file_name = dup_file_name;
}

void load_data_destroy(LoadData *load_data) {
  free((char *)load_data->relation_name);
  free((char *)load_data->file_name);
  load_data->relation_name = nullptr;
  load_data->file_name = nullptr;
}

void query_init(Query *query) {
  query->flag = SCF_ERROR;
  memset(&query->sstr, 0, sizeof(query->sstr));
}

Query *query_create() {
  Query *query = (Query *)malloc(sizeof(Query));
  if (nullptr == query) {
    LOG_ERROR("Failed to alloc memroy for query. size=%ld", sizeof(Query));
    return nullptr;
  }

  query_init(query);
  return query;
}

void query_reset(Query *query) {
  switch (query->flag) {
  case SCF_SELECT: {
    selects_destroy(&query->sstr.selection);
  } break;
  case SCF_INSERT: {
    inserts_destroy(&query->sstr.insertion);
  } break;
  case SCF_DELETE: {
    deletes_destroy(&query->sstr.deletion);
  } break;
  case SCF_UPDATE: {
    updates_destroy(&query->sstr.update);
  } break;
  case SCF_CREATE_TABLE: {
    create_table_destroy(&query->sstr.create_table);
  } break;
  case SCF_DROP_TABLE: {
    drop_table_destroy(&query->sstr.drop_table);
  } break;
  case SCF_CREATE_INDEX: {
    create_index_destroy(&query->sstr.create_index);
  } break;
  case SCF_DROP_INDEX: {
    drop_index_destroy(&query->sstr.drop_index);
  } break;
  case SCF_SYNC: {

  } break;
  case SCF_SHOW_TABLES:
    break;

  case SCF_DESC_TABLE: {
    desc_table_destroy(&query->sstr.desc_table);
  } break;

  case SCF_LOAD_DATA: {
    load_data_destroy(&query->sstr.load_data);
  } break;
  case SCF_BEGIN:
  case SCF_COMMIT:
  case SCF_ROLLBACK:
  case SCF_HELP:
  case SCF_EXIT:
  case SCF_ERROR:
    break;
  }
}

void query_destroy(Query *query) {
  query_reset(query);
  free(query);
}

List *list_create(size_t size, size_t max) {
  List *l = (List *)malloc(sizeof(List));
  l->values = (uint8_t *)malloc(size * max);
  l->size = size;
  l->len = 0;
  return l;
}

void list_append(List *list, void *value) {
  memcpy(list->values + (list->size * list->len++), value, list->size);
}

void list_append_list(List *list, List *append) {
  memcpy(list->values + (list->size * list->len), append->values,
         append->size * append->len);
  list->len += append->len;
}

void list_free(List *list) {
  free(list->values);
  free(list);
}

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus

std::ostream &operator<<(std::ostream &out, const SelectExpr &expr) {
  if (expr.name != nullptr) {
    out << expr.name;
  } else if (expr.agg != nullptr) {
    out << expr.agg->name;
  } else if (expr.attribute != nullptr) {
    if (expr.attribute->relation_name != nullptr) {
      out << expr.attribute->relation_name << '.';
    }

    out << expr.attribute->attribute_name;
  } else if (expr.value != nullptr) {
    if (expr.value->is_null) {
      out << "NULL";
    } else {
      switch (expr.value->type) {
      case INTS:
        out << *((int *)expr.value->data);
        break;
      case FLOATS:
        out << *((float *)expr.value->data);
        break;
      case CHARS:
        out << '\'' << (char *)expr.value->data << '\'';
        break;
      default:
        out << "??";
        break;
      }
    }
  } else if (expr.calc != nullptr) {
    out << expr.calc->name;
  } else {
    out << "??";
  }

  return out;
}

////////////////////////////////////////////////////////////////////////////////

extern "C" int sql_parse(const char *st, Query *sqls);

RC parse(const char *st, Query *sqln) {
  sql_parse(st, sqln);

  if (sqln->flag == SCF_ERROR)
    return SQL_SYNTAX;
  else
    return SUCCESS;
}