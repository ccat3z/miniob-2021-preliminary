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
#include "tuple_filter.h"

TableScaner::TableScaner(Trx *trx, Table *table) : trx_(trx), table_(table) {
}

TableScaner::~TableScaner() {
  for (DefaultConditionFilter * &filter : condition_filters_) {
    delete filter;
  }
  condition_filters_.clear();
}

const TupleSchema &TableScaner::schema() {
  return tuple_schema_;
};

const Table *TableScaner::table() {
  return table_;
}

void TableScaner::select_all_fields() {
  tuple_schema_.add_field_from_table(table_);
}

RC TableScaner::select_field(const char *field_name) {
  return tuple_schema_.add_field_from_table(table_, field_name);
}

bool table_contains_attr(const Table *table, RelAttr &attr) {
  if (attr.relation_name != nullptr && strcmp(table->table_meta().name(), attr.relation_name) != 0) {
    return false;
  }

  return table->table_meta().field(attr.attribute_name) != nullptr;
}

bool TableScaner::add_filter(Condition &condition) {
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

bool TableScaner::can_filter_by(RelAttr &attr) {
  return table_contains_attr(table_, attr);
}

void record_reader(const char *data, void *context) {
  TupleRecordConverter *converter = (TupleRecordConverter *)context;
  converter->add_record(data);
}
RC TableScaner::execute(TupleSet &tuple_set) {
  CompositeConditionFilter condition_filter;
  condition_filter.init((const ConditionFilter **)condition_filters_.data(), condition_filters_.size());

  tuple_set.clear();
  tuple_set.set_schema(tuple_schema_);
  TupleRecordConverter converter(table_, tuple_set);
  return table_->scan_record(trx_, &condition_filter, -1, (void *)&converter, record_reader);
}

CartesianSelectNode::CartesianSelectNode(
  std::unique_ptr<ExecutionNode> left_node,
  std::unique_ptr<ExecutionNode> right_node)
  : left_node(std::move(left_node)), right_node(std::move(right_node)) {
  tuple_schema_.append(this->left_node->schema());
  tuple_schema_.append(this->right_node->schema());
};
CartesianSelectNode::~CartesianSelectNode() { }
const TupleSchema &CartesianSelectNode::schema() { return tuple_schema_; };

RC CartesianSelectNode::execute(TupleSet &tuple_set) {
  TupleSet tuple_left, tuple_right;

  RC rc;
  if ((rc = left_node->execute(tuple_left)) != RC::SUCCESS) {
    return rc;
  }
  if ((rc = right_node->execute(tuple_right)) != RC::SUCCESS) {
    return rc;
  }

  tuple_set.clear();
  tuple_set.set_schema(tuple_schema_);
  for (auto &tuple_left : tuple_left.tuples()) {
    for (auto &tuple_right : tuple_right.tuples()) {
      Tuple tuple;
      for (auto &v : tuple_left.values()) {
        tuple.add(v);
      }
      for (auto &v : tuple_right.values()) {
        tuple.add(v);
      }

      tuple_set.add(std::move(tuple));
    }
  }

  return RC::SUCCESS;
};

std::unique_ptr<CartesianSelectNode> CartesianSelectNode::create(
  std::vector<std::unique_ptr<ExecutionNode>> &nodes
) {
  if (nodes.size() < 2) return nullptr;

  std::unique_ptr<CartesianSelectNode> root(new CartesianSelectNode(
    std::move(nodes[0]),
    std::move(nodes[1])
  ));

  auto it = nodes.begin();
  std::advance(it, 2);
  for (; it != nodes.end(); it++) {
    root = std::unique_ptr<CartesianSelectNode>(new CartesianSelectNode(
      std::move(root),
      std::move(*it)
    ));
  }

  return root;
}

std::unique_ptr<ProjectionNode> ProjectionNode::create(std::unique_ptr<ExecutionNode> child, RelAttr *attrs, int attr_num) {
  auto proj = std::unique_ptr<ProjectionNode>(new ProjectionNode(std::move(child), attr_num));

  auto &child_fields = proj->child->schema().fields();

  for (int i = attr_num - 1; i >= 0; i--) {
    auto &attr = attrs[i];
    for (size_t j = 0; j < child_fields.size(); j++) {
      auto &child_field = child_fields[j];
      if (
        (strcmp(attr.attribute_name, child_field.field_name()) == 0) &&
        (strcmp(attr.relation_name, child_field.table_name()) == 0)
      ) {
        proj->fields_map[attr_num - i - 1] = j;
        proj->tuple_schema_.add(child_field.type(), child_field.table_name(), child_field.field_name());
        break;
      }
    }
    if (proj->fields_map[attr_num - i - 1] == -1) {
      LOG_ERROR("Failed to find field %s.%s in child node", attr.relation_name, attr.attribute_name);
      return nullptr;
    }
  }

  return proj;
}

ProjectionNode::~ProjectionNode() {}
const TupleSchema &ProjectionNode::schema() { return tuple_schema_; }

RC ProjectionNode::execute(TupleSet &tuple_set) {
  tuple_set.clear();
  tuple_set.set_schema(tuple_schema_);

  TupleSet buf;
  RC rc = child->execute(buf);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  for (auto &v : buf.tuples()) {
    Tuple tuple;
    for (auto &i : fields_map) {
      tuple.add(v.values()[i]);
    }
    tuple_set.add(std::move(tuple));
  }

  return RC::SUCCESS;
}

FilterNode::FilterNode(std::unique_ptr<ExecutionNode> child, std::vector<std::unique_ptr<TupleFilter>> &&filters) : child(std::move(child)), filters(std::move(filters)) {
  tuple_schema_.append(this->child->schema());
}
FilterNode::~FilterNode() { }
const TupleSchema &FilterNode::schema() { return tuple_schema_; };
RC FilterNode::execute(TupleSet &tuple_set) {
  tuple_set.clear();
  tuple_set.set_schema(tuple_schema_);

  TupleSet buf;
  RC rc = child->execute(buf);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  for (auto &tuple : buf.tuples()) {
    bool add = true;
    for (auto &filter : filters) {
      if (!filter->filter(tuple)) {
        add = false;
        break;
      }
    }

    if (add) {
      Tuple t(tuple);
      tuple_set.add(std::move(t));
    }
  }

  return RC::SUCCESS;
}

std::unique_ptr<FilterNode> FilterNode::create(std::unique_ptr<ExecutionNode> child, std::vector<Condition *> &conditions) {
  std::vector<std::unique_ptr<TupleFilter>> filters;
  for (auto &cond : conditions) {
    auto filter = DefaultTupleFilter::create(child->schema(), *cond);
    if (filter == nullptr) {
      return nullptr;
    }

    filters.push_back(std::move(filter));
  }
  
  return std::unique_ptr<FilterNode>(new FilterNode(std::move(child), std::move(filters)));
}