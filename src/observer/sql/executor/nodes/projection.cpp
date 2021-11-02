// Copyright (c) 2021 Lingfeng Zhang(fzhang.chn@foxmail.com). All rights
// reserved. miniob is licensed under Mulan PSL v2. You can use this software
// according to the terms and conditions of the Mulan PSL v2. You may obtain a
// copy of Mulan PSL v2 at:
//          http://license.coscl.org.cn/MulanPSL2
// THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY
// KIND, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE. See the
// Mulan PSL v2 for more details.

#include "projection.h"
#include "common/log/log.h"
#include "filter.h"

ProjectionNode::ProjectionNode(std::unique_ptr<ExecutionNode> child,
                               SelectExpr *attrs, int attr_num)
    : child(std::move(child)) {
  this->fields_map.reserve(attr_num);

  auto &child_fields = this->child->schema().fields();

  for (int i = 0; i < attr_num; i++) {
    auto &expr = attrs[i];

    if (expr.attribute != nullptr) {
      add_field(expr.attribute);
    }
  }
}

ProjectionNode::~ProjectionNode() {}
const TupleSchema &ProjectionNode::schema() { return tuple_schema_; }

std::unique_ptr<ExecutionNode>
ProjectionNode::push_down_predicate(std::list<Condition *> &predicate) {
  auto new_child = child->push_down_predicate(predicate);
  if (new_child != nullptr) {
    child = std::move(new_child);
  }
  if (predicate.size() == 0) {
    child = std::make_unique<FilterNode>(std::move(child), predicate.begin(),
                                         predicate.end());
  }
  return nullptr;
}

RC ProjectionNode::next(Tuple &tuple) {
  Tuple child_tuple;
  RC rc = child->next(child_tuple);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  tuple.clear();
  for (auto &i : fields_map) {
    tuple.add(child_tuple.values()[i]);
  }

  return RC::SUCCESS;
}
void ProjectionNode::reset() { child->reset(); }

void ProjectionNode::add_field(const RelAttr *attr_p) {
  auto &child_fields = this->child->schema().fields();
  auto &attr = *attr_p;

  bool any_table = strcmp(attr.relation_name, "*") == 0;
  bool any_attr = strcmp(attr.attribute_name, "*") == 0;
  bool matched = false;
  for (size_t j = 0; j < child_fields.size(); j++) {
    auto &child_field = child_fields[j];
    if ((any_attr ||
         strcmp(attr.attribute_name, child_field.field_name()) == 0) &&
        (any_table ||
         strcmp(attr.relation_name, child_field.table_name()) == 0)) {
      matched = true;
      this->fields_map.push_back(j);
      this->tuple_schema_.add(child_field.type(), child_field.table_name(),
                              child_field.field_name());

      if (!(any_table || any_attr)) {
        break;
      }
    }
  }
  if (!matched) {
    std::stringstream ss;
    ss << "Failed to find field " << attr.relation_name << '.'
       << attr.attribute_name;
    throw std::invalid_argument(ss.str());
  }
}