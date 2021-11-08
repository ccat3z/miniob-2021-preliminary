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
#include "storage/common/table_meta.h"
#include <algorithm>

ProjectionNode::ProjectionNode(std::unique_ptr<ExecutionNode> child,
                               std::vector<const TableMeta *> tables,
                               SelectExpr *attrs, int attr_num)
    : child(std::move(child)), tables(std::move(tables)) {
  this->fields_map.reserve(attr_num);

  for (int i = attr_num - 1; i >= 0; i--) {
    auto &expr = attrs[i];

    if (expr.attribute != nullptr) {
      add_field(expr.attribute);
    } else if (expr.agg != nullptr) {
      RelAttr attr;
      attr.relation_name = strdup("");
      attr.attribute_name = expr.agg->name;
      add_field(&attr);
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

  // Any relations
  if (strcmp(attr.relation_name, "*") == 0) {
    RelAttr new_attr;
    new_attr.attribute_name = attr.attribute_name;
    for (auto table : tables) {
      new_attr.relation_name = strdup(table->name());
      add_field(&new_attr);
      free(new_attr.relation_name);
    }
    return;
  }

  // Any attributes
  if (strcmp(attr.attribute_name, "*") == 0) {
    // Find table
    auto table = std::find_if(
        tables.begin(), tables.end(), [&](const TableMeta *&table) {
          return strcmp(table->name(), attr.relation_name) == 0;
        });
    if (table == tables.end()) {
      std::stringstream ss;
      ss << "No such table " << attr.relation_name << " in child";
      throw std::invalid_argument(ss.str());
    }

    // Iter attributes in table
    RelAttr new_attr;
    new_attr.relation_name = attr.relation_name;
    for (int i = (*table)->sys_field_num(); i < (*table)->field_num(); i++) {
      new_attr.attribute_name = strdup((*table)->field(i)->name());
      add_field(&new_attr);
      free(new_attr.attribute_name);
    }
    return;
  }

  bool matched = false;
  for (size_t j = 0; j < child_fields.size(); j++) {
    auto &child_field = child_fields[j];
    if ((strcmp(attr.attribute_name, child_field.field_name()) == 0) &&
        (strcmp(attr.relation_name, child_field.table_name()) == 0)) {
      matched = true;
      this->fields_map.push_back(j);
      this->tuple_schema_.add(child_field.type(), child_field.table_name(),
                              child_field.field_name());
      break;
    }
  }
  if (!matched) {
    std::stringstream ss;
    ss << "Failed to find field " << attr.relation_name << '.'
       << attr.attribute_name;
    throw std::invalid_argument(ss.str());
  }
}