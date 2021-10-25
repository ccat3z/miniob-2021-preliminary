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
// Created by Wangyunlai on 2021/5/14.
//

#include "sql/executor/execution_node.h"
#include "storage/common/table.h"
#include "common/log/log.h"

SelectExeNode::SelectExeNode(Trx *trx, Table *table) : trx_(trx), table_(table) {
}

SelectExeNode::~SelectExeNode() {
  for (DefaultConditionFilter * &filter : condition_filters_) {
    delete filter;
  }
  condition_filters_.clear();
}

const TupleSchema &SelectExeNode::schema() {
  return tuple_schema_;
};

void SelectExeNode::select_all_fields() {
  tuple_schema_.add_field_from_table(table_);
}

RC SelectExeNode::select_field(const char *field_name) {
  return tuple_schema_.add_field_from_table(table_, field_name);
}

bool table_contains_attr(const Table *table, RelAttr &attr) {
  if (attr.relation_name != nullptr && strcmp(table->table_meta().name(), attr.relation_name) != 0) {
    return false;
  }

  return table->table_meta().field(attr.attribute_name) != nullptr;
}

bool SelectExeNode::add_filter(Condition &condition) {
  if (condition.left_is_attr && !table_contains_attr(table_, condition.left_attr)) {
    return false;
  }

  if (condition.right_is_attr && !table_contains_attr(table_, condition.right_attr)) {
    return false;
  }

  DefaultConditionFilter *condition_filter = new DefaultConditionFilter();
  RC rc = condition_filter->init(*table_, condition);
  if (rc != RC::SUCCESS) {
    delete condition_filter;
    return false;
  }
  condition_filters_.push_back(condition_filter);
  return true;
}

bool SelectExeNode::can_filter_by(RelAttr &attr) {
  return table_contains_attr(table_, attr);
}

void record_reader(const char *data, void *context) {
  TupleRecordConverter *converter = (TupleRecordConverter *)context;
  converter->add_record(data);
}
RC SelectExeNode::execute(TupleSet &tuple_set) {
  CompositeConditionFilter condition_filter;
  condition_filter.init((const ConditionFilter **)condition_filters_.data(), condition_filters_.size());

  tuple_set.clear();
  tuple_set.set_schema(tuple_schema_);
  TupleRecordConverter converter(table_, tuple_set);
  return table_->scan_record(trx_, &condition_filter, -1, (void *)&converter, record_reader);
}