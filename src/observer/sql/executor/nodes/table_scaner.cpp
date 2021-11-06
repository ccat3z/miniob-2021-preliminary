// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#include "table_scaner.h"
#include "common/log/log.h"
#include "storage/common/table.h"

TableScaner::TableScaner(Trx *trx, Table *table) : trx_(trx), table_(table) {}

TableScaner::~TableScaner() {
  for (DefaultConditionFilter *&filter : condition_filters_) {
    delete filter;
  }
  condition_filters_.clear();
}

const TupleSchema &TableScaner::schema() { return tuple_schema_; };

const Table *TableScaner::table() { return table_; }

void TableScaner::select_all_fields() {
  tuple_schema_.add_field_from_table(table_);
}

RC TableScaner::select_field(const char *field_name) {
  return tuple_schema_.add_field_from_table(table_, field_name);
}

bool table_contains_attr(const Table *table, RelAttr &attr) {
  if (attr.relation_name != nullptr &&
      strcmp(table->table_meta().name(), attr.relation_name) != 0) {
    return false;
  }

  return table->table_meta().field(attr.attribute_name) != nullptr;
}

bool TableScaner::add_filter(Condition &condition) {
  if (condition.left_expr.type == COND_EXPR_ATTR &&
      !table_contains_attr(table_, condition.left_expr.value.attr)) {
    return false;
  }

  if (condition.right_expr.type == COND_EXPR_ATTR &&
      !table_contains_attr(table_, condition.right_expr.value.attr)) {
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

bool TableScaner::can_filter_by(RelAttr &attr) {
  return table_contains_attr(table_, attr);
}

void record_reader(const char *data, void *context) {
  TupleRecordConverter *converter = (TupleRecordConverter *)context;
  converter->add_record(data);
}
RC TableScaner::execute(TupleSet &tuple_set) {
  CompositeConditionFilter condition_filter;
  condition_filter.init((const ConditionFilter **)condition_filters_.data(),
                        condition_filters_.size());

  tuple_set.clear();
  tuple_set.set_schema(tuple_schema_);
  TupleRecordConverter converter(table_, tuple_set);
  return table_->scan_record(trx_, &condition_filter, -1, (void *)&converter,
                             record_reader);
}

std::unique_ptr<ExecutionNode>
TableScaner::push_down_predicate(std::list<Condition *> &predicate) {
  for (auto it = predicate.begin(); it != predicate.end();) {
    if (add_filter(**it)) {
      it = predicate.erase(it);
    } else {
      it++;
    }
  }
  return nullptr;
}