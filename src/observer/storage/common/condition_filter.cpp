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
// Created by Wangyunlai on 2021/5/7.
//

#include "condition_filter.h"
#include "common/log/log.h"
#include "record_manager.h"
#include "sql/parser/parse.h"
#include "storage/common/table.h"
#include <stddef.h>

using namespace common;

ConditionFilter::~ConditionFilter() {}

DefaultConditionFilter::DefaultConditionFilter() {
  left_.is_attr = false;
  left_.attr_length = 0;
  left_.attr_offset = 0;
  left_.value = nullptr;

  right_.is_attr = false;
  right_.attr_length = 0;
  right_.attr_offset = 0;
  right_.value = nullptr;
}
DefaultConditionFilter::~DefaultConditionFilter() {}

RC DefaultConditionFilter::init(const ConDesc &left, const ConDesc &right,
                                AttrType attr_type, CompOp comp_op) {
  if (attr_type < CHARS || attr_type > DATE) {
    LOG_ERROR("Invalid condition with unsupported attribute type: %d",
              attr_type);
    return RC::INVALID_ARGUMENT;
  }

  if (comp_op < EQUAL_TO || comp_op >= NO_OP) {
    LOG_ERROR("Invalid condition with unsupported compare operation: %d",
              comp_op);
    return RC::INVALID_ARGUMENT;
  }

  left_ = left;
  right_ = right;
  attr_type_ = attr_type;
  comp_op_ = comp_op;
  return RC::SUCCESS;
}

RC DefaultConditionFilter::init(Table &table, Condition &condition) {
  const TableMeta &table_meta = table.table_meta();
  ConDesc left;
  ConDesc right;

  AttrType type_left = UNDEFINED;
  AttrType type_right = UNDEFINED;

  // Prepare attr first, because value_cast is based on attrs
  if (condition.left_expr.type == COND_EXPR_ATTR) {
    left.is_attr = true;
    const FieldMeta *field_left =
        table_meta.field(condition.left_expr.value.attr.attribute_name);
    if (nullptr == field_left) {
      LOG_WARN("No such field in condition. %s.%s", table.name(),
               condition.left_expr.value.attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }

    if (field_left->nullable()) {
      LOG_ERROR("Nullable field is not support yet in storage condition");
      return RC::INVALID_ARGUMENT;
    }

    left.attr_length = field_left->len();
    left.attr_offset = field_left->offset();

    left.value = nullptr;

    type_left = field_left->type();
  }

  if (condition.right_expr.type == COND_EXPR_ATTR) {
    right.is_attr = true;
    const FieldMeta *field_right =
        table_meta.field(condition.right_expr.value.attr.attribute_name);
    if (nullptr == field_right) {
      LOG_WARN("No such field in condition. %s.%s", table.name(),
               condition.right_expr.value.attr.attribute_name);
      return RC::SCHEMA_FIELD_MISSING;
    }

    if (field_right->nullable()) {
      LOG_ERROR("Nullable field is not support yet in storage condition");
      return RC::INVALID_ARGUMENT;
    }

    right.attr_length = field_right->len();
    right.attr_offset = field_right->offset();
    type_right = field_right->type();

    right.value = nullptr;
  }

  if (condition.left_expr.type == COND_EXPR_VALUE) {
    if (condition.left_expr.value.value.is_null) {
      LOG_ERROR("Nullable value is not support yet in storage condition");
      return RC::INVALID_ARGUMENT;
    }

    // Detect value's type by another attr
    if (condition.right_expr.type == COND_EXPR_ATTR) {
      value_cast(&condition.left_expr.value.value, type_right);
    }

    left.is_attr = false;
    left.value = condition.left_expr.value.value.data; // ??????type ??????????????????
    type_left = condition.left_expr.value.value.type;

    left.attr_length = 0;
    left.attr_offset = 0;
  }

  if (condition.right_expr.type == COND_EXPR_VALUE) {
    if (condition.right_expr.value.value.is_null) {
      LOG_ERROR("Nullable value is not support yet in storage condition");
      return RC::INVALID_ARGUMENT;
    }

    // Detect value's type by another attr
    if (condition.left_expr.type == COND_EXPR_ATTR) {
      value_cast(&condition.right_expr.value.value, type_left);
    }

    right.is_attr = false;
    right.value = condition.right_expr.value.value.data;
    type_right = condition.right_expr.value.value.type;

    right.attr_length = 0;
    right.attr_offset = 0;
  }

  if (type_left == UNDEFINED || type_right == UNDEFINED) {
    return RC::INVALID_ARGUMENT;
  }

  if (type_left != type_right) {
    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
  }

  return init(left, right, type_left, condition.comp);
}

bool DefaultConditionFilter::filter(const Record &rec) const {
  char *left_value = nullptr;
  char *right_value = nullptr;

  if (left_.is_attr) { // value
    left_value = (char *)(rec.data + left_.attr_offset);
  } else {
    left_value = (char *)left_.value;
  }

  if (right_.is_attr) {
    right_value = (char *)(rec.data + right_.attr_offset);
  } else {
    right_value = (char *)right_.value;
  }

  int cmp_result = 0;
  switch (attr_type_) {
  case CHARS: { // ???????????????????????????????????????
    // ??????C?????????????????????
    cmp_result = strcmp(left_value, right_value);
  } break;
  case INTS:
  case DATE: {
    // ???????????????????????????
    // ???int???float??????????????????????????????,???????????????????????????????????????
    int left = *(int *)left_value;
    int right = *(int *)right_value;
    cmp_result = left - right;
  } break;
  case FLOATS: {
    float left = *(float *)left_value;
    float right = *(float *)right_value;
    float result = left - right;
    if (result > 0) {
      cmp_result = 1;
    } else if (result < 0) {
      cmp_result = -1;
    } else {
      cmp_result = 0;
    }
  } break;
  default: {
  }
  }

  switch (comp_op_) {
  case EQUAL_TO:
    return 0 == cmp_result;
  case LESS_EQUAL:
    return cmp_result <= 0;
  case NOT_EQUAL:
    return cmp_result != 0;
  case LESS_THAN:
    return cmp_result < 0;
  case GREAT_EQUAL:
    return cmp_result >= 0;
  case GREAT_THAN:
    return cmp_result > 0;

  default:
    break;
  }

  LOG_PANIC("Never should print this.");
  return cmp_result; // should not go here
}

CompositeConditionFilter::~CompositeConditionFilter() {
  if (memory_owner_) {
    delete[] filters_;
    filters_ = nullptr;
  }
}

RC CompositeConditionFilter::init(const ConditionFilter *filters[],
                                  int filter_num, bool own_memory) {
  filters_ = filters;
  filter_num_ = filter_num;
  memory_owner_ = own_memory;
  return RC::SUCCESS;
}
RC CompositeConditionFilter::init(const ConditionFilter *filters[],
                                  int filter_num) {
  return init(filters, filter_num, false);
}

RC CompositeConditionFilter::init(Table &table, Condition *conditions,
                                  int condition_num) {
  if (condition_num == 0) {
    return RC::SUCCESS;
  }
  if (conditions == nullptr) {
    return RC::INVALID_ARGUMENT;
  }

  RC rc = RC::SUCCESS;
  ConditionFilter **condition_filters = new ConditionFilter *[condition_num];
  for (int i = 0; i < condition_num; i++) {
    DefaultConditionFilter *default_condition_filter =
        new DefaultConditionFilter();
    rc = default_condition_filter->init(table, conditions[i]);
    if (rc != RC::SUCCESS) {
      delete default_condition_filter;
      for (int j = i - 1; j >= 0; j--) {
        delete condition_filters[j];
        condition_filters[j] = nullptr;
      }
      delete[] condition_filters;
      condition_filters = nullptr;
      return rc;
    }
    condition_filters[i] = default_condition_filter;
  }
  return init((const ConditionFilter **)condition_filters, condition_num, true);
}

bool CompositeConditionFilter::filter(const Record &rec) const {
  for (int i = 0; i < filter_num_; i++) {
    if (!filters_[i]->filter(rec)) {
      return false;
    }
  }
  return true;
}
